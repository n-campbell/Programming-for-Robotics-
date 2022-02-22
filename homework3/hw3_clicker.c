#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void ) {
	int val1 = 10;
	int val2 = 20;
	int *b = &val1;
	int *c = b;
	int **d = &b;

	printf("q1: %d\n", **d); // 1

	*b = 1;
	b = &val2;

	printf("q2: %d\n", *b); // 2
	printf("q3: %d\n", *c); // 3
	printf("q4: %d\n", **d); // 4

	*d = &val1;

	printf("q5: %d\n", *b); // 5
	printf("q6: %d\n", *c); // 6
	printf("q7: %d\n", **d); // 7 

	return 0;
}

q1: 10
q2: 20
q3: 1
q4: 20
q5: 1
q6: 1
q7: 1