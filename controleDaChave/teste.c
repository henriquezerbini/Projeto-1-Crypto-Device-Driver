
#include <stdio.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


int printaBinario(int numero){
    printf("B = "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(numero));
    printf(" ");
}

int main()
{
	int n, i, cont,j=0;
	char a, aString[2], final[16];

	scanf("%s", aString);
	fflush(stdout);
	printf("a string eh %s e cont %i\n\n", aString, strlen(aString));
	cont = strlen(aString);
	for (i = 0; i < cont; i++)
	{
		if (aString[i] > 47 && aString[i] < 58)
		{
		    printf("numero\n");
			aString[i] = aString[i] - 48;
		}
		else if (aString[i]>64 && aString[i] <71)
		{
		    printf("letra\n");
			aString[i] = aString[i] - 55;
		}
		
	}
    for (i = 0; i < cont; i=i+2)
	{
	    printf("\n\n");
		printf("int = %.4i e indice eh %i e segundo numero int = %.4i e indice eh %i\n", aString[i], i,aString[i+1], i+1);
		fflush(stdout);
		printf("Binari eh: ");
		printaBinario(aString[i]);
		printaBinario(aString[i+1]);
		printf("\n");
		final[j] = (aString[i] = aString[i] << 4) + aString[i+1];
		printf("agora deslocado eh %i ", final[j]);
		fflush(stdout);
		//https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
		printf("Leading text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(final[j]));
		fflush(stdout);
		printf("\n");
		fflush(stdout);
		j++;
		
	}
    printf("i = %i e cont = %i", i, cont);
    return 0;
}
