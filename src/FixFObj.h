
// FixFObj.h
#ifndef	_FixFObj_H
#define	_FixFObj_H

typedef struct tabBLK {
   LIST_ENTRY  b_LE;
   DWORD       b_Len;
   TCHAR       b_Stg[1];
}BLK, * PBLK;

#if	AddFunc

#define  ERR_NONE       0
#define  ERR_DEFINE     -1
#define  ERR_MAXLN      -2
#define  ERR_MEMORY     -3
#define  ERR_CANCEL     -4
#define  ERR_ARRAY      -5    // ran out of data while filling in an array


#define  NewLnChr(c)    ( ( c == '\r' ) || ( c == '\n' ) )
#define  False          FALSE
#define  True           TRUE
#define  vbCrLf         "\r\n"
#define  Len            strlen
#define  DEF_MINTLEN    9

#define  is_if       1
#define  is_else     2
#define  is_endif    3
#define  is_ifdef    4
#define  is_undef    5
#define  is_ifndef   6

#define  ifo_Full       0x001       // -f:F
#define  ifo_Short      0x002       // -f:S
#define  ifo_Local      0x004       // -f:L
#define  ifo_Global     0x008       // -f:G
#define  ifo_Struct     0x010       // -f:T

#define  ifo_Def        (ifo_Full | ifo_Short)
#define  ifo_All  (ifo_Full | ifo_Short | ifo_Local | ifo_Global | ifo_Struct)

#define  IsNumeric(a)   ( ( a >= '0' ) && ( a <= '9' ) )
#define  IsUpper(a)     ( ( a >= 'A' ) && ( a <= 'Z' ) )
#define  IsLower(a)     ( ( a >= 'a' ) && ( a <= 'z' ) )
#define  IsAlphaNumeric(a) ( IsUpper(a) || IsLower(a) || IsNumeric(a) )


// Global list of 40 functions in fixfobj.c
extern INT      FixFunBody( LPTSTR lps );
extern VOID     GetTidyFunc( LPTSTR lpl, LPTSTR lps, DWORD dwMin );
extern DWORD    OutFunList( PLE pHead, DWORD iCnt, PLE pSht, LPTSTR lpf, PDWORD pdw, PLE pStr );
extern INT      InStr( LPTSTR lpb, LPTSTR lps );
extern INT      InStrWhole( LPTSTR lpb, LPTSTR lps );
extern INT      myInStrF( LPTSTR sStg, LPTSTR sFnd );
extern LPTSTR   Left( LPTSTR lpl, DWORD dwi );
extern LPTSTR   Right( LPTSTR lpl, DWORD dwl );
extern LPTSTR   Mid( LPTSTR lpl, DWORD dwb, DWORD dwl );
extern BOOL     IsInclude( LPTSTR sStg );
extern BOOL     IsPragma( LPTSTR sStg );
extern BOOL     IsDefine( LPTSTR sStg );
extern INT      IsAnIf( LPTSTR sStg );
extern INT      CondLine( LPTSTR lpb );
extern INT      MaxTrim( LPTSTR lpb );
extern PBLK     Add2sBlk( PLE pHead, LPTSTR lpl );
extern DWORD    stLen( PLE pHead );
extern DWORD    Get1Line( PLE pHead, LPTSTR lpl, DWORD dwmax );
extern INT      GetFunc( PLE pHead, LPTSTR lpl, DWORD dwmax );
extern DWORD    GetALine( LPTSTR lpl, DWORD dwmax, LPTSTR lpb, DWORD dws, PDWORD pdwi );
extern BOOL     IsStruct( LPTSTR sStg );
extern INT      InStrCnt( LPTSTR sStg, LPTSTR sFnd );
extern INT      InStrCntF( LPTSTR sStg, LPTSTR sFnd );
extern INT      OpenCnt( LPTSTR sStg );
extern INT      CloseCnt( LPTSTR sStg );
extern INT      OpenCntF( LPTSTR sStg );
extern INT      CloseCntF( LPTSTR sStg );
extern INT      LastInStr( LPTSTR sStg, LPTSTR sFnd );
extern INT      LastInStrF( LPTSTR sStg, LPTSTR sFnd );
extern BOOL     IsEndStruct( LPTSTR sStg );
extern INT      StripComments( LPTSTR sStg );
extern BOOL     IsDo( LPTSTR sStg );
extern BOOL     IsEndDo( LPTSTR sStg );
extern BOOL     IsArrayDef( LPTSTR lpl );
extern BOOL     IsArrayEnd( LPTSTR lpl );
extern BOOL     Out_Funcs( LPTSTR lpb, DWORD dws, LPTSTR lpf );
extern VOID     ShowFHelp( VOID );
extern INT      All2Upper( LPTSTR lps );
extern BOOL     GetListType( LPTSTR lpcmd );
extern DWORD    GetMaxLn( VOID );
// ============================================

#endif	// #if AddFunc

#endif	// _FixFObj_H
// eof - FixFObj.h
