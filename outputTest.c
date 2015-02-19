#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

int main()
{
	double i = 0;
	int j;
	int k = 10;
	
	for( j = 0; j <= 100; j++ )
	{
		i = (double)rand() / ((double)RAND_MAX - 1);
		printf( "%f\n", i );

		k = rand() % 32 + 1;

		printf( "%d\n", k);
		if( k == 0 )
		{
			printf("%s\n", "0 !!!");
			break;
		}
		//else if( k == 32 )
		//{
		//	printf("%s\n", "32 !!");
		//	break;
		//}
	}

	return 0;
}