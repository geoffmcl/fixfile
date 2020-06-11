// jobfile.cxx: implementation of the jobfile class.
//
//////////////////////////////////////////////////////////////////////
#ifdef  __cplusplus
#pragma message( "C++ is DEFINED ..." )
#else
#pragma message( "C++ is NOT defined ..." )
#endif  // __cplusplus

#ifdef   ADD_JOBFILE_CXX

#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;

//#include "stdlib.h"
#include "jobfile.h"

#ifdef   FIXED_CXX

#pragma warning( disable:4100 ) // unref warning
#pragma warning( disable:4511 ) // copy construct
#pragma warning( disable:4512 )
#pragma warning( disable:4663 )

#endif // #ifdef FIXED_CXX


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

jobfile::jobfile()
{
   cout << "jobfile instantiated ..." << endl;
   filename = "";

   pfile = 0;
}

jobfile::~jobfile()
{

}

bool jobfile::opnjfile(string s)
{
   filename = s;
   cout << "Openning file [" << filename << "]." << endl;
   pfile = fopen( s.c_str(), "r" );
   if(pfile != 0) {

      cout << "OK, open for reading ..." << endl;
      return true;
   }

   cout << "ERROR: Open FAILED!" << endl;
   return false;
}


void jobfile::closemyfile()
{
   if(pfile != 0) {

      cout << "Closing file [" << filename << "]." << endl;

      fclose(pfile);

      pfile = 0;

   }

}

bool ojf( char * pfn )
{
   jobfile * pjf = new jobfile;
   string s = pfn;

   if( pjf->opnjfile( s ) ) {


      pjf->closemyfile();

      delete pjf;
      return true;
   }



   delete pjf;
   return false;
}

#endif // #ifdef   ADD_JOBFILE_CXX
// eof - jobfile.cxx
