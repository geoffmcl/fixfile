

// FixVC.c
#include	"FixF32.h"
// #include	"FixVC.h"

int do_replace = 0;
static char _s_VC_Cmd[264];
static char _s_VC_Find[264];
static char _s_VC_Replace[264];

BOOL Get_VC_Flag( PTSTR pcin )
{
   BOOL bRet = TRUE; // all ok
   INT  c;
   PTSTR pc = _s_VC_Cmd;
   PTSTR p;
   strcpy(pc,pcin);
   pc++; // bump to next, after -c[...]
   Pgm_Flag |= VC_Out;
   c = toupper(*pc);

   if(c) {
      p = strrchr(pc,':');
      if((c == 'R')&&(pc[1] == ':')&&(p)&& (p > &pc[1])){
         pc += 2; // get to the FIND
         *p = 0; // end of find
         p++;  // get to REPLACE
         strcpy(_s_VC_Find, pc);
         strcpy(_s_VC_Replace, p);
         if((_s_VC_Find[0])&&(_s_VC_Replace[0])) {
            do_replace = 1;
         }
      } else {
         bRet = FALSE; // error
      }
   }

   return bRet;
}

PTSTR  pinbuf;
DWORD  insize;
PDWORD poutcnt;
PINT poutcnt2;
PTSTR  poutput;

BOOL VC_done_replace( INT c, PDWORD pdwi )
{
   DWORD dwj, dwn;
   int d;
   PTSTR pf, pr;
   if( do_replace && ( _s_VC_Find[0] == (TCHAR)c ) ) {
      // ok, check some more, this could be it
      pf = _s_VC_Find;
      for( dwj = *pdwi; dwj < insize; dwj++ ) {
         if( *pf != pinbuf[dwj] ) {
            return FALSE;
         }
         pf++;
         if( *pf == 0 ) { // all found
            // we need to add the 'replace' string, instead
            pf = _s_VC_Find;
            pr = _s_VC_Replace;
            dwn = *poutcnt;
            while( *pr ) {
               poutput[dwn++] = *pr++;
            }
            d = *poutcnt2;
            d += (dwj - *pdwi);
            *pdwi = dwj;
            *poutcnt = dwn;
            return TRUE;
         }
      }

   }
   return FALSE;
}


BOOL	Do_VC( LPTSTR pdata, DWORD dws, LPTSTR lpf )
{
	BOOL	bRet = TRUE;
   DWORD dwi;
   INT   c, d;
   PTSTR pln2 = gszLineB2;  // add it into here
//   PTSTR pln3 = gszLineB3;  // build it into here
   DWORD dwn = 0;
   if( VERB1 ) {
      sprtf( "Doing file %s of %u bytes with -c flag."MEOR,
         GetShortName(lpf,40),
         dws );
   }

   pinbuf = pdata;
   insize = dws;
   poutcnt = &dwn;
   poutcnt2 = &d;
   poutput = pln2;

   pln2[dwn++] = '"'; // start quotes
   d = 0;
   for(dwi = 0; dwi < dws; dwi++) {
      c = pdata[dwi]; // get char
      if( c >= ' ' ) { // greater than or equal to space
         if( do_replace && ( _s_VC_Find[0] == (TCHAR)c ) ) {
            if( !VC_done_replace( c, &dwi ) ) {
               // do the normal ...
               if(( c == '\\' )||( c == '"' )) {
                  pln2[dwn++] = '\\';
                  d++;
               }
               pln2[dwn++] = (TCHAR)c;
               d++;
            } // else all done, for this 'word' ...
         } else {
               // do the normal ...
               if(( c == '\\' )||( c == '"' )) {
                  pln2[dwn++] = '\\';
                  d++;
               }
               pln2[dwn++] = (TCHAR)c;
               d++;
         }
      } else {
         if( c == '\t' ) {
            pln2[dwn++] = '\\';
            d++;
            pln2[dwn++] = 't';
            d++;
         } else {
            // if a CR or LF char
            //if(d) {
            if(c == '\n') {
               if(dwn > 1) {
                  pln2[dwn++] = '"'; // add closing quote
               } else {
                  dwn = 0; // remove openning quote
               }
               pln2[dwn] = 0;
               strcat(pln2,"MEOR"MEOR);
               dwn = strlen(pln2);
               outit( pln2, dwn );
               d = 0;
               dwn = 0;
               pln2[dwn++] = '"'; // start quotes
            }
         }
      }
   } // for(dwi = 0; dwi < dws; dwi++)


   return bRet;
}

// eof - FixVC.c

