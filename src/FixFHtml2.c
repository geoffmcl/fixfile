

// FixFHtml2.c
#include	"FixF32.h"
extern BOOL g_bDoWord; // = FALSE;

// flag bits
#define  fb_AllSp    0x00000001
#define  fb_CrLf     0x00000002
#define  fb_GotFrom  0x00000004
#define  fb_HadFrom  0x00000008
#define  db_HadHdr   0x00000010

typedef struct tagHTMLN {
   LIST_ENTRY  list;
   DWORD       flag;
   TCHAR       line[1];
}HTMLN, * PHTMLN;

LIST_ENTRY    g_HtmlList = { &g_HtmlList, &g_HtmlList };

// header items, that end with ": ?"
PTSTR g_pHdrBgns[] = {
    "Received" ,
    "From"     ,
    "MIME-Version" ,
    "Content-Type" ,
    "Content-Transfer-Encoding" ,
    "Message-ID" ,
    "Date"       ,
    "To"         ,
    "X-Mailer"   ,
    "Subject"    ,
    "Sender" ,
    "Errors-To" ,
    "X-BeenThere" ,
    "X-Mailman-Version" ,
    "Precedence" ,
    "Reply-To" ,
    "List-Help" ,
    "List-Post" ,
    "List-Subscribe" ,
    "List-Id" ,
    "List-Unsubscribe" ,
    "List-Archive"
};

typedef VOID (*CHKBUF) (PTSTR,PDWORD);

typedef struct tagKCHK {
   PTSTR phtml;
   DWORD flag;
   CHKBUF pserv;
}KCHK, * PKCHK;

PTSTR pboddel[] = {
    "lang=EN-GB" ,
    "link=blue" ,
    "vlink=purple" ,
    "style='tab-interval:36.0pt'" ,
    0 
};

PTSTR ppdel[] = {
    "class=MsoNormal" ,
    "class=MsoPlainText" ,
    "style='margin-left:36.0pt'" ,
    0 
};

PTSTR ptddel[] = {
    "style='padding:.75pt .75pt .75pt .75pt'" ,
    0 
};

VOID Do_Del( PTSTR pb, PTSTR pdel, PDWORD pdw )
{
   DWORD dwoff = InStr(pb,pdel);
   if(dwoff) {
      PTSTR pl = Left(pb, (dwoff-1));
      PTSTR pr = Right(pb, (strlen(pb) - strlen(pdel) - (dwoff - 1)));
      dwoff = strlen(pl);
      if(*pr) {
         if((dwoff == 0) ||
            (dwoff && (pl[dwoff-1] <= ' ')) ) {
            if(*pr <= ' ')
               pr++;
         }
      }

      strcpy(pb,pl);
      strcat(pb,pr);
      dwoff = strlen(pb);
      while(dwoff--)
      {
         if(pb[dwoff] > ' ')
            break;
         pb[dwoff] = 0;
      }

      dwoff = strlen(pb);
      *pdw = dwoff;

   }
}

VOID  clrbod( PTSTR pb, PDWORD pdw )
{
   PTSTR p;
   DWORD i;
   for(i = 0; ; i++) {
      p = pboddel[i];
      if(p)
         Do_Del( pb, p, pdw );
      else
         break;
   }
}

VOID  clrp( PTSTR pb, PDWORD pdw )
{
   PTSTR p;
   DWORD i;
   for(i = 0; ; i++) {
      p = ppdel[i];
      if(p)
         Do_Del( pb, p, pdw );
      else
         break;
   }
}

VOID  clrtd( PTSTR pb, PDWORD pdw )
{
   PTSTR p;
   DWORD i;
   for(i = 0; ; i++) {
      p = ptddel[i];
      if(p)
         Do_Del( pb, p, pdw );
      else
         break;
   }
}

BOOL g_bNoLinks = TRUE;
VOID clra( PTSTR pb, PDWORD pdw )
{
   if(g_bNoLinks) {
      Do_Del( pb, pb, pdw );
   }
}

enum InTag {
   in_HTML,
   in_HEAD,
   in_SCRIPT,
   in_BODY,
   in_TITLE
};

KCHK g_pKeep[] = {
   { "HTML", 1, 0 },
   { "HEAD", 0, 0 },
   { "SCRIPT", 0, 0 },
   { "BODY", 0, &clrbod  },
   { "TITLE", 0, 0 },
   { "META", 0, 0 },
   { "H?", 4, 0 },
   { "P", 0, &clrp },
   { "A", 0, &clra },   // check A 'links' keep
   { "B", 0, 0 },
   { "IMG", 0, 0 },
   { "CENTER", 0, 0 },
   { "TABLE", 0, 0 },
   { "TR", 0, 0 },
   { "TD", 0, &clrtd },
   { 0 , 0, 0 }
};

DWORD dwKeepOff = 0;
PKCHK pKeep = 0;
BOOL bNegTag;
#define InScript  ( dwKeepOff == (in_SCRIPT + 1) )

BOOL InKeep( PTSTR ptag )
{
   PKCHK pk = &g_pKeep[0];
   PTSTR p = pk->phtml;
   DWORD dwi = 0;
   dwKeepOff = 0;
   pKeep = 0;
   bNegTag = FALSE;
   if(*ptag == '/') {
      ptag++;
      bNegTag = TRUE;
   }
   while(p) {
      dwi++;
      if( strcmpi(p,ptag) == 0 ) {
         dwKeepOff = dwi;
         pKeep = pk;
         return TRUE;
      }
      pk++;
      p = pk->phtml;
   }
   return FALSE;
}

BOOL  is_all_spacey(PTSTR ps)
{
   INT   c;
   while((c = *ps++) != 0)
   {
      if(c > ' ')
         return FALSE;
   }
   return TRUE;
}

#define  ES(a)    while( *a && (*a <= ' ') )a++
#define  ET(a)    while( *a && (*a > ' ' ) )a++

// check line is "From a@b.c date/time
BOOL  check_4_fromem(PTSTR ps)
{
   INT   c = *ps;
   DWORD pos = InStr(ps,"From ");
   INT   hadat = 0;
   PTSTR pbgn;

   if(pos != 1)
      return FALSE;
   c = 5;
   while(c--)
      ps++;

   pbgn = ps;  // start of e-mail
   c = *ps++;
   if(c < ' ')
      return FALSE;
   while((c = *ps++) != 0)
   {
      if(c == '@')
         hadat = TRUE;
      else if( c <= ' ' )
         break;
   }

   if(!hadat)
      return FALSE;

   ES(ps);
   ET(ps);
   ES(ps);
   ET(ps);
   ES(ps);

   if(!ISNUM(*ps))
      return FALSE;


   return TRUE;
}

#define  Traverse_Cont(pListHead,pListNext)\
   for( pListNext = pListNext->Flink; pListNext != pListHead; pListNext = pListNext->Flink )

VOID Out_HTML_Lines(PLE ph)
{
   PLE   pn;
   PTSTR ps;
   DWORD dwl;
   BOOL  hadout = FALSE;
   BOOL  out1 = FALSE;
   BOOL  issp, isem;
   PHTMLN   phl;

   Traverse_List(ph,pn)
   {
      phl = (PHTMLN)pn;
      //ps = (PTSTR)((PLE)pn + 1);
      ps = &phl->line[0];
      issp = is_all_spacey(ps);
      isem = check_4_fromem(ps); // this "From a@b.c date time BEGINS an e-mail
      dwl = strlen(ps);
      if(!hadout) {
         if( !issp ) {
            if(!out1) {
               // check line is "From a@b.c date/time
               //isem = check_4_fromem(ps);
               out1 = TRUE;
            }
            hadout = TRUE;
         }
      }

      if(hadout) {
         // 
         outit(ps, dwl);
         if(isem) {
//            PLE   pn2 = pn;
//            Traverse_Cont(ph,pn2)
//            {




//            }
         }
      }
   }
}

// BUT if we are in a SCRIPT, this could be if(a<b) expression
DWORD IsKeep( PTSTR lpchar, PDWORD pdwi, DWORD dws,
              PTSTR lpout, PDWORD pdwo, PINT pd )
{
   DWORD bRet = 0;
   LPTSTR   lpo2  = &gszOutBuf2[0];
   DWORD dwi = *pdwi;
   PBYTE lpb = (PBYTE)lpchar;
   DWORD dwo = 0;
   BOOL hadsp = FALSE;
   DWORD c = 0;
   DWORD lconv, ldel, ladd;

   lconv = ldel = ladd = 0;
   c = lpb[dwi];
   if( !bNegTag && InScript ) {
      DWORD dwout = *pdwo; // get OUTPUT count
      lpout[dwout++] = (TCHAR)c;
      // dwi++; the continue does this increment
      // *pdwi = dwi;   // update input counter
      *pdwo = dwout; // update output counter
      *pd = (INT)c;
      return 2;
   }

   dwi++; // to char FOLLOWING the '<' OPEN TAG
   for( ; dwi < dws; dwi++ ) {
      c = lpb[dwi];
      if(( c == '>' ) ||
         ( c <= ' ' ) ) {
         if(!hadsp) {
            hadsp = TRUE;
            lpo2[dwo] = 0;
            if( InKeep(lpo2) ) {
               bRet = 1;
            }
         }
      }

      if( c == '>' )
         break;

      // only ONE space added
      if( c <= ' ' ) {
         if(dwo) {
            if( lpo2[dwo-1] > ' ' ) {
               if( c != ' ' ) {
                  lconv++; //gConverted++;
                  c = ' ';
               }
            } else {
               ldel++; //gDiscard++;
               continue;
            }
         }
      }

      // build tag, and anything following
      lpo2[dwo++] = (TCHAR)c;
   }

   if(( bRet && dwo ) &&
      ( c == '>'    ) &&
      ( pKeep       ) ) {
      // *******************************
      // ok, keep = output this HTML tag
      DWORD dwout = *pdwo; // get OUTPUT count
      DWORD dw    = dwo;
      CHKBUF ps   = pKeep->pserv;

      lpo2[dwo] = 0;
      if(ps)
         (*ps)(lpo2,&dw);
      if(dw) {
         // ***********************
         // output the maybe ADJUSTED tag
         lpout[dwout++] = '<';
         while(dw--) {
            lpout[dwout++] = *lpo2;
            lpo2++;
            if( pKeep->flag == 1 ) {
               if(*lpo2==' ')
                  break;
            }
         }
         lpout[dwout++] = (TCHAR)c;
         // dwi++; the continue does this increment
         *pdwi = dwi;
         *pdwo = dwout;
         *pd = (INT)c;
         ldel += (dwo - dw);
         gDiscard += ldel;
         gAdded   += ladd;
         gConverted += lconv;
         // ***********************
      } else {
         // ok, ALL are to be DROPPED
         *pdwi = dwi;
         //*pdwo = dwout; // unchanged
         *pd = (INT)c;
         ldel += (dwo - dw);
         gDiscard += ldel;
         gAdded   += ladd;
         gConverted += lconv;

      }
   }

   return bRet;
}

void chkit( void )
{
   int i;
   i =0;
}

BOOL	Do_Del_HTML( LPTSTR lpb, DWORD dws, LPTSTR lpf )
{
static LPTSTR _s_pComBgn, _s_pComEnd;
   BOOL     bRet = TRUE;
   DWORD    dwi, dwo, dwBal;
   INT      c, d;
   LPTSTR   lpout = &gszOutBuf[0];
   DWORD    dwa, dwd, dwl, dwl2;
   BOOL     bDnCrLf = TRUE;
   BOOL     bInComm = FALSE;  // flag in a html\htm 'comment' <! to -->
   LPTSTR   lpbgn;   // beginning of line
   BOOL     bDidMEOL;   // only afer a Cr/Lf pair has been added to OUTPUT
   DWORD    dwDelSp;
#ifndef  NDEBUG
   LPTSTR   ptmp;
#endif
   PLE   ph = &g_HtmlList;
   PLE   pn;   // = (PLE)MALLOC( (sizeof(LIST_ENTRY) + dwo + 1) );
   PTSTR ps;   // = (PTSTR)((PLE)pn + 1);
   PHTMLN   phl;
   DWORD    dwflag = 0;
   DWORD    pos;
   if( !lpb || !dws )
      return FALSE;

   gDiscard = 0;
   gAdded   = 0;
   gConverted = 0;
   gdwWritten = 0;
   KillLList(ph); //    PLE   ph = &g_HtmlList;

   _s_pComBgn = lpb;
   _s_pComEnd = &lpb[dws-1];
   dwBal      = dws;
   c = d = 0;
   dwo = dwa = dwd = dwl = dwl2 = 0;
   lpbgn = lpout;    // begin is beginning of output
   bDidMEOL = TRUE;
   dwDelSp  = 0;
   for( dwi = 0; dwi < dws; dwi++ )
   {
#ifdef  NDEBUG
      c = lpb[dwi];
#else // !NDEBUG = DEBUG
      ptmp = &lpb[dwi]; // set the current pointer
      c = *ptmp;        // load the next
#endif   // NDEBUG y/n

      if( c == '<' ) // start of HTML / HTM == openning brace
      {
         LPTSTR   lptmp;
         if( g_bDoWord && IsKeep( lpb, &dwi, dws, lpout, &dwo, &d ) )
            continue;

         dwi++;
         dwd++;   // discarded item
         lptmp = &lpb[dwi];   // get the pointer
         if( dwi < dws )
         {
            if( lpb[dwi] == '!' )   // check for "<!" comment start
            {
               DWORD dwIfLen = 0; // = BufHasTailChar( &lpb[dwi+1], (dws - (dwi + 1)), ']' ) ) > 0 ) &&
               BOOL  bStComm = bExclMLComm;   // get the SWITCH
               if(( ( dwi + 3 ) < dws   ) &&
                  ( lpb[dwi+1] == '[' ) )
               {
                  dwIfLen = BufHasTailChar( &lpb[dwi+1], (dws - (dwi + 1)), ']' );
               }

               if(( dwIfLen                       ) &&
                  ( dwIfLen < 256                 ) &&
                  ( lpb[dwi + dwIfLen + 1] == '>' ) )
               {
                  // comment ended after xml code
                  // stay on simple exclude after "<" until ">",
                  // and that includes, like "<![if !mno]>"
                  bStComm = FALSE;
               }
               // HIT A ROAD BLOCK *********** no display of this data
//               if( bExclMLComm )
               // BUT only if it is a SIMPLE "<!" to "--[something]>"
               if( bStComm )
               {
                 _s_pComBgn = lptmp - 1;  // backup to the BEGIN "<!" command
                 bInComm = TRUE;      // set in a comment
                 dwBal   = dws - (dwi - 1);  // keep possible MAXIMUM of comment
                 // ************************************************************
                  if(( bCheckIf          ) &&
                     ( (dwi + 4) < dws   ) &&
                     ( gdwIfCnt          ) &&
                     ( lpb[dwi+1] == '[' ) &&
                     ( ( dwIfLen = BufHasTailChar( &lpb[dwi+1], (dws - (dwi + 1)), ']' ) ) > 1 ) &&
                     ( dwIfLen < 256     ) )
                  {
                     LPTSTR lpb2 = GetExBraceEx( &lpb[dwi+1], (dws - (dwi + 1)) );
                     //DWORD dwlen = strlen(lpb2);
                     if(( lpb2 ) &&
                        ( strcmpi(lpb2,"endif") == 0 ) )
                     {
                        gdwIfCnt--;
                     }
                     if(( (dwi + dwIfLen + 1) < dws ) &&
                        ( lpb[dwi + dwIfLen] == '>' ) )
                     {
                       _s_pComEnd = &lpb[dwi + dwIfLen + 1];  // backup to the BEGIN "<!" command
                     }
                  }
               }
            } else {
               // not a HTML comment begin

            }
         }

         // inner, faster discard loop
         // **************************
         for( ; dwi < dws; dwi++ )
         {
            c = lpb[dwi];
            if( bInComm )
            {
               // if in a COMMENT (HTML), then seek its END
               // is there an ESCAPE for this???
               if( c == '-' )
               {
                  // got the FIRST of "-->" close message
                  if( (dwi + 2) < dws )   // must always check the lookforward size
                  {
                     //DWORD dwii = dwi;    // keep BEGIN of commend END
                     // 20040510 Questioning this END OF COMMENT
                     // check for ( lpb[dwi+2] == '>' ) was commented out
                     // and bInComm = FALSE;      // set in a comment
                     // if had, like "<!--[if !mso]>", BUT THIS IS NOT END
                     // that is
                     if(( lpb[dwi+1] == '-' ) &&
                        ( lpb[dwi+2] == '>' ) ) {
                        // eat the first
                        dwi++;
                        dwd++;   // discarded item
                        // and second char
                        dwi++;
                        dwd++;   // discarded item
                        bInComm = FALSE;      // un-set in a comment
                        c = '>'; // reached the end of the HTML comment code
                        break; // all done in discard loop

                     }
#if 0 // delete code
                     else if(( lpb[dwi+1] == '-' ) &&  // get NEXT character
                        ( BufGotEnd( lpb, &dwi, dws, '>' ) ) ) // == '>' ) )
                     {

//                        ( BufGotEnd( lpb, (dwi+2), dws ) )  // == '>' ) )
                        // its a COMMENT exit
                        _s_pComEnd = &lpb[dwii];   // reached "--[if anything]>" sequence
                        // eat the first
                        dwi++;
                        dwd++;   // discarded item
                        // and second char
                        dwi++;
                        dwd++;   // discarded item

                        lptmp = &lpb[dwi+1]; // get NEXT character, just for DEBUG
                        // view of where the loop, after an inc, will start ... sanity

                        c = '>'; // we have reached the end of the HTML comment code
                        // 20040510 Questioning this END OF COMMENT
                        // bInComm = FALSE;      // set in a comment
                        //break;   // out of the discard loop
                     }
#endif // #if 0 // delete code
                  }
               }
            }
            else if( c == '>' )
            {
               dwd++;
               break;
            }
            dwd++;   // count a discard
         }
         // **************************
         // inner, faster discard loop ended

         d = c;      // establish previous of HTML inner dicard loop

         continue;   // and return the the OUTPUT loop, with a dwi increment
         // ================================================================
      }

      // ok, was NOT a "<" character, so OUTPUT it 
      // +++++++++++++++++++++++++++++++++++++++++

      if( c == 0x0a )   // if a LF
      {
         if( d != 0x0d )   // NOT preceeded by a CR
         {
            // enforce program rule
            lpout[dwo++] = 0x0d;
            dwa++;
#ifndef  NDEBUG
            if( VERB5 ) {
               _s_AddCRMsg++;
               if( _s_AddCRMsg < MXCRMSGS )  // say 10
               {
                  AddReport( "Added CR (0x0d) to line %d (%d)."MEOR, dwl, dwl2 );
               }
            }
#endif   // !NDEBUG
         }
      }
      else if( c == 0x0d ) // if a CR, then may be just first of a pair
      {
         if( dwo && ( d == '=' ) )
         {
            dwo--;
            dwd++;
         }
         if( (dwi + 1) < dws )
         {
            // if there is more in the buffer,
            // AND it is NOT a LF
            if( lpb[ dwi + 1 ] != 0x0a )
            {
               // then enforce program rule for EOL
               lpout[dwo++] = (TCHAR)c;   // add the CR
               c = 0x0a;
               dwa++;
#ifndef  NDEBUG
               if( VERB8 )
               {
                  //_s_AddLFMsg++;
                  //if( _s_AddLFMsg < MXCRMSGS )  // say 10
                  {
                     AddReport( "Added LF (0x0a) to line %d (%d)."MEOR, dwl, dwl2 );
                  }
               }
#endif   // !NDEBUG
            }
         }
         else
         {
            lpout[dwo++] = (TCHAR)c;
            c = 0x0a;
            dwa++;
#ifndef  NDEBUG
            if( VERB8 )
            {
               AddReport( "Added LF (0x0a) to line %d (%d)."MEOR, dwl, dwl2 );
            }
#endif   // !NDEBUG
         }
      }
//#define  ISLOWER(a)     (( a >= 'a' ) && ( a <= 'z' ))
//         ( ISLOWER(c) ) )

      if(( d == '&' ) &&
         ( (dwi + 4) < dws ) )
      {
         switch(c)
         {
         case 'n':   // ( c == 'n' ) ) // begin of npsp
            if(( bAddnpsp ) &&
               ( dwo      ) ) // we have OUPUT the '&' so this is an OVERWRITE
            {
               // could be a &nbsp arriving
                  if(( Plus( 1, 'b' ) ) &&
                     ( Plus( 2, 's' ) ) &&
                     ( Plus( 3, 'p' ) ) &&
                     ( Plus( 4, ';' ) ) )
                  {
                     dwi += 4;
                     dwd += 4;
                     c = ' '; // change LAST back to a SPACE
                     dwo--;   // backup the OUTPUT
                  }
            }
            break;

         case 'q':
            if(( bAddnpsp ) &&
               ( dwo      ) ) // we have OUPUT the '&' so this is an OVERWRITE
            {
               // could be a &nbsp arriving
                  if(( Plus( 1, 'u' ) ) &&
                     ( Plus( 2, 'o' ) ) &&
                     ( Plus( 3, 't' ) ) &&
                     ( Plus( 4, ';' ) ) )
                  {
                     dwi += 4;
                     dwd += 4;
                     c = '"'; // change LAST back to a quote for output
                     dwo--;   // backup the OUTPUT
                  }
            }
            break;

         case 'l':
            if(( bAddnpsp ) &&
               ( dwo      ) ) // we have OUPUT the '&' so this is an OVERWRITE
            {
               // could be a &nbsp arriving
                  if(( Plus( 1, 't' ) ) &&
                     ( Plus( 2, ';' ) ) )
                  {
                     dwi += 2;
                     dwd += 2;
                     c = '<'; // change LAST back to a quote for output
                     dwo--;   // backup the OUTPUT
                  }
            }
            break;

         case 'g':
            if(( bAddnpsp ) &&
               ( dwo      ) ) // we have OUPUT the '&' so this is an OVERWRITE
            {
               // could be a &nbsp arriving
                  if(( Plus( 1, 't' ) ) &&
                     ( Plus( 2, ';' ) ) )
                  {
                     dwi += 2;
                     dwd += 2;
                     c = '<'; // change LAST back to a quote for output
                     dwo--;   // backup the OUTPUT
                  }
            }
            break;

         }

      }  // if preceeding was an '&', which is the control from some alias transitions

      // *********************
      if(( bDidMEOL          ) &&
         ( Pgm_Flag & Rem_DS ) &&  // Remove initial SPACES
         ( c == ' '          ) )
      {
         BOOL  bDelSp = TRUE;
         if(( g_dwDel_CC       ) &&
            ( g_dwDel_CC != (DWORD)-1 ) )  // Set to MAX
         {
            if( dwDelSp >= g_dwDel_CC )
            {
               bDelSp = FALSE;
            }
         }

         if( bDelSp )
         {
            dwd++;   // count a DELETE character
            d = c;
            dwDelSp++;
            continue;
         }
      }
      else
      {
         bDidMEOL = FALSE;
         dwDelSp  = 0;
      }

      lpout[dwo++] = (TCHAR)c;   // add this to display
      // *********************
      lpout[dwo] = 0;
      if( strcmp(lpout, "ClockHeight") == 0 )
         chkit();

      if( c == 0x0a )
      {
         lpout[dwo] = 0;
         dwl2++;
         if( Pgm_Flag & Rem_CRLF )
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
               if( VERB8 )
               {
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

         if( ( dwo ) &&    // if we HAVE some output AND
            ( Pgm_Flag & Gt_Rem ) )  // flag of SOME removals
         {
#ifndef  NDEBUG
            DWORD    dwpd2 = gDiscard;
#endif   // !NDEBUG
            if( Chk_Remove( lpout, lpbgn, &dwo, dwl ) )
            {
               if(dwl)
                  dwl--;   // just lost a line
#ifndef  NDEBUG
               if( VERB8 )
               {
                  AddReport( "Discarded line %d (%d) of %d chars."MEOR,
                     dwl, dwl2,
                     (gDiscard - dwpd2) );
               }
#endif   // !NDEBUG
            }
         }

         lpout[dwo] = 0;
         // need to store lines, and be prepared for a BIG DISCARD of the
         // 'header', if an e-mail type thingy, within HTML
         if(dwo) {
//            PLE   pn;   // = (PLE)MALLOC( (sizeof(LIST_ENTRY) + dwo + 1) );
//            PTSTR ps;   // = (PTSTR)((PLE)pn + 1);
//            pn = (PLE)MALLOC( (sizeof(LIST_ENTRY) + dwo + 1) );
            pn = (PLE)MALLOC( (sizeof(HTMLN) + dwo) );
            if(pn) {
               phl = (PHTMLN)pn;
               phl->flag = 0;
               bRet = TRUE;
//               ps = (PTSTR)((PLE)pn + 1);
               ps = &phl->line[0];
               strcpy(ps,lpout);
               if( is_all_spacey(ps) ) {
                  phl->flag = fb_AllSp;
                  if((dwo == 2) && (ps[0] == '\r') && (ps[1] == '\n')) {
                     phl->flag |= fb_CrLf;
                  }
                  dwflag |= phl->flag;
               } else if( check_4_fromem(ps) ) {
                  phl->flag = fb_GotFrom;
                  dwflag |= fb_GotFrom;
               } else {
                  pos = InStr(ps, ": ");


               }

               InsertTailList(ph,pn);
//            bRet = outit(lpout,dwo);
            } else {
               bRet = FALSE;
            }
            dwo = 0;
            bDnCrLf = TRUE;
            if( !bRet )
               break;
         }

         lpbgn = &lpout[dwo];    // reset start of line

      }  // end if a LF - and special for HTML
      else if(( dwo >= 2 ) && ( d == '=' ) && ISHEX(c) )
      {
         // if the PREVIOUS was an '=' sign, and now we have HEXIFIED data
         if( ( Pgm_Flag & Show_Hex ) && // -h2a[nn] - show HEX to ASCII conversion
             ( (dwi + 1) < dws ) &&
             ISHEX( lpb[dwi+1] ) )
         {
            // we have something like =20 for space =E9 for accented char
            c = GetHEX2( &lpb[dwi] );
            dwo   -= 2; // BACK UP TO THE EQUAL SIGN
            lpout[dwo++] = (TCHAR)c;   // put in the HEX
            dwd += 3;   // discarded 3, and
            dwa++;   // added ONE
            dwi++;   // and move on in buffer
         }
      }

      if(( c == '\n' ) &&  // current is LF, and
         ( d == '\r' ) )   // and previous was CR
      {
         bDidMEOL = TRUE;
      }

      d = c;   // keep previous

      if(dwo >= MXOUTB)
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

   } // end -    for( dwi = 0; dwi < dws; dwi++ )

   Out_HTML_Lines(ph);

   // if add eof message
   if( VERB1 )
   {
      if( c != 0x0a )
      {
         lpout[dwo++] = 0x0d;
         lpout[dwo++] = 0x0a;
         dwa += 2;
#ifndef  NDEBUG
         if( VERB8 )
         {
            AddReport( "Added Cr/Lf to end line %d (%d) = 2 chars."MEOR,
                     dwl, dwl2 );
         }
#endif   // !NDEBUG
      }
   
      for( dwi = 0; ; dwi++ )
      {
         c = End_Msg[dwi]; // "eof" plus CR,LF
         if( c == 0 )
            break;
         lpout[dwo++] = (TCHAR)c;
         dwa++;
      }
#ifndef  NDEBUG
      if( VERB8 )
      {
         AddReport( "Added eof message to end %d (%d) = %d chars."MEOR,
                  dwl, dwl2,
                  strlen( End_Msg ) );
      }
#endif   // !NDEBUG
   }

   // if some left in output buffer
   if(dwo)
   {
      lpout[dwo] = 0;
      bRet = outit(lpout,dwo);
      dwo = 0;
   }

   gDiscard += dwd;
   gAdded   += dwa;

   gTotDisc += dwd;
   gTotAdd  += dwa;

   gdwTotWrite += gdwWritten;
   gliTotWrite.QuadPart += gdwWritten;

   gdwLines = dwl;
   gdwLine2 = dwl2;

   if( bInComm )
   {
      //sprintf(lpout,"NOTE: Exit in HTML comment \"<!\"!"MEOR );
      strcpy(lpout,"NOTE: Exit in HTML comment \"<!\"!"MEOR );
      dwo = strlen(lpout);
      bRet = outit(lpout,dwo);
      dwo = 0;
      if( dwBal < (1024-16) )
      {
         sprtf( "Com?[%s]"MEOR, _s_pComBgn );
      }
   }

   KillLList(ph); //    PLE   ph = &g_HtmlList;

#ifdef _WIN32
   UNREFERENCED_PARAMETER(lpf);
#endif

   return bRet;
}


// eof - FixFHtml2.c
