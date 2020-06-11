

// FixGeoff.h
#ifndef  _FixGeoff_HH
#define  _FixGeoff_HH

extern   BOOL  DoGeoff2( LPTSTR lpf );
extern   BOOL  DoGeoff1( LPTSTR lpf );
extern   BOOL  Get_G_Cmd( LPTSTR lpc );
typedef BOOL (*GEOFFCMD) (PTSTR);

#endif   // _FixGeoff_HH
// eof - FixGeoff.h
