

// FixFunc.c

#include	"FixF32.h"		// all incusive include

extern   BOOL  bDebug;   // special -f:d for DEBUG
extern   DWORD gdwMaxLn;   //  = DMAXLINE;      // can be changed with -f:DMAXLINE=nnnn
extern   INT   gdwFunOut;  // = ifo_Def;

extern   TCHAR sszBBr[];   //  = "{";
extern   TCHAR sszEBr[];   //  = "}";
extern   TCHAR sszBBr2[];  // = "(";
extern   TCHAR sszEBr2[];  // = ")";
extern   TCHAR sszBSB[];   //  = "[";
extern   TCHAR sszESB[];   //  = "]";

#define  BL(a)    ( a == -1 )
#define  VL(a)    ( a && ( !BL(a) ) )

/* comment */
INT GetCLine( LPTSTR lpl, INT dwmax, LPTSTR lpb, INT dws, PINT pdwi, PINT piLin )
{
   INT      dwk = 0;
   INT      dwi = *pdwi;
   INT      c, d;
   BOOL     bSp = TRUE;
   BOOL     bInCom = FALSE;
   BOOL     bInQ = FALSE;
   INT      iLin;
   LPTSTR   ptmp;

   d = 0;
   for( ; dwi < dws; dwi++ )
   {
      // WordBasic.EndOfLine (1)
      ptmp = &lpb[dwi];
      c = *ptmp;
      if( bInCom )
      {
         // wait only for CLOSE of comment
         if( c == '*' )
         {
            dwi++;
            c = lpb[dwi];
            if( c == 0x0d )
            {
                if( lpb[dwi+1] == 0x0a )
                   dwi++;
                iLin = *piLin;
                iLin++;
                *piLin = iLin;
            }
            else if( c == '/' )
               bInCom = FALSE;

         }
      }
      else if( bInQ )
      {
         // watch out for ESCAPED quotes WITHIN quotes!!!
         if( ( c == '"' ) && ( d != '\\' ) )
         {
            lpl[dwk++] = 'Z';
            lpl[dwk++] = (TCHAR)c;
            bInQ = FALSE;
         }
      }
      else
      {
         if( c > ' ' )
         {
            if( ( c == '/' ) && ( lpb[dwi+1] == '*' ) )
            {
               bInCom = TRUE;
               dwi++;
            }
            else if( ( c == '/' ) && ( lpb[dwi+1] == '/' ) )
            {
               dwi += 2;
               for( ; dwi < dws; dwi++ )
               {
                  c = lpb[dwi];
                  if( c == 0x0d )
                  {
                     if( lpb[dwi+1] == 0x0a )
                        dwi++;
                     break;
                  }
               }
               if(dwk)
               {
                  dwi++;
                  break;
               }
            }
            else
            {
               lpl[dwk++] = (TCHAR)c;
               bSp = FALSE;
               if( c == '"' )
                  bInQ = TRUE;
            }
         }
         else
         {
            if( c == 0x0d )
            {
                c = lpb[dwi+1];
                if( c == 0x0a )
                     dwi++;
               if( d == '\\' )   // if last sig char was 'C/C++ continuation, then
               {
                  iLin = *piLin;
                  iLin++;
                  *piLin = iLin;
               }
               else
               {
                  dwi++;
                  break;
               }
            }
            if( !bSp )
            {
               lpl[dwk++] = ' ';
               bSp = TRUE;
            }
         }
      }

      if( c > ' ' )
         d = c;

      //if( dwk >= DMAXLINE )
      if( dwk >= dwmax )
      {
         dwk = (INT)-1;
         break;
      }
   }

   if( !BL(dwk) )
   {
      INT   i = dwk;
      lpl[i] = 0;
      while(i)
      {
         i--;
         if( lpl[i] > ' ' )
            break;
         lpl[i] = 0;
         dwk--;
      }
   }
   *pdwi = dwi;

   if( dwk && VERB5 )
   {
      // show the 'C' line collected, and compressed. ie "strings" = "Z"
      sprtf( "Returning [%s]"MEOR, lpl );
      // ===============================================================
   }
   return dwk;

}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Out_Funcs
// Return type: BOOL 
// Arguments  : LPTSTR lpb
//            : DWORD dws
//            : LPTSTR lpf
// Description: Scan the given BUFFER, lpb, of LENGTH dws for C/C++
//              FUNCTIONS.
///////////////////////////////////////////////////////////////////////////////
BOOL  Out_Funcs( LPTSTR lpb, DWORD dwLen, LPTSTR lpf )
{
   BOOL        bRet = FALSE;
   INT         iErr = ERR_NONE;
   INT         dwi, dws;
//   DWORD       dwk;
   INT         iLin, iLen, iLen2;
   LPTSTR      sMsg = &gszMsgBuf[0];   // [1024]
   LPTSTR      lpl = &gszLineB1[0];    // [DMAXLINE]
   LPTSTR      lpt = &gszLineB2[0];    // [DMAXLINE]
   BOOL        bInCom = FALSE;
   INT         iPos;
//   INT         iPos2;
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
   dws = dwLen;
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
//      dwk = 0;
      bAdded = False;   // nothing added yet to functions
      bBlank = False;   // not a BLANK line yet
      iLen = GetCLine( lpl, gdwMaxLn, lpb, dws, &dwi, &iLin );
//      strcpy(lpl2, lpl);   // keep original
      iLin++;
      //  iBgnPos = WordBasic.GetSelStartPos()
      //  iEndPos = WordBasic.GetSelEndPos()
//      iEndPos = dwi;    // note = begin of NEXT line
      if( VL(iLen) )   // && (iBgnPos < iEndPos) )
      {
         if( iLen != (INT)strlen(lpl) )
            chkme("ERROR: Returned %d. Should be %d!!!"MEOR, iLen, strlen(lpl) );

         // sLine = WordBasic.[GetText$](iBgnPos, iEndPos)
         // 'sLn = MaxTrim(sLine)
         //  sLn = Tabs2Space(sLine)
         //   iLen = Len(sLn)
         //   If (iLen > 0) Then
         //iLen = CondLine(lpl);
         iPos = 0;
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
                  while( ( VL(iLen) ) && (lpl[iLen - 1] == '\\') && (dwi < dws) )
                  {
                     // ch = Mid(sLn, iLen, 1)
                     //   Do While ((iLen > 0) And (ch = "\"))
                     //   WordBasic.StartOfLine   ' unselect line ready to go down one
                     iLen = GetCLine( lpl, gdwMaxLn, lpb, dws, &dwi, &iLin );
                     iLin++;
                     if( !VL(iLen) )
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
                           iPos = MessageBox(NULL,sMsg, "DEFINE PROBLEM",
                              (MB_OK | MB_ICONINFORMATION) );
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
                        iLen = GetCLine( lpl, gdwMaxLn, lpb, dws, &dwi, &iLin );
                        iLin++;
                        //iLen = CondLine(lpl); // ' condition the extracted line of code
                        //iLen = StripComments(lpl);
                        // sLf = sLf & sLn
                        // 0 is ok in structures
                        if( ( !BL(iLen) ) && ( (strlen(lpt) + iLen) < gdwMaxLn ) )
                        {
                           strcat(lpt, lpl);
                        } // End If
                        else     // if( !VL(iLen) )
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
                              iPos = MessageBox(NULL,sMsg, "LENGTH PROBLEM",
                                       (MB_OK | MB_ICONINFORMATION) );
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
                     iLen = GetCLine( lpl, gdwMaxLn, lpb, dws, &dwi, &iLin );
                     iLin++;
                     //iLen = CondLine(lpl); // ' condition the extracted line of code
                     //iLen = StripComments(lpl);
                     if( !BL(iLen) )
                     {
                        strcat(lpt, lpl);
                     }
                     else
                     {
                           iErr = ERR_MAXLN;   // abort with this problem
                           break;
                     }
                     //iLen = StripComments(lpt);
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
                     iLen = GetCLine( lpl, gdwMaxLn, lpb, dws, &dwi, &iLin );
                     iLin++;
                     //iLen = CondLine(lpl); // ' condition the extracted line of code
                     //iLen = StripComments(lpl);
                     if( !BL(iLen) )
                     {
                        strcat(lpt, lpl);
                     }
                     else
                     //iLen = StripComments(lpt);
                     //if( ( dwi >= dws ) && !IsArrayEnd(lpt) )
                     {
                        iErr = ERR_ARRAY;   // ran out of data while filling in an array
                        break;
                     }
                  }
                  *lpl = 0;
                  iLen = 0;
               }

               // have checked for #include, an #if type, #define, struct
               // =======================================================
               if( VL(iLen) )
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
                  if( InStr(lpl, "InStrWhole") )
                     sLf = 0;
                  if( InStr(lpl, "myInStrF") )
                     sLf = 0;
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
                  if( bDebug )
                     sprtf( "Added [%s]"MEOR, lpl );

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
                     iPos = MessageBox(NULL,sMsg, "LENGTH PROBLEM",
                              (MB_OK | MB_ICONINFORMATION) );
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
                     iPos = MessageBox(NULL,sMsg, "DISCARD QUESTION",
                        (MB_YESNOCANCEL | MB_ICONQUESTION) );
                        // MB_YESNO | MB_ICONQUESTION) );
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
      if( iLin == 697 )   // Then
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


// eof - FixFunc.c
