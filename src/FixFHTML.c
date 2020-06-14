

// FixFHTML.c
#include	"FixF32.h"
// #include	"FixFHTML.h"
//  VersNum3    8  // -h = remove html fixes
//  VerDate	"1 April, 2002"
// 20020401 - Fixed -ds = remove leading space, added "<!" to -->" comments,
// decode of &quot, etc ...

extern   LPTSTR   GetExBrace( LPTSTR lps );  // unpack a string (ms?) if
extern   LPTSTR   GetExBraceEx( LPTSTR lps, DWORD dws );   // see if we ave ENDIF!!!
extern   DWORD    BufHasTailChar( LPTSTR lpb, DWORD dwMax, INT chr );
extern   VOID  Do_Happy_Msg(VOID);
extern BOOL Do_Del_HTML( LPTSTR lpb, DWORD dws, LPTSTR lpf );

#define  dmsg  sprtf

BOOL  g_bDoWord = FALSE;
BOOL  g_bTags2NL = FALSE;

VOID Give_H_Help(VOID)
{
	dmsg(	" -h[2a|?]   Del/Add HTML. (htm=Add, h2a=HEX-to-Asc, or ?=more switches)"MEOR );
}

VOID  Show_H_Help( LPTSTR lpc )
{

//	dmsg(	" -h[2a|?]   Delete HTML. (h2a=Show HEX as ASCII, or ?=more switches)"MEOR );
   sprtf( "Due to command [%s], showing ?. help?"MEOR, lpc );

	dmsg(	" -h plus - The delete HTML HELP."MEOR );
   dmsg( "    <nul> = Delete HTML from a file."MEOR );
   dmsg( "    tm    = Add HTML to a file."MEOR );
   dmsg( "    2a    = Show HEX as ASCII"MEOR );
   dmsg( "    F|R   = Find or Replace"MEOR );
   dmsg( "    +     = Add to F/R list."MEOR );
   dmsg( "    c[[[+|-]|[0|1]] is the Comment Switch (def=ON)."MEOR );
   // 20040510 - add -hw to delete WORD html 'extensions'
   dmsg( "    w     = Delete WORD html extensions."MEOR );
   // FIX20070626 - add -hn to put tags on a new line
   dmsg( "    n     = HTML tags to a NEW line."MEOR );

   Do_Happy_Msg();

	Pgm_Exit( 0 );
}

//VOID  Show_H_Help( LPTSTR lpc )

BOOL  Get_H_Cmd( LPTSTR lpc )
{
   BOOL  bRet = FALSE;  // no error yet
   INT      bgn, k, c;
   LPTSTR   lpb = &gszTmpBuf[0];
   INT   t = toupper(*lpc);
   //if( *lpc )
   //if(t)
   switch(t)
   {
   case 0:  // was else
      {
         // just -H, then simply
         Pgm_Flag |= Rem_HTML;   // Add working with HTML
//   	   Pgm_Flag |= Rem_CRLF;   // Add removal of dup Cr/Lf - should be 'selective'
      }
      break;

      // -hf:break:hr:<font color="#0000FF">break</font>
      // t = toupper(*lpc);
   case '?':   //      if( t == '?' )
      {
         Show_H_Help( lpc );
      }
      break;
   case '2':   // else if( t == '2' )
      {
         lpc++;
         if( toupper(*lpc) == 'A' )
         {
            lpc++;
            if( *lpc )
            {
               if( ISNUM(*lpc) )
               {
                  k = (*lpc - '0'); // get FIRST number
                  lpc++;
                  while( ISNUM(*lpc) )
                  {
                     k = ( k * 10 ) + ( *lpc - '0' );
                     lpc++;
                  }
                  if(k)
                     g_dwHexLen = (DWORD)k;   // set the minimum length
               }
               else
                  goto Bad_H;
            }

            Pgm_Flag |= Show_Hex; // -h2a[nn] - show HEX to ASCII conversion
         }
         else
            goto Bad_H;
      }
      break;
//      else if( ( t == 'F' ) ||
//          ( t == 'R' ) )
   case 'F':
   case 'R':
      {
         lpc++;
         bgn = *lpc;
         lpc++;
         c = *lpc;
         if( c == 0 )
            goto Bad_H;
         if( c == bgn )
            goto Bad_H;
         k = 0;
         while(c)
         {
            lpb[k++] = (TCHAR)c;
            lpc++;
            c = *lpc;
            if( c == bgn )
            {
               lpc++;
               break;
            }
         }

         if( c != bgn )
            goto Bad_H;

         lpb[k] = 0;    // this is the FIND item

         // gsFndRpl - find/replace list
         if( !Add2FRList( lpb, ((t=='F') ? iCreate : iAddRpl ) ) )
            goto Bad_H;

         c = toupper(*lpc);
         if(c)
         {
            if( c != 'H' )
               goto Bad_H;
            lpc++;
            bRet = Get_H_Cmd(lpc);
         }

         Pgm_Flag |= Rem_HTML;   // Add working with HTML

      }
      break;

      // FIX20041107
   case 'T':
      {
         lpc++;
         t = toupper(*lpc);
         if(t == 'M') {
            if( Pgm_Flag & Rem_HTML ) {  // removing is contrary
               goto Bad_H;
            }

            Pgm_Flag |= Add_HTML;   // add HTML to output

            break;
         }
         goto Bad_H;
      }
      break;
   case '+':   //  else if( t == '+' )
      {
         *lpb = '+';
         lpb[1] = 0;
         if( !Add2FRList( lpb, iSetRE ) )
            goto Bad_H;
         Pgm_Flag |= Rem_HTML;   // Add working with HTML
      }
      break;


   case 'C':
      {  // toggle comment processing (in delete)
         lpc++;
         c = *lpc;
         switch(c)
         {
         case 0:
         case '+':
         case '1':
            bExclMLComm = TRUE;
            break;

         case '-':
         case '0':
            bExclMLComm = FALSE;
            break;
         }
      }
      break;

   case 'W':
      g_bDoWord = TRUE;
      Pgm_Flag |= Rem_HTML;   // Add working with HTML
      lpc++;
      if(*lpc)
         return(Get_H_Cmd(lpc));
      break;
   case 'N':
      g_bTags2NL = TRUE;
      lpc++;
      if(*lpc)
         return(Get_H_Cmd(lpc));
      break;

   default: // else
      {
Bad_H:
         bRet = TRUE;
      }
      break;
   }

   return bRet;
}



//#define     frf_Repl    0x00000001
// gsFndRpl
typedef  struct tagFR {
   LIST_ENTRY  fr_Entry;
   DWORD       fr_dwFlag;
   INT         fr_iFind, fr_iFirst;
   TCHAR       fr_cFind, fr_cLast, fr_cFirst, fr_cNext;
   INT         fr_iCntMin, fr_iCntMax;
   INT         fr_iRepl;
   TCHAR       fr_szFind[264];
   TCHAR       fr_szRepl[264];
   TCHAR       fr_iTags;
   TCHAR       fr_szTags[264];
}FR, * PFR;

// gsFndRpl - find/replace list
#define     frf_Repl    0x00000001
#define     frf_RE      0x00000002
#define  frf_GetTag     0x80000000
#define  frf_CntMin     0x40000000
#define  frf_CntMax     0x20000000
#define  frf_Got1st     0x10000000
#define  frf_InTag      0x08000000

#define  fr_Broken   (TCHAR)-1
#define  fr_Any      (TCHAR)-2
#define  fr_Previous (TCHAR)-3

#define  frf_Mask (frf_GetTag | frf_CntMin | frf_CntMax | frf_Got1st |\
   frf_InTag )

//#define  ISUPPER(a)     (( a >= 'A' ) && ( a <= 'Z' ))
//#define  ISLOWER(a)     (( a >= 'a' ) && ( a <= 'z' ))
//#define  ISALPHA(a)     ( ISUPPER(a) || ISLOWER(a) )
//#define  ISNUM(a)       (( a >= '0' ) && ( a <= '9' ))

//(), (?:), (?=), []	Parentheses and Brackets
//*, +, ?, {n}, {n,}, {n,m}	Quantifiers
//^, $, \anymetacharacter	Anchors and Sequences
//|	Alternation
#define  RESPL1(a) ((a=='\\')||(a=='(')||(a=='[')||(a=='{')||(a=='^')||(a=='$'))
#define  REMETA(a)   ((a=='\\')||(a=='^')||(a=='$')||(a=='*')||\
   (a=='+')||(a=='?')||(a=='{')||(a=='}')||(a=='.')||(a=='(')||\
   (a==')')||(a=='|')||(a=='[')||(a==']'))

//typedef  struct tagFR {
//   LIST_ENTRY  fr_Entry;
//   DWORD       fr_dwFlag;
//   INT         fr_iFind;
//   TCHAR       fr_cFind;
//   INT         fr_iRepl;
//   TCHAR       fr_szFind[264];
//   TCHAR       fr_szRepl[264];
//   TCHAR       fr_iTags;
//   TCHAR       fr_szTags[264];
//}FR, * PFR;

BOOL  bExclMLComm = TRUE;
BOOL  bAddnpsp = TRUE;
BOOL  bCheckIf  = TRUE;
// first try is -hf"//(.*)$"hr:<font color="#008000">//\1</font>$:h+
// based on -
// "<(.*)>.*<\/\1>"	Match an HTML tag. BUT DO NOT UNDERSTAND WHY the
// forward slash has an escape charater in from of it!!!
// I would have preferred to see "<(.*)>.*</\1>", meaning
// find any tag with <  > around it, followed by anything until again
// </ +"same tag" >
// This is all in the RegExp object.
//
TCHAR GetFirstRE( PFR pfr )
{
   LPTSTR lpf = &pfr->fr_szFind[0];
   TCHAR    c = 0;
   DWORD    i = strlen(lpf);
   DWORD    j;
   TCHAR    d;

   pfr->fr_cFind = c;
   pfr->fr_cLast = c;
   pfr->fr_iCntMin = 0;
   pfr->fr_iCntMax = 0;
   pfr->fr_iTags = 0;
   j = pfr->fr_dwFlag;
   pfr->fr_dwFlag &= ~(frf_Mask);   // clear any one time bits
   if( j & frf_Got1st )
   {
      c = pfr->fr_cFirst;
      pfr->fr_iFind = pfr->fr_iFirst;
      pfr->fr_cFind = c;
      pfr->fr_dwFlag |= frf_Got1st;   // set the FIRST flag
      return c;
   }
   for( j = 0; j < i; j++ )
   {
      d = lpf[j];
      if( RESPL1(d) )
      {
         d = 0;
      }
      else
      {
         c = d;
         pfr->fr_iFind = j + 1;
         pfr->fr_cFind = c;
         pfr->fr_iFirst = j + 1;
         pfr->fr_cFirst = c;
         pfr->fr_dwFlag |= frf_Got1st;   // set the FIRST flag
         break;
      }
   }
   return c;
}

TCHAR GetNextRE( PFR pfr )
{
   LPTSTR lpf = &pfr->fr_szFind[0];
   TCHAR    c = 0;
   DWORD    i = strlen(lpf);
   DWORD    j;
   TCHAR    d, d2;

   if( pfr->fr_cFind )
      pfr->fr_cLast = pfr->fr_cFind;
   pfr->fr_cFind = c;
   for( j = pfr->fr_iFind; j < i; j++ )
   {
      d = lpf[j];
      if( RESPL1(d) )
      {
         // begin of some sequence
         j++;
         d2 = d;
         d = lpf[j];
         for( ; j < i; j++ )
         {
            // stay getting 'sequence'
            switch(d2)
            {
            case '\\':   // escape char
               if( REMETA(d) )   // if a SECOND escape, or other META character
               {
                  j--;
                  goto Ret_Char;
               }
               switch(d)
               {
               case 'b':   // \b	Matches a word boundary,
                  // that is, the position between a word and a space.
                  // For example, 'er\b' matches the 'er' in "never"
                  // but not the 'er' in "verb". 
                  break;
               case 'B':   // \B	Matches a nonword boundary.
                  // 'er\B' matches the 'er' in "verb" but not the 'er' in "never".
                  break;

               }
               break;
            case '{':   // open braces, like {n} say t{2}, means find 2 t's
               // and {n,} means match at least n time {n}
               // {n,}	n is a nonnegative integer.
               // Matches at least n times. For example, 
               // 'o{2,}' does not match the "o" in "Bob" and matches all the o's in "foooood".
               // 'o{1,}' is equivalent to 'o+'. 'o{0,}' is equivalent to 'o*'.
               j++;
               for( ; j < i; j++ )
               {
                  d = lpf[j];
                  pfr->fr_iCntMin = 0;
                  pfr->fr_iCntMax = 0;
                  pfr->fr_dwFlag &= ~(frf_CntMin|frf_CntMax);  // clear flag
                  if( d == '}' )
                  {
                     pfr->fr_dwFlag |= frf_CntMin;
                     d = pfr->fr_cFind;
                     goto Ret_Char;
                  }
                  if( ISNUM(d) )
                  {
                     if( pfr->fr_dwFlag & frf_CntMax )
                        pfr->fr_iCntMax = (pfr->fr_iCntMax * 10) + (d - '0');
                     else
                        pfr->fr_iCntMin = (pfr->fr_iCntMin * 10) + (d - '0');
                  }
                  else if( d == ',' )
                  {
                     pfr->fr_dwFlag |= frf_CntMax;
                  }
               }
               break;
            case '(':   // case of "(", closed with ")", means GET TAG
               pfr->fr_dwFlag |= frf_GetTag;
               break;
            case ')':
               pfr->fr_dwFlag &= ~(frf_GetTag);
               pfr->fr_szTags[pfr->fr_iTags] = 0;
               break;
            case '.':   // get ANY single character
               d = fr_Any;

               break;
            }
         }  // get sequence loop
      }
      else
      {
Ret_Char:
         c = d;
         pfr->fr_iFind = j + 1;
         pfr->fr_cFind = c;
         break;
      }
   }

   return c;
}

INT   InStrRE( LPTSTR lpb, PFR pfr )
{
   INT   iRet = 0;
   INT   i, j, k, l, ib;
   TCHAR    c;
   LPTSTR lps = &pfr->fr_szFind[0];
   i = strlen(lpb);  // string to search
   j = strlen(lps);  // regular expression we are looking for
   if( !i || !j )
      return 0;

   //c = *lps;   // get the first we are looking for
   c = GetFirstRE(pfr);   // get the first we are looking for
   l = i;   // - ( j - 1 );   // get the maximum length to search
   ib = 0;
   for( k = 0; k < l; k++ )
   {
      if( c == fr_Broken )
      {
         break;
      }
      else if( c == fr_Any )
      {
         // this is a match with everything
         if( ib == 0 )
            ib = (k + 1);     // keep the BEGIN position of the match
         if(lpb[k] == '\n' )  // and continue to end-of-line
         {
            c = GetNextRE(pfr);
            if( c == '\n' )
               iRet = ib;  // return NUMERIC position (that is LOGICAL + 1)
            break;   // and out of the outer search loop
         }
         if( pfr->fr_dwFlag & frf_GetTag )
            pfr->fr_szTags[ pfr->fr_iTags++ ] = lpb[k];
      }
      else if( lpb[k] == c )
      {
         // found the FIRST char
         if( ib == 0 )
            ib = (k + 1);  // keep BEGIN of match
         c = GetNextRE(pfr);  // GET THE next 'FIND' CHARACTER
         if( c == (TCHAR)-1 )
            continue;
         if( pfr->fr_dwFlag & frf_GetTag )
            pfr->fr_szTags[ pfr->fr_iTags++ ] = lpb[k];
         if( ( (k + 1) == l ) &&
             ( GetNextRE(pfr) == 0 ) )
         {
            iRet = ib;
            break;
         }
      }
      else
      {
         // FAILED- return to FIRST character
         c = GetFirstRE(pfr);
      }
   }  // for the search length
   return iRet;
}

VOID  Chk_Replace_RE( LPTSTR lpbgn, DWORD dwl, PDWORD pdw, PFR pfr )
{
   DWORD    iPos, iLen;
   LPTSTR   lpf, lpr, pr;
   INT      c;

   lpf = &pfr->fr_szFind[0];
   lpr = &pfr->fr_szRepl[0];
   iLen = strlen(lpf);
   iPos = InStrRE(lpbgn, pfr);   // special MATCH of REGULAR EXPRESSION
   if(iPos && !InStr(lpbgn, lpr) )
   {
      if( iPos > 1 )
      {
         c = lpbgn[iPos - 2];
         if( ISALPHA(c) || ISNUM(c) )
            return;
      }
      c = lpbgn[iPos + iLen - 1];
      if( ISALPHA(c) || ISNUM(c) )
         return;

      pr = Right(lpbgn, ((dwl+1) - iPos - iLen));
      strcpy( &lpbgn[iPos-1], lpr );
      strcat( lpbgn, pr );
      iPos = strlen(lpbgn);
      if( iPos > dwl )
      {
         iPos -= dwl;
         gAdded   += iPos;
         gTotAdd  += iPos;
      }
      else if( iPos < dwl )
      {
         gDiscard += (dwl - iPos);
         gTotDisc += (dwl - iPos);
         iPos -= dwl;
      }
      else
         iPos = 0;

      *pdw += iPos;
   }
}

VOID  Chk_Replace_OK( LPTSTR lpbgn, DWORD dwl, PDWORD pdw, PFR pfr )
{
   DWORD    iPos, iLen;
   LPTSTR   lpf, lpr, pr;
   INT      c;

   lpf = &pfr->fr_szFind[0];
   lpr = &pfr->fr_szRepl[0];
   iLen = strlen(lpf);
   iPos = InStr(lpbgn, lpf);
   if(iPos && !InStr(lpbgn, lpr) )
   {
      if( iPos > 1 )
      {
         c = lpbgn[iPos - 2];
         if( ISALPHA(c) || ISNUM(c) )
            return;
      }
      c = lpbgn[iPos + iLen - 1];
      if( ISALPHA(c) || ISNUM(c) )
         return;

      pr = Right(lpbgn, ((dwl+1) - iPos - iLen));
      strcpy( &lpbgn[iPos-1], lpr );
      strcat( lpbgn, pr );
      iPos = strlen(lpbgn);
      if( iPos > dwl )
      {
         iPos -= dwl;
         gAdded   += iPos;
         gTotAdd  += iPos;
      }
      else if( iPos < dwl )
      {
         gDiscard += (dwl - iPos);
         gTotDisc += (dwl - iPos);
         iPos -= dwl;
      }
      else
         iPos = 0;

      *pdw += iPos;
   }
}

VOID  Chk_Replace( LPTSTR lpbgn, DWORD dwl, PDWORD pdw )
{
   PLE   pH   = &gsFndRpl;
   PLE   pN;
   PFR   pfr;

   Traverse_List( pH, pN )
   {
      pfr = (PFR)pN;
      if( pfr->fr_dwFlag & frf_RE )
         Chk_Replace_RE( lpbgn, dwl, pdw, pfr );
      else
         Chk_Replace_OK( lpbgn, dwl, pdw, pfr );
   }
}



///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Do_Fix_HTML
// Return type: BOOL 
// Arguments  : LPTSTR lpb
//            : DWORD dws
//            : LPTSTR lpf
// Description: Remove HTML tag data
//              maybe also remove 'header', if present in e-mail like files
///////////////////////////////////////////////////////////////////////////////
BOOL	Do_Fix_HTML( LPTSTR lpb, DWORD dws, LPTSTR lpf )
{
   BOOL     bRet = TRUE;
   DWORD    dwi, dwo;
   INT      c, d;
   LPTSTR   lpout = &gszOutBuf[0];
   DWORD    dwa, dwd, dwl, dwl2;
   BOOL     bDnCrLf = TRUE;
   LPTSTR   lpbgn;   // beginning of line
   BOOL     bInH;
   DWORD    dwll, dwll2;    // length of current line

   gDiscard = 0;
   gAdded   = 0;
   gConverted = 0; // convert to a SPACE
   gdwWritten = 0;

   c = d = 0;
   dwo = dwa = dwd = dwl = dwl2 = 0;
   lpbgn = lpout;    // begin is beginning of output
   bInH = FALSE;
   dwll = 0;         // current line length
   for( dwi = 0; dwi < dws; dwi++ )
   {
      c = lpb[dwi];
      if( c == '<' )
         bInH = TRUE;
      else if( c == '>' )
         bInH = FALSE;

      if( c == 0x0a )
      {
         // if we have a LF, then check previous was a CR
         if( d != 0x0d )
         {
            // if NOT, then ADD a CR
            // =====================
            lpout[dwo++] = 0x0d;
            dwa++;   // and bump the ADDED count
#ifndef  NDEBUG
            if( VERB5 ) {
               AddReport( "Added CR (0x0d) to line %d (%d)."MEOR, dwl, dwl2 );
            }
#endif   // !NDEBUG
         }
      }
      else if( c == 0x0d )
      {
         // if we have a CR
         if( (dwi + 1) < dws )
         {
            // then check if the NEXT is a LF
            if( lpb[ dwi + 1 ] != 0x0a )
            {
               // If is is NOT, then ADD the CR
               lpout[dwo++] = (TCHAR)c;   // to the output
               c = 0x0a;   // and put a LF in c
               dwa++;      // and bump the ADDED count
#ifndef  NDEBUG
               if( VERB5 ) {
                  AddReport( "Added LF (0x0a) to line %d (%d)."MEOR, dwl, dwl2 );
               }
#endif   // !NDEBUG
            }
         }
         else
         {
            // CR is the LAST char in the buffer
            lpout[dwo++] = (TCHAR)c;   // so add it to output
            c = 0x0a;   // amke c a LF
            dwa++;      // bump the ADDED count
#ifndef  NDEBUG
            if( VERB5 ) {
               AddReport( "Added LF (0x0a) to line %d (%d)."MEOR, dwl, dwl2 );
            }
#endif   // !NDEBUG
         }

         // we have either ADDED this CR, and changed to a LF
         // or are letting the CR fall through because it is
         // DEFINTIELY followed by a LF

      }  // having dealt with CR and LF
      else  // it is NOT one of those chars
      {
         dwll++;  // bump this current lines LENGTH by one

      }

      // add this character to the output
      lpout[dwo++] = (TCHAR)c;

      if( c == 0x0a )
      {
         // reached END OF LINE
         lpout[dwo] = 0;   // ZERO terminate the OUTPUT
         dwl2++;           // count another file line processed

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
               if( VERB5 ) {
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

         if( ( dwo               ) &&    // if we HAVE some output AND
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
               if( VERB5 ) {
                  AddReport( "Discarded line %d (%d) of %d chars."MEOR,
                     dwl, dwl2,
                     (gDiscard - dwpd2) );
               }
#endif   // !NDEBUG
            }
         }

         dwll2 = strlen(lpbgn);
         if( dwo && dwll2 )
         {
            Chk_Replace( lpbgn, dwll2, &dwo );
         }

         lpout[dwo] = 0;
         // if processing an e-mail, embedded into a HTML document,
         // delete the 'header' lines, if possible
         if( dwo >= MXOUTIT )
         {
            lpout[dwo] = 0;
            bRet = outit(lpout,dwo);
            dwo = 0;
            bDnCrLf = TRUE;
            if( !bRet )
               break;
         }

         lpbgn = &lpout[dwo];    // reset start of line
         dwll = 0;   // restart LENGTH OF CURRENT line

      }  // reached END OF LINE in the buffer


      d = c;   // keep previous

      if( dwo >= MXOUTB )
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

   }  // end for( dwi = 0; dwi < dws; dwi++ )


   // if add eof message
   if( VERB1 )
   {
      if( c != 0x0a )
      {
         lpout[dwo++] = 0x0d;
         lpout[dwo++] = 0x0a;
         dwa += 2;
#ifndef  NDEBUG
         if( VERB5 ) {
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
      if( VERB5 ) {
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
      // what about Rem_DS?
      bRet = outit(lpout,dwo);
      dwo = 0;
   }

   gDiscard += dwd;
   gAdded   += dwa;

   gTotDisc += dwd;
   gTotAdd  += dwa;
   gTotConv += gConverted;

   gdwTotWrite += gdwWritten;
   gliTotWrite.QuadPart += gdwWritten;

   gdwLines = dwl;
   gdwLine2 = dwl2;

#ifdef _WIN32
   UNREFERENCED_PARAMETER(lpf);
#endif

   return bRet;
}

DWORD FndRplCount( VOID )
{
   PLE   pH   = &gsFndRpl;
   PLE   pN;
   DWORD dwc = 0;
   Traverse_List( pH, pN )
   {
      dwc++;
   }
   return dwc;
}


BOOL	Do_HTML( LPTSTR lpb, DWORD dws, LPTSTR lpf )
{
   BOOL  bRet = TRUE;
   if( Pgm_Flag & Add_HTML ) { // FIX20041107
         bRet = Do_Add_HTML(lpb, dws, lpf);
   } else {
      if(FndRplCount())
         bRet = Do_Fix_HTML(lpb, dws, lpf);
      else
         bRet = Do_Del_HTML(lpb, dws, lpf);
   }
   return bRet;
}

// gsFndRpl - find/replace list
static PFR  _s_last = 0;
PVOID   Add2FRList( LPTSTR lpf, INT flg )
{
   PFR   pfr = 0;
   DWORD dwl = 0;
   if( lpf )
      dwl = strlen(lpf);
   if(dwl)
   {
      if(flg == iCreate)
      {
         pfr = (PFR)MALLOC( sizeof(FR) );
         if( pfr )
         {
             memset(pfr, 0, (sizeof(FR)));
            strcpy( &pfr->fr_szFind[0], lpf );
            InsertTailList( &gsFndRpl, (PLE)pfr );
            _s_last = pfr;
         }
      }
      else if(flg == iAddRpl)
      {
         PLE   pH, pN;
         PFR   pfr2;
         pH = &gsFndRpl;
         Traverse_List(pH, pN)
         {
            pfr2 = (PFR)pN;
            if( !( pfr2->fr_dwFlag & frf_Repl ) )
            {
               pfr = pfr2;
               strcpy( &pfr->fr_szRepl[0], lpf );
               pfr->fr_dwFlag |= frf_Repl;
               _s_last = pfr;
               break;
            }
         }
      }
      else if(flg == iSetRE)
      {
         if( _s_last )
         {
            pfr = _s_last;
            pfr->fr_dwFlag |= frf_RE;
         }
      }
   }
   return pfr;
}

INT   GetHEX2( LPTSTR lpb )
{
   INT   iret = 32;
   if( ISNUM( lpb[0] ) && ISNUM( lpb[1] ) )
   {
      iret  = (((lpb[0] - '0') << 4) & 0xf0);   // get first nibble
      iret |= ( (lpb[1] - '0') & 0x0f );        // add second nibble
   }
   else if( ISHEX( lpb[0] ) && ISHEX( lpb[1] ) )
   {
      int i1, i2;
      i1 = toupper(lpb[0]);
      i2 = toupper(lpb[1]);
      if( ISNUM(i1) )
         i1 = (((i1 - '0') << 4) & 0xf0);    // get first nibble
      else
         i1 = (((i1 - ('0'+7)) << 4) & 0xf0);    // get first nibble

      if( ISNUM(i2) )
         i2 = ((i2 - '0') & 0x0f);    // get 2nd nibble
      else
         i2 = ((i2 - ('0'+7)) & 0x0f);    // get 2nd nibble
      iret = i1 + i2;
   }
   return iret;
}

INT  _s_AddCRMsg = 0;
//#define  Plus(a,b)   ( lpb[ dwi + a ] == b )

TCHAR gszMSIf[264];
INT   iMSIf = 0;
DWORD gdwIfCnt = 0;
static TCHAR _s_sznext[264];
// could add an IF / ENDIF list, and show it at the end ... *** TBD ***

BOOL  BufGotEnd( LPTSTR lpb, PDWORD pdw, DWORD dws, INT i )  // == '>' ) )
{
   BOOL  bRet = FALSE;
   DWORD dwi = *pdw; // get current after '--' found
   DWORD dwd = 0;
   DWORD dwb;
   INT      c, k;

   dwb  = dwi; // get current
   dwi += 2;   // get past the '--' to the next char
   k    = 0;
   for( ; dwi < dws; dwi++ )
   {
      //if( lpb[dwi] == i )
      c = lpb[dwi];
      if( c == i )
      {
         dwi -= 2;   // reduce back for the TRUE
         dwd  = dwi - dwb; // count of DELETED
         *pdw = dwi;
         bRet = TRUE;
         break;
      }

      //if( dwd < 256 )
      if( bCheckIf && ( k < 256 ) )
      {
         _s_sznext[k++] = (TCHAR)c;
      }
      dwd++;
   }

   if( k & bCheckIf )
   {
      LPTSTR lpb = GetExBrace(_s_sznext); // unpack the subject
      if( strcmpi(lpb,"endif") == 0 )
      {
         if( gdwIfCnt )
            gdwIfCnt--;
      }
      else
      {
         gdwIfCnt++;
      }
   }

   return bRet;
}


// eof - FixFHTML.c


