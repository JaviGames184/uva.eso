#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]){
	
	// No se tienen argumentos
	if(argc == 1){
		printf("UVaunzip: file1 [file2 ...]\n");	
		exit(1);
	}

	int numero;
	char caracter;
	int * buffern = (int *) malloc(2*sizeof(int));
	char * bufferc = (char *) malloc(2*sizeof(char));
	if(buffern == NULL){
		printf("UVaunzip: no se pudo reservar espacio");
		exit(1);
	}
	if(bufferc == NULL){
		printf("UVAunzip: no se pudo reservar espacio");
		exit(1);
	}

	for(int i=1; i<argc; i++){
		// Leemos el fichero
		FILE *fp = fopen(argv[i], "r");
		if(fp == NULL){
			printf("UVaunzip: no puedo abrir fichero");
			exit(1);
		}

		while(0 != fread(buffern, sizeof(int), 1, fp)){
			numero = buffern[0];
			
			fread(bufferc, sizeof(char), 1, fp);
			caracter = bufferc[0];

			for(int i=0; i<numero; i++){
				printf("%c", caracter);
			}
		}
	
		fclose(fp);
	}

	free(buffern);
	free(bufferc);
}
