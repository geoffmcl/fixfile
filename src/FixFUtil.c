
// ==================================================================
// Module:  FixFUtil.c
// Created: 24/02/01
// Function List:
// 
// ==================================================================
#include	"FixF32.h"
//#include  "FixFUtil.h"

#define  USESYSTIME     // switch to using sys time conversion

// forward reference
LPTSTR   FileTime2AscStg( FILETIME * pft );
void  DO(PTSTR a); // do output

/////////////////////////////////////////////////////////////////////////
// Diagnostic output
static   TCHAR    s_szSprtfBuf[1024];
TCHAR    g_szDiagDef[] = "TEMPFIXF.TXT";  // only ONE
TCHAR    g_szNextFile[264];
MYHAND   g_hDiagFile = 0;

VOID  MCDECL chkme( LPTSTR lpf, ... )
{
#ifndef  NDEBUG
   static TCHAR s_chkme[1024];
   LPTSTR   lpd = &s_chkme[0];
   va_list arglist;
   va_start(arglist, lpf);
   vsprintf( lpd, lpf, arglist );
   va_end(arglist);
   sprtf(lpd);
//#else    // NDEBUG
#ifdef _WIN32
    //UNREFERENCED_PARAMETER(lpf);
#endif
#endif   // NDEBUG n/y
   if(( lpf[0] == 'C' ) &&
      ( lpf[1] == ':' ) &&
      ( lpf[2] == 'E' ) )
   {
      Pgm_Exit( (INT)-1 );
   }
}

LPTSTR   GetNxtBuf( VOID )
{
   // (MXLINEB * MXLINES)
   INT   i = giLnBuf;      //     W.ws_iLnBuf
   i++;
   if( i >= MXLINES )
      i = 0;
   giLnBuf = i;
   return( &gszLnBuf[ (MXLINEB2 * i) ] );    // W.ws_szLnBuf
}

LPTSTR   GetExBrace( LPTSTR lps )
{
   LPTSTR lpb = GetNxtBuf();
   DWORD    dwl;

   strcpy(lpb,lps);
   dwl = strlen(lpb);
   if( dwl > 2 )
   {
      if(( *lpb == '['           ) &&  // First and last
         ( lpb[ dwl - 1 ] == ']' ) )
      {
         // ok, it will be uncased
         strcpy(lpb, &lps[1]);
         lpb[ dwl - 2 ] = 0;  // kill the tail brace
      }
   }
   return lpb;
}

LPTSTR  chkstrncpy( LPTSTR pd, LPTSTR ps, DWORD dws )
{
   //return( strncpy(pd,ps,dws) );
   DWORD dwi;
   for( dwi = 0; dwi < dws; dwi++ )
      pd[dwi] = ps[dwi];
   pd[dwi] = 0;
   return ps;
}

#define  MXIFDEF     256

DWORD    BufHasTailChar( LPTSTR lpb, DWORD dwMax, INT chr )
{
   DWORD dwi;
   for( dwi = 0; dwi < dwMax; dwi++ )
   {
      if( lpb[dwi] == chr )
      {
         dwi++;
         return dwi;
      }
   }
   return 0;
}

LPTSTR   GetExBraceEx( LPTSTR lps, DWORD dwMax )
{
//   DWORD    dwl = strlen(lps);
   DWORD    dws;
   if( dwMax > MXIFDEF )
      dws = (MXIFDEF - 1);
   else
      dws = dwMax;

   if(( dws > 2     ) &&
      ( *lps == '[' ) )
   {
      DWORD dwi;
      for( dwi = 1; dwi < dws; dwi++ )
      {
         if( lps[dwi] == ']' )
            break;
      }
      if(( dwi > 1 ) && ( dwi < dws ) )
      {
         // ok, it will be uncased
         LPTSTR lpb = GetNxtBuf();
         dwi--;
         //chkstrncpy(lpb, &lps[1], dwi);
         strncpy(lpb, &lps[1], dwi);
         lpb[dwi] = 0;  // kill the tail brace
         return lpb;
      }
   }

   return NULL;
}


VOID  SetNextDiag( VOID )
{
   // Used to use the CURRENT WORK DIRECTORY, rather than where the EXE is ...
   // but later changed to where the EXE is - less mess around the disk
   LPTSTR   lpf = &g_szNextFile[0];
   *lpf = 0;
   // gpArgv = argv;
#ifdef USE_COMP_FIO
   // maybe could use gpArgv
#else 
#ifdef _WIN32
   GetModulePath( lpf );
#endif
#endif
   strcat( lpf, g_szDiagDef );
}

VOID  DiagOpen( VOID )
{
//   ReadINI();
   SetNextDiag();
   // Create file each time
#ifdef USE_COMP_FIO
   g_hDiagFile = fopen(g_szNextFile, "w");
#else
   g_hDiagFile = CreateFile( g_szNextFile, // file name
      (GENERIC_READ | GENERIC_WRITE),  // access mode
      0,  // share mode
      0, // SD
      CREATE_ALWAYS, // how to create
      FILE_ATTRIBUTE_NORMAL,  // file attributes
      0 ); // handle to template file
#endif
}

VOID  DiagClose( VOID )
{
//   WriteINI();
    if (VFH(g_hDiagFile)) {
#ifdef USE_COMP_FIO
        fclose(g_hDiagFile);
#else
        CloseHandle(g_hDiagFile);
#endif
    }
    g_hDiagFile = 0;

}

VOID  DiagString( LPTSTR lps )
{
   DWORD    dwi = strlen(lps);
   DWORD    dww = 0;
   if( ( dwi              ) &&
       ( VFH(g_hDiagFile) ) )
   {
#ifdef USE_COMP_FIO
       dww = fwrite(lps, 1, dwi, g_hDiagFile);
       if (dww != dwi) {
           DiagClose();
       }
#else
       if( !( ( WriteFile( g_hDiagFile, lps, dwi, &dww, NULL ) ) &&
             ( dwi == dww                                   ) ) )
      {
         DiagClose();
         g_hDiagFile = INVALID_HANDLE_VALUE;
      }
#endif
   }
}


INT  MCDECL sprtf( LPTSTR lpf, ... )
{
   LPTSTR   lpd = &s_szSprtfBuf[0];
   INT      iRet;
   va_list arglist;
   va_start(arglist, lpf);
   // wvsprintf( lpd, lpf, arglist );   // WOW: This incarnation does NOT appe
   // to support things like %I64u!!!. So let's use the "earlier" version.
   // This appears the same with sprintf() and wsprintf()!!!
   iRet = vsprintf( lpd, lpf, arglist );
   va_end(arglist);
   DO(lpd);
   return iRet;
}



// **********************************************************
// Added 1998 May 17
// =================
/* =====================================================================
 *
 * GetFPath( lpFullPath, lpDrive, lpPath, lpFile, lpExtent )
 * alias splitfile splitpath, splitname
 * Purpose: Split the supplied FULL PATH into four components if
 * the buffers are supplied. Those components NOT required may be
 * NULL. This is a FAR PTR implementation of _splitpath in STDLIB.H.
 * The supplied buffers must be at least equal to the manifest contants
 * of _MAX_DRIVE, _MAX_DIR, _MAX_FNAME, _MAX_EXT in stdlib.h ...
 * The Drive will include the ':'; The Directory will include both the
 * leading and trailing '\' or '/'; The file name will be exactly that;
 * the Extent will include the '.' character; Such that a recombination
 * by say lstrcat will put it all back together ...
 * Any component requested that does not exist in the FULL PATH will 
 * contain a nul string.
 *
 * ===================================================================== */
void	GMGetFPath( LPSTR lpc, LPSTR lpd, LPSTR lpp, LPSTR lpf, LPSTR lpe )
{
	int	i, j, k;
	LPSTR	lps;
	char	c;

	lps = lpc;
	if( lpd )
		lpd[0] = 0;
	if( lpp )
		lpp[0] = 0;
	if( lpf )
		lpf[0] = 0;
	if( lpe )
		lpe[0] = 0;

	if( lps && *lps )
   {
      i = strlen( lps );
		if( lps[1] == ':' )	/* If a DRIVE ... */
		{	
			k = 2;
			if( lpd )	/* If a DRIVE requested ... */
			{
				lpd[0] = lps[0];
				lpd[1] = lps[1];
				lpd[2] = 0;
			}
			lps += 2;
			if( k <= i )
				i = i - k;
			else
				i = 0;
		}
		c = lps[0];
		if( i )
		{
			k = 0;
			if( (c == '\\') || (c == '/') )
			{
				for( j = 0; j < i; j++ )
				{
					c = lps[j];
					if( (c == '\\') || (c == '/') )
						k = j + 1;	/* include this char in move ... */
					if( lpp )
					{
						if( j < _MAX_DIR )
							lpp[j] = c;
					}
				}
				if( lpp )
				{
					if( k < _MAX_DIR )
						lpp[k] = 0;	/* Put the NUL at the correct place ... */
					else
						lpp[_MAX_DIR - 1] = 0;
				}
			}
			lps += k;
			if( k <= i )
				i = i - k;
			else
				i = 0;
		}
		c = lps[0];
		if( i )
		{
			k = 0;
			for( j = 0; j < i; j++ )
			{
				c = lps[j];
				if( c == '.' )
				{
					k = j;
					break;
				}
				if( lpf )
				{
					if( j < _MAX_FNAME )
						lpf[j] = c;
				}
			}
			lps += k;
			if( k <= i )
				i = i - k;
			else
				i = 0;
			if( lpf )
			{
				if( j < _MAX_FNAME )
					lpf[j] = 0;
				else
					lpf[_MAX_FNAME - 1] = 0;
			}
		}
		if( i && (c == '.') )
		{
			for( j = 0; j < i; j++ )
			{
				c = lps[j];
				if( lpe )
				{	
					if( j < _MAX_EXT )
						lpe[j] = c;
				}
			}
			if( lpe )
			{
				if( j < _MAX_EXT )
					lpe[j] = 0;
				else
					lpe[_MAX_EXT - 1] = 0;
			}
		}
	}
}

// set some global item for this file
// siwes from <stdlib.h>
// _makepath() and _splitpath()
TCHAR g_szActFullPath[_MAX_FNAME];
TCHAR g_szActDrive[_MAX_DRIVE];
TCHAR g_szActFolder[_MAX_DIR];
TCHAR g_szActTitle[_MAX_FNAME];
TCHAR g_szActExt[_MAX_EXT]; // in stdlib.h ...

void SetGlobalFileNames( PTSTR pfile )
{
   _fullpath(g_szActFullPath,pfile,_MAX_FNAME);
   if( !gmGetFullPath( g_szActFullPath, pfile, _MAX_FNAME ) )
      strcpy(g_szActFullPath,pfile);

   GMGetFPath( g_szActFullPath, g_szActDrive, g_szActFolder,
      g_szActTitle, g_szActExt );
}

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif // #ifndef INVALID_SET_FILE_POINTER

/*
   1 SEEK_SET Beginning of file
   2 SEEK_CUR Current position of the file pointer
   3 SEEK_END End of file
 */

__int64 MyFileSeek (MYHAND hf, __int64 distance, DWORD MoveMethod)
{
#ifdef USE_COMP_FIO
    int res = fseek(hf, distance, MoveMethod);
    if (res)
        return (__int64)-1;
    return distance;
#else // !USE_COMP_FIO
   LARGE_INTEGER li;

   li.QuadPart = distance;

   li.LowPart = SetFilePointer (hf, li.LowPart, &li.HighPart, MoveMethod);

   if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
   {
      li.QuadPart = -1;
   }

   return li.QuadPart;
#endif // USE_COMP_FIO
}


PWL   Add2WLList( LPTSTR lpf )
{
   PWL   pwl = 0;
   DWORD dwl = 0;
   if( lpf )
      dwl = strlen(lpf);
   if(dwl)
   {
      pwl = (PWL)MALLOC( sizeof(WL) );
      if( pwl )
      {
         pwl->wl_dwLen = dwl;
         strcpy( &pwl->wl_szFile[0], lpf );
         InsertTailList( &gsFileList, (PLE)pwl );

      }
   }
   return pwl;
}

PXL   Add2XLList( LPTSTR lpf )
{
   PXL   pxl = 0;
   DWORD dwl = 0;
   if( lpf )
      dwl = strlen(lpf);
   if(dwl)
   {
      pxl = (PXL)MALLOC( sizeof(XL) );
      if( pxl )
      {
         pxl->xl_dwLen = dwl;
         strcpy( &pxl->xl_szFile[0], lpf );
         InsertTailList( &gsExclList, (PLE)pxl );
      }
   }
   return pxl;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Add2RLList
// Return type: PRL 
// Argument   : LPTSTR lpf
// Description: Add this string to the -R (REMOVE)
//              list of strings.
// Linked list is gsRemList
// NOTE: Size of structure RL is fixed to same using many different memory
// allocation sizes, but perhaps this is NOT really needed!
///////////////////////////////////////////////////////////////////////////////
PRL   Add2RLList( LPTSTR lpf )
{
   PRL   prl = 0;
   DWORD dwl = 0;
   if( lpf )
      dwl = strlen(lpf);
   if( (dwl) &&
       (dwl < (MXRLINE - 1) ) )
   {
      prl = (PRL)MALLOC( sizeof(RL) );
      if( prl )
      {
         prl->rl_dwLen = dwl;
         strcpy( &prl->rl_szFile[0], lpf );
         InsertTailList( &gsRemList, (PLE)prl );
      }
   }
   return prl;
}
PRL   Add2rLList( LPTSTR lpf )
{
   PRL   prl = 0;
   DWORD dwl = 0;
   if( lpf )
      dwl = strlen(lpf);
   if( (dwl) &&
       (dwl < (MXRLINE - 1) ) )
   {
      prl = (PRL)MALLOC( sizeof(RL) );
      if( prl )
      {
         prl->rl_dwLen = dwl;
         strcpy( &prl->rl_szFile[0], lpf );
         InsertTailList( &gsRemText, (PLE)prl );
      }
   }
   return prl;
}

// gsReport - diagnostic report list
PDL   Add2DLList( LPTSTR lps )
{
   PDL   pdl = 0;
   DWORD dwl = 0;
   if( lps )
      dwl = strlen(lps);
   if(dwl)
   {
      pdl = (PDL)MALLOC( (sizeof(DL) + dwl) );
      if( pdl )
      {
         pdl->dl_dwLen = dwl;
         strcpy( &pdl->dl_szFile[0], lps );
         InsertTailList( &gsReport, (PLE)pdl );
      }
      if( VERB9 ) {
         sprtf( "Report: %s", lps );
      }
   }
   return pdl;
}

VOID  MCDECL AddReport( LPTSTR lpf, ... ) // like "Added CR to line %d (%d)."MEOR, dwl, dwl2 );
{
   static TCHAR _s_addrepbuf[1024];
   LPTSTR   lpd = _s_addrepbuf;
   va_list arglist;
   va_start(arglist, lpf);
   vsprintf( lpd, lpf, arglist );
   va_end(arglist);
   Add2DLList(lpd);
}

BOOL	GotWild( LPTSTR lpf )
{
	BOOL	   flg = FALSE;
	int		i,j;
	TCHAR	   tc;
   j = 0;
   if( lpf )
      j = lstrlen(lpf);
   if(j)
	{
		// we appear to have a C string
		for( i = 0; i < j; i++ )
		{
			tc = lpf[i];
			if( ( tc == '?' ) ||
				( tc == '*' ) )
			{
				flg = TRUE;
				break;
			}
		}
	}
	return flg;
}

INT SplitFileName( PTSTR pDest, PTSTR pPath )
{
   INT iRet = 0;
   INT ilen = strlen(pPath);
   INT i, ibgn;
   INT c;

   ibgn = 0;
   for( i = 0; i < ilen; i++ ) {
      c = pPath[i];
      if( ( c == ':' ) || ( c == '\\' ) || ( c == '/' ) ) {
         ibgn = i + 1;
      }

   }
   strcpy(pDest, pPath);
   if( ibgn ) {

      strcpy( pDest, &pPath[ibgn] );
      iRet = ilen - ibgn;

   }
   return iRet;
}

BOOL	SplitExt( LPTSTR lpb, LPTSTR lpe, LPTSTR lpf )
{
	BOOL	flg = FALSE;
	int		i, j, k, l;
	TCHAR 	c;
   LPTSTR   lpd;
	if( lpf && ((i = lstrlen( lpf )) != 0) )
	{
		//if( j = HasDot( lpf ) )
      j = 0;
      lpd = strrchr( lpf, '.' );
      if( lpd )
      {
         //lpd++;
         j = (lpd - lpf);
         if( j )
            j++;
      }
      else
      {
         lpd = lpf;
         j = 0;
      }
		if( j )
		{
			if( ( j < i ) &&
				( (i - j) <= 3 ) )
			{
				k = 0;
				for( l = 0; l < j; l++ )
				{
					c = lpf[l];
					if( ( c == '.' ) &&
                  ( &lpf[l] == lpd ) )
					{
						l++;
						break;
					}
					lpb[k++] = c;
				}
				lpb[k] = 0;    // terminate the BODY part (sans the dot)
				k = 0;   // start agin if there is an extent
				for( ; l < i; l++ )
				{
					c = lpf[l];
					if( c != '.' )
						lpe[k++] = c;
				}
				lpe[k] = 0; // terminate the extent part (if any)
			}
			else
			{
				if( lpb )
					lstrcpy( lpb, lpf );
				if( lpe )
					*lpe = 0;
			}
		}
		else
		{
			if( lpb )
				lstrcpy( lpb, lpf );
			if( lpe )
				*lpe = 0;
		}
	}
	return	flg;
}

BOOL	WildComp2( LPTSTR lp1, LPTSTR lp2 )
{
	BOOL	flg;
	int		i1, i2, j1, j2;
	TCHAR    c, d;

	flg = FALSE;
   i1 = i2 = 0;
	if( lp1 && lp2 )
   {
		i1 = lstrlen( lp1 );
      i2 = lstrlen( lp2 );
   }
   if( i1 && i2 )
	{
		j2 = 0;
		for( j1 = 0; j1 < i1; j1++ )
		{
			c = lp1[j1];
			d = lp2[j2];
			if( c != d )
			{
            // this is NOT strictly correct in that we could have been given
            // something this *list, so an asterick only matches everything
            // if it is the only character.
            // TO BE FIXED SOME DAY
				if( c == '*' )
				{
					flg = TRUE;
					break;
				}
				else if( d == '*' )
				{
					flg = TRUE;
					break;
				}
				else if( ( c == '?' ) || ( d == '?' ) )
				{
					// One char ok.
				}
				else
				{
					if( toupper( c ) != toupper( d ) )
						break;
				}
			}
			j2++;
		}
		if( !flg && ( j1 == i1 ) )
			flg = TRUE;
	}
	return flg;
}



///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : MatchFiles
// Return type: BOOL 
// Arguments  : LPTSTR lp1
//            : LPTSTR lp2 == from grmlib.c - feb 2001
// Description: 
//              
///////////////////////////////////////////////////////////////////////////////
BOOL	MatchFiles( LPTSTR lp1, LPTSTR lp2 )   // from grmlib.c - feb 2001
{
	BOOL	flg = FALSE;   // assume they DO NOT match
	TCHAR	body1[MAX_PATH];
	TCHAR	ext1[4];
	TCHAR body2[MAX_PATH];
	TCHAR	ext2[4];

	if( lp1 && lp2 &&
		*lp1 && *lp2 )
	{
		if( !GotWild( lp1 ) &&
			!GotWild( lp2 ) )
		{
			if( lstrcmpi( lp1, lp2 ) == 0 )
				flg = TRUE;
		}
		else
		{
			// One of the other HAS WILD CHAR(S)
			SplitExt( &body1[0], &ext1[0], lp1 );
			SplitExt( &body2[0], &ext2[0], lp2 );
			if( ( WildComp2( &body1[0], &body2[0] ) ) &&
				( WildComp2( &ext1[0], &ext2[0] ) ) )
			{
				flg = TRUE;
			}
		}
	}
	return flg;
}


BOOL  InXList( LPTSTR lpf )
{
   BOOL     bRet = FALSE;  // assume NOT in list
   PLE      pHead = &gsExclList;
   PLE      pNext;
   PXL      pxl;
   LPTSTR   lpx;

   Traverse_List( pHead, pNext )
   {
      pxl = (PXL)pNext;
      lpx = &pxl->xl_szFile[0];
      if( MatchFiles( lpx, lpf ) )
      {
         bRet = TRUE;
         break;
      }
   }
   return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : InRList
// Return type: BOOL 
// Arguments  : LPTSTR lpl
//            : DWORD dwLen
// Description: Check if this line contains the REMOVE LINE string,
//              returning TRUE if it matches one of the list items.
///////////////////////////////////////////////////////////////////////////////
BOOL  InRList( LPTSTR lpl, DWORD dwLen )
{
   BOOL     bRet = FALSE;  // assume NOT in list
   PLE      pHead = &gsRemList;
   PLE      pNext;
   PRL      prl;
   LPTSTR   lpx, lpf;
   DWORD    dwl;
   BOOL     bfst;

   Traverse_List( pHead, pNext )
   {
      prl = (PRL)pNext;
      lpx = &prl->rl_szFile[0];
      dwl = prl->rl_dwLen;
      bfst = FALSE;
      if( ( dwl > 2 ) &&
         ( lpx[0] == '^' ) &&
         ( lpx[1] == 'J' ) )
      {
         lpx += 2;
         dwl -= 2;
         bfst = TRUE;
      }
      if( dwLen >= dwl )
      {
         lpf = strstr( lpl, lpx );  // get instance of remove line in file line
         if( lpf )   // if we found an instance
         {
            if( bfst )
            {
               // only acceptable as first position
               if( lpf == lpl )
               {
                  bRet = TRUE;
                  break;
               }
            }
            else
            {
               // acceptable anywhere in this line
                  bRet = TRUE;
                  break;
            }
         }
      }
   }
   return bRet;
}

PRL  InrList( LPTSTR lpl, DWORD dwLen )
{
   PLE      pHead = &gsRemText;
   PLE      pNext;
   PRL      prl;
   LPTSTR   lpx, lpf;
   DWORD    dwl = dwLen;
   Traverse_List( pHead, pNext )
   {
      prl = (PRL)pNext;
      lpx = &prl->rl_szFile[0];
      dwl = prl->rl_dwLen;
      //if( dwLen >= dwl )
      //{
         lpf = strstr( lpl, lpx );  // get instance of remove line in file line
         if( lpf )   // if we found an instance
         {
            return prl;
         }
      //}
   }
   return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsValidFile4
// Return type: DWORD 
// Arguments  : LPTSTR lpf
//            : PWIN32_FIND_DATA pfd
// Description: Check if this is a FILE, FOLDER (directory) or WILD CARD
//              file name, and return the appropriate value.
// RETURN values
//#define  IS_FILE     1
//#define  IS_FOLDER   2
//#define  IS_WILD     4
///////////////////////////////////////////////////////////////////////////////
#ifdef USE_COMP_FIO

DWORD  IsValidFile4(LPTSTR lpf, PWIN32_FIND_DATA pfd)
{
    DWORD     flg = 0;
    struct stat buf;
    if (stat(lpf, &buf) == 0) {
        if (buf.st_mode & M_IS_DIR)
            flg = IS_FOLDER;
        else
            flg = IS_FILE;
        strcpy(pfd->cFileName, lpf);
//#ifdef _WIN64
//        pfd->nFileSizeHigh = buf.st_size >> 32;
//#else
        pfd->nFileSizeHigh = 0;
//#endif
        pfd->nFileSizeLow = buf.st_size & 0xFFFFFFFF;
        pfd->ftLastWriteTime.dwHighDateTime = buf.st_mtime >> 32;
        pfd->ftLastWriteTime.dwLowDateTime = buf.st_mtime & 0xFFFFFFFF;
        //pfd->ftCreationTime = pfd->ftLastWriteTime;
        //pfd->ftLastAccessTime = pfd->ftLastWriteTime;
    }
    return flg;

}
#else // !USE_COMP_FIO
DWORD  IsValidFile4( LPTSTR lpf, PWIN32_FIND_DATA pfd )
{
   DWORD     flg = 0;
   MYHAND   hFind;
   WIN32_FIND_DATA   fd;
   hFind = FindFirstFile( lpf, pfd );
   if( VFH(hFind) )
   {
      if( pfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
         flg = IS_FOLDER;
      else
      {
         if( FindNextFile( hFind, &fd ) )  // search handle and data buffer
            flg = IS_WILD;
         else
            flg = IS_FILE;
      }
      FindClose(hFind);
   }
   return flg;
}
#endif // USE_COMP_FIO y/n


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : gmGetFullPath
// Return type: BOOL 
// Arguments  : LPTSTR lpd
//            : LPTSTR lpf
//            : DWORD dws
// Description: Convert any RELATIVE path name to the FULL PATH
//              of the file or folder.
///////////////////////////////////////////////////////////////////////////////
BOOL  gmGetFullPath( LPTSTR lpd, LPTSTR lpf, DWORD dws )
{
   BOOL  bRet;
   if( _fullpath( lpd, lpf, dws ) )
      bRet = TRUE;
   else
      bRet = FALSE;
   return bRet;
}

BOOL  gmGetFileTitle( LPTSTR lpt, LPTSTR lpf, DWORD dws )
{
   BOOL  bRet;
   if( GetFileTitle( lpf, lpt, (WORD)dws ) )
      bRet = FALSE;
   else
      bRet = TRUE;
   return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : gmGetFolder
// Return type: VOID 
// Arguments  : LPTSTR lpd
//            : LPTSTR lpf
// Description: Return the DIRECTORY (FOLDER) from the given string lpf,
//              in the given buffer lpd.
///////////////////////////////////////////////////////////////////////////////
VOID  gmGetFolder( LPTSTR lpd, LPTSTR lpf )
{
   LPTSTR   lps;
   TCHAR    buf[264];

   strcpy(buf,lpf);
   lps = strrchr( buf, '\\' );   // get last instance of the "\" character
   if( lps )
      lps[1] = 0;    // if it exists terminate AFTER this character
   else
      lps = buf;     // else there is NO "\" character in the given string.

   strcpy( lpd, buf );
}



///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : gmSet_File_Pos
// Return type: BOOL
// Arguments  : MYHAND hf
//            : LARGE_INTEGER li
//            : DWORD dwFrom
// Description: 
//              
// Value Meaning of dwFrom
// FILE_BEGIN The starting point is zero or the beginning of the file. 
// FILE_CURRENT The starting point is the current value of the file pointer. 
// FILE_END The starting point is the current end-of-file position.
// From MSDN Jan 2001
// Case One: calling the function with lpDistanceToMoveHigh == NULL 
// Try to move hFile's file pointer some distance. 
//dwPtr = SetFilePointer (hFile, lDistance, NULL, FILE_BEGIN) ; 
//if (dwPtr == INVALID_SET_FILE_POINTER) // Test for failure
//{ 
    // Obtain the error code. 
    //dwError = GetLastError() ; 
    // Deal with failure. 
    // . . . 
//} // End of error handler 
// 
// Case Two: calling the function with lpDistanceToMoveHigh != NULL 
// Try to move hFile's file pointer some huge distance. 
//dwPtrLow = SetFilePointer (hFile, lDistLow, & lDistHigh, FILE_BEGIN) ;  
// Test for failure
//if (dwPtrLow == INVALID_SET_FILE_POINTER && (dwError = GetLastError()) != NO_ERROR )
//{ 
    // Deal with failure. 
    // . . . 
//} // End of error handler 
///////////////////////////////////////////////////////////////////////////////
BOOL  gmSet_File_Pos( MYHAND hf, LARGE_INTEGER li, DWORD dwFrom )
{
   BOOL  bRet = FALSE;  // assume FAILED
   if( VFH(hf) )
   {
      DWORD dwLow;
      ULONG dwl = li.LowPart;
      LONG  dwh = li.HighPart;
      dwLow = SetFilePointer( hf,   // file handle, with READ or WRITE access
         dwl,        // low DWORD to move
         &dwh,       // ptr to high DWORD to move
         dwFrom );   // like FILE_BEGIN = move method
      if( ( dwLow == INVALID_SET_FILE_POINTER ) &&
          ( GetLastError() != NO_ERROR ) )
      {
         bRet = FALSE;  // failed
      }
      else
      {
         bRet = TRUE;   // ok
      }
   }
   return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Get_Env_Var
// Return type: BOOL 
// Arguments  : LPTSTR lpd
//            : LPTSTR lpe
// Description: Copy a requested ENVIRONMENT variable, if found to the
//              given buffer. NOT TESTED!
///////////////////////////////////////////////////////////////////////////////
BOOL  Get_Env_Var( LPTSTR lpd, LPTSTR lpe )
{
   BOOL     bRet = FALSE;
   LPTSTR   lpr = getenv( lpe );
   *lpd = 0;
   if( lpr && *lpr )
   {
      strcpy(lpd, lpr);
      if( lpd[ strlen(lpd) - 1 ] != '\\' )
         strcat(lpd, "\\" );
      bRet = TRUE;
   }
   return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : GetShortName
// Return type: LPTSTR 
// Arguments  : LPTSTR lps
//            : DWORD dwmx
// Description: Given a string, return a buffer containing a string (about)
//              the size of the given dwmx, adding ... in the centre.
///////////////////////////////////////////////////////////////////////////////
LPTSTR   GetShortName( LPTSTR lps, DWORD dwmx )
{
   LPTSTR   lpb = GetNxtBuf();
   DWORD    dwi = strlen(lps);
   DWORD    dw2, dwt;
   if( dwi < MXLINEB )
      strcpy( lpb, lps );
   else
   {
      strncpy( lpb, lps, (MXLINEB-1) );
      lpb[ MXLINEB-1 ] = 0;   // ensure teminating NUL char
   }
   if( (dwi + 3) > dwmx )
   {
      if( dwmx < 27 ) {
         // give emphasis to FILE name
         PTSTR pfn = GetNxtBuf();
         dw2 = SplitFileName(pfn, lps);
         if( dw2 ) {
            if( dw2 < dwmx ) {
               dwt = dwmx - dw2;
               if(dwt > 3)
                  dwt -= 3;
               else
                  dwt  = 0;
               lpb[dwt] = 0;
               strcat(lpb,"...");
               strcat(lpb, pfn );
               return lpb;
            }
         }
      }
      dw2 = dwmx / 2;   // get half the maximum
      if( ( dw2 > 2 ) & ( dw2 < (MXLINEB / 2) ) )
      {
         dw2--;
         lpb[dw2] = 0;
         strcat(lpb,"...");
         dwt = dwi - dw2;
         strcat(lpb, &lps[dwt] );
      }
   }
   return lpb;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : TrimIB
// Return type: DWORD 
// Argument   : LPTSTR lps
// Description: Remove spacey stuf from BEGINNING and TAIL
//              NOTE WELL: Buffer is ALTERED, hence "IB"=IN BUFFER
///////////////////////////////////////////////////////////////////////////////
DWORD TrimIB( LPTSTR lps )
{
   DWORD dwk;
   dwk = LTrimIB(lps);
   dwk = RTrimIB(lps);
   return dwk;
}

DWORD LTrimIB( LPTSTR lps )
{
   DWORD    dwr = strlen(lps);
   LPTSTR   p   = lps;
   DWORD    dwk = 0;
   while(dwr)
   {
      if( *p > ' ' )
         break;
      dwk++;   // count chars to die
      p++;     // bump pointer
      dwr--;   // update return length
   }
   if(dwk)  // if chars to die
      strcpy(lps,p);    // copy remainder up to beginning
   return dwr;          // return length or reduced length
}

DWORD RTrimIB( LPTSTR lps )
{
   DWORD    dwr = strlen(lps);
   DWORD    dwk;
   dwk = dwr;        // copy of length
   while(dwk--)      // while not zero - post decrement
   {
      if( lps[dwk] > ' ' ) // is this above spacey
         break;   // yup - out of here
      lps[dwk] = 0;  // else zero it
      dwr--;   // reduce overall length
   }
   return dwr; // return length or reduced length
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : InStr
// Return type: INT 
// Arguments  : LPTSTR lpb
//            : LPTSTR lps
// Description: Return the position of the FIRST instance of the string in lps
//              Emulates the Visual Basic function.
///////////////////////////////////////////////////////////////////////////////
INT   InStr( LPTSTR lpb, LPTSTR lps )
{
   INT   iRet = 0;
   INT   i, j, k, l, m;
   TCHAR    c;

   if(( lpb == 0 ) ||
      ( lps == 0 ) )
      return 0;

   i = strlen(lpb); // source length
   j = strlen(lps); // find string
   if( i && j && ( i >= j ) )
   {
      c = *lps;   // get the first we are looking for
      l = i - ( j - 1 );   // get the maximum length to search
      for( k = 0; k < l; k++ )
      {
         if( lpb[k] == c )
         {
            // found the FIRST char so check until end of compare string
            for( m = 1; m < j; m++ )
            {
               if( lpb[k+m] != lps[m] )   // on first NOT equal
                  break;   // out of here
            }
            if( m == j )   // if we reached the end of the search string
            {
               iRet = k + 1;  // return NUMERIC position (that is LOGICAL + 1)
               break;   // and out of the outer search loop
            }
         }
      }  // for the search length
   }
   return iRet;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : InStrWhole
// Return type: INT 
// Arguments  : LPTSTR lpb
//            : LPTSTR lps
// Description: Specialised InStr() function
//              Not only searches for the lps string, but ensures it is whole!
// That is it is preceeded and followed by non-alpha-numeric characters!!
///////////////////////////////////////////////////////////////////////////////
INT   InStrWhole( LPTSTR lpb, LPTSTR lps )
{
   INT   iRet = 0;
   INT   i, j, k, l, m;
   TCHAR    c, cc, pc;
   i = strlen(lpb);
   j = strlen(lps);
   if( i && j && ( i >= j ) )
   {
      c = *lps;   // get the first we are looking for
      l = i - ( j - 1 );   // get the maximum length to search
      pc = 0;  // previous "ASSUMED" to not be alpha-numeric
      for( k = 0; k < l; k++ )
      {
         cc = lpb[k];
         if( cc == c )
         {
            // found the FIRST char
            if( !IsAlphaNumeric(pc) )
            {
               // and is preceeded by NON-alpha-numeric, so
               // check until end of compare string
               for( m = 1; m < j; m++ )
               {
                  if( lpb[k+m] != lps[m] )   // on first NOT equal
                     break;   // out of here, the inner search loop
               }
               if( m == j )   // if we reached the end of the search string
               {
                  if( !IsAlphaNumeric( lpb[k+m] ) )  // check NEXT character
                  {
                     iRet = k + 1;  // return NUMERIC position (that is LOGICAL + 1)
                     break;   // and out of the outer search loop
                  }
               }
            }
         }
         pc = cc;    // keep previous
      }  // for the search length
   }
   return iRet;
}



BOOL comp_stgn( LPTSTR pRem, LPTSTR p2 )
{
   INT   c1, c2;
   LPTSTR   p1 = LocalAlloc( LPTR, (strlen(pRem)+1) );

   if( !p1 )
   {
      chkme( "C:ERROR: Memory FAILED for %d bytes!"MEOR, (strlen(pRem)+1) );
      return FALSE;
   }

   strcpy( p1, pRem );
   MaxTrim(p1);

   c1 = *p1;
   c2 = *p2;

   while(c1)
   {

      if( c1 != c2 )
         goto Exit_Cmp;

      if(c2)
      {
         p2++;
         c2 = *p2;
         if( !c2 )   // end of string
         {
            c1 = c2; // set GOOD compare exit
            break;   // and out of here
         }
      }

      if(c1)
      {
         p1++;
         c1 = *p1;
         // if c1 == 0 will exit loop and FAIL

      }
   };
   // while( (c1 != 0) && (c2 != 0) );

Exit_Cmp:

   LocalFree(p1);

   if( c1 != c2 )
      return(FALSE);
   else
      return(TRUE);

}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : InStrIS
// Return type: INT 
// Arguments  : LPTSTR lpb
//            : LPTSTR lps
// Description: As above, but IGNORE spacey chars
//              
///////////////////////////////////////////////////////////////////////////////
INT   InStrIS( LPTSTR lpStg, LPTSTR lpCmp )
{
   INT   iRet = 0;
   INT   i, j, k, l;
   TCHAR    c;
   LPTSTR   lpb, lps;
   i = strlen(lpStg);
   j = strlen(lpCmp);
   if( i && j )   // && ( i >= j ) )
   {
      lpb = lpStg;   // must use REAL string to get correct offset
      //lpb = LocalAlloc( LPTR, (i+1) );
      //if(!lpb)
      //{
      //   chkme( "C:ERROR: Memory FAILED (%d bytes)"MEOR, i );
      //   return 0;
      //}
      lps = LocalAlloc( LPTR, (j+1) );
      if(!lps)
      {
         LocalFree(lpb);
         chkme( "C:ERROR: Memory FAILED (%d bytes)"MEOR, i );
         return 0;
      }
      //strcpy(lpb,lpStg);
      //MaxTrim(lpb);
      strcpy(lps,lpCmp);
      MaxTrim(lps);
      //i = strlen(lpb);
      j = strlen(lps);
      if( !i || !j || ( i < j ) )
         goto Clean_Up;

      c = *lps;   // get the first we are looking for
      l = i - ( j - 1 );   // get the maximum length to search
      for( k = 0; k < l; k++ )
      {
         if( lpb[k] == c )
         {
            // found the FIRST char so check until end of compare string
            if( comp_stgn( &lpb[k], lps ) )
            {
               iRet = k + 1;  // return NUMERIC position (that is LOGICAL + 1)
               break;   // and out of the outer search loop
            }
         }
      }  // for the search length

Clean_Up:

      //LocalFree(lpb);
      LocalFree(lps);

   }
   return iRet;
}

LPTSTR   FixNum( LPTSTR lpb )
{
   LPTSTR   lpr = GetNxtBuf();
   LPTSTR   p;
   *lpr = 0;
   while( *lpb && (*lpb <= ' ') )lpb++;
   p = lpr;
   while( *lpb )
   {
      if( ISNUM(*lpb) )
         *p++ = *lpb;
      else if( *lpb != ',' )  // skip the comma
         break;
      lpb++;
   }
   *p = 0;
   return lpr;
}

//  Platform SDK: Windows System Information 
//DosDateTimeToFileTime
//The DosDateTimeToFileTime function converts MS-DOS date and time values to a 
//64-bit file time. 
//BOOL DosDateTimeToFileTime(
//  WORD wFatDate,          // 16-bit MS-DOS date
//  WORD wFatTime,          // 16-bit MS-DOS time
//  LPFILETIME lpFileTime   // 64-bit file time
//);
//Parameters
//wFatDate 
//[in] Specifies the MS-DOS date. The date is a packed 16-bit value with the 
//following format. Bits Contents 
//0-4 Day of the month (1-31) 
//5-8 Month (1 = January, 2 = February, and so on) 
//9-15 Year offset from 1980 (add 1980 to get actual year) 
//wFatTime 
//[in] Specifies the MS-DOS time. The time is a packed 16-bit value with the 
//following format. Bits Contents 
//0-4 Second divided by 2 
//5-10 Minute (0-59) 
//11-15 Hour (0-23 on a 24-hour clock) 
//lpFileTime 
//[out] Pointer to a FILETIME structure to receive the converted 64-bit file time. 
//Return Values
//If the function succeeds, the return value is nonzero.
//If the function fails, the return value is zero. To get extended error 
//information, call GetLastError. 
//Remarks
//MAPI: For more information, see Syntax and Limitations for Win32 Functions 
//Useful in MAPI Development.
//Requirements 
//  Windows NT/2000: Requires Windows NT 3.1 or later.
//  Windows 95/98: Requires Windows 95 or later.
//  Header: Declared in Winbase.h; include Windows.h.
//  Library: Use Kernel32.lib.
//See Also
//Time Overview, Time Functions, FILETIME, FileTimeToDosDateTime, 
//FileTimeToSystemTime, SystemTimeToFileTime 
//Built on Thursday, October 12, 2000Requirements 
//  Windows NT/2000: Requires Windows NT 3.1 or later.
//  Windows 95/98: Requires Windows 95 or later.
//  Header: Declared in Winbase.h; include Windows.h.
//  Library: Use Kernel32.lib.
//See Also
//Time Overview, Time Functions, FILETIME, FileTimeToDosDateTime, 
//FileTimeToSystemTime, SystemTimeToFileTime
// take a string 29/01/01  19:06 and convert to FILETIME
// or the string 11-13-99   4:56p and converti ti to FILE TIME (if -wa = american format)
#define  MXDTSTG     64
static TCHAR   _s_szdt[MXDTSTG+16];
static   SYSTEMTIME  _s_st;
DWORD Asc2FileTime( LPTSTR lpb, FILETIME * pft )
{
   DWORD    dwl = 0;
#ifndef   USESYSTIME
   DWORD    wFatDate, wFatTime;
#endif   // #ifndef   USESYSTIME
   INT      c, iD, iM, iY, iHr, iMin;
   INT      ilen = 0;
   LPTSTR   lpdt = _s_szdt;
   SYSTEMTIME * pt = &_s_st;
   LPTSTR lpft;   // = FileTime2AscStg( pft );

//0-4 Day of the month (1-31) 
//5-8 Month (1 = January, 2 = February, and so on) 
//9-15 Year offset from 1980 (add 1980 to get actual year) 
   while( *lpb && (*lpb <= ' ') )
   {
      lpb++;  // skip any leading spaces
      dwl++;
   }
   if( !ISNUM(*lpb) )
      return 0;

   ilen = 0;
   iD = atoi(lpb);   // get DAY
   while( *lpb && ISNUM(*lpb) )
   {
      if( ilen < MXDTSTG )
         lpdt[ilen++] = *lpb;

      lpb++;  // skip first number
      dwl++;
   }

   if( *lpb == 0 )
      return 0;

   while( *lpb && !( ISNUM(*lpb) | ISALPHA(*lpb) ) )
   {
      lpb++;  // skip item BETWEEN numbers
      dwl++;
   }

      if( ilen < MXDTSTG )
         lpdt[ilen++] = '-';
//   if( *lpb != '/' )
//      return 0;
//   lpb++;
//   dwl++;

   if( !ISNUM(*lpb) )   // if NO second number
      return 0;

   iM = atoi(lpb);
   while( *lpb && ISNUM(*lpb) )
   {
      if( ilen < MXDTSTG )
         lpdt[ilen++] = *lpb;
      lpb++;  // skip second number
      dwl++;
   }

//   if( *lpb != '/' )
//      return 0;
//   lpb++;
//   dwl++;
   while( *lpb && !( ISNUM(*lpb) | ISALPHA(*lpb) ) )
   {
      lpb++;  // skip item(s) BETWEEN numbers
      dwl++;
   }
      if( ilen < MXDTSTG )
         lpdt[ilen++] = '-';

   if( !ISNUM(*lpb) )
      return 0;

   iY = atoi(lpb);
   while( *lpb && ISNUM(*lpb) )
   {
      if( ilen < MXDTSTG )
         lpdt[ilen++] = *lpb;
      lpb++;  // skip second number
      dwl++;
   }

   while( *lpb && (*lpb <= ' ') )
   {
      lpb++;  // skip space to time
      dwl++;
   }

      if( ilen < MXDTSTG )
         lpdt[ilen++] = ' ';

//5-10 Minute (0-59) 
//11-15 Hour (0-23 on a 24-hour clock) 
   if( !ISNUM(*lpb) )
      return 0;
   iHr = atoi(lpb);
   while( *lpb && ISNUM(*lpb) )
   {
      if( ilen < MXDTSTG )
         lpdt[ilen++] = *lpb;
      lpb++;  // skip hour number
      dwl++;
   }
   if( *lpb != ':' )
      return 0;

      if( ilen < MXDTSTG )
         lpdt[ilen++] = *lpb;
   lpb++;
   dwl++;

   if( !ISNUM(*lpb) )
      return 0;
   iMin = atoi(lpb);
   while( *lpb && ISNUM(*lpb) )
   {
      if( ilen < MXDTSTG )
         lpdt[ilen++] = *lpb;
      lpb++;  // skip minute number
      dwl++;
   }

   // FIX20040901 - Check if this is followed by AM or PM
   while( *lpb && ( *lpb == ' ' ) ) {
      lpb++;
      dwl++;
   }
   if( toupper(lpb[0]) == 'A' ) {
       if( toupper(lpb[1]) == 'M' ) {
           lpb += 2;
           dwl += 2;
       }
   } else if( toupper(lpb[0]) == 'P' ) {
       if( toupper(lpb[1]) == 'M' ) {
           lpb += 2;
           if(iHr < 12)
               iHr += 12;
           dwl += 2;
       }
   }
   // FIX20040901 - Check if this is followed by AM or PM

   if( g_dwFullDir & bf_American )
   {
      // two things to change
      // Day with Month, and
      // get the p or a, and fix HOURS
      c = iD;
      iD = iM;
      iM = c;
      c = toupper(*lpb);
      if( c == 'A' )
      {
         // ok, hour is OK, but still
         //lpb++;  // skip 'a' indicator
         //dwl++;
      }
      else if( c == 'P' )
      {
         iHr += 12;
         if(iHr == 24)
            iHr = 0;
         //lpb++;  // skip 'p' indicator
         //dwl++;
      }
      else
         return 0;   // failed on American switch

      while( *lpb && ( ISALPHA(*lpb) ) )
      {
         // skip the AM/PM indicator
         if( ilen < MXDTSTG )
            lpdt[ilen++] = *lpb;
         lpb++;
         dwl++;
      }
   }

   // range check everything
   if( (iD < 1) || (iD > 31) || (iM < 1) || (iM > 12) ||
      (iHr < 0) || (iHr > 23) || (iMin < 0) || (iMin > 59) )
   {
      return 0;
   }

   if( iY < 100 )
   {
      // we hv say 98, 99,00,02, etc
      if( iY < 80 )
         iY += 2000;
      else
         iY += 1900;
   }

   lpdt[ilen] = 0;

#ifdef   USESYSTIME

   memset(pt, 0, sizeof(SYSTEMTIME));

   pt->wDay    = (WORD)iD;
   pt->wMonth  = (WORD)iM;
   pt->wMinute = (WORD)iMin;
   pt->wHour   = (WORD)iHr; // NOTE PM CORRECTION
   pt->wYear   = (WORD)iY;

   if( !SystemTimeToFileTime( pt,   // system time
      pft ) )
      return 0;


   if(( (WORD)iD != pt->wDay ) ||
            ( (WORD)iM != pt->wMonth ) ||
            ( (WORD)iY != pt->wYear ) ||
            ( (WORD)iMin != pt->wMinute ) ||
            ( (WORD)iHr != pt->wHour ) )
   {
      lpft = FileTime2AscStg( pft );
      chkme( "WARNING: Different [%s] to [%s]"MEOR, lpdt, lpft );
   }

#else // !#ifdef   USESYSTIME

   if( iY < 1980 )
      return 0;

   iY -= 1980;

//   wFatDate = (short)((iY << 8) | (iM << 4) | iD);
//   wFatTime = (short)((iHr << 10) | (iMin << 4));
   wFatDate = ((iY << 8) | (iM << 4) | iD);
   wFatTime = ((iHr << 10) | (iMin << 4));

   if( !DosDateTimeToFileTime( (WORD)wFatDate,          // 16-bit MS-DOS date
      (WORD)wFatTime,          // 16-bit MS-DOS time
      pft ) )          // 64-bit file time
      return 0;

   lpdt[ilen] = 0;

   {
      LPTSTR lpft = FileTime2AscStg( pft );
      SYSTEMTIME  st;
      sprtf( "File=[%s] but get [%s]"MEOR, lpdt, lpft );
      if( FileTimeToSystemTime( pft, &st ) )  // file time to convert
      {
         if(( iD != st.wDay ) ||
            ( iM != st.wMonth ) ||
            ( (iY + 1980) != st.wYear ) ||
            ( iMin != st.wMinute ) )
         {
            chkme( "WARNING: Different"MEOR );
         }
      }
      else
      {
         chkme( "WARNING: FILETIME to SYSTEMTIME FAILED!"MEOR );
      }
   }

#endif   // #ifdef   USESYSTIME

   return dwl; // return LENGTH of string USED in Date/Time DECODE
   // into the 64-bit file time
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : NiceNumberStg
// Return type: LPTSTR 
// Arguments  : DWORD dwNum
//            : DWORD dwLen
// Description: A number with commas
//              FIX20030910 - ensure NO trailing 'rubbish'
///////////////////////////////////////////////////////////////////////////////
LPTSTR   NiceNumberStg( DWORD dwNum, DWORD dwLen )
{
   LPTSTR   lpb = GetNxtBuf();
   LPTSTR   lpb2 = GetNxtBuf();
   DWORD    dwi, dwl, dwj;

   sprintf(lpb,"%u", dwNum);
   dwl = strlen(lpb);
   dwi = dwj = 0;
   while(dwl--)
   {
      lpb2[dwi++] = lpb[dwj++];
      if( dwl && ( (dwl % 3) == 0 ) )
         lpb2[dwi++] = ',';
   }
   // *ENSURE* zero termination
   lpb2[dwi] = 0; // FIX20030910 for -w switch problem
   // could us dwi, but
   dwl = strlen(lpb2);
   *lpb = 0;
   while(dwl < dwLen)
   {
      strcat(lpb," ");
      dwl++;
   }

   strcat(lpb, lpb2);

   strcpy(lpb2,lpb);

   return lpb2;
}

LPTSTR   FileTime2AscStg( FILETIME * pft )
{
   LPTSTR lpb = GetNxtBuf();
   SYSTEMTIME  st;

   if( FileTimeToSystemTime( pft,   // file time to convert
      &st ) )   // receives system time
   {
//         "%02d-%02d-%02d %02d:%02d:%02d",
       sprintf(lpb,
           "%02d-%02d-%02d %2d:%02d",
           st.wDay,
           st.wMonth,
           (st.wYear % 100),
           st.wHour,
           st.wMinute);
         // st.wSecond
   }
   else
   {
      strcpy(lpb, "??-??-?? ??:??");
   }

   return lpb;
}



BOOL  Chk4Debug( LPTSTR lpd )
{
   BOOL     bret = FALSE;
   LPTSTR ptmp = &gszTmpBuf[0];
   LPTSTR   p;
   DWORD  dwi;

   strcpy(ptmp, lpd);
   dwi = strlen(ptmp);
   if(dwi)
   {
      dwi--;
      if(ptmp[dwi] == '\\')
      {
         ptmp[dwi] = 0;
         p = strrchr(ptmp, '\\');
         if(p)
         {
            p++;
            if( strcmpi(p, "DEBUG") == 0 )
            {
               *p = 0;
               strcpy(lpd,ptmp);    // use this
               bret = TRUE;
            }
         }
      }
   }
   return bret;
}

VOID  GetModulePath( LPTSTR lpb )
{
   LPTSTR   p;
   GetModuleFileName( NULL, lpb, 256 );
   p = strrchr( lpb, '\\' );
   if( p )
      p++;
   else
      p = lpb;
   *p = 0;
#ifndef  NDEBUG
   Chk4Debug( lpb );
#endif   // !NDEBUG

}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : ALeft
// Return type: LPTSTR 
// Arguments  : LPTSTR lpl
//            : DWORD dwi
// Description: Return the LEFT portion of a string
//              Emulates the Visual Basic function
///////////////////////////////////////////////////////////////////////////////
LPTSTR   ALeft( LPTSTR lpl, DWORD dwi )
{
   LPTSTR   lps = LocalAlloc(LPTR, (dwi+1));
   DWORD    dwk;
   if(!lps)
   {
      sprtf( "CRITIAL ERROR: Memory FAILED on %d bytes!"MEOR, (dwi+1) );
      exit(-1);
   }
   for( dwk = 0; dwk < dwi; dwk++ )
      lps[dwk] = lpl[dwk];
   lps[dwk] = 0;
   return lps;
}
LPTSTR   FLeft( LPTSTR lps )
{
   if(lps)
      LocalFree(lps);
   return NULL;
}

/*---------------------------------------
   was BumpFile.C -- Bridge for Windows
               (c) Geoff McLane, 1995 - 2000
  ---------------------------------------*/
#ifdef _MSC_VER
#pragma message ( "Public BumpFileName." )
#endif

VOID	BumpFileName( LPTSTR lps )
{
	int	i, j, k;
	char	c;
   i = 0;
   if(lps)
      i = strlen(lps);
//	if( ( lps ) &&
//		( i = strlen(lps) ) )
   if(i)
	{
		j = i - 1;	// back up 1
		c = lps[j];	// get char
		while(j)
		{
			c = lps[j];	// get char
			if( c == '.' )
			{
				j--;		// back to BEFORE point
				c = lps[j];	// get char
				break;	// got the POINT
			}
			j--;	// continue backing up
		}
		k = j;	// keep file name end - ie before point
		if(( k                             ) &&
			( ( c != '\\' ) || ( c != ':' ) ) )
		{
			// we appear to have some filename length
			while(j)
			{
				j--;
				c = lps[j];	// get char
				if( ( c != '\\' ) || ( c != ':' ) )
				{
					j++;
					break;
				}
			}
//			if( (j - k) < 8 )
//			{
//
//			}
//			else
			{
				while( k >= j )
				{
					c = lps[k];	// get char
					if( ( c >= '0' ) && ( c <= '9' ) )
					{
						if( c == '9' )
						{
							lps[k] = '0';
						}
						else
						{
							c++;
							lps[k] = c;
							break;
						}
					}
					else
					{
						// aint numeric
						lps[k] = '0';
						break;
					}
					k--;	// back up another
				}
			}
		}
	}
}

// HOW TO DETECT if the CONSOLE stdout is REDIRECTED to a file?
// ============= fprintf() fopen() ????
// Found a way! Presently using handle to get mode. If this get
// fails, then we are being redirected!!

void  DO(PTSTR a)
{
   DWORD _dww;
   if( gbRedON ) {
      WriteFile(ghErrOut,a,strlen(a),&_dww,NULL); }
   WriteFile(ghStdOut,a,strlen(a),&_dww,NULL);
   DiagString(a);
}


// specialised bump of JUST the file NAME
// NO PATH and NO EXTENT passed
int BumpName8( char * ps )
{
	int	i = 0;
	char	c;
	if( ps )
      i = strlen( ps );

   if(i) {
      int j = i;
		if( i < 8 ) {
         // just extend out to 8 characters first
			ps[i++] = '0';
			ps[i] = 0;
         return i;
      } else {
         // back up to the last NUMBER, if any ...
			while( i-- )
			{
				c = ps[i];
				if( (c >= '0') && (c <= '9') ) {
					if( c == '9' ) {
						ps[i] = '0';
                  j++;
               } else {
						c++;
						ps[i] = c;
						return j;
					}
            } else {
					ps[i] = '0';
					return j;
				}
			}
		}
	}
   return 0; // no new name
}

// from UTILS\GRMLib.c

// ==========================================================
// int	SplitFN( LPSTR lpdir, LPSTR lpfil, LPSTR lpext )
//
// INPUT: lpdir - Destination of PATH.
//        lpfil - Destination of FILENAME portion.
//        lpext - The combined PATH\FILNAME source.
//
// OUTPUT: The lpext string is moved to the lpdir buffer,
//		noting the last instance of ':', '\' or '/',
//		which is then used to copy the traling FILENAME
//		to lpfil, and zero terminating lpdir AFTER this
//		character.
//
// RETURN:	Length of DIRECTORY (if any)
//			0 if no PATH
// ==========================================================
int	SplitFN( LPSTR lpdir, LPSTR lpfil, LPSTR lpext )
{
	int		i, j, k, l;
	char	c;

	k = 0;		// Start NO DIRECTORY
	i = strlen( lpext ); 
	if(i)
	{
		if( lpdir )
			*lpdir = 0;
		if( lpfil )
			*lpfil = 0;
		for( j = 0; j < i; j++ )
		{
			c = lpext[j];
			if( ( c == ':' ) || ( c == '\\' ) || ( c == '/' ) )
			{
				k = j + 1;
			}
			if( lpdir )
				lpdir[j] = c;
		}
		if( lpdir )
			lpdir[j] = 0;	// ZERO termination
		if( k ) // position of drive colon, or either path separator
		{
			j = 0;	// Start at ZERO
			for( l = k; l < i; l++ )
			{
				// Move the tail in as the FILE NAME
				c = lpext[l];
				if( lpfil )
					lpfil[j] = c;
				j++;
			}
			if( lpfil )
				lpfil[j] = 0;	// Zero terminate FILE NAME
			if( lpdir )
				lpdir[k] = 0;	// Remove TAIL of DIRECTORY
		}
		else	// No PATH
		{
			// Take whole name as the FILE
			if( lpfil && lpdir )
				strcpy( lpfil, lpdir );
			if( lpdir )
				*lpdir = 0;	// and remove any PATH!
		}
	}
	return k;
}

int HasDot( LPSTR lps ) // chqnge 2004 to get first DOT from end
{
   int flg = 0;
   int i, j;
   char    c;
   i = strlen( lps );
   if(i) {
      for( j = (i - 1); j >= 0; j-- ) {
         c = lps[j];
         if( c == '.' ) {
           //flg = TRUE;
           flg = j + 1;    // Including the DOT
           break;
         }
      }
   }
   return flg;
}

// FIX20040317 A* should become A*.*
BOOL	grmlib_SplitExt( LPSTR lpb, LPSTR lpe, LPSTR lpf )
{
	BOOL	flg = FALSE;
	int		i, j, k, l;
	char	c;

   i = 0;
   if(lpf)
      i = strlen(lpf);
	if(i)
	{
		j = HasDot( lpf ); 
		if(j)
		{
			if( ( j < i ) &&
				( (i - j) <= 3 ) )
			{
				k = 0;
				for( l = 0; l < j; l++ )
				{
					c = lpf[l];
					if( c == '.' )
					{
						l++;
						break;
					}
					lpb[k++] = c;
				}
				lpb[k] = 0;
				k = 0;
				for( ; l < i; l++ )
				{
					c = lpf[l];
					if( c != '.' )
						lpe[k++] = c;
				}
				lpe[k] = 0;
			}
			else
			{
				if( lpb )
					strcpy( lpb, lpf );
				if( lpe )
					*lpe = 0;
			}
		}
		else
		{
         // has NO dot!
			if( lpb )
				strcpy( lpb, lpf );
         if( lpe ) {
				*lpe = 0;
            // FIX20040317 A* should become A*.*
            if( i && ( lpf[i-1] == '*' ) ) {
               strcpy(lpe,"*"); // add the second wild
            }
         }
		}
   } // if has length, i
	return	flg;
}

char* get_nn(char* num) // nice number nicenum add commas
{
    char* cp = GetNxtBuf();
    size_t len = strlen(num);
    size_t ii, jj, kk, out;
    if (len <= 3) {
        strcpy(cp, num);
    }
    else {
        ///if (len > 3) {
        size_t mod = len % 3;
        size_t max = (int)(len / 3);
        ii = 0;
        for (; ii < mod; ii++) {
            cp[ii] = num[ii]; // copy in upper numbers, if any
        }
        cp[ii] = 0;
        out = ii;
        for (jj = 0; jj < max; jj++) {
            if (!((mod == 0) && (jj == 0)))
                cp[out++] = ',';    // add in comma
            for (kk = 0; kk < 3; kk++) {
                // then 3 numbers
                cp[out++] = num[ii + (jj * 3) + kk];
            }
        }
        cp[out] = 0;
    }
    return cp;
}


char* Get_I64_Stg(__int64 num)
{
    char* cp = GetNxtBuf();
#if defined(_WIN32) && !defined(USE_COMP_FIO)
    sprintf(cp, "%I64d", num);
#else
    sprintf(cp, "%lld", num);
#endif
    cp = get_nn(cp);
    return cp;
}

// eof - FixFUtil.c
