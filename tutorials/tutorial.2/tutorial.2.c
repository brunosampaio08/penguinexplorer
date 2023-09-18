#include <stdio.h>

int main(){
	int array[5];
	for(int i = 0; i < 5; i++)
	{
		array[i] = 11+i*11;
	}

	array[6] = 100;

	return 0;
}

