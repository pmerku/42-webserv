#include <stdio.h>

int main(int argc, char** argv, char** envp) 
{
	printf("argc: %d\n", argc);
	printf("\nargv:\n");
	for (int i = 0; argv[i]; ++i)
		printf("%s\n", argv[i]);
	printf("\nenvp:\n");
	for (int i = 0; envp[i]; ++i)
		printf("%s\n", envp[i]);
	return (0);
}