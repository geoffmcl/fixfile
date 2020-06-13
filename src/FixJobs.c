
// FixJobs.c
#include "FixF32.h"
//#include "jobfile.hxx"
// #define  Do_Jobs     0x00400000  // -j process job file
// #define  Do_JobsF    0x00800000  // -jf fix job file
// #define  Do_JobsE    0x01000000  // -je fix email tags
// XML format
extern BOOL grmWriteFile( MYHAND * ph, LPTSTR lpb );
extern PTSTR g_pActMetChr;
#define MEMERR(a) if(a == 0) { chkme( "C:ERROR: We have NO MEMORY !!! EEEK !" ); }

// macro flags
#define  DoJobFix    ( Pgm_Flag & Do_JobsF )   // -jf
#define  DoJobFixE   ( Pgm_Flag & Do_JobsE )   // -je
#define  DoJobFixS   ( Pgm_Flag & Do_JobsS )   // -js
#define  DoJobFixW   ( Pgm_Flag & Do_JobsW )   // -jwnn
#define  DoJobFixD   ( Pgm_Flag & Do_JobsD )   // -jd delete head/tail of paste
#define  DoJobFixDP  ( Pgm_Flag & Do_JobsDP )   // -jd+ write delete to file
#define  DoJobFixT   ( Pgm_Flag & Do_JobsT )   // -jt trailing "<" fixed

#define  jf_InLabel  0x00000001  // openned "<"
#define  jf_EndLab1  0x00000002  // had "/" as FIRST
#define  jf_EndLab2  0x00000004  // had "/" as LAST
#define  jf_GotXML10 0x00000008  // had "<?xml version="1.0" ?>"
#define  jf_GotCom1  0x00000010  // had "<?xml version="1.0" ?>"
#define  jf_GotCom2  0x00000020  // had "<?xml version="1.0" ?>"
#define  jf_IsEmail  0x00000040  // had "<a@b.c>"
#define  jf_HadLabel 0x00000080  // had a LABEL in this LINE

// transient and other flags
//    if( DoJobFixD ) {
#define jf_InHdr1    0x00000100
#define jf_InHdr2    0x00000200
#define jf_InHdr   (jf_InHdr1 | jf_InHdr2)
#define jf_HadHdr1   0x00000400
#define jf_HadHdr2   0x00000800
#define jf_AllHdr ( jf_InHdr | jf_HadHdr1 | jf_HadHdr2 )
#define jf_InDel     0x00001000  // delete g_dwDelCnt of the stream - NOT USED!
#define jf_ToEOL     0x00002000  // delete to end of real line

#define  jf_HadFixT  0x00400000  // show only one -jt fix
#define  jf_HadFixE  0x00800000  // show only one -je fix
#define  jf_GotHiBit 0x01000000  // found a NON-ASCII char NO NO
#define  jf_WaitDn   0x02000000  // done a LENGTH remaining check
#define  jf_WaitEnd  0x04000000  // wait for end of line - ie override wrap for line
#define  jf_GotEmail 0x08000000  // got -je, and <a@b.c>, fixed
#define  jf_GotFatal 0x10000000  // *** GOT FATAL NON-FIXABLE ERROR ***
#define  jf_Closed   0x20000000  // this entry is CLOSED
#define  jf_HadOpen  0x40000000  // had primary open
#define  jf_GotError 0x80000000  // XML error
// original was to ONLY output the FIRST error,
// but if the fix is ON, then can continue with decode,
// BUT some errors, that HAVE NO FIX, should be FATAL type

#define  Label_Flags (jf_InLabel|jf_EndLab1|jf_EndLab2| \
   jf_GotXML10|jf_GotCom1|jf_GotCom2|jf_IsEmail)

#define  Wait_Flags  ( jf_WaitDn | jf_WaitEnd )

#define  XMLERR   "XMLERR="
#define  FXMLERR  MEOR"FATAL XMLERR="
#define  DELERR   MEOR"Warning="

TCHAR	g_szHelpj[] =
	"Switch -j[defstwo]: Dump as job file."MEOR
      // case 'D': Pgm_Flag |= Do_JobsD;
   "-jd   Delete header and tail from email paste."MEOR
      // case 'E': Pgm_Flag |= Do_JobsE;   // -je = DoJobFixE
   "-je   Fix email, http, and br tags, spacing out gt/lt"MEOR
      // case 'F': Pgm_Flag |= Do_JobsF;   // -jf
   "-jf   Fix output, where possible ..."MEOR
      // case 'O':   // -jo[output file] GetExQuotes(lpc); _fullpath gcGOutFile
   "-joOutFile Set output file name, in addition to -oname command."MEOR
      // case 'S': Pgm_Flag |= Do_JobsS;   // -js
   "-js   Space removal, in output."MEOR
      // case 'T': Pgm_Flag |= Do_JobsT;   // -jt
   "-jt   Trailing label openner converted to space."MEOR
      // case 'W': Pgm_Flag |= Do_JobsW;   // -jw DEF_WRAP_LEN g_dwWrap
   "-jw[nn] Wrap lines at this maximum value, on spaces, def=68..."MEOR
   "                                                  Happy File Fixing!";

TCHAR g_szXMLVer[] = "?xml version=\"1.0\" ?";

// Generic Email
TCHAR g_szHgb1[] = "geoffmclane@hotmail.com  MSN Home   |";
TCHAR g_szHgt1[] = "Report and Block Sender ";
TCHAR g_szHgb2[] = "|  |    |  |  | Inbox ";
TCHAR g_szHgt2[] = "Privacy Statement";

// IT2
TCHAR g_szHdb1[] = "geoffmclane@hotmail.com  MSN Home";
TCHAR g_szHdt1[] = "15153750";
TCHAR g_szHdb2[] = "MODIFY PROFILE";
TCHAR g_szHdt2[] = "Privacy Statement";

// ICON
TCHAR g_szHdb3[] = "geoffmclane@hotmail.com  MSN Home";
TCHAR g_szHdt3[] = "18103020";
TCHAR g_szHdb4[] = "MODIFY PROFILE";
TCHAR g_szHdt4[] = "Privacy Statement";

// SEEK
TCHAR g_szHdb5[] = "geoffmclane@hotmail.com  MSN Home";
TCHAR g_szHdt5[] = "15317621";
TCHAR g_szHdb6[] = "MODIFY PROFILE";
TCHAR g_szHdt6[] = "Privacy Statement";

// MYC
TCHAR g_szHdb7[] = "geoffmclane@hotmail.com  MSN Home";
//TCHAR g_szHdt7[] = "Here are the new jobs matching your job alert profile:";
TCHAR g_szHdt7[] = "http://mycareer.com.au/jobseeker/redirect.aspx?action=modify";
TCHAR g_szHdb8[] = "*Figures may include benefits and/or";
TCHAR g_szHdt8[] = "Privacy Statement";

// GREY
TCHAR g_szHdb9[] = "geoffmclane@hotmail.com  MSN Home";
TCHAR g_szHdt9[] = "Report and Block Sender";
TCHAR g_szHdb10[] = " |  |    |  |  | Inbox";
TCHAR g_szHdt10[] = "Privacy Statement";

enum AgTag {
   tg_SEEK,
   tg_IT2,
   tg_JKM,
   tg_MYC,
   tg_ICON,
   tg_GREY,
   tg_LOU,
   tg_JOBS,
   tg_AMBITION,
   tg_GENERIC,    // generic tag, if NOT one of the above
   tg_MAX
};

enum PtrTag {
   pt_Agency,
   pt_BgnH1,
   pt_EndH1,
   pt_BgnH2,
   pt_EndH2,
   pt_COUNT
};

//    if( DoJobFixD ) {
#define  AGTBLARRAY     pt_COUNT  // columns
PTSTR g_pAgTbls[AGTBLARRAY*tg_MAX] = {
   "SEEK", g_szHdb5, g_szHdt5, g_szHdb6, g_szHdt6 ,
   "IT2", g_szHdb1, g_szHdt1, g_szHdb2, g_szHdt2 ,
   "JKM", g_szHgb1, g_szHgt1, g_szHgb2, g_szHgt2 ,
   "MYC", g_szHdb7, g_szHdt7, g_szHdb8, g_szHdt8 ,
   "ICON", g_szHdb3, g_szHdt3, g_szHdb4, g_szHdt4 ,
   "GREY", g_szHdb9, g_szHdt9, g_szHdb10, g_szHdt10 ,
   "LOU", g_szHdb9, g_szHdt9, g_szHdb10, g_szHdt10 ,
   "JOBS", g_szHdb9, g_szHdt9, g_szHdb10, g_szHdt10 ,
   "AMBITION", g_szHdb9, g_szHdt9, g_szHdb10, g_szHdt10 ,
   "*", g_szHgb1, g_szHgt1, g_szHgb2, g_szHgt2 ,
};

DWORD g_dwJFlag;
DWORD g_dwWrap = DEF_WRAP_LEN;
DWORD g_dwDelCnt = 0; // delete until zero, 
// jf_InDel delete g_dwDelCnt of the stream
DWORD g_dwRemove_Flag;

#define  In_Label    ( g_dwJFlag & jf_InLabel  )
#define  Do_FixEE    ( g_dwJFlag & jf_GotEmail )
#define  Wait_End    ( g_dwJFlag & jf_WaitEnd  )

#define  Set_Flag(a) (g_dwJFlag |= a)
#define  Remove_Flag(a) (g_dwJFlag &= ~(a))
#define  Got_Flag(a) (g_dwJFlag & a)
#ifndef  LE
#define  LE LIST_ENTRY
#endif   // no LE defined

//   DWORD j_dwType;   // is format AGENCY+MONTH+DAY+COUNT
#define t_M    0x00000001 // got month first
#define t_MD   0x00000002 // is MONTH+DAY
#define t_A    0x00000004 // got agency first

#define t_AMD  0x00000010 // is format AGENCY+MONTH+DAY
#define t_AMDN 0X00000020 // is format AGENCY+MONTH+DAY+COUNT

typedef struct {
   LE j_list;  // double linked list entry
   DWORD j_dwFlag;   // bit flags
   DWORD j_dwLevel;  // open xml tags
   DWORD j_dwLine;   // line number in original
   DWORD j_dwLine1;   // line number in out file
   //BOOL  j_isAMNN;   // is format AGENCY+MONTH+DAY+COUNT
   DWORD j_dwType;   // bit flags, like t_AMDN = AGENCY+MONTH+DAY+COUNT
   DWORD j_dwAMon; // 0-11 Month
   INT   j_iADate; // 1-31 Day in Month
   DWORD j_dwActAge; // 0-? Agency offset
   DWORD j_dwAgMon; // 0-11 Month
   INT   j_iAgDate; // 1-31 Day in Month
   INT   j_iAgCount; // count of items
   INT   j_cLast; // last char _ maybe a plus sign
   TCHAR j_cbuff[1]; // data item - variable length
} JXML, * PJXML;

LIST_ENTRY  g_sLines = { &g_sLines, &g_sLines };
LIST_ENTRY  g_sXMLTags = { &g_sXMLTags, &g_sXMLTags };

DWORD dwXMLCnt;
DWORD dwXMLOpn;
DWORD dwLnCnt;

// DWORD gConverted;
DWORD gLnsOut;
DWORD gLnsDat; // lines of data within a label

TCHAR g_szFixed[] = MEOR" *FIXED*";

// quite rough compare of two labels
// note: CASE & '/' ARE IGNORED
// ****************************
BOOL SameXMLLabel( PTSTR pb1, PTSTR pb2 )
{
   DWORD cnt = 0;
   while( *pb1 && *pb2 ) {
      if( *pb1 == '/' ) {
         pb1++;
         continue;
      }
      if( *pb2 == '/' ) {
         pb2++;
         continue;
      }

      if(( *pb1 <= ' ' ) ||
         ( *pb2 <= ' ' ) ) {

         // XML LABEL ENDED
         if(( cnt ) &&
            ( *pb1 <= ' ' ) &&
            ( *pb2 <= ' ' ) )
            return TRUE;


         return FALSE;

      }


      if( toupper(*pb1) == toupper(*pb2) )
      {
         cnt++;   // count a MATCH
         pb1++;
         pb2++;
      }
      else
      {
         return FALSE;
      }

   }

   if( cnt )
      return TRUE;

   return FALSE;
}

#define  Reverse_List(pH,pN)\
   for( pN = pH->Blink; pN != pH; pN = pN->Blink )


PTSTR pLastOpen = "";
PLE   pLastLab = 0;
#define  MXLAST   16
PLE   pLastLabs[MXLAST] = { 0 };

#define SETLASTLAB(a) { \
   PLE __pn1,__pn2 = a; INT __i; \
for( __i = 0; __i < MXLAST; __i++ ) { \
__pn1 = pLastLabs[__i]; \
pLastLabs[__i] = __pn2; \
__pn2 = __pn1; } }

PTSTR pMonth[] = {
   "JAN",
   "FEB",
   "MAR",
   "APR",
   "MAY",
   "JUN",
   "JUL",
   "AUG",
   "SEP",
   "OCT",
   "NOV",
   "DEC",
   0
};


DWORD g_dwActAge = (DWORD)-1;
DWORD g_dwAgMon = (DWORD)-1;
INT   g_iAgDate;
INT   g_iAgCount;

DWORD g_dwAMon = (DWORD)-1;
INT   g_iADate;

BOOL IsAgMonth( PTSTR pa )
{
   PTSTR * plist = &pMonth[0];

   g_dwAgMon = 0;
   while( *plist ) {
      if( strcmpi( *plist, pa ) == 0 ) {
         return TRUE;
      }

      g_dwAgMon++;
      plist++;
   }


   g_dwAgMon = (DWORD)-1;
   return FALSE;
}

BOOL IsAMonth( PTSTR pa )
{
   PTSTR * plist = &pMonth[0];

   g_dwAMon = 0;
   while( *plist ) {
      if( strcmpi( *plist, pa ) == 0 ) {
         return TRUE;
      }

      g_dwAMon++;
      plist++;
   }


   g_dwAMon = (DWORD)-1;
   return FALSE;
}

//#define  AGTBLARRAY     5  // columns
//PTSTR g_pAgTbls[] = {
#define  SHOWAGENCY     VERB9
BOOL IsAgency( PTSTR pa )
{
//   PTSTR * plist = &pAgency[0];
   PTSTR * plist = &g_pAgTbls[0];
   DWORD cnt = tg_MAX;
   DWORD Prev = g_dwActAge;
   g_dwActAge = 0;
   for( cnt = 0; cnt < tg_MAX; cnt++ ) {
      plist = &g_pAgTbls[ (cnt * AGTBLARRAY) ];
      if( *plist && ( strcmpi( *plist, pa ) == 0 ) ) {
         if(SHOWAGENCY) {
            if( Prev != g_dwActAge ) {
               sprtf( "Set AGENCY to %s ..."MEOR, *plist );
            }
         }
         return TRUE;
      }
      g_dwActAge++;
   }
   //g_dwActAge = (DWORD)-1;
   
   g_dwActAge = (DWORD)tg_GENERIC;
   if(SHOWAGENCY) {
      if( Prev != g_dwActAge ) {
         plist = &g_pAgTbls[ ((tg_MAX - 1) * AGTBLARRAY) ];
         if(*plist)
            sprtf( "Set AGENCY to %s ..."MEOR, *plist );
         else
            sprtf( "Set AGENCY to BLANK! ..."MEOR, *plist );
      }
   }
   return FALSE;
}

// add LINE to LINE double linked LIST
// ===================================
VOID addLine2LineList(PSTR pline)
{
   PLE   phl = &g_sLines;
   PLE   pnl;
   PJXML pjx;
   PTSTR lpb; // = &pjx->j_cbuff[0];
   DWORD len = strlen(pline);

   pnl = MALLOC( sizeof(JXML) + len );
   MEMERR(pnl);
   ZeroMemory(pnl,( sizeof(JXML) + len ));
   pjx = (PJXML)pnl;
   lpb = &pjx->j_cbuff[0];
   strcpy(lpb,pline);

   pjx->j_dwAMon = g_dwAMon; // 0-11 Month
   pjx->j_iADate = g_iAgDate; // 1-31 Day in Month

   pjx->j_dwActAge = g_dwActAge; // 0-? Agency offset
   pjx->j_dwAgMon = g_dwAgMon; // 0-11 Month
   pjx->j_iAgDate = g_iAgDate; // 1-31 Day in Month
   pjx->j_iAgCount = g_iAgCount; // count of items
   if(len)
      pjx->j_cLast = pline[len-1]; // get any tail character
   pjx->j_dwFlag = g_dwJFlag; // import active flag
   pjx->j_dwLevel = dwXMLOpn;
   InsertTailList(phl,pnl);
}


void ChkJobLab( PJXML pjx )
{
   PTSTR lpb = &pjx->j_cbuff[0];
   PTSTR ptmp = gszTmpBuf;
   DWORD cnt;
   // if AGENCY + MONTH + DATE [ + COUNT ]
   // Step 1 = Kick off any END1 forward slash
   while( *lpb && ( *lpb == '/' ) ) {
      lpb++;
   }
   cnt = 0;
   while( *lpb && ISALPHANUM(*lpb) ) {
      ptmp[cnt++] = *lpb;  // commect leading alpha/number string
      lpb++;
   }
   ptmp[cnt] = 0; // close AGENCY
   if( cnt && IsAgency( ptmp ) ) {
      pjx->j_dwType |= t_A; // got Agency
      // Step 2 - We have a valid agency - kick it off
      while( *lpb && !ISALPHANUM(*lpb) ) {
         lpb++;
      }
      cnt = 0;
      while( *lpb && ISALPHANUM(*lpb) ) {
         ptmp[cnt++] = *lpb;
         lpb++;
      }
      ptmp[cnt] = 0;
      if( cnt && IsAgMonth( ptmp ) ) {
         // Step 3 - Have month. Get the day
         while( *lpb && !ISALPHANUM(*lpb) ) { lpb++; }
         if( ISNUM( *lpb ) ) {
            cnt = 0;
            while( *lpb && ISNUM(*lpb) ) {
               ptmp[cnt++] = *lpb;
               lpb++;
            }
            ptmp[cnt] = 0;
            g_iAgDate = atoi(ptmp);
            if( g_iAgDate > 0 ) {
               pjx->j_dwType |= t_AMD; // got Agency,Month,Date
               g_iAgCount = -1;
               // Step 4 - Have DATE, get COUNT indication
               while( *lpb && !ISALPHANUM(*lpb) ) { lpb++; }
               if( ISNUM( *lpb ) ) {
                  cnt = 0;
                  while( *lpb && ISNUM(*lpb) ) {
                     ptmp[cnt++] = *lpb;
                     lpb++;
                  }
                  ptmp[cnt] = 0;
                  g_iAgCount = atoi(ptmp);
                  if(g_iAgCount > 0) {
                     //pjx->j_isAMNN = TRUE;   // is format AGENCY+MONTH+DAY+COUNT
                     pjx->j_dwType |= t_AMDN; // got Agency,Month,Date,Count
                  } else {
                     g_iAgCount = -1;
                  }
               }
            }
         }
      }
   } else if( cnt && IsAMonth( ptmp ) ) {
      pjx->j_dwType |= t_M; // got Month
      // Repeat of Step 3 - Have month. Get the day
      while( *lpb && !ISALPHANUM(*lpb) ) { lpb++; }
      if( ISNUM( *lpb ) ) {
         cnt = 0;
         while( *lpb && ISNUM(*lpb) ) {
            ptmp[cnt++] = *lpb;
            lpb++;
         }
         ptmp[cnt] = 0;
         g_iADate = atoi(ptmp);
         if( g_iADate > 0 ) {
            pjx->j_dwType |= t_MD; // got Month,Day
         }
      }
   }

   pjx->j_dwAMon = g_dwAMon; // 0-11 Month
   pjx->j_iADate = g_iADate; // 1-31 Day in Month

   pjx->j_dwActAge = g_dwActAge; // 0-? Agency offset
   pjx->j_dwAgMon = g_dwAgMon; // 0-11 Month
   pjx->j_iAgDate = g_iAgDate; // 1-31 Day in Month
   pjx->j_iAgCount = g_iAgCount; // count of items
   pjx->j_cLast = *lpb; // get any tail character
}

MYHAND g_hOutFile = 0;
#define  grmCreateFile  Creat_A_File

VOID CreatJOut( VOID )
{
   LPTSTR   lpf  = &gcGOutFile[0];   // = [264] = {"\0"};
   g_hOutFile = (MYHAND)-1;
   if(*lpf)
      g_hOutFile = grmCreateFile( lpf );
}

VOID Write_J_Label( PTSTR pline )
{
   if(pline)
   {
      PTSTR pg = gszGenBuf;
      strcpy(pg,"<");
      strcat(pg, pline);
      strcat(pg,">");
      addLine2LineList(pg);
      if( g_hOutFile == 0 )
         CreatJOut();
      if( VFH(g_hOutFile) )
      {
         strcat(pg, MEOR );
         grmWriteFile( &g_hOutFile, pg );
      }
   }
}

void  Add2XML( PTSTR pl, DWORD line )
{
   DWORD len = 0;
   PTSTR perr = gszErrBuf; // W.ws_szErrBuf  // [1024]
   if( pl )
      len = strlen(pl);
   if( len ) {
      PLE   ph = &g_sXMLTags; // XML line list
      DWORD size = (sizeof(JXML) + len);
      PLE   pn = MALLOC(size);

      MEMERR(pn);
      Write_J_Label( pl );
      if(pn) {
         PJXML pjx2; // = (PJXML)pn2;
         PTSTR lpb2; // = &pjx2->j_cbuff[0];
         PLE   pn2;
         PJXML pjx = (PJXML)pn;
         //PTSTR lpb = (PTSTR)((PLE)pn + 1);
         PTSTR lpb = &pjx->j_cbuff[0];

         ZeroMemory(pjx, size); // note ensure all zero start

//         pjx->j_dwLine = line;   // 1, 2, 3, ... etc
//         pjx->j_dwlevel = dwXMLOpn + 1;
         pjx->j_dwFlag = g_dwJFlag; // import active flag
         strcpy(lpb,pl);
         pjx->j_dwLevel = dwXMLOpn;
         // InsertTailList(ph,pn);

         if( Got_Flag( jf_EndLab1 ) ) {
            // this is a CLOSE as first CHAR
            pjx->j_dwFlag |= jf_Closed;   // CLOSED due to END CHAR '/' first char
            if( SameXMLLabel( lpb, pLastOpen ) ) {
               pn2 = pLastLab;
               pjx2 = (PJXML)pn2;
               lpb2 = &pjx2->j_cbuff[0];
               pjx2->j_dwFlag |= jf_Closed;   // CLOSED

               if( dwXMLOpn )
               {
                  dwXMLOpn--;
               }
               else
               {
                  strcpy(perr,FXMLERR);
                  sprintf(EndBuf(perr),"Ln%4d:%4d ",
                        line,
                        (gLnsOut + 1));
                  strcat(perr,"Errant XML close with none open!");
                  strcat(perr,MEOR);

                  Add2DLList(perr);
                  if( !Got_Flag(jf_GotFatal) ) {  // FATAL XML error

                     Set_Flag( jf_GotFatal );
                     sprtf(perr);
                  }
               }

               // This CLOSES all BACK to this LABEL
               Reverse_List( ph, pn2 )
               {
                  // PJXML pjx2 = (PJXML)pn2;
                  // PTSTR lpb2 = &pjx2->j_cbuff[0];
                  pjx2 = (PJXML)pn2;
                  lpb2 = &pjx2->j_cbuff[0];
                  if( !(pjx2->j_dwFlag & jf_Closed) ) {   // CLOSED
                     pLastOpen = lpb2;   // keep LAST OPEN XML
                     pLastLab = pn2;
                     SETLASTLAB(pn2);
                     break;
                  }
               }
            } else {
               // last pointer - like poped off the stack is NOT correct
               strcpy(perr,FXMLERR);
               sprintf(EndBuf(perr),"Ln%4d:%4d ",
                        line,
                        (gLnsOut + 1));
               strcat(perr,"Errant XML close not current open!");
               strcat(perr,MEOR);
               // NOT if( SameXMLLabel( lpb, pLastOpen ) ) {
               sprintf(EndBuf(perr),"C=<%s> vs L=<%s>!"MEOR, lpb, pLastOpen );
               Add2DLList(perr);
               if( !Got_Flag(jf_GotFatal) ) {  // XML error

                  Set_Flag( jf_GotFatal );
                  sprtf(perr);

               }
            }

         } else if( Got_Flag( jf_EndLab2 ) ) {
            // this is a CLOSE as last CHAR
// #define  jf_Closed   0x20000000  // this entry is CLOSED
            pjx->j_dwFlag |= jf_Closed;   // CLOSED
         } else if( Got_Flag( jf_GotCom1 ) && Got_Flag( jf_GotCom1 ) ) {
            pjx->j_dwFlag |= jf_Closed;   // CLOSED
         } else {
            pLastOpen = lpb;   // keep LAST OPEN XML
            pLastLab = pn;
            SETLASTLAB(pn);

            if( dwXMLOpn == 0 ) {
               if( Got_Flag( jf_HadOpen ) ) {
                  // sprintf(perr,FXMLERR"Ln%4d: Errant XML 2nd primary open!"MEOR,
                  //      line );
                  strcpy(perr,FXMLERR);
                  sprintf(EndBuf(perr),"Ln%4d:%4d ",
                        line,
                        (gLnsOut + 1));
                  strcat(perr,"Errant XML 2nd primary open!");
                  strcat(perr,MEOR);
                  Add2DLList(perr);
                  if( !Got_Flag(jf_GotFatal) ) {  // XML error

                     Set_Flag( jf_GotFatal );
                     sprtf(perr);

                  }

               }
            }
            dwXMLOpn++;
            Set_Flag( jf_HadOpen );
         }

         pjx->j_dwLine = line;   // 1, 2, 3, ... etc
//         pjx->j_dwLevel = dwXMLOpn;
         pjx->j_dwLine1 = gLnsOut + 1;

         InsertTailList(ph,pn);
         dwXMLCnt++;  // count an XML in
         ChkJobLab( pjx ); // decode type

      } else {
         chkme("C:ERROR No Memory!"MEOR);
      }
   }
}

void OutXMLLabs( void )
{
   PTSTR perr = gszErrBuf; // W.ws_szErrBuf  // [1024]
   PLE   ph = &g_sXMLTags;
   PLE   pn;
   DWORD dwc;
   PTSTR pform;

   ListCount2(ph,&dwc);
   if(dwc == 0) {
      sprtf("There are NO xml labels found."MEOR );
      return;
   }


   sprtf("Found %d xml labels ..."MEOR, dwc );

   Traverse_List( ph, pn )
   {
      PJXML pjx = (PJXML)pn;
      //PTSTR lpb = (PTSTR)((PLE)pn + 1);
      PTSTR lpb = &pjx->j_cbuff[0];
      DWORD dwi = pjx->j_dwLevel;
      //sprtf("Line %d: %s"MEOR,
      sprintf(perr, "Line %4d:%4d ",
         pjx->j_dwLine,
         pjx->j_dwLine1 );

      if( dwi && ( pjx->j_dwFlag & jf_EndLab1 ))
         dwi--;   // put END back to PARENT OPENNER LEVEL

      // modify maximim level
      if(dwi > 40) dwi = 40;
      while(dwi != 0) {
         strcat(perr,"   ");
         dwi--;
      }

      strcat(perr, lpb  );

      // ======================
//         if( Got_Flag( jf_EndLab1 ) ) {
            // this is a CLOSE as first CHAR
//         } else if( Got_Flag( jf_EndLab2 ) ) {
            // this is a CLOSE as last CHAR
// #define  jf_Closed   0x20000000  // this entry is CLOSED
//            pjx->j_dwFlag |= jf_Closed;   // CLOSED

      if( pjx->j_dwFlag & jf_EndLab1 )
         strcat( perr, " End1" );

      if( pjx->j_dwFlag & jf_EndLab2 )
         strcat( perr, " End2" );

      // ======================
      // if( pjx->j_isAMNN ) {   // is format AGENCY+MONTH+DAY+COUNT
      if( pjx->j_dwType & t_AMD ) { // got Agency,Month,Date
         pform = " (a=%d m=%d d=%d c=?)";
         if( pjx->j_dwType & t_AMDN ) // got Agency,Month,Date,Count
            pform = " (a=%d m=%d d=%d c=%d)";
         sprintf(EndBuf(perr), pform,
            pjx->j_dwActAge, // 0-? Agency offset
            pjx->j_dwAgMon, // 0-11 Month
            pjx->j_iAgDate, // 1-31 Day in Month
            pjx->j_iAgCount ); // count of items, if valid
      } else if( pjx->j_dwType & t_MD ) { // got Month,Day
         pform = " (m=%d d=%d)";
         sprintf(EndBuf(perr), pform,
            pjx->j_dwAMon, // 0-11 Month
            pjx->j_iADate ); // 1-31 Day in Month
      }

      strcat(perr, MEOR );
      sprtf(perr);

   }
   sprtf("Listed %d xml labels."MEOR, dwc );
}


BOOL IsBlankLine(PTSTR p)
{
   if( *p == 0 )
      return TRUE;

   while( *p <= ' ' )
   {
      if( *p == 0 )
         return TRUE;
      p++;
   }

   return FALSE;
}

//DWORD gConverted;
//DWORD gLnsOut;
VOID Write_J_Ouput( PTSTR pline )
{
   if(pline)
   {
      PTSTR pg = gszGenBuf;
      *pg =0;
      if( Got_Flag( jf_HadLabel ) ) {
         DWORD len2 = strlen(pline);
         if( len2 && ( pline[len2-1] != '>' ) ) {
            // NOTE this only get the BEST case
            PTSTR ptmp = strrchr(pline,'>');
            if(ptmp != 0) {
               ptmp++;
               while( *ptmp && (*ptmp <= ' ') ) ptmp++;
               strcpy(pg,ptmp);
            } else {
               strcpy(pg, pline);
            }
         } else {
            // have one or more labels or a blank
         }
      } else {
         strcpy(pg, pline);
      }
      if(*pg) {
         addLine2LineList(pg);
         if( g_hOutFile == 0 )
            CreatJOut();
         if( VFH(g_hOutFile) )
         {
            strcat(pg, MEOR );
            grmWriteFile( &g_hOutFile, pg );
         }
      }
   }
}

#define M_INSTR   (( p                ) && \
                   ( len >= strlen(p) ) && \
                   ( InStr( pline, p ) > 0 ))

TCHAR g_szDiscFile[] = "tempdisc.txt";
MYHAND   g_hDiscFile = 0;
DWORD g_dwDelLnsS = 0;
DWORD g_dwDelLns = 0;

void WriteDiscard( PTSTR pline, DWORD linenum, DWORD len )
{
   PTSTR pg = gszGenBu2;
   if(g_hDiscFile == 0) {
      if( DoJobFixDP ) { // were we given a -jd+, write delete
         g_hDiscFile = grmCreateFile( g_szDiscFile );
         sprintf(pg,"Discard line file [%s]"MEOR, g_szDiscFile );
         if( VFH(g_hDiscFile) ) {
            grmWriteFile( &g_hDiscFile, pg );
         } else {
            g_hDiscFile = INVALID_HANDLE_VALUE;
         }
      } else {
            g_hDiscFile = INVALID_HANDLE_VALUE;
      }
   }

   // if we have a DISCARD file
   if( VFH(g_hDiscFile) ) {
      strcpy(pg,"[");
      strcat(pg,pline);
      //strcat(pg,"]"MEOR);
      sprintf(EndBuf(pg),"]%4d(%d)"MEOR, linenum,len);
      grmWriteFile( &g_hDiscFile, pg );
   }

   if(len)
      g_dwDelLns++;
   else
      g_dwDelLnsS++; // spacey lines
}

void CloseDiscard(void)
{
   PTSTR pg = gszGenBu2;
   sprintf(pg,"Discarded %d lines, %d spacey ..."MEOR,
      (g_dwDelLns + g_dwDelLnsS),
      g_dwDelLnsS);
   if(g_dwDelLns || g_dwDelLnsS) {
      if( VFH(g_hDiscFile) ) {
         sprintf(EndBuf(pg),"Written to %s file... closing ..."MEOR,
            g_szDiscFile );
      }
      if(VERB)
         sprtf(pg);
   }

   if( VFH(g_hDiscFile) ) {
      grmWriteFile( &g_hDiscFile, pg );
   }
}

#define SHOWDISCARD(a,b,c)  WriteDiscard(a,b,c)

// #define  DoJobFixW   ( Pgm_Flag & Do_JobsW )   // -jwnn
// DWORD g_dwWrap = 40;
VOID OutJFile( DWORD type, PTSTR pline, DWORD len, DWORD linenum )
{
   PTSTR pg = gszGenBuf;
//   DWORD len;
//   PLE   phl = &g_sLines;
//   PLE   pnl;
//   PJXML pjx;
//   PTSTR lpb; // = &pjx->j_cbuff[0];
//   char * p;

   if( DoJobFixS ) { // add SPACE supression
      if( IsBlankLine(pline) ) {
         SHOWDISCARD(pline,linenum,0);
         gDiscard += (len + 2);
         return;
      }
   }

   // *** DELETE DATA FLAG IS UP ***
   // care: Could lose GOOD data, if search faulty!!!
   if(( DoJobFixD ) &&
      ( g_dwActAge != -1 ) &&
      ( g_dwActAge < tg_MAX ) &&
      ( !Got_Flag(jf_GotFatal) ) ) {
      // NO FATAL XML error, proceed with DELETE
      PTSTR p = 0;
      PTSTR * pstrs = &g_pAgTbls[ (g_dwActAge * AGTBLARRAY) ];
      DWORD iflag = 0;
      DWORD idisk = 0;
      // check for PASTE headers/tailers to delete
//#define jf_InHdr1    0x00000100
//#define jf_InHdr2    0x00000200
//#define jf_InHdr   (jf_InHdr1 | jf_InHdr2)
      if( Got_Flag(jf_InHdr) ) {
         // ***GOT SOME HEADER ACTIVE***
         // looking for END of deleted section
         if( Got_Flag(jf_InHdr1) ) {
            p = pstrs[2];
            if( M_INSTR ) {
               Remove_Flag(jf_InHdr1);
               Set_Flag(jf_HadHdr1);
               iflag++;
            }
            SHOWDISCARD(pline,linenum,len);
            gDiscard += (len + 2);
            idisk++;
            //return;
         } else {
            p = pstrs[4];
            if( M_INSTR ) {
               Remove_Flag(jf_InHdr2);
               Set_Flag(jf_HadHdr2);
               iflag++;
            }
            SHOWDISCARD(pline,linenum,len);
            gDiscard += (len + 2);
            idisk++;
            //return;
         }
      } else if( !Got_Flag(jf_HadHdr2) ) {
         // no header
         if( Got_Flag( jf_HadHdr1 ) ) {
            // get START of HEADER 2
            p = pstrs[3];
            if( M_INSTR ) {
               Set_Flag( jf_InHdr2 );
               SHOWDISCARD(pline,linenum,len);
               gDiscard += (len + 2);
               //iflag++;
               idisk++;
               //return;
            }
         } else {
            //#define jf_HadHdr2   0x00000800
            // get START of HEADER 1
            p = pstrs[1];
            if( M_INSTR ) {
               Set_Flag( jf_InHdr1 );
               SHOWDISCARD(pline,linenum,len);
               gDiscard += (len + 2);
               //iflag++;
               idisk++;
               //return;
            }
         }
      }

      if(idisk) {
         // discard this line
         if( iflag && type ) {

            Set_Flag( jf_ToEOL );
         }
         return;
      }
   }

   // check for WAIT UNTIL FILE END OF LINE to END DISCARDING
   if( Got_Flag( jf_ToEOL ) ) {
      if( type == 0 ) {
         // real END OF LINE, after WRAPPING
         Remove_Flag( jf_ToEOL );
      }

      SHOWDISCARD(pline,linenum,len);
      gDiscard += (len + 2);

      return;
   }
   // add LINE to LINE double linked LIST
   //addLine2LineList(pline);

   Write_J_Ouput( pline );

   //len = sprintf(pg,"%s"MEOR, pline);
   strcpy(pg, pline);
   strcat(pg, MEOR );
//   len = strlen(pg);
//   p = strrchr(pg,'>');

//   sprtf(pg); // note may have "%" format openner, unescaped, so
//   outit(pg,len); // includes gdwWritten += len;
//   if( Got_Flag( jf_HadLabel ) ) {
   outit(pg,strlen(pg)); // includes gdwWritten += len;
   gLnsOut++;
}

BOOL g_gotat = FALSE;
BOOL g_gotdot = FALSE;
BOOL g_gotdot2 = FALSE;
BOOL g_isbr = FALSE;
// return TRUE if like
// <Greythorn@greythorn.com>
// FIX20040427 - include Uwe's SPECIAL   "<"<>
BOOL EmailTagEx( PTSTR lpb, DWORD dwn )
{
   PTSTR pb = lpb;
   DWORD len = dwn;
   //BOOL gotat = FALSE;
   //BOOL gotdot = FALSE;
   //BOOL gotdot2 = FALSE;
   INT   c;
   PTSTR pln1 = gszLineB1;  // add it into here
   DWORD cnt = 0;

   g_gotat = FALSE;
   g_gotdot = FALSE;
   g_gotdot2 = FALSE;
   g_isbr = FALSE;
   if( pb && len && ( *pb == '<' ) ) {
      pb++;
      len--;
      c = *pb;
      while( c && len ) {
         // assume NOT an EMAIL (or HTTP, domain link string) if any of these
//         if(( c == '>') || ( c <= ' ' ) || ( c == '/'))
         if(( c == '>') || ( c <= ' ' ) ) // this ENDS THE SCAN
            break;

         if( cnt < 1023 )
            pln1[cnt++] = *pb;

         if( c == '@' )
            g_gotat = TRUE;
         else if( c == '.' ) {
            g_gotdot = TRUE;
            if(g_gotat)
               g_gotdot2 = TRUE;
         }
         // on to NEXT char
         pb++; // bump, and
         c = *pb; // get NEXT
         len--; // reduce length
      }

      //if( gotat && ( c == '>' ) && gotdot2 )
      if( c == '>' ) {
         if( g_gotdot ) // && gotdot2 )
            return TRUE;
         // FIX20040329 - EXCLUDE <br> tag
         if( cnt == 2 ) {
            pln1[cnt] = 0;
            if( strcmpi(pln1,"br") == 0) {
               g_isbr = TRUE;
               return TRUE;
            }
         }
      }
   }
   return FALSE;
}

DWORD Len2End( PTSTR pb, DWORD dwmax )
{
   DWORD len = 0;
   INT   c = *pb;
   while(c && dwmax) {
      if(( c < ' ')&&( c != '\t' ))
         break;
      len++;   // bump kength to END
      pb++; // bump to next
      c = *pb; // update char
      dwmax--;
   }
   return len;
}

#define  OUTLINE(a)  OutJFile( a, pln3, dwc, linecnt )


#ifdef USE_64_BITS
#define M_GOTMORE  ((uli.QuadPart+1) < ulib.QuadPart)
#define M_REMAINS   (ulib.QuadPart - uli.QuadPart)
#define M_BUMPCNT uli.QuadPart++
#else // !#ifdef USE_64_BITS
#define M_GOTMORE  ((uli+1) < ulib)
#define M_REMAINS   (ulib - uli)
#define M_BUMPCNT uli++
#endif // #ifdef USE_64_BITS y/n

#define  ILNCNT  ( linecnt + 1 )

BOOL  DoJobs( PTSTR lpf )
{
   PLE   ph = &g_sXMLTags;
   PLE   phl = &g_sLines;
   PTSTR pln2 = gszLineB2;  // add it into here
   PTSTR pln3 = gszLineB3;  // build it into here
#ifdef USE_64_BITS
   ULARGE_INTEGER  uli, ulib;
#else // !#ifdef USE_64_BITS
   DWORD  uli, ulib;
#endif // #ifdef USE_64_BITS y/n
   PBYTE   lpbgn, lpb, pblock;
   DWORD    c, d;
   DWORD    dwc, dwn, dwi;
   //DWORD    lines = 0;
   DWORD linecnt = 0;
   DWORD linelen = 0; // length of BUFFER line
   PTSTR perr = gszErrBuf; // W.ws_szErrBuf  // [1024]
   PTSTR p;
   DWORD dwoff;
   PMPDFIL pmf = GetFileMap( lpf );

   KillLList(ph); //    PLE   ph = &g_sXMLTags;
   KillLList(phl); //    PLE   ph = &g_sLines;
   dwLnCnt = 0;
   g_dwJFlag = 0;
   dwXMLCnt = 0;
   dwXMLOpn = 0;
   gDiscard = 0;
   gAdded   = 0;
   gdwWritten = 0;
   gConverted = 0;
   gLnsOut = 0;
   gLnsDat = 0; // data lines within a LABEL
	if( !pmf )
		return FALSE;

#ifdef USE_64_BITS
   ulib.QuadPart = pmf->sliSize.QuadPart;
#else // !#ifdef USE_64_BITS
   if( pmf->sliSize.HighPart ) {
      ulib = (DWORD)-1;
   } else {
      ulib = pmf->sliSize.LowPart;
   }
#endif // #ifdef USE_64_BITS y/n
   lpbgn = (PBYTE)pmf->pData;
   d = 0;
   pblock = 0;
   dwc = dwn = 0;
   pln2[dwn] = 0;
   *perr = 0;

#ifdef USE_64_BITS
   ulib.QuadPart = pmf->sliSize.QuadPart;
   sprtf( "Processing %s of %s bytes ..."MEOR,
               GetShortName(lpf,40),
       Get_I64_Stg(ulib.QuadPart));
   uli.QuadPart  = 0;
   for( ; uli.QuadPart < ulib.QuadPart; uli.QuadPart++ )
#else // !#ifdef USE_64_BITS
   sprtf( "Processing %s of %s bytes ..."MEOR,
               GetShortName(lpf,40),
       Get_I64_Stg(pmf->sliSize.QuadPart) );
   uli  = 0;
   for( ; uli < ulib; uli++ )
#endif // #ifdef USE_64_BITS y/n
//   ojf( lpf );
   // process the buffer, line by line
   {
#ifdef USE_64_BITS
      lpb = &lpbgn[ uli.QuadPart ];
#else // !#ifdef USE_64_BITS
      lpb = &lpbgn[ uli ];
#endif // #ifdef USE_64_BITS y/n
      c = *lpb;   // extract character

#ifdef   ADD_HIGH_BIT_CHECK2
      // XML is 7-bit = ASCII
      if( c & 0x80 ) {
         // hi-bit in an ASCII field
         Set_Flag(jf_GotHiBit); // found a NON-ASCII char NO NO
         sprintf(perr, XMLERR"Ln%4d: Errant XML character set!"MEOR,
                        (lines + 1) );
// gsReport - diagnostic report list = PDL

                  if( !Got_Flag(jf_GotError) ) {  // XML error

                     Set_Flag( jf_GotError );

                     dwi = dwc;  // line count at present
                     pln3[dwi++] = (TCHAR)c;
                     pln3[dwi++] = '?';
                     pln3[dwi] = 0;
                     sprtf("Line %d:[%s]"MEOR, (lines + 1), pln3 );
                     sprtf(perr);
                  }
         Add2DLList(perr);

      }
#endif // #ifdef   ADD_HIGH_BIT_CHECK2

      // *** IN LABEL ***
      if( In_Label ) {
// ******************************************************************
         // seeking END label
         // *****************
         if(( c == '>' ) ||
            ( ( c < ' ' ) && ( c != '\t' ) ) )
         {
            // got END
            pln2[dwn] = 0;
            pln3[dwc] = 0;
            if( c != '>' ) {
               strcpy(perr,FXMLERR);
               sprintf(EndBuf(perr),"Ln%4d:%4d ",
                     ILNCNT,  // lines + 1,
                     (gLnsOut + 1));
               strcat(perr,"Errant XML end of line closure!");
               strcat(perr,MEOR);
               sprintf(EndBuf(perr),"Line=[%s]%d"MEOR, pln3, dwc);
               if(dwn) {
                  // label ended by end of line!
                  sprintf(EndBuf(perr),"Label=[%s]%d ended by EOL!"MEOR, pln2, dwn);
               } else {
                  sprintf(EndBuf(perr),"Label began at EOL!"MEOR );
               }
               Add2DLList(perr);
               if( !Got_Flag(jf_GotFatal) ) {  // XML error
                  Set_Flag( jf_GotFatal );
                  sprtf(perr);
               }
            } else {
               // IS REALLY LABEL END - that is we have an '>' char
               if( DoJobFixE &&  // gotEmailTagEx() ) {
                  Do_FixEE  ) {  // Set_Flag(jf_GotEmail);
                  gConverted++; // convert to a SPACE
                  c = ' '; // change char to SPACE
                  Remove_Flag(jf_GotEmail); // *** DONE THE EMAIL *** Do_FixEE=FALSE
                  // and DO NOT add this line to XML tags
                  // ************************************
               } else {
                  Add2XML(pln2, ILNCNT);
                  Set_Flag( jf_HadLabel );
               }
            }

            // Remove_Flag( (jf_InLabel | jf_EndLab1 | jf_EndLab2) );
            Remove_Flag( Label_Flags );
            dwn = 0;
            pblock = lpb;
            gLnsDat = 0; // data lines within a LABEL
         } else {
            // *** waiting for XML label END ***
            if( DoJobFixE && Do_FixEE ) { // in an email fix
               // so nothing to do here
            } else if( c == '/' ) {

               if( dwn == 0 ) {
                  Set_Flag( jf_EndLab1 );
               } else {
                  if(( M_GOTMORE ) &&
                     ( lpb[1] == '>' ) ) {

                     // negation last char
                     Set_Flag( jf_EndLab2 );
                  }
               }
            } else if( c == '?' ) {
               if( dwn == 0 ) {
                  Set_Flag( jf_GotCom1 );
               } else {
                  if(( M_GOTMORE ) &&
                     ( lpb[1] == '>' ) ) {
                     // comment last char
                     Set_Flag( jf_GotCom2 );
                  }
               }
            }

            // add label character
            if( dwn < DMAXLINE )
               pln2[dwn++] = (TCHAR)c; // add character to xml LABEL string

            if( c > ' ' ) {
               if( DoJobFixE &&  // gotEmailTagEx() ) {
                    Do_FixEE &&
                    ( c == '@' ) ) {  // Set_Flag(jf_GotEmail);
                  dwoff = 1; // am FIXING "<a@b.c>" to " a@b.c "! So LET '@' thru
               } else {
                  dwoff = IsValidXMLChar( (TCHAR)c, 1 ); 
               }
               if( dwoff == 0 ) {
                  strcpy(perr,FXMLERR);
                  sprintf(EndBuf(perr),"Ln%4d:%4d ",
                        ILNCNT,
                        (gLnsOut + 1));
                  sprintf(EndBuf(perr),"Invalid XML character! %c IL!!",
                        c );
                  strcat(perr,MEOR);
                  dwi = dwc;  // line counter at present, left unchanged
                  pln3[dwi++] = (TCHAR)c;
                  pln3[dwi++] = '?';
                  pln3[dwi] = 0;
                  sprintf(EndBuf(perr),"Line %d:[%s]"MEOR,
                     ILNCNT,
                     pln3 );
                  Add2DLList(perr);

                  if( !Got_Flag(jf_GotFatal) ) {  // XML error
                     Set_Flag( jf_GotFatal );
                     sprtf(perr);
                  }
               }
            }
         }
// ******************************************************************
      }
      else // if( !In_Label )
      {
// ******************************************************************
         // NOT IN LABEL
         // ************
         g_dwRemove_Flag = 0;
         if( c == '<' ) {
            // got BEGIN OF LABEL, MAYBE!!!
            dwn = (DWORD) M_REMAINS;
            //if( DoJobFixE && gotEmailTagEx() ) {
            // jf_IsEmail = "<a@b.c>"
// FIX20040427 - include Uwe's SPECIAL   "<"<>
            if(( d == '"' ) &&
               ( dwn >= 3 ) &&
               ( lpb[1] == '"' ) &&
               ( lpb[2] == '<' ) &&
               ( lpb[3] == '>' )) {
               Set_Flag(jf_InDel);
               gDiscard++;
               g_dwDelCnt = 3;
//            pln3[dwc++] = (TCHAR)c;
               if(dwc) {
                  gDiscard++;
                  dwc--;
                  if(linelen) // if line LENGTH
                     linelen--;
               }
               while(g_dwDelCnt) {
                  gDiscard++;
                  g_dwDelCnt--;
                  M_BUMPCNT;
               }
               Remove_Flag(jf_InDel);
               continue;
            }
// FIX20040427 - include Uwe's SPECIAL   "<"<>

            if( EmailTagEx( (PTSTR)lpb, dwn ) )
               Set_Flag(jf_IsEmail);
            else
               Remove_Flag(jf_IsEmail);

            if( Got_Flag(jf_IsEmail) ) {
               // this xml tag LOOK LIKE AN EMAIL expression
               // ******************************************
               if( DoJobFixE ) { // if given -je FIX THIS EMAIL TAG
// #define  jf_HadFixE  0x00800000  // show only one -je fix
                  // simply by repacing the < & > with space,
                  // and excusing the '@' not allowed in a tag???
                  Set_Flag(jf_GotEmail); // set Do_FixEE to TRUE!
                  gConverted++; // convert to a SPACE
                  c = ' '; // change to SPACE
               } else {
                  // else NOT NEEDED, because in LABEL collection
                  // will flag an error when the '@' is encountered!!!
                  gLnsDat = 0; // data lines within a LABEL
                  g_dwRemove_Flag |= jf_AllHdr; // Remove_Flag(jf_AllHdr);
                  Set_Flag( jf_InLabel );
               }
            } else {
               if( DoJobFixT ) { //  ( Pgm_Flag & Do_JobsT )   // -jt trailing "<" fixed
                  dwn = (DWORD) M_REMAINS;
                  //if( Len2End( (PTSTR)lpb, dwn ) < 2 ) {
                  if( dwn && ( lpb[1] < ' ' ) ) {
// #define  jf_HadFixT  0x00400000  // show only one -jt fix
                     sprintf(perr,XMLERR"Ln%4d:%4d Invalid XML start label! %c NIL"MEOR,
                              ILNCNT,
                              (gLnsOut + 1),
                              c );
                     dwi = dwc;  // line count at present
                     pln3[dwi++] = (TCHAR)c;
                     pln3[dwi++] = '?';
                     pln3[dwi] = 0;
                     sprintf(EndBuf(perr),"Line %d:[%s]%d",
                        ILNCNT,
                        pln3,
                        dwi );

                     strcat(perr, g_szFixed); // " *FIXED*");
                     strcat(perr,MEOR);
                     if( !Got_Flag( jf_HadFixT ) ) {
                        sprtf(perr);
                        Set_Flag( jf_HadFixT );
                     }
                     Add2DLList(perr);
                     gConverted++; // convert to a SPACE
                     c = ' '; // change to SPACE
                  } else {
                     gLnsDat = 0; // data lines within a LABEL
                     g_dwRemove_Flag |= jf_AllHdr; // Remove_Flag(jf_AllHdr);
                     Set_Flag( jf_InLabel );
                  }
               } else {
                  gLnsDat = 0; // data lines within a LABEL
                  g_dwRemove_Flag |= jf_AllHdr; // Remove_Flag(jf_AllHdr);
                  Set_Flag( jf_InLabel );
               }
            }

            // Set_Flag( jf_InLabel );
            if(( g_dwRemove_Flag & jf_InHdr ) &&
               ( Got_Flag( jf_InHdr )       )) {
               dwi = dwc;  // line count at present
               sprintf(perr,DELERR"Ln%4d:%4d In delete at XML start label! NIL"MEOR,
                              ILNCNT,
                              (gLnsOut + 1) );
               strcat(perr,MEOR);
               //if( !Got_Flag( jf_HadFixT ) ) {
                  sprtf(perr);
               //   Set_Flag( jf_HadFixT );
               //}
               Add2DLList(perr);
            }

            // now REMOVE all the delete label flags
            Remove_Flag(g_dwRemove_Flag); // = jf_AllHdr; // Remove_Flag(jf_AllHdr);
            pblock = lpb;
            dwn = 0; // start xml LABEL counter
            // ================================
         } else if( c == '&' ) {
            // we have beginning of META character
            dwn = (DWORD) M_REMAINS;
            dwoff = ValidXMLMetaChar( (PTSTR)lpb, dwn );
            if( dwoff ) {
//   { "space, non-breaking",0,"&nbsp;"," " },
               if( g_pActMetChr && (strcmp(g_pActMetChr, "&nbsp;") == 0) ) {
                  dwoff = 0; // This is an ERROR
               }
            }
            if( !dwoff ) {
               PTSTR ptmp = gszTmpBuf; // this was set up in FixXML.c!!!
               sprintf(perr,XMLERR"Ln%4d:%4d Invalid XML meta character! [%s] NIL"MEOR,
                        ILNCNT,
                        (gLnsOut + 1),
                        ptmp );
               dwi = dwc;  // line count at present
               pln3[dwi++] = (TCHAR)c;
               pln3[dwi++] = '?';
               pln3[dwi] = 0;
               sprintf(EndBuf(perr),"Line %d:[%s]",
                  ILNCNT,
                  pln3 );

               if( DoJobFix ) { //    ( Pgm_Flag & Do_JobsF )   // -jf
                  //p = GetXMLMetamp();
                  p = GetXMLMeta4('&'); // convert "&" to "&amp;"
                  if( p && *p && (p[ strlen(p) - 1 ] == ';') ) {
                     while( *p && ( *p != ';' ) ) {
                        if( dwc < DMAXLINE ) {
                           pln3[dwc++] = *p;
                           gAdded++;
                        }
                        p++; // to next char
                     }
                     c = *p; // ';';
                  } else {
                     if( dwc < DMAXLINE ) { pln3[dwc++] = (TCHAR)c; gAdded++; }
                     if( dwc < DMAXLINE ) { pln3[dwc++] = 'a'; gAdded++;      }
                     if( dwc < DMAXLINE ) { pln3[dwc++] = 'm'; gAdded++;      }
                     if( dwc < DMAXLINE ) { pln3[dwc++] = 'p'; gAdded++;      }
                     c = ';';
                  }
                  strcat(perr, g_szFixed);
               }

               strcat(perr,MEOR);
               Add2DLList(perr);

               if( !Got_Flag(jf_GotError) ) {  // XML error
                  Set_Flag( jf_GotError );
                  sprtf(perr);
               }

            }

            dwn = 0;
         } else if( c > ' ' ) {

            //dwoff = IsValidXMLChar( (TCHAR)c, 0 );
            if( DoJobFixE &&  // gotEmailTagEx() ) {
                 Do_FixEE &&
                 ( c == '>' ) ) {  // Set_Flag(jf_GotEmail);
// #define  jf_HadFixE  0x00800000  // show only one -je fix
               dwoff = 1; // am FIXING "<a@b.c>" to " a@b.c "! So LET '@' thru
               Remove_Flag(jf_GotEmail);
               gConverted++; // convert to a SPACE
               c = ' '; // change to a SPACE
               p = XMLERR"Ln%4d:%4d Email/URL XML label! NIL"MEOR;
               if(g_isbr)
                  p = XMLERR"Ln%4d:%4d Errant BR XML label! NIL"MEOR;
               sprintf(perr,p, ILNCNT, (gLnsOut + 1) );
               dwi = dwc;  // line count at present
               pln3[dwi++] = (TCHAR)c;
               pln3[dwi++] = '?';
               pln3[dwi] = 0;
               sprintf(EndBuf(perr),"Line %d:[%s]%d",
                        ILNCNT,
                        pln3,
                        dwi );
               strcat(perr, g_szFixed); // " *FIXED*");
               strcat(perr,MEOR);
               if( !Got_Flag( jf_HadFixE ) ) {
                        sprtf(perr);
                        Set_Flag( jf_HadFixE );
               }
               Add2DLList(perr);
            } else {
               dwoff = IsValidXMLChar( (TCHAR)c, 0 ); 
            }

            if( dwoff == 0 ) {
               sprintf(perr,XMLERR"Ln%4d:%4d Invalid XML character! %c NIL"MEOR,
                        ILNCNT,
                        (gLnsOut + 1),
                        c );
               dwi = dwc;  // line count at present
               pln3[dwi++] = (TCHAR)c;
               pln3[dwi++] = '?';
               pln3[dwi] = 0;
               sprintf(EndBuf(perr),"Line %d:[%s]",
                  ILNCNT,
                  pln3 );

               if( DoJobFix ) { //    ( Pgm_Flag & Do_JobsF )   // -jf
                  // convert character to meta character ASCII
                  p = GetXMLMeta4( (TCHAR) c ); // convert say ">" to "&gt;"
                  if(p && *p && (p[ strlen(p) - 1 ] == ';') ) {
                     while( *p && ( *p != ';' ) ) {
                        if( dwc < DMAXLINE ) {
                           pln3[dwc++] = *p;
                           gAdded++;
                        }
                        p++; // to next char
                     }
                     c = *p; // ';';
                  } else {
                     c = ' ';
                     gConverted++; // convert to a SPACE
                  }
                  strcat(perr, g_szFixed); // " *FIXED*");
               }

               strcat(perr,MEOR);

               if( !Got_Flag(jf_GotError) ) {  // XML error
                  Set_Flag( jf_GotError );
                  sprtf(perr);
               }

               Add2DLList(perr);
            }
         }
// ******************************************************************
      } // IN or NOT IN A LABEL

      // IS END OF LINE?
      if(( c < ' ' ) && ( c != '\t' )) {
         // end of line
         if( c == '\n' ) {
            pln3[dwc] = 0;
//            sprtf("%s"MEOR, pln3);
            linecnt++;
            OUTLINE(0);
            dwc = 0;
            Remove_Flag( Wait_Flags ); // = jf_WaitDn jf_WaitEnd ); // remove WAIT flag
            gLnsDat++; // data lines within a LABEL
            Remove_Flag( jf_HadLabel );
            Remove_Flag( jf_ToEOL );
            // =======================================
            linelen = 0; // restart buffer line length
            // =======================================
         }

         // break at a certain LINE
         if( linecnt == 7113 ) {
            static BOOL _s_bdndbg = FALSE;
            if( !_s_bdndbg ) {

               _s_bdndbg = TRUE;
               sprtf("Reached line %d!"MEOR, linecnt );
            }
         }
      } else {
         // add character
         if( dwc < DMAXLINE ) {
            pln3[dwc++] = (TCHAR)c;
         }
         // ===========================
         linelen++; // bump line LENGTH
         // ===========================

         if( !In_Label && !Wait_End && // jf_WaitEnd
              DoJobFixW && //  ( Pgm_Flag & Do_JobsW )   // -jwnn
            ( g_dwWrap != 0 ) &&
            ( dwc > g_dwWrap ) ) { 

            if( !Got_Flag(jf_WaitDn) ) {
               dwn = (DWORD) M_REMAINS;
               if( Len2End( (PTSTR)lpb, dwn ) < 11 )
                  Set_Flag(jf_WaitEnd);
               Set_Flag(jf_WaitDn);
            }
            // time to SPLIT line to output
            if( !Wait_End && !ISALPHANUM(c) ) {
               pln3[dwc] = 0;
               gAdded += 2;
               OUTLINE(1);
               dwc = 0;
               gLnsDat++; // data lines within a LABEL
               Remove_Flag( jf_HadLabel );
            }
         }
      }

      d = c;
   }
      if( In_Label ) {
            pln2[dwn] = 0;
            Add2XML(pln2, ILNCNT);
            sprtf("Line %d: Errant XML label!"MEOR,
                        ILNCNT );
            Remove_Flag( (jf_InLabel | jf_EndLab1 | jf_EndLab2) );
            dwn = 0;

      }

   if(VERB)
      sprtf( "Done input file. Closing MAP ..."MEOR );

   DeleteFileMap( pmf );

   gTotDisc += gDiscard; // accumulate total dicards
   gTotAdd  += gAdded;  // and additions

   gdwTotWrite += gdwWritten; // accumulate TOTAL written out
   gliTotWrite.QuadPart += gdwWritten;

   gdwLine2 = linecnt;  // number of lines INPUT
   gdwLines = gLnsOut;  // lines of OUTPUT

   CloseDiscard();

   if( VERB1 )
   {
      LPTSTR   lpo;
      if( VFH(gOut_Hand) )
         lpo = &gszOFName[0];
      else
      {
         if( gbRedON )
            lpo = "Redirected StdOut";
         else
            lpo = "Standard Out";
      }

#ifdef USE_64_BITS
      sprtf( "Done file %s of %s bytes, lines %d."MEOR,
         GetShortName(lpf,40),
          Get_I64_Stg(ulib.QuadPart),
         linecnt );
#else // !#ifdef USE_64_BITS
      sprtf( "Done file %s of %s bytes, lines %d."MEOR,
         GetShortName(lpf,40),
          Get_I64_Stg(ulib),
         linecnt );
#endif // #ifdef USE_64_BITS y/n
      sprtf( "Added %d, discarded %d, converted %d bytes. Lines now %d"MEOR,
         gAdded, gDiscard, gConverted,
         gdwLines );
      sprtf( "Written %d bytes to %s."MEOR,
         gdwWritten,
         lpo );   // OF_Name, "Standard Out" or redirected

      if(VERB4) {
         // show what was collected
         OutXMLLabs();
      }
   }


   KillLList(ph); //    PLE   ph = &g_sXMLTags;
   KillLList(phl); //    PLE   ph = &g_sLines;


	return TRUE;
}

// eof - FixJobs.c
