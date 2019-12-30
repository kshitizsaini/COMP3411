#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
int main(int argc, char *argv[]) {
	assert(argc == 2);
	FILE *fp = fopen(argv[1],"r");
	int X = 0, O = 0;
	char ch;
	while ((ch = fgetc(fp)) != EOF) {
		if (ch == 'X') X++;
		else if (ch == 'O') O++;
	}
	printf("X : O = %d : %d\n", X, O);
	return 0;
}
