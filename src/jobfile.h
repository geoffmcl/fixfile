// jobfile.h: interface for the jobfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JOBFILE_H__51ED1FA0_769C_11D8_936D_444553540001__INCLUDED_)
#define AFX_JOBFILE_H__51ED1FA0_769C_11D8_936D_444553540001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class jobfile  
{
public:
	void closemyfile(void);
	bool opnjfile( string s );
	FILE * pfile;
	string filename;
	jobfile();
	virtual ~jobfile();

};


#endif // !defined(AFX_JOBFILE_H__51ED1FA0_769C_11D8_936D_444553540001__INCLUDED_)

// eof - jobfile.h
