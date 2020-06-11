

// FixFHelp.h
#ifndef	_FixFHelp_H
#define	_FixFHelp_H

extern	VOID  Give_Help( INT iRet );
extern   BOOL  Chk_Command( INT icnt, LPTSTR * lpa );
extern   VOID  ShowCmd( VOID );
extern   BOOL  Got_FileC( LPTSTR lpc );

// FIX20051205 - add delete hi-bit characters - only ASCII output
//	dmsg(	" -dh[s]     Delete hi-bit characters -dhs substitute a space."MEOR );
extern int g_DelHiBit, g_SubSpace;

#endif	// _FixFHelp_H
// eof - FixFHelp.h

