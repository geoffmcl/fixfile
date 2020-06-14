

// FixFHtmlA.c

#include	"FixF32.h"

char * p_file_name;
int done_hdr;
int done_para;
int para_cnt;
DWORD h_in_lns; // in/out line counter
DWORD h_out_lns;
DWORD h_out_tot;
DWORD h_add_cnt;
char * para_opn = "<p>"MEOR;
char * para_clo = "</p>"MEOR;

char * line_brk = "<br>";
char * eol = MEOR;
char * htm_header1 = "<html><head><title>";
char * htm_header2 = "</title>"MEOR
"</head>"MEOR
"<body bgcolor=\"#EEFFFF\">"MEOR;


char * htm_top = "<a name=\"pagetop\"></a>"MEOR;
char * htm_totop = "<a href=\"#pagetop\">top</a><br>"MEOR;

char * htm_header3 = "<h1 align=\"center\">";
char * htm_header4 = "</h1>"MEOR;

char * htm_backout = "<p align=\"center\">"MEOR
"<a href=\"#\" onClick=\"history.go(-1)\">Back</a>"MEOR
"<input type=button value=\"Back\" onClick=\"history.go(-1)\">"MEOR
"</p>"MEOR;


char * htm_header5 = "</body></html>"MEOR;

char sz_file_name[264];
#define  TO_TOP_MAX        25    // put a TOP marker, each this number

// ==========================================================
// helper functions
// convert UTC FILETIME to local FILETIME and then
//     local   FILETIME to local SYSTEMTIME
/*
from : https://www.tutorialspoint.com/c_standard_library/c_function_localtime.htm
struct tm {
   int tm_sec;         / * seconds,  range 0 to 59          * /
   int tm_min;         / * minutes, range 0 to 59           * /
   int tm_hour;        / * hours, range 0 to 23             * /
   int tm_mday;        / * day of the month, range 1 to 31  * /
   int tm_mon;         / * month, range 0 to 11             * /   
   int tm_year;        / * The number of years since 1900   * /
   int tm_wday;        / * day of the week, range 0 to 6    * / 
   int tm_yday;        / * day in the year, range 0 to 365  * /
   int tm_isdst;       / * daylight saving time             * /
};

from : https://docs.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-systemtime
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

*/
BOOL Is_Valid_ST(SYSTEMTIME* pst)
{
    if (pst->wYear > 30827)
        return FALSE;
    if ((pst->wMonth < 1) || (pst->wMonth > 12))
        return FALSE;
    if ((pst->wDay < 1) || (pst->wDay > 31))
        return FALSE;
    if ((pst->wHour < 0) || (pst->wHour > 23))
        return FALSE;
    if ((pst->wMinute < 0) || (pst->wMinute > 59))
        return FALSE;
    if ((pst->wSecond < 0) || (pst->wSecond > 59))
        return FALSE;
    return TRUE;
}

BOOL     FT2LST( FILETIME * pft, SYSTEMTIME * pst )
{
   BOOL  bRet = FALSE;
#ifdef USE_COMP_FIO
   time_t local = pft->dwLowDateTime;
   // (pft->dwHighDateTime << 32)
   struct tm* info = localtime(&local);
   if (info) {
       pst->wYear = info->tm_year + 1900;
       pst->wMonth = info->tm_mon + 1;
       pst->wDayOfWeek = info->tm_wday;
       pst->wDay = info->tm_mday;
       pst->wHour = info->tm_hour;
       pst->wMinute = info->tm_min;
       pst->wSecond = info->tm_sec;
       pst->wMilliseconds = 0;
       if (Is_Valid_ST(pst))
           bRet = TRUE;
   }

#else
   FILETIME    ft;
   if( ( FileTimeToLocalFileTime( pft, &ft ) ) && // UTC file time converted to local
       ( FileTimeToSystemTime( &ft, pst)     ) )
   {
      bRet = TRUE;
   }
#endif
   return bRet;
}

INT	GetFDTYear( FILETIME * pft )
{
   INT         i = 0;
   SYSTEMTIME  st;
   if( FT2LST( pft, &st ) )
      i = st.wYear;

   return i;
}


// add seconds
static TCHAR _s_GetFDTSStg[64];
LPTSTR	GetFDTSStg( FILETIME * pft )
{
   LPTSTR   lps = _s_GetFDTSStg; // _sGetSStg();
   SYSTEMTIME  st;
   //FILETIME    ft;
   //if( ( FileTimeToLocalFileTime( pft, &ft ) ) && // UTC file time converted to local
   //    ( FileTimeToSystemTime( &ft, &st)     ) )
   if( FT2LST( pft, &st ) )
   {
      sprintf(lps,
         "%02d/%02d/%04d %02d:%02d:%02d",
         (st.wDay & 0xffff),
         (st.wMonth & 0xffff),
         (st.wYear & 0xffff),
         (st.wHour & 0xffff),
         (st.wMinute & 0xffff),
         (st.wSecond & 0xffff) );
   }
   else
   {
		strcpy( lps, "??/??/?? ??:??:??" );
   }
   return lps;
}

LPTSTR	GetFDTStg( FILETIME * pft )
{
   LPTSTR lps = GetFDTSStg(pft);
   LPTSTR p = strrchr(lps,':');
   if(p) *p = 0;
   return lps;
}

// =================

void add_size_time(char * lb, WIN32_FIND_DATA * pfd)
{
   LARGE_INTEGER li;
   li.LowPart = pfd->nFileSizeLow;
   li.HighPart = pfd->nFileSizeHigh;
   //sprintf(lb, "Loading file [%s],", &pfd->cFileName[0] ); // file );
   //sprintf(lb, "%s,", &pfd->cFileName[0] ); // file );
   sprintf(EndBuf(lb), " %s bytes, ", Get_I64_Stg(li.QuadPart));
   strcat(lb, GetFDTStg( &pfd->ftLastWriteTime ));
}

int set_file_load( char * lb, char * file )
{
    int iret = 0;
    WIN32_FIND_DATA fd;
#ifdef USE_COMP_FIO
    struct stat buf;
    if (stat(file, &buf))
        goto No_Go;
    if (buf.st_mode & M_IS_DIR)
        goto No_Go;
    strcpy(lb, file);
    fd.ftLastWriteTime.dwHighDateTime = buf.st_mtime >> 32;
    fd.ftLastWriteTime.dwLowDateTime = buf.st_mtime & 0xFFFFFFFF;
    fd.nFileSizeHigh = 0;
    // fd.nFileSizeHigh = buf.st_size >> 32;
    fd.nFileSizeLow = buf.st_size & 0xFFFFFFFF;
    add_size_time(lb, &fd); // GetLoadingMsg(lb, &fd);
    return 1;
No_Go:
    sprintf(lb, "Can NOT locate [%s] file!", file);
    //LOG(lb);
    iret = 0;
#else
   MYHAND hand = FindFirstFile(file, &fd);
   if( VFH(hand) ) {
      FindClose(hand);
      if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
         goto No_Go;
      strcpy(lb, fd.cFileName);
      add_size_time(lb, &fd); // GetLoadingMsg(lb, &fd);
      iret = 1;
   } else {
       // *** NO GO ***
No_Go:
      sprintf(lb, "Can NOT locate [%s] file!", file);
      //LOG(lb);
      iret = 0;
   }
#endif 

   return iret;

}


VOID set_sz_name( LPTSTR pfn )
{
   if( !set_file_load( sz_file_name, pfn ) ) {
      // alternative ...
      strcpy(sz_file_name, GetShortName(pfn,200) );
   }
}


DWORD oi( LPTSTR p )
{
   DWORD len = strlen(p);
   outit(p,len);
   return len;
}

VOID dooutit( LPTSTR lpb, DWORD dwo )
{
   int i;
   // add the HTML
   if( !done_hdr ) {
      done_hdr = 1;
      i = 0;
      i += oi(htm_header1); // = "<html><head><title>";
      i += oi(sz_file_name);
      i += oi(htm_header2); // = "</title></head><body>"MEOR;
      i += oi(htm_top); // = "<a name=\"pagetop\"></a>"MEOR;

      i += oi(htm_header3); // = "<h1 align=\"center\">";
      i += oi(sz_file_name); // prepared title
      i += oi(htm_header4); // = "</h1>"MEOR;
      i += oi(htm_backout); // = "<p align=\"center\">"MEOR

      h_add_cnt += i;
      h_out_tot += i;
   }
   if( !done_para ) {
      i = oi(para_opn);
      h_add_cnt += i;
      h_out_tot += i;
      done_para = 1;
   }

   if(dwo) {
      outit(lpb,dwo);
      h_out_tot += dwo;
      i = oi(line_brk);
      h_add_cnt += i;
      h_out_tot += i;
   }

   h_out_tot += oi(eol);

   h_out_lns++;

   if( ( h_out_lns % TO_TOP_MAX ) == 0 ) {

      i = oi(htm_totop); // = "<a href=\"#pagetop\">top</a>"MEOR;
      h_add_cnt += i;
      h_out_tot += i;

   }
}

BOOL Do_Add_HTML( LPTSTR lpb, DWORD dws, LPTSTR lpf )
{
   LPTSTR ptmp;
   DWORD  dwi, dwo;
   INT    i, c, d;
   LPTSTR lpout = &gszOutBuf[0];

   p_file_name = lpf;
   h_out_lns = 0;
   dwo = 0;
   h_add_cnt = 0;
   h_in_lns = h_out_lns = 0;
   set_sz_name( lpf );
   sprintf(lpout,"Processing %s"MEOR, sz_file_name);
   d = 0;
   done_para = para_cnt = 0;
   done_hdr = 0;
   for( dwi = 0; dwi < dws; dwi++ )
   {
      ptmp = &lpb[dwi]; // set the current pointer
      c = *ptmp;        // load the next
      lpout[dwo++] = (TCHAR)c;
      if( c == 0x0a )   // if a LF
      {
         dwo--; // back off end of line
         if( d != 0x0d )   // NOT preceeded by a CR
         {
            // enforce program rule
            //lpout[dwo++] = 0x0d;
            h_add_cnt++;
#ifndef  NDEBUG
            if( VERB8 )
            {
               _s_AddCRMsg++;
               if( _s_AddCRMsg < MXCRMSGS )  // say 10
               {
                  AddReport( "Added CR (0x0d) to line %d (%d)."MEOR, h_in_lns, h_out_lns );
               }
            }
#endif   // !NDEBUG
         }
      }
      else if( c == 0x0d ) // if a CR, then may be just first of a pair
      {
         dwo--; // back off end of line
         if( (dwi + 1) < dws )
         {
            // if there is more in the buffer,
            // AND it is NOT a LF
            if( lpb[ dwi + 1 ] != 0x0a )
            {
               // then enforce program rule for EOL
               //lpout[dwo++] = (TCHAR)c;   // add the CR
               c = 0x0a;
               h_add_cnt++;
#ifndef  NDEBUG
               if( VERB8 )
               {
                  //_s_AddLFMsg++;
                  //if( _s_AddLFMsg < MXCRMSGS )  // say 10
                  {
                     AddReport( "Added LF (0x0a) to line %d (%d)."MEOR, h_in_lns, h_out_lns );
                  }
               }
#endif   // !NDEBUG
            }
         }
         else
         {
            //lpout[dwo++] = (TCHAR)c;
            c = 0x0a;
            h_add_cnt++;
#ifndef  NDEBUG
            if( VERB8 )
            {
               AddReport( "Added LF (0x0a) to line %d (%d)."MEOR, h_in_lns, h_out_lns );
            }
#endif   // !NDEBUG
         }
      }

      if( c == 0x0a ) {
         lpout[dwo] = 0;
         dooutit(lpout,dwo); // out a LINE
         dwo = 0;
      }
      d = c;
   } // cycle for complete buffer
   
   if(dwo) {
      lpout[dwo] = 0;
      dooutit(lpout,dwo);
      dwo = 0;
   }

   if( done_para ) {
      i = oi(para_clo);
      h_out_tot += i;
      h_add_cnt += i;
      done_para = 0;
   }
   if( done_hdr && ( h_out_lns % TO_TOP_MAX ) ) {

      i = oi(htm_totop); // = "<a href=\"#pagetop\">top</a>"MEOR;
      h_add_cnt += i;
      h_out_tot += i;
   }

   if( done_hdr ) {
      i = 0;
      i += oi(htm_backout); // = "<p align=\"center\">"MEOR
      i += oi(htm_header5); // = "</body></html>"MEOR;
      h_out_tot += i;
      h_add_cnt += i;
      done_hdr = 0;
   }

   return TRUE;
}

// eof - FixFHtmlA.c
