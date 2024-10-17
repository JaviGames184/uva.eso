#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char * argv[]){
	
	// No tiene los argumentos correctos
	if(argc==1){
		printf("UVagrep: searchterm [file ...]\n");
		exit(1);
	}

	// Elemento a buscar
	char * palabra = argv[1];

	// Leemos de teclado si no se tiene fichero
	if(argc==2){
		char linea[100];
		
		while(fgets(linea, 100, stdin)!=NULL){
			if(strstr(linea, palabra) != NULL){
                		printf("%s", linea);
                	}

		}
	}
	
	// Buscamos en todos los ficheros
	for(int i=2; i < argc; i++){
		FILE *fp = fopen(argv[i], "r");
		if(fp==NULL){
			printf("UVagrep: cannot open file\n");
			exit(1);
		}

		// Leemos por lineas
		char * linea;
		size_t tam = 100;
		
		linea = (char *) malloc(tam*sizeof(char));
		if(linea == NULL){
			exit(1);
		}

		while((getline(&linea, &tam, fp))!=-1){
			
			// Si tiene el elemento se imprime la línea
			if(strstr(linea, palabra) != NULL){
				printf("%s", linea);		
			}
	
		}

		// Cerramos el fichero
		fclose(fp);

		// Liberamos la memoria
		free(linea);
	}	
}
