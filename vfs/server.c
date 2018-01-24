#include <unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include "cpu.h"


int main(int argc, char *argv[])
{
	int shmid;
	io = shalloc(&shmid, sizeof(SharedMem));
	
	printf("Server Mem %p @ %d\n", io, shmid);
	
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
		printf("Forked\n");
		//~ init_platform();
//		init_gl();
		//~ GLFWvidmode*m = glfwGetVideoMode(glfwGetPrimaryMonitor());
		//~ printf("%dx%d (%d,%d,%d) %d",m->width, m->height, m->redBits, m->greenBits, m->blueBits, m->refreshRate);
		//~ render_loop(1.0/59);
		//~ char data[] = {100,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31, 32, 33,34,35};
		//~ int type=0;
		int child_term = 0;
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

			printf("%d ", io->screen[0]);
			fflush(stdout);
			usleep(500000);
		}
		kill(pid, SIGKILL);
		waitpid(-1, NULL, 0);
		death(NULL, NULL, 0);
	}
}
