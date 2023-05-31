/* sieve.c -- Eratosthenes Sieve prime number calculation */
/* scaled down with MAX_PRIME set to 17 instead of 8091 */

#define MAX_ITER	1
#define MAX_PRIME	17

char	flags[MAX_PRIME];

main ()
{
	register int i,k;
	int	prime,count,iter;

	for (iter = 1;iter<=MAX_ITER;iter++)
		{
		count = 0;
		for(i = 0; i<MAX_PRIME; i++)
			flags[i] = 1;
		for(i = 0; i<MAX_PRIME; i++)
			if(flags[i])
				{
				prime = i + i + 3;
				k = i + prime;
				while (k < MAX_PRIME)
					{
					flags[k] = 0;
					k += prime;
					}
				count++;
				printf(" prime %d = %d\n", count, prime);
				}
		}
	printf("\n%d primes\n",count);
}
