#ifndef SUBPROC_C
#define SUBPROC_C


typedef struct s_SubProc SubProc;
struct s_SubProc {
	int pid;
	int status;
};

int subproc_init(SubProc *self, const char *path, char *const argv[]);
int subproc_status(SubProc *self);
void subproc_fini(SubProc *self);
void subproc_on_exit(int status, void *arg);
void subproc_signal(SubProc *self);

#if __INCLUDE_LEVEL__ == 0


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include "util.c"

int subproc_init(SubProc *self, const char *path, char *const argv[])
{
	memset(self, 0, sizeof(SubProc));
	self->pid = fork();
	if (self->pid < 0) {
		self->pid = 0;
		return -1;
	}
	if (self->pid > 0) {
		DEBUG("PID: %d", self->pid);
		return self->pid;
	}
	// we are the child process so exec
	execv(path, argv);
	ABORT(0, "The program '%s' could not be found", path);
}

int subproc_status(SubProc *self)
{
	if (self->status)
		return self->status;
	int wpid = waitpid(self->pid, &self->status, WNOHANG);
	if (wpid < 0)
		ABORT(1, "WaitPID Failed %d", wpid);
	if (wpid == 0)
		return 0;
	
	if (WIFEXITED(self->status))
		self->status =  1 + WEXITSTATUS(self->status); 
	else if (WIFSIGNALED(self->status))
		self->status = -WTERMSIG(self->status);
	else
		self->status = -1;
	return self->status;
}

void subproc_fini(SubProc *self)
{
	subproc_status(self);
	if (!self->status) {
		DEBUG("Killing child %d", self->pid);
		kill(self->pid, SIGKILL);
	} else if (self->status < 0)
		DEBUG("Child killed by signal %d", -self->status);
	else if (self->status == 1)
		DEBUG("Child exited normally");
	else
		DEBUG("Child exited with error %d", self->status-1);
	waitpid(self->pid, NULL, 0);
}

void subproc_on_exit(int status, void *arg)
{
	subproc_fini((SubProc*)arg);
}

void subproc_signal(SubProc *self)
{
	kill(self->pid, SIGUSR1);
}


#endif
#endif
