
// FixGeoff.c
#include "FixF32.h"
#ifdef ADDGEOFF2  // moving the dump4 -g command to here in FixF32

typedef struct tagRUNSTR {
   LPTSTR   lpexe;
   LPTSTR   lpcmd;
}RUNSTR, * PRUNSTR;

// ===============================================================
//                                1234567890123456
static   TCHAR _s_szBnd[264] = { "----------------" };
static   DWORD _s_iMinBnd = MMINBND;   // default length
static   TCHAR _s_iBgnChr = ';';       // first character of separator

#define  ginstr   InStr

TCHAR	g_szHelpg[] =
	"Switch -g[xpis][o][b]: Dump as geoff's telephone file."MEOR
   "  -gh | g?    This brief help."MEOR
   "  -gx[1|0]    Set/Reset Do Extra. (def. gbDoExtra=F)."MEOR
   "  -gp[1|0]    Set/Reset Show password(s) (def. gbChkPWD=T)."MEOR
   "  -gi[1|0]    Set/Reset Ignore 123ABC (def. gbIgnoreMNU=T)."MEOR
   "  -gs[1|0]    Set/Reset primary entry. (def. gbUseSee=F)."MEOR
   "  -g[o[\"]Name[\"]] Output file name (gcGOutFile)."MEOR
   "  -gb\";usep-\" Set block separation. (def. ;----------------)."MEOR
   "                                                  Happy File Fixing!";

TCHAR g_szMSDEV[] = "\"C:\\Program Files\\Microsoft Visual Studio\\"
      "Common\\MSDev98\\Bin\\MSDEV.EXE\"";

//TCHAR g_szMSEXCEL3[] = "\"C:\\Program Files\\Microsoft Office\\"
//      "Office\\Excel.exe\"";

TCHAR g_szMSEXCEL1[] = "C:\\Program Files\\Microsoft Office\\Office\\";
TCHAR g_szMSEXCEL2[] = "EXCEL.exe";

TCHAR g_szMSEXCLNK[] = "C:\\Windows\\Start Menu\\Programs\\French Menu\\"
   "Microsoft Office Tools\\";
TCHAR g_szMSEXCLNK2[] = "Microsoft Excel.lnk";

BOOL  gbUseLnk = FALSE; // appears LNK does not work
BOOL  bTrySys  = TRUE;

TCHAR  g_szCmdLine[264];
TCHAR  g_szExeFile[264];
WIN32_FIND_DATA   wfd;

// forward references
void  OutBlock( LPTSTR lpb, DWORD dwLen, DWORD dwBlkNum );
void  OutLineMem( void );
DWORD WINAPI do_runexcell( PVOID lpParam );  // thread data
LPTSTR   GetExQuotes( LPTSTR cp );

VOID  RunExcell( LPTSTR lpf )
{
   static RUNSTR rs;
   PRUNSTR  prs = &rs;
   LPTSTR   lpexe, lpcmd, lptmp;
   MYHAND   hFind;

   memset(prs, 0, sizeof(RUNSTR));

   lpexe = g_szExeFile;
   lpcmd = g_szCmdLine;

   //strcpy( lpexe, "start ");   // THIS FAILED???

   if( gbUseLnk )
   {
      strcpy( lpexe, g_szMSEXCLNK  );
      strcat( lpexe, g_szMSEXCLNK2 );
   }
   else
   {
      strcpy( lpexe, g_szMSEXCEL1 );
      strcat( lpexe, g_szMSEXCEL2 );
   }

   strcpy( lpcmd, "\""  );
   strcat( lpcmd, lpexe );
   strcat( lpcmd, "\""  );
   strcat( lpcmd, " " );
   strcpy( lpcmd, lpf );   // file as command line argument

   prs->lpexe = lpexe;
   prs->lpcmd = lpcmd;

   hFind = FindFirstFile(lpexe, &wfd);
   if( VFH(hFind) )
   {
      FindClose(hFind);
   }
   else
   {
      lptmp = GetExQuotes( lpexe );
      if( lptmp != lpexe )
      {
         hFind = FindFirstFile(lptmp, &wfd);
         if( VFH(hFind) )
         {
            FindClose(hFind);
            strcpy(lpexe,lptmp);
         }
         else
         {
            sprtf( "ERROR: Failed to locate"MEOR
               "[%s]? No CreateProcess done!"MEOR,
               lptmp );
            return;
         }
      }
      else
      {
         sprtf( "ERROR: Failed to locate"MEOR
            "[%s]? No CreateProcess done!"MEOR,
            lpexe );
         return;
      }
   }

   hFind = FindFirstFile(lpf, &wfd);
   if( VFH(hFind) )
   {
      FindClose(hFind);
   }
   else
   {
      sprtf( "ERROR: Failed to locate"MEOR
         "[%s]? No CreateProcess done!"MEOR,
         lpf );
      return;
   }

   if( bTrySys )
   {
      // try using system()
      strcpy(lpexe, "start ");
      strcat(lpexe, lpf     );
      sprtf( "Passing [%s] to system() function ..."MEOR, lpexe);
      system(lpexe);
   }
   else
   {
      // this does NOT seem to LOAD the CSV file?????????
      do_runexcell( (PVOID)prs );
   }
}

VOID  OutBlock2( LPTSTR pblock, DWORD dwl )
{
   if( pblock && dwl )
   {
      OutBlock( pblock, dwl, gdwBlkCnt );
   }
}

DWORD  IsBound( LPTSTR lps, DWORD dws )
{
   DWORD  dwRet = 0;
   DWORD dwi;
   TCHAR  c, d;
//      ( dws > MMINBND ) )
   if(( lps              ) &&
      ( dws > _s_iMinBnd ) )
   {
      if( ginstr(lps,_s_szBnd) == 1 )
      {
         //dwRet = MMINBND;
         dwRet = _s_iMinBnd;
         d = _s_szBnd[dwRet - 1];   // get last char - for repeat continued boundary
         // we could also find the end of the boundary, or end of buffer
         //for( dwi = MMINBND; dwi < dws; dwi++ )
         for( dwi = dwRet; dwi < dws; dwi++ )   // until END OF BUFFER
         {
            c = lps[dwi];  // extract char
            //if( c != '-' )
            if( c != d )
            {
               if( ( c < ' ' ) && ( c != '\t' ) )
               {
                  dwRet++;
                  break;
               }
            }
            dwRet++;
         }
      }
   }
   return dwRet;
}



int	IsDoGeoff( LPTSTR lpb, DWORD dwl )
{
	int	iRet = 0;
   DWORD dwi;
   TCHAR c;

   if( ( lpb ) &&
      ( dwl ) )
   {
      iRet = 1;   // it looks ok so far
      for( dwi = 0; dwi < dwl; dwi++ )
      {
         c = lpb[dwi];  // get char
         if( c )
         {
            if( c < 0 )
            {
               // just a hi-bit, let it go
            }
            else if( c < ' ' )
            {
               if( ( c == 0x09 ) ||
                  ( c == 0x0a ) ||
                  ( c == 0x0d ) ||
                  ( c == 0x1a ) )
               {
                  // these are ok
               }
               else
               {
                  // unknown
                  iRet = 0;
                  break;
               }
            }
         }
         else
         {
            // a null char is real BAD
            iRet = 0;
            break;
         }
      }
   }
	return iRet;
}

DWORD  DoGBuf_NOT_USED( LPTSTR lpb, DWORD dws )
{
   DWORD dwi, dwo, dwc;
   INT   c, d;
   
   dwc = 0;
   d   = _s_iBgnChr;

   for( dwi = 0; dwi < dws; dwi++ )
   {
      c = lpb[dwi];  // extract file char
      //if( c == ';' )
      if( c == d )
      {
         dwo = IsBound( &lpb[ (dwi+1) ], (dws - dwi - 1) );
         if( dwo )
         {
            gdwBlkCnt++;    // bump the BLOCK count
            dwc++;
            dwi += dwo;
         }
      }
   }
   return dwc;
}

DWORD  DoGBufCnt( LPTSTR lpb, DWORD dws )
{
   DWORD dwi, dwo, dwc;
   INT   c, d;
   
   dwc = 0;
   d   = _s_iBgnChr;

   for( dwi = 0; dwi < dws; dwi++ )
   {
      c = lpb[dwi];  // extract file char
      //if( c == ';' )
      if( c == d )
      {
         dwo = IsBound( &lpb[ (dwi+1) ], (dws - dwi - 1) );
         if( dwo )
         {
            dwc++;
            dwi += dwo;
         }
      }
   }
   return dwc;
}

#define  IDW(a)   a++;if(a==0)a--

DWORD  IsBound2( LPTSTR lps, PULARGE_INTEGER pdws )
{
   DWORD dwRet = 0;
   ULARGE_INTEGER dwi, dwm, dws;
   INT  c, d;

   dwm.QuadPart = 0;
   dwm.LowPart  = _s_iMinBnd;

   dws = *pdws;
   //dws.QuadPart = *pdws->QuadPart;   // balance of buffer

   if( lps && ( dws.QuadPart > dwm.QuadPart ) )
   {
      if( ginstr(lps,_s_szBnd) == 1 )
      {
         //dwRet = MMINBND;
         dwRet = _s_iMinBnd;
         d = _s_szBnd[dwRet - 1];   // get last char - for repeat continued boundary
         // we could also find the end of the boundary, or end of buffer
         //for( dwi = MMINBND; dwi < dws; dwi++ )
         dwi.QuadPart = 0;
         dwi.LowPart  = dwRet;
         for( ; dwi.QuadPart < dws.QuadPart; dwi.QuadPart++ )   // until END OF BUFFER
         {
            c = lps[dwi.QuadPart];  // extract char
            //if( c != '-' )
            if( c != d )
            {
               if( ( c < ' ' ) && ( c != '\t' ) )
               {
                  //dwRet++;
                  IDW(dwRet);
                  break;
               }
            }
            //dwRet++;
            IDW(dwRet);
         }
      }
   }
   return dwRet;
}


BOOL  DoGeoff2( LPTSTR lpf )
{
   PMPDFIL pmf = GetFileMap( lpf );
   DWORD    dws, dwc, dwo, dwl;
   LPTSTR   lpbgn, lpb, pblock;
   ULARGE_INTEGER  uli, ulib, ulibal;
   INT      c, d;

   dwc = 0;
   if(pmf && pmf->pData && pmf->sliSize.QuadPart)
   {
      uli.QuadPart  = 0;
      ulib.QuadPart = pmf->sliSize.QuadPart;
      lpbgn = pmf->pData;
      while( ulib.QuadPart )
      {
         if( ulib.HighPart )
            dws = (DWORD)-1;
         else
            dws = ulib.LowPart;

         lpb = &lpbgn[ uli.QuadPart ];

         dwc += DoGBufCnt( lpb, dws );

         ulib.QuadPart -= dws;
         uli.QuadPart  += dws;
      }

      if( dwc )   // if we have BLOCKS
      {
         // process the BLOCKS
         sprtf( "Input file appears to have %d blocks ... processing ..."MEOR,
            dwc );
         ulib.QuadPart = pmf->sliSize.QuadPart;
         uli.QuadPart  = 0;
      }
      else
      {
         return FALSE;
      }

//      while( ulib.QuadPart )
//      {
//         if( ulib.HighPart )
//            dws = (DWORD)-1;
//         else
//            dws = ulib.LowPart;
//
//         lpb = &lpbgn[ uli.QuadPart ];
//
//         dwc -= DoGBuf( lpb, dws );

//         ulib.QuadPart -= dws;
//         uli.QuadPart  += dws;
//      }
      d   = _s_iBgnChr;
      pblock = lpbgn;   // degin FIRST block start
      dwl    = 0;       // and LENGTH of BLOCK
      // ******* NB: Maximum single BLOCK is 64K ********
      for( uli.QuadPart = 0; uli.QuadPart < ulib.QuadPart; uli.QuadPart ++ )
      {
         lpb = &lpbgn[ uli.QuadPart ];
         c = *lpb;   // extract character
         if( ( c == d ) &&
            ( lpb[1] == _s_szBnd[0] ) )
         {
            ulibal.QuadPart = (ulib.QuadPart - uli.QuadPart);
            dwo = IsBound2( &lpb[1], &ulibal );
            if( dwo && dwl )
            {
               if(gdwBlkCnt)
                  dwl--;      // remove first INC of main loop
               gdwBlkCnt++;    // bump the BLOCK count
               OutBlock2( pblock, dwl );
               uli.QuadPart += dwo;
               pblock = &lpbgn[ uli.QuadPart ]; // get begin of NEXT block
               dwl = 0; // NOTE INC as part of main loop
            }
         }
         IDW(dwl);
      }

      if(VERB)
         sprtf( "Done input file. Closing MAP ..."MEOR );

      DeleteFileMap( pmf );

      OutLineMem();  // does LineMemOut(...) to -o[-|+]output.fil

      return TRUE;
   }

   return FALSE;
}

LPTSTR   GetExQuotes( LPTSTR cp )
{
   LPTSTR   lps;
   INT      ilen;
   if( *cp != 34 )
      return cp;
   lps = GetNxtBuf();
   strcpy(lps, &cp[1]);
   ilen = strlen(lps);
   if(ilen && ( lps[ilen-1] == 34 ) )
   {
      ilen--;
      lps[ilen] = 0;
   }
   return lps;
}

// -g - process the input as a sort of free form database
// There must be a unique separator between each record.
// -gb";--------------" - Change boundary/separation string
// -go"Output File in delin. form.CSV"
// ***********************************
BOOL  Get_G_Cmd( LPTSTR lpc )
{
   DWORD dwl = strlen(lpc);
   DWORD dwi;
   INT   c;
   LPTSTR   cp;
   LPTSTR   ptmp;
   LPTSTR   lps;

   if( Pgm_Flag & Do_Geoff )
   {
      // multiple commands - are ok
      // return FALSE;
   }

   c = *lpc;
   if( c == 0 )
   {
      //return TRUE;
      goto Set_Flag; // no command = ADD FLAG
   }

   for( dwi = 0; dwi < dwl; dwi++ )
   {
      cp = &lpc[dwi];
      c = toupper(*cp);
      switch( c )
      {
#ifdef ADDGEOFF1  // -g1 command
      case '1':
         Pgm_Flag |= Do_Geoff1; // -g1 - process special file to CSV output
         break;
#endif   // #ifdef ADDGEOFF1  // -g1 command

      case 'B':
         {
            LPTSTR   lpb = _s_szBnd;
            cp++; // past the 'B'
            lps = GetExQuotes(cp);
            cp = lps;
            if( ( dwl > 2 ) && ( dwl < 256 ) )
            {
               _s_iBgnChr = *cp;       // get first character of separator
               cp++;    // past first
               ptmp = &lpc[dwi + 2];
               ptmp = cp;  // pointer to balance of line
               //strcpy(_s_szBnd, &lpc[dwi + 2] );   // get new = { "----------------" };
               //strcpy(_s_szBnd, ptmp );   // get new = { "----------------" };
               strcpy(lpb, ptmp );   // get new = { "----------------" };

               //_s_iMinBnd = strlen(_s_szBnd);   // length
               _s_iMinBnd = strlen(lpb);   // length

               goto Set_Flag; // got command = ADD FLAG
            }
            else
            {
               // errant -gs"something" switch
               return FALSE;
            }
         }
         break;

      case '?':
      case 'H':
         {
            // show HELP
            sprtf(g_szHelpg);
            //exit(0);
            Pgm_Exit(0);
         }
         break;
//  gbDoExtra;  // x
//  gbChkPWD;   // = TRUE; p[0|1|+|-]
//  gbIgnoreMNU;// = TRUE; i // ignore 123ABC
//  gbUseSee;  // = FALSE; s // add see (primary entry)
      // =================
      case 'X':
      case 'P':
      case 'I':
      case 'S':
         {
            LPBOOL pb = &gbDoExtra;
            cp = &lpc[dwi];
            if( c == 'X' )
               pb = &gbDoExtra;
            else if( c == 'P' )
               pb = &gbChkPWD;
            else if( c == 'I' )
               pb = &gbIgnoreMNU;
            else if( c == 'S' )
               pb = &gbUseSee;
            else
               return FALSE;

            //gbDoExtra = TRUE;
            *pb = TRUE;

            cp++;
            c = *cp;
            if( ( c == '0' ) ||
               ( c == '-' ) )
            {
               //gbDoExtra = FALSE;
               *pb = FALSE;
               cp++;
               c = *cp;
               dwi++;
            }
            else if( ( c == '1' ) ||
               ( c == '+' ) )
            {
               //gbDoExtra = TRUE;
               *pb = TRUE;
               cp++;
               c = *cp;
               dwi++;
            }
            else if( c )
            {
               // is this an ERROR???
               // NO - can be more otion letters, so NO return FALSE;
            }
         }
         break;

      case 'O':   // -go[output file]
         {
            LPTSTR   lps2 = GetNxtBuf();
            // had the G, now

            cp++; // bump past the 'O'
            lps = GetExQuotes(cp);
            _fullpath(lps2,lps,256);
            strcpy( &gcGOutFile[0], lps2 );
            goto Set_Flag; // got command = ADD FLAG
         }
         break;

      default:
         {
            // unknown switch
            return FALSE;
         }
         break;
      }
   }

Set_Flag:

   Pgm_Flag |= Do_Geoff;

   return TRUE;

}


// ************************* from dump4 *****

// Dump4g.c
//#include	"Dump4.h"
//#include "DumpList.h"

//#define	BLDDATE		"14 October, 2000"	// add current DATE to As At entry (if found)
#undef   CHKMSTANG      // for chkme on Ms Tang record
#undef   ADDCHKARM      // stop at Armand ...
#undef   CHKANNICK
#undef   CHKROWINA
#undef   CHKYVAN
#undef   CHKSAMELET
#undef  CHKGEORGE
#undef  CHKMHSIN

//#define  NMLEN    24
//#define  NMMX2C   45
//#define  MMXBU    10
//#define  MXNUMSET       24

#define  Had_Coma    0x00000001
#define  Had_Colon   0x00000002
#define  Had_OpenBr  0x00000004
#define  Had_CloseBr 0x00000008
#define  Had_OpenBc  0x00000010
#define  Had_CloseBc 0x00000020
#define  Had_OpenBs  0x00000040
#define  Had_CloseBs 0x00000080
#define  Had_Cr  0x00000100
#define  Had_Lf 0x00000200
#define  Had_CrLf  0x00000400
#define  Had_Nl   0x00000800

#define  Had_Xchg 0x00001000  // exchanged "AND" for "&"
#define  Got_TWO  0x00002000  // we have Annie & Geoff (split in two)
#define  Done_TWO 0x00004000  // and have listed BOTH
#define  Got_Num  0x00008000  // we have FOUND an 8, or 7 digit number in stream

#define  Had_PWD  0x00010000  // had PWD ... in stream
#define  Done_X1  0x00020000
#define  Done_X2  0x00040000


#define  Got_No01    0
#define  Got_No02    1

DWORD dwMaxbu      = MMXBU;
BOOL  bAddbu       = TRUE;
BOOL  bInverse     = FALSE;
BOOL  bDoSort      = TRUE;
BOOL  bAddShort    = TRUE;
BOOL  bRemoveComts = TRUE;
BOOL  bDoMaxLen    = TRUE;
BOOL  bAddthdr     = FALSE;
BOOL  bDoFill2     = FALSE;
BOOL  bNo2Commas   = TRUE;
BOOL  bAddSpPad    = FALSE;
BOOL  bUseArray    = FALSE;

//extern   MYHAND	grmCreateFile( LPTSTR fn );
//extern   BOOL     grmWriteFile( MYHAND * ph, LPTSTR lpb );
//extern   BOOL     grmCloseFile( MYHAND * ph );
BOOL     grmCloseFile( MYHAND * ph )
{
   MYHAND   h = *ph;
   if( VFH(h) )
   {
#ifdef USE_COMP_FIO
       fclose(h);
#else //!USE_COMP_FIO
      CloseHandle(h);
#endif // USE_COMP_FIO y/n
      *ph = 0;
      return TRUE;
   }
   return FALSE;
}

BOOL     grmWriteFile( MYHAND * ph, LPTSTR lpb )
{
   MYHAND   h = *ph;
   if( VFH(h) )
   {
      DWORD dww;
      DWORD dwl = strlen(lpb);
      if(dwl)
      {
#ifdef USE_COMP_FIO
          dww = fwrite(lpb, 1, dwl, h);
          if (dwl == dww)
              return TRUE;
          grmCloseFile(ph);
          *ph = 0;
#else // !USE_COMP_FIO
         if( WriteFile(h,lpb,dwl,&dww,NULL) && ( dwl == dww ) )
         {
            return TRUE;
         }
         grmCloseFile( ph );
         *ph = INVALID_HANDLE_VALUE;
#endif // USE_COMP_FIO y/n
      }
      else
      {
         return TRUE;
      }
   }
   return FALSE;
}


DWORD GetWdCnt( LPTSTR lptmp );

void  chkcpy( LPTSTR lpd, LPTSTR lps )
{
//   strcpy( &lpb[dwt], &lpb[dwoff] );
   strcpy( lpd, lps );
}
void  chkcpyn( LPTSTR lpd, LPTSTR lps, DWORD dwl )
{
//   strcpy( &lpb[dwt], &lpb[dwoff] );
   lstrcpyn( lpd, lps, dwl );
}

// right trim a buffer
int  grtrim( LPTSTR lps )
{
   int   iRet = 0;
   int   i, c;
   i = 0;
   if( lps )
      i = strlen(lps);
   if(i)
   {
      i--;
      c = lps[i];
      while( c && ( c <= ' ' ) )
      {
         lps[i] = 0;
         iRet++;
         if(i) {
            i--;
            c = lps[i];
         } else
            break;
      }
   }
   return iRet;
}

int  gaddsp( LPTSTR lps )
{
   int   iRet = 0;
   int   i, c;
   i = 0;
   if( lps )
      i = lstrlen(lps);
   if( i )
   {
      i--;
      c = lps[i];
      if( c > ' ' )
      {
         strcat(lps," "); // append a SPACE
         iRet++;
      }
   }
   return iRet;
}


int	IsDoGeoff_ORG( LPTSTR lpb, DWORD dwl )
{
	int	iRet = 0;
   DWORD dwi;
   TCHAR c;

   if( ( lpb ) &&
      ( dwl ) )
   {
      iRet = 1;   // it looks ok so far
      for( dwi = 0; dwi < dwl; dwi++ )
      {
         c = lpb[dwi];  // get char
         if( c )
         {
            if( c < 0 )
            {
               // just a hi-bit, let it go
            }
            else if( c < ' ' )
            {
               if( ( c == 0x09 ) ||
                  ( c == 0x0a ) ||
                  ( c == 0x0d ) ||
                  ( c == 0x1a ) )
               {
                  // these are ok
               }
               else
               {
                  // unknown
                  iRet = 0;
                  break;
               }
            }
         }
         else
         {
            // a null char is real BAD
            iRet = 0;
            break;
         }
      }
   }
	return iRet;
}

//#define  MMINBND     16
//                           1234567890123456
//static   TCHAR _s_szBnd[] = "----------------";
//extern   int   ginstr( LPTSTR lps, LPTSTR lpi );
//extern   int   ginstriw( LPTSTR lps, LPTSTR lpi );
//extern   int   ginstri( LPTSTR lps, LPTSTR lpi );

DWORD  IsBound_ORG( LPTSTR lps, DWORD dws )
{
   DWORD  dwRet = 0;
   DWORD dwi;
   char  c;
   if( ( lps ) &&
      ( dws > MMINBND ) )
   {
      if( ginstr(lps,_s_szBnd) == 1 )
      {
         dwRet = MMINBND;
         // we could also find the end of the boundary, or end of buffer
         for( dwi = MMINBND; dwi < dws; dwi++ )
         {
            c = lps[dwi];
            if( c != '-' )
            {
               if( c < ' ' )
               {
                  dwRet++;
                  break;
               }
            }
            dwRet++;
         }
      }
   }
   return dwRet;
}

//void  OutBlock( &lpb[dwbgn], dwb, dwBlkNum );

#define  pl_1        0
#define  pl_A        1
#define  pl_X1       2
#define  pl_X2       3
#define  MXLSTC      4

#define  m_pl1    _s_pmb[pl_1]
#define  m_pla    _s_pmb[pl_A]
#define  m_plx1   _s_pmb[pl_X1]
#define  m_plx2   _s_pmb[pl_X2]
#define  PLE   PLIST_ENTRY

typedef struct tagMBLST {
	LIST_ENTRY	m_sFBLinks;
   DWORD       m_dwLen;
   int         iRank;
   int         iOrder;
   DWORD       m_dwFlag;
   DWORD       m_dwType;
   DWORD       m_dwWC;
   DWORD       m_dwTwc;
   void *      m_pmb[MXLSTC];
	TCHAR		   m_cBuf[1];
}MBLST;
typedef	MBLST	* LPMBLST;

//MBLST    gsMBLst = {
//   { &gsMBLst.m_sFBLinks, &gsMBLst.m_sFBLinks }, 0 };
MBLST    gsMBLst = {
   { (PLE)&gsMBLst, (PLE)&gsMBLst }, 0 };

LPMBLST  lpgMB = &gsMBLst;

//#define  gdwFlag      lpgmb->m_dwFlag
//#define  gdwWdCnt    lpgmb->m_dwWC
//#define  gdwTitCnt   lpgmb->m_dwTwc    // = GetWdCnt(lpn)
#define  gdwFlag      lpgMB->m_dwFlag
#define  gdwWdCnt    lpgMB->m_dwWC
#define  gdwTitCnt   lpgMB->m_dwTwc    // = GetWdCnt(lpn)

LIST_ENTRY	gsLines = {
	&gsLines,
		&gsLines };


//#ifdef  _DumpList_H

#define  LstCnt2  ListCount2

//#else // NOT #ifdef  _DumpList_H
// removed all
//#endif   // #ifndef  _DumpList_H

//#define	dMALLOC(a)	LocalAlloc(LPTR,a)
//#define  dMFREE(a)   LocalFree(a)

//               gdwWdCnt = GetWdCnt(lpn);
//int   giLnCnt; // count 1, 2 ... max 3 until code added
//TCHAR gcHeadOrg[ (NMMX2C + 15) ];   // Orginal
//TCHAR gcHeadX1[ (NMMX2C + 15) ];    // Last name rotation
//TCHAR gcHeadX2[ (NMMX2C + 15) ];    // Split of last if hythenated
//#define  pl_1        0
//#define  pl_A        1
//#define  pl_X1       2
//#define  pl_X2       3
//#define  MXLSTC      4
//#define  m_pl1    _s_pmb[pl_1]
//#define  m_pla    _s_pmb[pl_A]
//#define  m_plx1   _s_pmb[pl_X1]
//#define  m_plx2   _s_pmb[pl_X2]
// if it needs it, add an END OF LINE Cr/Lf pair - DOS file form
// ==============
int   gaddeol( LPTSTR lps, PINT pi )
{
   int   iRet = 0;
   int   i;
   i = *pi;
   if(i)
   {
      i--;  // back to last char
//      c = lps[i]; // get the char
      if( lps[i] > ' ' )
      {
         i++;
         strcat(lps,MEOR);   // "\r\n" = 2
         iRet = strlen(MEOR);
         i += iRet;
         *pi = i;
      }
   }
   return iRet;
}

//TCHAR gcHead2[1024]; // [ (NMMX2C + 15) ];;
//TCHAR gcTailSee[1024];
//INT   giLnCnt = 0;
//LPTSTR   glpOut2 = 0;

#define  dMALLOC     MALLOC
#define  dMFREE      MFREE
#define  gcOutBuf    gszOutBuf
#define  gcOutBuf2   gszOutBuf2
//DWORD    gdwOutSz = MXOUTBUF;
//LPTSTR   glpOut = 0;
//LPTSTR   glpOut2 = 0;

BOOL		bAdd2List( LPSTR lps, DWORD dwFlag )
{
   static LPMBLST  _s_pmb[MXLSTC];
   BOOL     gbAlloc = FALSE;
   BOOL  bRet = FALSE;
	int		i, k;
	LPMBLST	lpmb, lpmb2;
   LPTSTR   lpd;
   DWORD    dwSiz;
   DWORD    dwo;
   BOOL     bGot2 = FALSE;
   int      iLns = 0;   // no extra lines to play with

//   LPTSTR   lpn = &gcHead1[0];
//   LPTSTR   lpn2 = &gcHead2[0];
   LPTSTR   lph2 = &gcHead2[0]; // [ (NMMX2C + 15) ];;
   LPTSTR   lpt;

   lpt = &gcTailSee[0];
   for( i = 0; i < MXLSTC; i++ )
      _s_pmb[i] = 0;
   k = 0;
   i = 0;
	if( lps )
      i = strlen(lps);
   if( i )
	{
      if( giLnCnt > 1 )
         iLns = giLnCnt - 1;
      bGot2 = (BOOL) ( dwFlag & Got_TWO );
      gaddeol(lps,&i);
      dwSiz = (sizeof(MBLST) + i + 1);
//		if( lpmb = (LPMBLST)dMALLOC( (sizeof(MBLST) + i + 1) ) )
		lpmb = (LPMBLST)dMALLOC( dwSiz );
		if( lpmb )
		{
         m_pl1 = lpmb;
         k++;
         memset(lpmb, 0, dwSiz);

         lpmb->m_dwLen  = i;  // set length of string
         lpmb->m_dwFlag = dwFlag;

         lpd = &lpmb->m_cBuf[0];
         strcpy(lpd,lps);
			InsertTailList(&gsLines, &lpmb->m_sFBLinks);

//         if( ( giLnCnt > 1 ) ||
         if( ( iLns ) ||   // if we have one or two SPLITS of the header
            ( bGot2 ) )    // or we have two prenoms / christian names Got_TWO
         {
            DWORD    dws = (dwSiz + 264);
            LPTSTR   lps2;
//            LPTSTR   lph2 = &gcHead2[0]; // [ (NMMX2C + 15) ];;

//            if( (dwSiz + 264) > MXOUTBUF )  // (1024*16)
            if( dws > MXOUTBUF )  // (1024*16)
            {
               // YOW - getting real big 
               chkme( "Use mem for this size ..." );
//               glpOut2 = dMALLOC( (dwSiz + 264) );
               glpOut2 = dMALLOC(dws);
               gbAlloc = (glpOut2 ? TRUE : FALSE);
               if( !glpOut2 )
               {
                  chkme( "C:ERROR: Bigger OUT memory FAILED ... this is BAD!!!" );
               }
            }

            lps2 = glpOut2;   // maybe fixed or dMALLOC( (dwSiz + 264) ) )
            dwo = ginstr(lps,",");
//            if( lps2 = ALLOC( (dwSiz + 264) ) )
            if( gbUseSee )
            {
               //lpt = &gcTailSee[0];
               if(dwo)
               {
                  //#define  MXNUMSET       24
                  LPTSTR   lpnm = &gcNum[0]; // = [MXNUMSET+8];
                  if( *lpnm )
                  {
                     strcpy(lpt,", ");
                     strcat(lpt,lpnm);
                     strcat(lpt," see ");
                  }
                  else
                  {
                     strcpy(lpt,", see ");
                  }
                  chkcpyn(EndBuf(lpt), lps, dwo);
                  if( dwFlag & Got_Num )
                  {

                  }
               }
            }

            // === this is a previously build HEADER 2 ===
            // if the name looked like say "Annie & Geoff", then ...
            // =====================================================
//            if( ( lph2 ) &&
//               ( *lph2 ) &&
//               ( lps2  ) ) // maybe fixed or ALLOC( (dwSiz + 264) ) )
            if( lps2 ) // maybe fixed or ALLOC( (dwSiz + 264) ) )
            {
//               DWORD dwo;
//               if( dwo = ginstr(lps,",") )
               if( bGot2 && dwo && *lph2 )
               {
                  strcpy(lps2,lph2);  // get the previously prepared 2nd head, and
                  if( gbUseSee )
                  {
                     // TCHAR gcTailSee[ ((NMMX2C*2) + 15) ];
                     //strcat(lps2,", see ");
                     //chkcpyn(EndBuf(lps2), lps, dwo);
                     strcat(lps2,lpt);
                  }
                  else
                  {
                     strcat(lps2, &lps[ (dwo - 1) ] );  // this current tail, incl ", " bgng
                 }
                  i = strlen(lps2);
                  if( i )
                  {
                     gaddeol(lps2,&i);
                     dwSiz = (sizeof(MBLST) + i + 1);
//		if( lpmb = (LPMBLST)ALLOC( (sizeof(MBLST) + i + 1) ) )
		               lpmb2 = (LPMBLST)dMALLOC( dwSiz );
		               if( lpmb2 )
		               {
                        m_pla = lpmb2;
                        k++;
                        memset(lpmb2, 0, dwSiz);
                        dwFlag |= Done_TWO;  // and have listed BOTH

                        lpmb2->m_dwLen  = i;  // set length of string
                        lpmb2->m_dwFlag = dwFlag;
         // back to FIRST listing
         lpmb->m_dwFlag |= dwFlag;

                        lpd = &lpmb2->m_cBuf[0];
                        strcpy(lpd,lps2);
                        // and add the SECOND entry to the LIST
			               InsertTailList(&gsLines, &lpmb2->m_sFBLinks);
                        // and have listed BOTH
                     }
                  }
                  else
                  {
                     chkme( "What! A zero string here!!!" );
                  }
               }

               if( iLns && dwo )
               {
                  LPTSTR   lpn2 = &gcHeadX1[0];
                  strcpy(lps2,lpn2);  // get the previously prepared head, and
                  if( gbUseSee )
                     strcat(lps2,lpt);
                  else
                     strcat(lps2, &lps[ (dwo - 1) ] );  // this current tail
                  i = lstrlen(lps2);
                  if( i )
                  {
                     gaddeol(lps2,&i);
                     dwSiz = (sizeof(MBLST) + i + 1);
//		if( lpmb = (LPMBLST)ALLOC( (sizeof(MBLST) + i + 1) ) )
		               lpmb2 = (LPMBLST)dMALLOC( dwSiz );
                     if( lpmb2 )
		               {
                        m_plx1 = lpmb2;
                        k++;
                        memset(lpmb2, 0, dwSiz);
                        dwFlag |= Done_X1;  // and have listed extended 1

                        lpmb2->m_dwLen  = i;  // set length of string
                        lpmb2->m_dwFlag = dwFlag;
         // back to FIRST listing
         //lpmb->m_dwFlag |= dwFlag;

                        lpd = &lpmb2->m_cBuf[0];
                        strcpy(lpd,lps2);
                        // and add the SECOND entry to the LIST
			               InsertTailList(&gsLines, &lpmb2->m_sFBLinks);
                        // and have listed BOTH
                     }
                  }
                  else
                  {
                     chkme( "What! A zero string here!!!" );
                  }
                  iLns--;
                  if( iLns )
                  {
                  LPTSTR   lpn3 = &gcHeadX2[0];
                  strcpy(lps2,lpn3);  // get the previously prepared head, and
                  if( gbUseSee )
                     strcat(lps2,lpt);
                  else
                     strcat(lps2, &lps[ (dwo - 1) ] );  // this current tail
                  i = lstrlen(lps2);
                  if( i )
                  {
                     gaddeol(lps2,&i);
                     dwSiz = (sizeof(MBLST) + i + 1);
//		if( lpmb = (LPMBLST)ALLOC( (sizeof(MBLST) + i + 1) ) )
//		               if( lpmb2 = (LPMBLST)ALLOC( dwSiz ) )
		               lpmb2 = (LPMBLST)dMALLOC( dwSiz );
		               if(lpmb2)
		               {
                        m_plx2 = lpmb2;
                        k++;
                        memset(lpmb2, 0, dwSiz);
                        dwFlag |= Done_X2;  // and have listed BOTH

                        lpmb2->m_dwLen  = i;  // set length of string
                        lpmb2->m_dwFlag = dwFlag;
         // back to FIRST listing
         //lpmb->m_dwFlag |= dwFlag;

                        lpd = &lpmb2->m_cBuf[0];
                        strcpy(lpd,lps2);
                        // and add the SECOND entry to the LIST
			               InsertTailList(&gsLines, &lpmb2->m_sFBLinks);
                        // and have listed BOTH
                     }
                     else
                     {
                        chkme( "C:ERROR: We have NO MEMORY !!! EEEK !" );
                     }
                  }
                  else
                  {
                     chkme( "What! A zero string here!!!" );
                  }
                  }

               }

               if( gbAlloc )
               {
                  lstrcpyn( &gcOutBuf2[0], lps2, MXOUTBUF );
                  dMFREE(lps2);
                  glpOut2 = &gcOutBuf2[0];
               }

            }
         }
         bRet = TRUE;
		}
      else
      {
         chkme("C:ERROR: Memory FAILED!!!");
      }
	}

   if( k > 1 )
   {
      // we have MULTIPLE listings
      for( i = 0; i < MXLSTC; i++ )
      {
//		   if( lpmb = (LPMBLST)_s_pmb[i] )   // = (LPMBLST)ALLOC( dwSiz ) )
		   lpmb = (LPMBLST)_s_pmb[i]; // = (LPMBLST)ALLOC( dwSiz ) )
		   if(lpmb)   
         {
            // pass possible set of entries to each of the entries
            memcpy( &lpmb->m_pmb[0], &_s_pmb[0], (sizeof(LPMBLST) * MXLSTC) );
            lpmb->m_dwFlag |= dwFlag;  // update the FLAG
         }
      }
   }
   return bRet;
}

int	bGetListCnt( void )
{
	PLIST_ENTRY	ple;
	int	i = 0;
//	Traverse_List( &gsLines )
   PLIST_ENTRY _SX_Flink;
   PLIST_ENTRY _SX_ListHead;
//#define	Traverse_List(ListHead)
   for(_SX_ListHead = (&gsLines),_SX_Flink = _SX_ListHead->Flink;_SX_Flink != _SX_ListHead; _SX_Flink = _SX_Flink->Flink )
	{
		ple = _SX_Flink;
		i++;
	}
	return i;
}

//BOOL  DeleteList( void )
//{
//   BOOL  bRet = FALSE;
//	LPMBLST	lpmb;
//   int   i, j;
//   if( j = bGetListCount )
//   {
//      while(j)
//      {
//         lpmb->s_fbLinks 
//         j--;
//      }
//   }
//   return bRet;
//}

void  DeleteListMem( PLIST_ENTRY ple )
{
   LPTSTR   lps;
//   PLINEHEADER plh;
   LPMBLST plh;
   while( !IsListEmpty(ple) )
   {
      plh = (LPMBLST)RemoveHeadList(ple);
      lps = &plh->m_cBuf[0];
      dMFREE(plh);
   }
}

void  DeleteLineMem( void )
{
   DeleteListMem(&gsLines);
}

DWORD  SetRank( LPMBLST plThis, int iNum, LPTSTR lpThis )
{
   DWORD    dwRet = 0;
   LPTSTR   lps;
   LPMBLST plh;
	int	i = 0;
   int   id;
   DWORD    dwl,dwm,dwc;
//	   Traverse_List( &gsLines )
   PLIST_ENTRY peh = (&gsLines);
   PLIST_ENTRY pel = peh->Flink;
   PLIST_ENTRY peb = peh->Blink;

   dwc = dwl = dwm = 0;
//#define	Traverse_List(ListHead)
//   for(_SX_ListHead = (&gsLines),_SX_Flink = _SX_ListHead->Flink;_SX_Flink != _SX_ListHead; _SX_Flink = _SX_Flink->Flink )

   for( ; pel != peh; pel = pel->Flink )
	{
      peb = pel->Blink;
//         plh = (LPMBLST)_SX_Flink;
      plh = (LPMBLST)pel;
      if( plh != plThis )
      {
         dwc++;
         lps = &plh->m_cBuf[0];
            //SetRank( plh, i, lps );
         id = strcmp(lps,lpThis);
//       if( lstrcmp(lps,lpThis) > 0 )
         if( id > 0 )
         {
            dwm++;
         }
         else if( id < 0 )
         {
            dwl++;
         }
		   i++;
       }
	}

   if(dwc)
   {
      if( bInverse )
      {
         dwRet = dwm + 1;
      }
      else
      {
         dwRet = dwl + 1;
      }
   }

   UNREFERENCED_PARAMETER(iNum);

   return dwRet;

}

void  SortList( int iCnt )
{
   LPTSTR   lps;
   LPMBLST plh;
	int	i = 0;
//	   Traverse_List( &gsLines )
   PLIST_ENTRY pFlink;
   PLIST_ENTRY pListHead;
//#define	Traverse_List(ListHead)
   for(pListHead = (&gsLines),pFlink = pListHead->Flink;pFlink != pListHead; pFlink = pFlink->Flink )
	   {
         plh = (LPMBLST)pFlink;
         lps = &plh->m_cBuf[0];
         plh->iRank =
            SetRank( plh, i, lps );
		   i++;
	   }
   UNREFERENCED_PARAMETER(iCnt);
}

#define  grmCreateFile  Creat_A_File
#define  VH             VFH

VOID  LineMemOut( MYHAND * ph, LPTSTR lps )
{
   static DWORD _s_dwdninit = 0;
   grmWriteFile( ph, lps );
   if( VFH(gOut_Hand) )
   {
      // assume writing CSV - Take length of FIRST to comma,
      // and space fill all others to this same level into out file
      DWORD dwo = InStr(lps, ",");
      DWORD dwl = strlen(lps);
      if( ( dwo > 1 ) && ( dwl > 3 ) )
      {
         LPTSTR   pleft = Left(lps, (dwo - 1));
         if( _s_dwdninit == 0 )
            _s_dwdninit = dwo;
         while(strlen(pleft) < _s_dwdninit)
            strcat(pleft," ");
         grmWriteFile( &gOut_Hand, pleft );
         pleft = Right(lps, (dwl - (dwo - 1)));
         grmWriteFile( &gOut_Hand, pleft );
      }
      else
      {
         grmWriteFile( &gOut_Hand, lps );
      }
   }
}

void  OutLineMem( void )
{
   BOOL  bOk = FALSE;
	PLIST_ENTRY	ple;
   LPTSTR   lps;
   LPMBLST plh;
   PLIST_ENTRY pFlink;
   PLIST_ENTRY pListHead;
	int	i = 0;
   int   iCnt = 0;
   MYHAND   h = 0;
   LPTSTR   lps2 = glpOut2;
   LPTSTR   lpf  = &gcGOutFile[0];   // = [264] = {"\0"};

   LstCnt2( &gsLines, &iCnt );
   
   if( iCnt )
   {
      if( *lpf )
         h = grmCreateFile( lpf );
      if( VH(h) )
      {
         // handle is OPEN
      }
      else
      {
         h = 0;
      }

      if( bDoSort )
      {
         if(h)
         {
            sprintf(lps2,
               MEOR"Sorted list of %d names and telephone numbers to [%s] ..."MEOR,
               iCnt,
               lpf );

            if(VERB)
               sprtf(lps2);
//            grmWriteFile( &h, lps2 );
         }
         else
         {
            if(VERB)
            {
              sprtf( MEOR
               "Sorted list of %d names and telephone numbers ..."
               MEOR,
               iCnt);
            }
         }
      }
      else
      {
            sprintf(lps2,MEOR
               "List of %d names and telephone numbers ..."
               MEOR,
               iCnt);
            if(VERB)
               sprtf(lps2);
//            grmWriteFile( &h, lps2 );
      }

      if( bDoSort )
      {
         SortList(iCnt);
         i = 0;
         for(pListHead = (&gsLines),pFlink = pListHead->Flink;pFlink != pListHead; pFlink = pFlink->Flink )
         {
            ple = pFlink;
            plh = (LPMBLST)pFlink;
            lps = &plh->m_cBuf[0];
            if( plh->iRank == i )
            {
               if( VERB6 )
                  sprtf(lps);
//               grmWriteFile( &h, lps );
               LineMemOut( &h, lps );
            }
	      }

         while( i <= iCnt )
         {
            i++;
//	   Traverse_List( &gsLines )
            for(pListHead = (&gsLines),pFlink = pListHead->Flink;pFlink != pListHead; pFlink = pFlink->Flink )
	         {
		         ple = pFlink;
               plh = (LPMBLST)pFlink;
               lps = &plh->m_cBuf[0];
               if( plh->iRank == i )
               {
                  if( VERB6 )
                     sprtf(lps);
                  //grmWriteFile( &h, lps );
                  LineMemOut( &h, lps );
               }
	         }
         }
      }
      else
      {
         // no sort required
            for(pListHead = (&gsLines),pFlink = pListHead->Flink;pFlink != pListHead; pFlink = pFlink->Flink )
	         {
		         ple = pFlink;
               plh = (LPMBLST)pFlink;
               lps = &plh->m_cBuf[0];
//               if( plh->iRank == i )
//               {
               if(VERB6)
                  sprtf(lps);

                  //grmWriteFile( &h, lps );
                  LineMemOut( &h, lps );
//               }
	         }
      }

   
      sprintf(lps2,"End List of %d names and telephone numbers ..."
         MEOR,
         iCnt);
      if(VERB)
         sprtf(lps2);
//      grmWriteFile( &h, lps2 );
      if( VFH(h) )
         bOk = TRUE;

      grmCloseFile( &h );

   }

   DeleteLineMem();

   if( bOk )
   {
      RunExcell( lpf );
   }
}

//LPTSTR   glpOut = &gcOutBuf[0];
//DWORD    gdwOutSz = MXOUTBUF;
//#define  IsNChr(a)   ( ( a >= '0' ) && ( a <= '9' ) )
//#define  IsUChr(a)   ( ( a >= 'A' ) && ( a <= 'Z' ) )
//#define  IsLChr(a)   ( ( a >= 'a' ) && ( a <= 'z' ) )

//#define  IsPChr(a)   ( IsNChr(a) || IsUChr(a) || IsLChr(a) )

//#define  NMLEN    24
//#define  NMMX2C   45
//#define  MMXBU    10
//BOOL  bAddbu = FALSE;

//DWORD dwMaxbu = MMXBU;
//TCHAR gcHead2[ (NMMX2C + 15) ];
// ============================


DWORD  chkbu( LPTSTR lpb, LPDWORD pdw )
{
   DWORD dwRet = 0;
   DWORD dwo = *pdw;
   LPTSTR   lpc = &lpb[dwo];
   TCHAR    c;

//   LPTSTR   lpd;
   c = *lpc;
   // 1. Must END with a COLON
   if( ( bAddbu ) &&
      ( ( c == ':' ) ||
      ( ( c == ' ' ) && ( ginstr(lpc,":") == 2 ) ) ) )
   {
      dwo--;
//      lpd = &lpb[dwo];
      if( lpb[dwo] == ' ' )
      {
         // tolerate ONE space between end of word and colon
         if(dwo)
            dwo--;
//         lpd = &lpb[dwo];
      }
      // if there is a word, back up until it ends,
      // or back at beginning of buffer
      while( dwo && ( lpb[dwo] > ' ' ) )
      {
            dwo--;
            dwRet++;
//         lpd = &lpb[dwo];
      }

      if( ( dwRet ) &&
         ( dwRet < dwMaxbu ) )
      {
         if( lpb[dwo] <= ' ' )
         {
            dwo++;   // go forward OFF spacey type
         }

         // set the possible new position
         if( *pdw != dwo )
         {
            // set NEW
            *pdw = dwo;
         }
//         lpd = &lpb[dwo];
      }

   }
   return dwRet;
}

BOOL  IsBgnBr( TCHAR c )
{
   BOOL  bRet = FALSE;
   if( ( c == '+' ) ||
       ( c == '(' ) ||
       ( c == '[' ) ||
       ( c == '{' ) )
   {
      bRet = TRUE;
   }
   return bRet;
}

#define  IsNBgn(c)   ( ( c == ':' ) || ( c == '-' ) || ( c == ')' ) )
#define  IsNChr(a)   ( ( a >= '0' ) && ( a <= '9' ) )
#define  IsUChr(a)   ( ( a >= 'A' ) && ( a <= 'Z' ) )
#define  IsLChr(a)   ( ( a >= 'a' ) && ( a <= 'z' ) )

#define  IsPChr(a)   ( IsNChr(a) || IsUChr(a) || IsLChr(a) )

//#define  MXNUMSET       24
//TCHAR gcNum[MXNUMSET+8];
BOOL  ChkNum( LPTSTR lpb, DWORD dwo, DWORD dwend, LPTSTR lphd )
{
   BOOL  bRet = FALSE;
   LPTSTR   lptmp;
   DWORD    dwi, dwl;
   TCHAR    c;
   int      ii;
   int      jj, kk;
//   if( ( lpb ) &&
//      ( lpbinn ) &&
//      ( lpbgn ) )
   kk = 0;
   lptmp = lpb; 
   if(lptmp)
   {
      dwl  = dwend - dwo;  // end offset minus beginning = LENGTH
      dwi  = dwo; // keep beginning of NUMBER itself
      if( ( bAddbu ) && // config ON, and
         ( dwo ) )   // we have data to back up onto
      {
         // *****************************
         gbGotBU1 = bRet = (BOOL)chkbu( lpb, &dwo );
         // *****************************
      }

      if(dwl)
      {
         LPTSTR   lpnm = &gcNum[0];
         lptmp = &lpb[dwi];
         c = *lptmp;
         while( ( dwl ) &&
            ( c ) &&
            ( (c <= ' ') || ( IsNBgn(c) ) ) )
         {
            lptmp++;
            dwl--;
            c = *lptmp;
         }

         if( dwl )
         {
//         if( dwl < MXNUMSET )
//            lstrcpyn(lpnm, &lpb[dwi], dwl);
//         else
//            lstrcpyn(lpnm, &lpb[dwi], MXNUMSET);
            if( dwl < MXNUMSET )
            {
               lstrcpyn(lpnm, lptmp, dwl);
            }
            else
            {
               chkme( "WARNING: Could be losing NUMBERS!!!" );
               lstrcpyn(lpnm, lptmp, MXNUMSET);
            }

            grtrim(lpnm);  // remove any CONTROL ending
            jj = lstrlen(lpnm);
            if( ( gbAddComp ) &&
                ( jj        ) )
            {
               kk = 0;
               for( ii = 0; ii < jj; ii++ )
               {
                  c = lpnm[ii];
                  if( c > ' ' )
                     lpnm[kk++] = c;
               }

               // *********************
               // zero terminate buffer 
               lpnm[kk] = 0;
//               if( lstrlen(lpnm) != jj )
//                  chkme( "Good - Using compressed numering ..." );
            }

            // === ET LENGTH ===
            kk = lstrlen(lpnm);

         }

         // ==============================================
         if( kk )
            gdwFlag |= Got_Num;  // we have a NUMBER moved up or NOT

         if( dwo )
            chkcpy( lpb, &lpb[dwo] );

         lptmp = lpb;

      }

         // a non letter of the alphabet, upper or lower character
//         if( *lpbinn )
//         {
//            *lpbinn = FALSE;

//               ( dwnc >= 8 ) &&

//            if( ( *lpbgn ) &&
//               ( dwo ) )
//            {
//               LPTSTR   lptmp;
//               int   ii, jj, kk;
//               TCHAR    c;

//               lptmp = &lpb[dwo];   // get start of "number" ...
//               bRet = (BOOL)chkbu( lpb, &dwo );
//               lptmp = &lpb[dwo];   // get REAL start of "number" ...
//               chkcpy( lpb, &lpb[dwo] );
//               lptmp = lpb;

            if( !bRet )
            {
//               int   jj, kk;
//               TCHAR    c;
               // also tidy up the front of the number ...
               ii = 0;
               c = *lptmp;

               // we had NO BACKUP to get header for number
//               while( *lptmp && (*lptmp <= ' ') )
               while( ( c ) &&
                  ( (c <= ' ') || ( IsNBgn(c) ) ) )
               {
                  ii++;
                  lptmp++;
                  c = *lptmp;
               }

               jj = lstrlen(lptmp); 
               if(jj)
               {
                  for( kk = 0; kk < jj; kk++ )
                  {
                     c = *lptmp;
                     if(!c)
                        break;
                     if( IsNChr(c) )
                     {
                        // IT IS A NUMBER
                        break;   // all done
                     }
                     else if( IsBgnBr(c) )
                     {
                        // beginning brace type
                        break;
                     }
                     ii++;
                     lptmp++;
                  }
               }

               if(ii)
               {
                  //strcpy( &lpb[dwo], &lpb[ (dwo + ii) ] );
                  //chkcpy( &lpb[dwo], &lpb[ (dwo + ii) ] );
                  gbGotCU = TRUE;
                  chkcpy( lpb, &lpb[ii] );
               }


            }  // we had NO BACKUP to get header for number

//            else
//            {

//               chkme( "I have been BACKED up ..." );
//            }

//               chkcpy( lpb, &lpb[dwo] );

//               break;
//               bRet = TRUE;

//            }

//            *lpbgn = FALSE;
//         }

   }

   UNREFERENCED_PARAMETER(lphd);

   return bRet;

}

#define  MMXNUMS     64

typedef struct tagANUM {
   DWORD    dwBgn;   // begin offset of number type
   DWORD    dwNOff;  // offset of 1st REAL NUMBER
   DWORD    dwEnd;   // end offset (with tail)
   DWORD    dwBUp;   // begin offset WITH Heading
   DWORD    dwNCnt;  // count of numeral in number
   DWORD    dwTCnt;  // total character count
}ANUM;
typedef ANUM * LPANUM;

typedef struct tagNARR {
   DWORD    dwCnt;   // count of number arrays found
   ANUM     sNums[MMXNUMS];
}NARR;
typedef NARR * LPNARR;

NARR  gsNArr;

DWORD GetNums( LPTSTR lpb, DWORD dwLen, LPTSTR lphd )
{
   DWORD dwr = 0;
   DWORD dwi, dwc;
   TCHAR c;
   LPNARR   lpna = &gsNArr;
   LPANUM   lpan;
   BOOL     bin, binn, bgn, bic;
   LPTSTR   lpc;

#define  m_dwnc  lpan->dwNCnt
#define  m_dwtc  lpan->dwTCnt

#define  m_dwon  lpan->dwNOff

#define  m_dwo   lpan->dwBgn

   lpna->dwCnt = dwc = 0;
   lpan = &lpna->sNums[dwc];

   binn = bgn = bic = FALSE;

   m_dwnc = 0;
   m_dwo = m_dwon = dwi = 0;
   for( ; dwi < dwLen; dwi++ )
   {
      c = lpb[dwi];
      if( ( c > 0 ) && !IsUChr(c) && !IsLChr(c) )
      {
         bic = FALSE;
         // it is NOT an ALPHABETIC character - A-Z, a-z
         // it is NUMBER LIKE
         // ############################################
         bin = IsNChr(c);
         // or at least numeric type ...
         if( binn )
         {
            m_dwtc++;
         }
         else
         {
            // we are beginning a number like run ...
            m_dwo = dwi;  // keep this beginning
            binn = TRUE;
            m_dwtc = 1;    // total is 1 char
         }
         if( bin )
         {
            if( !bgn )
            {
               m_dwon = dwi;
               bgn = TRUE;
               m_dwnc = 1;
            }
            else
            {
               m_dwnc++;
            }
         }
      }
      else
      {
         // an UPPER or lower case character
         // CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
         // Check if have Num
         if( !bic )
         {

//         if( binn && bgn && (dwnc >= 8) && dwo )
            if( binn && bgn && m_dwnc )
            {
               //bDnT1 = TRUE;
               if( dwc < MMXNUMS )
               {
                  lpc = &lpb[lpan->dwNOff];
                  lpan->dwEnd = dwi;   // set END of number
                  dwr++;

                  // move on to next buffer for next number array
                  dwc++;
                  //lpan = &lpna->sNums[dwc];
                  // ============================================
                  // OR
                  lpan++;
                  // ============================================
               }
               else
               {
                  if( !gbDnCompile )
                  {
                     chkme( "CHECK: There are MORE numbers than in 64 array size !!! " );
//                     break;
                     gbDnCompile = TRUE;
                  }
               }
            }


            binn = bgn = FALSE;
            m_dwtc = m_dwnc = 0;
            m_dwo  = m_dwon = dwi;

            bic = TRUE;

         }
      }

   }


// {
      // out of character loop
//    {
         if( !bic )
         {

            if( binn && bgn && m_dwnc )
            {
               // maybe a final number
               //bDnT1 = TRUE;
               if( dwc < MMXNUMS )
               {
                  lpc = &lpb[lpan->dwNOff];
                  dwc++;
                  dwr++;
                  lpan->dwEnd = dwi;   // set END of number

//                  lpan = &lpna->sNums[dwc];
                  lpna++;
                  m_dwtc = m_dwnc = 0;
                  m_dwo  = m_dwon = dwi;

               }
               else
               {
                  chkme( "CHECK: There are MORE number than in array size !!! " );
//                break;
               }


            }


//            binn = bgn = FALSE;
//            m_dwtc = m_dwnc = 0;
//            m_dwo  = m_dwon = dwi;
//            bic = TRUE;
         }
//    }
// }
   // very END OF STRING
   lpan->dwEnd = dwi;   // set END of number
   lpc = &lpb[(dwi - 3)];
   UNREFERENCED_PARAMETER(lphd);

   return dwr;
}

DWORD Move2Nums( LPTSTR lpIn, DWORD dwIn, LPTSTR lphd )
{
   DWORD dwr = 0;
   DWORD dwi, dwj, dwc, dwo, dwon, dwnc;
   DWORD dwLen = dwIn;
   TCHAR    c, d;
   BOOL     bin, binn, bgn, bic;
   LPTSTR   lpb = lpIn;
   LPTSTR   lpc;
   BOOL     bDnT1 = FALSE;
   DWORD    dwa, dwm, dwe;
   LPNARR   lpna; // = &gsNArr;
   LPANUM   lpan; // = &lpna->sNums[0];

   // ============================
   // Remove any Comma plus space(s)
   // ============================
   if( ( lpb[0] == ',' ) &&
      ( dwLen ) )
   {
      dwLen--;
      lpb++;
      while( *lpb && ( *lpb <= ' ' ) && ( dwLen ) )
      {
         dwLen--;
         lpb++;
      }
   }

   dwj = lstrlen(lpb);
   dwa = GetNums( lpb, dwj, lphd );

   lpna = &gsNArr;
   lpan = &lpna->sNums[0];
   dwo  = lpan->dwBgn;   // dwo,
   dwe  = lpan->dwEnd;
   dwon = lpan->dwNOff;
   dwc  = lpan->dwTCnt;
   dwnc = lpan->dwNCnt;
   if( ( bUseArray ) &&
      ( dwa ) )
   {
      // check the array we have
//      LPNARR   lpna = &gsNArr;
//      LPANUM   lpan = &lpna->sNums[0];
      DWORD    dwcc;

      if( dwa > MMXNUMS )
         dwa = MMXNUMS;

//      lpna = &gsNArr;
//      lpan = &lpna->sNums[0];
      for( dwcc = 0; dwcc < dwa; dwcc++ )
      {
         //lpan = &lpna->sNums[dwcc];
//         if( lpan->dwNCnt >= 8 )
         dwnc = lpan->dwNCnt;
         dwo  = lpan->dwBgn;   // dwo,
         dwe  = lpan->dwEnd;
         if( dwnc >= 8 )
         {
            dwr = (DWORD)ChkNum( lpb, // and not dwnc, // lpan->dwNCnt,   // dwnc,
               dwo,  // lpan->dwBgn,   // dwo,
               dwe,
               lphd );
            if( dwr )
            {
               dwnc = 0;
               bDnT1 = TRUE;
               return dwr;
            }
            break;
         }
         lpan++;

      }

      lpan = &lpna->sNums[0];
      for( dwcc = 0; dwcc < dwa; dwcc++ )
      {
         //lpan = &lpna->sNums[dwcc];
//         if( lpan->dwNCnt >= 7 )
         dwnc = lpan->dwNCnt;
         dwo  = lpan->dwBgn;   // dwo,
         dwe  = lpan->dwEnd;
         if( dwnc >= 7 )
         {
            dwr = (DWORD)ChkNum( lpb, // and not dwnc, // lpan->dwNCnt,   // dwnc,
               dwo,  // lpan->dwBgn,   // dwo,
               dwe,
               lphd );
            if(dwr)
            {
               dwnc = 0;
               bDnT1 = TRUE;
               return dwr;
            }
            break;
         }

         lpan++;

      }

      lpan = &lpna->sNums[0];

      return dwr;

   }
//   if( ( lphd[0] == 'S' ) &&
//      ( lphd[1] == 'i' ) &&
//      ( lphd[2] == 'm' ) )
//   {
//      chkme( "Is this Simone .... !!!" );
//   }
   d = 0;
   bic = bgn = binn = FALSE;
   dwnc = 0;
   dwm = 0;
   for( dwi = 0; dwi < dwj; dwi++ )
   {
      //c = lpb[dwi];
      lpc = &lpb[dwi];
      c = *lpc;
      if( ( c > 0 ) && !IsUChr(c) && !IsLChr(c) )
      {
         bic = FALSE;
         // it is NOT an ALPHABETIC character - A-Z, a-z
         // it is NUMBER LIKE
         // ############################################
         bin = IsNChr(c);
         // or at least numeric type ...
         if( bin )
         {
            if( !bgn )
            {
               dwon = dwi;
               bgn = TRUE;
               dwnc = 1;
               if( !binn )
               {
                  // we are beginning a number run ...
                  dwo = dwi;  // keep this beginning
                  binn = TRUE;
                  dwc = 0;
               }
            }
            else
            {
               dwnc++;
            }
         }

         if( binn )
         {
            dwc++;
         }
         else
         {
            // we are beginning a number run ...
            dwo = dwi;  // keep this beginning
            binn = TRUE;
            dwc = 1;
         }
      }
      else
      {
         if( !bic )
         {
            if( bgn && dwnc )
            {
//   DWORD    dwBgn;   // begin offset of number type
//   DWORD    dwNOff;  // offset of 1st REAL NUMBER
//   DWORD    dwEnd;   // end offset (with tail)
//   DWORD    dwBUp;   // begin offset WITH Heading
//   DWORD    dwNCnt;  // count of numeral in number
//   DWORD    dwTCnt;  // total character count
               lpan->dwBgn  = dwo;
               lpan->dwNOff = dwon;
               lpan->dwEnd  = dwi;
               lpan->dwTCnt = dwc;
               lpan->dwNCnt = dwnc;
               if( dwm < (MMXNUMS - 1) )
               {
                  dwm++;
                  if( binn && bgn && (dwnc >= 8) )
                  {
                     // we are exiting the LOOP on this
                  }
                  else
                  {
                     // bump the pointer
                     lpan++;
                  }
               }
            }
         // an UPPER or lower case character
         // CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
         //ChkNum( LPTSTR lpb, LPBOOL lpbinn, LPBOOL lpbgn, DWORD dwnc, DWORD dwo )
//         if( binn && bgn && (dwnc >= 8) && dwo )
            if( binn && bgn && (dwnc >= 8) )
            {
               //ChkNum( lpb, &binn, &bgn, dwnc, dwo );
               //dwnc = 0;
               //bDnT1 = TRUE;
               break;
            }

            bic = TRUE;
         }

         binn = bgn = FALSE;
         dwc = dwnc = 0;
         dwo  = dwi;

      }
   }

   // out of the for
   //ChkNum( lpb, &binn, &bgn, dwnc, dwo );
//   if( binn && bgn && (dwnc >= 8) && dwo )
   dwe  = dwi;
   if( binn && bgn && (dwnc >= 8) )
   {
//      dwr = (DWORD)ChkNum( lpb, &binn, &bgn, dwnc, dwo );
//    dwr = (DWORD)ChkNum( lpb, dwnc, dwo );
//      dwr = (DWORD)ChkNum( lpb, dwnc, dwo, lphd );
      dwr = (DWORD)ChkNum( lpb,
         dwo,  // begin
         dwe,  // lpan->dwEnd,
         lphd );
            dwnc = 0;
            bDnT1 = TRUE;
   }
   binn = bgn = FALSE;

   if( !bDnT1 )
   {
      binn = bgn = FALSE;
      dwc = dwnc = 0;
      dwo  = dwi;
      for( dwi = 0; dwi < dwj; dwi++ )
      {
         //c = lpb[dwi];
         lpc = &lpb[dwi];
         c = *lpc;
         if( ( c > 0 ) && !IsUChr(c) && !IsLChr(c) )
         {
            // it is NUMBER
            bin = IsNChr(c);
            // or at least numeric type ...
            if( bin )
            {
               if( !bgn )
               {
                  dwon = dwi;
                  bgn = TRUE;
                  dwnc = 1;
                  if( !binn )
                  {
                     // we are beginning a number run ...
                     dwo = dwi;  // keep this beginning
                     binn = TRUE;
                     dwc = 0;
                  }
               }
               else
               {
                  dwnc++;
               }
            }
            if( binn )
            {
               dwc++;
            }
            else
            {
               // we are beginning a number run ...
               dwo = dwi;  // keep this beginning
               binn = TRUE;
               dwc = 1;
            }
         }
         else
         {
            // a character
            //ChkNum( LPTSTR lpb, LPBOOL lpbinn, LPBOOL lpbgn, DWORD dwnc, DWORD dwo )
//            if( binn && bgn && (dwnc >= 7) && dwo )
            if( binn && bgn && (dwnc >= 7) )
            {
               //ChkNum( lpb, &binn, &bgn, dwnc, dwo );
               //dwnc = 0;
               //bDnT1 = TRUE;
               // quest has ended
               dwe = dwi;
               break;
            }

            binn = bgn = FALSE;
            dwc = dwnc = 0;
            dwo  = dwi;
         }
      }

      // out of for

//      if( binn && bgn && (dwnc >= 7) && dwo )
      if( binn && bgn && (dwnc >= 7) )
      {

//      dwr = (DWORD)ChkNum( lpb, &binn, &bgn, dwnc, dwo );
//      dwr = (DWORD)ChkNum( lpb, dwnc, dwo );
//         ChkNum( lpb, &binn, &bgn, dwnc, dwo );
//      dwr = (DWORD)ChkNum( lpb, dwnc, dwo, lphd );
//      dwr = (DWORD)ChkNum( lpb, dwo, lphd );
      dwr = (DWORD)ChkNum( lpb,
         dwo,
         dwe,
         lphd );
         dwnc = 0;
         bDnT1 = TRUE;
      }

      binn = bgn = FALSE;

   }  // 2nd try for a 7 digit number

   return dwr;

}

LPTSTR   glpSalutes[] = {
   { "MR" },
   { "MRS" },
   { "MS" },
   { "MISS" },
   { 0 }
};
#define  MMAXSAL      4
#define  MMINSAL      2

//#define  MMXEND      128
// this seems good using excel to print the results.
// =================  78
#define  MMXEND      128 - 50

DWORD dwMaxLen = MMXEND;

int   IsSal( LPTSTR lpn )
{
   int   iRet = 0;      // assume NONE
   DWORD dwk = 0;
   LPTSTR   lps;

   DWORD    dwl = 0;
   LPTSTR   * plps = &glpSalutes[0];
//   LPTSTR   lps;
   TCHAR    c;
//   int      id;
   TCHAR    cUBuf[16+4];
   LPTSTR   lpt = &cUBuf[0];
//   DWORD    dwsal;
   DWORD    dwi;
   DWORD    dwo;
   DWORD    dwnc, dwuc, dwnlc;    // number and upper counts (excl ()[].)

   dwi = dwl = 0;
   lpt[dwi] = 0;
   dwuc = 0;
   if( lpn )
      dwl = lstrlen(lpn);
   if(dwl)    // very min of cap "M" = 1
   {
      if( dwl < 16 )
         dwo = dwl;
      else
         dwo = 15;

      // copy first up to FIFTEEN characters
      dwnlc = dwnc = dwuc = 0;  // start counters
      for( ; dwi < dwo; dwi++ )
      {
         //c = toupper(lpn[dwi]);
         c = lpn[dwi];
         if( c <= ' ' )
            break;
         // into lpt, the uppercased buffer of the header
         // ===============================================================
         if( ( c != '(' ) && ( c != ')' ) &&
            ( c != '[' ) && ( c != ']' ) &&
            ( c != '.' ) )
         {
            // ++++++++++++++++++++++
            // add it to our buffer
            lpt[dwk++] = (char)toupper(c);  // toupper(lpn[dwi]);
            // keep some counters
            if( IsNChr(c) )
            {
               dwnc++;
            }
            else if( IsUChr(c) )
            {
               dwuc++;
            }
            else if( !IsLChr(c) )
            {
               dwnlc++;
            }
            // ++++++++++++++++++++++
         }

         // =============================================
      }

      //lpt[dwi] = 0;
      lpt[dwk] = 0;  // zero terminate what is there

      if( ( gbIgnoreMNU ) &&
         ( dwnc         ) &&
         ( ( dwnc + dwuc + dwnlc ) == dwk ) )
      {
         iRet--;
         //return iRet;
         goto DoneSal;
      }

      if( ( dwk < MMINSAL ) ||
         ( dwk > MMAXSAL ) )
      {
         // out of range of interest
         goto DoneSal;
      }

//      iRet += ChkRSal( lpn, plps, lpt, dwi );
      if( ( plps ) &&
         ( *plps ) )
      {
//         while( lps = *plps++ )
         lps = *plps; 
         while(lps)
         {
            dwo = strlen(lps);
            if( dwo == dwk )
            {
               // the BIG compare
               if( lstrcmp(lps,lpt) == 0 )
               {
                  iRet++;
                  break;
               }
            }
            plps++;
            lps = *plps; 
         }
      }
      if( !iRet )
      {
         // is NOT a SAL
         if( (lpt[0] == 'L' ) &&
            (lpt[1] == 'T' ) &&
            (lpt[2] == 'D' ) )
         {
            if(dwk >= 3)
               iRet++;
         }
      }
   }

DoneSal:

   if( !iRet )
   {
      if( ( dwk == 1 ) &&
         ( dwuc == 1 ) )
      {
         // single capital letter is NOT used as a name alternative
         iRet -= 2;
      }
   }

   return iRet;

}

int   ChkRSal( LPTSTR lpn, LPTSTR * plps, LPTSTR lpt, DWORD dwsal )
{
   int   iRet = 0;
   LPTSTR   lps;
   DWORD    dwo;
   TCHAR    c;
   int      id;
   DWORD    dwi, dwl;

   if( plps && dwsal )
   {
         // extract the salutations
         lps = *plps;
         while( lps )
         {
            dwo = strlen(lps);
            if( dwo == dwsal )
            {
               LPTSTR   lpnn = &lpn[dwo];
               // build its length in the uppercased buffer
               c = lpt[dwo];
               //lpt[dwo] = 0;
               // the BIG compare
               id = lstrcmp(lps,lpt);
               //lpt[dwo] = c;
               if( id == 0 )
               {
                  // it starts with this
                  // LPTSTR   lpnn = &lpn[dwo];

                  // move up to the NAME
                  while( *lpnn && ( *lpnn <= ' ' ) )
                  {
                     lpnn++;
                  }

                  // bring the NAME to the front
                  strcpy(lpn,lpnn);
                  dwl = lstrlen(lpn);

                  // clean tail of name of spacey stuff
                  while( dwl && ( lpn[ (dwl - 1) ] <= ' ' ) )
                  {
                     dwl--;
                  }

                  if( dwl < (DWORD)lstrlen(lpn) )
                  {
                     lpn[dwl] = 0;
                     strcat(lpn," ");
                  }

                  // ensure a SPACE after name(s)
                  dwl = lstrlen(lpn);
                  if( dwl )
                  {
                     c = lpn[ (dwl - 1) ];
                     if( c > ' ' )
                     {
                        strcat(lpn," ");
                     }
                  }

                  // and copy the salutation
                  for( dwi = 0; dwi < dwo; dwi++ )
                  {
                     if(dwi)
                        lpt[dwi] = (char)tolower(lps[dwi]);
                     else
                        lpt[dwi] = lps[dwi]; // first in uppercase

                  }

                  // zero terminate the string
                  lpt[dwi] = 0;

//                  strcat(lpn,lps);
                  strcat(lpn,lpt);

                  iRet ++;    // send a POSITIVE signal back

               }
            }
            plps++;
            lps = *plps;
         }
   }
   return iRet;
}


/* ==========================================
   int   Check4Ms( LPTSTR lpn )

   Purpose:

      1. If string begins with a salutation
      given in the compiled list, then put it
      to the end. ie Bring NAME to front.

      2. If a String begins with initial,
      put the NAME to the front, then add
      the intitials after a space ...
   ========================================== */
#define  Load(a)\
   if(*lpnn)\
   {\
      if(dwi)\
         dwi--;\
      a = *lpnn++;\
   }



int   ChkLtrs( LPTSTR lpn, DWORD dwl, LPTSTR lpt )
{
   int   iRet = 0;
   TCHAR c;
   DWORD dwi;
   
            int   is = 0;
            LPTSTR   lpnn = lpn;
            c = *lpnn++;
            dwi = dwl;
            if( IsUChr(c) )
            {
               // we could have M H Sin, for example
               is++;
               Load(c);
               if( c == '.' )
                  Load(c);
               if( c == ' ' )
               {
NxtLett:
                  while( dwi && ( c == ' ' ) )
                     Load(c);
                  if( dwi && IsUChr(c) )
                  {
                     Load(c);
                     if( c == '.' )
                        Load(c);
                     if( c == ' ' )
                     {
                        is++; // we appear to have a second letter
                        goto NxtLett;
                     }
                     else
                     {
                        // we are into a word following a letter or maybe L.
                        // which should be moved to the FRONT, and the letters
                        // will follow the name ... so
                        LPTSTR   lpc;
                        DWORD    dwo;
                        dwi++;   // remove 2nd character count to get here
                        dwo = dwl - dwi;
                        lpc = &lpn[dwo];
                        lstrcpyn(lpt,lpn,dwo);  // keep the letters here
                        grtrim(lpt);
                        chkcpy(lpn, &lpn[dwo] );
                        gaddsp(lpn);
                        strcat(lpn,lpt);
                        iRet++;

                     }
                  }
               }
            }

   return iRet;
}


int   Check4Ms( LPTSTR lpn )
{

   int   iRet = 0;      // assume NONE

   DWORD    dwl = 0;
   LPTSTR   * plps = &glpSalutes[0];
//   LPTSTR   lps;
   TCHAR    c;
//   int      id;
   TCHAR    cUBuf[16+4];
   LPTSTR   lpt = &cUBuf[0];
//   DWORD    dwsal;
   DWORD    dwi;
   DWORD    dwo;

   dwi = dwl = 0;
   lpt[dwi] = 0;
   if( lpn )
      dwl = lstrlen(lpn);
   if( dwl >= 3 ) // very min of "A Z" = 3
   {
      // get a global flag
      //LPMBLST  lpgmb = &gsMBLst;

      if( dwl < 16 )
         dwo = dwl;
      else
         dwo = 15;
      // copy first up to FIFTEEN characters
      for( ; dwi < dwo; dwi++ )
      {
         c = (char)toupper(lpn[dwi]);
         if( c <= ' ' )
            break;
         // into lpt, the uppercased buffer of the header
         lpt[dwi] = c;  // toupper(lpn[dwi]);
         // =============================================
      }

      lpt[dwi] = 0;  // zero terminate what is there

 //     if( dwl >= 4 ) )  // very minim of say "ms a" = 4 chars
         // **************************************************
#ifdef   CHKMSTANG

      if( ginstr(lpn, "Ms Tang") == 1 )
         chkme( "Doing the tale of the Ms salut..." );
#endif   /* CHKMSTANG */

      if( ( dwi < MMINSAL ) ||
         ( dwi > MMAXSAL ) )
      {
         // out of range of interest
         goto DoneSal;
      }

      iRet += ChkRSal( lpn, plps, lpt, dwi );

DoneSal:

      // another thing the check for now that the salutation has been move
      // to after the name, we hope ....
      // Does it begin with initials
//      ( dwl = lstrlen(lpn) ) &&
//      ( dwl >= 3 ) )    // very min of "A Z" = 3
      gdwTitCnt = GetWdCnt(lpn); 
      if(gdwTitCnt)
      {
         if( gdwTitCnt >= 2 )
         {

            iRet += ChkLtrs( lpn, dwl, lpt );

         } // we have at least two word
      }

   }

   return iRet;
}

// ***** IMPORTED *****

int   ginstri( LPTSTR lps, LPTSTR lpi )
{
   int   iRet = 0;
   DWORD   i, j, k, l;
   LPTSTR   lpc;
   INT     c, cb;

   i = j = 0;
   cb = 0;
   if( lps && lpi )
   {
      i = strlen(lps);
      j = strlen(lpi);
      cb = toupper(*lpi);
   }
   if( i && j && ( j <= i ) && cb )
   {
      if( j == i )
      {
         if( lstrcmpi(lps,lpi) == 0 )
            iRet++;  // it starts at POSITION 1
      }
      else
      {
         l = i - j;
         for( k = 0; k < l; k++ )
         {
            lpc = &lps[k];
//            if( *lpi == lps[k] )
            c = toupper(*lpc);
//            if( *lpi == *lpc )
            if( c == cb )
            {
               if( j == 1 )
               {
                  iRet = k + 1;
                  return iRet;
               }
               else
               {
                  DWORD jj;
                  for( jj = 1; jj < j; jj++ )
                  {
//                     lpc = &lps[k+jj];
                     if( toupper(lpi[jj]) != toupper( lps[ k + jj ] ) )
                        break;
                  }
                  if( jj == j )
                  {
                     iRet = k + 1;
                     return iRet;
                  }
               }
            }
         }
      }
   }
   return iRet;
}

int   ginstriw( LPTSTR lps, LPTSTR lpi )
{
   int   iRet = ginstri(lps,lpi);
   if( iRet )
   {
      DWORD   i, j, l;
//      LPTSTR   lpc;
      INT     c, cb;
      i = j = 0;
      cb = 0;
      if( lps && lpi )
      {
         i = strlen(lps);
         j = strlen(lpi);
         cb = toupper(*lpi);
      }
      if( i && j && ( j <= i ) && cb )
      {
         if( j == i )
         {
            //if( lstrcmpi(lps,lpi) == 0 )
            //   iRet++;  // it starts at POSITION 1
         }
         else
         {
            l = i - j;
            if( iRet == 1 )
            {
               // done left
            }
            else
            {
               l = iRet - 2;
               c = lps[l];
               if( (c < 0) || IsPChr(c) )
               {
                  // this is BAD
                  iRet = 0;
               }
            }
            if( iRet )
            {
               l = ( (iRet - 1) + j );
               c = lps[l];
               if( (c < 0) || IsPChr(c) )
               {
                  // this is BAD
                  iRet = 0;
               }
            }
         }  
      } 
   }
   return iRet;
}


// ********************

/* =========================================================


   ========================================================= */
int   ChkXchg( LPTSTR lptmp, LPTSTR lps )
{
   int   iRet = 0;
   int   i = 0;
   if( ( lptmp ) &&
      ( lps ) )
      i = lstrlen(lps);
   if( i > 1 )
   {
      DWORD dwo;
//      ( i > 2 ) )
      //LPMBLST  lpgmb = &gsMBLst;
//               if( dwo = ginstriw(lptmp,"AND") )
      dwo = ginstriw(lptmp,lps); 
      if(dwo)
      {
         dwo--;
         if(dwo)
         {
            lptmp[dwo] = '&';
            dwo++;
                     //strcpy( &lptmp[dwo], &lptmp[ ( dwo + 2 ) ] );
//            chkcpy( &lptmp[dwo], &lptmp[ ( dwo + 2 ) ] );
            iRet = (i - 1);
//            chkcpy( &lptmp[dwo], &lptmp[ ( dwo + (i - 1)) ] );
            chkcpy( &lptmp[dwo], &lptmp[ ( dwo + iRet) ] );
//                     dwc++;
//#define  Had_Xchg 0x00001000  // exchanged "AND" for "&"
            gdwFlag |= Had_Xchg;
                  
         }
               
      }

   }
   return iRet;
}

DWORD GetWdCnt( LPTSTR lptmp )
{
   DWORD dwc = 0;
   DWORD i, k;
   TCHAR c;

   c = 0;
   i = 0;
   if( lptmp )
   {
      c = *lptmp;
      i = lstrlen(lptmp);
   }
   if( c )
   {
//      if( ( c >= 'A' ) && ( c <= 'Z' ) )
      if( IsPChr( c ) )
      {
         // begins with alpha-numeric
         k = 1;
      }
      else
      {
         k = 0;
      }

      while(i--)
      {
         if( k )
         {
            if( ( c > 0 ) && ( c <= ' ' ) )
            {
               // end of word
               dwc++;
               k = 0;
            }
         }
         else
         {
            // waiting in spacey
            if( ( IsPChr(c) ) ||
               ( c == '-' ) )
            {
               k = 1;
            }
         }

         if(c)
         {
            lptmp++;
            c = *lptmp;
         }
         else
         {
            break;
         }
      }

      // we had at least ONE alphanumeric character
      // ==========================================
      if( k )
      {
         // then block termination is also an END OF WORD, so
         dwc++;
      }
   }
   return dwc;
}

DWORD FixInfo( LPTSTR lpn, LPTSTR lpb, DWORD dwoff, DWORD dwLen )
{
   DWORD dwRet = 0;
#ifdef _WIN32
   UNREFERENCED_PARAMETER(lpn);
   UNREFERENCED_PARAMETER(lpb);
   UNREFERENCED_PARAMETER(dwoff);
   UNREFERENCED_PARAMETER(dwLen);
#endif
   return dwRet;
}


/* ========================================================================
   DWORD CheckBlock( LPTSTR lpIn, DWORD dwIn, LPDWORD pdwFlag, DWORD dwLines )

   ======================================================================== */
DWORD CheckBlock( LPTSTR lpIn, DWORD dwIn, LPDWORD pdw, DWORD dwLines )
{
   //LPMBLST  lpgmb = &gsMBLst;
   DWORD  dwRet = 0;
   LPTSTR   lpb = lpIn;
   DWORD  dwLen = dwIn;
   DWORD dwFlag = *pdw;
   int      i;
   TCHAR    c;

   // CLEAN FRONT of "spacey" stuff
   i = 0;
   while( (*lpb) && ( *lpb <= ' ' ) )
   {
      i++;
      lpb++;
   }
   if( i )
   {
      // PERMANENT FIX
      //strcpy(lpIn, &lpIn[i]);
      strcpy(lpIn, lpb);
      lpb = lpIn;
      dwLen = strlen(lpb);
   }

   if( ( lpb ) && ( dwLen ) && ( dwFlag ) )
   {
      if( dwFlag & Had_Coma )
      {
         DWORD dwoff = ginstr(lpb,",");
//         if( ( dwoff ) &&
//            ( dwoff < NMMX2C ) )
         if( dwoff )
         {
            // we have the first COMMA
//            static TCHAR s_cHead[ (NMMX2C + 15) ];
//            LPTSTR   lptmp = &s_cHead[0];
            LPTSTR   lpn = &gcHead1[0];
            LPTSTR   lps1;
            DWORD dwo, dwt, dwc, dwe;

            dwc = 0;
            if( dwoff < NMMX2C )
            {
               // copy the head into a buffer
               dwoff--;    // skip the comma
               c = lpb[dwoff];
               lpb[dwoff] = 0;
//               strcpy(s_cHead,lpb);
//               strcpy(lptmp,lpb);
               strcpy(lpn,lpb);
               lpb[dwoff] = c;
               dwoff++; // back to position of comma + 1

               if( Check4Ms(lpn) )
               {
                  dwc++;
               }

               lps1 = "AND";
               if( ChkXchg(lpn,lps1) )
               {
                     dwc++;
               }
#ifdef   CHKANNICK

               if( ginstr(lpn,"Annick ") == 1 )
                  chkme("WARNING: This one appears to miss !!! Annick!");
#endif   // CHKANNICK

               lps1 = "ET";
               if( ChkXchg(lpn,lps1) )
               {
                     dwc++;
               }

               gdwWdCnt = GetWdCnt(lpn);

               if( gdwWdCnt >= 3 )
               {
                  DWORD _dl, _ds, _da;
                  if( ( ( _dl = lstrlen(lpn)) > 0 ) &&
                     ( _dl < NMMX2C           ) &&
                     ( ( _ds = ginstr(lpn," ") ) > 0 ) &&
                     ( _ds > 1                 ) &&
                     ( ( _da = ginstr(lpn,"&") ) > 0 ) &&
                     ( _da > _ds               ) )
                  {
                     DWORD _ds2, _ds3;
                     if( (_ds2 = ginstr( &lpn[_ds]," ")) > 0 )
                     {
                        LPTSTR lph2 = &gcHead2[0]; // [ (NMMX2C + 15) ];
                        strcpy(lph2,lpn); // make complete COPY
                        if( (_ds3 = ginstr( &lpn[ (_ds + _ds2 ) ]," " )) > 0 )
                        {
                           // we have found a 3rd space, so there is MORE
//                           _ds3 = (_ds + _ds2 + 1);
                           _ds3 += (_ds + _ds2);
//                           chkcpy( &lptmp[(_ds-1)], &lptmp[ _ds3 ] );
                           chkcpy( &lpn[_ds], &lpn[ _ds3 ] );
                        }
                        else
                        {
                           // there is NO MORE, so
                           lpn[ (_ds - 1) ] = 0; // zero terminate the entry
                        }
//                        chkcpy( &lptmp[ (_ds - 1) ], &lptmp[ (_ds + _ds2 + 1) ] );
//                        chkcpy( &lptmp[ (_ds - 1) ], &lptmp[ _ds3 ] );
                        dwc++;
                        // set second subject first
                        chkcpy(lph2,&lph2[ (_ds + _ds2) ] );

                        // *** SET THE FLAG ***
                        gdwFlag |= Got_TWO;
                        // ********************

                     }
                  }
               }

               // and FINAL ENDING LAST POSSIBLE ADJUSTMENT TO THE HEADER
               // =======================================================
               if( ( dwo = strlen(lpn) ) > 0 )
               {
                  // if LESS than usual name size
                  if( dwo < NMLEN )
                  {
                     if( bDoFill2 )
                     {
                        while( dwo < NMLEN )
                        {
                           strcat(lpn," ");
                           dwo = strlen(lpn);
                        }
                        dwc++;
                     }
                     else if( ( bAddSpPad ) &&
                        ( lpn[ (dwo - 1) ] > ' ' ) )
                     {
                           strcat(lpn," ");
                           dwo = strlen(lpn);
                        dwc++;
                     }
                  }

                  // if MORE than usual name size
                  if( dwo > NMLEN )
                  {
                     dwc++;
                     lpn[NMLEN] = 0;
                  }
               }
            }

            dwLen = strlen(lpb);
//            if(dwc)
            if(dwLen)
            {
               LPTSTR   lpend;
               //if( strlen(lpb) != dwLen )
               //{
               //   chkme( "WHAT! Length is NOT correct? %d vs %d !!!"MEOR,
               //      dwLen,
               //      strlen(lpb) );
               //   dwLen = strlen(lpb);
               //}
               //if( ( lpend = dMALLOC(dwLen) ) != 0 )
               if( dwLen < 1024 )
                  lpend = dMALLOC(1024 + 256);
               else
                  lpend = dMALLOC(dwLen + 256);

               if( lpend != 0 )
               {
                  // ===========================================
                  DWORD _dwo = dwoff;
//                  FixInfo( lptmp, lpb, dwoff, dwLen );
                  while( lpb[_dwo] && ( lpb[_dwo] <= ' ' ) )
                  {
                     _dwo++;
                  }
                  //strcpy(lpend, &lpb[dwoff]);
                  strcpy(lpend, &lpb[_dwo]);
                  dwe = strlen(lpend);

                  // put the FIXED header
                  strcpy(lpb,lpn);
                  // then put back the first comma
                  strcat(lpb,", ");

#ifdef   CHKROWINA

               if( ginstr(lpn,"Rowina D") == 1 )
                  chkme( "CHECK: Rowina LOSES her PLUS sign! WHY !!!" );
#endif   // CHKROWINA

#ifdef   CHKGEORGE

               if( ginstr(lpn,"George G") == 1 )
                  chkme( "CHECK: No lpnm (tele. num) set up for George!!!" );

#endif   // CHKGEORGE

//                  if( Move2Nums(lpend,dwLen,lptmp) == 0 )
                  Move2Nums(lpend,dwLen,lpn);
                  {
                     if( bAddthdr )
                     {
//                  strcat(lpb,", tel: ");
                     strcat(lpb,"t:");
                     }
                  }

                  if( bDoMaxLen )
                  {
                     dwe = lstrlen(lpend);
                     if( dwe > dwMaxLen )
                     {
                        // this is a long line
                        dwe--;   // back to last char
                        dwt = dwMaxLen;   // get the max. len
                        while( lpend[dwt] > ' ' )
                        {
                           dwt++;
                           if(dwt >= dwe )
                              break;
                        }
                        lpend[dwt] = 0;   // terminate it
                        if( (dwt < dwe) && (lpend[dwe] < ' ') )
                        {
                           strcat(lpend,MEOR); // = "\r\n");
                        }
                     }
                  }

                  dwe = lstrlen(lpend);
                  if( bNo2Commas )
                  {
                     for( dwt = 0; dwt < dwe; dwt++ )
                     {
                        if( lpend[dwt] == ',' )
                           lpend[dwt] = ' ';
                     }
                  }

                  strcat(lpb,lpend);

                  // ===========================================
                  dMFREE(lpend);
                  dwoff = ginstr(lpb,",");   // get NEW position
               }
               else
               {
                  chkme( "C:ERROR: Memory FAILED !!! " );
               }
            }

            dwt = 0;
            lps1 = lpb;
            while( ( dwo = ginstr(lps1," ") ) != 0 )
            {
               if( ( dwt + dwo ) > dwoff )
               {
                  dwt = 0;
                  break;
               }
               dwt += dwo;
               if( dwt > NMLEN )
                  break;
               lps1 = &lps1[dwo];
            }

            if( dwt )
            {
//               strcpy( &lpb[dwt], &lpb[dwoff] );
               chkcpy( &lpb[dwt], &lpb[dwoff] );
               dwRet = lstrlen(lpb);

            }
         }
      }
      else
      {
         chkme( "YEEK, a non comma entry ... " );
      }
   }
   else
   {
      chkme( "No FLAG! What does it say ...???" );
   }

   lpb = lpIn;
   dwLen = dwIn;
   UNREFERENCED_PARAMETER(dwLines);

   return dwRet;
}


BOOL  bGotSigChrs( LPTSTR lpout )
{
   BOOL  bRet = FALSE;
   int   i, j;
   TCHAR    c;

   if( ( lpout ) &&
      ( ( j = lstrlen(lpout) ) > 0 ) )
   {
      for( i = 0; i < j; i++ )
      {
         c = lpout[i];
         if( ( c < 0 ) ||
            ( c > ' ' ) )
         {
            bRet = TRUE;
            break;
         }
      }
   }

   return bRet;
}

/* ==============================================================================
   void  OutBlock( LPTSTR lpb, DWORD dwLen, DWORD dwBlkNum )

   ============================================================================== */
//MBLST    gsMBLst = {
//   { &gsMBLst.m_sFBLinks, &gsMBLst.m_sFBLinks }, 0 };
//#define     gdwFlag      lpgmb->m_dwFlag
//               else if( c == 'X' )
//               {
//                  cp++;
//               }
LPTSTR   glpSps[] = {
   { " " },
   { "," },
   { ":" },
   { "." },
   { "@" },
   { 0 }
};

int   grtrimall( LPTSTR lps )
{
   int   iRet = 0;
   int   i = lstrlen(lps);
   TCHAR    c;
   while( i > 0 )
   {
      i--;
      c = lps[i];
      if( IsPChr(c) )   //   ( IsNChr(a) || IsUChr(a) || IsLChr(a) )
         break;
      lps[i] = 0;
      iRet++;
   }
   return iRet;
}

/* ==================================================================
 * int   GetXHdrs( LPTSTR lpn, DWORD dwi )
 *
 * PURPOSE: If -gX[1] switch, then try to split up the HEADER
 *       into "names" for extra ALPHABETIC entries per person
 *
 */
int   GetXHdrs( LPTSTR lpn, DWORD dwi )
{
   int   iRet = 0;
   LPTSTR   lpn2 = &gcHeadX1[0];
   LPTSTR   lpn3 = &gcHeadX2[0];
            // now if we want multiple entries per entry then
            // ==============================================
            if( gbDoExtra )
            {
               DWORD dwc;
//               GetXHdrs(lpn,dwi);
#ifdef   CHKYVAN
               if( ginstr(lpn,"Yvan ") == 1 )
                  chkme( "Why is this NOT split into 2, or is it ...???" );
#endif   // CHKYVAN
               if( ( dwc = GetWdCnt(lpn) ) > 1 )
               {
                  // rotate the LAST word to the FRONT then follow with rest
                  // Annie's birthday, thus must shower and off to lunch
                  // avec la mere (  this is her mother / c'est la m�re de Annie )
                  DWORD dwj, dwk;
                  LPTSTR   lps = lpn;  // only now interested in our NAME buffer
                  dwk = 0; // extracted from the FILE buffer given ...
                  while( ( dwj = ginstr(lps," ") ) > 0 )
                  {
                     // get to last space
                     dwk += dwj;
                     lps += dwj;
                  }
                  // we should since we already checked WORD COUNT
                  if( ( dwk        ) &&
                     ( dwk < dwi ) )
                  {
                     DWORD dwh;
                     // we have the offset to the last SPACE
                     if( IsSal( &lpn[dwk] ) ) // ChkSal check if begin as salutation
                     {
                        BOOL  bgs = TRUE;
                        if( dwc > 2 )  // then we could back up one more ...
                        {
                           DWORD dwk2;
                           lps = lpn;  // only now interested in our NAME buffer
                           dwk2 = 0; // extracted from the FILE buffer given ...
                           while( ( ( dwj = ginstr(lps," ") ) > 0 ) &&
                              ginstr( &lps[dwj]," " ) )
                           {
                              // get to the 2nd last space
                              dwk2 += dwj;
                              lps += dwj;
                           }
                           if( dwk2 )
                           {
                              if( !IsSal( &lpn[dwk2] ) ) // ChkSal check if begin as salutation
                              {
                                 dwk = dwk2;
                                 bgs = FALSE;
                              }
                           }
                        }
                        if( bgs )
                        {
                           chkme( "CHECK ME: Sorry, its a SAL SAL so not X1 ..." );
                           goto Exit_xh;
                        }
                     }

                     if( ( gbNotSame ) &&
                        ( *lpn == lpn[dwk] ) )
                     {

#ifdef   CHKSAMELET

                           chkme( "Same letter inhibit is ON ..." );
#endif   // CHKSAMELET

                           goto Exit_xh;
                     }

                     // **********************************************
                     // Build X1 - Last name to FIRST
                     // **********************************************
                     // 1. copy it into 2 ...
                     chkcpy(lpn2, &lpn[dwk]);   // make it the SUBJECT
                     giLnCnt++;

                     // now for hyphenated names must also be split
                     if( ( dwh = ginstr(lpn2,"-") ) > 0 )
                     {
                        chkcpy(lpn3, &lpn2[dwh]);
                        dwh--;   // back up off the HYPHEN "-"
                        lpn2[dwh] = 0; // and CHOP the first here
                        giLnCnt++;
                        if( ( dwh = ginstr(lpn3,"-") ) > 0 )
                        {
                           chkme( "TO BE DONE: This is a multiple family name ..." );
                        }
                        // show loss and end of both
                        strcat(lpn3,"+");
                        strcat(lpn2,"+");

                        // fix this name with asterix separator
                        gaddsp(lpn3);
                        // asterix SEPARATION, plus a SPACE
                        strcat(lpn3,"*"
                           " ");
                        chkcpyn(&lpn3[lstrlen(lpn3)],lpn,dwk);
                        grtrimall(lpn3);
                     }

                     gaddsp(lpn2);  // ensure a SPACE at end
                     // asterix SEPARATION, plus a SPACE
                     strcat(lpn2,"*"
                        " ");
                     chkcpyn(&lpn2[lstrlen(lpn2)],lpn,dwk);
                     grtrimall(lpn2);

                  }
                  else
                  {
                     chkme( "HEY: Word count was greater than 1, thus why HERE!!!" );
                  }
Exit_xh:

                  if( giLnCnt > 1 )
                     iRet++;

               }
            }


   return iRet;

}

/* ===========================================================
   int  GetHdrs( LPTSTR lpc, DWORD dwIn )

   PURPOSE: To try splitting the HEAD of the BLOCK
      into a set of HEADERS (for different alphabetic insertions)

   COMMAND SWITCH: -gX[1]

   At this time the BLOCK of file data has been neatly copied
   into an OUT buffer, removing all controls and
   multiple spaces ...

   =========================================================== */
int  GetHdrs( LPTSTR lpc, DWORD dwIn )
{
   int   iRet = 0;
   DWORD    dwi = dwIn;
   LPTSTR   lpn = &gcHeadOrg[0];
   LPTSTR   lpn2 = &gcHeadX1[0];
   LPTSTR   lpn3 = &gcHeadX2[0];

   // kill all the previous HEADERS
   *lpn = *lpn2 = *lpn3 = 0;
   giLnCnt = 0;

         if( dwi < NMMX2C )
         {
            lstrcpyn(lpn,lpc,dwi); // copy up to, but EXCLUDING the comma
            if(grtrim(lpn))   // close up the buffer
               dwi = lstrlen(lpn);  // and correct length if changed
            giLnCnt++;
            iRet++;
            // -gX[1] switch
            // now if we want multiple entries per entry then
            // ==============================================
            if( gbDoExtra )
            {
               // other headers are built -gX1 switch
               // ***********************************
               GetXHdrs(lpn,dwi);
               // ***********************************
            }
         }
         else
         {
            lstrcpyn(lpn,lpc,NMMX2C);  // just get what is there
         }

   return iRet;

}

int  GetHdrs_OK( LPTSTR lpc, DWORD dwIn )
{
   int   iRet = 0;
   DWORD    dwi = dwIn;
   LPTSTR   lpn = &gcHeadOrg[0];
   LPTSTR   lpn2 = &gcHeadX1[0];
   LPTSTR   lpn3 = &gcHeadX2[0];

   *lpn = *lpn2 = *lpn3 = 0;
   giLnCnt = 0;
         if( dwi < NMMX2C )
         {
            lstrcpyn(lpn,lpc,dwi); // copy up to, but EXCLUDING the comma
            if(grtrim(lpn))   // close up the buffer
               dwi = lstrlen(lpn);  // and correct length if changed
            giLnCnt++;
            iRet++;
            // now if we want multiple entries per entry then
            // ==============================================
            if( gbDoExtra )
            {
               DWORD dwc;
//               GetXHdrs(lpn,dwi);
#ifdef   CHKYVAN
               if( ginstr(lpn,"Yvan ") == 1 )
                  chkme( "Why is this NOT split into 2, or is it ...???" );
#endif   // CHKYVAN
               if( ( dwc = GetWdCnt(lpn) ) > 1 )
               {
                  // rotate the LAST word to the FRONT then follow with rest
                  // Annie's birthday, thus must shower and off to lunch
                  // avec la mere (  this is her mother / c'est la m�re de Annie )
                  DWORD dwj, dwk;
                  LPTSTR   lps = lpn;  // only now interested in our NAME buffer
                  dwk = 0; // extracted from the FILE buffer given ...
                  while( ( dwj = ginstr(lps," ") ) > 0 )
                  {
                     // get to last space
                     dwk += dwj;
                     lps += dwj;
                  }
                  // we should since we already checked WORD COUNT
                  if( ( dwk        ) &&
                     ( dwk < dwi ) )
                  {
                     DWORD dwh;
                     // we have the offset to the last SPACE
                     if( IsSal( &lpn[dwk] ) ) // ChkSal check if begin as salutation
                     {
                        BOOL  bgs = TRUE;
                        if( dwc > 2 )  // then we could back up one more ...
                        {
                           DWORD dwk2;
                           lps = lpn;  // only now interested in our NAME buffer
                           dwk2 = 0; // extracted from the FILE buffer given ...
                           while( ( ( dwj = ginstr(lps," ") ) > 0 ) &&
                              ginstr( &lps[dwj]," " ) )
                           {
                              // get to the 2nd last space
                              dwk2 += dwj;
                              lps += dwj;
                           }
                           if( dwk2 )
                           {
                              if( !IsSal( &lpn[dwk2] ) ) // ChkSal check if begin as salutation
                              {
                                 dwk = dwk2;
                                 bgs = FALSE;
                              }
                           }
                        }
                        if( bgs )
                        {
                           chkme( " sorry, its a SAL ..." );
                           goto Exit_Add;
                        }
                     }

                     if( ( gbNotSame ) &&
                        ( *lpn == lpn[dwk] ) )
                     {

#ifdef   CHKSAMELET

                           chkme( "Same letter inhibit is ON ..." );
#endif   // CHKSAMELET

                           goto Exit_Add;
                     }

                     chkcpy(lpn2, &lpn[dwk]);   // make it the SUBJECT
                     giLnCnt++;
                     // now for hyphenated names must also be split
                     if( ( dwh = ginstr(lpn2,"-") ) > 0 )
                     {
                        chkcpy(lpn3, &lpn2[dwh]);
                        dwh--;   // back up off the HYPHEN "-"
                        lpn2[dwh] = 0; // and CHOP the first here
                        giLnCnt++;
                        if( ( dwh = ginstr(lpn3,"-") ) > 0 )
                        {
                           chkme( "TO BE DONE: This is a multiple family name ..." );
                        }
                        gaddsp(lpn3);
                        chkcpyn(&lpn3[lstrlen(lpn3)],lpn,dwk);
                     }
                     gaddsp(lpn2);
                     chkcpyn(&lpn2[lstrlen(lpn2)],lpn,dwk);
                  }
                  else
                  {
                     chkme( "HEY: Word count was greater than 1, thus why HERE!!!" );
                  }
               }
            }
         }
         else
         {
            lstrcpyn(lpn,lpc,NMMX2C);  // just get what is there
         }

Exit_Add:

   return iRet;

}

void  AddDT4( LPTSTR lpb )
{
   SYSTEMTIME  st;
   GetLocalTime(&st);
   sprintf( EndBuf(lpb),
      "%02d/%02d/%02d  %02d:%02d",
		st.wDay,
		st.wMonth,
		(st.wYear % 100),
		st.wHour,
		st.wMinute );
}

void  OutBlock( LPTSTR lpbuf, DWORD dwInLen, DWORD dwBlkNum )
{
//   LPTSTR   lpout = &gcOutBuf[0];
   //LPMBLST  lpgmb = &gsMBLst;
   LPTSTR   lpout, lpc;
//   DWORD gdwFlag;
   DWORD dwi, dwk, dwcnt, dwl;
   char  c, d, d2, d3, d4;
   BOOL  bdcc = FALSE;
   BOOL  badd = TRUE;
   BOOL  sc = FALSE;
   BOOL  hadsc = FALSE;
   int   sccnt = 0;
   BOOL  igpwd = FALSE;
   LPTSTR   lpt = &gcTailSee[0];
   // LPTSTR   lpnm = &gcNum[0]; // = [MXNUMSET+8];
   gcNum[0] = 0;  // clear any rsidual number
   *lpt = 0;
   gbGotBU1 = gbGotCU = FALSE;   // (BOOL)chkbu( lpb, &dwo );
   dwl = dwInLen;
   lpout = glpOut;
   if(( lpbuf      ) &&
      ( dwl      ) &&
      ( lpout != 0 ) )
   {
      dwcnt = gdwFlag = dwk = 0;
      d = d2 = d3 = d4 = 0;
      lpc = lpbuf;

      while( ( ( c = *lpc ) > 0 ) && ( c <= ' ' ) )
      {
         lpc++;   // move over spaces
         if(dwl)
            dwl--;   // and reduce LENGTH of BLOCK
      }

#ifdef   ADDCHKARM

      if( ( lpc[0] == 'A' ) &&
         ( lpc[1] == 'r' ) &&
         ( lpc[2] == 'm' ) )
         chkme( "Entry of Armond ..." );

#endif   // ADDCHKARM
#ifdef   CHKMHSIN

      if( ginstr(lpc, "M H Sin") == 1 )
         chkme( "Doing the tale of the M H Sin ...s.salut... Why an X1???" );

#endif   /* CHKMHSIN */

      gcHeadOrg[0] = 0;
//      if( ( dwi = ginstr(lpc,",") ) > 1 )
//         GetHdrs(lpc,dwi);

      //for( dwi = 0; dwi < dwLen; dwi++ )
      for( dwi = 0; dwi < dwl; dwi++ )
      {
         if(
            ( d4 == 'P' ) &&
            ( d3 == 'W' ) &&
            ( d2 == 'D' ) )
         {
            // flag we have a PWD
            gdwFlag |= Had_PWD;  // had PWD ... in stream
            // yeek, this is private data in this line
            // =======================================
            if( gbChkPWD )
               igpwd = TRUE;
         }
         //c = lpb[dwi];
         //lpc = &lpb[dwi];
         c = *lpc++;
         badd = TRUE;   // note - DEFAULT is to ADD character
         sc = TRUE;
         if( ( c < 0 ) || ( c > ' ' ) )
         {
            if( ( c < 0 ) || ( IsPChr(c) ) )
               sc = FALSE;

            // hi-bit and significant ascii chars
            //badd = TRUE;
            switch( c )
            {
            case ',':   // found a comma in the string
               gdwFlag |= Had_Coma;
               break;
            case ':':
               gdwFlag |= Had_Colon;
               break;
            case '(':
               gdwFlag |= Had_OpenBr;
               break;
            case ')':
               gdwFlag |= Had_CloseBr;
               break;
            case '{':
               gdwFlag |= Had_OpenBc;
               break;
            case '}':
               gdwFlag |= Had_CloseBc;
               break;
            case '[':
               gdwFlag |= Had_OpenBs;
               break;
            case ']':
               gdwFlag |= Had_CloseBs;
               break;
            }
            bdcc = FALSE;

            if( hadsc )
            {
               sccnt++;
            }
            else
            {
               if( bRemoveComts )
               {
                  if( c == ';' )
                  {
                     dwi++;
                     //for( ; dwi < dwLen; dwi++ )
                     for( ; dwi < dwl; dwi++ )
                     {
                        //c = lpb[dwi];
                        c = *lpc++;
//                        if( c == '\n' )
                        if( ( c < ' ' ) &&
                           ( c != '\t' ) )
                        {
                           // back up to catch this / these control chars
                           dwi--;
                           lpc--;
                           break;
                        }
                     }
                     badd = FALSE;  // add NOTHING of this line
                     c = ' ';    // kill whatever it is anyway
                  }
                  else
                  {
                     hadsc = TRUE;
                     sccnt = 1;
                  }
               }
               else
               {
                     hadsc = TRUE;
                     sccnt = 1;
               }
            }
         }
         else  // if( c <= ' ' )
         {
            // space or control chars
            if( c == '\n' )
            {
               dwcnt++;
               gdwFlag |= Had_Lf;
               if( d == '\r' )
                  gdwFlag |= Had_CrLf;
               hadsc = FALSE;
               sccnt = 0;
               igpwd = FALSE;
            }
            else if( c == '\r' )
            {
               gdwFlag |= Had_Cr;
               igpwd = FALSE;
            }

            // always just add only one space for these
            // ========================================
            if( ( !bdcc ) &&
               ( dwk ) )
            {
               bdcc = TRUE;
               //badd = TRUE;
               c = ' ';
            }
            else
            {
               // else DUMP chars
               badd = FALSE;
            }
         }

         if( badd )
         {
            if( igpwd )
               c = '*';

            lpout[dwk++] = c;
            //badd = FALSE;
         }

//         if( dwk >= MXOUTBUF )
         if( dwk >= gdwOutSz )
         {
            chkme( "YEEK, the size of the buffer needs to be really expanded ..." );

            lpout[dwk] = 0;
/////            strcat(lpout,"\r\n");
            sprtf(lpout);
            dwk = 0;
         }
         // ======================
         d4 = d3;
         d3 = d2;
         d2 = (char)toupper(d);
         d = c;   // keep previous
         // ======================
      }  // for chars in the block

      if( dwk )
      {
         int   i;
         LPTSTR   lpo2; // = glpOut2;   // = &gcOutBuf2[0];
         LPTSTR   lpn2; // = &gcHeadX1[0];
         // we have COPIED the data from the FILE buffer to an OUT buffer for massageing
         // ============================================================================
            lpout[dwk] = 0;

//         if( ( dwi = ginstr(lpout,",") ) > 1 )
//            GetHdrs(lpout,dwi);

            if( bGotSigChrs(lpout) )
            {
               if( dwBlkNum == 1 )  // added 14 October, 2000 ADD CURRENT DATE to AAAA string
               {
                  if( ( lpout[0] == 'A' ) &&
                     (  lpout[1] == 'A' ) &&
                     (  lpout[2] == 'A' ) &&
                     (  lpout[3] == 'A' ) )
                  {
                     strcat(lpout," [Done ");
                     AddDT4(lpout);
                     strcat(lpout,"]");
                  }
               }
               strcat(lpout,MEOR); // = "\r\n");
               i = lstrlen(lpout);
               if( VERB )
                  sprtf(lpout);

               if( ( lpo2 = glpOut2 ) != 0 )
               {
                  *lpo2 = 0;
                  if( i < MXOUTBUF )
                     strcpy(lpo2,lpout); // take COPY of line
               }
               if( bAddShort )
               {
                  CheckBlock( lpout, dwk, &gdwFlag, dwcnt );
                  if( VERB5 ) // was VERB3 )
                  {
                     if( bGotSigChrs(lpout) )
                     {
                        if( lstrlen(lpout) == i )
                        {
                           if( ( lpo2 ) &&
                              ( lstrcmp(lpout,lpo2) == 0 ) )
                           {
                              sprtf( "<no changes>"MEOR ); // = "\r\n" );
                           }
                           else
                           {
                              sprtf(lpout);
                           }
                        }
                        else
                        {
                           sprtf(lpout);

                        }

//                        sprtf(lpout);
                     }
                  }
               }

               if( ( dwi = ginstr(lpout,",") ) > 1 )
                  GetHdrs(lpout,dwi);
               // =========================
               bAdd2List( lpout, gdwFlag );
               // =========================
               if( bAddShort && VERB5 )
               {
                  int iLns = 0;
                  BOOL  bGot2;
                  DWORD dwo;
                  if( giLnCnt > 1 )
                     iLns = giLnCnt - 1;
                  bGot2 = (BOOL) ( gdwFlag & Got_TWO );
//                  if( ( VERB3 ) &&
//                     ( gdwFlag & Got_TWO ) ) // Done_TWO
                  if( ( iLns ) ||
                     ( bGot2 ) ) // Done_TWO
                  {
                     LPTSTR   lpo2 = glpOut2;   // = &gcOutBuf2[0];
                     if( lpo2 && bGot2 )
                     {
                        //LPTSTR   lph2 = &gcHead2[0]; // [ (NMMX2C + 15) ];;
                        lpn2 = &gcHead2[0]; // [ (NMMX2C + 15) ];;
                        strcpy(lpo2,lpn2);
                        if( gbUseSee )
                        {
                           if( *lpt )
                              strcat(lpo2,lpt);
                           else
                              strcat(lpo2, "WARNING: No TAIL generated!!! WHY???" );
//                              chkme( "WARNING: No TAIL generated!!! WHY???" );
                        }
                        strcat(lpo2,MEOR);
                        // just show adjusted HEADER
                        //strcat(lpo2, &lpout[ (dwo - 1) ] );  // this current tail
                        sprtf(lpo2);
                     }
                     if( ( lpo2 && iLns ) &&
                        ( dwo = ginstr(lpout,",") ) > 0 )
                     {
//                  LPTSTR   lpn2 = &gcHeadX1[0];
//                  LPTSTR   lpn3 = &gcHeadX2[0];
//                  strcpy(lps2,lpn2);  // get the previously prepared head, and
//                  strcat(lps2, &lps[ (dwo - 1) ] );  // this current tail
//                  if( i = lstrlen(lps2) )
//                        LPTSTR   lpn2 = &gcHeadX1[0];
                        lpn2 = &gcHeadX1[0];
                        strcpy(lpo2,lpn2);  // get the previously prepared head, and
                        //strcat(lpo2, &lpout[ (dwo - 1) ] );  // this current tail
                        if( gbUseSee )
                        {
                           if( *lpt )
                              strcat(lpo2,lpt);
                           else
                              strcat(lpo2, "WARNING: No TAIL generated!!! WHY???" );
//                              chkme( "WARNING: No TAIL generated!!! WHY???" );
                        }
                        strcat(lpo2,MEOR);
                        sprtf(lpo2);
                        if( iLns > 1 )
                        {
                           lpn2 = &gcHeadX2[0];
                           strcpy(lpo2,lpn2);  // get the previously prepared head, and
                           //strcat(lpo2, &lpout[ (dwo - 1) ] );  // this current tail
                           if( gbUseSee )
                           {
                              if( *lpt )
                                 strcat(lpo2,lpt);
                              else
                                 strcat(lpo2, "WARNING: No TAIL generated!!! WHY???" );
//                                 chkme( "WARNING: No TAIL generated!!! WHY???" );
                           }
                           strcat(lpo2,MEOR);
                           sprtf(lpo2);
                        }
                     }
                  }
               }
            }
            else
            {
               //chkme( "Just discarding a <BLANK> line ....!!!" );
            }

            dwk = 0;
      }
   }
   else
   {
      chkme( "C:ERROR: CHECK some itnernal parameter ..." );
   }
}

#ifndef  ADDGEOFF2
//extern   int  DoGeoff( LPDFSTR lpdf ); // -g gbDoGeoff = TRUE
int	DoGeoff_ORG( LPDFSTR lpdf )
{
   int   iRet = 0;
   HFILE hf;
   LPTSTR fn;
   BYTE * lpb;
   DWORD rd;
//   DWORD fsiz;
   DWORD dwi;
   char  c;
   LPTSTR   lpc;
   DWORD    dwBlkCnt = 0;
//   LPTSTR   lpout = &g_Stg[0];
//      ( fsiz = lpdf->qwSize.LowPart ) &&

   if( ( hf = (HFILE)lpdf->hf ) != 0 &&
      ( fn = lpdf->fn ) != 0 &&
      ( lpb = lpdf->lpb ) != 0 &&
      ( rd = lpdf->dwrd ) != 0 &&
      ( IsDoGeoff((char *)lpb,rd ) ) )
   {
      //LPMBLST  lpgmb = &gsMBLst;
      DWORD dwbgn, dwb;
      dwbgn = dwi = 0;    // begin of data block
//      for( ; dwi < fsiz; dwi++ )
      for( ; dwi < rd; dwi++ )
      {
         lpc = (LPTSTR)&lpb[dwi];  // point to char
         c = *lpc;  // extract char
         if( c == ';' ) // maybe commence of boundary line
         {
            dwb = IsBound( (char *)&lpb[dwi+1], ( rd - (dwi+1) ) );
            if( dwb )
            {
               dwBlkCnt++;    // bump the BLOCK count
               gdwFlag = 0;   // restart global flag for EACH block
               // ==================================================
               OutBlock( (char *)&lpb[dwbgn], (dwi-dwbgn), dwBlkCnt );
               // ==================================================
               dwi += dwb;
               lpc = (char *)&lpb[dwi];  // point to char
               dwbgn = dwi;
            }
         }
      }
   }

   return iRet;

}

#endif   // #ifndef  ADDGEOFF2

// eof - Dump4g.c

// ************************* end dump4g *****

STARTUPINFO          sSI;
PROCESS_INFORMATION  sPI;

VOID  Set_SI( STARTUPINFO * p, LPTSTR pcmdline, BOOL bIsCon )
{
   /* Launch the process and waits for it to complete */
   STARTUPINFO si;

   memset( &si, 0, sizeof(STARTUPINFO) );

   si.cb = sizeof(STARTUPINFO);
   //si.lpReserved = NULL;
   //si.lpReserved2 = NULL;
   //si.cbReserved2 = 0;
   if( bIsCon )
      si.lpTitle = pcmdline; 

   //si.lpDesktop = (LPTSTR)NULL;

   si.dwFlags = STARTF_FORCEONFEEDBACK;

   memcpy( p, &si, sizeof(STARTUPINFO) );

}

DWORD WINAPI do_runexcell( PVOID lpParam )  // thread data
{
   PRUNSTR              prs   = (PRUNSTR)lpParam;
   LPTSTR               lpcmd = prs->lpcmd;  // &g_szCmdLine[0];
   LPTSTR               lpexe = prs->lpexe;
   STARTUPINFO         * psSI = &sSI;
   PROCESS_INFORMATION * psPI = &sPI;

//   dc4w_UI(TRUE);

   // CreateThread - NO CREATE PROCESS and wait for it ...
   memset(psSI, 0, sizeof(STARTUPINFO) );

   Set_SI( psSI, lpcmd, FALSE );

   memset(psPI, 0, sizeof(PROCESS_INFORMATION));

   // system(
   if( CreateProcess( lpexe,   // name of executable module
      lpcmd,   // command line string
      NULL, // SD
      NULL, // SD
      FALSE,   // handle inheritance option
      CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS, // creation flags
      NULL, // new environment block
      NULL, // current directory name
      psSI,      // startup information
      psPI  ) ) // process information
   {
      DWORD dwi;
      TCHAR    szhex[64];
      LPTSTR   lps = &szhex[0];
      sprtf( "Got [%s] running. id=%X"MEOR, g_szExeFile, psPI->dwProcessId );

      /* wait for completion. */
      dwi = WaitForSingleObject( psPI->hProcess, INFINITE );
      switch(dwi)
      {
      case WAIT_ABANDONED:
         lps = "Abandoned";
         break;
      case WAIT_OBJECT_0:
         lps = "ClosedApp";   // "Object0";
         break;
      case WAIT_TIMEOUT:
         lps = "TimeOut";
         break;
      case WAIT_FAILED:
         lps = "Failed";
         break;
      default:
         lps = &szhex[0];
         sprintf(lps, "%#X", dwi );
         break;
      }

      /* close process and thread handles */
      CloseHandle( psPI->hThread);
      CloseHandle( psPI->hProcess);
      sprtf( "Closed [%s]. id=%X ?=%s"MEOR, g_szExeFile, psPI->hThread, lps );

   }
   else
   {
      // failed to start - WHY?
      static TCHAR _s_cErrMsg[1024];
      DWORD    dwError;
      LPTSTR   lpe = &_s_cErrMsg[0];
      PVOID    lpMsgBuf;

      dwError = GetLastError();  // get error indications
      sprintf(lpe, "ERROR: YEEK! Failed to launch"MEOR
              "EXE [%s]"MEOR,
              lpexe );
      if(lpcmd && *lpcmd)
         sprintf(EndBuf(lpe), "CMD [%s]"MEOR, lpcmd);

      sprintf(EndBuf(lpe),"Error id = %d (%#x)",
         dwError,
         dwError );

      if( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
         FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL,
         dwError,    // = GetLastError(),
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
         (LPTSTR) &lpMsgBuf,
         0,
         NULL ) )
      {
         sprintf(EndBuf(lpe), MEOR"MSG [%s]", (char *)lpMsgBuf);
         // Process any inserts in lpMsgBuf.
         // Display the string.
         //MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
         // Free the buffer.
         LocalFree( lpMsgBuf );
      }
      sprtf( MEOR"%s"MEOR, lpe );

   }

//   dc4w_UI(FALSE);

   return( (DWORD) 0xadded );
}


// ************ FIX20031207 **************
#ifdef ADDGEOFF1  // moving the dump4 -g1 command to here in FixF32

//#define SEPCHAR      ","
#define SEPCHAR      ";"

TCHAR g_szSepChar[] = SEPCHAR;

// helper macros
#define EatSp(a)  while( *a && (*a <= ' ') )a++
#define EatNum(a) while( *a && ISNUM(*a) )a++
#define EatNumt(a) while( *a && (ISNUM(*a) || ( *a == '-' ) ) )a++
#define EatNums(a) while( *a && (ISNUM(*a) || ( *a == ' ' ) ) )a++
#define EatItem(a) while( *a && (*a > ' ') )a++
#define ISPC4(a) (ISNUM(a[0]) && ISNUM(a[1]) && ISNUM(a[2]) && ISNUM(a[3]))
#define ISPOSTCODE(a) ISPC4(a) && !ISNUM(a[-1]) && !ISNUM(a[4])

// like
//Taylor G A,18 Hobart St,Riverstone,2765,(02) 9627 5835,
//Taylor G A,66a Edward St,Sylvania,2224,(02) 9544 6049,
//Taylor G A,74 Victoria St,Waverley,2024,(02) 9369 3885,
//Taylor G & D,14 Rosebery St,Heathcote,2233,(02) 9520 8998,
//Taylor G & G,61 Florence Tce,Scotland Island,2105,(02) 9979 7309,
//Taylor G & J,16 Bangalay Pl,Macquarie Fields,2564,(02) 9605 5783,
//Taylor G & M,17 Cambewarra Crs,Berowra Heights,2082,(02) 9456 4142,
//Taylor G & P,12 Felton St,Telopea,2117,(02) 9890 1467,
TCHAR g_szCSVHdr[264] = { 
 "Name_Inits"SEPCHAR
    "Street_Address"SEPCHAR
    "Suburbia"SEPCHAR
    "Code"SEPCHAR
    "Tel_Number1"SEPCHAR
    "Tel_Number2"
};


LIST_ENTRY  gsLines1 = { &gsLines1, &gsLines1 };

PTSTR g_pStreet[] = {
   "Ave", "Rd", "Pde", "St", "Cl", "Pl", "Drv", "Crt", "Gr", "Cres",
      "Tce", "Crs", "La", "Cct" };

#define  nStCount  ( sizeof(g_pStreet) / sizeof(PTSTR) )

TCHAR g_szExcl1[264] = { "Mobile Service" };
PTSTR g_pFndSt;

DWORD FindStreet( PTSTR pln )
{
   DWORD dwi;
   g_pFndSt = 0;
   for( dwi = 0; dwi < nStCount; dwi++ )
   {
      PTSTR ps = g_pStreet[dwi];
      // DWORD iPos = InStr( pln, ps );
      DWORD iPos = InStrWhole( pln, ps );
      if(iPos) {
         g_pFndSt = ps;
         return iPos;
      }
   }
   return 0;
}

VOID SplitatStreet( PTSTR pln )  // do the STREET split
{
   DWORD len;  // = strlen(g_pFndSt);
   PTSTR prite;   // = Right( pln, strlen(pln) - (iPos - 1) - len );
   DWORD iPos = FindStreet(pln);
   PTSTR pln3 = gszLineB3;  // build it into here
   strcpy(pln3, pln);
   if(iPos) {
      // work to do
      len = strlen(g_pFndSt);
      //prite = Right( pln, strlen(pln) - (iPos - 1) - len );
      //pln3[(iPos - 1 + len)] = 0;  // terminate after the street
      // right trim 
      //grtrim(pln3);
      //while( *prite && (*prite == ' ') )
      //   prite++;
      //strcat(pln3,SEPCHAR);
      //strcat(pln3,prite);
      //strcpy(pln, pln3);
   } else {
      // yay! No street, like
      // The Cresent Dee Why,
      // Kingsway Cronulla
      // The Parapet Castlecrag
      // and maybe others
      // try skip any first number, and count words, to
      // next number, and make a SPLIT somewhere
      PTSTR pbgn;
      PTSTR p = pln3;
      DWORD dwc = 0;
      BOOL  bgotn;
      PTSTR ptrs[4];
      EatSp(p);
      bgotn = (ISNUM(*p)) ? TRUE : FALSE;
      EatNumt(p);
      pbgn = p;   // start of WORD count
      EatSp(p);
      ptrs[dwc] = p;
      while( *p )
      {
         EatItem(p);
         EatSp(p);

         dwc++;   // bump word (item) count
         if(dwc < 4)
            ptrs[dwc] = p; // store pointer

         if(ISNUM(*p))  // stop at POSTAL CODE number
            break;
      }
      switch(dwc)
      {
      case 0:
         iPos = (ptrs[0] - pln3) + 1;
         //iPos = (p - pln3) + 1;
         break;
      case 1:
         iPos = (ptrs[0] - pln3) + 1;
         break;
      case 2:
         iPos = (ptrs[1] - pln3) + 1;
         break;
      case 3:
         iPos = (ptrs[2] - pln3) + 1;
         break;
      case 4:
         iPos = (ptrs[2] - pln3) + 1;
         break;
      default:
         iPos = (ptrs[2] - pln3) + 1;
         break;
      }

      len = 0;
   }
   // action the SPLIT
      prite = Right( pln, strlen(pln) - (iPos - 1) - len );
      pln3[(iPos - 1 + len)] = 0;  // terminate after the street
      // right trim 
      grtrim(pln3);
      while( *prite && (*prite == ' ') )
         prite++;

      strcat(pln3,SEPCHAR);
      strcat(pln3,prite);
      strcpy(pln, pln3);
}


PTSTR RemoveStr( PTSTR pln, PTSTR pdel )
{
   DWORD iPos = InStr( pln, pdel );
   if( iPos ) {

      PTSTR pleft = Left( pln, iPos - 1 );
      PTSTR prite = Right( pln, strlen(pln) - (iPos - 1) - strlen(pdel) );
      PTSTR lps = GetNxtBuf();
      DWORD len = strlen(pleft);

      //if( *prite == ',' ) {
      if( *prite == g_szSepChar[0] ) {
         if( len && ( pleft[ len - 1 ] == ' ' ) ) {
            len--;
            pleft[len] = 0;
         }
      }
      strcpy( lps, pleft );
      strcat( lps, prite );

      return lps;
   }
   return 0;
}

VOID  LineMemOut1( MYHAND * ph, LPTSTR lps )
{
   static TCHAR _s_outbuf[1024];
   PTSTR pb = _s_outbuf;

   strcpy(pb,lps);
   strcat(pb,MEOR);

   if( ph && VFH( *ph ) )
      grmWriteFile( ph, pb );

   if( VFH(gOut_Hand) )
      grmWriteFile( &gOut_Hand, pb );
      // assume writing CSV - Take length of FIRST to comma,
}

VOID  OutLineMem1(VOID)
{
   PLE   ph = &gsLines1;
   PLE   pn;
   PTSTR lpb;
   int   iCnt = 0;
   MYHAND   h = 0;
   //PTSTR   lps2 = glpOut2;
   PTSTR   lpf  = &gcGOutFile[0];   // = [264] = {"\0"};
   PTSTR pln3 = gszLineB3;  // build it into here

   LstCnt2( ph, &iCnt );
   //if( iCnt )
   if( IsListEmpty(ph) ) {

   } else {
      if( *lpf )
         h = grmCreateFile( lpf );
      if( VH(h) )
      {
         // handle is OPEN
      }
      else
      {
         h = 0;
      }
      lpb = pln3;
      strcpy(lpb, g_szCSVHdr);
      // strcat(lpb, MEOR);
      LineMemOut1( &h, lpb );

      Traverse_List(ph,pn)
      {
         lpb = (PTSTR)((PLE)pn + 1);
         LineMemOut1( &h, lpb );
      }

      if( VH(h) )
      {
         grmCloseFile( &h );
         RunExcell( lpf );
      }
   }
}

BOOL AddLines( PTSTR plines )
{
   PTSTR pln1 = gszLineB1;  // build it into here
   PTSTR pln2 = gszLineB2;  // build it into here
   PTSTR pln3 = gszLineB3;  // build it into here
   PTSTR lpb = plines;
   PLE   ph = &gsLines1;
   PLE   pn;
   DWORD len;
   if( *pln1 && *pln2 && (*lpb == ' ') ) {

      strcpy(pln3,pln1);
      strcat(pln3,SEPCHAR);
      strcat(pln3,pln2);
      len = strlen(pln3);
      pn = MALLOC( (sizeof(LIST_ENTRY) + len + 1) );
      if(pn) {
         lpb = (PTSTR)((PLE)pn + 1);
         strcpy(lpb,pln3);
         InsertTailList(ph,pn);
         return TRUE;
      }
   }
   return FALSE;
}

// examples
// Taylor G J,18 Royal St Chatswood 2067 Mobile Service 0411 506 565,
// Taylor G J,35 Clifton Rd Clovelly 2031 (02) 9665 4357,
// back up numbers, including spaces and braces
INT   MassageLine2(VOID)
{
   INT   bcnt = 0;
   PTSTR pln2 = gszLineB2;  // add it into here
   PTSTR pln3 = gszLineB3;  // build it into here
   DWORD len = strlen(pln2);
   if(len) {
      SplitatStreet( pln2 );  // do the STREET split
      len = strlen(pln2);
   }
   if(len) {
      // get to the END
      PTSTR lpb = &pln2[ len - 1 ];
      TCHAR c = *lpb;
      DWORD numcnt = 0;
      DWORD spcnt = 0;
      DWORD bcnt = 0;
      TCHAR d = 0;
      PTSTR pend;
      // back over the telephone number (02) 1234 5678 or 0400 385 194
      while( ( lpb > pln2 ) &&
         (( ISNUM(c) ) || ( c == ' ' ) || ( c == ')' ) || ( c == '(' ) ) )
      {
         if( ISNUM(c) )
            numcnt++;
         else if( c == ' ' )
            spcnt++;
         else
            bcnt++;

         d = c;
         if(lpb > pln2) {
            // back up and get previous
            lpb--;
            c = *lpb;
         } else
            break;

         // some limits, country specific
         if( numcnt == 10 ) {
            // continue BACKING off telephone NUMBER, and spaces
            while( ( lpb > pln2 ) &&
               ( ( c == ' ' ) || ( c == ')' ) || ( c == '(' ) ) )
            {
               lpb--;
               c = *lpb;
            }
            break;
         }
      }

      if( ISALPHA(c) || ISNUM(c) )
         lpb++;

      pend = lpb; // keep this BEFORE number location

      while(*lpb == ' ')
         lpb++;

      if( *lpb ) {
         strcpy(pln3,lpb); // copy NUMBER into buffer 3
         *pend = 0;  // zero term. the line buffer 2
         strcat(pln2,SEPCHAR);   // add separator
         strcat(pln2,pln3);   // put BACK the number
         bcnt = 1;
         if(pend > pln2) {
            lpb = pend - 1;   // back up ONE
            while(( lpb > pln2 ) && ( !ISNUM(*lpb) )) {
               lpb--;
            }
            if( ISNUM(*lpb) ) {
               pend = lpb + 1;
            }
            if(lpb > pln2) {
               // this should be the POST CODE
               numcnt = 0;
               while(( lpb > pln2 ) && ( ISNUM(*lpb) ))
               {
                  numcnt++;
                  lpb--;
               }

               if(numcnt == 4) {
                  // discard "Mobile Service"
                  // *pend = 0;
                  pend = lpb;
                  while( !ISNUM(*lpb) )
                     lpb++;

                  strcpy(pln3,lpb); // keep the TAIL
                  if( InStr( pln3, g_szExcl1 ) ) { // "Mobile Service" 
                     lpb = RemoveStr( pln3, g_szExcl1 );
                     if(lpb)
                        strcpy(pln3,lpb);
                  }
                  lpb = pend;
                  while(( lpb > pln2 ) && ( *lpb == ' ' ))
                     lpb--;
                  lpb++;
                  *lpb = 0;
                  strcat(pln2,SEPCHAR);
                  strcat(pln2,pln3);

               }
            }
         }
      }

   }
   return bcnt;
}

VOID  ChkExtraLine( PMPDFIL pmf, ULARGE_INTEGER * puli )
{
   PTSTR pln2 = gszLineB2;  // add it into here
   PTSTR pln3 = gszLineB3;  // build it into here
   ULARGE_INTEGER  uli;
   PTSTR lpb, pln;
   TCHAR c, d;
   DWORD dwc, dwn;

   //MassageLine2();   // add comma delimeters

   uli.QuadPart = (*puli).QuadPart;
   lpb = &pmf->pData[ uli.QuadPart ];
   pln = pln3;
   *pln = 0;
   d = 0;
   dwn = dwc = 0;

   for( ; uli.QuadPart < pmf->sliSize.QuadPart; uli.QuadPart++ )
   {
      c = *lpb++;
      if( c > ' ' )
      {
         *pln++ = c;
         if( ISNUM(c) )
            dwn++;   // number count on 3rd line
      }
      else if( c == ' ' )
      {
         if( d > ' ' )
            *pln++ = c;
      }
      else
      {
         if(( c == '\r' ) ||
            ( c == '\n' ) ) {
            if((c == '\r') && ( (uli.QuadPart + 1) < pmf->sliSize.QuadPart ) &&
               (*lpb == '\n') ) {
               c = '\n';
               uli.QuadPart++;
            }
            if( *pln3 )
               break;
            else {
               if( dwc )
                  break;
               dwc++;
            }
         }
      }
      d = c;
   }

   *pln = 0;   // zero terminate extraction
   // trim the extra line
   pln = pln3;
   c = *pln;
   if( dwn == 10 ) {
      while(c) {
         if(( ISNUM(c) ) || ( c == '(' ))
            break;
         pln++;
         c = *pln;
      }
      if(pln > pln3) {
         PTSTR lps = GetNxtBuf();
         strcpy(lps,pln);
         strcpy(pln3,lps);
      }
   }
   strcat(pln2,SEPCHAR);
   strcat(pln2,pln3);
   if( *pln3 )
      (*puli).QuadPart = uli.QuadPart;

}

BOOL  DoGeoff1( LPTSTR lpf )
{
   PMPDFIL pmf = GetFileMap( lpf );
   DWORD    dws, dwc, dwo, dwl;
   LPTSTR   lpbgn, lpb, pblock;
   ULARGE_INTEGER  uli, ulib, ulibal;
   DWORD    c, d;
   BOOL     nl;
   PTSTR pln1 = gszLineB1;  // build it into here
   PTSTR pln2 = gszLineB2;  // build it into here
   PTSTR pln;
   PLE   ph = &gsLines1;

   dwc = 0;
   d = '\n';
   nl = TRUE;
   dws = dwo = dwl = 0;
   pln = pln1;
   if(pmf && pmf->pData && pmf->sliSize.QuadPart)
   {
      KillLList(ph); //    PLE   ph = &gsLines1;

      uli.QuadPart  = 0;
      ulib.QuadPart = pmf->sliSize.QuadPart;
      ulibal.QuadPart = pmf->sliSize.QuadPart;
      lpbgn = pmf->pData;
      pblock = lpbgn;
      for(uli.QuadPart = 0; uli.QuadPart < ulib.QuadPart; uli.QuadPart++ )
      {
         lpb = &lpbgn[ uli.QuadPart ];
         c = *lpb;

         if(( c == '\r' ) ||
            ( c == '\n' ) ) {
            if((c == '\r') && ( (uli.QuadPart + 1) < ulib.QuadPart ) &&
               (lpb[1] == '\n') ) {
               c = '\n';
               uli.QuadPart++;
            }

            dwl = 0; // restart line count
            nl = TRUE;
            if(dwo == 0) {
               pblock = lpb;
            } else {
               // we are starting NEXT line
               uli.QuadPart++;
               ulibal.QuadPart = pmf->sliSize.QuadPart - uli.QuadPart;
               *pln = 0;
               while( pln > pln1 ) {
                  pln--;
                  if(*pln > ' ')
                     break;
                  *pln = 0;
               }
               pln = pln2;
               *pln = 0;
               d = ' ';
               for( ; uli.QuadPart < ulib.QuadPart; uli.QuadPart++ )
               {
                  lpb = &lpbgn[ uli.QuadPart ];
                  c = *lpb;
                  if(( c == '\r' ) ||
                     ( c == '\n' ) ) {
                     if((c == '\r') && ( (uli.QuadPart + 1) < ulib.QuadPart ) &&
                        (lpb[1] == '\n') ) {
                        c = '\n';
                        uli.QuadPart++;
                     }
                     break;
                  }
                  if(*pln2) {
                     if( c == ' ' ) {
                        if( d != ' ' ) {
                           *pln++ = (TCHAR)c;
                        }
                     } else {
                        *pln++ = (TCHAR)c;
                     }
                  } else if( c > ' ' ) {
                     // first non-space line 2 character
                     *pln++ = (TCHAR)c;
                  }
                  d = c;
               }
               *pln = 0;
               while( pln > pln2 ) {
                  pln--;
                  if(*pln > ' ')
                     break;
                  *pln = 0;
               }

               if( *pln2 ) {
                  MassageLine2();   // add comma delimeters
                  ChkExtraLine( pmf, &uli );
               }
               lpb = pblock;
               if( AddLines( lpb ) )
                  dwc++;
            }
            pln = pln1;
            dwo = 0;
            *pln = 0;
         } else {
            if(( dwl == 0 ) && ( c == ' ' ) && ( d == '\n' )) {
               if( (uli.QuadPart + 1) < ulib.QuadPart ) {
                  if( ISUPPER(lpb[1]) ) {
                     pblock = lpb;
                     dwo++;
                  }
               }
            } else {
               *pln++ = (TCHAR)c;
            }
            dwl++;
         }

         //dwc += DoGBufCnt( lpb, dws );
         //ulib.QuadPart -= dws;
         //uli.QuadPart  += dws;

         d = c;
      }

      if( dwc )   // if we have BLOCKS
      {
         // process the BLOCKS
         sprtf( "Input file appears to have %d blocks ... processing ..."MEOR,
            dwc );
         ulib.QuadPart = pmf->sliSize.QuadPart;
         uli.QuadPart  = 0;
      }
      else
      {
         return FALSE;
      }

#if 0 // excluded code
//      while( ulib.QuadPart )
//      {
//         if( ulib.HighPart )
//            dws = (DWORD)-1;
//         else
//            dws = ulib.LowPart;
//
//         lpb = &lpbgn[ uli.QuadPart ];
//
//         dwc -= DoGBuf( lpb, dws );

//         ulib.QuadPart -= dws;
//         uli.QuadPart  += dws;
//      }
      d   = _s_iBgnChr;
      pblock = lpbgn;   // degin FIRST block start
      dwl    = 0;       // and LENGTH of BLOCK
      // ******* NB: Maximum single BLOCK is 64K ********
      for( uli.QuadPart = 0; uli.QuadPart < ulib.QuadPart; uli.QuadPart ++ )
      {
         lpb = &lpbgn[ uli.QuadPart ];
         c = *lpb;   // extract character
         if( ( c == d ) &&
            ( lpb[1] == _s_szBnd[0] ) )
         {
            ulibal.QuadPart = (ulib.QuadPart - uli.QuadPart);
            dwo = IsBound2( &lpb[1], &ulibal );
            if( dwo && dwl )
            {
               if(gdwBlkCnt)
                  dwl--;      // remove first INC of main loop
               gdwBlkCnt++;    // bump the BLOCK count
               OutBlock2( pblock, dwl );
               uli.QuadPart += dwo;
               pblock = &lpbgn[ uli.QuadPart ]; // get begin of NEXT block
               dwl = 0; // NOTE INC as part of main loop
            }
         }
         IDW(dwl);
      }
#endif // #if 0 // excluded code

      if(VERB)
         sprtf( "Done input file. Closing MAP ..."MEOR );

      DeleteFileMap( pmf );

      OutLineMem1();  // does LineMemOut(...) to -o[-|+]output.fil

      KillLList(ph); //    PLE   ph = &gsLines1;

      return TRUE;
   }

   return FALSE;
}

#endif   // #ifdef ADDGEOFF1  // moving the dump4 -g command to here in FixF32

// ********************************************************

#endif   //  ADDGEOFF2  // moving the dump4 -g command to here in FixF32
// =====================================================================

// eof - FixGeoff.c
