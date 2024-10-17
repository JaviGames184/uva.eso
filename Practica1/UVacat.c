// Javier Ramos Jimeno

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]){
	
	// Si no se nos proporciona ningún archivo
	if(argc==0){
		exit(0);
	}
	
	// Importación de un fichero
	for(int i=1; i < argc; i++){
		FILE *fp = fopen(argv[i], "r");
		if(fp == NULL){
			printf("UVacat: no puedo abrir fichero\n");
			exit(1);
		}

		// Salida del fichero
		char buffer[100];
		
		while(fgets(buffer,100,fp) != NULL){
			printf("%s", buffer);
		}

		// Cerrar fichero
		fclose(fp);
	}		
}




