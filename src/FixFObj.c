
// FixFObj.c
// Process as a C/C++ and output function names that have body
// COFF -
#include "FixF32.h"

#if   AddFunc
// FUNCTION LIST EXTRACTION
#undef   ADDDO2
#define  SKIPDQUOTES

extern   VOID  Do_Happy_Msg(VOID);
extern   VOID  Do_VerMsg(VOID);

INT   gdwFunOut = ifo_Def;
BOOL  bDebug = False;   // special -f:d for DEBUG
DWORD dwMxSLine = 75;
BOOL  bBeTidy = TRUE;
DWORD giMinTLen = DEF_MINTLEN;
DWORD gdwMaxLn  = DMAXLINE;      // can be changed with -f:DMAXLINE=nnnn

BOOL  bFixFun = TRUE;
BOOL  bSpaceFun = FALSE;

TCHAR sszBBr[]  = "{";
TCHAR sszEBr[]  = "}";
TCHAR sszBBr2[] = "(";
TCHAR sszEBr2[] = ")";
TCHAR sszBSB[]  = "[";
TCHAR sszESB[]  = "]";

// ***** SHORT LIST OF 40 FUNCTIONS IN fixfobj.c *****
// ************************************************************************
// FixFunBody() GetTidyFunc() OutFunList() InStr() InStrWhole() myInStrF()
// Left() Right() Mid() IsInclude() IsPragma() IsDefine() IsAnIf()
// CondLine() MaxTrim() Add2sBlk() stLen() Get1Line() GetFunc() GetALine()
// IsStruct() InStrCnt() InStrCntF() OpenCnt() CloseCnt() OpenCntF()
// CloseCntF() LastInStr() LastInStrF() IsEndStruct() StripComments() IsDo()
// IsEndDo() IsArrayDef() IsArrayEnd() Out_Funcs() ShowFHelp() All2Upper()
// GetListType() GetMaxLn()
// ************************************************************************
// LIST OF 40 FUNCTIONS IN fixfobj.c
// ***************************************************************************************
// INT      FixFunBody( LPTSTR lps )
// VOID     GetTidyFunc( LPTSTR lpl, LPTSTR lps, DWORD dwMin )
// DWORD    OutFunList( PLE pHead, DWORD iCnt, PLE pSht, LPTSTR lpf, PDWORD pdw, PLE pStr )
// INT      InStr( LPTSTR lpb, LPTSTR lps )
// INT      InStrWhole( LPTSTR lpb, LPTSTR lps )
// INT      myInStrF( LPTSTR sStg, LPTSTR sFnd )
// LPTSTR   Left( LPTSTR lpl, DWORD dwi )
// LPTSTR   Right( LPTSTR lpl, DWORD dwl )
// LPTSTR   Mid( LPTSTR lpl, DWORD dwb, DWORD dwl )
// BOOL     IsInclude( LPTSTR sStg )
// BOOL     IsPragma( LPTSTR sStg )
// BOOL     IsDefine( LPTSTR sStg )
// INT      IsAnIf( LPTSTR sStg )
// INT      CondLine( LPTSTR lpb )
// INT      MaxTrim( LPTSTR lpb )
// PBLK     Add2sBlk( PLE pHead, LPTSTR lpl )
// DWORD    stLen( PLE pHead )
// DWORD    Get1Line( PLE pHead, LPTSTR lpl, DWORD dwmax )
// INT      GetFunc( PLE pHead, LPTSTR lpl, DWORD dwmax )
// DWORD    GetALine( LPTSTR lpl, DWORD dwmax, LPTSTR lpb, DWORD dws, PDWORD pdwi )
// BOOL     IsStruct( LPTSTR sStg )
// INT      InStrCnt( LPTSTR sStg, LPTSTR sFnd )
// INT      InStrCntF( LPTSTR sStg, LPTSTR sFnd )
// INT      OpenCnt( LPTSTR sStg )
// INT      CloseCnt( LPTSTR sStg )
// INT      OpenCntF( LPTSTR sStg )
// INT      CloseCntF( LPTSTR sStg )
// INT      LastInStr( LPTSTR sStg, LPTSTR sFnd )
// INT      LastInStrF( LPTSTR sStg, LPTSTR sFnd )
// BOOL     IsEndStruct( LPTSTR sStg )
// INT      StripComments( LPTSTR sStg )
// BOOL     IsDo( LPTSTR sStg )
// BOOL     IsEndDo( LPTSTR sStg )
// BOOL     IsArrayDef( LPTSTR lpl )
// BOOL     IsArrayEnd( LPTSTR lpl )
// BOOL     Out_Funcs( LPTSTR lpb, DWORD dws, LPTSTR lpf )
// VOID     ShowFHelp( VOID )
// INT      All2Upper( LPTSTR lps )
// BOOL     GetListType( LPTSTR lpcmd )
// DWORD    GetMaxLn( VOID )
// ***************************************************************************************

// Local list of 40 functions in fixfobj.c
INT      FixFunBody( LPTSTR lps );
VOID     GetTidyFunc( LPTSTR lpl, LPTSTR lps, DWORD dwMin );
DWORD    OutFunList( PLE pHead, DWORD iCnt, PLE pSht, LPTSTR lpf, PDWORD pdw, PLE pStr );
INT      InStr( LPTSTR lpb, LPTSTR lps );
INT      InStrWhole( LPTSTR lpb, LPTSTR lps );
INT      myInStrF( LPTSTR sStg, LPTSTR sFnd );
LPTSTR   Left( LPTSTR lpl, DWORD dwi );
LPTSTR   Right( LPTSTR lpl, DWORD dwl );
LPTSTR   Mid( LPTSTR lpl, DWORD dwb, DWORD dwl );
BOOL     IsInclude( LPTSTR sStg );
BOOL     IsPragma( LPTSTR sStg );
BOOL     IsDefine( LPTSTR sStg );
INT      IsAnIf( LPTSTR sStg );
INT      CondLine( LPTSTR lpb );
INT      MaxTrim( LPTSTR lpb );
PBLK     Add2sBlk( PLE pHead, LPTSTR lpl );
DWORD    stLen( PLE pHead );
DWORD    Get1Line( PLE pHead, LPTSTR lpl, DWORD dwmax );
INT      GetFunc( PLE pHead, LPTSTR lpl, DWORD dwmax );
DWORD    GetALine( LPTSTR lpl, DWORD dwmax, LPTSTR lpb, DWORD dws, PDWORD pdwi );
BOOL     IsStruct( LPTSTR sStg );
INT      InStrCnt( LPTSTR sStg, LPTSTR sFnd );
INT      InStrCntF( LPTSTR sStg, LPTSTR sFnd );
INT      OpenCnt( LPTSTR sStg );
INT      CloseCnt( LPTSTR sStg );
INT      OpenCntF( LPTSTR sStg );
INT      CloseCntF( LPTSTR sStg );
INT      LastInStr( LPTSTR sStg, LPTSTR sFnd );
INT      LastInStrF( LPTSTR sStg, LPTSTR sFnd );
BOOL     IsEndStruct( LPTSTR sStg );
INT      StripComments( LPTSTR sStg );
BOOL     IsDo( LPTSTR sStg );
BOOL     IsEndDo( LPTSTR sStg );
BOOL     IsArrayDef( LPTSTR lpl );
BOOL     IsArrayEnd( LPTSTR lpl );
BOOL     Out_Funcs( LPTSTR lpb, DWORD dws, LPTSTR lpf );
VOID     ShowFHelp( VOID );
INT      All2Upper( LPTSTR lps );
BOOL     GetListType( LPTSTR lpcmd );
DWORD    GetMaxLn( VOID );
// ===========================================

INT   FixFunBody( LPTSTR lps )
{
   INT   iPos1, iPos2;
   INT   iLen;

   iLen  = Len(lps);
   iPos1 = InStr(lps, "(");
   iPos2 = InStr(lps, ")");
   if( ( iLen > 2 ) &&
       ( iPos1 == 1 ) &&
       ( iPos2 == iLen ) )
   {
      // only act on "(.)" exactly
      if( bSpaceFun )
      {
         if( ( lps[1] != ' ' ) ||
             ( lps[iLen - 2] != ' ' ) )
         {
            // and only if no spaces already
            LPTSTR   ptmp = Mid(lps, 2, (iLen - 2));   // get after the '('
            iLen = MaxTrim(ptmp);
            strcpy(lps, "( ");
            strcat(lps, ptmp);
            strcat(lps, " )");
            iLen = Len(lps);
         }
      }
      else
      {
         if( ( lps[1] == ' ' ) ||
             ( lps[iLen - 2] == ' ' ) )
         {
            // and only if no spaces already
            LPTSTR   ptmp = Mid(lps, 2, (iLen - 2));   // get after the '('
            iLen = MaxTrim(ptmp);
            strcpy(lps, "(");
            strcat(lps, ptmp);
            strcat(lps, ")");
            iLen = Len(lps);
         }
      }
   }
   return iLen;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : GetTidyFunc
// Return type: VOID 
// Arguments  : LPTSTR lpl
//            : LPTSTR lps
//            : DWORD dwMin
// Description: Try to adjust the funtion name with a minimum "return type"
//              followed by name.
///////////////////////////////////////////////////////////////////////////////
VOID  GetTidyFunc( LPTSTR lpl, LPTSTR lps, DWORD dwMin )
{
   INT   iLen, iPos, iLen2, iPos2;
   LPTSTR   ptmp, ptmp1, ptmp2, ptmp3;

   *lpl = 0;
   if( bBeTidy )
   {
      iLen = MaxTrim(lps);
      iPos = InStr(lps, "(");
      if(iPos)
      {
         ptmp = Left(lps, (iPos - 1));    // get left "name" of function, excl '('
         ptmp3 = Right(lps, (iLen - (iPos - 1)));
         if( bFixFun )
            iLen2 = FixFunBody(ptmp3);
         iLen2 = MaxTrim(ptmp);
         if(iLen2)
         {
            iPos2 = LastInStr(ptmp, " ");
            if(iPos2)
            {
               ptmp1 = Left(ptmp, iPos2);  // get return and type, incl ' '
               ptmp2 = Right(ptmp, (iLen2 - iPos2));  // get name
               while( Len(ptmp1) < dwMin )
                  strcat(ptmp1, " ");
               // put function back together
               strcpy(lpl, ptmp1);     // begin with return (and type)
               strcat(lpl, ptmp2);     // add back function name
               strcat(lpl, ptmp3);     // and (potentially modified) body
            }
         }
      }
   }

   if( *lpl == 0 )
      strcpy(lpl, lps);
}


// MAIN PURPOSE OF MODULE - OUTPUT SOME LISTS
// ******************************************
///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : OutFunList
// Return type: DWORD 
// Arguments  : PLE pHead
//            : DWORD iCnt
//            : PLE pSht
//            : LPTSTR lpf
//            : PDWORD pdw
//            : PLE pStr
// Description: Output the collected FUNCTION LISTS
//              Either ifo_Short and/or ifo_Full per INT iFout value
///////////////////////////////////////////////////////////////////////////////
DWORD OutFunList( PLE pHead, DWORD iCnt, PLE pSht, LPTSTR lpf, PDWORD pdw, PLE pStr )
{
   LPTSTR      sMsg = &gszMsgBuf[0];
   LPTSTR      lpl = &gszLineB1[0];   // = [DMAXLINE]
   LPTSTR      lpt = &gszLineB2[0];   // = [DMAXLINE]
   PBLK        pb;
   PLE         pNext;
   DWORD       dwi, dwl, dwc;
   DWORD       dwlns = 0;
   DWORD       dwtot = 0;
   LPTSTR      lps, ptmp, ptmp2;
   INT         iPos, iLen, iPos2, iLen2;

   if( gpLineBuf1 )
      lpl = gpLineBuf1;
   if( gpLineBuf2 )
      lpt = gpLineBuf2;

   if( gdwFunOut & ifo_Short )
   {
      dwc = 0;
      Traverse_List( pSht, pNext )
      {
         dwc++;
      }
      if(dwc)
      {
         strcpy(sMsg, "// ***** SHORT LIST OF ");
         sprintf(EndBuf(sMsg), "%d", dwc );
         strcat(sMsg, " FUNCTIONS IN ");
         gmGetFileTitle( lpt, lpf, 256 );
         strcat(sMsg, lpt);
         strcat(sMsg, " *****"MEOR );
         dwl = dwMxSLine;

         dwi = strlen(sMsg);
         outit( sMsg, dwi );
         dwlns++;
         dwtot += dwi;

         strcpy(sMsg, "// *");
         while( strlen(sMsg) < dwl )
            strcat(sMsg,"*");
         strcat(sMsg,MEOR);

         dwi = strlen(sMsg);
         outit( sMsg, dwi );
         dwlns++;
         dwtot += dwi;

         strcpy(sMsg, "//");
         dwi = strlen(sMsg);
         Traverse_List( pSht, pNext )
         {
            pb = (PBLK)pNext;
            if( (dwi + pb->b_Len + 2) > dwl )
            {
               strcat(sMsg,MEOR);

               dwi = strlen(sMsg);
               outit( sMsg, dwi );
               dwlns++;
               dwtot += dwi;

               strcpy(sMsg, "//");
            }
            strcat(sMsg," ");
            strcat(sMsg, &pb->b_Stg[0]);
            strcat(sMsg, "()");
            dwi = strlen(sMsg);
         }
         if(dwi)
         {
            strcat(sMsg,MEOR);

            dwi = strlen(sMsg);
            outit( sMsg, dwi );
            dwlns++;
            dwtot += dwi;
         }
      
         strcpy(sMsg, "// *");
         while( strlen(sMsg) < dwl )
            strcat(sMsg,"*");
         strcat(sMsg,MEOR);

         dwi = strlen(sMsg);
         outit( sMsg, dwi );
         dwlns++;
         dwtot += dwi;
      }
   }

   if( gdwFunOut & ifo_Full )
   {
      strcpy(sMsg, "// LIST OF ");
      sprintf(EndBuf(sMsg), "%d", iCnt );
      strcat(sMsg, " FUNCTIONS IN " );
      gmGetFileTitle( lpt, lpf, 256 );
      strcat(sMsg, lpt);
      strcat(sMsg, MEOR );

      dwi = strlen(sMsg);
      dwl = dwi;
      Traverse_List( pHead, pNext )
      {
         pb = (PBLK)pNext;
         if( (pb->b_Len + 5) > dwl )
            dwl = pb->b_Len + 5;
      }
      outit( sMsg, dwi );
      dwlns++;
      dwtot += dwi;

      strcpy(sMsg, "// *");
      while( strlen(sMsg) < dwl )
         strcat(sMsg,"*");
      strcat(sMsg,MEOR);

      dwi = strlen(sMsg);
      outit( sMsg, dwi );
      dwlns++;
      dwtot += dwi;

      Traverse_List( pHead, pNext )
      {
         pb = (PBLK)pNext;
         lps = &pb->b_Stg[0];
         strcpy(sMsg, "// ");
         if( bBeTidy )
         {
            GetTidyFunc( lpl, lps, giMinTLen );
            strcat(sMsg, lpl);
         }
         else
            strcat(sMsg, lps);
         strcat(sMsg,MEOR);

         dwi = strlen(sMsg);
         outit( sMsg, dwi );
         dwlns++;
         dwtot += dwi;
      }
   
      strcpy(sMsg, "// *");
      while( strlen(sMsg) < dwl )
         strcat(sMsg,"*");
      strcat(sMsg,MEOR);

      dwi = strlen(sMsg);
      outit( sMsg, dwi );
      dwlns++;
      dwtot += dwi;
   }

   if( gdwFunOut & ifo_Global )
   {
      strcpy(sMsg, MEOR"// Global list of ");
      sprintf(EndBuf(sMsg), "%d", iCnt );
      strcat(sMsg, " functions in " );
      gmGetFileTitle( lpt, lpf, 256 );
      strcat(sMsg, lpt);
      strcat(sMsg, MEOR );

      dwi = strlen(sMsg);
      outit( sMsg, dwi );
      dwlns++;
      dwtot += dwi;
      dwl = dwi;

      Traverse_List( pHead, pNext )
      {
         pb = (PBLK)pNext;
         lps = &pb->b_Stg[0];
         strcpy(sMsg, "extern ");
         if( bBeTidy )
         {
            GetTidyFunc( lpl, lps, giMinTLen );
            strcat(sMsg, lpl);
         }
         else
            strcat(sMsg, lps);
         strcat(sMsg, ";");
         strcat(sMsg,MEOR);

         dwi = strlen(sMsg);
         outit( sMsg, dwi );
         dwlns++;
         dwtot += dwi;
      }
   
      strcpy(sMsg, "// =");
      while( strlen(sMsg) < dwl )
         strcat(sMsg,"=");
      strcat(sMsg,MEOR);
      strcat(sMsg,MEOR);

      dwi = strlen(sMsg);
      outit( sMsg, dwi );
      dwlns++;
      dwtot += dwi;

   }

   if( gdwFunOut & ifo_Local )
   {
      strcpy(sMsg, MEOR"// Local list of ");
      sprintf(EndBuf(sMsg), "%d", iCnt );
      strcat(sMsg, " functions in " );
      gmGetFileTitle( lpt, lpf, 256 );
      strcat(sMsg, lpt);
      strcat(sMsg, MEOR );

      dwi = strlen(sMsg);
      outit( sMsg, dwi );
      dwlns++;
      dwtot += dwi;
      dwl = dwi;

      Traverse_List( pHead, pNext )
      {
         pb = (PBLK)pNext;
         lps = &pb->b_Stg[0];
         *sMsg = 0;
         if( bBeTidy )
         {
            GetTidyFunc( lpl, lps, giMinTLen );
            strcat(sMsg, lpl);
         }
         else
            strcat(sMsg, lps);
         strcat(sMsg, ";");
         strcat(sMsg,MEOR);

         dwi = strlen(sMsg);
         outit( sMsg, dwi );
         dwlns++;
         dwtot += dwi;
      }
   
      strcpy(sMsg, "// =");
      while( strlen(sMsg) < dwl )
         strcat(sMsg,"=");
      strcat(sMsg,MEOR);
      strcat(sMsg,MEOR);

      dwi = strlen(sMsg);
      outit( sMsg, dwi );
      dwlns++;
      dwtot += dwi;

   }

   if( gdwFunOut & ifo_Struct )
   {
      dwc = 0;
      *lpl = 0;
      Traverse_List( pStr, pNext )
      {
         pb = (PBLK)pNext;
         lps = &pb->b_Stg[0];
         if( IsEndStruct(lps) )
         {
            iPos = InStrWhole(lps, "struct");
            iLen = Len(lps);
            if( ( iPos > 0 ) && (iLen > 7) )
            {
               ptmp = Right(lps, (iLen - (iPos+5)));  // get AFTER "struct"
               iLen = MaxTrim(ptmp);    // trim it up, and get resultant lenght
               iPos2 = InStr(ptmp,sszBBr);  // get the beginning open braces
               if( iPos2 > 0 )
               {
                  ptmp2 = 0;  // assume NO tag name
                  if( iPos2 > 1 )   // appear to have a tag name
                  {
                     ptmp2 = Left(ptmp, (iPos2 - 1));
                     iLen2 = MaxTrim(ptmp2);
                     if( iLen2 == 0 )
                        ptmp2 = 0;
                  }
                  ptmp = Right(ptmp, (iLen - iPos2)); // get remainder after open brace
                  iLen = MaxTrim(ptmp);    // trim it up, and get resultant lenght
                  iPos = LastInStr(ptmp, sszEBr);
                  if (iPos > 0) // Then
                  {
                     ptmp = Right(ptmp, (iLen - iPos)); // get AFTER last brace
                     iLen = MaxTrim(ptmp);
                     iPos = InStr(ptmp,";"); // get the END OF STRUCTURE definition
                     if(iPos > 1)   // must have at least one character following
                     {
                        ptmp = Left(ptmp, (iPos - 1));
                        iLen = MaxTrim(ptmp);
                        if( *lpl == 0 )
                           strcat(lpl, "// struct ");
                        else
                           strcat(lpl,MEOR"// struct ");

                        if( ptmp2 )
                        {
                           strcat(lpl, ptmp2);
                           strcat(lpl,", ");
                        }
                        strcat(lpl, ptmp);
                        dwc++;   // count a structure definition
                     }
                     else if(ptmp2)
                     {
                        // we only have a tag name
                        if( *lpl == 0 )
                           strcat(lpl, "// struct ");
                        else
                           strcat(lpl,MEOR"// struct ");

                        strcat(lpl, ptmp2);
                        dwc++;   // count a structure definition
                     }
                  }
               }
            }
         }
      }

      dwi = strlen(lpl);
      if(dwc && dwi)
      {
         strcat(lpl, MEOR);
         if(dwc > 1)
            sprintf(sMsg, "// ===== %d STRUCTURES DEFINED IN ", dwc);
         else
            strcpy(sMsg, "// ===== STRUCTURE DEFINED IN ");
         gmGetFileTitle( lpt, lpf, 256 );
         strcat(sMsg, lpt);
         strcat(sMsg, " ====="MEOR );
         dwl = strlen(sMsg) - 5;

         dwi = strlen(sMsg);
         outit( sMsg, dwi );
         dwlns++;
         dwtot += dwi;

         dwi = strlen(lpl);
         outit( lpl, dwi );
         dwlns++;
         dwtot += dwi;

         strcpy(sMsg, "// ");
         while(strlen(sMsg) < dwl)
            strcat(sMsg, "=");
         strcat(sMsg,MEOR);
         strcat(sMsg,MEOR);

         dwi = strlen(sMsg);
         outit( sMsg, dwi );
         dwlns++;
         dwtot += dwi;

      }
   }

   *pdw = dwtot;
   if( VERB5 )
   {
      sprtf( "Have output %d lines of function information (%d bytes)."MEOR,
         dwlns,
         dwtot );
   }
   return dwlns;

}


#if 0 // code to UTIL module
///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : InStrWhole
// Return type: INT 
// Arguments  : LPTSTR lpb
//            : LPTSTR lps
// Description: Specialised InStr() function
//              Not only searches for the lps string, but ensures it is whole!
// That is it is preceeded and followed by non-alpha-numeric characters!!
///////////////////////////////////////////////////////////////////////////////
INT   InStrWhole( LPTSTR lpb, LPTSTR lps )
{
   INT   iRet = 0;
   INT   i, j, k, l, m;
   TCHAR    c, cc, pc;
   i = strlen(lpb);
   j = strlen(lps);
   if( i && j && ( i >= j ) )
   {
      c = *lps;   // get the first we are looking for
      l = i - ( j - 1 );   // get the maximum length to search
      pc = 0;  // previous "ASSUMED" to not be alpha-numeric
      for( k = 0; k < l; k++ )
      {
         cc = lpb[k];
         if( cc == c )
         {
            // found the FIRST char
            if( !IsAlphaNumeric(pc) )
            {
               // and is preceeded by NON-alpha-numeric, so
               // check until end of compare string
               for( m = 1; m < j; m++ )
               {
                  if( lpb[k+m] != lps[m] )   // on first NOT equal
                     break;   // out of here, the inner search loop
               }
               if( m == j )   // if we reached the end of the search string
               {
                  if( !IsAlphaNumeric( lpb[k+m] ) )  // check NEXT character
                  {
                     iRet = k + 1;  // return NUMERIC position (that is LOGICAL + 1)
                     break;   // and out of the outer search loop
                  }
               }
            }
         }
         pc = cc;    // keep previous
      }  // for the search length
   }
   return iRet;
}

#endif   // #if 0 // code to UTIL module

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : myInStrF
// Return type: INT 
// Arguments  : LPTSTR sStg
//            : LPTSTR sFnd
// Description: Specialised version of InStr in that it IGNORES finds that are
//              WITHIN text QUOTATIONS. ie A find "{" in strcat(lpb,"{") would
// fail.
///////////////////////////////////////////////////////////////////////////////
INT   myInStrF(LPTSTR sStg, LPTSTR sFnd)
{
   INT   iPos, iLen, iLen2;
   INT   iPos1, iPos2;
   LPTSTR   sTmp, sTmp2;

   iPos = 0;
   iLen = Len(sStg);
   if (iLen > 0) // Then
   {
      iPos = InStr(sStg, sFnd);
      if (iPos > 1) // Then              ' if AFTER the beginning of the line
      {
         iPos1 = InStr(sStg, "\""); //   ' set position of QUOTES
         if (iPos1 > 0) // Then
         {
            sTmp2 = &gszTmpBuf[0];
            // sTmp2 = sStg
            strcpy(sTmp2, sStg);
            iLen2 = Len(sTmp2);
            do
            {
               sTmp = Left(sTmp2, iPos1); // ' get the part BEFORE the QUOTES
               iPos2 = InStr(sTmp, sFnd);
               if (iPos2 == 0)  // Then
               {
                  // ' no it is AFTER the beginning QUOTE
                  sTmp = Right(sTmp2, (iLen2 - iPos1));  //   ' get the remainder
                  iPos1 = InStr(sTmp, "\""); //   ' set position of QUOTE
                  if (iPos1 > 0) // Then
                  {
                     iLen = Len(sTmp);
                     sTmp2 = Left(sTmp, iPos1);
                     iLen2 = Len(sTmp2);
                     iPos2 = InStr(sTmp2, sFnd);
                     if (iPos2 == 0) // Then
                     {
                        // ' no it is NOT in the first set of QUOTES
                        sTmp2 = Right(sTmp, (iLen - iPos1));   //    ' get the remainder
                        iPos1 = InStr(sTmp2, "\"");   //   ' any more quotes
                        iLen2 = Len(sTmp2);
                        if (iPos1 == 0) // Then
                        {
                           // ' no more quotes = it is NOT within QUOTES
                           iLen2 = 0;
                        } // End If
                     }
                     else
                     {
                        iPos = 0;   //    ' it is inside QUOTES!
                        iLen2 = 0;  //   ' exit the loop
                     } // End If
                  }
                  else
                  {
                     // ' no second QUOTES
                     iLen2 = 0;  // this would sort of an ERROR in C/C++ code!!!
                  } // End If
               }
               else
               {
                  // ' it is BEFORE the QUOTES begin
                  iLen2 = 0;  // exit the loop returning the POSITION
               } // End If
            } while( iLen2 );
         } // End If
      } // End If
   } // End If
   return iPos;      // myInStrF = iPos
} // End Function


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Left
// Return type: LPTSTR 
// Arguments  : LPTSTR lpl
//            : DWORD dwi
// Description: Return the LEFT prortion of a string
//              Emulates the Visual Basic function
///////////////////////////////////////////////////////////////////////////////
LPTSTR   Left( LPTSTR lpl, DWORD dwi )
{
   LPTSTR   lps = GetNxtBuf();
   DWORD    dwk;
   for( dwk = 0; dwk < dwi; dwk++ )
      lps[dwk] = lpl[dwk];
   lps[dwk] = 0;
   return lps;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Right
// Return type: LPTSTR 
// Arguments  : LPTSTR lpl
//            : DWORD dwl
// Description: Returns a buffer containing the RIGHT postion of a string
//              Emulates the Visual Basic function.
///////////////////////////////////////////////////////////////////////////////
LPTSTR   Right( LPTSTR lpl, DWORD dwl )
{
   LPTSTR   lps = GetNxtBuf();
   DWORD    dwk = strlen(lpl);
   DWORD    dwi;
   *lps = 0;
   if( ( dwl ) &&
      ( dwk ) &&
      ( dwl <= dwk ) )
   {
      if( dwl == dwk )  // is it right ALL
         dwi = 0;
      else
         dwi = dwk - dwl;
      strcpy(lps, &lpl[dwi] );
   }

   return lps;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Mid
// Return type: LPTSTR 
// Arguments  : LPTSTR lpl
//            : DWORD dwb
//            : DWORD dwl
// Description: Returns a buffer containing the MIDDLE portion of a string.
//              Emulates the Visual Basic function.
///////////////////////////////////////////////////////////////////////////////
LPTSTR   Mid( LPTSTR lpl, DWORD dwb, DWORD dwl )
{
   LPTSTR   lps = GetNxtBuf();
   DWORD    dwk = strlen(lpl);
   DWORD    dwi, dwr;
   *lps = 0;
   if( ( dwl ) && 
      ( dwb ) &&
      ( dwl ) &&
      ( dwb <= dwk ) &&
      ( dwl <= (dwk - (dwb - 1)) ) )
   {
      dwr = 0;
      for(dwi = (dwb - 1); (dwi < dwk), (dwr < dwl); dwi++ )
      {
         lps[dwr++] = lpl[dwi];
      }
      lps[dwr] = 0;
   }
   return lps;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsInclude
// Return type: BOOL 
// Argument   : LPTSTR sStg
// Description: Specialised search for #include in a C/C++ line
//              Returns TRUE if it is an #include "." or
// #include <Z> line.
///////////////////////////////////////////////////////////////////////////////
BOOL  IsInclude( LPTSTR sStg )
{
   BOOL  bIs = FALSE;
   INT   iLen, iPos;
   TCHAR ch;
   LPTSTR   pPos;

   iLen = strlen(sStg);
   if( iLen > 11 )   // Then                   ' MINIMUM is "#include <a>" = 12
   {
      if( sStg[0] == '#' )
      {
         // iPos = InStr(sStg, "#")
         // If iPos = 1 Then
         for( iPos = 1; iPos < iLen; iPos++ )
         {
            // sStg = Right(sStg, (iLen - 1))
            // sStg = MaxTrim(sStg)
            if( sStg[iPos] > ' ' )
               break;
         }
         pPos = &sStg[iPos];
         iLen = strlen( pPos );
         if( iLen > 10 )   // Then
         {
            iPos = InStr(pPos, "include");
            if( iPos == 1 )   //Then
            {
               // ch = Mid(sStg, 8, 1)
               ch = pPos[7];
               //     iVal = Asc(ch)
               if( ch <= ' ' )
               {
                  // If (iVal <= 32) Then
                  bIs = True;
               }  //     End If
            } //    End If
         } //  End If
      } //  End If
   } // End If
   // IsInclude = bIs
   //End Function
   return bIs;
}

BOOL  IsPragma( LPTSTR sStg )
{
   BOOL     bIs = FALSE;
   INT      iLen, iPos;
   LPTSTR   pPos = sStg;

   iLen = strlen(pPos);
   if( iLen > 8 )   // Then                   ' MINIMUM is "#pragma 1" = 9
   {
      if( pPos[0] == '#' )
      {
         pPos++;
         while( *pPos <= ' ' )
            pPos++;
         iLen = strlen( pPos );
         if( iLen > 7 )   // Then
         {
            iPos = InStrWhole(pPos, "pragma");
            if( iPos == 1 )   //Then
               bIs = True;
         } //  End If
      } //  End If
   } // End If
   return bIs; // IsPragma = bIs
}  // End Function


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsDefine
// Return type: BOOL 
// Argument   : LPTSTR sStg
// Description: Specialised search for #define in a C/C++ line of code.
//              Returns TRUE if found.
///////////////////////////////////////////////////////////////////////////////
BOOL  IsDefine( LPTSTR sStg )
{
   BOOL  bIs = FALSE;
   INT   iLen, iPos;
   TCHAR ch;
//   INT   iVal;

   iLen = strlen(sStg);
   if( iLen > 8 ) // Then               ' MINIMUM is "#define a" = 9
   {
      iPos = InStr(sStg, "#");
      if( iPos == 1 )   //Then
      {
         sStg++;
         while( (*sStg != 0) && (*sStg <= ' ') )
            sStg++;
         // sStg = Right(sStg, (iLen - 1))
         // sStg = MaxTrim(sStg)
         iLen = strlen(sStg);
         if( iLen > 9 ) //Then
         {
            iPos = InStr(sStg, "define");
            if( iPos == 1 ) // Then
            {
               ch = sStg[6];
               // iVal = Asc(ch)
               // If (iVal <= 32) Then
               if( ch <= ' ' )
                  bIs = True;
               // End If
            } //  End If
         } // End If
      } // End If
   } // End If
   // IsDefine = bIs
   // End Function
   return bIs;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsAnIf
// Return type: BOOL 
// Argument   : LPTSTR sStg
// Description: Specialised search for an #if, #else, #ifdef, #ifndef, #endif
//              in a line of C\C++ code.
// Added #undef a, and change to returning in INT to denote the type
///////////////////////////////////////////////////////////////////////////////
INT  IsAnIf( LPTSTR sStg )
{
   //'DESCRIPTION: Check if this line is an "if..." line
   //'NOTE: Line must have been conditioned by my CondLine (was Tabs2Space) function
   INT   bIs = 0;
   INT   iLen, iPos;
   LPTSTR   lpb;

   iLen = strlen(sStg);
   if( iLen > 4 ) // Then                       ' MINIMUM is "#if a" or "#else" = 5
   {
      //' we have the length for a little "if a"
      //iPos = InStr(sStg, "#")
      if( sStg[0] == '#' )
      {
         iPos = 1;
      // If (iPos = 1) Then
         //   sStg = Right(sStg, (iLen - 1))  ' get string without "#" character
         //   sStg = MaxTrim(sStg)            ' and ensure NO leading stuff
         for( ; iPos < iLen; iPos++ )
         {
            // sStg = Right(sStg, (iLen - 1))
            // sStg = MaxTrim(sStg)
            if( sStg[iPos] > ' ' )
               break;
         }
         lpb = &sStg[iPos];
         iLen = strlen( lpb ); //     ' get its lenght
         if( iLen > 3 ) // Then              ' minimum is "if a" or "else" = 4
         {
            // ' we have the length for an "if a" or an "else"
            iPos = InStr(lpb, "if ");
            if (iPos == 1) // Then
               bIs = is_if;    // signal an if
            else
            {
               iPos = InStr(lpb, "else");
               if (iPos == 1) // Then
                  bIs = is_else;
               else if (iLen > 4) // Then
               {
                  // ' we have the length for an "endif"
                  iPos = InStr(lpb, "endif");
                  if (iPos == 1) // Then
                     bIs = is_endif;
                  else if (iLen > 7) // Then      ' minimum is "ifdef a" = 7
                  {
                     // ' we have the length for an "ifdef a" or "undef a"
                     iPos = InStr(lpb, "ifdef ");
                     if (iPos == 1) // Then
                     {
                        bIs = is_ifdef;
                     }
                     else
                     {
                        iPos = InStr(lpb, "undef ");
                        if( iPos == 1 )
                           bIs = is_undef;
                        else if (iLen > 8) //Then  ' minimum is "ifndef a" = 8
                        {
                           //' we have the length for an "ifndef a"
                           iPos = InStr(lpb, "ifndef ");
                           if (iPos == 1) // Then
                               bIs = is_ifndef;
                           // End If          ' search for #ifndef
                        } // End If           ' search for #undef
                     } // End If              ' search for #ifdef
                  } // End If                  ' search for #endif
               } // End If                      ' search for #else
            } // End If                          ' search for #if
         } // End If                              ' still has minimum "if a" or "else" = 4
      } // End If                                  ' begin with the important "#" character
   } // End If                                      ' MINIMUM length of 5 = "#if a" or "#else"
   // IsAnIf = bIs
   // End Function
   return bIs;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : CondLine (was Tabs2Space)
// Return type: INT 
// Argument   : LPTSTR lpb
// Description: Specialised TRIMMING function.
//              1. All TAB characters are converted to A SPACE
//              2. All multiple spaces are reduce to ONE SPACE
//              3. Leading and Trailing characters <= " " are REMOVED
// *** AND ***  4. ALL QUOTED " " strings are REMOVED and replaced with "Z"!!!
// Return the resultant length of the string now in the buffer.
// NOTE: The buffer is MODIFIED.
///////////////////////////////////////////////////////////////////////////////
INT  CondLine( LPTSTR lpb )
{
   INT   k = 0;
   INT   iLen = strlen(lpb);
   INT   i, c;
   BOOL  bSp = FALSE;
#ifdef   SKIPDQUOTES
   BOOL  bQ = FALSE;
#endif   // SKIPDQUOTES
   if( iLen )
   {
      k = 0;
      for( i = 0; i < iLen; i++ )
      {
         c = lpb[i];
#ifdef   SKIPDQUOTES
         if( bQ )    // if had the first QUOTE
         {
            if( c == '"' )
            {
               bQ = FALSE;
               lpb[k++] = (TCHAR)'Z';  // add a single 'Z' to replace DOUBLE quotes
               lpb[k++] = (TCHAR)c;
               bSp = FALSE;
            }
         }
         else
#endif   // SKIPDQUOTES
         if( c > ' ' )
         {
            lpb[k++] = (TCHAR)c;
#ifdef   SKIPDQUOTES
            if( c == '"' )
               bQ = TRUE;
#endif   // SKIPDQUOTES
            bSp = FALSE;
         }
         else if( k )
         {
            if( !bSp )
            {
               lpb[k++] = ' ';
               bSp = TRUE;
            }
         }
      }
      lpb[k] = 0;
      i = k;
      while(i)
      {
         i--;
         if( lpb[i] > ' ' )
            break;
         lpb[i] = 0;    // kill this trailing spacey char
         k--;           // and REDUCE the returned length
      }
   }
   return k;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : MaxTrim
// Return type: INT 
// Argument   : LPTSTR lpb
// Description: Actually same as the above CondLine()
//              
///////////////////////////////////////////////////////////////////////////////
INT  MaxTrim( LPTSTR lpb )
{
   return( CondLine(lpb) );
}



///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Add2sBlk
// Return type: PBLK 
// Arguments  : PLE pHead
//            : LPTSTR lpl
// Description: Add a string to the given LINKED LIST
//              added the string length to the structures,
// and returning the pointer to the ALLOCATED buffer.
///////////////////////////////////////////////////////////////////////////////
PBLK  Add2sBlk( PLE pHead, LPTSTR lpl )
{
   PBLK  pb = 0;
   INT   i = 0;
   if(lpl)
      i = strlen(lpl);
   if(i)
   {
      pb = (PBLK)MALLOC( (sizeof(BLK) + i) );
      if(pb)
      {
         pb->b_Len = (DWORD)i;
         strcpy( &pb->b_Stg[0], lpl );
         InsertTailList(pHead, (PLE)pb);
      }
   }
   return pb;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : stLen
// Return type: DWORD 
// Argument   : PLE pHead
// Description: Specialised calculation of the current lenght of all the
//              string in the given LINKED LIST.
///////////////////////////////////////////////////////////////////////////////
DWORD stLen( PLE pHead )
{
   PLE   pNext;
   DWORD dwk = 0;
   PBLK  pb;
   Traverse_List( pHead, pNext )
   {
      pb = (PBLK)pNext;
      dwk += pb->b_Len;
   }
   return dwk;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Get1Line
// Return type: DWORD 
// Arguments  : PLE pHead
//            : LPTSTR lpl
//            : DWORD dwmax
// Description: Convert the strings in the given LINKED LIST to one continuous
//              line in the buffer given.
// Returns the LENGTH of the line.
// NOTE: If given a MAXIMUM lenght of ZERO, or if the length will overrun
// the maximum given, the function RETURNS the REQUIRED LENGTH.
///////////////////////////////////////////////////////////////////////////////
DWORD    Get1Line( PLE pHead, LPTSTR lpl, DWORD dwmax )
{
   PLE      pNext;
   LPTSTR   lps;
   DWORD    dwl = 0;
   PBLK     pb;
   Traverse_List( pHead, pNext )
   {
      pb = (PBLK)pNext;
      lps = &pb->b_Stg[0];
      if( (dwl + pb->b_Len) > dwmax )
      {
         dwl = dwmax;
         break;
      }         
      strcat(lpl, lps);
      dwl = strlen(lpl);
   }
   if( dwl == dwmax )
   {
      dwl = 0;
      Traverse_List( pHead, pNext )
      {
         pb = (PBLK)pNext;
         dwl += pb->b_Len;
      }
   }
   if( bDebug )   // special -f:d to show more
   {
      if( dwl > gdwMaxLine )  // DEBUG function to see largest FUNCTION (-f)
         gdwMaxLine = dwl;
   }
   return dwl;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : GetFunc
// Return type: LPTSTR 
// Argument   : PLE pHead
// Description: Specialised function to examine the collected C/C++ strings
//              and determine if it represents a FUNCTION with BODY.
///////////////////////////////////////////////////////////////////////////////
INT   GetFunc( PLE pHead, LPTSTR lpl, DWORD dwmax )
{
   DWORD    dwl;
   INT      iLen;

   *lpl = 0;
   dwl = Get1Line( pHead, lpl, dwmax );
   if( dwl && ( dwl < dwmax ) )
   {
      LPTSTR   sStg;
      INT      iPos1, iPos2, iPos3, iPos4;
      LPTSTR   sHead, sBody;
      // sFu = ""
      // sBld = ""
      sStg = lpl;
      iLen = strlen(sStg);
      iPos1 = InStr(sStg, "(");
      iPos2 = InStr(sStg, ")");
      iPos3 = InStr(sStg, sszBBr);  // seek "{"
      iPos4 = InStr(sStg, sszEBr);  // seek "}"
      if( (iLen > 0) &&
         (iPos4 > 0) && 
         (iPos3 > 0) &&
         (iPos2 > 0) &&
         (iPos1 > 2) &&
         (iPos1 < iPos2) &&
         (iPos2 < iPos3) &&
         (iPos3 < iPos4) ) // Then
      {
         sHead = Left(sStg, (iPos1 - 1));
         sBody = Mid(sStg, iPos1, (iLen - ((iPos1 - 1) + (iLen - iPos2))));
         if( MaxTrim(sHead) && MaxTrim(sBody) )
         {
            // sFu = Trim(sHead) & Trim(sBody)
            //'sFu = Trim(sFu)
            strcpy(lpl, sHead);
            strcat(lpl, sBody);
         } // End If
         else
            *lpl = 0;
      } //  End If
      else
         *lpl = 0;
      // GetFunc = sFu
      iLen = strlen(lpl);
   }  // End Function
   else
   {
      iLen = dwl;
   }
   return iLen;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : GetALine
// Return type: DWORD 
// Arguments  : LPTSTR lpl
//            : DWORD dwmax
//            : LPTSTR lpb
//            : DWORD dws
//            : PDWORD pdwi
// Description: Extract a LINE of C/C++ code from the BUFFER (lpb) up to the
//              END of the buffer size (dws) beginning at the given
// OFFSET (*pdwi) up to the MAXIMUM lenght (dwmax) returning the data in
// the passed buffer (lpl), returning the LENGTH fetched (dwl).
// NOTE: dwl == dwmax sould be considered an ERROR, but it is "overlooked"
// at the moment!!! ONE DAY SOON!!
///////////////////////////////////////////////////////////////////////////////
DWORD GetALine( LPTSTR lpl, DWORD dwmax, LPTSTR lpb, DWORD dws, PDWORD pdwi )
{
   DWORD    dwk = 0;
   DWORD    dwi = *pdwi;
   INT      c;

   for( ; dwi < dws; dwi++ )
   {
      // WordBasic.EndOfLine (1)
      c = lpb[dwi];
      if( NewLnChr(c) )
      {
         if( ( c == '\r' ) && ( ( dwi + 1 ) < dws ) )
         {
            c = lpb[dwi+1];
            if( NewLnChr(c) )
               dwi++;
         }
         dwi++;
         break;
      }
      lpl[dwk++] = (TCHAR)c;
      //if( dwk >= DMAXLINE )
      if( dwk >= dwmax )
         break;
   }

   lpl[dwk] = 0;
   *pdwi = dwi;
   return dwk;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsStruct
// Return type: BOOL 
// Argument   : LPTSTR sStg
// Description: Check if the line contains the word "struct" and if NOT
//              followed by an ALPHA-NUMERIC character.
///////////////////////////////////////////////////////////////////////////////
BOOL  IsStruct(LPTSTR sStg)
{
   BOOL  bIs = False;
   INT   iLen, iPos;
//   INT   ch;

   iLen = strlen(sStg);
   if( iLen > 6 ) // Then                   ' MINIMUM is "struct " or "struct{" = 7
   {
      iPos = InStrWhole(sStg, "struct");
      //if( (iPos > 0) && (strcmp(Mid(sStg, (iPos + 6), 1), " ") == 0 ))  // Then
      if( iPos > 0 )
      {
//         ch = sStg[iPos + 5];    // extract the 7th character after "struct"
//         if( !IsAlphaNumeric(ch) )   // this is already done in InStrWhole()!
         {
            bIs = True; // it is what we seek
         }
      } //  End If
   } // End If
   return bIs;   // IsStruct = bIs
}  // End Function


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : InStrCnt
// Return type: INT 
// Arguments  : LPTSTR sStg
//            : LPTSTR sFnd
// Description: Specialised COUNTER of a particular string in a line of
//              C/C++ code.
// NOTE the stronger InStrCntF() that will EXCLUDE finds within DOUBLE QUOTES.
// But note the function CondLine() will replace ALL double quotes strings
// with "Z", so this can be used AFTER CondLine()!
///////////////////////////////////////////////////////////////////////////////
INT   InStrCnt(LPTSTR sStg, LPTSTR sFnd)
{
   INT   iCnt, iLen, iPos, iLen2;
   iCnt = 0;
   iLen = strlen(sStg);
   iLen2 = strlen(sFnd);
   if( (iLen > 0) && (iLen2 > 0) ) // Then
   {
      iPos = InStr(sStg, sFnd);
      if (iPos > 0) // Then
      {
         do //  ' we have at least one
         {
            iCnt = iCnt + 1;
            sStg = Right(sStg, (iLen - (iPos + (iLen2 - 1))));
            iLen = Len(sStg);
            iPos = InStr(sStg, sFnd);
         } while( iPos > 0 );
      } //End If
   } // End If
   // InStrCnt = iCnt
   return iCnt;
}  // End Function


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : InStrCntF
// Return type: INT 
// Arguments  : LPTSTR sStg
//            : LPTSTR sFnd
// Description: Specialised COUNTER of a particular string in a line of
//              C/C++ code that will EXCLUDE finds within DOUBLE QUOTES.
///////////////////////////////////////////////////////////////////////////////
INT   InStrCntF(LPTSTR sStg, LPTSTR sFnd)
{
   INT   iCnt, iLen, iPos, iLen2;
   iCnt = 0;
   iLen = strlen(sStg);
   iLen2 = strlen(sFnd);
   if( (iLen > 0) && (iLen2 > 0) ) // Then
   {
      iPos = myInStrF(sStg, sFnd);
      if (iPos > 0) // Then
      {
         do //  ' we have at least one
         {
            iCnt = iCnt + 1;
            sStg = Right(sStg, (iLen - (iPos + (iLen2 - 1))));
            iLen = Len(sStg);
            iPos = myInStrF(sStg, sFnd);
         } while( iPos > 0 );
      } //End If
   } // End If
   return iCnt;   // InStrCntF = iCnt
}  // End Function

INT   OpenCnt(LPTSTR sStg)
{
   INT   iCnt = InStrCnt(sStg, sszBBr);   // seek "{"
   return iCnt;
}

INT   CloseCnt(LPTSTR sStg)
{
   INT   iCnt = InStrCnt(sStg, sszEBr);   // seek "}"
   return iCnt;
}

INT   OpenCntF(LPTSTR sStg)
{
   INT   iCnt = InStrCntF(sStg, sszBBr);  // seek "{"
   return iCnt;
}

INT   CloseCntF(LPTSTR sStg)
{
   INT   iCnt = InStrCntF(sStg, sszEBr);  // seek "}"
   return iCnt;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : LastInStr
// Return type: INT 
// Arguments  : LPTSTR sStg
//            : LPTSTR sFnd
// Description: Specialise function to return the NUMERIC position of the
//              LAST instance of a string within a line.
///////////////////////////////////////////////////////////////////////////////
INT   LastInStr(LPTSTR sStg, LPTSTR sFnd)
{
   INT   iPos, iLen1, iLen2, iPos2;
   LPTSTR   sTmp;
   iPos = 0;
   iLen1 = Len(sStg);
   iLen2 = Len(sFnd);
   if( (iLen1 > 0) && (iLen2 > 0) && (iLen1 >= iLen2) ) // Then
   {
      iPos = InStr(sStg, sFnd);
      if (iPos > 0) // Then
      {
         sTmp = Right(sStg, (iLen1 - iPos)); //  ' get remainder
         iPos2 = InStr(sTmp, sFnd);
         if (iPos2 > 0) // Then
         {
            do
            {
               iLen2 = Len(sTmp);
               iPos = iPos + iPos2;
               sTmp = Right(sTmp, (iLen2 - iPos2));   //  ' get remainder
               iPos2 = InStr(sTmp, sFnd);
            } while( iPos2 > 0 );
         } // End If
      } // End If
   } // End If
   return iPos;   // LastInStr = iPos
} // End Function


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : LastInStrF
// Return type: INT 
// Arguments  : LPTSTR sStg
//            : LPTSTR sFnd
// Description: Specialise function to return the NUMERIC position of the
//              LAST instance of a string within a line,
// AND will EXCLUDE finds made within DOUBLE QUOTES
///////////////////////////////////////////////////////////////////////////////
INT   LastInStrF(LPTSTR sStg, LPTSTR sFnd)
{
   INT   iPos, iLen1, iLen2, iPos2;
   LPTSTR   sTmp;
   iPos = 0;
   iLen1 = Len(sStg);
   iLen2 = Len(sFnd);
   if( (iLen1 > 0) && (iLen2 > 0) && (iLen1 >= iLen2) ) // Then
   {
      iPos = myInStrF(sStg, sFnd);
      if (iPos > 0) // Then
      {
         sTmp = Right(sStg, (iLen1 - iPos)); //  ' get remainder
         iPos2 = myInStrF(sTmp, sFnd);
         if (iPos2 > 0) // Then
         {
            do
            {
               iLen2 = Len(sTmp);
               iPos = iPos + iPos2;
               sTmp = Right(sTmp, (iLen2 - iPos2));   //  ' get remainder
               iPos2 = myInStrF(sTmp, sFnd);
            } while( iPos2 > 0 );
         } // End If
      } // End If
   } // End If
   return iPos;   // LastInStr = iPos
} // End Function


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsEndStruct
// Return type: BOOL 
// Argument   : LPTSTR sStg
// Description: Specialised check if we have reached the END of a "struct"
//              definition.
///////////////////////////////////////////////////////////////////////////////
BOOL  IsEndStruct(LPTSTR sStg)
{
   BOOL  bIs = False;
   INT   iLen;
   INT   iPos1, iPos2, iPos3;
   LPTSTR   sTmp;
   if( IsStruct(sStg) ) // Then
   {
      iLen = strlen(sStg);
      iPos1 = OpenCnt(sStg) - CloseCnt(sStg);
      if (iPos1 == 0) // Then
      {
         iPos2 = LastInStr(sStg, sszEBr);    // seek "}"
         if (iPos2 > 0) // Then
         {
            sTmp = Right(sStg, (iLen - iPos2));
            iPos3 = InStr(sTmp, ";");
            if (iPos3 > 0)  //Then
            {
               bIs = True;
            } // End If
         } // End If
      } // End If
   } // End If
   // IsEndStruct = bIs
   return bIs;
} // End Function


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : StripComments
// Return type: INT 
// Argument   : LPTSTR sStg
// Description: Specialised "comment" stripper. Naturally ALL following a
//              "//" will be discarded, and IFF "/*" and "*/" found the
// block of comment will be removed.
// BUFFER given will be MODIFIED
///////////////////////////////////////////////////////////////////////////////
INT   StripComments(LPTSTR sStg)
{
//   INT   iRet = 0;
   INT   iLen, iPos, iPos1, iPos2;
   LPTSTR   sLf, sRt;

   iLen = Len(sStg);
   if (iLen > 0) // Then
   {
      if (InStr(sStg, "/*")) // Then
      {
         // ' search for both the beginning and if found, then the ending
         sLf = 0;
         sRt = 0;
         iPos = myInStrF(sStg, "//");
         iPos1 = myInStrF(sStg, "/*");
         if( (iPos > 0) && ((iPos1 == 0) || (iPos < iPos1)) ) // Then
         {
            //sStg = Trim(Left(sStg, (iPos - 1)))
            sLf = Left(sStg, (iPos - 1));
            MaxTrim(sLf);
            //iLen = Len(sStg)     ' refresh any remainder length
            strcpy(sStg,sLf);
            iLen = Len(sStg);  //     ' refresh any remainder length
            if (iPos1 > 0) // Then
            {
               iPos1 = myInStrF(sStg, "/*");
            } // End If
         } // End If

         if (iPos1 > 0) // Then
         {
            //sLf = Trim(Left(sStg, (iPos1 - 1)))
            sLf = Left(sStg, (iPos1 - 1));
            MaxTrim(sLf);
            iPos2 = myInStrF(sStg, "*/");
            if (iPos2 > 0) // Then
            {
               // ' if we have the close right here
               // sRt = Trim(Right(sStg, (iLen - (iPos2 + 1))))
               // sRet = sLf & sRt    ' return this without comments
               sRt = Right(sStg, (iLen - (iPos2 + 1)));
               strcpy(sStg, sLf);
               strcat(sStg, sRt);
            } // End If
         } // End If
      }  // ElseIf InStr(sStg, "//") Then
      else if( InStr( sStg, "//" ) )
      {
         iPos = myInStrF(sStg, "//");
         if (iPos > 0) // Then
         {
            // sRet = Trim(Left(sStg, (iPos - 1)))
            sLf = Left(sStg, (iPos - 1));
            MaxTrim(sLf);
            strcpy(sStg, sLf);
         } // End If
      } // End If
   }  // End If
   // StripComments = sRet
   iLen = Len(sStg);
   return iLen;
} // End Function

#ifdef   ADDDO2
BOOL  IsDo(LPTSTR sStg)
{
   BOOL  bIs = False;
   INT   iLen, iPos;
   LPTSTR   sTmp;
   iLen = strlen(sStg);
   if( iLen > 1 ) // Then                   ' MINIMUM is "do" = 2
   {
      iPos = InStr(sStg, "do");
      if( iPos > 0 )
      {
         sTmp = Mid(sStg, (iPos + 1), 1);
         if( ( *sTmp == ' ' ) ||
            ( *sTmp == 0 ) )
         {
            bIs = True;
         }
      } //  End If
   } // End If
   return bIs;
}  // End Function

BOOL  IsEndDo(LPTSTR sStg)
{
   BOOL  bIs = False;
   INT   iLen;
   INT   iPos1, iPos2, iPos3;
   LPTSTR   sTmp;
   if( IsDo(sStg) ) // Then
   {
      iLen = strlen(sStg);
      iPos1 = OpenCnt(sStg) - CloseCnt(sStg);
      if (iPos1 == 0) // Then
      {
         iPos2 = LastInStr(sStg, sszEBr);    // seek "}"
         if (iPos2 > 0) // Then
         {
            sTmp = Right(sStg, (iLen - iPos2));
            iPos3 = InStr(sTmp, ";");
            if (iPos3 > 0)  //Then
            {
               bIs = True;
            } // End If
         } // End If
      } // End If
   } // End If
   // IsEndStruct = bIs
   return bIs;
} // End Function

#endif   // #ifdef   ADDDO2


BOOL  IsArrayDef( LPTSTR lpl )
{
   BOOL  bIs = False;
   INT   iLen = strlen(lpl);
   INT   iPos1, iPos2, iPos3;
   if( iLen > 8 )    // a b[]=""; is minimum of 9
   {
      if( ( (iPos1 = InStr(lpl, sszBSB)) > 0 ) &&
          ( (iPos2 = InStr(lpl, sszESB)) > 0 ) &&
          ( (iPos3 = InStr(lpl, "="   )) > 0 ) &&
          ( !InStr(lpl, "=="  )              ) &&
          ( !InStr(lpl, ">="  )              ) &&
          ( !InStr(lpl, "<="  )              ) &&
          ( !InStr(lpl, "!="  )              ) &&
          (  iPos1 < iPos2                   ) &&
          (  iPos2 < iPos3                   ) )
      {
         bIs = True;
      }
   }
   return bIs;
}

BOOL  IsArrayEnd( LPTSTR lpl )
{
   BOOL  bIs = False;
   INT   iLen = strlen(lpl);
   INT   iPos1, iPos2, iPos3;
   LPTSTR   pTmp;

   if( iLen > 8 )    // a b[]=""; is minimum of 9
   {
      if( ( (iPos1 = InStr(lpl, sszBSB)) > 0 ) &&
          ( (iPos2 = InStr(lpl, sszESB)) > 0 ) &&
          ( (iPos3 = InStr(lpl, "="   )) > 0 ) &&
          ( !InStr(lpl, "=="  )              ) &&
          ( !InStr(lpl, ">="  )              ) &&
          ( !InStr(lpl, "<="  )              ) &&
          ( !InStr(lpl, "!="  )              ) &&
          (  iPos1 < iPos2                   ) &&
          (  iPos2 < iPos3                   ) )
      {
         // we have the START of something
         // now to find the END of it!
         if( InStr(lpl, sszBBr) )   // if we have a OPEN BRACE that is '{'
         {
            if( OpenCnt(lpl) == CloseCnt(lpl) )
            {
               // this is a good sign
               iPos1 = LastInStr(lpl, sszEBr);  // get LAST closing brace position
               if(iPos1)
               {
                  pTmp = Right(lpl, (iLen - iPos1));  // get the last ending bit
                  iPos2 = InStr(pTmp,";");
                  if(iPos2)
                     bIs = True;
               }
            }
         }
         else  // NO open brace
         {
            pTmp = Right(lpl, (iLen - iPos3));
            iPos2 = InStr(pTmp,";");
            if(iPos2)
               bIs = True;
         }
      }
   }
   return bIs;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Out_Funcs2
// Return type: BOOL 
// Arguments  : LPTSTR lpb
//            : DWORD dws
//            : LPTSTR lpf
// Description: Scan the given BUFFER, lpb, of LENGTH dws for C/C++
//              FUNCTIONS.
///////////////////////////////////////////////////////////////////////////////
BOOL  Out_Funcs2( LPTSTR lpb, DWORD dws, LPTSTR lpf )
{
   BOOL        bRet = FALSE;
   INT         iErr = ERR_NONE;
   DWORD       dwi, dwk;
   INT         iLin, iLen, iLen2;
   LPTSTR      sMsg = &gszMsgBuf[0];   // [1024]
   LPTSTR      lpl = &gszLineB1[0];    // [DMAXLINE]
   LPTSTR      lpt = &gszLineB2[0];    // [DMAXLINE]
   BOOL        bInCom = FALSE;
   INT         iPos, iPos2;
   LIST_ENTRY  sBlk;
   LIST_ENTRY  sFun;
   LIST_ENTRY  sHds, sStr;
   INT         iBr, iBra;
   BOOL        bBr, bBra;
   BOOL        bAdded, bBlank;
   LPTSTR      sLf;
   DWORD       dwout;
   INT         iHadData, iBgnBlk;

   if( gpLineBuf1 )
      lpl = gpLineBuf1;
   if( gpLineBuf2 )
      lpt = gpLineBuf2;

   // WordBasic.StartOfDocument
   // WordBasic.StartOfLine
   dwi = 0;
   iLin = 0;
   InitLList( &sBlk );
   InitLList( &sFun );
   InitLList( &sHds );
   InitLList( &sStr );
   iBr = 0;
   bBr = FALSE;
   iBra = 0;
   bBra = FALSE;
   gAdded = 0;
   gDiscard = 0;
   gdwLine2 = 0;
   gdwLines = 0;
   dwout = 0;
   iHadData = iBgnBlk = 0;
   gdwMaxLine = 0;   // DEBUG function to see largest FUNCTION (must have -f:d)
   do
   {
//      iBgnPos = dwi;
      dwk = 0;
      bAdded = False;   // nothing added yet to functions
      bBlank = False;   // not a BLANK line yet
      dwk = GetALine( lpl, gdwMaxLn, lpb, dws, &dwi );
//      strcpy(lpl2, lpl);   // keep original
      iLin++;
      //  iBgnPos = WordBasic.GetSelStartPos()
      //  iEndPos = WordBasic.GetSelEndPos()
//      iEndPos = dwi;    // note = begin of NEXT line
      if( dwk )   // && (iBgnPos < iEndPos) )
      {
         // sLine = WordBasic.[GetText$](iBgnPos, iEndPos)
         // 'sLn = MaxTrim(sLine)
         //  sLn = Tabs2Space(sLine)
         //   iLen = Len(sLn)
         //   If (iLen > 0) Then
         iLen = CondLine(lpl);
         iPos = 0;
         if( iLen > 1 )
         {
            if( bInCom )   // Then
            {
               // ' only searching for end of comment
               iPos2 = InStr(lpl, "*/");
               if( iPos2 ) // Then
               {
                  bInCom = False;   // coming OUT fo the COMMENT
                  if( (iPos2 + 1) < iLen )  // Then
                  {
                     sLf = Right(lpl, (iLen - (iPos2 + 1)));
                     iLen = MaxTrim(sLf);
                     strcpy(lpl,sLf);  // put any remainder back into line buffer
                  }
                  else
                  {
                     *lpl = 0;
                     iLen = 0;
                  }
               }
            }
            else
            {
               // search for the beginning, and if found also for the end
               //iPos1 = InStr(sLn, "/*")
               iPos = myInStrF(lpl, "/*");
               iPos2 = myInStrF(lpl, "//");
               if( (iPos2 > 0) &&
                  ( (iPos == 0) || (iPos2 < iPos) ) )
               {
                  // ' maybe nothing to do on this line
                  lpl[iPos2-1] = 0;     // drop the trailing comment
                  iLen = MaxTrim(lpl);
                  if(iPos)
                  iPos = myInStrF(lpl, "/*");
               }
               if (iPos > 0) //Then
               {
                  *lpt = 0;   // zero a temporary buffer
                  // sLf = Left(sLn, (iPos1 - 1))
                  sLf = Left(lpl, (iPos - 1));  // get the left
                  iLen2 = MaxTrim(sLf); 
                  if(iLen2)
                     strcat(lpt,sLf);
                  sLf = Right(lpl, (iLen - (iPos + 1)));
                  if( (iLen2 = MaxTrim(sLf)) > 1 )
                  {
                     iPos = myInStrF(sLf, "*/");
                     if (iPos > 0)  //Then
                     {
                        // ' if we have the close right here
                        sLf = Right(sLf, (iLen2 - (iPos + 1)));
                        iLen2 = MaxTrim(sLf);
                        if(iLen2)
                           strcat(lpt,sLf);
                        // sBlk = sBlk & sRt
                        bInCom = False;
                        strcpy(lpl,lpt);
                        iLen = strlen(lpl);
                     }
                     else
                     {
                        bInCom = True;
                        iLen = 0;      // no further processing of this line
                     }  //  End If
                  }
                  else
                  {
                        bInCom = True;
                        iLen = 0;      // no further processing of this line
                  }
               }
            }
         }
         if( !bInCom && ( iLen > 1 ) )// Then
         {
            // removed any trailing comment
            iPos = myInStrF(lpl, "//");
            if (iPos > 0) // Then
            {
               // ' maybe nothing to do on this line
               lpl[iPos-1] = 0;     // drop the trailing comment
               iLen = MaxTrim(lpl);
            }  // ElseIf (iLen > 1) Then
         } // End If

         // completed BOTH types of comments
         // ================================
         if( !bInCom && iLen )
         {
            //if( !IsInclude(lpl) && !IsAnIf(lpl) )
            if( IsPragma(lpl) )
            {
               iLen = 0;
            }
            else if( IsInclude(lpl) )
            {
               // an #include counts as a BLANK line like separator
               bBlank = True;
               iLen = 0;
            }
            else if( ( iPos = IsAnIf(lpl) ) > 0 )
            {
               // just thrown away, except if it was say an #undef, but how to know
               //if( iPos == is_undef )
               //   bBlank = True;
               iLen = 0;
            }
            else // if( !IsInclude(lpl) && !IsAnIf(lpl) )
            {
               if( IsDefine(lpl) )  // Then
               {
                  while( (iLen > 0) && (lpl[iLen - 1] == '\\') && (dwi < dws) )
                  {
                     // ch = Mid(sLn, iLen, 1)
                     //   Do While ((iLen > 0) And (ch = "\"))
                     //   WordBasic.StartOfLine   ' unselect line ready to go down one
                     dwk = GetALine( lpl, gdwMaxLn, lpb, dws, &dwi );
                     iLin++;
                     if( dwk )
                     {
                        // If (WordBasic.LineDown(1)) Then
                        //      WordBasic.EndOfLine (1)
                        //        iBgnPos = WordBasic.GetSelStartPos()
                        //        iEndPos = WordBasic.GetSelEndPos()
                        //        If (iBgnPos < iEndPos) Then
                        //            sLine = WordBasic.[GetText$](iBgnPos, iEndPos)
                        //            'sLn = MaxTrim(sLine)
                        iLen = CondLine(lpl);
                        //       iLen = Len(sLn)
                        //            If iLen Then
                        //                ch = Mid(sLn, iLen, 1)
                        //            End If
                        //        End If
                     }
                     else if( dwi < dws )
                     {
                        iLen = 0;
                     }
                     else
                     {
                        strcpy(sMsg, "WARNING: On line ");
                        sprintf(EndBuf(sMsg), "%d", iLin);
                        strcat(sMsg, " ran out of file before"MEOR
                           "the end of the #define!"MEOR );
                        strcat(sMsg, "Can only exit function with ERROR!");
                        if(VERB1)
                        {
                           sprtf( "%s"MEOR, sMsg );
                        }
                        //        WordBasic.LineUp (1)
                        //        sLn = ""
                        if( bDebug )   // Then
                        {
#ifdef USE_COMP_FIO
                            if (!VERB1)
                                sprtf("%s"MEOR, sMsg);
#else
                           iPos = MessageBox(NULL,sMsg, "DEFINE PROBLEM", // !USE_COMP_FIO
                              (MB_OK | MB_ICONINFORMATION) );
#endif
                        }
                        iErr = ERR_DEFINE;
                     }  //  End If
                  }  // Loop while a #define continuation marker
                  //      sLn = ""
                  //      iLen = 0
                  //  End If
                  *lpl = 0;
                  iLen = 0;   // strlen(lpl);
                  // an #define counts as a BLANK line like separator
                  bBlank = True;
                  //iPos = 0;
               }  // end if IsDefine()
               else if( IsStruct(lpl) )
               {
                  strcpy(lpt, lpl); // copy to temporary
                  if( !IsEndStruct(lpt) )
                  {
                     do
                     {
                        dwk = GetALine( lpl, gdwMaxLn, lpb, dws, &dwi );
                        iLin++;
                        iLen = CondLine(lpl); // ' condition the extracted line of code
                        iLen = StripComments(lpl);
                        // sLf = sLf & sLn
                        if( ( strlen(lpt) + iLen ) < gdwMaxLn )
                        {
                           strcat(lpt, lpl);
                           // sLf = StripComments(sLf)
                           iLen = StripComments(lpt);
                           //iCnt2 = iCnt2 + 1
                        } // End If
                        else
                        {
                           // another RECOMPILE error
                           strcpy(sMsg, "ERROR: BUFFER OVERRUN"MEOR);
                           sprintf(EndBuf(sMsg), "Need to re-run with -f:DMAXLINE=nnnn "
                              "currently %d increased to more than %d!"MEOR,
                              gdwMaxLn, iLen );
                           strcat(sMsg, "Can only exit function with ERROR!");
                           if(VERB1)
                           {
                               sprtf( "%s"MEOR, sMsg );
                           }
                           if( bDebug )   // Then
                           {
#ifdef USE_COMP_FIO
                               if (VERB1) {
                                   // have already logged a messge
                               }
                               else {
                                   sprtf("%s"MEOR, sMsg);
                               }
#else
                               iPos = MessageBox(NULL,sMsg, "LENGTH PROBLEM", // !USE_COMP_FIO
                                       (MB_OK | MB_ICONINFORMATION) );
#endif
                           }
                           iLen = 0;
                           iErr = ERR_MAXLN;   // abort with this problem
                           break;
                        }
                     } while( !IsEndStruct(lpt) );
                  }
                  if( iErr == ERR_NONE )
                  {
                     if( !Add2sBlk( &sStr, lpt ) )
                     {
                        iErr = ERR_MEMORY;
                     }
                  }

                  *lpl = 0;
                  iLen = 0;
                  // a struct counts as a BLANK line like separator
                  bBlank = True;
               }  // end if IsStruct()
#ifdef   ADDDO2
               else if( IsDo(lpl) )
               {
                  strcpy(lpt, lpl); // copy to temporary
                  while( !IsEndDo(lpt) )
                  {
                     dwk = GetALine( lpl, gdwMaxLn, lpb, dws, &dwi );
                     iLin++;
                     iLen = CondLine(lpl); // ' condition the extracted line of code
                     iLen = StripComments(lpl);
                     strcat(lpt, lpl);
                     iLen = StripComments(lpt);
                  }
                  *lpl = 0;
                  iLen = 0;
               }
#endif   // ADDDO2
               else if( IsArrayDef(lpl) )
               {
                  strcpy(lpt, lpl);
                  while( !IsArrayEnd(lpt) )
                  {
                     dwk = GetALine( lpl, gdwMaxLn, lpb, dws, &dwi );
                     iLin++;
                     iLen = CondLine(lpl); // ' condition the extracted line of code
                     iLen = StripComments(lpl);
                     strcat(lpt, lpl);
                     iLen = StripComments(lpt);
                     if( ( dwi >= dws ) && !IsArrayEnd(lpt) )
                     {
                        iErr = ERR_ARRAY;   // ran out of data while filling in an array
                     }
                  }
                  *lpl = 0;
                  iLen = 0;
               }

               // have checked for #include, an #if type, #define, struct
               // =======================================================
               if( iLen )
               {
                  // not #include, an #if type nor a #define
                  // and we still have length
                  {
                     iHadData++;    // we have "something"
                     // NOTE: This ONLY works because the earlier CondLine()
                     // function REPLACED all text in DOUBLE QUOTES with a
                     // simple "Z" thus will NOT encounter say strcpy(lpb,"{}")
                     // An alternative would be to use the specialised
                     // myInStrF() function which EXCLUDES finds within quotes!

                     // seek "{"
                     sLf = lpl;
                     while( ((iLen2 = strlen(sLf)) > 0) &&
                        ( (iPos = InStr(sLf,sszBBr)) > 0 ) )
                     {
                        iBr++;
                        bBr = True;
                        sLf+= iPos;
                     }

                     // seek "}"
                     sLf = lpl;
                     while( ((iLen2 = strlen(sLf)) > 0) &&
                        ( (iPos = InStr(sLf,sszEBr)) > 0 ) )
                     {
                        iBr--;
                        sLf+= iPos;
                     }

                     // seek "("
                     sLf = lpl;
                     while( ((iLen2 = strlen(sLf)) > 0) &&
                        ( (iPos = InStr(sLf,sszBBr2)) > 0 ) )
                     {
                        iBra++;
                        bBra = True;
                        sLf+= iPos;
                     }

                     // seek ")"
                     sLf = lpl;
                     while( ((iLen2 = strlen(sLf)) > 0) &&
                        ( (iPos = InStr(sLf,sszEBr2)) > 0 ) )
                     {
                        iBra--;
                        sLf+= iPos;
                     }



                     iPos = InStr(lpl, ";");
                     if (iPos < iLen) //Then
                     {
                         if( Add2sBlk( &sBlk, lpl ) )
                            bAdded = True;
                         else
                            iErr = ERR_MEMORY;
                     }
                  }

               }  // end with length
            }  // end if #include or #if
         }
         //else if( !bInCom )
         //{
         //   bBlank = True;
         //} //  End If

         if( bAdded )
         {
            iLen = stLen( &sBlk );
            if( (bBr) && (iBr == 0) && (iLen > 5) && !bInCom &&
               ( iErr == ERR_NONE ) ) //Then
            {
               // ' potential function!
               //    If (Not bInCom) And (Not bErr) Then
               iLen = GetFunc( &sBlk, lpl, gdwMaxLn );
               if( (iLen > 0) && !((DWORD)iLen > gdwMaxLn) )// Then
               {
                  // ' we have a new function to add
                  // sLf = sFunc1
                  // If (iLenb > iMax) Then
                  // iMax = iLenb
                  // End If
                  iPos = InStr(lpl, "(");
                  sLf = 0;
                  if (iPos > 0) // Then
                  {
                     sLf = Left(lpl, (iPos - 1));
                     iLen = MaxTrim(sLf);
                     iPos = InStr(sLf, " ");
                     while ((iPos > 0) && (iLen > 0))
                     {
                        sLf = Right(sLf, (iLen - iPos)); //  ' right excl. space
                        iLen = MaxTrim(sLf);
                        iPos = InStr(sLf, " ");
                     } // Loop
                  } // End If
                  if( sLf )
                     Add2sBlk( &sHds, sLf );
                  Add2sBlk( &sFun, lpl );
                  //         sBlk = ""
                  KillLList( &sBlk );
                  //     End If
                  // End If
               } // End If
               else if( (DWORD)iLen > gdwMaxLn )
               {
                  strcpy(sMsg, "ERROR: BUFFER OVERRUN"MEOR);
                  sprintf(EndBuf(sMsg), "Need to re-run with -f:DMAXLINE "
                     "currently %d increased to more than %d!"MEOR,
                     gdwMaxLn, iLen );
                  strcat(sMsg, "Can only exit function with ERROR!");
                  if(VERB1)
                  {
                      sprtf( "%s"MEOR, sMsg );
                  }
                  if( bDebug )   // Then
                  {
#ifdef USE_COMP_FIO
                      if (!VERB1)
                          sprtf("%s"MEOR, sMsg);
#else
                     iPos = MessageBox(NULL,sMsg, "LENGTH PROBLEM", // !USE_COMP_FIO
                              (MB_OK | MB_ICONINFORMATION) );
#endif
                  }
                  iLen = 0;
                  iErr = ERR_MAXLN;   // abort with this problem
               }
            } // End If
         }
         if( iBr == 0 )  // Then
         {
            bBr = False;
         } // End If
      }
      else
      {
         bBlank = True;
      }

      // try to clean out the collected items
      if( bBlank )
      {
         // a BLANK line, or a "blank" like area such as an #include or a #define
         if( !bInCom && (iErr == ERR_NONE) )
         {
            // ' this line has NO length, thus is a break
            if (iBr == 0) // Then
            {
               if (stLen( &sBlk ) > 0 ) // Then
               {
                  strcpy(sMsg, "WARNING: On line ");
                  sprintf(EndBuf(sMsg), "%d", iLin);
                  strcat(sMsg, " found a break, thus suggest");
                  strcat(sMsg, vbCrLf);
                  strcat(sMsg, "discarding the following:");
                  strcat(sMsg, vbCrLf);
                  strcat(sMsg, "[");
                  *lpl = 0;
                  Get1Line( &sBlk, lpl, gdwMaxLn );
                  strcat(sMsg, lpl);
                  strcat(sMsg, "]"vbCrLf );
                  strcat(sMsg, "Proceed with discard?");
                  if( VERB5 )
                  {
                     sprtf( "%s"MEOR, sMsg );
                  }
                  // iLenb = MsgBox(sMsg, (vbQuestion Or vbYesNo), "DEBUG QUERY")
                  if( bDebug )   // Then
                  {
#ifdef USE_COMP_FIO
                      if (!VERB5)
                          sprtf("%s"MEOR, sMsg);
#else
                     iPos = MessageBox(NULL,sMsg, "DISCARD QUESTION", // !USE_COMP_FIO
                        (MB_YESNOCANCEL | MB_ICONQUESTION) );
                        // MB_YESNO | MB_ICONQUESTION) );
#endif
                  }
                  else
                  {
                     iPos = IDYES;
                  }
                  //          sBlk = ""
                  if( iPos == IDYES ) // Then
                  {
                     KillLList( &sBlk );
                  }
                  else if( iPos == IDCANCEL )
                  {
                     iErr = ERR_CANCEL;
                  }
                  //      End If
                  //  End If
               }  // End If
            }
         } //  End If
      }

   //     WordBasic.StartOfLine   ' unselect line ready to go down one
      if( iLin == 78 )   // Then
      {
         sLf = &lpb[dwi];  // where are we
      } //  End If

   // Loop While (WordBasic.LineDown(1))
      // ****** continue until end of file buffer ******
   } while( ( dwi < dws ) && ( iErr == ERR_NONE ) );
   // done whole buffer

   ListCount2( &sFun,&iPos);

   dwout = 0;
   gdwError = (DWORD)iErr;    // set global error marker

   if( iPos && (iErr == ERR_NONE) )
   {
      // output the list of functions
      if( gdwFunOut )   // if we have SOME flags
         gdwLines = OutFunList( &sFun, iPos, &sHds, lpf, &dwout, &sStr );

      bRet = True;   // but this is SUCCESS anyway

   }

   gdwLine2 = iLin;
   gDiscard = dws - dwout;

   KillLList( &sBlk );
   KillLList( &sFun );
   KillLList( &sHds );
   KillLList( &sStr );

   if( bDebug && VERB5 )
   {
      sprtf( "NOTE: Largest function line was %d bytes of MAXIMUM %d."MEOR,
         gdwMaxLine, // DEBUG function to see largest FUNCTION (must have -f:d)
         gdwMaxLn );
   }

   return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : ShowFHelp
// Return type: VOID 
// Argument   : VOID
// Description: Show the list of -f:???? functions.
//              Note only the first character is decoded.
// Also some HELP is ONLY output if -v9 preceeds the -f:? on the command line.
// but later decided show all all the time ...
///////////////////////////////////////////////////////////////////////////////
VOID  ShowFHelp( VOID )
{
   static BOOL _s_DnFHlp = False;
   if( !_s_DnFHlp )
   {
      _s_DnFHlp = True;
      Do_VerMsg();
      sprtf("HELP ON THE -f[...] OPTIONS"MEOR);
      sprtf( "Function List Extraction (-f) has following options:"MEOR );
      sprtf( "-f:All = Output both summary and full list of functions."MEOR );
      sprtf( "-f:A+ = Output ALL the lists collected."MEOR );
      sprtf( "-f:Full = Output only the full list of functions."MEOR );
      sprtf( "-f:Summary = Output only the summary list of functions."MEOR );
      sprtf( "-f:Local = Add output of a local list of functions. (Must be after F or S)"MEOR );
      sprtf( "-f:Global = Add output of a global list of (extern) functions. (after F or S)"MEOR );
      sprtf( "-f:M[nnnn][+|-] to set/unset the bBeTidy and change the length.(def=9)"MEOR );
      sprtf( "Note: Only the first letter of A, F, L, S, etc. need be given."MEOR );
      sprtf( "Also the last -f option will override any previous -f."MEOR );
      sprtf( "-f alone will default to -f:All."MEOR );
      //if( VERB9 )
      {
         //sprtf( "-v9 followed by -f:? adds this output and clears the output flag."MEOR );
         sprtf( "-f:D[+|-] to turn on|off some additional DEBUG output."MEOR );
         sprtf( "-f:T to add output structures defined in module."MEOR );
         sprtf( "-f:0 to CLEAR all output of functions."MEOR );
         sprtf( "-f:BFIXFUN[+|-] to set/unset fix function spacing.(def=T)"MEOR );
         sprtf( "-f:BSPACEFUN[+|-] to set/unset space in above fix function.(def=F)"MEOR );
         sprtf( "-f:DMAXLINE=nnnn[K] to establish a new line buffer size. (def=4096)"MEOR );
      }
      Do_Happy_Msg();
      Pgm_Exit(0);
   }
}

INT  All2Upper( LPTSTR lps )
{
   INT   i, iLen;
   TCHAR c;

   iLen = strlen(lps);
   for( i = 0; i < iLen; i++ )
   {
      c = (TCHAR)toupper(lps[i]);
      lps[i] = c;
   }
   return iLen;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : GetListType
// Return type: BOOL 
// Argument   : LPTSTR lpc
// Description: Handle the various COMMAND LINE inputs beginning with -f:????
//              Note -f:? will output the above HELP once, and then kill all
// other function output, perhaps except WARNINGS and ERRORS.
// toupper(
///////////////////////////////////////////////////////////////////////////////
BOOL   GetListType( LPTSTR lpcmd )
{
   BOOL     bRet = True;
   DWORD    dwl = strlen(lpcmd);
   LPTSTR   lpc = &gszTmpBuf[0];   // = [1024]
   INT      iPos;
   INT      c;

   if(dwl)
   {
      if( *lpcmd == ':' )
      {
         lpcmd++;
         strcpy(lpc,lpcmd);
         All2Upper(lpc);
         //c = toupper(*lpc);
         c = *lpc;
         switch(c)
         {
         case '0':
            gdwFunOut = 0;
            break;

         case 'A':
            gdwFunOut |= (ifo_Full | ifo_Short);
            lpc++;
            iPos = InStr(lpc, "+");
            if(iPos)
               gdwFunOut |= ifo_All;
            break;
         case 'B':
            iPos = InStrWhole(lpc, "BFIXFUN");
            if(iPos)
            {
               iPos = InStr(lpc, "-");
               if(iPos)
                  bFixFun = False;
               else
                  bFixFun = True;
            }
            else
            {
               iPos = InStrWhole(lpc, "BSPACEFUN");
               if(iPos)
               {
                  iPos = InStr(lpc, "-");
                  if(iPos)
                     bSpaceFun = False;
                  else
                     bSpaceFun = True;
               }
               else
               {
                  bRet = False;
               }
            }
            break;

         case 'D':
            lpc++;
            if( *lpc == 'M' )
            {
               iPos = InStrWhole(lpc, "MAXLINE");
               if( (iPos == 1) && (lpc[7] == '=') && (IsNumeric(lpc[8])) )
               {
                  lpc += 8;   // bump to END
                  dwl = 0;
                  while( IsNumeric(*lpc) )
                  {
                     dwl = (dwl * 10) + (*lpc - '0');
                     lpc++;
                  }
                  if( *lpc == 'K' )
                     dwl *= 1024;
                  if( gpLineBuf1 )
                     MFREE(gpLineBuf1);
                  if( gpLineBuf2 )
                     MFREE(gpLineBuf2);
                  gpLineBuf1 = 0;
                  gpLineBuf2 = 0;
                  if( dwl > gdwMaxLn )
                  {
                     gdwMaxLn = dwl;
                     gpLineBuf1 = MALLOC(dwl);
                     gpLineBuf2 = MALLOC(dwl);
                     if( !gpLineBuf1 || !gpLineBuf2 )
                        bRet = FALSE;
                  }
               }
               else
               {
                  bRet = FALSE;
               }
            }
            else
            {
               // assumed to be DEBUG
               iPos = InStr(lpc, "-");
               if(iPos)
                  bDebug = False;
               else
                  bDebug = True;
            }
            break;
         case 'F':
            gdwFunOut = ifo_Full;
            break;
         case 'G':
            gdwFunOut |= ifo_Global;
            break;
         case 'L':
            gdwFunOut |= ifo_Local;
            break;
         case 'M':
            lpc++;
            iPos = InStr(lpc, "-");
            if(iPos)
               bBeTidy = False;
            else
               bBeTidy = True;
            iPos = 0;
            dwl = 0;
            while( *lpc )
            {
               if( IsNumeric( *lpc ) )
               {
                  if(iPos)
                     dwl = (dwl * 10) + (*lpc - '0');
                  else
                     dwl = (*lpc - '0');
                  iPos++;
               }
               lpc++;
            }
            if( iPos )
            {
               // had some numerics
               if(dwl)
                  giMinTLen = dwl;
               else
                  bRet = False;  // signal an error
            }
            break;
         case 'S':
            gdwFunOut = ifo_Short;
            break;
         case 'T':
            gdwFunOut |= ifo_Struct;
            break;
         case '?':
            ShowFHelp();
            gdwFunOut = 0;
            break;
         default:
            bRet = False;
         }
      }
      else
      {
         bRet = False;
      }
   }
   else
   {
      // just -f = default
      gdwFunOut = ifo_Def;    // set to DEFAULT
   }
   if( !bRet )
      ShowFHelp();

   return bRet;
}

DWORD GetMaxLn( VOID )
{
   return( gdwMaxLn );
}

#endif   // #if   AddFunc
// FUNCTION LIST EXTRACTION
// eof - FixFObj.c
