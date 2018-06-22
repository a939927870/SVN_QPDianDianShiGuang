
#include "Func.h"
#if _LINUX

int openDevNull(int fd) {
    int tmpfd;
    close(fd);
#if defined(__WIN32)
    /* Cygwin should work with /dev/null */
    tmpfd = open("nul", O_RDWR);
#else
    tmpfd = open("/dev/null", O_RDWR);
#endif
    if (tmpfd != -1 && tmpfd != fd) {
        dup2(tmpfd, fd);
        close(tmpfd);
    }
    return (tmpfd != -1) ? 0 : -1;
}

void daemonize(char *dir) {
    if (0 != fork()) exit(0);

    if (-1 == setsid()) exit(0);

    if (0 != fork()) exit(0);

    if (0 != chdir(dir)) exit(0);

        openDevNull(STDIN_FILENO);
        openDevNull(STDOUT_FILENO);
        openDevNull(STDERR_FILENO);
}

void SetCoreFileUnlimit()
{
	struct rlimit rlim;
	struct rlimit rlim_new;
	if(getrlimit(RLIMIT_CORE, &rlim) == 0) {
		rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
		if(setrlimit(RLIMIT_CORE, &rlim_new) != 0) {
			rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
			setrlimit(RLIMIT_CORE, &rlim_new);	
		}

	}

}


void SetProcessStackSize(size_t size)
{
	struct rlimit rlim;
	struct rlimit rlim_new;
	if(getrlimit(RLIMIT_STACK, &rlim) == 0) {
		if(size > 0) {
			rlim_new.rlim_cur = rlim_new.rlim_max = size;
		} else {
			rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
		}
		if(setrlimit(RLIMIT_STACK, &rlim_new) != 0) {
			rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
			setrlimit(RLIMIT_STACK, &rlim_new);	
		}

	}
}

void SetSockFileSize(size_t size)
{
	struct rlimit rlim;
	struct rlimit rlim_new;
	if(getrlimit(RLIMIT_NOFILE, &rlim)  == 0) {
		if(size > 0) {
			rlim_new.rlim_cur = rlim_new.rlim_max = size;
		} else {
			rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
		}
		if(setrlimit(RLIMIT_NOFILE, &rlim_new) != 0) {
			rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
			setrlimit(RLIMIT_NOFILE, &rlim_new);	
		}
	}
}


int WriteProcessPID(char *dir)
{
	char pidfile[512] = {0};
	int n;
	pid_t pid = getpid();
	char pidbuff[64] = {0};

	n = snprintf(pidfile, 512, "%s/server.pid", dir);
	pidfile[n] = 0;

	n = snprintf(pidbuff, 64, "%d", pid);
	pidbuff[n] = 0;

	FILE *fp = fopen(pidfile, "w");
	if(!fp) return -1;

	fwrite(pidbuff, n, 1, fp);

	fclose(fp);

	return 0;
}
#endif



