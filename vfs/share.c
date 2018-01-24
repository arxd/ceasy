#ifndef SHARE_H
#define SHARE_H

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define BUF_SIZE (4)//(0x1000>>3)-1)

typedef unsigned long u64;

typedef struct s_IPC {
	volatile int f; // free start
	volatile int d; // data start
	volatile int cur;
	volatile int remain;
	u64 buf[BUF_SIZE];
} IPC;


typedef struct s_Package Package;
struct s_Package {
	int len;
	int type;
	//~ int remain;
	//~ int cur;
	union {
		void *pdata;
		u64 *ldata;
	};
	Package *next, *prev;
};

//~ void *shalloc(int *shmid, int size);
//~ void ipc_free_pkg(Package *pkg);
//~ void ipc_send(int len, int type, u64 *data);
//~ int ipc_connect(int *shmid, int parent);
//~ Package *ipc_recv_block(void);

//~ Package *ipc_recv(void);
//~ void ipc_yield(void);
//~ void ipc_debug(void);




IPC *g_send, *g_recv;
Package g_pkgs;
int g_connected;
int g_parent;


void death(const char *title, const char *err_str, int errid)
{	
	if (title)
		printf("%s:%d: %s\n", title, errid, err_str);
	//~ if (fs_window)
		//~ glfwDestroyWindow(fs_window);
	//~ if (w_window)
		//~ glfwDestroyWindow(w_window);
	//~ glfwTerminate();
	//~ if (io)
		//~ shfree(
		//~ shmctl (shmid, IPC_RMID, 0);
	exit(errid);

}




void print_stars(IPC *ipc)
{
	char c[] = {'_', 'X'};
	int s[] = {ipc->d, ipc->f};
	int p = (ipc->d > ipc->f);
	int i=-1;
	while (++i < BUF_SIZE)
		printf("%c", (i == ipc->f)? '.' : ((i < s[p]) ? c[p] : ((i < s[p^1])? c[p^1] : c[p])));
	printf(" %2d,%2d r:%d ", s[0], s[1], ipc->remain);
	if (ipc->remain > 5)
		exit(1);
}


void ipc_debug(void)
{
	printf("SEND: ");print_stars(g_send); printf("\nRECV: ");
	print_stars(g_recv); printf("\n");
	fflush(stdout);
	
	
}


void ipc_yield(void)
{
	sched_yield();
}

void *shalloc(int *shmid, int size)
{
	if (size) {
		*shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
		if (*shmid < 0)
			death("shmget", "", 2);
	}
	void *mem = shmat(*shmid, NULL,0);
	if (mem == (void*)-1)
		death("shmat", "" , 3);
	return mem;
}

//~ int ipc_connect(int shmid, int size)
//~ {
	//~ memset(&g_pkgs, 0, sizeof(Package));
	//~ g_pkgs.next = g_pkgs.prev = &g_pkgs;

	//~ g_parent = !!size;
	//~ g_connected = 1;
	//~ void *mem = shmat(shmid, NULL, 0);
	//~ if (mem == (void*)-1) {
		//~ mem = malloc(sizeof(IPC)*2);
		//~ g_connected = 0;
	//~ }
	//~ if (parent) {
		//~ memset(mem, 0, sizeof(IPC)*2);
		//~ g_send = (IPC*) mem;
		//~ g_recv = ((IPC*)mem)+1;
	//~ } else {
		//~ g_recv = (IPC*) mem;
		//~ g_send = ((IPC*)mem)+1;
	//~ }
	//~ return g_connected;
//~ }

static int pull()
{
	int f = g_recv->f;
	int d = g_recv->d;
	int len = f - d;
	//if(g_parent){printf("PULL %d\n",len);fflush(stdout);}
	if (!len)
		return 0;
	if (len < 0)
		len = BUF_SIZE - d;
	
	Package *pkg = g_pkgs.prev;
	if (g_recv->remain == 0) {
		//create a new package
		pkg = malloc(sizeof(Package));
		pkg->next = &g_pkgs;
		pkg->prev = g_pkgs.prev;
		pkg->prev->next = pkg;
		g_pkgs.prev = pkg;
		u64 hdr = g_recv->buf[d];
		pkg->len = (int)hdr;
		pkg->type = (int)(hdr>>32);
		g_recv->remain = ((pkg->len)>>3) + !!(pkg->len&0x7);
		g_recv->cur = 0;
		pkg->pdata = 0;
#ifdef DEBUG
		printf("R   : ");print_stars(g_recv);printf("   0/%2d    [%d/%c]\n", g_recv->remain, pkg->len, pkg->type+'A' );
		fflush(stdout);
#endif
		g_recv->d = (d+1)%BUF_SIZE;
		if (g_recv->remain)
			pkg->pdata = malloc(sizeof(u64)*g_recv->remain);

	} else {
		if (g_recv->remain < len)
			len = g_recv->remain;
#ifdef DEBUG
		printf("R   : ");print_stars(g_recv);printf("  %2d\n", len);
		fflush(stdout);
#endif
		g_recv->remain -= len;
		while (len--)
			pkg->ldata[g_recv->cur++] = g_recv->buf[d++];
		g_recv->d = (d == BUF_SIZE)? 0 : d;
	}
	
	pull();
}

static inline int get_free(void)
{
	int d = g_send->d;
	int f = g_send->f;
	return (d <= f) ? (BUF_SIZE - f + !!d) : d-f;
}

static inline void send1(u64 ebyte)
{
	int free;
	//~ if(g_parent){printf("Free %d, %d, %d (%d)\n", free, g_send->d, g_send->f, BUF_SIZE); fflush(stdout);}
	while ((free = get_free()) < 2) {
		pull();
		ipc_yield();
	}
	g_send->buf[g_send->f] = ebyte;
	g_send->f = (g_send->f == BUF_SIZE-1)? 0 : (g_send->f + 1);
}

void ipc_send(int len, int type, u64 *data)
{
	if (!g_connected)
		return;

	int tot = (len>>3)+!!(len&0x7);
#ifdef DEBUG
	printf("   S: ");print_stars(g_send);printf("   0/%2d    [%d/%c]\n", tot, len, type+'A');
	fflush(stdout);
#endif
	send1(((u64)type<<32) | len);
	if(!len)
		return;
	
	
	int free,nlong = (len>>3)+1;
	goto skip_first_yield;
	while (nlong) {
		pull();
		ipc_yield();
	skip_first_yield:
		free = get_free();
#ifdef DEBUG
		printf("   S: ");print_stars(g_send);printf("  %2d/%2d (%d)\n", (nlong-1), tot, free-1);
		fflush(stdout);
#endif
		int f = g_send->f;
		while (--free && --nlong)
			g_send->buf[f++] = *data++;
		g_send->f = (f==BUF_SIZE)?0:f;
	}
	// send the dregs
	int xtra = len&0x7;
	if (xtra) {
		u64 x = 0;
		memcpy(&x, ((char*)data)+(len-xtra), xtra);
#ifdef DEBUG
		printf("   S: ");print_stars(g_send);printf("  %2d/%2d 0x%lx\n", tot, tot, x);
		fflush(stdout);
#endif
		send1(x);
	}
	
}

Package *ipc_recv(void)
{
	pull();
	if (g_pkgs.next == &g_pkgs || (g_pkgs.next->next == &g_pkgs && g_recv->remain))
		return NULL;
	Package *ret = g_pkgs.next;
	ret->next->prev = &g_pkgs;
	g_pkgs.next = ret->next;
	return ret;
}
Package *ipc_recv_block(void)
{
	Package *pkg;
	while (!(pkg=ipc_recv()))
		sched_yield();
	Package *p = g_pkgs.next;
	int n=0;
	while (p != &g_pkgs) {
		n++;
		p = p->next;
	}
#ifdef DEBUG
	printf("GOT :   inqueue:%d  [%d/%c]\n", n, pkg->len, pkg->type+'A');
#endif	
	return pkg;
}

void ipc_free_pkg(Package *pkg)
{
	free(pkg->pdata);
	free(pkg);
}

#endif

