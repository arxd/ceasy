#ifndef SHARE_MEM_W_C
#define SHARE_MEM_W_C

#include <stdint.h>

typedef struct s_SharedMem SharedMem;

struct s_SharedMem {
	uint64_t shmid;
	void *mem;
};

int shm_init(SharedMem *self, int size);
void shm_fini(SharedMem *self);
void shm_on_exit(int status, void *arg);

#if __INCLUDE_LEVEL__ == 0 || defined(NOLIB)

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

// #define BUF_SIZE 256
// TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
// TCHAR szMsg[]=TEXT("Message from first process.");

// int _tmain()
// {
   // HANDLE hMapFile;
   // LPCTSTR pBuf;

   // hMapFile = CreateFileMapping(
                 // INVALID_HANDLE_VALUE,    // use paging file
                 // NULL,                    // default security
                 // PAGE_READWRITE,          // read/write access
                 // 0,                       // maximum object size (high-order DWORD)
                 // BUF_SIZE,                // maximum object size (low-order DWORD)
                 // szName);                 // name of mapping object

   // if (hMapFile == NULL)
   // {
      // _tprintf(TEXT("Could not create file mapping object (%d).\n"),
             // GetLastError());
      // return 1;
   // }
   // pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
                        // FILE_MAP_ALL_ACCESS, // read/write permission
                        // 0,
                        // 0,
                        // BUF_SIZE);

   // if (pBuf == NULL)
   // {
      // _tprintf(TEXT("Could not map view of file (%d).\n"),
             // GetLastError());

       // CloseHandle(hMapFile);

      // return 1;
   // }


   // CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
    // _getch();

   // UnmapViewOfFile(pBuf);

   // CloseHandle(hMapFile);

   // return 0;
// }


int shm_init(SharedMem *self, int size)
{
	HANDLE hMapFile;
	
	if (size) {
		static int name_int = 100;
		memset(self, 0, sizeof(SharedMem));
		self->shmid = ++name_int;
		char name[256];
		snprintf(name, 256, "Global\\Shm%d", self->shmid);
		printf("Creating shared mem %s\n", name);
		hMapFile = CreateFileMapping(
			INVALID_HANDLE_VALUE,    // use paging file
			NULL,                    // default security
			PAGE_READWRITE,          // read/write access
			0,                       // maximum object size (high-order DWORD)
			size,                // maximum object size (low-order DWORD)
			name);                 // name of mapping object
		printf("%p\n", hMapFile);
		if (hMapFile == NULL){
			_tprintf(TEXT("Could not create file mapping object (%d).\n"), GetLastError());
			return 0;
		}
		// Attach
		
		self->mem = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS,0,0,size);
		if (self->mem == NULL) {
			_tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
			CloseHandle(hMapFile);	
			return 0;
		}
		self->shmid = (uint64_t)hMapFile;
		
	} else {
		printf("CLIENT\n");
	}
	return 1;
}

void shm_fini(SharedMem *self)
{
	if (self->mem) {
		printf("Detach mem segment");
		UnmapViewOfFile(self->mem);
	}
	if (self->shmid) {
		printf("Release shmid");
		CloseHandle((HANDLE)self->shmid);
	}
	
	memset(self, 0, sizeof(SharedMem));
}

void shm_on_exit(int status, void *arg)
{
	shm_fini((SharedMem*)arg);
}


#endif
#endif
