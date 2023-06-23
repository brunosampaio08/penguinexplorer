#include <stdio.h>
#include <stdlib.h>

int my_recursion(int i){
	if(i <= 0)
	{
		return 0;
	}
	return i += my_recursion(i-1);
}

int main(){
	int my_int = 3;
	int my_second_int = 13;
	int my_return_int = -1;

	my_return_int = my_recursion(my_int);

	return 0;
}
