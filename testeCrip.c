#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cript(int tamanho, int key, char *string);

int main(){
 char string[50];
 int tamanho_string;
 int chave;

 printf("Digite uma string: ");
 scanf("%s",string);
 printf("Digite uma chave: ");
 scanf("%i", &chave);
 
 tamanho_string = strlen(string);
 printf("Tamanho string: %i", tamanho_string);
 printf("\n");

 cript(tamanho_string, chave, &string[0]);
 printf("String criptografada: ");

 for(int i=0;i<tamanho_string;i++){
	printf("%c",string[i]);
 }
 
 printf("\n");
 
 system("pause");



}

cript(int tamanho, int key, char *string){

	int teste;
	for(int i=0;i!=tamanho;i++){
		printf("Valor vetor = %i e Chave = %i", *string,key);
		teste = *string = (*string^key);
		printf("->XOR = %i\n", teste);
		string++;
	}

}
