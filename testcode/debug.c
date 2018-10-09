#include <stdio.h>

int main(int argc, char const *argv[])
{

#ifdef DEBUG
	printf("debug mode\n");
#endif
	printf("normal mode\n");
	return 0;
}