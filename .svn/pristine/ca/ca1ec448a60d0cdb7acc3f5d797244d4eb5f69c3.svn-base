#include "LBase.h"
#include "Work.h"
#if _LINUX
#include "../LBase/Func.h"
#else
#include "MiniDump.h"
#endif

#define STACK_SIZE (20*1024*1024)

int main(int argc, char *argv[])
{
#if _LINUX
	char strCurPath[512] = {0};
	getcwd(strCurPath, 512);
	chdir(strCurPath);
	SetCoreFileUnlimit();
	SetSockFileSize(0);
	SetProcessStackSize(STACK_SIZE);
	if(argc == 1) daemonize(strCurPath);
	int nRet = WriteProcessPID(strCurPath);
	if(nRet < 0) exit(0);

	//if(argc == 1) daemonize("/opt/CardServer/CenterServer");
#else
	MiniDumper minDump(true);
#endif
	gWork.Init();
	gWork.Start();
	gWork.Join();
	return 0;
}
