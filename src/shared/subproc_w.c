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

int subproc_init(SubProc *self, const char *path, char *const argv[])
{
}

int subproc_status(SubProc *self)
{
}

void subproc_fini(SubProc *self)
{
}

void subproc_on_exit(int status, void *arg)
{
}

void subproc_signal(SubProc *self)
{
}


#endif
#endif
