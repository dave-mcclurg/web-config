#ifndef GUICONSOLE_H
#define GUICONSOLE_H

#ifdef _DEBUG
void RedirectIOToConsole();
#endif

#endif

//#include <crtdbg.h>
//#ifdef _DEBUG
//RedirectIOToConsole();
//#endif
//
//// test CrtDbg output
//_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
//_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );
//_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
//_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDERR);
//_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
//_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDERR);
//_RPT0(_CRT_WARN, "This is testing _CRT_WARN output\n");
//_RPT0(_CRT_ERROR, "This is testing _CRT_ERROR output\n");
//_ASSERT( 0 && "testing _ASSERT" );
//_ASSERTE( 0 && "testing _ASSERTE" );
