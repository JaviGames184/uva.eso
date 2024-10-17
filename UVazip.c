#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]){

	if(argc == 1){
		printf("UVazip: file1 [file2 ...]\n");
		exit(1);
	}

	char caracter;
	char sigcaracter;
	int cont;

	for(int i = 1; i < argc; i++){
		// Lectura del fichero
		FILE *fichero = fopen(argv[i],"r");
		if(fichero==NULL){
			printf("UVazip: no puedo abrir fichero de lectura");
			exit(1);
		}
	
        	//Se lee el fichero por caracteres
		if(i==1){	
			cont = 1;
			caracter = fgetc(fichero);
		}

		if(i!=1){
			caracter = sigcaracter;
		}

		while(caracter!=EOF){
			sigcaracter = (char)fgetc(fichero);
		
			if(caracter == sigcaracter){
				cont ++;
			} else {
				if((sigcaracter != EOF) || ((sigcaracter==EOF) && (i==(argc-1)))){
				
					fwrite(&cont, sizeof(int), 1, stdout);
					fwrite(&caracter, sizeof(char), 1, stdout);

					cont = 1;
				}

				if(sigcaracter == EOF){
					sigcaracter = caracter;
					caracter = EOF;
				} else {
					caracter = sigcaracter;
				}
							
			}
	
		}

		fclose(fichero);
	
	}
	
	exit(0);	

}
