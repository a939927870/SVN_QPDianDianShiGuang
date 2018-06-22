#ifndef MINIDUMP_H
#define MINIDUMP_H
//========================================================================
// Minidump.h : This is a crash trapper - similar to a UNIX style core dump
//========================================================================
#include "LBase.h"
#include <tchar.h>
#include <cstdio>
#include <crtdbg.h>
#include <TlHelp32.h>
#include "dbghelp.h"

#include <list>

class MiniDumper
{
public:
	MiniDumper(bool headless);

protected:
	static MiniDumper *gpDumper;
	static LONG WINAPI Handler( struct _EXCEPTION_POINTERS *pExceptionInfo );

	virtual void VSetDumpFileName(void);
	virtual MINIDUMP_USER_STREAM_INFORMATION *VGetUserStreamArray() { return NULL; }
	virtual const TCHAR *VGetUserMessage() { return _T(""); }

	_EXCEPTION_POINTERS *m_pExceptionInfo;
	time_t m_lTime;

	TCHAR m_szDumpPath[_MAX_PATH] = { 0 };
	TCHAR m_szAppPath[_MAX_PATH] = { 0 };
	TCHAR m_szAppBaseName[_MAX_PATH] = { 0 };
	LONG WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo );
	BOOL m_bHeadless;
};

#define USER_DATA_BUFFER_SIZE (4096)


#endif