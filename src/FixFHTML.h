
// FixFHTML.h
#ifndef	_FixFHTML_H
#define	_FixFHTML_H

extern	BOOL	Do_HTML( LPTSTR lpb, DWORD dws, LPTSTR lpf );

#define  MXCRMSGS    10

// exported by FixFHTML.c
// data
extern BOOL  bExclMLComm;
extern BOOL  bAddnpsp;
extern BOOL  bCheckIf;
extern TCHAR gszMSIf[]; // [264];
extern INT   iMSIf;
extern DWORD gdwIfCnt;
extern INT  _s_AddCRMsg;
// services
extern VOID  Show_H_Help( LPTSTR lpc );
extern   LPTSTR   GetExBrace( LPTSTR lps );  // unpack a string (ms?) if
extern   LPTSTR   GetExBraceEx( LPTSTR lps, DWORD dws );   // see if we ave ENDIF!!!
extern   DWORD    BufHasTailChar( LPTSTR lpb, DWORD dwMax, INT chr );
extern BOOL  BufGotEnd( LPTSTR lpb, PDWORD pdw, DWORD dws, INT i );  // == '>' ) )
extern INT   GetHEX2( LPTSTR lpb );

// shared macro - very specific
#define  Plus(a,b)   ( lpb[ dwi + a ] == b )

#endif	// _FixFHTML_H
// eof - FixFHTML.h

