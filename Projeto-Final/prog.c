
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

int main(){
   int ret, fd;
char opcao[20];
char aux;
   char stringToSend[BUFFER_LENGTH];
   fd = open("/dev/ebbchar", O_RDWR);             // Open the device with read/write access
   if (fd < 0){
      perror("Falhou ao abrir o device");
      return errno;
   }
   printf("Digite a operação e o dado\n");
printf("C - Criptografar | D - Descriptografar | H - Hash\n");
fflush(stdout);
   scanf("%[^\n]%*c", stringToSend);                // Read in a string (with spaces)
aux = stringToSend[0];
   ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
   if (ret < 0){
      perror("Não conseguiu escrever a mensagem");
      return errno;
   }

if(aux == 'C'){
strcpy(opcao, "criptografado");
}
else if(aux == 'D'){
strcpy(opcao, "descriptografado");
}
else if(aux == 'H'){
strcpy(opcao, "em hash");
}


   printf("Presione enter para receber o dado\n");
   getchar();

   ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
   if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   printf("O dado %s eh: [%s]\n", opcao, receive);
   return 0;
}
