#include <stdio.h>
#include <stdlib.h>

int my_recursion(int i)
{
	if(i <= 0)
	{
		return 0;
	}
	return i += my_recursion(i-1);
}

int my_pointer_function(int *i)
{
	(*i) += 111;
	return (*i)-7;
}

int main()
{
	int my_int = 3;
	int my_second_int = 13;
	//int my_int_array[5];
	//int *my_heap_array;

	int my_return_int = -1;

	//my_heap_array = malloc(sizeof(int)*8);

	//my_int_array[0] = 44;
	//my_int_array[2] = 55;

	//my_heap_array[4] = 101;

	my_return_int = my_recursion(my_int);

	//my_return_int = my_pointer_function(&my_int);

	//my_return_int = my_pointer_function(my_int_array);

	return 0;
}
