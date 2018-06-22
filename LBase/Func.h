#ifndef __FUNC_H_
#define __FUNC_H_

#if _LINUX
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>

void daemonize(char *dir); 
void SetCoreFileUnlimit();
void SetSockFileSize(size_t size); 
void SetProcessStackSize(size_t size);
int WriteProcessPID(char *dir);

#endif  //#if _LINUX

#endif
