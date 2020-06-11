

// FixFWork.h
#ifndef	_FixFWork_H
#define	_FixFWork_H

//#define		MXOUTB		2048     // an output buffer
#define		MXOUTB		1024*16  // a 16K output buffer
#define     MXOUTBUF    MXOUTB
#define     MXOUTIT     1024     // start looking for a LF (end of line)
#define     MX1LINE     75       // add cr/lf


//#define     DMAXLINE       1024     // must have lines less than this
#define     DMAXLINE       4096  // must have FUNCTIONS less than this
#define     MXTMPB         1024

//#define     MXLINEB     520      // was 264
#define     MXLINEB     DMAXLINE   // was 520
#define     MXLINEB2    (MXLINEB + 32)    // allow some overrun
#define     MXLINES     32       // was 16

// g_iKeepRaw BIT FLAG
#define  gf_KeepHi   0x01
#define  gf_KeepLo   0x02

// -W[F,T] F = Full listing, T = Try Hard
//   BOOL     ws_dwFullDir;   // g_dwFullDir bit flags
#define     bf_FullDir     0x00000001
#define     bf_TryHard     0x00000002
#define     bf_American    0x00000004     // -wa
#define     bf_FullDir2    0x00000008     // -wf2 

#define     bf_SortList    0x00000010

#define     bf_SizeSort    0x00000020
#define     bf_DateSort    0x00000040
#define     bf_NameSort    0x00000080

#define     bf_XcludeD     0x00000100
#define     bf_XcludeF     0x00000200

// -g(eoff) tele list constants
#define  MMINBND     16
#define  NMLEN          24
#define  NMMX2C         45
#define  MMXBU          10
#define  MXNUMSET       24


typedef struct {

   DWORD    ws_dwSize;     // gdwSize
	int		ws_iVerbose;

   HANDLE   ws_hStdOut;    // ghStdOut = Standard out
   HANDLE   ws_hErrOut;    // ghErrOut = Error out

   BOOL     ws_bRedON;     // gbRedON = being redirected - use stderr for output
   DWORD    ws_dwMode;     // gdwMode
   BOOL     ws_UnifDiff;   // gbUnifDiff
   BOOL     ws_DelBOM;      // gbDelBOM

   HANDLE   ws_Out_Hand;   // gOut_Hand = FILE if any given

   DWORD    ws_dwError;    // gdwError

   DWORD    ws_dwWrkCnt;   // gdwWrkCnt

	int		ws_nInpCnt;
	LPTSTR   ws_lpOut;
//	int		ws_nMaxCol;
//	int		ws_nMinCol;

   // command line buffer items
   int      ws_iArgs;      // gcArgs = argc;
   char * * ws_pArgv;      // gpArgv = argv
	int		ws_nCurOut;    // gnCurOut
   int      ws_nCurLn;     // gnCurLn

   DWORD    ws_Discard;    // gDiscard - 1 Discard2	label word
   DWORD    ws_Added;      // gAdded - 1 Added2	label word
   DWORD    ws_Converted;  // gConverted++; // like convert to a SPACE
   DWORD    ws_TotDisc;    // gTotDisc - 1 Discard2	label word
   DWORD    ws_TotAdd;      // gTotAdd - 1 Added2	label word
   DWORD    ws_TotConv;    // gTotConv - like convert to a SPACE

   DWORD    ws_dwHexLen;   // g_dwHexLen -h2a[nn] size

   DWORD    ws_dwWritten;  // gdwWritten
   DWORD    ws_dwTotWrite; // gdwTotWrite
   LARGE_INTEGER  ws_liTotWrite; // gliTotWrite

   DWORD    ws_dwLines;    // gdwLines
   DWORD    ws_dwLine2;    // gdwLine2
   DWORD    ws_dwTotLines;  // gdwTotLine

   DWORD    ws_dwMaxLine;  // gdwMaxLine - DEBUG function to see largest FUNCTION (-f)
   DWORD    ws_MaxLine;    // g_dwMaxLine = MXLINE; // set DEFAULT line length

   DWORD    ws_dwDel_CC;   // g_dwDel_CC
   DWORD    ws_dwOff_CC;   // g_dwOff_CC

   DWORD    ws_dwTab_Size;  // g_dwTab_Size  // TAB size
//   DWORD    ws_dwSp_Sz;    // g_dwSp_Sz = 250000;   // 1	dw	0d090h Sp_Sz2	dw	3	; 250,000 bytes
   DWORD    ws_dwSp_Sz;    // g_dwSp_Sz = 2,000,000;   // bytes

   // -W[F] = Full listing
   BOOL     ws_dwFullDir;   // g_dwFullDir

   INT      ws_iKeepRaw;   // g_iKeepRaw

   // -g switch stuff
   // -g(eoff) tele list stuff
   BOOL     w_bGotBU1, w_bGotCU;   // (BOOL)chkbu( lpb, &dwo );
   BOOL     w_bDnCompile;     // gbDnCompile
   BOOL     w_bNotSame; // gbNotSame - I IF def CHKSAMELET =chkme( "Same letter inhibit is ON ..." );
   BOOL     w_bAddComp; // gbAddComp    = FALSE;
   INT      w_iLnCnt;   // giLnCnt -  count 1, 2 ... max 3 until code added
   DWORD    w_dwBlkCnt; // gdwBlkCnt - count of BLOCKS processed

   TCHAR    w_cHeadOrg[ (NMMX2C + 15) ];   // gcHeadOrg - Orginal
   TCHAR    w_cHeadX1[ (NMMX2C + 15) ];    // gcHeadX1 - Last name rotation
   TCHAR    w_cHeadX2[ (NMMX2C + 15) ];    // cHeadX2 - Split of last if hythenated
   // Split of information when listed say as "Annie & Geoff", then sort
   // will catch BOTH names
   TCHAR    w_cHead1[ (NMMX2C + 15) ]; // gcHead1
   TCHAR    w_cHead2[ (NMMX2C + 15) ]; // gcHead2
   TCHAR    w_cTailSee[ ((NMMX2C*2) + 15) ]; // gcTailSee
   TCHAR    w_cNum[MXNUMSET+8];   // gcNum - could be attached to TAIL idea

   BOOL     ws_bDoExtra; // gbDoExtra = -g[x[0|-|+|1]]
   BOOL     ws_bChkPWD;  // gbChkPWD = TRUE; // -gp[0|1|+|-]
   BOOL     ws_bIgnoreMNU;  // gbIgnoreMNU  = TRUE; -gi[...] ignore 123ABC
   BOOL     ws_bUseSee;     // gbUseSee     = FALSE; -gs[.] use SEE in place of ...
   DWORD    w_dwOutSz;  // gdwOutSz = MXOUTBUF;
   LPTSTR   w_lpOut;    // glpOut
   LPTSTR   w_lpOut2;   // glpOut2
   TCHAR    w_cGOutFile[264]; // gcGOutFile - -go"output file" command

   LIST_ENTRY  ws_sFileList;  // gsFileList
   LIST_ENTRY  ws_sExclList;  // gsExclList
//#if   AddRem
   LIST_ENTRY  ws_sRemList;   // gsRemList
   LIST_ENTRY  ws_sRemText;   // gsRemText
   int         ws_iRemText;   // g_iRemText
//#endif   // AddRem
   LIST_ENTRY  ws_sReport;    // gsReport
   LIST_ENTRY  ws_sFndRpl;    // gsFndRpl - find/replace list

   LPTSTR   ws_pLineBuf1;      // gpLineBuf1
   LPTSTR   ws_pLineBuf2;      // gpLineBuf2

   WIN32_FIND_DATA   ws_sFD;  // gsdFD

   INT      ws_iLnBuf;        // giLnBuf
	TCHAR    ws_szLnBuf[(MXLINEB2 * MXLINES)];   // gszLnBuf

   TCHAR    ws_szOFName[264];  // gszOFName

   TCHAR    ws_szDecimal[256];   // gszDecimal
   TCHAR    ws_szTitle[264];  // gszTitle
   TCHAR    ws_szFolder[264];  // gszFolder
   TCHAR    ws_szName[264];    // gszName
   TCHAR    ws_szRoot[264];    // gszRoot

   TCHAR    ws_szMsgBuf[MXTMPB + 32];   // gszMsgBuf = 1024
   TCHAR    ws_szGenBuf[MXTMPB + 32];   // gszGenBuf = 1024
   TCHAR    ws_szGenBu2[MXTMPB + 32];   // gszGenBu2 = 1024
   TCHAR    ws_szTmpBuf[MXTMPB + 32];   // gszTmpBuf = 1024
   TCHAR    ws_szErrBuf[MXTMPB + 32];   // gszErrBuf = 1024

   TCHAR    ws_szLineB1[ DMAXLINE + 32 ];   // gszLineB1
   TCHAR    ws_szLineB2[ DMAXLINE + 32 ];   // gszLineB2
   TCHAR    ws_szLineB3[ DMAXLINE + 32 ];   // gszLineB3

	TCHAR    ws_szOutBuf[MXOUTB + 32];     // gszOutBuf
	TCHAR    ws_szOutBuf2[MXOUTB + 32];    // gszOutBuf2

}WRK, * PWRK;

extern   PWRK	   gpW;
#define  W        (*gpW)

#define  gdwSize     W.ws_dwSize    // ALLOCATION SIZE
#define  gOut_Hand   W.ws_Out_Hand  // = Standard OUT (or FILE)

#define  ghStdOut    W.ws_hStdOut   // = Standard out
#define  ghErrOut    W.ws_hErrOut   // = Error out
#define  gbRedON     W.ws_bRedON    // = being redirected - use stderr for output
#define  gdwMode     W.ws_dwMode

#define  gdwError    W.ws_dwError   // global error flag

#define  gsFileList  W.ws_sFileList    // list of input file (in gnInpCnt)
#define  gsExclList  W.ws_sExclList    // -x exclude list
//#if   AddRem
#define  gsRemList   W.ws_sRemList     // Remove line list
#define  gsRemText   W.ws_sRemText     // Remove text list
#define  g_iRemText  W.ws_iRemText   // g_iRemText

//#endif   // AddRem
#define  gsReport    W.ws_sReport      // diagnostic report output

#define  gsFndRpl    W.ws_sFndRpl      // find/replace list

#define	giVerbose	W.ws_iVerbose

#define  gDiscard    W.ws_Discard      // 1 Discard2	label
#define  gAdded      W.ws_Added        // 1 Added2	label word
#define  gConverted  W.ws_Converted    // convert say to a SPACE
#define  gTotDisc    W.ws_TotDisc      // total discarded
#define  gTotAdd     W.ws_TotAdd       // total added
#define  gTotConv    W.ws_TotConv      // convert say to a SPACE

#define  gdwWritten  W.ws_dwWritten
#define  gdwTotWrite W.ws_dwTotWrite
#define  gliTotWrite W.ws_liTotWrite

#define  gdwLines    W.ws_dwLines
#define  gdwLine2    W.ws_dwLine2
#define  gdwTotLines W.ws_dwTotLines

#define	gnInpCnt		W.ws_nInpCnt
//#define	gnMaxCol		W.ws_nMaxCol
//#define	gnMinCol		W.ws_nMinCol
#define  gszFolder   W.ws_szFolder
#define  gszName     W.ws_szName
#define  gszRoot     W.ws_szRoot
#define  gszTitle    W.ws_szTitle
#define  gsFD        W.ws_sFD

// an output buffer, and its count
#define	gszOutBuf	W.ws_szOutBuf
#define	gszOutBuf2  W.ws_szOutBuf2

#define	gnCurOut		W.ws_nCurOut
#define	gnCurLn		W.ws_nCurLn

// (MXLINEB * MXLINES)
#define  giLnBuf     W.ws_iLnBuf
#define  gszLnBuf  W.ws_szLnBuf

#define  gszDecimal  W.ws_szDecimal
#define  gdwWrkCnt   W.ws_dwWrkCnt

#define  gszGenBuf   W.ws_szGenBuf  // [1024]
#define  gszGenBu2   W.ws_szGenBu2  // [1024]
#define  gszTmpBuf   W.ws_szTmpBuf  // [1024]
#define  gszMsgBuf   W.ws_szMsgBuf  // [1024]
#define  gszErrBuf   W.ws_szErrBuf  // [1024]

#define  gpLineBuf1  W.ws_pLineBuf1
#define  gpLineBuf2  W.ws_pLineBuf2

#define  gszLineB1   W.ws_szLineB1  // [DMAXLINE]
#define  gszLineB2   W.ws_szLineB2  // [DMAXLINE]
#define  gszLineB3   W.ws_szLineB3  // [DMAXLINE]

#define  gdwMaxLine  W.ws_dwMaxLine // DEBUG function to see largest FUNCTION (-f)

#define  gcArgs      W.ws_iArgs     // = argc;
#define  gpArgv      W.ws_pArgv     // = argv

#define  gszOFName   W.ws_szOFName
#define  g_dwMaxLine W.ws_MaxLine   // = MXLINE; // set DEFAULT line length

#define  g_dwDel_CC  W.ws_dwDel_CC
#define  g_dwOff_CC  W.ws_dwOff_CC

#define  g_dwTab_Size W.ws_dwTab_Size  // TAB size
#define  g_dwSp_Sz   W.ws_dwSp_Sz      // -s[nnnn] = 250000 or 2 MB, or ...

// -W... do directory listing
#define  g_dwFullDir  W.ws_dwFullDir

// -h2a[nn] switch
#define  g_dwHexLen  W.ws_dwHexLen

#define  g_iKeepRaw  W.ws_iKeepRaw  // Keep RAW characters

   // -g switch stuff
   // -g(eoff) tele list stuff
#define  gbGotBU1 W.w_bGotBU1
#define  gbGotCU  W.w_bGotCU     // (BOOL)chkbu( lpb, &dwo );
#define  gbDnCompile W.w_bDnCompile
#define  gbNotSame   W.w_bNotSame   // - I IF def CHKSAMELET =chkme( "Same letter inhibit is ON ..." );
#define  gbAddComp   W.w_bAddComp   //    = FALSE;
#define  giLnCnt     W.w_iLnCnt     // -  count 1, 2 ... max 3 until code added
#define  gcHeadOrg   W.w_cHeadOrg   // - Orginal
#define  gcHeadX1    W.w_cHeadX1    // - Last name rotation
#define  gcHeadX2    W.w_cHeadX2    // - Split of last if hythenated
   // Split of information when listed say as "Annie & Geoff", then sort
   // will catch BOTH names
#define  gcHead1     W.w_cHead1
#define  gcHead2     W.w_cHead2
#define  gcTailSee   W.w_cTailSee
#define  gcNum       W.w_cNum    // - could be attached to TAIL idea

#define  gbDoExtra   W.ws_bDoExtra     // = -g[x[0|-|+|1]]
#define  gbChkPWD    W.ws_bChkPWD      //  = TRUE; // -gp[0|1|+|-]
#define  gbIgnoreMNU W.ws_bIgnoreMNU   //  = TRUE; -gi[...] ignore 123ABC
#define  gbUseSee    W.ws_bUseSee      //     = FALSE; -gs[.] use SEE in place of ...

#define gdwBlkCnt W.w_dwBlkCnt // g- count of BLOCKS processed DWORD

#define gdwOutSz W.w_dwOutSz // g= MXOUTBUF; DWORD
#define glpOut W.w_lpOut // g LPTSTR
#define glpOut2 W.w_lpOut2 // g LPTSTR

#define gcGOutFile W.w_cGOutFile // g- -go"output file" command

#define  VERB     ( ( giVerbose > 0 ) && ( Pgm_Flag & VerbON ) )

#define gbUnifDiff W.ws_UnifDiff // Convert Unified DIFF patch headers '\' to '/'
#define gbDelBOM W.ws_DelBOM // remove any UTF-8 BOM from file'


#endif	// _FixFWork_H
// eof - FixFWork.h
