

// FixFUtil.h
#ifndef	_FixFUtil_H
#define	_FixFUtil_H

#define  MXRLINE        264

#define	VERB1		( giVerbose > 0 )
#define	VERB2		( giVerbose > 1 )
#define	VERB3		( giVerbose > 2 )
#define	VERB4		( giVerbose > 3 )
#define	VERB5		( giVerbose > 4 )
#define	VERB6		( giVerbose > 5 )
#define	VERB7		( giVerbose > 6 )
#define	VERB8		( giVerbose > 7 )
#define	VERB9		( giVerbose > 8 )

#define	VFHO(hf)	( ( hf ) && ( hf != HFILE_ERROR ) )
#define  VFH(h)   ( ( h ) && ( h != INVALID_HANDLE_VALUE ) )

#define	IF_VERB2( _x_ )		if( VERB2 ) { _x_ }

#define  wlf_DnExp      0x00000001
#define  wlf_Processed  0x00000002
#define  wlf_TooBig     0x20000000     // remove this restriction some day
#define  wlf_Null       0x40000000
#define  wlf_Exclude    0x80000000

// gsFileList
typedef  struct   tagWL {
   LIST_ENTRY  wl_Entry;
   DWORD       wl_dwFlag;
   DWORD       wl_dwLen;
   DWORD       wl_dwType;
   WIN32_FIND_DATA   wl_sFD;
   TCHAR       wl_szFull[264];
   TCHAR       wl_szTitle[264];
   TCHAR       wl_szFile[264];
}WL, * PWL;

// gsExclList
typedef  struct   tagXL {
   LIST_ENTRY  xl_Entry;
   DWORD       xl_dwLen;
   TCHAR       xl_szFile[264];
}XL, * PXL;

// gsRemList
typedef  struct   tagRL {
   LIST_ENTRY  rl_Entry;
   DWORD       rl_dwLen;
   TCHAR       rl_szFile[MXRLINE];     // was [264];
}RL, * PRL;

// gsReport
typedef  struct   tagDL {
   LIST_ENTRY  dl_Entry;
   DWORD       dl_dwLen;
   TCHAR       dl_szFile[1];
}DL, * PDL;

extern   INT   _cdecl sprtf( LPTSTR lpf, ... );
extern   VOID  _cdecl AddReport( LPTSTR lpf, ... );
extern   VOID  _cdecl chkme( LPTSTR lpf, ... );

//#ifdef	NDEBUG
//#define	DO1(a,b)
//#else	/* !NDEBUG */
//#define	DO1(a,x)		sprt(a,x)
//#endif	/* NDEBUG y/n */
#define  DO1      sprtf

extern   VOID  DiagOpen( VOID );
extern   VOID  DiagClose( VOID );
extern   __int64 MyFileSeek( HANDLE hf, __int64 distance, DWORD MoveMethod );

extern	PWL   Add2WLList( LPTSTR lpf );
extern   PXL   Add2XLList( LPTSTR lpf );
extern   PRL   Add2RLList( LPTSTR lpf );
extern   PRL   Add2rLList( LPTSTR lpf );
extern   PDL   Add2DLList( LPTSTR lpf );

extern   BOOL  InXList( LPTSTR lpf );
extern   BOOL  InRList( LPTSTR lpl, DWORD dwLen );
extern   PRL  InrList( LPTSTR lpl, DWORD dwLen );

#define  IS_FILE     1
#define  IS_FOLDER   2
#define  IS_WILD     4
extern   DWORD    IsValidFile4( LPTSTR lpf, PWIN32_FIND_DATA pfd );
extern   BOOL     gmGetFullPath( LPTSTR lpd, LPTSTR lpf, DWORD dws );
extern   BOOL     gmGetFileTitle( LPTSTR lpt, LPTSTR lpf, DWORD dws );
extern   VOID     gmGetFolder( LPTSTR lpd, LPTSTR lpf );
extern   LPTSTR   GetShortName( LPTSTR lps, DWORD dwmx );
extern   LPTSTR   GetNxtBuf( VOID );

#define     iCreate     0
#define     iAddRpl     1
#define     iSetRE      2

extern   PVOID Add2FRList( LPTSTR lpf, INT flg );

extern   DWORD    TrimIB( LPTSTR lps );
extern   DWORD    LTrimIB( LPTSTR lps );
extern   DWORD    RTrimIB( LPTSTR lps );
extern   INT      InStr( LPTSTR lpb, LPTSTR lps );
extern   LPTSTR   FixNum( LPTSTR lpb );
extern   DWORD    Asc2FileTime( LPTSTR lpb, FILETIME * pft );
extern   INT      InStrIS( LPTSTR lpb, LPTSTR lps );
extern INT InStrWhole( LPTSTR lpb, LPTSTR lps );

extern   VOID     GetModulePath( LPTSTR lpb );

// ALLOCATE/FREE functions
extern   LPTSTR   ALeft( LPTSTR lpl, DWORD dwi );
extern   LPTSTR   FLeft( LPTSTR lps );

extern   VOID	BumpFileName( LPTSTR lps );   // bump a file name

// ******************* Apr 2004 ******************
extern TCHAR g_szActFullPath[]; // [_MAX_FNAME];
extern TCHAR g_szActDrive[]; // [_MAX_DRIVE];
extern TCHAR g_szActFolder[]; // [_MAX_DIR];
extern TCHAR g_szActTitle[]; // [_MAX_FNAME];
extern TCHAR g_szActExt[]; // [_MAX_EXT]; // in stdlib.h ...
extern void SetGlobalFileNames( PTSTR pfile );
// *******************

#endif	// _FixFUtil_H
// eof - FixFUtil.h
