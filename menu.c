#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<fcntl.h>
#include<unistd.h>

int main(void){

	char entrada[20];
	char dados[18];
	int tamanho, j=0;
	FILE *arquivo;
	

	fflush(stdin);
	gets(entrada);
	tamanho = strlen(entrada);

	for(int i = 2;i < tamanho;i++){
		dados [j] = entrada[i];
		j++;
	}



	if(entrada[0] == 'c'){
		printf("Cifrar!\n");
		arquivo = fopen ("arquivoMenu.txt", "w");
		if(arquivo == NULL)
		{
			printf("Erro ao abrir o arquivo\n");
		}
		fprintf(arquivo, "%s", dados);
		printf("Escrevendo dados....\n");
		sleep(5);
		printf("Dados escritos com sucesso!\n");
		fclose(arquivo);
		
		
	}
	if(entrada[0] == 'd'){
		printf("Decifrar!\n");
	}
	if(entrada[0] == 'h'){
		printf("Resumo criptografico!\n");
	}


	return 0;

	

}
