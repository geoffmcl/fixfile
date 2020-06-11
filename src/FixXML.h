
// FixXML.h
#ifndef _FixXML_h
#define _FixXML_h

// XML format
extern DWORD IsValidXMLChar( TCHAR c, DWORD type );
extern DWORD IsValidEscapeSeq( PTSTR pfnd );
extern DWORD ValidXMLMetaChar( PTSTR pb, DWORD dwn );
extern PTSTR GetXMLMetamp( VOID );
extern PTSTR GetXMLMetfor( PTSTR pDesc ); // care returns 0 if NOT FOUND
extern PTSTR GetXMLMeta4( TCHAR c ); // convert say ">" to "&gt;"

#endif // #ifndef _FixXML_h
// eof - FixXML.h
