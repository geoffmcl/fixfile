
// FixFVers.h
#ifndef	_FixFVers_H
#define	_FixFVers_H
/* ======= From original ASM FIXFILE.ASM ========
;------------------------------------------------------------------------------
;	Informatique Rouge
;	Commenced 1989 to 94,95,96,97,98,99,2000,1,2,3,4,5...2012...2020
;	Geoff R. McLane: mailto: GeoffAir@hotmail.com
;------------------------------------------------------------------------------
   ============================================================================ */
//;------------------------------------------------------------------------------
//;	Modification History - growing UPWARDS
//;------------------------------------------------------------------------------
#define  VersNum1    2
#define  VersNum2    0
#define  VersNum3    0   // FIX20200611 - make into github repo
#define  VerDate	"11 June, 2020" // in Dell03 - MSVC16 2019 x64

//  VersNum3    28   // FIX201227 - to -ud also remove any UTF-8 BOM
//  VerDate	"27 March, 2012" // in Win7-PC - MSVC10 ...

// VersNum3    27   // FIX20080425 - add -ud to conv to unix and '\' to '/'
// VerDate	"25 April, 2008" // in DELL01 - MSVC8 ...

// VersNum3    26   // FIX20070626 - add -hn to put tags on a new line
//  VerDate	"26 June, 2007" // in DELL01 - MSVC8 ...

// VersNum3    25  // FIX20070101 - fix -r (remove text) and -R (remove LINE)
// VerDate	"1 January, 2007" // also first in MSVC8 ...

// VersNum3    24  // FIX20061230 - fix -u - unix file endings only
//  VerDate	"30 December, 2006" // also first in MSVC8 ...

// FIX20051205 - add delete hi-bit characters - only ASCII output
// dmsg(	" -dh[s]     Delete hi-bit characters -dhs substitute a space."MEOR );
//  VersNum3    23  // FIX20050203 - add -dh[s] to DELETE hi-bit chars, subst space
//  VerDate	"5 December, 2005" // also first in MSVC7.1 ...

// VersNum3    22  // FIX20050203 - add -c - output C/C++ text form
// VerDate	"3 February, 2005" // hopefully ...
#define ADD_TXT2VC // FIX20050203 - add -c - output C/C++ text form

// VersNum3    21  // FIX20041120 improve -w (directory/folder) output...
// VerDate	"20 November, 2004" // get largest length, and size, and format output
//  VersNum3    20  // to PRO-1 FIX20041107 add to html...
//  VerDate	"7 November, 2004" // to FixF-15.zip

// VersNum3    19  // move back to Antony Unika machine...
// VerDate	"1 September, 2004" // from FixF-13.zip
#define __RPCASYNC_H__ // kill this include

//  VersNum3    18  // -hw, added table, tr, and td ...
//  VerDate	"22 June, 2004"
//  VersNum3    17  // more fixes and fixes
//  VerDate	"28 April, 2004"
#define  ADDJOBS1   // only when completed - 22 Jan. 2004 Fin ???
// VersNum3    16  // add -jw68fesd & -jotempjobs.txt
// VerDate	"27 March, 2004"  // see FixJobs.c

// VersNum3    15  // add -j to massage job files to csv
// VerDate	"22 January, 2004"  // see -j
#define  ADDGEOFF1   // only when completed - 07 Dec. 2003 Fin ???
// VersNum3    14  // add -g1 to massage special files
// VerDate	"7 December, 2003"  // see -g1
//  VersNum3    12  // add -wx[d|f]:one;two... to EXCLUDE DIR or FILES
//  VerDate	"1 September, 2003"  // see -w? help
//#define  ADD_WX_CMD     // add -wx[d|f]
#undef  ADD_WX_CMD     // OFF, until devel complete and tested

// VersNum3    11  // -w small fix in output of sorted list
// VerDate	"10 August, 2003"  // see -w? help

// VersNum3    10  // -f bumped all/most buffers to 4096 byts
// VerDate	"28 August, 2002"  // see FixFHelp for -f:? complex switch

// VersNum3    9  // -w = DIR to list - additions and fixes
// VerDate	"9 April, 2002"  // see FixFHelp, or -w?!
//  VersNum3    8  // -h = remove html fixes
//  VerDate	"1 April, 2002"  // april fools day - good for progs!

// VersNum3    7  // start -g = process my phone.txt file
// VerDate	"9 February, 2002"  // *** TBC ***
#define  ADDGEOFF2   // only when completed - 6 March, 2002???

//  VersNum3    6  // fix TAB replaced with ^I
//  VerDate	"27 January, 2002"  // this meant
// change line 846 = if( ( c < ' ' ) && ( c != 't' ) ) - 27JAN2002 - think this should be
// to = if( ( c < ' ' ) && ( c != '\t' ) )  // ie is LESS than SPACE, and NOT TAB

//  VerDate	"10 September, 2001"  // fix -w - dir list to simple list
//  VersNum3    5  // tidy up HELP strings
//  VerDate	"31 August, 2001"  // add '@' before hi-bit and reduce char
// VersNum3    4  // add g_dwMaxLine and Fix_LS functionality
// VersNum1    1
// VersNum2    0
//; VersNum3      1       // add version number - 29/12/98 grm
//;  add work on DIR listing 2000/01/13 - grm
//; VersNum3                EQU             2t
//; VersNum3    3

#define  CoLogo   "Informatique Rouge"
#define  PgmPurp  "FIX FILES"
#define  PgmName  "FIXF32"
// As an ASM 16-bit program = FixFile
//;<'24rd May, 1994'>	-	Initial release	Geoff R McLane
//;<'31st May, 1994'>	; Fix for add when CR added	grm
//;<'3rd October, 1994'>	; Add -R<text> for remove line with
//; and @Input file for LONG inputs - Too big for DOS ...
//;<'4th October, 1994'>	; Add to -D removing wholly BLANK LINES
//;<'7th October, 1994'>	; Add to -S to SPLIT OUTPUT FILED grm
//;<'15th November, 1994'>	; Do pre-check for -v[?] switch - grm
//;<'19th November, 1994'>	; -p Paragraphs ie remove Cr/Lf - NOT COMPLETE
//;<'1st December, 1994'>	; -ls Correctly SPLIT lines at spaces
//;<'8th May, 1995'> ; -ls Make this DEFAULT and use 65 chars
//;<'9th Jan., 1996'> ; -h[x] HTML Removal
//;<'10th Jan., 1996'> ; Fix for HTML Removal
//;<'18th Feb., 1996'> ; -RR[Find string][ ]Replace string
//;<'25th April, 1996'> ; Allow -l as 0 = Disable length, and
//; changed closing HTML must be >20H+'>' - See [_HTML_Pre] ...
//;<'12th November, 1996'> ; Def is DELETE Input file, and
//; put any HTML file into the TEMP environment ...
//;<'15th March, 1997'> ; Add -ds[n] Del space at beginning line
//;<'29th December, 1998'> ; Add version number after this date
//;<'13th January, 2000'> ; Add fix of DIR listing
// As 'C' CONSOLE 32-bit program = FixF32
// VerDate	"20 February, 2001"  // convert to 32-bits
//  VerDate	"12 May, 2001"  // added g_dwMaxLine and Fix_LS functionality
//  VerDate	"19 July, 2001"  // tidy up HELP messages (if bRedON send to BOTH)
//  VerDate	"21 August, 2001"  // add -h2a - show HEX as ASCII

#define     MXARGS      256
#define     MXARGSS     ( MXARGS * sizeof(PVOID) )
#define	   DEF_VERB    1

//#define     MALLOC   malloc
//#define     MFREE    free
#define     MALLOC(a)   LocalAlloc(LPTR,a)
#define     MFREE(a)    LocalFree(a)

//#define		MXINPS		256 // FIX20050205 removed
//#define		MAXCOL		78
//#define		MINCOL		65
//;;;MXLINE	equ	80		; Maximum line length
//;MXLINE	equ	65		; Maximum (DEFAULT) line length - 8May95 - grm
// MXLINE	72		//= g_dwMaxLine; Maximum (DEFAULT) line length - 10May01 - grm
// wit better windows editors, this DOS 80 character limit is somewhat outmoded!
// #define     MXLINE   256   // FIX20050205 - bump this up
#define     MXLINE   1024   // FIX20070101 - bump this up

#define     MEOR     "\r\n"
//;	FEATURES
#define  RemCRLF  1  //; -d  = remove duplicate CR/LF characters
#define  AddMult	1  //; Add multiple input files from a list file
#define  AddRem   1  //; -R[...] = Add REMOVE LINE feature ...
#define  AddSplit 1  //; Add SPLIT OUTPUT FILE ...
#define  AddPara	0  //; Add -p[aragraphs] option ... NOT COMPLETED - grm
#define  AddLS    1  //; Add -ls to correct line split
#define  RemHTML	1  //; Add -h[x] to REMOVE HTML stuff from a file.
#define  AddDS	   1  //; Add -ds[nn] to REMOVE data from beginning of line
#define  AddDIR	1  //; Add -w switch
#define  AddFunc  1  // Add -f Function List Etraction

#define  MXFILNAM	   264   //; Full file name buffer ...

//; ============================================================
//;	Values for Pgm_Flag (and previous Pgm_Flag2)
//;	===================
#define  VerbON   0x1
#define  FileIN	0x2
#define  GotAppe	0x4  //	; We are APPENDING to a file
#define  Out_Fil	0x8  //	; Given an OUTPUT file ...
#define  Rem_CRLF	 0x10 //	; Add duplicate Cr/Lf removal (inc ' ')
#define  Mul_Files 0x20 //		; We have multiple file
#define  BS_Dup	 0x40 //		; Suppress char duplication with BS
#define  Tab_SP	 0x80 //		; Convert TABS to spaces
#define  Do_Unix	 0x100   //		; Remove ALL CR characters for output
#define  Gt_Rem	 0x200   // 	; We have a REMOVE LINE file ..
#define  Do_Split  0x400   //		; -s[nnn] SPLIT OUTPUT FILE ...
#define  Fix_PARA  0x800   //		; -p Paragraphs (Contr. -d Rem_CRLF)
#define  Fix_LS	 0x1000  //		; -ls Correct line split
#define  Rem_HTML	 0x2000  //		; -h Delete HTML data ...
#define  Rem_SUBS	 0x4000  //		; -R[emove] lines with this ...
#define  Rem_DIAG	 0x8000  //		; -R+[ but don't delete the file ...
//; ============================================================
#define  Rem_DS     0x00010000
#define  Fix_DIR    0x00020000 //	; -w Convert DIR listing
#define  Do_Funcs   0x00040000  // -f = Function List Extraction
#define  Show_Hex    0x00080000  // -h2a - show HEX to ASCII conversion
#define  Do_Geoff    0x00100000  // -g - process phone file
#define  Do_Geoff1   0x00200000  // -g1 - process speciql file to CSV output
#define  Do_Jobs     0x00400000  // -j process job file
#define  Do_JobsF    0x00800000  // -jf fix job file
#define  Do_JobsE    0x01000000  // -je fix email tags
#define  Do_JobsS    0x02000000  // -js keep spaces
#define  Do_JobsW    0x04000000  // -jw[nn] def=40
#define  Do_JobsD    0x08000000  // -jd delete header/tail from copy/paste
#define  Do_JobsT    0x10000000  // -jt delete trailing "<"
#define  Do_JobsDP   0x20000000  // -jd+ write delete header/tail to file
#define  Add_HTML    0x40000000  // -htm = add HTML to output
#define  VC_Out      0x80000000  // -c #ifdef ADD_TXT2VC // FIX20050203 - add -c - output C/C++ text form

#define DEF_WRAP_LEN 120 // 78 .. 68 // DWORD g_dwWrap

//;;;Def_Flag	equ	VerbON or Fix_LS or Rem_DIAG	; DEFAULT FLAG
// #define  Def_Flag ( VerbON | Fix_LS )   //	; DEFAULT FLAG
#define  Def_Flag    VerbON   //	; DEFAULT FLAG

#endif	// _FixFVers_H
// eof - FixFVers.h
