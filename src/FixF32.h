
// FixF32.h
#ifndef	_FixF32_H
#define	_FixF32_H
#ifdef  __cplusplus
extern  "C"
{
#endif  // __cplusplus

#ifdef _MSC_VER
#pragma warning(disable:4996)   // added for MSVC8
#endif

#include "FixFVers.h" // version, and other FIRST things

#include <stdio.h>
#ifdef _WIN32
//////////////////////////////////////////////////////////////
#define	WIN32_MEAN_AND_LEAN
#include	<windows.h>
#include	<direct.h>
#ifdef USE_COMP_FIO
typedef FILE* MYHAND;
typedef MYHAND* PMYHAND;
#else // !USE_COMP_FIO
typedef HANDLE MYHAND;
typedef MYHAND* PMYHAND;
#endif // USE_COMP_FIO
//////////////////////////////////////////////////////////////
#else // !_WIN32 == unix
/////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> // for strlen, ...
#include <stdint.h>
#include <inttypes.h> // for "%" PRId64 " bytes"
#include <ctype.h> // for toupper, ...
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef FILE * MYHAND;
typedef MYHAND* PMYHAND;
typedef bool BOOL;
typedef BOOL* PBOOL;
typedef char * LPTSTR;
typedef char * PTSTR;
typedef char * LPSTR;
typedef int INT;
typedef char TCHAR;
typedef long LONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef __int64_t __int64;
typedef void * PVOID;
#define TRUE true
#define FALSE false
typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    LONGLONG QuadPart;
} LARGE_INTEGER;
typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    };
    ULONGLONG QuadPart;
} ULARGE_INTEGER;
typedef struct _LIST_ENTRY {
    struct _LIST_ENTRY *Flink;
    struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;
typedef void VOID;
typedef DWORD * PDWORD;
#define INVALID_HANDLE_VALUE NULL
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, * PSYSTEMTIME, * LPSYSTEMTIME;

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME;
typedef struct _WIN32_FIND_DATA {
    DWORD dwFileAttributes;
    //FILETIME ftCreationTime;
    //FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    char  cFileName[264];
    char   cAlternateFileName[14];
} WIN32_FIND_DATA, * PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;
#define FILE_BEGIN SEEK_SET // 1 Beginning of file
#define FILE_CURRENT SEEK_CUR // 2 Current position of the file pointer1
#define FILE_END SEEK_END // 3 End of file
#define _strnicmp strnicmp
////////////////////////////////////////////////
#endif
#include <stdio.h>      // for vsprintf()

#include "FixFList.h"
#include "FixFWork.h"   // work structure
#include "FixFUtil.h"
#include "FixFObj.h"    // -f = Function List Extraction
#include "FixFHelp.h"   // Give_Help()
#include "FixFHTML.h"   // process a HTML file
#include "FixFDir.h"   // process a DIR file
#include "FixGeoff.h"   // process my particular PHONE file
#include "FixXML.h"  // some XML constants
#include "FixFHtmlA.h"  // add HTML FIX20041107
#include "FixVC.h" // FIX20050203 - added -c output in 'text' format for C/C++
#include "FixFMap.h" // File mapping functions

#define  EndBuf(a)      ( a + strlen(a) )
#define  Chk_Num(c)   ( ( c >= '0' ) && ( c <= '9' ) )

#define  ISUPPER(a)     (( a >= 'A' ) && ( a <= 'Z' ))
#define  ISLOWER(a)     (( a >= 'a' ) && ( a <= 'z' ))
#define  ISALPHA(a)     ( ISUPPER(a) || ISLOWER(a) )
#define  ISNUM(a)       (( a >= '0' ) && ( a <= '9' ))
#define  ISALPHANUM(a)  ( ISALPHA(a) || ISNUM(a) )
#define  ISHEX(a)   ( ISNUM(a) ||\
      ( ISUPPER(a) && ( a >= 'A' ) && ( a <= 'F' ) ) ||\
      ( ISLOWER(a) && ( a >= 'a' ) && ( a <= 'f' ) ) )

extern   BOOL  outit( LPTSTR lpb, DWORD dwLen );
extern   VOID  Pgm_Exit( INT iRet );   // single exit to application
extern   BOOL  Chk_Dupes( LPTSTR lpout, PDWORD pdw, BOOL bCrLf );
extern   BOOL  Chk_Remove( LPTSTR lpout, LPTSTR lpbgn, PDWORD pdw, DWORD dwLine );

extern   DWORD Pgm_Flag;
extern   TCHAR End_Msg[];  // = "eof\r\n";   // eof plus CR,LF

extern   MYHAND  Open_A_File( LPTSTR lpf );
extern   MYHAND   Creat_A_File( LPTSTR lpf );


#ifdef  __cplusplus
//extern  "C"
}
#endif  // __cplusplus
#endif	// _FixF32_H
// eof - FixF32.h
