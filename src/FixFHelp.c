
// FixFHelp.c
#include "FixF32.h"
// #include "FixFHelp.h"
extern DWORD g_dwWrap;  // approx = 68
extern LPTSTR   GetExQuotes( LPTSTR cp );
extern BOOL Get_H_Cmd( LPTSTR lpc );
extern VOID Give_H_Help(VOID);

extern   BOOL  bExclMLComm;   // state of comment exclude
#ifdef  ADD_WX_CMD     // add -wx[d|f]
extern   LIST_ENTRY  sXcludeD;   // = { &sXcludeD, &sXcludeD };
extern   LIST_ENTRY  sXcludeF;   // = { &sXcludeF, &sXcludeF };
#endif   // #ifdef  ADD_WX_CMD     // add -wx[d|f]

#if	AddDIR
//	Cmp	Al,'W'	;  = ' -w Convert DIR listing file.'> ;
//    case 'W':
extern BOOL  Set_W_Cmd( LPTSTR lpc );
#endif   //		; AddDIR

// FIX20051205 - add delete hi-bit characters - only ASCII output
//	dmsg(	" -dh[s]     Delete hi-bit characters -dhs substitute a space."MEOR );
int   g_DelHiBit = 0;
int   g_SubSpace = 0;
BOOL  gbNoEOF = FALSE;

// -hf:break:r:<font color="#0000FF">break</font>
// "-hf:break:r:<font color=\"#0000FF\">break</font>"
TCHAR Ver_Msg[] = "\t"CoLogo" - "PgmPurp" - "VerDate;

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Do_VerNum
// Return type: VOID 
// Argument   : VOID
// Description: Ouput the current version of the application
//              See FixFVers.h for numbers.
///////////////////////////////////////////////////////////////////////////////
VOID  Do_VerNum( VOID )
{
//; FIX981229 - Add version number after date
//; =========================================
   sprtf( " v.%d.%d.%d"MEOR, VersNum1, VersNum2, VersNum3 );
}

VOID  Do_VerMsg(VOID)
{
   sprtf( Ver_Msg );
	Do_VerNum();   //; FIX981229 - Add version number after this date
}

#define  dmsg  sprtf

VOID  Do_Happy_Msg(VOID)
{
	dmsg(	"                                            Happy fixing ..." );
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Give_Help
// Return type: VOID 
// Argument   : INT iRet
// Description: Show the HELP messages,
//              and exit the process.
///////////////////////////////////////////////////////////////////////////////
VOID  Give_Help( INT iRet )
{
   Do_VerMsg();

	dmsg(	"Useage:     FixF32 [@]InputFileName [Switches]"MEOR );
	dmsg(	"Purpose:    Various 'fixes' like ensure Cr/Lf, and wrap at %d, etc."MEOR, MXLINE );
	dmsg(	"Switches:   Must be preceeded by \"-\" or \"/\". Blank or -? = this help..."MEOR );

	dmsg(	" -b[-|+]    Suppress Backspaced duplications"MEOR );
	dmsg(	" -B[-|+]    Remove UTF-8 BOM from file"MEOR );

#ifdef ADD_TXT2VC // FIX20050203 - add -c - output C/C++ text form
   dmsg( " -c[name]   Output as single C/C++ text form."MEOR );
#endif // #ifdef ADD_TXT2VC // FIX20050203 - add -c - output C/C++ text form
	dmsg(	" -d[s[nn]]  Delete dup. Cr/Lf & -ds[nn] Delete begin line spaces [nn] chars"MEOR );
   // FIX20051205 - add delete hi-bit characters - only ASCII output
	dmsg(	" -dh[s]     Delete hi-bit characters -dhs substitute a space."MEOR );
#if   AddFunc
   dmsg( " -f         'C' Function list extraction. -f:? for help on options)"MEOR );
#endif   // AddFunc
//#if	RemHTML
//	like, dmsg(	" -h[2a]     Delete HTML data from file. (h2a=Show HEX as ASCII)"MEOR );
   Give_H_Help();
//#endif   //	; RemHTML
#ifdef  ADDJOBS1   // -j 22 Jan. 2004
// Switch -j[defstwo]: Dump as job file.
   dmsg( " -j[defstwo?] Process xml job file. d=delete e=email f=fix s=spc w=68 see-j?"MEOR );
#endif // #ifdef  ADDJOBS1   // -j 22 Jan. 2004
//#if	AddLS
	dmsg(	" -l[s][nnn] [s] Def. split at space ([-s] undo). [nnn] length."MEOR );
//#endif   // ; AddLS y/n
   dmsg( " -n         No addition of eof message ..."MEOR );
	dmsg(	" -o[-|+]FileName Output file. -=new +=append. Default is STANDARD OUT"MEOR );
#if	AddPara
	dmsg(	" -p         Write in paragraph form. ie Remove Cr/Lf ..."MEOR );
#endif   // ; AddPara
#if	AddRem
	dmsg(	" -r[text]   Remove text listed between [ & ]. -R is remove WHOLE LINES"MEOR );
#endif   // ; AddRem
#if	AddSplit
	dmsg(	" -s[nnn[K]] Split output file at nnn[K] size. (Def=250000)"MEOR );
#endif   // ; AddSplit
   // g_dwTab_Size
	dmsg(	" -t[s][nn]  Set Tab size to nn (Def.=3). s=convert to spaces"MEOR );
//	dmsg(	" -ts[nn]    Convert tabs to spaces of nn length"MEOR );
	dmsg(	" -u[d]      Unix format (No Cr) d = header from '\\' to '/'. Also sets -B & -n"MEOR );
	dmsg(	" -vnn       Toggle verbosity. v0=Off v1-9=On (Def=1)"MEOR );
#if	AddDIR
	dmsg(	" -w[af?..]  Convert DIR file to ordered list. (Options=AFTO ? for help)"MEOR  );
#endif   // ; AddDIR

//	dmsg ( " -??????    ?????? ?????????. ?????? ????? (Default=ON)"MEOR );
   dmsg( " -x         Exclude files like these from list. (Also -x@Input)"MEOR );
//; Then the NOTES ...
//	dmsg(	"Notes: -oFileName or -o+FileName to append if existing."MEOR );
//	dmsg(	"       and -o-FileName to force creation of new file."MEOR );
#if	AddRem
	dmsg(	"       Add ^J in front of \"text\" to only check begin of line,"MEOR );
   dmsg( "       and double inverted commas (\") must be used if spaces."MEOR  );
#endif   // ; AddRem

   Do_Happy_Msg();

	Pgm_Exit( iRet );
}

extern VOID  Show_W_Help( LPTSTR lpc );


VOID  Add2CmdBuf( LPTSTR lpc, INT j )
{
   if( (gnCurOut + j) < MXOUTB )
   {
      if(gnCurOut)
      {
         if( (gnCurLn + j) > MX1LINE )
         {
            strcat(gszOutBuf, MEOR);
            gnCurLn = 0;
         }
         else
            strcat(gszOutBuf," ");
      }
      strcat(gszOutBuf,lpc);
      gnCurOut += j;
      gnCurLn  += j;
   }
}

VOID  ShowCmd( VOID )
{
   INT   i, k, l;
   INT   j = gnCurOut;
   INT   c;
   if(j)
   {
      if( j > 65 )
      {
         LPTSTR   lpb = &gszGenBuf[0];
         l = k = 0;
         for( i = 0; i < j; i++ )
         {
            c = gszOutBuf[i];
            lpb[k++] = (TCHAR)c;
            if( k > 65 )
            {
               if( c <= ' ' )
               {
                  lpb[k] = 0;
                  if( l == 0 )
                     sprtf( "Command Line:[%s]"MEOR, lpb );
                  else
                     sprtf( "\t[%s]"MEOR, lpb );
                  l += k;
                  k = 0;
               }
            }
            else if( k > 75 )
            {
                  lpb[k] = 0;
                  if( l == 0 )
                     sprtf( "Command Line:[%s]"MEOR, lpb );
                  else
                     sprtf( "\t[%s]"MEOR, lpb );
                  l += k;
                  k = 0;
            }
         }
      }
      else
      {
         sprtf( "Command Line:[%s]."MEOR, gszOutBuf );
      }
   }
   else
   {
      sprtf( "Command Line: ERROR: No command line given!"MEOR );
   }
}

VOID  ShowCmd2( VOID )
{
   INT   i, argc;
   char * * argv;
   gszOutBuf[0] = 0;
   gnCurOut     = 0;
   gnCurLn      = 0;
   argc = gcArgs;
   argv = gpArgv;
   for( i = 1; i < argc; i++ )
      Add2CmdBuf( argv[i], strlen(argv[i]) );

   ShowCmd();
}
///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Show_Bad_Cmd
// Return type: VOID 
// Arguments  : LPTSTR lpc
//            : INT iRet
// Description: Command line error detected.
//              
///////////////////////////////////////////////////////////////////////////////
VOID  Show_Bad_Cmd( LPTSTR lpc, INT iRet )
{
   sprtf( "ERROR ON COMMAND LINE"MEOR
      "ERROR: Invalid command [%s]!"MEOR,
      lpc );

   ShowCmd2();

   //Give_Help( iRet );
	Pgm_Exit( iRet );
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Set_V_Cmd
// Return type: BOOL 
// Argument   : LPTSTR lps
// Description: Set VERBOSITY of output
//              from -v[nnn]. Can be 0 to 9 (but greater allowed)
///////////////////////////////////////////////////////////////////////////////
BOOL  Set_V_Cmd( LPTSTR lps )
{
   BOOL  bRet = FALSE;
   DWORD dwi = 0;
   INT   c;

   Pgm_Flag ^= VerbON;
   if( *lps )
   {
      c = *lps;
      while( c )
      {
         if( Chk_Num( c ) )
         {
            dwi = (dwi * 10) + (c - '0');
         }
         else
         {
            dwi = (DWORD)-1;
            break;
         }
         lps++;      // bump to next
         c = *lps;   // and get character
      }
      if( dwi == (DWORD)-1 )
      {
         // was NOT all numbers
         bRet = TRUE;
      }
      else
      {
         // set the verbosity level
         giVerbose = dwi;
         if(dwi)  // if > 0
            Pgm_Flag |= VerbON;  // set general flag
         else
            Pgm_Flag &= ~(VerbON);  // else reste general flag
      }
   }
   return bRet;
}



///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Get_OUTF
// Return type: BOOL 
// Argument   : LPTSTR lpf
// Description: 
//              
///////////////////////////////////////////////////////////////////////////////
BOOL  Get_OUTF( LPTSTR lpf )
{
   BOOL  bRet = FALSE;  // no problem yet
   BOOL  fApd = FALSE;  // use append
   DWORD dwc = 0;
   if( Pgm_Flag & Out_Fil )   // already using OUTPUT file
   {
      return TRUE;
   }
   if( *lpf == '+' )
   {
      fApd = TRUE;
      Pgm_Flag |= GotAppe; // Mark the appending
      lpf++;
   }
   else if( *lpf == '-' )
   {
      fApd = FALSE;
      Pgm_Flag &= ~(GotAppe); // UNMark the appending
      lpf++;
   }

   gszOFName[dwc] = 0;
   while( *lpf )
   {
      gszOFName[dwc++] = *lpf;
      lpf++;
   }

   if( dwc )
   {
      gszOFName[dwc] = 0;
      Pgm_Flag |= Out_Fil; // Show using OUTPUT file
   }
   else
   {
      bRet = TRUE;
   }
   return bRet;
}


BOOL  Get_LL( LPTSTR lps )
{
   BOOL  bRet = TRUE;   // assume ok
   DWORD dwc = 0;
   BOOL  bMinus = FALSE;
   INT   c;
   INT   NSign;
//	call	Get_Cmd_Chr		; Get char
//	Mov	Dx,0			; Start accumulation
//;	jz	Bad_LL			; If ends now = BAD
//	Jz	Set_LL			; If ends now LENGTH = 0 = Disabled
//	Cmp	Al,' '			; Is it  > 20H
//	Jbe	Get_LL			; Wait for > 20H

#if	AddLS    //			; Correct LINE SPLIT
   if( *lps == '-' )
   {
//	Cmp	Al,'-'			; --ls = Undo '-ls' DEFAULT ...
//	Jne	Chk_LLS
      bMinus = TRUE;
      lps++;
//	Call	Peek_Chr	; Peek at NEXT
//	Cmp	Al,'S'		; MUST be followed by an 'S'
      if( toupper( *lps ) == 'S' )
      {
         lps++;
//	Mov	Al,'-'		; Put this char BACK ...
//	Jne	Nxt_LL		; Else assume it is beginning of a number
//	call	Get_Cmd_Chr
         Pgm_Flag &= ~(Fix_LS);
      }
//	Jmp	Short Chk_LLn
//Chk_LLS:
   }
   else if( toupper( *lps ) == 'S' )
   {
//	Cmp	Al,'S'			; Do we have -ls ?
//	Jne	Nxt_LL
      Pgm_Flag |= Fix_LS;  // ONLY split at <=20h (else ({[ / * ;)
      lps++;
   }
//Chk_LLn:
//	call	Get_Cmd_Chr		; Get after 'S'
//	jz	Dn_LL
//	Cmp	Al,' '
//	ja	Nxt_LL
//Dn_LL:	Jmp	Chk_Cmd
#endif   //	; AddLS

//Nxt_LL:
   NSign = 0;  // Assume NO sign
   if( *lps == '-' )
   {
//	   Cmp	Al,'-'			; Accept NEGATIVE numbers
//	   Jne	Nxt_LL1			; Then only a number ...
	   NSign = 1;
//	call	Get_Cmd_Chr		; And get the NEXT CHAR ...
//Nxt_LL1:
   }
   c = *lps;
   if( ( c >= '0' ) && ( c <= '9' ) )
   {
      while( Chk_Num( c ) )
      {
//	Call	Chk_Num			; Check range
//	Jc	Bad_LL			; NOT A NUMBER 0-9
         dwc = ( dwc * 10 ) + ( c - '0' );
//	Call	Add_10			; Accumulate DECIMAL number
//	Jc	Bad_LL			; Overflow in size
//	call	Get_Cmd_Chr		; get next char
//	jz	Got_LL			; End of number
//	cmp	al,' '			; or <= 20H also END
//	ja	Nxt_LL1
         lps++;      // bump
         c = *lps;   // and get next
      }
      if( c == 0 ) // && ( dwc ) )
      {
         if( NSign )
         {
            if(dwc)
               dwc--;
            g_dwMaxLine = ~( dwc );
         }
         else
            g_dwMaxLine = dwc;
      }
      else
         goto Bad_Num;
   }
   else
   {
Bad_Num:
      bRet = FALSE;
   }
//Got_LL:
//	Or	Dx,Dx			; Check if ZERO?
//;	Jz	Bad_LL			; Can NOT accept ZERO MAX LINE LENGTH
//	Jz	Set_LL			; ZERO MAX = Disable length check
//	Cmp	NSign,0			; Is it a NEGATIVE number
//	Je	Set_LL
//	Dec	Dx			; Back 1
//	Not	Dx			; For a logical NOT ...
//Set_LL:
//	Mov	[MaxLine],Dx		; And set this MAX line 
//	Jmp	Chk_Cmd
//Bad_LL:
//	Jmp	Bad_Command
   return bRet;
}

BOOL  Get_Del_Flag( LPTSTR lpc )
{
   BOOL  bRet = TRUE;   // assume ok
   INT   c;
   DWORD dwc = 0;
//	Call	Get_Cmd_Chr		; Get NEXT character
//#if	AddDS
//	Jz	Got_Del_D
//	Cmp	Al,' '
//	Jbe	Got_Del_D
//	Cmp	Al,'S'			; Is it -ds
//	Je	Got_Del_DS
//	Jmp	Bad_Command		; NOT for us!!!
//	If	AddDS
   c = toupper( *lpc );
   if( c == 'S' ) {
//Got_Del_DS:
//	Push	Ax
//	Sub	Ax,Ax
//	Dec	Ax
      g_dwDel_CC = (DWORD)-1;  // Set to MAX
//	Pop	Ax
      Pgm_Flag |= Rem_DS;  // Remove initial SPACES
//	Call	Get_Cmd_Chr
//	Jz	Dn_Del_DS
//	Cmp	Al,' '
//	Jbe	Dn_Del_DS
      lpc++;
      c = *lpc;
      if( c == 0 )
      {
         // leave at MAXIMUM - that is remove ALL leading spaces
         // before checking dumplicate Cr/Lf, but should roll back
      }
      else if( Chk_Num( c ) ) // have been given an exact number to remove
      {
         while( Chk_Num( c ) )
         {
            dwc = (dwc * 10) + (c - '0');
            lpc++;
            c = *lpc;
//	Call	Chk_Num			; Is it a NUMBER
//	Jnc	Get_DS_Num
//Bad_Del_DS:
//	Jmp	Bad_Command
//Dn_Del_DS:
//	Jmp	Chk_Cmd
//Get_DS_Num:
//	Sub	ah,ah
//	Mov	Dx,Ax		; Set first number
//Nxt_DS_Num:
//	Call	Get_Cmd_Chr
//	Jz	End_DS_Num
//	Cmp	Al,' '
//	Jbe	End_DS_Num
//	Call	Chk_Num
//	Jc	Bad_Del_DS
//	Sub	Ah,Ah
//	Call	Add_10
//	Jnc	Nxt_DS_Num
//	Jmp	Bad_Del_DS
//End_DS_Num:
//	Or	Dx,Dx
//	Jz	Bad_Del_DS
//	Mov	[Del_CC],Dx
//	Jmp	Dn_Del_DS
         }
         if( c == 0 )
         {
            g_dwDel_CC = dwc;
         }
         else
            goto Bad_Num;
      }
      else
      {
Bad_Num:
         bRet = FALSE;
         return bRet;
      }
   } else if ( c == 'H' ) {  // FIX20051205 - add -dh[s] delete hi-bit characters
      // only ASCII output
      lpc++;
      c = toupper(*lpc);
      if( c == 'S' ) { // have -dhs
         g_SubSpace = 1;
      } else if (c) {
         return FALSE;
      }
      g_DelHiBit = 1;
   } else if ( c ) {
      return FALSE; // bad -d? argument
   } else {
//#endif   //	; AddDS
//Got_Del_D:	;	Just -d
//	Endif		; AddDS
	   Pgm_Flag |= Rem_CRLF;   // Add removal of dup Cr/Lf
//	Jmp	Chk_Cmd			; and added 4 Oct 94 - Wholly BLANK lns
   }
   return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Get_Tab_Siz
// Return type: BOOL 
// Argument   : LPTSTR lpc
// Description: Decode a -T TAB COMMAND
//              
///////////////////////////////////////////////////////////////////////////////
BOOL  Get_Tab_Siz( LPTSTR lpc )
{
   BOOL  bRet = TRUE;   // asumme ok
   INT   c;
   DWORD dwc = 0;
   if( toupper( *lpc ) == 'S' )
   {
//	Cmp	Al,'S'
//	Jne	Not_TS
      Pgm_Flag |= Tab_SP;  // Change TABS to spaces
      lpc++;
//	Call	Get_Cmd_Chr	; Get next character
//	Jz	Dn_TAB_Siz
//Not_TS:
//	Cmp	Al,' '
//	Jbe	Dn_TAB_Siz
   }
   c = *lpc;
   if( c )
   {
      if( Chk_Num(c) )
      {
         while( Chk_Num(c) )
         {
            dwc = (dwc * 10) + (c - '0');
            lpc++;
            c = *lpc;
         }
         if( ( c == 0 ) && ( dwc > 0 ) && ( dwc < 255 ) )
         {
//	Call	Chk_Num		; See if we have a number
//	Jc	Bad_TAB_Siz
//	Sub	Dx,Dx
//Get_TAB2:
//	Call	Add_10
//	Jc	Bad_TAB_Siz
//	Call	Get_Cmd_Chr
//	Jz	Gt_TAB_Siz
//	Cmp	Al,' '
//	Ja	Get_TAB2
//Gt_TAB_Siz:
//	Or	Dh,Dh
            g_dwTab_Size = dwc;
         }
         else
         {
            bRet = FALSE;
            goto Bad_TAB_Siz;
         }
//	Or	Dl,Dl
//	Jz	Bad_TAB_Siz
//	Mov	Tab_Size,Dl	; Set new tab size
//Dn_TAB_Siz:
//	Jmp	Chk_Cmd
      }
      else
      {
Bad_TAB_Siz:
         bRet = FALSE;
      }
//	Jmp	Bad_Command
   }

   return bRet;
}

#if	AddRem

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Get_Rem_Ln
// Return type: BOOL 
// Argument   : LPTSTR lpc == Get the data of a REMOVE LINE request
// Description: NOT YET COMPLETED
//              
//              case 'R':
//              case 'r':
///////////////////////////////////////////////////////////////////////////////
BOOL  Get_Rem_Ln( LPTSTR lpc, BOOL bLine )  // Get the data of a REMOVE LINE request
{
   BOOL  bRet = FALSE;  // assume FAILED
   DWORD dwl = strlen(lpc);
   DWORD dwm = 2;

   // also if from an input file there can be a space first
   while( (dwl) && (*lpc <= ' ') )
   {
      lpc++;
      dwl--;
   }
   // this ONLY happens if it is from a file I think
   if( ( dwl > 2 ) && (lpc[0] == '"') && (lpc[dwl-1] == '"') )
   {
      // it is encased in inverted commas - remove them
      dwl--;
      lpc[dwl] = 0;
      dwl--;
      lpc++;
   }
   if( ( dwl         ) &&
       ( dwl > dwm   ) &&
       ( *lpc == '[' ) )
   {
      LPTSTR   lpb = gszGenBuf;
      BOOL     bGIC = FALSE;
      lpc++;   // to next position
      dwl--;   // reduce length
      dwm--;   // and reduce minimum
      if( *lpc == '"' )    // if there is an inverted comma inside
      {
         bGIC = TRUE;
         lpc++;   // bump to next position
         dwl--;   // reduce lenght
         dwm++;   // must have a termination
      }
      if( dwl > dwm )
      {
         strcpy( lpb, lpc );  // keep beginning
         dwl--;
         if( lpb[dwl] == ']' )
         {
            lpb[dwl] = 0;
            if(dwl)
               dwl--;
         }
         dwm = dwl;
         if( bGIC )
         {
            if( lpb[dwl] == '"' )
            {
               lpb[dwl] = 0;
               if(dwl)
                  dwl--;
               dwm = dwl;
            }
            else
               dwm = 0;    // FAILED
         }
         if(dwm)
         {
            if( bLine ) { // is -R = remove whole LINE
               if( Add2RLList(lpb) )   // add this line to the REMOVE list
               {
                  Pgm_Flag |= Gt_Rem;  // flag we have SOME removals
                  bRet = TRUE;
               }
            } else {
               if( Add2rLList(lpb) )   // add this line to the TEXT REMOVE list
               {
                  g_iRemText = 1;  // flag we have SOME removals
                  bRet = TRUE;
               }
            }
         }
      }
   }
   return bRet;
}

#endif   //	; AddRem

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Get_Inp_Fil
// Return type: BOOL 
// Argument   : LPTSTR lpc
// Description: Got a @InputFile or a -@InputFile
//              Open and load the file into allocated memory, and
// scan the buffer for COMMANDS. Note a ";" denotes a COMMENT
//
///////////////////////////////////////////////////////////////////////////////
BOOL  Get_Inp_Fil( LPTSTR lpc )
{
   BOOL  bRet = FALSE;   // assume fail
   if( *lpc )
   {
      HANDLE   h = CreateFile( lpc, // file name
         GENERIC_READ,  // access mode
         FILE_SHARE_READ,  // share mode
         0, // SD
         OPEN_EXISTING, // how to create
         FILE_ATTRIBUTE_NORMAL,  // file attributes
         0 ); // handle to template file
      if( VFH(h) )
      {
         DWORD dws;
         DWORD dwh = 0;
         dws = GetFileSize(h, &dwh);
         if( dws && !dwh )
         {
            LPTSTR   lpb = MALLOC( (dws + 8 + MXARGSS) );
            if(lpb)
            {
               if(( ReadFile( h, lpb, dws, &dwh, NULL ) ) &&
                  ( dws == dwh ) )
               {
                  LPTSTR * lpv;
                  DWORD    dwc, dwb, dwi, dwk;
                  INT      c, d;
                  LPTSTR   lptmp;

                  lpv = (LPTSTR *)((LPTSTR)lpb + dws);
                  dwc = 0;
                  lpv[dwc++] = "dummy";
                  for(dwh = 0; dwh < dws; dwh++)
                  {
                     lptmp = &lpb[dwh];
                     c = lpb[dwh];
                     if( c > ' ' )
                     {
                        if( c == ';' )
                        {
                           // line starts with a COMMENT
                           // **************************
                           dwh++;
                           for( ; dwh < dws; dwh++ )
                           {
                              c = lpb[dwh];  // so goto the END OF LINE / FILE
                              if( ( c < ' ' ) && ( c != '\t' ) )
                                 break;
                           }
                           // **************************
                           // removed COMMENT
                        }
                        else
                        {
                           // we have a COMMAND
                           dwk = 0;
                           dwb = dwh;  // keep the beginning offset
                           lpb[dwb+dwk] = (TCHAR)c;
                           dwk++;
                           dwh++;   // bump past the beginning
                           d = c;
                           for( ; dwh < dws; dwh++ )
                           {
                              c = lpb[dwh];  // extract, waiting for END
                              if( c == ';' )
                              {
                                 // except if escaped
                                 if( d == '\\' )
                                 {
                                    if(dwk)
                                       dwk--;
                                 }
                                 else
                                 {
                                    break;
                                 }
                              }
                              else if( ( c < ' ' ) && ( c != '\t' ) )
                              {
                                 break;   // we have the command length
                              }

                              lpb[dwb+dwk] = (TCHAR)c;
                              dwk++;

                              d = c;   // keep previous
                           }

                           //lpb[dwh] = 0;  // zero terminate the collection
                           lpb[dwb+dwk] = 0;  // zero terminate the collection
                           //for( dwi = dwh; dwi > dwb; dwi-- )
                           for( dwi = (dwb+dwk); dwi > dwb; dwi-- )
                           {
                              // backup, removing spaces
                              if( lpb[dwi] > ' ' ) {
                                 // FIX20040427 - also remove any double inverted commas
                                 if( lpb[dwi] == '"' ) {
                                    if( lpb[dwb] == '"' ) {
                                       if( (dwb + 1) < (dwi - 1) ) {
                                          lpb[dwi] = 0;
                                          dwi--;
                                          dwb++;
                                       }
                                    }
                                 }
                                 break;
                              }
                              lpb[dwi] = 0;  // stuff a NUL
                           }


                           // ============================================
                           lpv[dwc++] = &lpb[dwb]; // set start of command
                           // ============================================

                           if( c == ';' ) // if terminated by a comment
                           {
                              dwh++;   // bump past it, and go to END OF LINE / FILE!
                              // ****************************************************
                              for( ; dwh < dws; dwh++ )
                              {
                                 c = lpb[dwh];
                                 if( ( c < ' ' ) && ( c != '\t' ) )
                                    break;
                              }
                              // ****************************************************
                           }
                        }
                     }
                  }

                  if( dwc > 1 )
                  {
                     if( !Chk_Command( dwc, lpv ) )
                        bRet = TRUE;
                  }
                  else
                  {
                     // A file with NO commands is an ERROR!
                     bRet = TRUE;
                  }
               }
               MFREE(lpb);
            }
         }

         CloseHandle(h);
      }
   }
   return bRet;
}

#if	AddSplit

//;	-s[nnnn]	Split OUTPUT file size ...
BOOL  Get_Sp_Siz( LPTSTR lpc )
{
   BOOL  bRet = TRUE;   // assume ok
   Pgm_Flag |= Do_Split;   // we have at least -s
   if( *lpc )
   {
      DWORD dws  = 0;
      INT   c = *lpc;
      while( c )
      {
         if( Chk_Num(c) )
         {
            dws = (dws * 10) + (c - ' ');
         }
         else
         {
            bRet = FALSE;
            break;
         }
         lpc++;
         c = *lpc;
      }  // while numbers (or NOT)
      if( bRet )
      {
         if(dws)
            g_dwSp_Sz = dws;
         else
            bRet = FALSE;
      }
   }
   return bRet;
}

#endif   //		; AddSplit
	

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Get_X_Inp
// Return type: BOOL 
// Argument   : LPTSTR lpc
// Description: -X Command to EXCLUDE certain files from the processing.
//              NAMES or MASKS are added to a LINKED LIST structure for
// later reference.
///////////////////////////////////////////////////////////////////////////////
BOOL  Get_X_Inp( LPTSTR lpc )
{
   BOOL  bRet = FALSE;   // assume fail
   if( *lpc )
   {
      HANDLE   h = CreateFile( lpc, // file name
         GENERIC_READ,  // access mode
         FILE_SHARE_READ,  // share mode
         0, // SD
         OPEN_EXISTING, // how to create
         FILE_ATTRIBUTE_NORMAL,  // file attributes
         0 ); // handle to template file
      if( VFH(h) )
      {
         DWORD dws;
         DWORD dwh = 0;
         dws = GetFileSize(h, &dwh);
         if( dws && !dwh )
         {
            LPTSTR lpb = MALLOC( (dws + 8) );
            if(lpb)
            {
               if( ( ReadFile( h, lpb, dws, &dwh, NULL ) ) &&
                  ( dws == dwh ) )
               {
                  DWORD    dwc, dwb, dwi;
                  INT      c;
                  dwc = 0;
                  for(dwh = 0; dwh < dws; dwh++)
                  {
                     c = lpb[dwh];
                     if( c > ' ' )
                     {
                        if( c == ';' )
                        {
                           dwh++;
                           for( ; dwh < dws; dwh++ )
                           {
                              c = lpb[dwh];
                              if( ( c < ' ' ) && ( c != '\t' ) )
                                 break;
                           }
                        }
                        else
                        {
                           dwb = dwh;
                           dwh++;
                           for( ; dwh < dws; dwh++ )
                           {
                              c = lpb[dwh];
                              if( ( c == ';' ) || ( ( c < ' ' ) && ( c != '\t' ) ) )
                                 break;
                           }
                           lpb[dwh] = 0;  // zero terminate the collection
                           for( dwi = dwh; dwi > dwb; dwi-- )
                           {
                              // backup, removing spaces
                              if( lpb[dwi] > ' ' )
                                 break;
                              lpb[dwi] = 0;
                           }
                           // ============================================
                           if( !Add2XLList( &lpb[dwb] ) ) // start of command
                           {
                              dwc = (DWORD)-1;
                              break;
                           }
                           dwc++;
                           // ============================================
                        }
                     }
                  }
                  if( dwc != (DWORD) -1 )
                  {
                     bRet = TRUE;
                  }
                  else
                  {
                     bRet = TRUE;
                  }
               }
               MFREE(lpb);
            }
         }
         CloseHandle(h);
      }
   }
   return bRet;
}

BOOL  Get_X_List( LPTSTR lpc )
{
   BOOL  bRet = FALSE;  // assume NOT ok!
   if( *lpc )
   {
      if( *lpc == '@' )
      {
         lpc++;
         bRet = Get_X_Inp(lpc);
      }
      else
      {
         if( Add2XLList(lpc) )
            bRet = TRUE;
      }
   }
   return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Got_FileC
// Return type: BOOL 
// Argument   : LPTSTR lpc
// Description: Build a WORK LIST of FILES or FILE MASKS.
//              MASKS will be later EXPANDED to FileNames
///////////////////////////////////////////////////////////////////////////////
BOOL  Got_FileC( LPTSTR lpc )
{
   BOOL  bRet = FALSE;   // assume NOT ok
   WIN32_FIND_DATA   fd;
   PWL   pwl;
   DWORD dwt;
   // should check???
//   if(gszName[0])
//      return FALSE;  // ONLY ONE ALLOWED

   dwt = IsValidFile4( lpc, &fd );
   if( dwt )
   {
      if( !gmGetFullPath( &gszName[0], lpc, 256 ) )
         strcpy( gszName, lpc );
      if( !gmGetFileTitle( gszTitle, gszName, 256 ) )
         strcpy( gszTitle, gszName );
      Pgm_Flag |= FileIN;
      pwl = Add2WLList( lpc );   // add to gsFileList as a WL entry
      if( pwl )
      {
         pwl->wl_dwFlag = 0;
         pwl->wl_dwType = dwt;
         strcpy( &pwl->wl_szFull[0], gszName );
         strcpy( &pwl->wl_szTitle[0], gszTitle );
         memcpy( &pwl->wl_sFD, &fd, sizeof(WIN32_FIND_DATA) );
         bRet = TRUE;
      }
   }
   return bRet;
}

// case 'W':
//;;;Fix_DIR		equ	0002h	; -w Convert DIR listing
//#if	AddDIR
// -wo[SD-]
// #define     bf_XcludeD     0x00000100
// #define     bf_XcludeF     0x00000200

#ifdef  ADDJOBS1   // -j 22 Jan. 2004

extern TCHAR g_szHelpj[];
// case 'J': lpc++;
// extern int DoJobs( PTSTR lpf );
int Get_J_Cmd(PTSTR lpc)
{
   int iret = 0;
   DWORD len = strlen(lpc);
   DWORD wrap = DEF_WRAP_LEN;
   while( lpc && *lpc ) {
      DWORD c = toupper(*lpc);
      switch(c)
      {
      case '?':
      case 'H':
         {
            // show HELP
            sprtf(g_szHelpj);
            //exit(0);
            Pgm_Exit(0);
         }
         break;
      case 'D':
         Pgm_Flag |= Do_JobsD;   // -jd delete header/tail from paste
         if(( len > 1 )&&( lpc[1] == '+' )) {
            lpc++; // eat the -jD+ sign
            Pgm_Flag |= Do_JobsDP; // -jd+ write delete lines to tempdel.txt
         }
         break;
      case 'E':
         Pgm_Flag |= Do_JobsE;   // -je
         break;
      case 'F':
         Pgm_Flag |= Do_JobsF;   // -jf
         break;
      case 'S':
         Pgm_Flag |= Do_JobsS;   // -js
         break;
      case 'T':
         Pgm_Flag |= Do_JobsT;   // -jt
         break;
      case 'W':
         Pgm_Flag |= Do_JobsW;   // -jw
         wrap = DEF_WRAP_LEN;
         if(( len > 1 )&&( ISNUM(lpc[1] ) )) {
            wrap = atoi( &lpc[1] );
            while( ISNUM( lpc[1] ) ) lpc++;
            // bump to last number
         }
         g_dwWrap = wrap;
         break;
      case 'O':   // -jo[output file]
         {
            LPTSTR   lps; // = GetNxtBuf();
            LPTSTR   lps2 = GetNxtBuf();
            // had the G, now

            lpc++; // bump past the 'O'
            lps = GetExQuotes(lpc);
            _fullpath(lps2,lps,256);
            strcpy( &gcGOutFile[0], lps2 );
            goto Set_Flag; // got command = ADD FLAG
         }
         break;
      default:
         iret++;
         break;
      }

      if( iret != 0 )
         break;

      lpc++; // bump to NEXT
      len--; // reduce available length of string
   }

Set_Flag:

   Pgm_Flag |= Do_Jobs;   // -j

   return iret;
}

#endif // #ifdef  ADDJOBS1   // -j 22 Jan. 2004

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Chk_Command
// Return type: BOOL 
// Arguments  : INT icnt
//            : LPTSTR * lpa
// Description: Process the 'C' command line array
//              keeping a copy in the gszOutBuf (upto MXOUTB)
///////////////////////////////////////////////////////////////////////////////
BOOL  Chk_Command( INT icnt, LPTSTR * lpa )
{
   BOOL     bRet = FALSE;
   INT      i, j, c;
   LPTSTR   lpc, lpc1;
   BOOL     isup = FALSE;
   for( i = 1; i < icnt; i++ )
   {
      lpc = lpa[i];  // get the pointer
      if(lpc)
      {
         j = strlen(lpc);
         if( j )
         {
            c = *lpc;
            Add2CmdBuf( lpc, j );
            switch(c)
            {
            case '-':
            case '/':
               {
                  lpc++;
                  c = toupper(*lpc);
                  if( c == *lpc )
                     isup = TRUE;
                  else
                     isup = FALSE;
                  switch(c)
                  {
                  case '?':
                     Give_Help( 0 );
                     break;
                  case '@':
                     lpc++;
                     if( !Get_Inp_Fil(lpc) )
                        Show_Bad_Cmd( lpa[i], '@' );
                     break;

                  case 'B':
                      // dmsg(	" -b         Suppress Backspaced duplications"MEOR );
	                  // dmsg(	" -B         Remove UTF-8 BOM from file"MEOR );
                     lpc++;
                     if( *lpc ) {
                         if (*lpc == '-') {
                             // negative means NOT
                              if (isup)
                                  gbDelBOM = FALSE;
                              else
                                  Pgm_Flag &= ~BS_Dup;
                         } else if (*lpc == '+') {
                             // plus means POSITIVE
                              if (isup)
                                  gbDelBOM = TRUE;
                              else
                                  Pgm_Flag |= BS_Dup;
                         } else {
                            Show_Bad_Cmd( lpa[i], 
                                (isup ? 'B' : 'b') );
                         }
                     } else {
                          // nothing means POSITIVE
                          if (isup)
                              gbDelBOM = TRUE;
                          else
                              Pgm_Flag |= BS_Dup;
                     }
                     break;

#ifdef ADD_TXT2VC // FIX20050203 - add -c - output C/C++ text form
                     // dmsg( " -c[name]   Output as single C/C++ text form."MEOR );
                  case 'C':
                     if( !Get_VC_Flag( lpc ) ) // Pgm_Flag |= VC_Out;
                        Show_Bad_Cmd( lpa[i], 'C' );
                     break;
#endif // #ifdef ADD_TXT2VC // FIX20050203 - add -c - output C/C++ text form

                  case 'D':
                     lpc++;
                     if( !Get_Del_Flag( lpc ) )
                        Show_Bad_Cmd( lpa[i], 'D' );
                     break;
#if   AddFunc
                  case 'F':
                     lpc++;
                     if( !GetListType(lpc) )
                        Show_Bad_Cmd( lpa[i], 'F' );
                     Pgm_Flag |= Do_Funcs;
                     break;
#endif   // AddFunc

#ifdef   ADDGEOFF2   // only when completed
                  case 'G':
                     lpc++;
                     if( !Get_G_Cmd( lpc ) )
                        Show_Bad_Cmd( lpa[i], 'G' );
                     break;
#endif   // #ifdef   ADDGEOFF2   // only when completed

                  case 'H':
                     lpc++;
                     if( Get_H_Cmd(lpc) )
                        Show_Bad_Cmd( lpa[i], 'H' );
                     break;

#ifdef  ADDJOBS1   // -j 22 Jan. 2004
                  case 'J':
                     lpc++;
                     if( Get_J_Cmd(lpc) )
                        Show_Bad_Cmd( lpa[i], 'J' );
                     break;

#endif // #ifdef  ADDJOBS1   // -j 22 Jan. 2004
                  case 'L':
                     lpc++;
                     if( !Get_LL( lpc ) )
                        Show_Bad_Cmd( lpa[i], 'L' );
                     break;
                  case 'N':
                     gbNoEOF = TRUE;
                     break;
                  case 'O':
                     lpc++;
                     if( Get_OUTF(lpc) )
                        Show_Bad_Cmd( lpa[i], 'O' );
                     break;
#if	AddPara
                  case 'P':
                     lpc++;
                     if( *lpc )
                        Show_Bad_Cmd( lpa[i], 'P' );
                     Pgm_Flag |= Fix_PARA;   // Add remove Cr/Lf EXCEPT pairs
                     break;
#endif			; AddPara
#if	AddRem
                  case 'R':
                     lpc++;
                     if( *lpc == 0 )   // if entered as -R "[ this ]"
                     {
                        if( ( i + 1 ) < icnt )
                        {
                           i++;
                           lpc1 = lpa[i];  // get the pointer
                           if(lpc1)
                           {
                              j = strlen(lpc1);
                              if(j)
                              {
                                 Add2CmdBuf( lpc1, j );
                                 lpc = lpc1;
                              }
                           }
                        }
                     }
                     if( !Get_Rem_Ln(lpc, isup) )  // Get the data of a REMOVE LINE request
                        Show_Bad_Cmd( lpa[i], (isup ? 'R' : 'r') );
                     break;
#endif   //	; AddRem
#if	AddSplit
                  case 'S':
                     lpc++;
                     if( !Get_Sp_Siz( lpc ) )
                        Show_Bad_Cmd( lpa[i], '@' );
                     break;
#endif   // ; AddSplit
                  case 'T':
                     lpc++;
                     if( !Get_Tab_Siz( lpc ) )
                        Show_Bad_Cmd( lpa[i], 'T' );
                     break;
                  case 'U':
                     lpc++;
                     if( *lpc ) {
                        c = toupper(*lpc);
                        if( c == 'D' ) {
                           gbUnifDiff = TRUE;   // Convert Unified DIFF patch headers '\' to '/'
                           gbNoEOF = TRUE;      // no 'eof' added 
                           gbDelBOM = TRUE;     // remove any UTF-8 BOM from file
                           lpc++;
                           if( *lpc )
                              Show_Bad_Cmd( lpa[i], 'U' );
                        } else {
                           Show_Bad_Cmd( lpa[i], 'U' );
                        }
                     }
                     Pgm_Flag |= Do_Unix;
                     break;
                  case 'V':
                     lpc++;
                     if( Set_V_Cmd(lpc) )
                        Show_Bad_Cmd( lpa[i], 'V' );
                     break;
//;;;Fix_DIR		equ	0002h	; -w Convert DIR listing
#if	AddDIR
//	Cmp	Al,'W'	;  = ' -w Convert DIR listing file.'> ;
                  case 'W':
                     lpc++;
                     if( Set_W_Cmd(lpc) )
                        Show_Bad_Cmd( lpa[i], 'W' );
                     break;
#endif   //		; AddDIR
                  case 'X':
                     lpc++;
                     if( !Get_X_List(lpc) )
                        Show_Bad_Cmd( lpa[i], 'X' );
                     break;
                  default:
                     goto Got_Err;
                     break;
                  }  // end switch case
               }  // case of - or / switch
               break;
            case '@':
               lpc++;
               if( !Get_Inp_Fil(lpc) )
                  Show_Bad_Cmd( lpa[i], '@' );
               break;
            default:
               if( !Got_FileC( lpc ) )
                  Show_Bad_Cmd( lpa[i], 'I' );
               break;
            }
         }
         else
            goto Got_Err;
      }
      else
      {
Got_Err:
         Give_Help( 1 );
         bRet = TRUE;
         break;
      }
   }

   return bRet;

}


// eof - FixFHelp.c
