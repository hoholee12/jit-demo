#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#define PAGE_SIZE 4096

//code + count = 4kb
struct asmbuf {
	uint8_t code[PAGE_SIZE - sizeof(uint64_t)];
	uint64_t count;
};

#ifdef _WIN32
#include<windows.h>
struct asmbuf* asmbuf_create(void){
	DWORD type = MEM_RESERVE | MEM_COMMIT;
	return VirtualAlloc(NULL, PAGE_SIZE, type, PAGE_READWRITE);
}

void asmbuf_finalize(struct asmbuf* buf){
	DWORD old;
	VirtualProtect(buf, sizeof(*buf), PAGE_EXECUTE_READ, &old);
}

void asmbuf_free(struct asmbuf* buf){
	VirtualFree(buf, 0, MEM_RELEASE);
}

#else
#include<sys/mman.h>
struct asmbuf* asmbuf_create(void){
	int prot = PROT_READ | PROT_WRITE;
	int flags = MAP_ANONYMOUS | MAP_PRIVATE;
	return mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);
}

void asmbuf_finalize(struct asmbuf* buf){
	mprotect(buf, sizeof(*buf), PROT_READ | PROT_EXEC);
}

void asmbuf_free(struct asmbuf* buf){
	munmap(buf, PAGE_SIZE);
}
#endif

int main(int argc, char* argv[]){
	printf("hello,  world!\n");

	return 0;
}
