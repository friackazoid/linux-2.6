#include <stdio.h>

int main(){
	unsigned long xer;
	__asm__ volatile ("mov %%cr3, %0\n":"=r"(xer));

	printf("\n [*** 42Tdbg] %X",xer);
	return 0;
}
