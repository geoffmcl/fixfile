
// FixXML.c
#include "FixF32.h"
// XML format
DWORD IsValidXMLChar( TCHAR c, DWORD type );
DWORD IsValidEscapeSeq( PTSTR pfnd );

#define XLMETACHR    16 // max seen is 8!

// Special Character Table
typedef struct {
   PTSTR Description;
   PTSTR Character;
   PTSTR Code;
   PTSTR Display;
} XMLCODES, * PXMLCODES;

XMLCODES g_sXMLCodes[] = {
   { "at sign", "@", 0,0 },   // ok in body, but NOT in a LABEL
   { "ampersand",0,"&amp;","&" },
   { "approximately equal to",0,"&cong;"," " },
   { "asterisk","*",0,0 },
   { "backslash","\\", 0 ,0 },
   { "bracket, left curly","{",0,0 },
   { "bracket, left square","[", 0,0 },
   { "bracket, right curly","}", 0,0 },
   { "bracket, right square","]", 0,0 },
   { "bullet", 0, "&bull;"," " },
   { "caret","^", 0, 0 },
   { "cent sign", 0, "&cent;"," " },
   { "circumflex", 0, "&circ;"," " },
   { "colon",":", 0, 0 },
   { "comma",",",0,0 },
   { "copyright sign",0,"&copy;"," " },
   { "currency sign, general",0,"&curren;"," " },
   { "dagger",0,"&dagger;"," " },
   { "dagger, double",0,"&Dagger;"," " },
   { "degree sign",0,"&deg;"," " },
   { "delta, capital",0,"&Delta;"," " },
   { "diamond, open",0,"&#x2756;"," " },
//   { "diamond, solid",0,"&#x2726;" }, // not yet supported
   { "diamond, white (contains small black)",0,"&#x25c8;"," " },
   { "division sign",0,"&#x247;"," " },
   { "dollar sign","$",0,0 },
   { "ellipse",0,"&hellip;"," " },
   { "em-dash",0,"&mdash;"," " },
   { "en-dash",0,"&ndash;"," " },
   { "equal sign","=",0,0 },
   { "Eurodollar",0,"&euro;"," " },
   { "exclamation mark","!",0,0 },
   { "exclamation mark, inv",0,"&iexcl;"," " },
   { "fleuron",0,"&#x2766;"," " },
   { "florin",0,"&fnof;"," " },
   { "fraction, one-quarter",0,"&frac14;"," " },
   { "fraction, one-half",0,"&frac12;"," " },
   { "fraction, three-quarters",0,"&frac34;"," " },
   { "grave accent","'",0,0 },
   { "greater-than sign",0,"&gt;",">" },
   { "greater-or-equal sign",0,"&ge;"," " },
   { "hyphen","-",0,0 },
   { "infinity sign",0,"&infin;"," " },
   { "Lb sign",0,"&pound;"," " },
   { "less-than sign",0,"&lt;","<" },
   { "less-or-equal sign",0,"&le;"," " },
   { "letters, lowercase","a-z",0,0 },
   { "letters, uppercase","A-Z",0,0 },
   { "micro sign (mu, lc)",0,"&micro;"," " },
   { "middle dot",0,"&middot;"," " },
   { "multiplication sign",0,"&#x215;"," " },
   { "not sign",0,"&not;"," " },
   { "numerals","0-9",0,0 },
   { "or","|",0,0 },
   { "omega",0,"&omega;"," " },
   { "ordinal, feminine",0,"&ordf;"," " },
   { "ordinal, masculine",0,"&ordm;"," " },
   { "parenthesis, left","(",0,0 },
   { "parenthesis, right",")",0,0 },
   { "percentage sign","%",0,0 },
   { "percentile/mil",0,"&permil;"," " },
   { "period",".",0,0 },
   { "Pi, cap",0,"&Pi;"," " },
   { "pi, lc",0,"&pi;"," " },
   { "plus sign","+",0,0 },
   { "plus/minus sign",0,"&plusmn;"," " },
   { "pound sign","#",0,0 },
   { "question mark","?",0,0 },
   { "question mark, inverted",0,"&iquest;"," " },
   { "quote, double","\"", 0,0 },
   { "quote, left double angle",0,"&laquo;"," " },
   { "quote, left double",0,"&ldquo;"," " },
   { "quote, left single",0,"&lsquo;"," " },
   { "quote, left single angle",0,"&lsaquo;"," " },
   { "quote, right double angle",0,"&raquo;"," " },
   { "quote, right double",0,"&rdquo;"," " },
   { "quote, right single",0,"&rsquo;"," " },
   { "quote, right single angle",0,"&rsaquo;"," " },
   { "quote, single","'",0,0 },
   { "registered sign",0,"&reg;"," " },
   { "section sign",0,"&sect;"," " },
   { "Semicolon",";", 0,0 },
   { "sigma, cap",0,"&Sigma;"," " },
   { "slash","/",0,0 },
   { "space, non-breaking",0,"&nbsp;"," " },
   { "square root",0,"&radic;"," " },
   { "superscript 1",0,"&sup1;"," " },
   { "superscript 2",0,"&sup2;"," " },
   { "superscript 3",0,"&sup3;"," " },
//   { "superscript numerals","use <sup>tags</sup>" },
   { "tilde","~",0,0 },
   { "trademark sign",0,"&#x8482;"," " },
//   { "underline","__" or use <u>tags</u>
   { "underscore","_", 0,0 },
   { "Yen sign",0,"&yen;"," " },
   { "A-acute, cap",0,"&Aacute;"," " },
   { "a-acute, lc",0,"&aacute;"," " },
   { "A-circumflex, cap",0,"&Acirc;"," " },
   { "a-circumflex, lc",0,"&acirc;"," " },
   { "AE ligature, cap",0,"&AElig;"," " },
   { "ae ligature, lc",0,"&aelig;"," " },
   { "A-grave, cap",0,"&Agrave;"," " },
   { "a-grave, lc",0,"&agrave;"," " },
   { "A-ring, cap",0,"&Aring;"," " },
{ "a-ring, lc",0,"&aring;"," " },
{ "A-tilde, cap",0,"&Atilde;"," " },
{ "a-tilde, lc",0,"&atilde;"," " },
{ "A-umlaut, cap",0,"&Auml;"," " },
{ "a-umlaut, lc",0,"&auml;"," " },
{ "C-cedille, cap",0,"&Ccedil;"," " },
{ "c-cedille, lc",0,"&ccedil;"," " },
{ "E-acute, cap",0,"&Eacute;"," " },
{ "e-acute, lc",0,"&eacute;"," " },
{ "E-circumflex, cap",0,"&Ecirc;"," " },
{ "e-circumflex, lc",0,"&ecirc;"," " },
{ "E-grave, cap",0,"&Egrave;"," " },
{ "e-grave, lc",0,"&egrave;"," " },
{ "Eth, cap (Icelandic)",0,"&ETH;"," " },
{ "eth, lc (Icelandic)",0,"&eth;"," " },
{ "E-umlaut, cap",0,"&Euml;"," " },
{ "e-umlaut, lc",0,"&euml;"," " },
{ "I-acute, cap",0,"&Iacute;"," " },
{ "i-acute, lc",0,"&iacute;"," " },
{ "I-circumflex, cap",0,"&Icirc;"," " },
{ "i-circumflex, lc",0,"&icirc;"," " },
{ "I-grave, cap",0,"&Igrave;"," " },
{ "i-grave, lc",0,"&igrave;"," " },
{ "I-umlaut, cap",0,"&Iuml;"," " },
{ "i-umlaut, lc",0,"&iuml;"," " },
{ "N-tilde, cap",0,"&Ntilde;"," " },
{ "n-tilde, lc",0,"&ntilde;"," " },
{ "O-acute, cap",0,"&Oacute;"," " },
{ "o-acute, lc",0,"&oacute;"," " },
{ "O-circumflex, cap",0,"&Ocirc;"," " },
{ "o-circumflex, lc",0,"&ocirc;"," " },
{ "OE ligature, cap",0,"&OElig;"," " },
{ "oe ligature, lc",0,"&oelig;"," " },
{ "O-grave, cap",0,"&Ograve;"," " },
{ "o-grave, lc",0,"&ograve;"," " },
{ "O-slash, cap",0,"&Oslash;"," " },
{ "o-slash, lc",0,"&oslash;"," " },
{ "O-tilde, cap",0,"&Otilde;"," " },
{ "o-tilde, lc",0,"&otilde;"," " },
{ "O-umlaut, cap",0,"&Ouml;"," " },
{ "o-umlaut, lc",0,"&ouml;"," " },
{ "S-caron, cap",0,"&Scaron;"," " },
{ "s-caron, lc",0,"&scaron;"," " },
{ "sz ligature, lc (German)",0,"&szlig;"," " },
{ "Thorn, cap (Icelandic)",0,"&THORN;"," " },
{ "thorn, lc (Icelandic)",0,"&thorn;"," " },
{ "U-acute, cap",0,"&Uacute;"," " },
{ "u-acute, lc",0,"&uacute;"," " },
{ "U-circumflex, cap",0,"&Ucirc;"," " },
{ "u-circumflex, lc",0,"&ucirc;"," " },
{ "U-grave, cap",0,"&Ugrave;"," " },
{ "u-grave, lc",0,"&ugrave;"," " },
{ "U-umlaut, cap",0,"&Uuml;"," " },
{ "u-umlaut, lc",0,"&uuml;"," " },
{ "Y-acute, cap",0,"&Yacute;"," " },
{ "y-acute, lc",0,"&yacute;"," " },
{ "y-umlaut, lc",0,"&yuml;"," " },
// and TERMINATION
   { 0, 0, 0, 0 }
};

PTSTR g_pActMetChr = 0;
PXMLCODES pactx = 0;
BOOL  IsActiveChar( TCHAR c )
{
   if( pactx != 0 ) {
      PTSTR pc = pactx->Character;
      if( pc != 0 ) {
         if( *pc == c )
            return TRUE;
      }
   }
   return FALSE;
}

DWORD IsValidXMLChar( TCHAR c, DWORD type )
{
   PXMLCODES px = &g_sXMLCodes[0];
   DWORD dwoff = 0;

   pactx = 0;

   if(( type == 1   ) &&  // if IN a LABEL definition
      ( c    == '@' ) )
      return 0; // *** FAIL an '@' in a LABEL ***

   while( px->Description ) {
      PTSTR pc = px->Character;
      dwoff++;
      if(pc) { // if a POINTER to the character
         DWORD len = strlen(pc);
         if( len == 1 ) {  // single, or

            if( *pc == c ) {
               pactx = px;
               return dwoff;
            }
         } else if( len == 3 ) { // range

            if(( c >= pc[0] ) && ( c <= pc[2] )) {
               pactx = px;
               return dwoff;
            }
         }
     }
      px++;
   }


   return 0; // is NOT valid XML character
}

DWORD IsValidEscapeSeq( PTSTR pfnd )
{
   PXMLCODES px = &g_sXMLCodes[0];
   DWORD dwoff = 0;
   while( px->Description ) {
      PTSTR pc = px->Code;
      dwoff++;
      if(pc) {
         if( strcmp( pfnd, pc ) == 0 ) {
            g_pActMetChr = pc;
            return dwoff;
         }
     }
      px++;
   }


   g_pActMetChr = 0;
   return 0;
}

PTSTR GetXMLMetfor( PTSTR pDesc )
{
   PXMLCODES px = &g_sXMLCodes[0];
   while( px->Description ) {
      if( strcmp(px->Description,pDesc) == 0 ) {
         if( px->Code )
            return px->Code;
         
         return 0;

      }
      px++;
   }

   return 0;
}

PTSTR GetXMLMetamp( VOID )
{
   return( GetXMLMetfor( "ampersand" ) );
}

DWORD ValidXMLMetaChar( PTSTR pb, DWORD dwn )
{
   PTSTR p = pb;
   DWORD len = 0;
   while(( *p > ' ' ) && ( *p != ';' ) && dwn ) {
      p++;
      len++;
      dwn--;
   }
   if( *p == ';' )
      len++;

   if(len) {
      DWORD dwi;
      PTSTR ptmp = gszTmpBuf; // be aware this is used in FixJobs.c
//    PTSTR ptmp = gszTmpBuf; // this was set up in FixXML.c!!!

      for( dwi = 0; dwi < len; dwi++ )
      {
         ptmp[dwi] = pb[dwi];
      }
      ptmp[dwi] = 0;

      return ( IsValidEscapeSeq( ptmp ) );

   }

   g_pActMetChr = 0;
   return 0;

}

// extern PTSTR GetXMLMeta4( TCHAR c ); // convert say ">" to "&gt;"
PTSTR GetXMLMeta4( TCHAR c )
{
   PXMLCODES px = &g_sXMLCodes[0];
   while( px->Description ) {
      PTSTR pc = px->Code;
      PTSTR pd = px->Display;
      if( pc && pd && ( *pd != ' ' ) ) {
         if( *pd == c )
            return pc;
      }
      px++;
   }

   return 0;
}
 

// EOF
// FixXML.c
