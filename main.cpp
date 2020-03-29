#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<sys/mman.h>
#define PAGE_SIZE 4096

//code + count = 4kb
typedef struct {
	uint8_t code[PAGE_SIZE - sizeof(uint64_t)];
	uint64_t count;
} asmbuf;

asmbuf* asmbuf_create(void){
	int prot = PROT_READ | PROT_WRITE;
	int flags = MAP_ANONYMOUS | MAP_PRIVATE;
	return mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);
}

void asmbuf_finalize(asmbuf* buf){
	mprotect(buf, sizeof(*buf), PROT_READ | PROT_EXEC);
}

void asmbuf_free(asmbuf* buf){
	munmap(buf, PAGE_SIZE);
}

int main(int argc, char* argv[]){
	printf("hello,  world!\n");

	return 0;
}
