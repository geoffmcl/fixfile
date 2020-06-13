// FixFMap.c

#include "FixF32.h"

//#define  g_flProtext             PAGE_READONLY
//#define  g_ReqAccess             FILE_MAP_READ

//typedef  struct   tagMPDFIL {
//   LPTSTR   pName;   // name of file
//   MYHAND   hFile;   // file handle
//   MYHAND   hMap;    // map handle
//   ULARGE_INTEGER sliSize;   // size of file data
//   LPTSTR   pData;   // pointer to file data
//}MPDFIL, * PMPDFIL;
// typedef MPDFIL *  PMF;


VOID  DeleteFileMap( PMPDFIL pmf )
{
   if(pmf)
   {
      if( VFH( pmf->hMap  ) )
         CloseHandle( pmf->hMap );

      if( VFH( pmf->hFile ) )
         CloseHandle( pmf->hFile );

      MFREE(pmf);
   }
}

PMPDFIL  GetFileMap( LPTSTR lpf )
{
   PMPDFIL  pmf = 0;
   MYHAND   h   = 0;
   MYHAND   hm  = 0;
   LPTSTR   lpb = 0;
   if(lpf && *lpf)
   {
      h = CreateFile( lpf, // file name
         GENERIC_READ,  // access mode
         FILE_SHARE_READ,  // share mode
         0, // SD
         OPEN_EXISTING, // how to create
         FILE_ATTRIBUTE_NORMAL,  // file attributes
         0 ); // handle to template file
   }
   if( VFH(h) )
   {
      hm = CreateFileMapping( h,	// handle to file to map 
			NULL,	// optional security attributes
			g_flProtext,	// protection for mapping object
			0,				// high-order 32 bits of object size
			0,				// low-order 32 bits of object size
			NULL );		// name of file-mapping object
      if( VFH(hm) )
      {
         lpb = MapViewOfFile( hm,	// file-mapping object to map into address space
					g_ReqAccess,	// access mode
					0,		// high-order 32 bits of file offset
					0,		// low-order 32 bits of file offset
					0 );	// number of bytes to map
         if(lpb)
         {
            // success
            //UnmapViewOfFile( lpb );      // address
            //CloseHandle(hm);
            //CloseHandle(h);
            pmf = (PMPDFIL)MALLOC( sizeof(MPDFIL) );
            if(pmf)
            {
               pmf->pName  = lpf;   // pointer to NAME of file
               pmf->hFile  = h;     // read handle
               pmf->hMap   = hm;    // map handle
               // return BUFFER pointer
               pmf->pData  = lpb;
               // and file SIZE
               pmf->sliSize.LowPart =
                  GetFileSize( h, &pmf->sliSize.HighPart );
            }
            else
            {
               chkme( "C:ERROR: Mem FAILED!!!!"MEOR );
            }
         }
         else
         {
            // FAILED TO MAP VIEW (READING ONLY)!!!
            gdwError = GetLastError();
            //CloseHandle(hm);
            //CloseHandle(h);
         }
      }
      else
      {
         // FAILED TO MAP FILE FOR READING!!!
         gdwError = GetLastError();
         //CloseHandle(h);
      }
   }

   if( !pmf )
   {
      if(VFH(hm))
         CloseHandle(hm);

      if(VFH(h))
         CloseHandle(h);
   }

   return pmf;
}




// eof - FixFMap.c
