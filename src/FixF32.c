
// FixF32.c
#include "FixF32.h"
#undef  EXTRA_DEBUG

extern BOOL  gbNoEOF;   // add eof at end
extern void  DO(PTSTR a);
extern BOOL  g_bTags2NL;
//
// Simply idea is to FIX a file such that it is MORE readable
// This includes such things as ENSURING there is a Cr/Lf on every line,
// delete lines containing given words, and
// WRAP line at g_dwMaxLine length, at <=20h if Fix_LS flag is on (default)
//
// The command line is read, and a LIST of files to process is added to
// gsFileList, a double linked list. ProcessFiles() is called to action
// each of the files on the list.
// It in turn calls Do_File( pwl, lpf ) to map the file, then call
// Do_Buffer( lpb, dws ); to process it.
//
#ifdef  ADDJOBS1   // -j 22 Jan. 2004
// case 'J': lpc++; int Get_J_Cmd(PTSTR lpc) if( Pgm_Flag & Do_Jobs )   // -j
extern int DoJobs( PTSTR lpf );
#endif // #ifdef  ADDJOBS1   // -j 22 Jan. 2004

#define  Clr_Flag1	( Inp_Char2 = FALSE ) //	; Zero this FLAG
#define  Chk_Flag1   ( Inp_Char2 == TRUE )
#define  Set_Flag1	( Inp_Char2 = TRUE )

typedef struct tagInpStr {    // InpStr	struc
   DWORD    I_Ln;
   LPTSTR   I_Ptr;
}InpStr, * PIS; //	ends

//;===========================================================|
//;	Used by _Set_I_Line, _Get_I_Line and _End_I_Line
//;	------------------------------------------------
#define  MxInps	20 // Up to 20 lines at a TIME held
#define  MxIO	   0x8000   // ; INPUT Read_File buffer (FAR)
#define  MxOIO    1024     // ; Write out in blocks of ... (near)
#define  MxLin	   280	//	; Maximum INPUT line length
#define  BgnBr	   '['
#define  EndBr	   ']'
#define  MxFN	   260

InpStr   Inp_Array[ MxInps + 1 ]; //	db	MxInps * size InpStr dup(?) PLUS one ...
TCHAR    Inp_File[MXFILNAM + 8]; // Various NAMES ...
DWORD    Out_Appe = 0;  // Out_Appe2	label word

//;	The FLAGS themselves (and their DEFAULT value)
//;	====================
DWORD Pgm_Flag	= Def_Flag;
//Pgm_Flag2	DW	Def_Flag2
DWORD    Out_FSiz = 0;  //	Out_FSiz2	label word
#if	AddSplit
DWORD    Out_TFS = 0;   // Out_TFS2	label word
#endif   // ; AddSplit

// forward references
BOOL  Setup( VOID );    // allocate and SET work memory
BOOL  Get_O_File( VOID );

static TCHAR	sszInp[] = "InputFile";
TCHAR   End_Msg[] = "eof\r\n";   // eof plus CR,LF

PWRK	   gpW = 0;


void	SetFFZ( LPWIN32_FIND_DATA pfd )
{
	if( pfd )
	{
		pfd->cFileName[0] = 0;
		pfd->cAlternateFileName[0] = 0;
	}
}

int	GetNum( LPSTR lps, int	mx )
{
	int		i, j, k;
	char	c;

	k = mx;
	if( lps && *lps )
   {
      i = strlen( lps );
		k = 0;
		for( j = 0; j < i; j++ )
		{
			c = lps[j];
			if( Chk_Num( c ) )
			{
				k = ( k * 10 ) + ( c - '0' );
			}
			else if( ( c == ':' ) || ( c <= ' ' ) )
			{
				// do nothing
			}
			else
			{
				break;
			}
		}
	}
	if( k == 0 )
		k = mx;
	return k;
}



VOID  Close_O_File( VOID )
{
#ifdef _WIN32
   if( VFH( gOut_Hand ) )   //		; Get OUT handle
      CloseHandle(gOut_Hand);
#else
    if (VFH(gOut_Hand))   //		; Get OUT handle
        fclose(gOut_Hand);
#endif
   gOut_Hand = 0;
}

VOID  End_O_File( VOID )
{
   DWORD dwt = 0;
//      gszOFName[dwc] = 0;
//      Pgm_Flag |= Out_Fil; // Show using OUTPUT file
   if( Pgm_Flag & Out_Fil )   //	; Using OUTPUT file?
   {
      Close_O_File(); // ; Ensure it is close
      Pgm_Flag &= ~( Out_Fil );  // Remove any OUT FILE indicator
      dwt += Out_FSiz;
      //	Or	Ax,Out_FSiz2
#if	AddSplit
      dwt += Out_TFS;
      //	Or	Ax,Out_TFS2
#endif	// ; AddSplit
      if( dwt )
      {
         if( !( Pgm_Flag & GotAppe ) ) //	; Mark the appending
         {
            //	Mov	Dx,Offset OF_Name
	         // DOS_DELETE		; Remove an EMPTY OUT FILE
         }
      }
   }
}

//;	Search environment, and ADD directory of TEMP ...
//;	==================
//
VOID  Add_TEMPE( LPTSTR lpr )
{
   LPTSTR   lpe = getenv( "TEMP" );
   *lpr = 0;
   if( lpe && *lpe )
   {
      strcat(lpr, lpe);
      if( lpr[ strlen(lpr) - 1 ] != '\\' )
         strcat(lpr, "\\" );
   }
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Open_A_File
// Return type: HANDLE 
// Argument   : LPTSTR lpf
// Description: Open a file with READ ONLY
//              Return ZERO if failed, else the active HANDLE
///////////////////////////////////////////////////////////////////////////////
HANDLE  Open_A_File( LPTSTR lpf )
{
    HANDLE h = 0;
#ifdef _WIN32
   h = CreateFile( lpf, // file name
         (GENERIC_READ | GENERIC_WRITE),  // access mode
         FILE_SHARE_READ,  // share mode
         0, // SD
         OPEN_EXISTING, // how to create
         FILE_ATTRIBUTE_NORMAL,  // file attributes
         0 ); // handle to template file
#else // !#ifdef _WIN32
    h = fopen(lpf, "r");
#endif // #ifdef _WIN32 y/n

   if( !VFH(h) )
      h = 0;
   return h;
}

HANDLE   Creat_A_File( LPTSTR lpf )
{
    HANDLE h = 0;
#ifdef _WIN32
    h = CreateFile( lpf, // file name
         (GENERIC_READ | GENERIC_WRITE),  // access mode
         FILE_SHARE_READ,  // share mode
         0, // SD
         CREATE_ALWAYS, // how to create
         FILE_ATTRIBUTE_NORMAL,  // file attributes
         0 ); // handle to template file
#else // !#ifdef _WIN32
    h = fopen(lpf, "w");
#endif // #ifdef _WIN32 y/

   if( !VFH(h) )
      h = 0;
   return h;
}

BOOL  Do_File_Create( LPTSTR lpf, PHANDLE ph )
{
   BOOL  bRet = FALSE;
   HANDLE h = Creat_A_File( lpf );
   if( VFH(h) )
   {
      if(ph)
         *ph = h;
      bRet = TRUE;
   }
   return bRet;
}

//      gszOFName[dwc] = 0;
//      Pgm_Flag |= Out_Fil; // Show using OUTPUT file
BOOL  Get_O_File( VOID )
{
   BOOL  bRet = FALSE;  // no problem yet
   BOOL fApd = (Pgm_Flag & GotAppe); // Mark the appending
   LPTSTR   lpf = &gszOFName[0];
   HANDLE   h;
   if( fApd )
   {
      LARGE_INTEGER  li;
      h = Open_A_File( lpf);
      if( !VFH(h) )
         goto Do_Create;
      li.QuadPart = MyFileSeek( h, 0, FILE_END );
      if( li.HighPart )
      {
         // one day fix this
         bRet = TRUE;
         CloseHandle(h);
      }
      else
      {
         Out_Appe = li.LowPart;
         gOut_Hand = h;
      }
   }
   else
   {
Do_Create:
      h = Creat_A_File( lpf );
      if( !VFH(h) )
      {
         bRet = TRUE;
      }
      else
      {
         gOut_Hand = h;
      }
   }
   return bRet;
}


//; ==============================================================
//; Init Uninitialised Data Segment
VOID  Do_Init( VOID )
{
//	; Items to INIT'ed to ZERO
//	; ========================
   Out_FSiz = 0;  // 1 	Mov	Out_FSiz2,Ax
#if	AddSplit
   Out_TFS = 0;  //	Mov	Out_TFS2,Ax
#endif   //		; AddSplit
   Out_Appe = 0;  // 1 Mov	Out_Appe2,Ax
#if	AddDS
   g_dwDel_CC = 0; // Set as ZERO
	g_dwOff_CC = 0;
#endif	//; AddDS
//	; Other items it initialise
//	; =========================
//	Inc	Ax
   ghStdOut = GetStdHandle( STD_OUTPUT_HANDLE );   // Standard out
   ghErrOut = GetStdHandle( STD_ERROR_HANDLE  );   // error out
   if( VFH(ghStdOut) )
   {
      if( !GetConsoleMode( ghStdOut, &gdwMode ) )
         gbRedON = TRUE;
   }

//	Mov	Ax,MXLINE	; Get DEFAULT line length
	g_dwMaxLine = MXLINE; // Set default LINE length
//	Out_One = Buf_One;   // Default OUTPUT
//	Ret
   g_dwTab_Size = 3;  // Default TAB size

   //g_dwSp_Sz = 250000;   // 1	dw	0d090h Sp_Sz2	dw	3	; 250,000 bytes
   g_dwSp_Sz = 2000000;   // 1	dw	0d090h Sp_Sz2	dw	3	; 2,000,000 bytes

   g_dwFullDir = bf_SortList | bf_FullDir2 | bf_TryHard | bf_DateSort; // set defaults

}
// ; ==============================================================

// allocate and SET work memory -    Setup();    // intial SETUP
// ============================
BOOL  Setup( VOID )
{
   BOOL  bRet = FALSE;
	gpW = MALLOC( sizeof(WRK) );
	if( gpW )
	{
		ZeroMemory( gpW, sizeof(WRK) );
      DiagOpen();
      sprtf( "Begin FixF32 with %d bytes of work memory."MEOR, sizeof(WRK) );
      gdwSize = sizeof(WRK);  //  W.ws_dwSize    // ALLOCATION SIZE

//		gnMaxCol  = MAXCOL;
//		gnMinCol  = MINCOL;
		giVerbose = DEF_VERB;

      // -g options
      gbDoExtra   = FALSE;  // x
      gbChkPWD    = TRUE;    // p[0|1|+|-]
      gbIgnoreMNU = TRUE;  // i // ignore 123ABC
      gbUseSee    = FALSE; // s // add see (primary entry)
      gdwOutSz    = MXOUTBUF;    // DWORD
      glpOut      = &gszOutBuf[0];  // glpOut // g LPTSTR
      glpOut2     = &gszOutBuf2[0]; // glpOut2

      g_dwMaxLine = MXLINE; // set DEFAULT line length
      g_dwHexLen  = 1;

      InitLList( &gsFileList );  // initialise the INPUT file list
      InitLList( &gsExclList );  // -x exclude list
      InitLList( &gsRemList  );   // -R["... ..."] remove LINE list
      InitLList( &gsRemText  );   // -r["... ..."] remove TEXT list
      g_iRemText = 0;
      InitLList( &gsReport   );   // diagnostic report items
      InitLList( &gsFndRpl   );  // -hf:find:hr:replace:

      g_iKeepRaw = gf_KeepHi;

      Do_Init();

      if( VFH(ghStdOut) )
         bRet = TRUE;   // we didn't get STANDARD OUT handle!!!!!!!

	}
   if( !bRet )
      exit( (INT)-1 );

	return bRet;
}

void	CleanUp( void )
{

   DiagClose();
   if( gpW )
   {
      KillLList( &gsFileList );  // free the file list (if any)
      KillLList( &gsExclList );  // free the exclude list (if any)
      KillLList( &gsRemList  );   // free the remove LINE list (if any)
      KillLList( &gsRemText  );   // free the remove TEXT list (if any)
      KillLList( &gsReport   );   // free the report list (if any)
      KillLList( &gsFndRpl   );  // -hf:find:hr:replace:

      {
         if( gpLineBuf1 )
            MFREE(gpLineBuf1);
         if( gpLineBuf2 )
            MFREE(gpLineBuf2);
      }

      // finally FREE the main memory block
      MFREE(gpW);
   }
   gpW = 0;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Pgm_Exit
// Return type: VOID 
// Argument   : INT iRet
// Description: Single sole exit to application
//              
///////////////////////////////////////////////////////////////////////////////
VOID  Pgm_Exit( INT iRet )
{
   End_O_File();      // Ensure the OUT FILE (if any) CLOSED
	CleanUp();
   exit( iRet );
}

VOID  ShowWL( VOID )
{
   PLE      pHead = &gsFileList;
   PLE      pNext;
   DWORD    dwc, dwx, dwf;
   PWL      pwl;
   LPTSTR   lpf, lpt;
   LARGE_INTEGER  li;
   LPTSTR   lpd = gszDecimal;

   dwc = dwx = dwf = 0;
   Traverse_List( pHead, pNext )
   {
      dwc++;   // total count
      pwl = (PWL)pNext;
      if( pwl->wl_dwFlag & wlf_Exclude )
         dwx++;
      else if( pwl->wl_dwType & IS_FILE )
         dwf++;
   }
   if( dwc )
   {
      if(dwx)
      {
         sprtf( "Of list of %d entries, minus %d excluded, have %d to process."MEOR,
            dwc, dwx, dwf );
      }
      else
      {
         sprtf( "Of list of %d entries, have %d to process."MEOR, dwc, dwf );
      }
      dwc = 0;
      Traverse_List( pHead, pNext )
      {
         dwc++;
         pwl = (PWL)pNext;
         //lpf = &pwl->wl_szFile[0];
         lpf = &pwl->wl_szFull[0];
         *lpd = 0;
         li.HighPart = pwl->wl_sFD.nFileSizeHigh;
         li.LowPart  = pwl->wl_sFD.nFileSizeLow;
         switch( pwl->wl_dwType )
         {
         case IS_FILE:
            lpt = "File";
            sprintf( lpd, "%I64u", li.QuadPart );
            break;
         case IS_FOLDER:
            lpt = "Folder";
            break;
         case IS_WILD:
            lpt = "Wild";
            break;
         default:
            lpt = "????";
            break;
         }
         sprtf( "%3d: %s %s bytes. (%s) %s."MEOR,
            dwc, lpf, lpd,
            lpt,
            ( pwl->wl_dwFlag & wlf_Exclude ? "EXCLUDED" : 
            ( pwl->wl_dwType & IS_FILE ? "Ok" : "Exclude" ) ) );
      }
      sprtf( "Will process %d files from above list."MEOR, gdwWrkCnt );
   }
}

DWORD ExpandList( PLE pHead )
{
   DWORD    dwc = 0;
   PLE      pNext;
   PWL      pwl;
   HANDLE   hFind;
   LPTSTR   lpf = &gszName[0];
   PWIN32_FIND_DATA  pfd = &gsFD;
   LPTSTR   lpn = &gszFolder[0];
   LARGE_INTEGER  li;

   Traverse_List( pHead, pNext )
   {
      pwl = (PWL)pNext;
      *lpf = 0;
      if( !( pwl->wl_dwFlag & wlf_DnExp ) )
      {
         if( pwl->wl_dwType & IS_WILD )
         {
            strcpy(lpf, &pwl->wl_szFull[0]);
         }
         else if( pwl->wl_dwType & IS_FOLDER )
         {
            strcpy(lpf, &pwl->wl_szFull[0]);
            strcat(lpf, "\\*.*" );
         }
         if( *lpf )
         {
            hFind = FindFirstFile( lpf, pfd );
            if( VFH(hFind) )
            {
               do
               {
                  //if( ( pfd->dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY ) &&
                  if( pfd->cFileName[0] != '.' )
                  {
                     gmGetFolder( lpn, lpf );
                     strcat(lpn, &pfd->cFileName[0] );
                     Got_FileC( lpn );
                  }
               } while( FindNextFile( hFind, pfd ) );
            }
            FindClose( hFind );
         }
         pwl->wl_dwFlag |= wlf_DnExp;
      }
   }

   Traverse_List( pHead, pNext )
   {
      pwl = (PWL)pNext;
      if( pwl->wl_dwType & IS_FILE )
      {
         lpf = &pwl->wl_szFull[0];
         li.HighPart = pwl->wl_sFD.nFileSizeHigh;
         li.LowPart  = pwl->wl_sFD.nFileSizeLow;
         if( li.QuadPart )
         {
            if( li.HighPart )
            {
               pwl->wl_dwFlag |= (wlf_Exclude & wlf_TooBig);
            }
            else
            {
               if( !InXList( lpf ) )
               {
                  dwc++;
               }
               else
               {
                  pwl->wl_dwFlag |= wlf_Exclude;
               }
            }
         }
         else
         {
               pwl->wl_dwFlag |= (wlf_Exclude & wlf_Null);
         }
      }
   }
   return dwc;
}

BOOL  ChangeOUTFile( VOID )
{
   HANDLE   h;
   LPTSTR   lpn, lpc, lpp;  // = GetNxtBuf();   // get a buffer
   INT      i;
   char *   cp;
   LPTSTR   lpf = &gszOFName[0];
   lpc = GetNxtBuf();
   lpp = GetNxtBuf();
   strcpy(lpc,lpf);
   cp  = strrchr( lpc, '\\' );
   if(cp)
   {
      *cp = 0; // kill last folder ...
      strcpy(lpp,lpc);
      cp++;
   }
   else
   {
      cp = lpf;
      *lpp = 0;
   }
   lpn = GetNxtBuf();   // get a buffer
   strcpy(lpn,cp);
   BumpFileName(lpn);   // bump a file name  number before the ".", or at end ...
   *lpc = 0;
   if( *lpp )
   {
      strcpy(lpc,lpp);
      strcat(lpc,"\\");
   }
   strcat(lpc,lpn);
   if( Do_File_Create( lpc, &h ) )
   {
      // have the NEW handle
      if( VFH(gOut_Hand) )    // get active OUTPUT handle
         CloseHandle( gOut_Hand );
      gOut_Hand = h;
      strcpy(lpf,lpc);  // update OUTPUT name ...
      return TRUE;
   }


   i = 0;
   return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : outit
// Return type: BOOL 
// Arguments  : LPTSTR lpb
//            : DWORD dwLen
// Description: Ouput to either the open file, or standard out, even if
//              stdout could be re-directed.
///////////////////////////////////////////////////////////////////////////////
BOOL  outit( LPTSTR lpb, DWORD dwLen )
{
   static BOOL _s_sptfld = FALSE;
   DWORD    dww;
   HANDLE   h;
   BOOL     bRet;

   h = gOut_Hand;    // get active OUTPUT handle
   if( VFH(h) )
   {
      bRet = WriteFile(h, lpb, dwLen, &dww, NULL);
      gdwWritten += dwLen;
      if(( Pgm_Flag & Do_Split ) &&   // we have at least -s
         ( g_dwSp_Sz           ) )
      {
         if( gdwWritten > g_dwSp_Sz )
         {
            if( ChangeOUTFile() )   // if success
            {
               gdwTotWrite += gdwWritten; // accumulate TOTAL written out
               gliTotWrite.QuadPart += gdwWritten; // 64-bit accumulation
               gdwWritten = 0;
            }
            else
            {
               _s_sptfld = TRUE;
               Pgm_Flag &= ~( Do_Split ); // *** REMOVE THE FLAG *** no panic
            }
         }
      }
   }
   else  // if( !VFH(h) )
   {
      h = ghStdOut;  // if NONE, use console standard out
      bRet = WriteFile(h, lpb, dwLen, &dww, NULL);
      gdwWritten += dwLen;
   }


   if( ( dwLen > 0     ) &&
       ( VERB9         ) &&
       ( h != ghStdOut ) )
   {
      PTSTR p = strchr(lpb,'%');
      if(p) {
         // use alternate OUTPUT
         // extern void  DO(PTSTR a);
         DO( lpb );
         if( lpb[dwLen - 1] >= ' ' )
            DO( MEOR );

      } else {
         if( lpb[dwLen - 1] >= ' ' ) {
            sprtf( "%s"MEOR, lpb );
         } else {
            sprtf(lpb);
         }
      }
   }

   return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Chk_Dupes
// Return type: BOOL 
// Arguments  : LPTSTR lpout
//            : PDWORD pdw
// Description: Have just added another 0x0a (LF) to the output
//              Check backwards and remove any duplicates
// Return FALSE if this last pair, and any spaces were removed,
// else return TRUE to count another line.
///////////////////////////////////////////////////////////////////////////////
BOOL  Chk_Dupes( LPTSTR lpout, PDWORD pdw, BOOL bCrLf )
{
   BOOL  bRet = TRUE;
   DWORD dwi = *pdw;
   DWORD dwc = 0;
   INT   c;
   if(dwi)
      dwi--;
   while( dwi != (DWORD)-1 )
   {
      c = lpout[dwi];
      if( c > ' ' )
         break;
      dwi--;
      dwc++;
   }
   if( ( bCrLf ) && ( dwi == (DWORD)-1 ) && (dwc >= 2) )
   {
      dwi++;   // back to ZERO
      dwc = *pdw;
      gDiscard += (dwc - dwi);   // keep count of discards
      *pdw = dwi;    // set reduced length of buffer
      bRet = FALSE;  // do NOT count a NEW line
   }
   else if( dwc >= 4 )
   {
      dwi++;   // get to first CR
      lpout[dwi++] = 0x0d;
      lpout[dwi++] = 0x0a;
      dwc = *pdw;
      gDiscard += (dwc - dwi);   // keep count of discards
      *pdw = dwi;    // set reduced length of buffer
      bRet = FALSE;  // do NOT count a NEW line
   }

   return bRet;
}

void  Delete_Block( PTSTR ps, DWORD bgn, DWORD len, DWORD max )
{
   DWORD dwi;
   PTSTR pt1; // = &ps[dwi];
   PTSTR pt2; // = &ps[len+dwi];
   for( dwi = bgn; (dwi+len) < max; dwi++ ) {
      pt1 = &ps[dwi];
      pt2 = &ps[len+dwi];
      // ps[dwi] = ps[len+dwi];
      *pt1 = *pt2;
   }
   pt1 = &ps[dwi];
   *pt1 = 0;
}


BOOL  Chk_Remove( LPTSTR lpout, LPTSTR lpbgn, PDWORD pdw, DWORD dwLine )
{
   BOOL  bRet = FALSE;  // assume NO removal
   DWORD dwo = *pdw;    // get output length
   DWORD dwl;

   dwl = ( lpbgn - lpout );   // get length prior to this beginning
   if( dwo > dwl )
   {
      dwl = dwo - dwl;
      if( InRList( lpbgn, dwl ) )
      {
         // remove this line from the output
         dwo -= dwl;       // reduce output size accordingly
         *pdw = dwo;       // set new length of output
         gDiscard += dwl;  // accumuate removals
         bRet = TRUE;      // have removed a line
      } else if( g_iRemText ) {
         PRL prl = InrList( lpbgn, dwl );
#ifdef   EXTRA_DEBUG
         if(( lpbgn[0] == 'd' ) && !prl ) {
            PLE   ph = &gsRemText;
            PLE   pn;
            PTSTR lpx;
            sprtf( MEOR"FAILED CHECKING: %s", lpbgn);
            Traverse_List( ph, pn ) {
               prl = (PRL)pn;
               lpx = &prl->rl_szFile[0];
               sprtf( "FAILED: %s (len=%d)"MEOR, lpx, prl->rl_dwLen ); 
            }
            prl = 0;
         }
#endif // #ifdef   EXTRA_DEBUG
         while(prl) {
            PTSTR   lpx, lpf;
            lpx = &prl->rl_szFile[0];
            lpf = strstr( lpbgn, lpx );  // get instance of remove line in file line
            if(lpf) {
               if( ((lpf - lpbgn) + prl->rl_dwLen) < dwl ) {
                  Delete_Block( lpbgn, (lpf - lpbgn), prl->rl_dwLen, dwl ); // get offset to beginning
                  dwo -= prl->rl_dwLen;
                  dwl -= prl->rl_dwLen;
                  *pdw = dwo; // set new length of output
                  gDiscard += prl->rl_dwLen; // and accumulate removals
#ifndef  NDEBUG
                  if( VERB5 ) {
                  AddReport( "Discarded text from line %d %d chars."MEOR,
                     dwLine, prl->rl_dwLen );
                  }
#endif   // !NDEBUG
               }
            }
            prl = InrList( lpbgn, dwl );
         }
      }
   } else {
      sprtf("WARNING: Line %d: Ouput length %u NOT greater last length %u!"MEOR,
         dwLine, dwo, dwl ); 
   }
#ifdef  NDEBUG
   UNREFERENCED_PARAMETER(dwLine);
#endif // #ifdef  NDEBUG
   return bRet;
}

#define  ISUHEX(a)   ( ISNUM(a) || ( ( a >= 'A' ) && ( a <= 'F' ) ) )

VOID  stophere(VOID)
{
   int i;
   i = 0;
}

VOID  Show_Hex2Ascii( LPTSTR lpb, DWORD dws )
{
   DWORD    dwi, dwl, dwb, dwii, dwo, dwb2, dwh1, dwh2, dwl2;
   INT      c, d;
   LPTSTR   lpout = &gszOutBuf[0];
   LPTSTR   lpo2  = &gszOutBuf2[0];
   LPTSTR   ptmp, cp;
   BOOL     bDnHdr = FALSE;
   DWORD    dwhl = g_dwHexLen;

   // post processing, looking for HEX to show the ASCII
   dwl2 = dwh1 = dwb2 = dwo = dwb = dwl = 0;
   ptmp = lpb;
   if( dwhl == 0 )
      dwhl = 1;

   for( dwi = 0; dwi < dws; dwi++ )
   {
      c = lpb[dwi];  // extract a CHAR
      if( ( c == 0x0d ) || ( c == 0x0a ) )
      {
         // we have the END of this line
         dwi++;
         for( ; dwi < dws; dwi++ )
         {
            c = lpb[dwi];  // extract a CHAR
            if( c >= ' ' )
            {
               dwi--;   // back up one to let the loop do the increment
               break;
            }
         }

//         if( InStr(ptmp, "\"Ntlanman") == 1 )
//            stophere();

         // have reached END of BUFFER, or BEGIN NEXT LINE
         for( dwii = dwb; dwii <= dwi; dwii++ )
         {
            cp = &lpb[dwii];
            c = toupper(lpb[dwii]);  // extract a CHAR
            if( ISUHEX(c) && ( (dwii+1) <= dwi ) )
            {
               dwh2 = dwii;      // begin of HEX
               d = ( c - '0' );
               if( d > 9 )
                  d -= (( 'A' - 1 ) - '9' );    // get value
               dwii++;  // bump to next
               cp = &lpb[dwii];
               c = toupper(lpb[dwii]);  // extract a CHAR
               while( ISUHEX(c) )
               {
                  if( ISNUM(c) )
                     d = ( d << 4 ) + ( c - '0' );
                  else
                     d = ( d << 4 ) + ( c - ('0' + (('A' - 1) - '9')) );
                  dwii++;  // bump to NEXT
                  if( dwii >= dwi )
                     break;
                  cp = &lpb[dwii];
                  c = toupper(lpb[dwii]);  // extract a CHAR
               }
               if( ( d >= ' ' ) && ( d < 0x7f ) )
               {
                  // we have an ASCII - get the output
                  lpout[dwo++] = (TCHAR)d;
                  lpout[dwo] = 0;
                  if( dwh1 == 0 )
                     dwh1 = dwh2;   // keep BEGIN of possible HEX data
               }
            }
         }

         //if( dwo > 1 )
         if( dwo > dwhl )  // -h2a[nn] switch
         {
            // we have gathered some HEX, so first OUT the line itself
            // from dwb2, the starting offset
            // dwl is general line length of significant data
            if( !bDnHdr )
            {
               strcpy(lpo2, "Display of HEX to ASCII follows"MEOR);
               outit(lpo2,strlen(lpo2));
               bDnHdr = TRUE;
            }
            if( dwl < MXOUTB )
               dwii = dwl;
            else
               dwii = MXOUTB;
            strncpy( lpo2, ptmp, dwii );
            lpo2[dwii] = 0;
            strcat(lpo2,MEOR);
            outit(lpo2,strlen(lpo2));
            if(dwh1)
            {
               dwii = dwh1 - dwb;   // get length BEFORE first HEX
               if(dwii)
               {
                  *lpo2 = 0;
                  while(dwii--)
                  {
                     strcat(lpo2," ");
                  }
                  outit(lpo2,strlen(lpo2));
               }
            }
            strcat(lpout,MEOR);
            outit(lpout,strlen(lpout));   // and bang it out
            dwl2++;
         }

         dwo = 0;
         dwb = dwi + 1;  // keep this beginning
         dwb2 = dwb;
         ptmp = &lpb[dwb];
         dwh1 = 0;
         dwl  = 0;
         dwl--;         // ensure begins ZERO
      }
      dwl++;
   }
   if( bDnHdr )
   {
      sprintf(lpout, "Done %d line(s) of HEX to ASCII"MEOR, dwl2 );
      outit(lpout,strlen(lpout));   // and bang it out
   }
}


DWORD  OutTheLine( LPTSTR lpbgn, LPTSTR lpout, DWORD dwlen, PDWORD pdwl, DWORD dwl2,
                  PBOOL pbDnCrLf )
{
   DWORD dwo     = dwlen;
   DWORD dwl     = *pdwl;
   BOOL  bDnCrLf = *pbDnCrLf;
   BOOL  bOutNow = FALSE;
   lpout[dwo] = 0;   // zero terminate the output

   if( dwo && ( Pgm_Flag & Do_Unix ) && gbUnifDiff ) {
      // convert unified Diff header lines to '/' from '\'
      bOutNow = TRUE;   // no storage when this is ON - bang it out NOW
      if(( strchr( lpout, '\\' ) )&&
         ((*lpout == 'd')||(*lpout == '+')||(*lpout == '-')))
      {
         if(( _strnicmp(lpout, "diff ", 5) == 0 )||
            ( _strnicmp(lpout, "+++ ", 4 ) == 0 )||
            ( _strnicmp(lpout, "--- ", 4 ) == 0 ))
         {
            for( dwo = 0; dwo < dwlen; dwo++ )
            {
               if( lpout[dwo] == '\\' ) {
                  lpout[dwo] = '/';
                  gConverted++; // mark a CONVERTED character
               }
            }
            dwo = dwlen;
         }
      }
   }
   if( Pgm_Flag & Rem_CRLF )  // check if we are REMOVING duplicated Cr/Lf
   {
#ifndef  NDEBUG
      DWORD    dwpd1 = gDiscard;
#endif   // !NDEBUG
      if( Chk_Dupes( lpout, &dwo, bDnCrLf ) )
      {
         dwl++;
      }
      else
      {
         lpout[dwo] = 0;
#ifndef  NDEBUG
         if( VERB5 ) {
             AddReport( "Discarded duplicate CR/LF of line %d (%d) = %d chars."MEOR,
                     dwl, dwl2,
                     (gDiscard - dwpd1) );
         }
#endif   // !NDEBUG
      }
   }
   else
   {
      dwl++;
   }

   if(( dwo               ) &&    // if we HAVE some output AND
      (( Pgm_Flag & Gt_Rem ) || g_iRemText) )     // flag of SOME removals
   {
#ifndef  NDEBUG
      DWORD    dwpd2 = gDiscard;
#endif   // !NDEBUG
      if( Chk_Remove( lpout, lpbgn, &dwo, dwl ) )
      {
         if(dwl)
            dwl--;   // just lost a line
#ifndef  NDEBUG
         if( VERB5 ) {
             AddReport( "Discarded line %d (%d) of %d chars."MEOR,
             dwl, dwl2,
             (gDiscard - dwpd2) );
          }
#endif   // !NDEBUG
       }
    }

    if(( dwo && ( dwo >= MXOUTIT ) )|| // if we have reached OUTPUT time
       ( bOutNow ))  // or we have MODIFIED the ouput above, then OUT IT NOW
    {
       BOOL bRet;
       lpout[dwo] = 0;      // zero terminate the output
       bRet = outit(lpout,dwo);   // and bang it out
       dwo = 0;             // re-start output
       bDnCrLf = TRUE;      // flag some output done
       if( !bRet )          // if output FAILED
          Pgm_Exit(-1); // = exit(-1);
       //    break;   // then let's get out of here
    }

   *pdwl     = dwl;
   *pbDnCrLf = bDnCrLf;
#ifdef  NDEBUG
   UNREFERENCED_PARAMETER(dwl2);
#endif   // NDEBUG

   return dwo;

}

#define  KeepHi  ( g_iKeepRaw & gf_KeepHi ) 
#define  KeepLo  ( g_iKeepRaw & gf_KeepLo ) 

BOOL  GotDebug( PTSTR pout, DWORD len )
{
   DWORD i;
   for( i = 0; i < len; i++ )
   {
      if( pout[i] == 'M' ) {
         if( ( i + 3 ) < len ) {
            if(( pout[i+1] == 'i' ) &&
               ( pout[i+2] == 's' ) &&
               ( pout[i+3] == 'e' ) ) {
               return TRUE;
            }
         } else {
            return FALSE;
         }
      }
   }

   return FALSE;
}

#define got_zero 0x00000001
#define got_high 0x00000002
#define got_tab  0x00000004
#define got_asc  0x00000008
#define got_ctl  0x00000010
#define got_ctl1 0x00000020

BOOL do_ascii( char * cp, int len )
{
   BOOL bRet = TRUE;
   char * lpout = gszOutBuf;
   int c, dwj, dwo;
   dwo = 0;
   if( len > 10 ) {
      for( dwj = 0; dwj < len; dwj++ ) {
         c = cp[dwj];
         if( c < ' ' )
            c = ' ';
         lpout[dwo++] = (TCHAR)c;
      }
      lpout[dwo++] = '\r';
      lpout[dwo++] = '\n';
      lpout[dwo] = 0;
      bRet = outit(lpout,dwo);   // and bang it out
   }
   return bRet;
}

BOOL ascii_set( char * lpb, int dwi, int dwba )
{
   char * cp = &lpb[dwba]; // view the ASCII
   int len = dwi - dwba; // from begin of ASCII
   return( do_ascii( cp, len ) );
}

#define clear_line_count   if(ascii) ascii_set(lpb,dwi,dwba); \
      ascii = acnt = scnt = 0



BOOL Do_Bin_Buffer( LPTSTR lpb, DWORD dws )
{
   BOOL bRet = TRUE;
   DWORD dwi, dwb, lf, bal, dwc, len, dwba;
   int acnt, scnt, ascii;
   int c;

   dwb = dwi = dwc = 0;
   lf = 0;
   bal = dws;
   while(bal) { // stay while chars
      // get to END OF LINE (or buffer)
      ascii = acnt = scnt = 0; // clear line count
      lf = 0;
      dwba = dwb = dwi;
      for( ; dwi < dws; dwi++ )
      {
         c = lpb[dwi];  // extract a CHAR
         dwc++; // count a fetch
         if(c) {
            if( c & 0x80 ) {
               // high bit
               lf |= got_high;
               clear_line_count;
            } else {
               if( c < ' ' ) {
                  if(( c == '\r' ) || ( c == '\n' )) {
                     dwi++; // bump past it
                     c = lpb[dwi];  // extract a CHAR
                     if(( c == '\r' ) || ( c == '\n' )) {
                        dwi++;
                     }
                     // end of line
                     break;
                  } else {
                     if( c == '\t' ) {
                        lf |= got_tab;
                     } else {
                        if( c == 1 ) {
                           lf |= got_ctl1;
                           clear_line_count;
                        }
                        lf |= got_ctl;
                     }
                  }
               } else {
                  if(ascii == 0) {
                     dwba = dwi; // begin of ASCII
                  }
                  lf |= got_asc;
                  ascii++;
                  if( c > ' ' ) {
                     acnt++; // sig ascii count
                  } else {
                     scnt++;
                  }
               }
            }
         } else {
           lf |= got_zero;
           clear_line_count;
         }
      }

      // at end of LINE - see what we GOT
      if(ascii) {
         bRet = ascii_set( lpb, dwi, dwba );
      }

      // ***************************************
      len = dwi - dwb; // get length reviewed
      bal -= len; // remove length
      // ***************************************
   }

   return bRet;
}

DWORD AddCharacter( PTSTR lpout, DWORD dwo, TCHAR c, TCHAR n )
{
   if( g_bTags2NL && ( c == '<' ) && ( n != '/') ) {
      BOOL dounix = (Pgm_Flag & Do_Unix);
      if( dounix ) {
         if( dwo ) {
            if( lpout[dwo - 1] != '\n' )
            {
               lpout[dwo++] = '\n';
               gAdded++;  // and additions
            }
         } else {
            lpout[dwo++] = '\n';
            gAdded++;  // and additions
        }
      } else {
         // seek CR/LF pair
         if( dwo ) {
            if( lpout[dwo - 1] != '\n' )
            {
               lpout[dwo++] = '\r';
               lpout[dwo++] = '\n';
               gAdded++;  // and additions
               gAdded++;  // and additions
            }
         } else {
            lpout[dwo++] = '\r';
            lpout[dwo++] = '\n';
            gAdded++;  // and additions
            gAdded++;  // and additions
         }
      }
   }
   lpout[dwo++] = c;
   return dwo;
}

// FIX20120327 - potentially DELETE any BOM chars sequences
// Set by -B option, AND by -ud (converting of UNIX diff files)
// # BOM Representations of byte order marks by encoding
typedef struct sBOMSets {
    char *name;
    DWORD  len;
    BYTE seq[4];
}BOMSets, *PBOMSets;

BOMSets bomsets[] = {
    { "UTF-8",       3, {0xEF, 0xBB, 0xBF , 0   }}, // 1 239 187 191   
    { "UTF-16 (BE)", 2, {0xFE, 0xFF, 0,     0   }}, // 2 254 255
    { "UTF-16 (LE)", 2, {0xFF, 0xFE, 0,     0   }}, // 3 255 254    
    { "UTF-32 (BE)", 4, {   0,    0, 0xFE, 0xFF }}, // 4  0 0 254 255
    { "UTF-32 (LE)", 4, {0xFF, 0xFE, 0x00, 0    }}, // 5  255 254 0 0
    { "UTF-7",       4, {0x2B, 0x2F, 0x76, 0x38 }}, // 6  2B 2F 76 39  2B 2F 76 2B  2B 2F 76 2F
    { "UTF-1",       3, {0xF7, 0x64, 0x4C, 0    }}, // 7 247 100 76 
    { "UTF-EBCDIC",  4, {0xDD, 0x73, 0x66, 0x73 }}, // 8 221 115 102 115
    { "SCSU",        3, {0x0E, 0xFE, 0xFF, 0    }}, // 9 14 254 255
    { "BOCU-1",      3, {0xFB, 0xEE, 0x28, 0    }}, //10 251 238 40
    { "GB-18030",    4, {0x84, 0x31, 0x95, 0x33 }}, //11 132 49 149 51
    { 0,             0, {0,    0,    0,    0    }}
};

static BOOL done_BOM_Init = FALSE;
static BYTE    BOM_byts[16];
static DWORD   BOM_bcnt = 0;
VOID do_BOM_Init(VOID)
{
    PBOMSets pbs = &bomsets[0];
    DWORD dwoff = 1;
    BYTE b;
    DWORD dwi;
    BOM_bcnt = 0;
    while (pbs->name) {
        b = pbs->seq[0];
        dwoff = 1;
        for (dwi = 0; dwi < BOM_bcnt; dwi++) {
            if (BOM_byts[dwi] == b) {
                dwoff = 0;
                break;
            }
        }
        if (dwoff)
            BOM_byts[BOM_bcnt++] = b;
        pbs++;
    }
    done_BOM_Init = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : is_BOM
///////////////////////////////////////////////////////////////////////////////
BOOL is_BOM( DWORD c, LPTSTR lpb, DWORD *poffset, DWORD dws )
{
    BYTE b = (BYTE)c;
    DWORD dwi = *poffset;
    DWORD dwj, skip, len, set;
    PBOMSets pbs;
    BYTE  *bp = (BYTE *)lpb;
    if (!done_BOM_Init) {
        done_BOM_Init = TRUE;
        do_BOM_Init();
    }
    skip = 0;
    len = 1;
    for (dwj = 0; dwj < BOM_bcnt; dwj++) {
        if (BOM_byts[dwj] == b) { // got a FIRST byte - check for BOM sequence
            pbs = &bomsets[0];
            skip = 0;
            while (pbs->len) {
                if (pbs->seq[0] == b) {
                    len = pbs->len;
                    if ((dwi + len) < dws) {
                        for (set = 1; set < len; set++) {
                            if (bp[dwi+set] != pbs->seq[set])
                                break;
                        }
                        if (set == len) {
                            // skip this sequence
                            skip = 1;
                        }
                    }
                }
                if (skip) {
                    dwi += (len - 1);
                    *poffset = dwi;   // set the NEW offset
                    gDiscard += len;  // accumulate this files discards
                    gTotDisc += len;  // accumulate total dicards
                    if (VERB1) {
                        AddReport( "Discarded %s BOM, %d bytes."MEOR,
                            pbs->name, len );
                    }
                    return TRUE;  // got the sequence and length of sequence
                }
                pbs++;
            }   // while checking the exact BOM sequence
        } // first byte equal
    }   // for the count set
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Do_Buffer
// Return type: BOOL 
// Arguments  : LPTSTR lpb
//            : DWORD dws
// Description: Simple buffer processing of the mapped view of the file.
//              
///////////////////////////////////////////////////////////////////////////////
BOOL  Do_Buffer( LPTSTR lpb, DWORD dws )
{
   BOOL     bRet = TRUE;
   DWORD    dwi, dwo, dwi2;
   DWORD    c, d, n;
   LPTSTR   lpout = &gszOutBuf[0];
   DWORD    dwa, dwd, dwl, dwl2, dwsc, dwtil;
   BOOL     bDnCrLf = TRUE;
   LPTSTR   lpbgn;   // beginning of line
   DWORD    dwll;    // current line length
   DWORD    dwml = g_dwMaxLine;  // extract the MAXIMUM line length
//   INT      giKeep = g_iKeepRaw; // extract the keep raw flag
   DWORD    dwoff = 0;
   DWORD    dwhalf;
   BOOL     dounix = (Pgm_Flag & Do_Unix);   // get the TO UNIX FLAG

   dwhalf = dwml / 2;
   gDiscard = 0;
   gAdded   = 0;
   gdwWritten = 0;

   c = d = 0;
   dwo = dwa = dwd = dwl = dwl2 = 0;
   dwsc = 0;   // significant chacter count in buffer
   dwtil = 0;  // tabs in this LINE
   lpbgn = lpout;    // begin is beginning of output
   dwll = 0;         // current line length is ZERO
   // run thru the mapped file buffer
   // ********************************************************
   for( dwi = 0; dwi < dws; dwi++ )
   {
      c = (lpb[dwi] & 0xff);  // extract a CHAR
      dwi2 = dwi + 1;
      if(dwi2 < dws)
         n = (lpb[dwi2] & 0xff);  // extract NEXT CHAR
      else
         n = 0;
      if( gbDelBOM ) {
          if (is_BOM( c, lpb, &dwi, dws )) {
              continue;
          }
      }
      if( c == 0x0a )
      {
         // we have a LF. Check if previous was a CR
         dwl2++;  // COUNT a FILE buffer LINE
         // ****************************************
         if( d != 0x0d )   // if previous NOT!!!!
         {
            // if NOT, add a CR now
            if( !dounix ) {
               lpout[dwo++] = 0x0d;
               dwa++;   // and bump ADDED count
#ifndef  NDEBUG
               if( VERB5 ) {
                  AddReport( "Added CR (0x0d) to line %d (%d)."MEOR, dwl, dwl2 );
               }
#endif   // !NDEBUG
            }
         }
         // else all is well - add this char after the CR
      }
      else if( c == 0x0d ) // starting with CR ...
      {
         // we have a CR. If more in buffer, check NEXT is a LF
         if( (dwi + 1) < dws )
         {
            // we have MORE, is it a LF
            if( lpb[ dwi + 1 ] != 0x0a )
            {
               // NO
               // FIX20050205 - watch out for 0x0d,0x0d,0x0a
               if( ( (dwi + 2) < dws        ) &&
                   ( lpb[ dwi + 1 ] == 0x0d ) &&
                   ( lpb[ dwi + 2 ] == 0x0a ) ) {
                  // YES, just toss this FIRST CR
                  gDiscard++;
                  d = c;   // keep previous
#ifndef  NDEBUG
                  if( VERB5 ) {
                     AddReport( "Discarded CR (0x0d) of 0D 0D 0A triplet - line %d (%d)."MEOR, dwl, dwl2 );
                  }
#endif   // !NDEBUG
                  continue; // go for next char ...
               } else {
                  // NO, so add the CR
                  if( dounix ) {
                     gConverted++; // mark a CONVERTED character
                     c = 0x0a;   // switch c to a LF
#ifndef  NDEBUG
                     if( VERB5 ) {
                        AddReport( "Converted CR (0x0d) to LF line %d (%d)."MEOR, dwl, dwl2 );
                     }
#endif   // !NDEBUG
                  } else { // NOT unix
                     lpout[dwo++] = (TCHAR)c;
                     c = 0x0a;   // switch c to a LF
                     dwa++;      // and bump ADDED count
#ifndef  NDEBUG
                     if( VERB5 ) {
                        AddReport( "Added LF (0x0a) to line %d (%d)."MEOR, dwl, dwl2 );
                     }
#endif   // !NDEBUG
                  }
               }
            } // if NOT 0D 0A pair
         }
         else
         {
            // this CR is LAST in buffer
            if( dounix ) {
               gConverted++; // mark a CONVERTED character
               c = 0x0a;   // switch c to a LF
#ifndef  NDEBUG
               if( VERB5 ) {
                  AddReport( "Converted CR (0x0d) to LF line %d (%d)."MEOR, dwl, dwl2 );
               }
#endif   // !NDEBUG
            } else {
               lpout[dwo++] = (TCHAR)c;   // so add it
               c = 0x0a;   // switch c to a LF
               dwa++;      // bump the ADDED count
#ifndef  NDEBUG
               if( VERB5 ) {
                  AddReport( "Added LF (0x0a) to line %d (%d)."MEOR, dwl, dwl2 );
               }
#endif   // !NDEBUG
            }
         }

         // we have either ADDED this CR, and changed to a LF
         // or are letting the CR fall through because it is
         // DEFINITELY followed by a LF
      }  // having dealt with CR and LF
      else  // it is NOT one of those chars CR nor LF
      {
         // deal with HIGH bit characters
// FIX20051205 - add delete hi-bit characters - only ASCII output
//	dmsg(	" -dh[s]     Delete hi-bit characters -dhs substitute a space."MEOR );
         if (( g_DelHiBit ) && ( c & 0x80 ) ) {
            if( g_SubSpace ) {
               c = ' '; // set it as a SPACE character
               gConverted++; // mark a CONVERTED character
            } else {
               gDiscard++;
               d = c;   // keep previous
#ifndef  NDEBUG
               if( VERB5 ) {
                     AddReport( "Discarded Hi-bit character - line %d (%d)."MEOR, dwl, dwl2 );
               }
#endif   // !NDEBUG
               continue; // go for next char ...
            }
         }
         if( c > ' ' )
            dwsc++;  // count SIGNIFICANT characters seens

         if( !KeepHi )
         {
            if( c & 0x80 )
            {
               lpout[dwo++] = '@';  // insert leader char
               c &= 0x7f;           // and reduce character
               dwa++;      // bump the ADDED count
               dwll++;  // bump this current lines LENGTH by one
#ifndef  NDEBUG
               if( VERB5 ) {
                  AddReport( "Added '@' before hi-bit to line %d (%d)."MEOR, dwl, dwl2 );
               }
#endif   // !NDEBUG
            }
         }
         if( !KeepLo ) {

//            if( ( c < ' ' ) && ( c != 't' ) ) - 27JAN2002 - think this should be
//            if( ( c < ' ' ) && ( c != '\t' ) )  // ie is LESS than SPACE, and NOT TAB
            if( c < ' ' ) // finally is LESS than SPACE
            {
               if( c == '\t' ) // if TAB
               {
                  // Description: Decode a -T TAB COMMAND BOOL  Get_Tab_Siz( LPTSTR lpc )
                  //   if( toupper( *lpc ) == 'S' )
                  dwtil++; // TABs encountered
                  if( Pgm_Flag & Tab_SP ) // Change TABS to spaces
                  {
                     INT   icnt = (((dwll / g_dwTab_Size) + 1) * g_dwTab_Size) - dwll;
                     c = ' ';
                     if(icnt)
                        icnt--;  // we are already poised to add ONE, so remove it
                     while(icnt) // and add any remainder to the end of TAB
                     {
                        lpout[dwo++] = (TCHAR)c;
                        dwa++;      // bump the ADDED count
                        dwll++;  // bump this current lines LENGTH by one
                        icnt--;
                     }
                  }
               }
               else  // NOT CR/LF/TAB - splot it out
               {
                  lpout[dwo++] = '^';
                  c |= '@';   // bump to ASCII
                  dwa++;      // bump the ADDED count
                  dwll++;  // bump this current lines LENGTH by one
#ifndef  NDEBUG
                  if( VERB5 ) {
                     AddReport( "Added '^' before control to line %d (%d)."MEOR, dwl, dwl2 );
                  }
#endif   // !NDEBUG
               }
            }

         }  // not giKeep

         dwll++;  // bump this current lines LENGTH by one

         if(( dwll > dwhalf ) && // ( dwll > dwml ) &&
            ( Pgm_Flag & Fix_LS ) )    // try to break the line only on a SPACE
         {
            if( c == '<' ) {
               if( !dounix ) {
                  lpout[dwo++] = 0x0d;
                  dwa++;
               }
               lpout[dwo++] = 0x0a;
               dwa++;
#ifndef  NDEBUG
               if( VERB5 ) {
                  AddReport( "Split line at %d adding %s Line=%d (%d) = 2 chars."MEOR,
                     dwll,
                     (dounix ? "Lf" : "Cr/Lf"),
                     dwl, dwl2 );
               }
#endif   // !NDEBUG
               dwo = OutTheLine( lpbgn, lpout, dwo, &dwl, dwl2, &bDnCrLf );
               lpbgn = &lpout[dwo];    // reset start of line
               dwll  = 1;              // and restart it length
            }
         }
      }  // not CR or LF character done


      // add the character to the output
      if( c == 0x0d ) {
         if( dounix ) {
            gDiscard++;
         } else {
            lpout[dwo++] = (TCHAR)c;
         }
      } else {
         dwo = AddCharacter( lpout, dwo, (TCHAR)c, (TCHAR)n );
         //lpout[dwo++] = (TCHAR)c;
      }
      //      if( GotDebug( lpout, dwo ) )
      //   sprtf("GotDebug"MEOR);

      if( c == 0x0a )
      {
         // ====================================================================
         // if it is the END OF A FILE LINE
         dwo = OutTheLine( lpbgn, lpout, dwo, &dwl, dwl2, &bDnCrLf );
         lpbgn = &lpout[dwo];    // reset start of line
         dwll  = 0;              // and restart it length
      }  // ====================================================================
      else if( c != 0x0d )
      {
         if( dwll > dwml )
         {
            if( Pgm_Flag & Fix_LS )    // try to break the line only on a SPACE
            {
               if( c <= ' ' )
               {
                  if( !dounix ) {
                     lpout[dwo++] = 0x0d;
                     dwa++;
                  }
                  lpout[dwo++] = 0x0a;
                  dwa++;
#ifndef  NDEBUG
                  if( VERB5 ) {
                     AddReport( "Split line at %d adding %s Line=%d (%d) = 2 chars."MEOR,
                        dwll,
                        (dounix ? "Lf" : "Cr/Lf"),
                        dwl, dwl2 );
                  }
#endif   // !NDEBUG
                  dwll = 0;   // restart current line
               }
            }
            else
            {
               if( dounix ) {
                  lpout[dwo++] = 0x0d;
                  dwa++;
               }
               lpout[dwo++] = 0x0a;
               dwa++;
#ifndef  NDEBUG
               if( VERB5 ) {
                  AddReport( "Split line at %d adding %s Line=%d (%d) = 2 chars."MEOR,
                        dwll,
                        (dounix ? "Lf" : "Cr/Lf"),
                        dwl, dwl2 );
                }
#endif   // !NDEBUG
                dwll = 0;   // restart current line
            }

            if( dwll == 0 )
            {
               // have CLOSED this file LINE, so get it out
               // if it is the END OF A FILE LINE
               dwo = OutTheLine( lpbgn, lpout, dwo, &dwl, dwl2, &bDnCrLf );
               lpbgn = &lpout[dwo];    // reset start of line
               dwll  = 0;              // and restart it length
            }
         }
      }

      if( c == '<' )
         dwoff = dwi;

      d = c;   // keep previous
      if( dwo >= MXOUTB )
      {
         // we should really keep the last line!
         // but we will face that problem later.
         chkme( "CHECK ME: How did we get %d bytes in output!?!"MEOR, dwo );
         lpout[dwo] = 0;
         bRet = outit(lpout,dwo);
         dwo = 0;
         bDnCrLf = FALSE;
         if( !bRet )
            break;
      }

   }  // for the SIZE of the BUFFER - whole mapped file now used
   // ********************************************************
   // DOES NOT PRESENTLY ACCOUNT FOR FILES GREATER THAN 0xffffffff
   // That is 4,294,967,295 bytes - MUST ONE DAY IMPROVE THIS
   // ********************************************************

   // if add eof message
   if( VERB1 && !gbNoEOF )   // add eof at end
   {
      if( c != 0x0a )
      {
         if( !dounix ) {
            lpout[dwo++] = 0x0d;
            dwa++;
         }
         lpout[dwo++] = 0x0a;
         dwa++;
#ifndef  NDEBUG
         if( VERB5 ) {
            AddReport( "Added %s to end line %d (%d) = 2 chars."MEOR,
               (dounix ? "Lf" : "Cr/Lf"),
               dwl, dwl2 );
         }
#endif   // !NDEBUG
      }
   
      for( dwi = 0; ; dwi++ )
      {
         c = End_Msg[dwi]; // "eof" plus CR,LF
         if( c == 0 )
            break;
         if( dounix ) {
            if( c != 0x0d ) {
               lpout[dwo++] = (TCHAR)c;
               dwa++;
            }
         } else {
            lpout[dwo++] = (TCHAR)c;
            dwa++;
         }
      }
#ifndef  NDEBUG
      if( VERB5 ) {
         AddReport( "Added eof message to end %d (%d) = %d chars."MEOR,
                  dwl, dwl2,
                  strlen( End_Msg ) );
      }
#endif   // !NDEBUG
   }

   // if some left in output buffer
   if( dwo )
   {
      lpout[dwo] = 0;            // zero terminate the stuff in there
      bRet = outit(lpout,dwo);   // bang it out
      dwo = 0;                   // and reset outpu
   }

   gDiscard += dwd;  // accumulate this files discards
   gAdded   += dwa;  // and additions

   gTotDisc += dwd;  // accumulate total dicards
   gTotAdd  += dwa;  // and additions

   gdwTotWrite += gdwWritten; // accumulate TOTAL written out
   gliTotWrite.QuadPart += gdwWritten;

   gdwLines = dwl;            // number of lines output

   gdwLine2 = dwl2;           // and number of file buffer lines = count of 0x0a
   if(( dwl2 == 0 ) && ( dwsc > 0 ))
      gdwLine2 = 1;           // well there was some data there = 1 line

   if( Pgm_Flag & Show_Hex ) // -h2a - show HEX to ASCII conversion
      Show_Hex2Ascii( lpb, dws );

   return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Do_File
// Return type: BOOL 
// Arguments  : PWL pwl
//            : LPTSTR lpf
// Description: Process a FILE from the file list
//              Open, Map and get Map View
///////////////////////////////////////////////////////////////////////////////
BOOL  Do_File( PWL pwl, LPTSTR lpf )
{
   BOOL           bRet = FALSE;  // begin with failed
   LARGE_INTEGER  li;
   HANDLE         h = 0;
   DWORD          dws;

   li.HighPart = pwl->wl_sFD.nFileSizeHigh;
   li.LowPart  = dws = pwl->wl_sFD.nFileSizeLow;

   if( Pgm_Flag & Do_Geoff ) {
      PTSTR pt = "-g";
      if( Pgm_Flag & Do_Geoff1 )
         pt = "-g1";
      if( VERB1 ) {
         sprtf( "Doing file %s of %I64u bytes with %s flag."MEOR,
                  GetShortName(lpf,40),
                  li,
                  pt );
      }

#ifdef ADDGEOFF1  // -g1 command
      if( Pgm_Flag & Do_Geoff1 )
         bRet = DoGeoff1(lpf);
      else
#endif   // #ifdef ADDGEOFF1  // -g1 command
         bRet = DoGeoff2(lpf);

      return bRet;
   }

#ifdef  ADDJOBS1   // -j 22 Jan. 2004
// case 'J': lpc++; int Get_J_Cmd(PTSTR lpc)
   if( Pgm_Flag & Do_Jobs )   // -j
   {
      // extern int DoJobs( PTSTR lpf );
      bRet = DoJobs(lpf);
      return bRet;
   }

#endif // #ifdef  ADDJOBS1   // -j 22 Jan. 2004
//   li.HighPart = pwl->wl_sFD.nFileSizeHigh;
//   li.LowPart  = dws = pwl->wl_sFD.nFileSizeLow;
   if(dws == 0)   // a million and 1 chance
   {
      if( li.HighPart )
         dws--;
   }
   if(dws)
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
      HANDLE hm = CreateFileMapping( h,	// handle to file to map 
			NULL,	// optional security attributes
			g_flProtext,	// protection for mapping object
			0,				// high-order 32 bits of object size
			0,				// low-order 32 bits of object size
			NULL );		// name of file-mapping object
      if( VFH(hm) )
      {
         LPTSTR lpb = MapViewOfFile( hm,	// file-mapping object to map into address space
					g_ReqAccess,	// access mode
					0,		// high-order 32 bits of file offset
					0,		// low-order 32 bits of file offset
					0 );	// number of bytes to map
         if(lpb)
         {
            if( Pgm_Flag & Do_Funcs ) {
               if( VERB1 ) {
                  sprtf( "Doing file %s of %I64u bytes with -f flag."MEOR,
                           GetShortName(lpf,40),
                           li );
               }
               bRet = Out_Funcs( lpb, dws, lpf );
//            } else if( Pgm_Flag & Rem_HTML ) {  // Add working with HTML
            } else if( Pgm_Flag & (Rem_HTML|Add_HTML) ) {  // FIX20041107
               if( VERB1 ) {
                  sprtf( "Doing file %s of %I64u bytes with -h flag."MEOR,
                           GetShortName(lpf,40),
                           li );
               }
               bRet = Do_HTML( lpb, dws, lpf );
            } else if( Pgm_Flag & Fix_DIR ) {
               if( VERB1 ) {
                  sprtf( "Doing file %s of %I64u bytes with -w flag."MEOR,
                           GetShortName(lpf,40),
                           li );
               }
               bRet = Do_Dir( lpb, dws, lpf );
#ifdef ADD_TXT2VC // FIX20050203 - add -c - output C/C++ text form
            } else if( Pgm_Flag & VC_Out ) {
               bRet = Do_VC( lpb, dws, lpf );
               //   dmsg( " -c[name]   Output as single C/C++ text form."MEOR );
#endif // #ifdef ADD_TXT2VC // FIX20050203 - add -c - output C/C++ text form
            } else {
               if( VERB1 ) {
                  sprtf( "Doing file %s of %I64u bytes, max=%d sp=%s."MEOR,
                           GetShortName(lpf,26),
                           li,
                           g_dwMaxLine,
                           (( Pgm_Flag & Fix_LS ) ? "On" : "Off" ) );    // try to break the line only on a SPACE
               }

               // case 'B': -b
               if( Pgm_Flag & BS_Dup ) {
                  bRet = Do_Bin_Buffer( lpb, dws );
               } else {
                  bRet = Do_Buffer( lpb, dws );
               }
            }

            UnmapViewOfFile( lpb );      // address

            // if SUCCESS
            if( bRet && VERB1 )
            {
               LPTSTR   lpo;
               PTSTR    pMsg = gszMsgBuf;
               if( VFH(gOut_Hand) )
                  lpo = &gszOFName[0];
               else
               {
                  if( gbRedON )
                     lpo = "Redirected StdOut";
                  else
                     lpo = "Standard Out";
               }

               sprtf( "Done file %s of %I64u bytes."MEOR,
                  GetShortName(lpf,40),
                  li );
               sprintf(pMsg, "Added %d", gAdded);
               if( gConverted )
                  sprintf(EndBuf(pMsg), ", Converted %d", gConverted);
               if( gDiscard )
                  sprintf(EndBuf(pMsg), ", Discarded %d", gDiscard);
               sprintf(EndBuf(pMsg), ". Lines %d, now %d", gdwLine2, gdwLines );
               sprtf("%s"MEOR, pMsg);

               sprtf( "Written %d bytes to %s."MEOR,
                  gdwWritten,
                  lpo );   // OF_Name, "Standard Out" or redirected
//	If	AddSplit
//	mov	Ax,Out_FSiz1
//	mov	Dx,Out_FSiz2
//	Add	Out_TFS1,Ax
//	Adc	Out_TFS2,Dx
//	Mov	Ax,Out_TFS1
//	Mov	Dx,Out_TFS2
            }
            else
            {
               // something FAILED
            }
         }
         else
         {
            // FAILED TO MAP VIEW (READING ONLY)!!!
            gdwError = GetLastError();
            if( VERB1 )
            {
               sprtf( "WARNING: Failed to get map view of file!"MEOR
                  "\tERROR=%d!"MEOR,
                  lpf,
                  gdwError );
            }
         }
         CloseHandle(hm);
      }
      else
      {
         // FAILED TO MAP FILE FOR READING!!!
         gdwError = GetLastError();
         if( VERB1 )
         {
            sprtf( "WARNING: Failed to map [%s] file!"MEOR
               "\tERROR=%d!"MEOR,
               lpf,
               gdwError );
         }
      }
      CloseHandle(h);
   }
   else if( VERB1 )
   {
      if(dws)
      {
         // FAILED TO OPEN FILE FOR SIMPLE READING ONLY!!!
         gdwError = GetLastError();
         sprtf( "WARNING: Failed to open [%s] file!"MEOR
            "\tERROR=%d!"MEOR,
            lpf,
            gdwError );
      }
      else
      {
         sprtf( "WARNING: Nothing in file to process!"MEOR, lpf );
      }
   }
   return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : ProcessFiles
// Return type: INT 
// Argument   : VOID
// Description: Process the file or set of files presently available in
//              the linked file list.
///////////////////////////////////////////////////////////////////////////////
INT  ProcessFiles( VOID )
{
   INT      iRet = 0;
   LPTSTR   lpf;
   PLE      pHead = &gsFileList;
   PLE      pNext;
   PWL      pwl;
   DWORD    dwc, dwn;
   LARGE_INTEGER  li;

   dwc = 0;
   Traverse_List( pHead, pNext )
   {
      pwl = (PWL)pNext;
      if( !( pwl->wl_dwFlag & wlf_Exclude ) &&
           ( pwl->wl_dwType & IS_FILE ) )
      {
         dwc++;
      }
   }
   dwn = 0;
   Traverse_List( pHead, pNext )
   {
      pwl = (PWL)pNext;
      if( !( pwl->wl_dwFlag & wlf_Exclude ) &&
           ( pwl->wl_dwType & IS_FILE ) )
      {
         dwn++;   // count a file to be processed
         lpf = &pwl->wl_szFull[0];  // get pointer to name
         if( VERB2 )
         {
            li.HighPart = pwl->wl_sFD.nFileSizeHigh;
            li.LowPart  = pwl->wl_sFD.nFileSizeLow;
            if( VERB9 ) {
               sprtf( MEOR"Processing %s of %I64u bytes ...(%d of %d)"MEOR,
                  lpf,
                  li,
                  dwn, dwc );
            } else {
               sprtf( MEOR"Processing %s of %I64u bytes ...(%d of %d)"MEOR,
                  GetShortName(lpf,40),
                  li,
                  dwn, dwc );
            }
         }

         gdwError = 0;  // start with NO global error
         // map file and process it
         if( !Do_File( pwl, lpf ) )
         {
            // FAILED to complete file
            iRet++;
            li.HighPart = pwl->wl_sFD.nFileSizeHigh;
            li.LowPart  = pwl->wl_sFD.nFileSizeLow;
            if( VERB1 )
            {
               if( VERB2 )
               {
                  sprtf( "WARNING: Processing failed! (%d of %d)"MEOR,
                     dwn, dwc );
               }
               else
               {
                  sprtf( "WARNING: Processing %s of %I64u bytes FAILED! (%d of %d)"MEOR,
                     GetShortName(lpf,40),
                     li.QuadPart,
                     dwn, dwc );
               }
            }
            if( ( gdwError ) && ( dwn < dwc ) )
            {
               LPTSTR   lpm = &gszMsgBuf[0];
               INT      i;
               //LPTSTR   lpt = &gszTmpBuf[0];
               sprintf(lpm, "ERROR while processing file %s of %I64u bytes."MEOR
                  "ERROR value is %d!"MEOR
                  "This is file %d of a set of %d."MEOR
                  "Continue processing the list?",
                  lpf, li.QuadPart,
                  gdwError,
                  dwn, dwc );

               i = MessageBox( NULL, lpm, "PROCESSING ERROR",
                  MB_YESNO | MB_ICONINFORMATION );
               if( i == IDNO )
               {
                  break;
               }
            }
         }
      }
   }
   return iRet;
}

// #ifndef  NDEBUG
VOID  ShowReport( VOID )
{
   if(VERB5) {
      LPTSTR   lps;
      PLE      pHead = &gsReport;
      PLE      pNext;
      PDL      pdl;
      Traverse_List( pHead, pNext )
      {
         pdl = (PDL)pNext;
         lps = &pdl->dl_szFile[0];
         sprtf( lps );
      }
   }
}
// #endif   // !NDEBUG

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : main
// Return type: int 
// Arguments  : int argc
//            : char * argv[]
// Description: Operating system ENTRY to program, after 'C' setup of command
//              line.
///////////////////////////////////////////////////////////////////////////////
int	main( int argc, char * argv[] )
{
   int   i = 0;

   Setup();    // intial SETUP
   if( argc > 1 )
   {
      gcArgs = argc;
      gpArgv = argv;

      if( Chk_Command( argc, argv ) )
         Give_Help( 3 );
      if( IsListEmpty( &gsFileList ) )
      {
         sprtf( "ERROR: No input file(s) given!"MEOR );
         Give_Help( 4 );
      }
      if( Pgm_Flag & Out_Fil ) // if using OUTPUT file
      {
         if( Get_O_File() )
         {
            // failed to get OUTPUT file
            if( VERB1 )
            {
               sprtf( "ERROR: Unable to open/create output file of name"MEOR
                  "\t%s!"MEOR,
                  &gszOFName[0] ); // file name
               Pgm_Exit( 10 );
            }
         }
      }
      if( VERB9 ) {
         sprtf( "NOTE: Using a work allocation of %d bytes. (DMAXLINE=%d)."MEOR,
            gdwSize, // = sizeof(WRK);  //  W.ws_dwSize    // ALLOCATION SIZE
            GetMaxLn() );  // gdwMaxLn );    // = def [DMAXLINE]
      }

      gdwWrkCnt = ExpandList( &gsFileList );    // expand any files

      if( VERB5 )
         ShowCmd();
      gnCurOut = 0;
      if( VERB8 )
         ShowWL();

      if( VERB9 ) {
         sprtf( "Got VERB 9, and %s ..."MEOR, (g_iRemText ? "remove text" : "NO remove") );
      }

      if( gdwWrkCnt == 0 )
      {
         sprtf( "ERROR: No input file(s) found from given command!"MEOR );
         Give_Help( 5 );
      }

      i = ProcessFiles();

   }
   else
   {
      sprtf( "ERROR: No command line given!"MEOR );
      Give_Help(2);
   }

//#ifndef  NDEBUG
   ShowReport();
//#endif   // !NDEBUG

	Pgm_Exit( i );

	return i;
}

#undef  ADDTIMETEST
#ifdef   ADDTIMETEST

/* TIMES.C illustrates various time and date functions including:
 *      time            _ftime          ctime       asctime
 *      localtime       gmtime          mktime      _tzset
 *      _strtime        _strdate        strftime
 *
 * Also the global variable:
 *      _tzname
 */

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>

void main()
{
   int i = 0;
    char tmpbuf[128], ampm[] = "AM";
    time_t ltime;
    struct _timeb tstruct;
    struct tm *today, *gmt, xmas = { 0, 0, 12, 25, 11, 93 };

    /* Set time zone from TZ environment variable. If TZ is not set,
     * the operating system is queried to obtain the default value 
     * for the variable. 
     */
    Setup();
    _tzset();

    /* Display operating system-style date and time. */
    _strtime( tmpbuf );
    sprtf( "OS time:\t\t\t\t%s\n", tmpbuf );
    _strdate( tmpbuf );
    sprtf( "OS date:\t\t\t\t%s\n", tmpbuf );

    /* Get UNIX-style time and display as number and string. */
    time( &ltime );
    sprtf( "Time in seconds since UTC 1/1/70:\t%ld\n", ltime );
    sprtf( "UNIX time and date:\t\t\t%s", ctime( &ltime ) );

    /* Display UTC. */
    gmt = gmtime( &ltime );
    sprtf( "Coordinated universal time:\t\t%s", asctime( gmt ) );

    /* Convert to time structure and adjust for PM if necessary. */
    today = localtime( &ltime );
    if( today->tm_hour > 12 )
    {
   strcpy( ampm, "PM" );
   today->tm_hour -= 12;
    }
    if( today->tm_hour == 0 )  /* Adjust if midnight hour. */
   today->tm_hour = 12;

    /* Note how pointer addition is used to skip the first 11 
     * characters and printf is used to trim off terminating 
     * characters.
     */
    sprtf( "12-hour time:\t\t\t\t%.8s %s\n",
       asctime( today ) + 11, ampm );

    /* Print additional time information. */
    _ftime( &tstruct );
    sprtf( "Plus milliseconds:\t\t\t%u\n", tstruct.millitm );
    sprtf( "Zone difference in seconds from UTC:\t%u\n", 
             tstruct.timezone );
    sprtf( "Time zone name:\t\t\t\t%s\n", _tzname[0] );
    sprtf( "Daylight savings:\t\t\t%s\n", 
             tstruct.dstflag ? "YES" : "NO" );

    /* Make time for noon on Christmas, 1993. */
    if( mktime( &xmas ) != (time_t)-1 )
   sprtf( "Christmas\t\t\t\t%s\n", asctime( &xmas ) );

    /* Use time structure to build a customized time string. */
    today = localtime( &ltime );

    /* Use strftime to build a customized time string. */
    strftime( tmpbuf, 128,
         "Today is %A, day %d of %B in the year %Y.\n", today );
    sprtf( tmpbuf );

   Pgm_Exit( i );

}


#endif   // #ifdef   ADDTIMETEST

// eof - FixF32.c

