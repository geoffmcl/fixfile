// FixFMap.h
#ifndef  _FixFMap_h_
#define  _FixFMap_h_

#define  g_flProtext             PAGE_READONLY
#define  g_ReqAccess             FILE_MAP_READ

// #pragma message( "Defining PMPDFIL ..." )
// file mapping structure
typedef  struct   tagMPDFIL {
   LPTSTR   pName;   // name of file
   HANDLE   hFile;   // file handle
   HANDLE   hMap;    // map handle
   ULARGE_INTEGER sliSize;   // size of file data
   LPTSTR   pData;   // pointer to file data
}MPDFIL, * PMPDFIL;

// functions to get and delete the map
extern   VOID  DeleteFileMap( PMPDFIL pmf );
extern   PMPDFIL  GetFileMap( LPTSTR lpf );

#endif   // #ifndef  _FixFMap_h_
// eof - FixFMap.h
