
// FixFDir.c
#include	"FixF32.h"
// #include	"FixFDir.h"
extern   VOID  Do_Happy_Msg(VOID);

#define dmsg    sprtf

// VersNum3    21  // FIX20041120 improve -w (directory/folder) output...
// VerDate	"20 November, 2004" // get largest length, and size, and format output

// output to include Directory/Folder, Size/Bytes, Date/Time
BOOL g_ListDir = TRUE;
BOOL g_ListSz = TRUE;
BOOL g_ListDt = TRUE;
BOOL g_ListDup = TRUE;
BOOL g_ListFormat = TRUE; // make the output a flat file
DWORD g_dwDupsSkip = 0;

VOID  Show_W_Help( LPTSTR lpc )
{
// 	dmsg(	" -w[af...]  Convert DIR file to ordered list. "
//   "(a=americ.mdy, f[2]=full t=try o[SN0]= Order)"MEOR  );

   sprtf( "Due to command [%s], showing ?. help?"MEOR, lpc );

	dmsg(	" -w plus - Convert a DIR file to a list."MEOR );
   dmsg( "    <nul> = Perform default conversion to date ordered list."MEOR );
   dmsg( "    a     = Switch Day and Month. ie MM-DD-YY type."MEOR );
   dmsg( "    f     = Show converted file details. (def.)"MEOR );
   dmsg( "    f2    = Show full file details from file."MEOR );
   dmsg( "    o     = Order list, newest first. (default)."MEOR );
   dmsg( "    o[-|0]= Remove list ordering. (def=On)"MEOR );
   dmsg( "    o[D|N|S] = Order list per Date (def), Name or Size."MEOR );
   dmsg( "    t     = Try Harder with DIR file. (def=On)"MEOR );
    // adjust list output
   dmsg( "    l[1234]= List with 1=Folder, 2=Size, 3=Date 4=Dupes (def=1234)"MEOR );
   dmsg( "    l5    = Format List to even width (def=ON)"MEOR );
   Do_Happy_Msg();

	Pgm_Exit( 0 );
}

BOOL  Set_W_Cmd( LPTSTR lpc )
{
   INT   c, ilen;
   LPTSTR   pbgn = lpc - 2;   // show command from the '-' character

   ilen = strlen(lpc);
   while( *lpc )
   {
      c = toupper(*lpc);
      switch(c)
      {
      case '?':
         Show_W_Help( pbgn );
         break;

      case 'A':
         g_dwFullDir |= bf_American;
         break;

      case 'F':
         {
            if((lpc[1]) && (lpc[1] == '2'))
            {
               // followed by a 2, then
               lpc++;   // use up this char
               g_dwFullDir |= bf_FullDir; // set -wf2
               g_dwFullDir &= ~(bf_FullDir2); // remove default
            }
            else
            {
               g_dwFullDir |= bf_FullDir2; // long name, and information = -wf = def
               g_dwFullDir &= ~(bf_FullDir); // remove any -wf2
            }
         }
         break;

      case 'L': // listing options 1=Folder 2=Size 3=Date/Time
         lpc++;   // bump to NEXT
         if( ISNUM(*lpc) ) {
            while( ISNUM(*lpc) ) {
               c = *lpc;
               switch(c)
               {
               case '1':
                  lpc++;
                  g_ListDir = TRUE;
                  if(*lpc == '-') {
                     g_ListDir = FALSE;
                     lpc++;
                  }
                  break;
               case '2':
                  lpc++;
                  g_ListSz = TRUE;
                  if(*lpc == '-') {
                     g_ListSz = FALSE;
                     lpc++;
                  }
                  break;
               case '3':
                  lpc++;
                  g_ListDt = TRUE;
                  if(*lpc == '-') {
                     g_ListDt = FALSE;
                     lpc++;
                  }
                  break;
               case '4':
                  lpc++;
                  g_ListDup = TRUE;
                  if(*lpc == '-') {
                     g_ListDup = FALSE;
                     lpc++;
                  }
                  break;
               case '5': // FIX20041120 improve -wl5 output...
                  g_ListFormat = TRUE; // make the output a flat file
                  lpc++;
                  if(*lpc == '-') {
                     g_ListFormat = FALSE; // no attempt at fixed width
                     lpc++;
                  }
                  break;
               default:
                  return TRUE; // FAILED
                  break;
               }
            }
            lpc--;
         } else {
            return TRUE; // FAILED
         }
         break;
      case 'O':
         g_dwFullDir |= bf_SortList;
         if( lpc[1] )
         {
            lpc++;
            c = toupper(*lpc);
            while( c )
            {
               if( c == 'S' )
               {
                  g_dwFullDir |= bf_SizeSort;
                  g_dwFullDir &= ~( bf_DateSort | bf_NameSort );  // clear these out
               }
               else if( c == 'D' )
               {
                  g_dwFullDir |= bf_DateSort;
                  g_dwFullDir &= ~( bf_SizeSort | bf_NameSort );  // clear these out
               }
               else if( c == 'N' )
               {
                  g_dwFullDir |= bf_NameSort;
                  g_dwFullDir &= ~( bf_DateSort | bf_SizeSort );  // clear these out
               }
               else if(( c == '-' ) || ( c == '0' ) || ( c== 'O' ))
               {
                  g_dwFullDir &= ~( bf_SortList | bf_SizeSort | bf_NameSort);  // remove the flags
               }
               else
               {
                  break;   // NOT an ORDER type char
               }
               lpc++;
               c = toupper(*lpc);   // get NEXT
            }  // while characters are part of -wO[SDN0] date,size,name,off
            lpc--;   // backup to let main loop take it
         }
         break;

      case 'T':
         g_dwFullDir |= bf_TryHard;
         if( lpc[1] )
         {
            lpc++;
            c = toupper(*lpc);
            if(( c == '-' ) || ( c == '0' ) || ( c== 'O' ))
               g_dwFullDir &= ~(bf_TryHard); // remove flag, and eat char
            else if(( c == '+' ) || ( c == '1' ))
               g_dwFullDir |= bf_TryHard; // add flag and remove char
            else
               lpc--;   // leave char for main loop
         }
         break;
#ifdef  ADD_WX_CMD     // add -wx[d|f]
      case 'X':
         {
            PLE   ph, pn;
            PBLK  pb;
            PTSTR p, pc;
            lpc++;   // onto NEXT
            ilen = strlen(lpc);  // get LENGTH of TAIL - must be [d|f]:f
            if( ilen < 3 )
               return TRUE;   // failed
            if( lpc[1] != ':' )
               return TRUE;   // failed

            if( toupper(lpc[0]) == 'D' ) {
               ph = &sXcludeD;
               g_dwFullDir |= bf_XcludeD;
            } else if( toupper(lpc[0]) == 'F' ) {
               ph = &sXcludeF;
               g_dwFullDir |= bf_XcludeF;
            } else {
               return TRUE;   // is an ERROR
            }

            lpc += 2;   // bump past the [d|f]: to point at the file or dir
            pc = lpc;
            p = strchr(pc,';');
            while(p) {
               p++;
               ilen = p - pc;
               pn = (PLE)MALLOC( (sizeof(BLK) + ilen) );
               pb = (PBLK)pn;
               pb->b_Len = ilen;
               strncpy( pb->b_Stg, pc, ilen );
               pb->b_Stg[ilen] = 0;
               InsertTailList(ph,pn);
               pc = p;
               p = strchr(pc,';');
            }
            ilen = strlen(pc);
            if(ilen) {
               pn = (PLE)MALLOC( (sizeof(BLK) + ilen) );
               pb = (PBLK)pn;
               pb->b_Len = ilen;
               strncpy( pb->b_Stg, pc, ilen );
               pb->b_Stg[ilen] = 0;
               InsertTailList(ph,pn);
            }
            lpc = pc + ilen;
         }
         break;
#endif // #ifdef  ADD_WX_CMD     // add -wx[d|f]

      default:
         return TRUE;   // all else is ERROR
         break;
      }

      if( *lpc )
         lpc++;   // bump pointer to next char
   }

   Pgm_Flag |= Fix_DIR;   //-W

   return FALSE;

}


//#define  MXPTHNM        47
#define  MXPTHNM        52    // FIX20030910 - smooth out display output

#define  flg_IsHeaderV1 0x00000001  // Volume in Drive
#define  flg_IsFolder   0x00000002  // Just a <DIR> entry
#define  flg_IsDir      0x00000004  // Directory of 
#define  flg_IsTotal    0x00000008  // nnnn file(s) nnnn bytes
#define  flg_IsTail     0x00000010  // nnnn dir(s) nnnn aa free
#define  flg_IsTHeader  0x00000020  // Total Files Listed:
#define  flg_IsHeaderV2 0x00000040  // Volume Serial Number
#define  flg_IsHeaderTo 0x00000080  // (Too many file) special for SORTED

#define  flg_OK         0x00010000  // just an OK flag

#define  flg_DnOut      0x40000000
#define  flg_IsFile     0x80000000

#define  flg_IsHeaderV  (flg_IsHeaderV1 | flg_IsHeaderV2)
//#define  flg_Min  (flg_IsHeaderV | flg_IsFolder | flg_IsDir | flg_IsTotal)
#define  WFD   WIN32_FIND_DATA
#define  PWFD  WFD *
#define  USEWFDSTR

typedef  struct tagLNLIST {
   LIST_ENTRY  pList;   // LIST_ENTRY must be first
   DWORD       dwRank;  // Just the ORDER found in the file
   DWORD       dwLn;    // Line number (perhaps not accurate!)
   DWORD       dwOrd;   // eventual out ORDER
   DWORD       dwOrd2;  // sorted DATE order
   DWORD       dwLen;   // length of line
   DWORD       dwFlag;  // various flags
   DWORD       dwSize;  // size of file (if a file)
   DWORD       dwOK;    // decoded file time and date
#ifdef   USEWFDSTR
   WFD         sFD;
#else
   FILETIME    sFT;     // date/time of file
#endif
   INT         ll_iPos;    // offset to FULL file name
   TCHAR       szPath[264];   // previous FOLDER name found
   TCHAR       szLn[1]; // actual line data
}LNLIST, * PLNLIST;

//#define  SKIPSPACE(p)   while( *p && (*p <= ' '))p++

TCHAR szDirOf[] = "Directory of ";
INT   iDirOf; // length of above string
#ifdef  ADD_WX_CMD     // add -wx[d|f]
LIST_ENTRY  sXcludeD = { &sXcludeD, &sXcludeD };
LIST_ENTRY  sXcludeF = { &sXcludeF, &sXcludeF };
#endif   // #ifdef  ADD_WX_CMD     // add -wx[d|f]



VOID  ShowFlag( DWORD dwf )
{
   LPTSTR   lpt = &gszTmpBuf[0]; // setup a temp buffer
   strcpy( lpt, "Flags= " );
   if( dwf == 0 )
      strcat(lpt, "<NONE!!!>");
   if( dwf & flg_IsHeaderV1 )
      strcat(lpt, "VDrive "); // Volume in Drive
   if( dwf & flg_IsHeaderV2 )
      strcat(lpt, "Serial# "); // Volume Serial Number
   if( dwf & flg_IsDir )
      strcat(lpt, "Directory "); // Directory of 
   if( dwf & flg_IsTotal )
      strcat(lpt, "file(s)&bytes ");   // nnnn file(s) nnnn bytes
   if( dwf & flg_IsTail )
      strcat(lpt, "dir(s)&free ");  // nnnn dir(s) nnnn aa free
   if( dwf & flg_IsTHeader )
      strcat(lpt, "Total ");  // Total Files Listed:
   if( dwf & flg_IsFolder )
      strcat(lpt, "<DIR> "); // Just a <DIR> entry
   if( dwf & flg_IsHeaderTo )
      strcat(lpt, "Sort ");  // (Too many file) special for SORTED
   TrimIB(lpt);
   strcat(lpt,MEOR);
   sprtf(lpt);
}

DWORD longest_file_name;
DWORD longest_dir_name;
DWORD largest_file_size;
VOID  _cleardorder( PLE ph )
{
   DWORD len;
   PLE      pn;
   PLNLIST  pll, plldir;
   LPTSTR   lpb = &gszGenBuf[0]; // a general buffer
   LPTSTR ptmp;
   longest_file_name = 0;
   longest_dir_name = 0;
   largest_file_size = 0;
   Traverse_List( ph, pn )
   {
      pll = (PLNLIST)pn;   // cast to my structure
      if(pll->dwFlag & flg_IsDir) {
         plldir = pll;  // keep 'last' directory pointer
         ptmp = &plldir->szLn[0];
         strcpy(lpb, &ptmp[iDirOf]);
         TrimIB(lpb);
         len = strlen(lpb);
         if(len > longest_dir_name)
            longest_dir_name = len;

      } else if( pll->dwFlag & flg_IsFile ) {
         len = strlen(pll->sFD.cFileName);
         if(len > longest_file_name)
            longest_file_name = len;

         if( pll->dwSize > largest_file_size )
            largest_file_size = pll->dwSize;

         pll->dwOrd = 0;      // SET NO ORDER NUMBER
      }
   }
}

// this can be very long to go through and thru and thru the list
// why not do ONE pre-sort of the list, and return the 'next'
// each call there after
PLE   pYoungest = 0;
void  _setyougest( PLE ph )
{
   INT      icnt;
   PLE      pn;
   PLNLIST  pll;
   ULARGE_INTEGER liMin, liMax, liCur;
//VOID GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime);  // file time
   FILETIME ft;
#ifdef USE_COMP_FIO
   liMin.QuadPart = time(NULL);
   ft.dwLowDateTime = liMin.LowPart;
   ft.dwHighDateTime = liMin.HighPart;
#else 

   GetSystemTimeAsFileTime(&ft);  // file time
   liMin.LowPart  = ft.dwLowDateTime;
   liMin.HighPart = ft.dwHighDateTime;
   // this is system time - maybe should bump 'some' time, but
   // for now 'assume' this is a directory file *ON* this system
   // thus all date/time on files *MUST* be less than this!!!
#endif 
   liMax.QuadPart = 0;  // or perhaps -(__int64)liMin.QuadPart;
   icnt = 0;
   Traverse_List( ph, pn )
   {
      pll = (PLNLIST)pn;   // cast to my structure
      // 100-nanoseconds intevals since January 1, 1601 (UTC). 
      if(( pll->dwFlag & flg_IsFile ) && ( pll->dwOrd == 0 ))   // IF NO ORDER NUMBER
      {

         liCur.LowPart = pll->sFD.ftLastWriteTime.dwLowDateTime;
         liCur.HighPart = pll->sFD.ftLastWriteTime.dwHighDateTime;
         if(liCur.QuadPart > liMax.QuadPart)
            liMax.QuadPart = liCur.QuadPart;
         if(liCur.QuadPart < liMin.QuadPart)
            liMin.QuadPart = liCur.QuadPart;

         icnt++;
      }
   }
   {
      LPTSTR lpb = GetNxtBuf();
      sprintf(lpb, "Max = %s and Min = %s",
         Get_I64_Stg(liMax.QuadPart), Get_I64_Stg(liMin.QuadPart) );
   }
   // hmmm, with the MIN and MAX values, should be able to compute RANK
   {
      LPTSTR lpb = GetNxtBuf();
      double max; // = ( liMax.QuadPart / 10000.0 );
      double min; // = ( liMin.QuadPart / 10000.0 );
      double dif,cur; // = (max - min);
      double av; //  = (dif / (double)icnt);
      LARGE_INTEGER li;

      li.QuadPart = liMax.QuadPart;
      max = ( (double)li.QuadPart / 100000000.0 );
      li.QuadPart = liMin.QuadPart;
      min = ( (double)li.QuadPart / 100000000.0 );
      dif = (max - min);
      av  = (dif / (double)icnt);
      li.LowPart  = ft.dwLowDateTime;
      li.HighPart = ft.dwHighDateTime;
      cur = ( (double)li.QuadPart / 100000000.0 );
      
      sprintf(lpb, "Min %.5f Max %.5f Dif %.5f av %.5f cur %.5f",
         min, max, dif, av, cur );

      if(min > max)
         cur = 0.0;
   }
}

long MyCompareFileTime(FILETIME * pft1, FILETIME * pft2)
{
    long lg = 0;
    if (pft1->dwHighDateTime == pft2->dwHighDateTime) {
        if (pft1->dwLowDateTime < pft2->dwLowDateTime)
            lg = -1;
        else if (pft1->dwLowDateTime > pft2->dwLowDateTime)
            lg = 1;
    }
    else {
        if (pft1->dwHighDateTime < pft2->dwHighDateTime)
            lg = -1;
        else if (pft1->dwHighDateTime > pft2->dwHighDateTime)
            lg = 1;
    }
    return lg;
}

#ifndef _WIN32
#define CompareFileTime MyCompareFileTime 
#endif // !_WIN32
// #define  Traverse_Cont(pListHead,pListNext)
//   for( pListNext = pListNext->Flink; pListNext != pListHead; pListNext = pListNext->Flink )

PLE   _getyoungest( PLE ph )
{
   PLE      pn;
   PLNLIST  pll, pl1;
   LONG     lg;
   static PLE pfirst = 0;
   INT      icnt = 0;
   //_setyougest( ph );
   pl1 = 0;
   Traverse_List( ph, pn )
   {
      pll = (PLNLIST)pn;   // cast to my structure
      if(( pll->dwFlag & flg_IsFile ) && ( pll->dwOrd == 0 ))   // IF NO ORDER NUMBER
      {
         if( pl1 )   // if we have a PREVIOUS without order, ie a first
         {
            // -1 = First file time is less than second file time.
            //lg = CompareFileTime( &pl1->sFT, &pll->sFT );
            lg = CompareFileTime( &pl1->sFD.ftLastWriteTime , &pll->sFD.ftLastWriteTime );
            if( lg < 0 )
               pl1 = pll;  // keep this 'less than 0' item
         }
         else
         {
            pl1 = (PLNLIST)pn;   // keep first without ORDER
            pfirst = pn;
         }
         icnt++;
      }
   }

   return (PLE)pl1;
}

PLE   _getyoungest_slow_for_large_numbers( PLE ph )
{
   PLE      pn;
   PLNLIST  pll, pl1;
   LONG     lg;

   pl1 = 0;
   Traverse_List( ph, pn )
   {
      pll = (PLNLIST)pn;   // cast to my structure
      if(( pll->dwFlag & flg_IsFile ) && ( pll->dwOrd == 0 ))   // IF NO ORDER NUMBER
      {
         if( pl1 )   // if we have a PREVIOUS without order
         {
            // -1 = First file time is less than second file time.
            //lg = CompareFileTime( &pl1->sFT, &pll->sFT );
            lg = CompareFileTime( &pl1->sFD.ftLastWriteTime , &pll->sFD.ftLastWriteTime );
            if( lg < 0 )
               pl1 = pll;  // keep this 'less than 0' item
         }
         else
         {
            pl1 = (PLNLIST)pn;   // keep first without ORDER
         }
      }
   }

   return (PLE)pl1;
}


PLE   _getlargest( PLE ph )
{
   PLE      pn;
   PLNLIST  pll, pl1;

   pl1 = 0;
   Traverse_List( ph, pn )
   {
      pll = (PLNLIST)pn;   // cast to my structure
      if(( pll->dwFlag & flg_IsFile ) && ( pll->dwOrd == 0 ))   // IF NO ORDER NUMBER
      {
         if( pl1 )   // if we have a PREVIOUS without order
         {
            if( pl1->dwSize < pll->dwSize )
               pl1 = pll;  // return the LARGER
         }
         else
         {
            pl1 = (PLNLIST)pn;   // keep first without ORDER
         }
      }
   }
   return (PLE)pl1;
}

PLE   _getnxtname( PLE ph )
{
   PLE      pn;
   PLNLIST  pll, pl1;
   INT      i;

   pl1 = 0;
   Traverse_List( ph, pn )
   {
      pll = (PLNLIST)pn;   // cast to my structure
      if(( pll->dwFlag & flg_IsFile ) && ( pll->dwOrd == 0 ))   // IF NO ORDER NUMBER
      {
         if( pl1 )   // if we have a PREVIOUS without order
         {
            i = strcmpi( &pl1->szLn[pl1->ll_iPos], &pll->szLn[pl1->ll_iPos] );
            if( i > 0 )
               pl1 = pll;  // return the LARGER
         }
         else
         {
            pl1 = (PLNLIST)pn;   // keep first without ORDER
         }
      }
   }
   return (PLE)pl1;
}

PLE   _getavailable( PLE ph )
{
   PLE      pn;
   PLNLIST  pll;
   Traverse_List( ph, pn )
   {
      pll = (PLNLIST)pn;   // cast to my structure
      if(( pll->dwFlag & flg_IsFile ) && ( pll->dwOrd == 0 ))   // IF NO ORDER NUMBER
         return pn;
   }
   return 0;
}

PLE   _getnext( PLE ph, BOOL bSort, BOOL bSize, BOOL bName )
{
   PLE   pn = 0;
   if( bSort )
   {
      if(bName)
         pn = _getnxtname(ph);
      else if(bSize)
         pn = _getlargest(ph);
      else
         pn = _getyoungest(ph);
   }
   else
      pn = _getavailable(ph);

   return pn;
}

//   SortDirList( ph, dwfcv );
VOID  SortDirList( PLE ph, DWORD dwfcv );
///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : OrderDirList2
// Return type: VOID 
// Arguments  : PLE   ph
//            : DWORD dwfcv
// Description: DEFAULT sorting is LATEST ie NEWEST first,
//              down to OLDEST last, or other orderings
///////////////////////////////////////////////////////////////////////////////
VOID  OrderDirList2( PLE ph, DWORD dwfcv )
{
   DWORD    iCnt = 0;
   PLE      pn;
   PLNLIST  pll;
   BOOL     bSize = (g_dwFullDir & bf_SizeSort);
   BOOL     bSort = (g_dwFullDir & bf_SortList);
   BOOL     bName = (g_dwFullDir & bf_NameSort);

   if(VERB2) {
       if(bSort) {
           if(bName)
               sprtf("Sorting by NAME ..."MEOR);
           else if(bSize)
               sprtf("Sorting by SIZE ..."MEOR);
           else
               sprtf("Sorting by DATE ..."MEOR);
       } else {
         sprtf("No SORT. Using order found in file ..."MEOR);
       }
   }
   _cleardorder( ph );
//   if( bSort && !bSize && !bName ) {
//      SortDirList( ph, dwfcv );
//      return;  // all done - using hopefully FASTER bubble sort
//   }
   pn = _getnext(ph, bSort, bSize, bName);
   while(pn)
   {
      iCnt++;
      pll = (PLNLIST)pn;   // cast to my structure
      pll->dwOrd = iCnt;   // SET AN ORDER NUMBER
      pn = _getnext(ph, bSort, bSize, bName);
   }

   if( iCnt != dwfcv )
      chkme( "HEY! Two different counts %d vs %d!?", iCnt, dwfcv );

}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : SortDirList
// Return type: VOID 
// Arguments  : PLE ph
//            : DWORD dwfcv
// Description: DEFAULT sorting is LATEST ie NEWEST first,
//              down to OLDEST last
///////////////////////////////////////////////////////////////////////////////
VOID  SortDirList( PLE ph, DWORD dwfcv )
{
   DWORD    iCnt = 0;
   PLE      pn;
   PLNLIST  pll;

Do_Again:

   Traverse_List( ph, pn )
   {
      pll = (PLNLIST)pn;   // cast to my structure
      if(( pll->dwFlag & flg_IsFile ) && ( pll->dwOrd == 0 ))
      {
         PLE      pn2;
         PLNLIST  pl2, pls;
         LONG     lg;
         pls = 0;
         Traverse_List( ph, pn2 )
         {
            if( pn != pn2 )
            {
               pl2 = (PLNLIST)pn2;   // cast to my structure
               if( ( pl2->dwFlag & flg_IsFile ) && ( pl2->dwOrd == 0 ) )
               {
                  // it should be two files
                  lg = CompareFileTime( &pll->sFD.ftLastWriteTime, &pl2->sFD.ftLastWriteTime );
                  if( lg < 0 )   // if 2nd is LATER = OLDER
                  {
                     if( pls )
                     {
                        lg = CompareFileTime( &pls->sFD.ftLastWriteTime, &pl2->sFD.ftLastWriteTime );
                        if( lg < 0 )   // if 2nd is LATER = OLDER
                        {
                           pls = pl2;  // set latest
                        }
                     }
                     else
                        pls = pl2;  // set latest
                  }
               }
            }
         }
         if( pls == 0 )    // if NO later file time
            pls = pll;     // then THIS is the latest
         iCnt++;
         pls->dwOrd = iCnt;   // SET AN ORDER NUMBER
      }
   }

   if(VERB2) sprtf( "Sorted %d file lines ...(of %d)"MEOR, iCnt, dwfcv );

   Traverse_List( ph, pn )
   {
      pll = (PLNLIST)pn;   // cast to my structure
      if( ( pll->dwFlag & flg_IsFile ) && ( pll->dwOrd == 0 ) )
      {
         if(VERB3) sprtf( "Moment - Sorting %d file lines ...(of %d)"MEOR, (dwfcv - iCnt), dwfcv );
         goto Do_Again;
      }
   }
}

extern   LPTSTR   NiceNumberStg( DWORD dwNum, DWORD dwLen );
extern   LPTSTR   FileTime2AscStg( FILETIME * pft );


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Do_Dir_Out
// Return type: VOID 
// Arguments  : PLNLIST plldir
//            : PLNLIST pll
// Description: Output the 'sorted' line
//              
///////////////////////////////////////////////////////////////////////////////
static TCHAR   _s_szlastdir[264] = { "\0" };
// FIX2004901 - Added -
// output to include Directory/Folder, Size/Bytes, Date/Time
//BOOL g_ListDir = TRUE;
//BOOL g_ListSz = TRUE;
//BOOL g_ListDt = TRUE;

VOID  Do_Dir_Out( PLNLIST plldir, PLNLIST pll )
{
   LPTSTR   pln, ptmp;
   LPTSTR   lpt = &gszTmpBuf[0]; // setup a temp buffer
   LPTSTR   lpb = &gszGenBuf[0]; // and another general buffer
   DWORD    dwi, dwj;
   BOOL     bFullDir = (g_dwFullDir & bf_FullDir);
   LPTSTR   plast = _s_szlastdir;

   pln = &pll->szLn[0]; // get pointer to line data
   if(plldir)  // should ALWAYS have this
   {
      ptmp = &plldir->szLn[0];
      strcpy(lpb, &ptmp[iDirOf]);
      TrimIB(lpb);
      if( bFullDir )
      {
         sprintf(lpt, "%s [%s]", pln, lpb );
      }
      else
      {
         *lpt = 0;
         dwi = 0;

         if( g_ListDir ) {
            // Add DIRECTORY/FOLDER
            strcpy(lpt,lpb);
            dwi = strlen(lpt);
            if(dwi)
            {
               if( lpt[dwi-1] != '\\' ) {
                  strcat(lpt,"\\");
                  dwi++;
               }
            }

            // FIX20030910 - maybe suppress 'duplicate' directories/folders
            if( strcmp(plast,lpt) ) {  // if *NEW*
               strcpy(plast,lpt);   // make it the LAST
            } else { // it is a REPEAT
               for(dwj = 0; dwj < dwi; dwj++) {
                  lpt[dwj] = ' ';   // take it down to SPACES
               }
            }
         } // g_ListDir

         // Add FULL NAME
         // ****************************
         strcpy(lpb, &pln[pll->ll_iPos]);    // offset to FULL NAME OF FILE
         TrimIB(lpb);
         strcat(lpt,lpb);
         // ****************************

         if( g_dwFullDir & bf_FullDir2 )
         {
            // Add File Size
            if( g_ListSz ) {
               strcat(lpt," ");
               dwi = strlen(lpt);   // get it LENGTH
               // hmmm - if strlen(lpt) > MXPTHNM we should TRY to remove
               // any/some spaces from in front of number
               if(dwi > MXPTHNM) {  // already over the 80th column, so
                  LPTSTR pn = NiceNumberStg( pll->dwSize, 12 );
                  while(dwi > MXPTHNM)
                  {
                     if(*pn == ' ')
                        pn++;
                     else
                        break;
                     dwi--;
                  }
                  strcat(lpt,pn);   // possibly a SHORTER number to fix line better
               } else {
                  while(strlen(lpt) < MXPTHNM)
                     strcat(lpt," ");  // fill out to this column
                  strcat(lpt, NiceNumberStg( pll->dwSize, 12 ));
               }
            }

            if( g_ListDt ) {
               // ADD Date/Time
               strcat(lpt," ");
               strcat(lpt, FileTime2AscStg( &pll->sFD.ftLastWriteTime ) );
            }
         }
      }
   }
   else
   {
      if( bFullDir )
      {
         strcpy(lpt,pln);
      }
      else
      {
         strcpy(lpb, &pln[pll->ll_iPos]);    // offset to FULL NAME OF FILE
         TrimIB(lpb);
         strcpy(lpt,lpb);
         if( g_dwFullDir & bf_FullDir2 )
         {

         }
      }
      if(VERB2)
      {
         strcat(lpt, " [Folder???]" );
      }
   }

   strcat(lpt,MEOR);
   dwi = strlen(lpt);
   outit(lpt,dwi);
//                  gAdded, gDiscard,
//                  gdwLine2,
   gdwLines++; // another LINE to the OUTPUT

   if((VERB4) && !(VERB9) )
      sprtf(lpt);

}

BOOL IsSameFile( PLNLIST pll1, PLNLIST pll2 )
{
   PTSTR pln1 = &pll1->szLn[0]; // get pointer to line data
   PTSTR pln2 = &pll2->szLn[0]; // get pointer to line data
   PTSTR pf1 = &pln1[pll1->ll_iPos];    // offset to FULL NAME OF FILE
   PTSTR pf2 = &pln2[pll2->ll_iPos];
   if( strcmp(pf1,pf2) == 0 ) {
      // have the SAME name
      if( g_ListSz ) {
         // if listing the SIZE
         if( pll1->dwSize != pll2->dwSize ) {
            // and SIZE not equal
            return FALSE; // out this entry also
         }
      }

      if( g_ListDt ) {
         // if listing the DATE/TIME
         LONG lg = CompareFileTime( &pll1->sFD.ftLastWriteTime , &pll2->sFD.ftLastWriteTime );
         if( lg != 0 ) {
            // but DATE/TIME not equal
            return FALSE;
         }
      }

      return TRUE;
   }

   return FALSE;
}

VOID  Do_Dir_Out_If( PLE ph, PLNLIST plldir, PLNLIST pll )
{
   //PLE   ph = &sLines;
   PLE   pn;
   PLNLIST pll2;
   Traverse_List( ph, pn )
   {
      pll2 = (PLNLIST)pn; // cast to my structure
      if(( pll2 != pll               ) &&
         ( pll2->dwFlag & flg_IsFile ) &&
         ( pll2->dwFlag & flg_DnOut  ) ) {
         // we have OUTPUT this file
         // Is it the SAME file?
         if( IsSameFile( pll2, pll ) ) {
            g_dwDupsSkip++;
            return;  // ALL DONE with this entry
         }
      }
   }
   Do_Dir_Out( plldir, pll );
}

LPTSTR   chkstrcpyn( LPTSTR lpd, LPTSTR lps )
{
   if( strlen(lps) < 256 )
      return( strcpy(lpd, lps) );
   else
   {
      strncpy(lpd,lps,255);
      lpd[255] = 0;
      return lpd;
   }
}

LPTSTR   chkstrcpy( LPTSTR lpd, LPTSTR lps )
{
   return( strcpy(lpd,lps) );
}

static TCHAR _s_szActPath[264] = { "\0" };   // keep the ACTIVE PATH
// to be COPIED to each FILE for a FASTER run through the list
///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Do_Dir = due to Fix_DIR flag in Pgm_Flag - from -w...
// Return type: BOOL 
// Arguments  : LPTSTR lpb
//            : DWORD dws
//            : LPTSTR lpf
// Description: Process a flag if( Pgm_Flag & Fix_DIR ). ie -w switch
//              
///////////////////////////////////////////////////////////////////////////////
BOOL    g_bIsNTLine = FALSE; // Start NOT NT/XP line
BOOL IsNTLine(PTSTR ps)
{
    if( ( ISNUM(ps[0]) ) &&
        ( ISNUM(ps[1]) ) &&
        ( ps[2] == '/' ) &&
        ( ISNUM(ps[3]) ) &&
        ( ISNUM(ps[4]) ) &&
        ( ps[5] == '/' ) &&
        ( ISNUM(ps[6]) ) &&
        ( ISNUM(ps[7]) ) &&
        ( ISNUM(ps[8]) ) &&
        ( ISNUM(ps[9]) ) &&
        ( ps[10] == ' ') )
    {
        return TRUE;   
    }

    return FALSE;
}

PTSTR strcpychk(PTSTR pd, PTSTR ps)
{
    return( strcpy(pd,ps) );
}

VOID chkme2(PLNLIST pll)
{
    if(pll->dwFlag == 0)
        chkme("No FLAG for [%s]"MEOR, pll->szLn);
}


TCHAR szFls1[] = " file(s) ";
TCHAR szFls2[] = " File(s) ";
TCHAR szDrs1[] = " dir(s)";
TCHAR szDrs2[] = " Dir(s)";
TCHAR szTHr1[] = "Total files listed:";
TCHAR szTHr2[] = "Total Files Listed:";

BOOL g_bGotType = FALSE;
PTSTR g_pFls = szFls1;
PTSTR g_pDrs = szDrs1;
PTSTR g_pTHr = szTHr1;
BOOL SetType1(VOID)
{
    g_pFls = szFls1;
    g_pDrs = szDrs1;
    g_pTHr = szTHr1;
    g_bGotType = TRUE;
    return TRUE;
}
BOOL SetType2(VOID)
{
    g_pFls = szFls2;
    g_pDrs = szDrs2;
    g_pTHr = szTHr2;
    g_bGotType = TRUE;
    return TRUE;
}

BOOL IsTHeader( PTSTR ps )
{
    if( g_bGotType ) {
        if( InStr(ps, g_pTHr) )
            return TRUE;
    } else {
        if( InStr(ps, szTHr1) )
            return( SetType1() );
        else if( InStr(ps, szTHr2) )
            return( SetType2() );
    }
    return FALSE;
}

BOOL IsTotal( PTSTR ps )
{
    if( InStr(ps, " bytes") ) {
        if( g_bGotType ) {
            if( InStr(ps, g_pFls) )
                return TRUE;
        } else {
            // NOT got TYPE yet
            if( InStr(ps, szFls1) ) {
                return( SetType1() );
            } else if( InStr(ps, szFls2) ) {
                return( SetType2() );
            }
        }
    }
    return FALSE;
}

BOOL IsTail( PTSTR ps )
{
    if( InStr(ps, " free") ) {
        if( g_bGotType ) {
            if( InStr(ps, g_pDrs) )
                return TRUE;
        } else {
            // NOT got TYPE yet
            if( InStr(ps, szDrs1) ) {
                return( SetType1() );
            } else if( InStr(ps, szDrs2) ) {
                return( SetType2() );
            }
        }
    }
    return FALSE;
}

BOOL	Do_Dir( LPTSTR pFile, DWORD dws, LPTSTR lpf )
{
	BOOL	      bRet = FALSE;
   DWORD       dwi, dwk, dwlns, dwblk, dwrank;
   INT         c;
   LIST_ENTRY  sLines;
   PLE   ph = &sLines;
   PLE   pn;
   LPTSTR      pbgn, ptmp, pln;
   PLNLIST     pll, plldir;
   DWORD       iPos, iCnt;
   DWORD       dwFlag, dwdcnt, dwfcnt, dwfcv;
   LPTSTR      lpt = &gszTmpBuf[0]; // setup a temp buffer = MXTMPB (1024)
   // PTSTR   pfls = szFls1;
   // PTSTR   pdrs = szDrs1;
   InitLList( ph );  // start with no lines    PLE   ph = &sLines;

   dwrank = 0;
   for( dwi = 0; dwi < dws; dwi++ )
   {
      if( pFile[dwi] == '\n' )
         dwrank++;
   }

//                  gAdded, gDiscard,
   gdwLine2 += dwrank;  // gdwLines

   if(VERB1) sprtf( "Processing [%s](%d lines) as a Directory File ..."MEOR, lpf, dwrank );

   if(VERB2) sprtf( "Moment - Collecting file lines ..."MEOR );

   iDirOf = strlen(szDirOf);
   dwk   = 0;
   dwlns = 0;
   dwblk = 0;
   dwFlag = 0;
   dwdcnt = 0;
   dwfcnt = dwfcv = 0;
   dwrank = 0;
   pbgn  = pFile; // set BEGIN of a LINE
   for( dwi = 0; dwi < dws; dwi++ )
   {
      ptmp = &pFile[dwi];
      c = *ptmp;
      if(( c == '\r' ) || ( c == '\n' ) )
      {
         // reached end of line
         dwk++;   // count this char
         dwi++;   // bump to next
         ptmp = &pFile[dwi];  // get potential begin pointer
         for( ; dwi < dws; dwi++ )  // but walk to begin of next line
         {
            c = pFile[dwi];   // get char
            if( ( c >= ' ' ) || ( c == '\t' ) )
            {
               ptmp = &pFile[dwi];  // get pointer to this new start
               dwi--;   // back up for the loop increment
               break;   // and out of inner loop
            }
            dwk++;   // count another
         }

         dwlns++;    // count another line
         pn = MALLOC( (sizeof(LNLIST) + dwk) );
         if(!pn)
         {
            chkme( "C:ERROR: Get line memory FAILED! (%d bytes)"MEOR, (sizeof(LNLIST) + dwk) );
            goto Tidy_Exit;
         }
         memset(pn, 0, (sizeof(LNLIST) + dwk));
         pll = (PLNLIST)pn;   // cast to my structure
         pll->dwLn   = dwlns;  // set the line number
         pll->dwOrd  = 0;      // no order yet
         pll->dwFlag = 0;     // no flags yet
         pln = &pll->szLn[0]; // get pointer to line data

         strncpy(pln, pbgn, dwk);   // copy in this length
         pln[dwk] = 0;        // and zero terminate
         dwk = RTrimIB(pln);
         if(dwk)  // if we have LENGTH
         {
            pll->dwLen = dwk;    // keep its new length
            if( VERB9 )
            {
               if( dwk < (MXTMPB - 16) )
                  sprtf( "%5d [%s]"MEOR, dwlns, pln );
               else
               {
                  LPTSTR lplf = ALeft(pln, (MXTMPB - 16));
                  sprtf( "%5d [%s]..."MEOR, lplf );
                  FLeft(lplf);
               }
            }

            iPos = InStr( pln, szDirOf ); // "Directory of "
            if( iPos )
            {
               pll->dwFlag |= flg_IsDir;  // just mark as DIRECTORY
               dwdcnt++;   // count a directory
               pll->ll_iPos = (iPos - 1) + strlen(szDirOf); // offset to FULL NAME OF DIRECTORY
               chkstrcpyn( _s_szActPath, &pln[pll->ll_iPos]  );   // keep the PATH
               strcpy( pll->szPath, _s_szActPath );   // keep the PATH
            }
            else if(( *pln == ' ' ) &&
               ( InStr(pln, " Volume in drive ") ) )
            {
               pll->dwFlag |= flg_IsHeaderV1;  // just mark as header
            }
            else if( (*pln == ' ' ) &&
               ( InStr(pln, " Volume Serial Number ") ) )
            {
               pll->dwFlag |= flg_IsHeaderV2;  // just mark as header
            }
            else if( (*pln == ' ' ) &&
               ( InStr(pln, " (Too many files, ") ) )  // "directory not sorted)"
            {
               pll->dwFlag |= flg_IsHeaderTo;  // just mark as header
            }
            else if( InStr(pln, " <DIR> ") )
            {
               pll->dwFlag |= flg_IsFolder;  // just mark as folder
            }
            else if( IsTotal(pln) ) // InStr(pln, pfls) && InStr(pln, " bytes") )
            {
                // FIX20040901 - Include NT/XP listing
                // eg 453 File(s)    459,157,953 bytes" vs
                // eg 453 file(s)    459,157,953 bytes"
               pll->dwFlag |= flg_IsTotal;  // just mark as TOTAL
            }
            else if( IsTail(pln) ) // InStr(pln, pdrs) && InStr(pln, " free") )
            {
                // FIX20040901 - Include NT/XP listing
               pll->dwFlag |= flg_IsTail;  // just mark as TAIL of listing
            }
            else if( IsTHeader(pln) ) // was ( *pln == 'T' ) && ( InStr(pln, "Total files listed:") ) )
            {
               pll->dwFlag |= flg_IsTHeader;  // just mark as Theader
            }
            else
            {
               INT   i,k;
               // should be a file
               // setup WIN32_FIND_DATA
               // FIX20040901 - Must include NT/XP file listing, like ...
               // "29/01/2000  07:35 PM        18,157,193 Win95-98US.zip"
               if( dwfcnt == 0 ) {
                   g_bIsNTLine = IsNTLine(pln);
               }
               dwfcnt++;
               strcpy(lpt, pln);    // copy the line
               k = 0;
               iCnt = 0;
               pbgn = lpt;
               if( g_bIsNTLine ) {
                   /// deal with new NT/XP dir line
                   // what to do about ALTERNATE name???
                   pll->sFD.cAlternateFileName[0] = 0; // set NONE
                   pll->dwOK = Asc2FileTime( pbgn, &pll->sFD.ftLastWriteTime );
                   if( pll->dwOK > 0 ) {
                       iCnt = pll->dwOK;
                       pbgn += iCnt; // BUMP to END OF DATE/TIME
                       while( *pbgn && ( *pbgn == ' ' ) ) {
                           pbgn++;
                           iCnt++;
                       }
                       if( ISNUM(*pbgn) ) {
                          LPTSTR   p = FixNum(pbgn); // remove any commas
                          //__int64 i64 = _atoi64(p);
                          __int64 i64 = atoi(p);
                          LARGE_INTEGER li; // = _atoi64(p);
                          li.QuadPart = i64;
                          pll->dwSize = li.LowPart; // atoi(p);
                          pll->sFD.nFileSizeLow = pll->dwSize;
                          pll->sFD.nFileSizeHigh = li.HighPart;
                          while( *pbgn && (ISNUM(*pbgn) || (*pbgn == ',')) )
                          {
                             pbgn++;
                             iCnt++;
                          }
                          while( *pbgn && (*pbgn <= ' ') )
                          {
                             pbgn++;
                             iCnt++;
                          }
                          if( *pbgn ) {
                            dwrank++;
                            iPos = iCnt;
                            pll->dwRank = dwrank;   // order FOUND in FILE
                            pll->ll_iPos = iPos;    // offset to FULL NAME OF FILE
                            strcpychk( pll->sFD.cFileName, &pln[iPos] );
                            pll->dwFlag |= flg_IsFile;    // flag a VALID file
                            dwfcv++;
                            strcpy( pll->szPath, _s_szActPath );   // keep the PATH
                          }
                       }
                   }
                   if( pll->dwFlag == 0 )
                       chkme2(pll);
               } else {
                   // OLD win98 line
                   for( i = 0; i < 14; i++ )
                   {
                      if(pbgn[i] > ' ')
                         pll->sFD.cAlternateFileName[k++] = pbgn[i];
                      else if(k)
                      {
                         if( iCnt == 0 )
                         {
                            if( pll->sFD.cAlternateFileName[k-1] != '.' )
                            {
                               pll->sFD.cAlternateFileName[k++] = '.';
                            }
                            iCnt++;
                         }
                      }

                      if(k >= 14)
                         break;
                   }

                   //MaxTrim(lpt);        // remove ALL tab/double spaces, etc
                   iCnt = 12;
                   pbgn = &lpt[iCnt];     // get past DOS 8+Sp+3 name
                   while( *pbgn && !ISNUM(*pbgn) )
                   {
                      pbgn++;
                      iCnt++;
                   }
                   if( ISNUM(*pbgn) )
                   {
                      LPTSTR   p = FixNum(pbgn);
                      pll->dwSize = atoi(p);
                      pll->sFD.nFileSizeLow = pll->dwSize;
                      while( *pbgn && (ISNUM(*pbgn) || (*pbgn == ',')) )
                      {
                         pbgn++;
                         iCnt++;
                      }
                      while( *pbgn && (*pbgn <= ' ') )
                      {
                         pbgn++;
                         iCnt++;
                      }
                      // SYSTEMTIME
                      pll->dwOK = Asc2FileTime( pbgn, &pll->sFD.ftLastWriteTime );
                      if(pll->dwOK)
                      {
                         pbgn[pll->dwOK] = 0;
                         iPos = InStrIS(pln, pbgn);
                         if(iPos)
                         {
                            iPos--;
                            p = pln;
                            p = &pln[iPos];
                            iPos += pll->dwOK;
                            p = &pln[iPos];
                            //iPos += iCnt;
                            //p = &pln[iPos];
                            while( *p && (*p <= ' '))
                            {
                               iPos++;
                               p++;
                            }
                            dwrank++;
                            pll->dwRank = dwrank;   // order FOUND in FILE
                            pll->ll_iPos = iPos;    // offset to FULL NAME OF FILE
                            strcpy( pll->sFD.cFileName, &pln[iPos] );
                            pll->dwFlag |= flg_IsFile;    // flag a VALID file
                            dwfcv++;
                            strcpy( pll->szPath, _s_szActPath );   // keep the PATH
                         }
                      }
                   }
               } // new NT/XP line or OLD WIN98 LINE
            }

            dwFlag |= pll->dwFlag;  // add it to ALL flag

            InsertTailList(ph,pn);  // insert into the LIST
         }
         else  // a blank line
         {
            dwblk++;    // count a BLANK line - NOTE:This is perhaps NOT ALL blanks!!!
            MFREE(pn);  // and toss this memory
         }
         pbgn = ptmp;   // get begin of next line
         dwk = 0;       // restart a line
      }
      else
      {
         dwk++;         // bump the NOT CR/LF count
      }
   }  // for the LENGTH of the BUFFER

   dwk = (dwlns - dwblk);
   if( !dwk )
   {
      //if(VERB1)
      sprtf( "File is EMPTY!"MEOR, lpf );
      goto Tidy_Exit;
   }

   if(VERB9)
      ShowFlag( dwFlag );

   //if( (dwFlag & flg_Min) != flg_Min ) = FIX20010910 - This NOT correct
   if(( dwFlag & flg_IsHeaderV ) &&    // if it has 1 or both "Volume" header(s)
      ( dwFlag & flg_IsDir     ) &&    // at least ONE "Directory of"
      ( dwFlag & flg_IsTHeader ) )     // at least ONE "Total Files Listed:"
   {
      // then we will ASSUME is a DIRECTORY LISTING
      dwFlag |= flg_OK;
   }
   else if(( g_dwFullDir & bf_TryHard ) &&
      ( dwFlag & flg_IsDir ) &&  // at least one "Directory of "
      ( dwFlag & flg_IsFile ) )    // flag a VALID file
   {
      // then we will ASSUME it is a DIRECTORY LISTING
      dwFlag |= flg_OK;
   }
   else
   {
      // does NOT look good enough
      sprtf( "File does not appear to be a Directory List!"MEOR, lpf );
      goto Tidy_Exit;
   }

   if(VERB2) sprtf( "Moment - Sorting %d file lines ...(of %d)"MEOR, dwfcv, dwfcnt );

   if(VERB4 && (dwfcv != dwfcnt) )
   {
      Traverse_List( ph, pn )
      {
         pll = (PLNLIST)pn;   // cast to my structure
         if( pll->dwFlag == 0 )
         {
            pln = &pll->szLn[0]; // get pointer to line data
            sprtf( "Failed on [%s]"MEOR, pln );
         }
      }
   }

//   SortDirList( ph, dwfcv );
   OrderDirList2( ph, dwfcv );   // set the dwRank for OUTPUT order

   iCnt = 0;

   // ******* LOOP to do OUTPUT of each FILE entry, giving the DIRECTORY *******
   // =========================================================================
Next_Ord:

   iCnt++;

   plldir = 0;
   // NOTE: list is in order FOUND IN FILE 
   // output is per RANK done from SORT
   Traverse_List( ph, pn )
   {
      pll = (PLNLIST)pn;   // cast to my structure
      //pln = &pll->szLn[0]; // get pointer to line data
      //iPos = InStr(pln, "Directory of ");
      if(pll->dwFlag & flg_IsDir)
      {
         plldir = pll;  // keep 'last' directory pointer
      }
      else if( ( pll->dwFlag & flg_IsFile ) &&
         !( pll->dwFlag & flg_DnOut ) )
      {
         // it should be a file
         if( pll->dwOrd == iCnt )
         {
            if( g_ListDup ) {
               Do_Dir_Out( plldir, pll );
            } else {
               Do_Dir_Out_If( ph, plldir, pll ); // conditional OUTPUT
            }
            pll->dwFlag |= flg_DnOut;
            goto Next_Ord;
         }
      }
   }
   // =========================================================================
   // ******* DONE LOOP                                                 *******

   if( iCnt != (dwfcv+1) )
   {
      //if(VERB2)
      sprtf( "WARNING: Appear to have missed %d!"MEOR, (dwfcv - iCnt) );
      if(VERB2)
      {
         Traverse_List( ph, pn )
         {
            pll = (PLNLIST)pn;   // cast to my structure
            if(pll->dwFlag & flg_IsDir)
            {
               plldir = pll;  // keep directory pointer
            }
            else if(( pll->dwFlag & flg_IsFile ) &&
                   !( pll->dwFlag & flg_DnOut  ) )
            {
               // it should be a file
               Do_Dir_Out( plldir, pll );
               pll->dwFlag |= flg_DnOut;
            }
         }
      }
   }

   if( !g_ListDup && g_dwDupsSkip ) {
      if( VERB2 ) {
         sprtf( "NOTE: Skipped %d duplicate entries..."MEOR, g_dwDupsSkip );
      }
   }

   bRet = TRUE;

Tidy_Exit:

   KillLList( ph ); //    PLE   ph = &sLines;

	return bRet;
}

// eof - FixFDir.c
