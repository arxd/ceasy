#include <unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include "cpu.h"
#include "window.h"
#include "glhelper.c"

void event_callback(int type, float ts, int p1, int p2)
{
	printf("EVENT %d\n", type);
	
	
}

int main(int argc, char *argv[])
{
	int shmid;
	uint8_t *mem = shalloc(&shmid, SCREEN_W*SCREEN_H+256*4 + sizeof(IPC)*2);
	
	int pid = fork();
	
	if (pid == 0) {
		char *prog = argv[1];
		int a;
		for (a = 0; a < argc-1; ++a)
			argv[a] = argv[a+1];
		argv[a] = NULL;
		argv[0] = alloca(10);
		snprintf(argv[0], 10, "%x", shmid);
		execv(prog, argv);
		printf("The program '%s' could not be found", prog);
		death(NULL, NULL, -128);
		
	} else {
		printf("PID %d\n",pid);
		io.screen = mem;
		io.palette = mem+SCREEN_W*SCREEN_H;
		IPC *ipc = (IPC*)(io.palette + 256*4);
		ipc_init(ipc, ipc+1);
		win_init(event_callback);
		
		int child_term = 0;
		double prev_t = win_time();
		while(!child_term)
		{
			
			int status, e;
			if ((e=waitpid(-1, &status, WNOHANG)) > 0) {
				if (WIFEXITED(status)) {
					printf("Child exited status : %d\n", WEXITSTATUS(status));
					child_term = 1;
				} else if (WIFSIGNALED(status)) {
					printf("Child exited by signal: %d\n", WTERMSIG(status));
					child_term = 1;
				} else {
					printf("Some other exit mode %d\n", status);
					child_term = 1;
				}
			} else if (e) {
				printf("WAITPID %d\n",e);
				child_term  = 1;
			}
			double t = win_time() - prev_t;
			prev_t += t;
			//~ printf("%f ", 1.0/t);
			//~ fflush(stdout);
			clear(0.0, 0.31, 0.23);
			win_update();
		}
		win_fini();
		kill(pid, SIGKILL);
		waitpid(-1, NULL, 0);
		death(NULL, NULL, 0);
	}
}
