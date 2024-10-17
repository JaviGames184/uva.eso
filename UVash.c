#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


#define TAM 300
#define MAX_ARG 50


/*===============================================================================================*/
// Funciones
/*===============================================================================================*/
/*
 * Función que procesa cada una de las líneas de la entrada
 *
 * Linea: Cada una de las líneas de la entrada
 * Argumento: Se utiliza para guardar cada una de las palabras que se tienen en la entrada
 * Redireccion: Se utiliza para indicar si es necesario realizar redirección (redireccion != 0) o no(redireccion == 0)
 * 				Devuelve un -1 si se encuentra más de una redireccion o si no se tienen los argumentos necesarios
 * Narg: Devuelve el número de argumentos que se tienen en la línea
 */
int procesarEntrada(char* linea, char** argumento, int* redireccion);
/*************************************************************************************************/
/*
 * Función que ejecuta el comando recibido en argumento[0]
 *
 * Argumento: Cada uno de los argumentos que utilizara el comando
 * Narg: Numero de argumentos
 * Redireccion: Se utiliza para indicar si es necesario realizar redirección (redireccion != 0) o no(redireccion == 0)
 *				Contiene un -1 si se encuentra más de una redireccion o si no se tienen los argumentos necesarios
 * Narg: Devuelve el número de argumentos que se tienen en la línea
 * Error_message: Mensaje de error utilizado en caso de que se produzca algún error
 */
void ejecutarComando(char** argumento, int narg, int redireccion, char* error_message);
/*************************************************************************************************/
/*
 * Función que se encarga de procesar una entrada y ejecutarla
 *
 * Entrada: Cada línea que contiene el comando a ejecutar
 * Len: Longitud de la entrada
 */
void UVash(char* entrada, int len);
/*************************************************************************************************/
/*
 * Función que ejecuta todos los comandos recibidos por la entrada que se encuentren separados por &
 *
 * Entrada: Cada línea que contiene los comandos a ejecutar
 * Error_message: Mensaje de error utilizado en caso de que se produzca un error
 */
void ejecutarComandoParalelo(char* entrada, char* error_message);
/*===============================================================================================*/


/*===============================================================================================*/
// Main
/*===============================================================================================*/
int main(int argc, char* argv[]){
	// Variables
	int len;
	size_t t = TAM;
	char error_message[30] = "An error has occurred\n";
	char* entrada = (char*) malloc(TAM*sizeof(char));
	if(entrada == NULL){
		fprintf(stderr, "%s", error_message);
		exit(1);
	}

	if(argc == 1){
		// Modo interactivo
		while(1){
			printf("UVash> ");
			len = getline(&entrada, &t, stdin);

			if(strchr(entrada, '&') == NULL){
				// No tenemos comandos paralelos
				UVash(entrada, len);
			} else {
				// Tenemos comandos paralelos
				ejecutarComandoParalelo(entrada, error_message);
			}
		}
	} else if(argc == 2){
		// Modo Batch
		FILE *fichero = fopen(argv[1],"r");
		if(fichero == NULL){
			fprintf(stderr, "%s", error_message);
			exit(1);
		}

		while((len = getline(&entrada, &t, fichero))!=-1){
			if(strchr(entrada, '&') == NULL){
				// No tenemos comandos paralelos
				UVash(entrada, len);
			} else {
				// Tenemos comandos paralelos
				ejecutarComandoParalelo(entrada, error_message);
			}
		}
		fclose(fichero);
		free(entrada);
		exit(0);
	} else {
		// Se tienen varios ficheros
		fprintf(stderr, "%s", error_message);
		exit(1);
	}

}

/*===============================================================================================*/
// Funciones
/*===============================================================================================*/
/*
 * Función que procesa cada una de las líneas de la entrada
 *
 * Linea: Cada una de las líneas de la entrada
 * Argumento: Se utiliza para guardar cada una de las palabras que se tienen en la entrada
 * Redireccion: Se utiliza para indicar si es necesario realizar redirección (redireccion != 0) o no(redireccion == 0)
 * 				Devuelve un -1 si se encuentra más de una redireccion o si no se tienen los argumentos necesarios
 * Narg: Devuelve el número de argumentos que se tienen en la línea
 */
int procesarEntrada(char* linea, char** argumento, int* redireccion){
	int narg = 0;
	char* elemento;
	char* red[MAX_ARG];
	int numred = 0;
	int numficheros = 0;

	redireccion[0] = 0;

	if(strchr(linea, '>') != NULL){
		// Tenemos redireccion
		while((elemento = strsep(&linea, ">\n")) != NULL){
			if(strlen(elemento) > 0){
				red[numred] = elemento;
				numred++;
			}
		}

		// Se comprueba que el número de parámetros sea correcto
		if(numred != 2) {
			redireccion[0] = -1;
		} else {
			// Separamos los argumentos
			while((elemento=strsep(&red[0], " \t\n")) != NULL){
				if(strlen(elemento)>0){
					argumento[narg] = elemento;
					narg++;
				}
			}
			// Incluimos la ruta en la que se debe escribir el archivo
			redireccion[0] = narg;
			while((elemento=strsep(&red[1], " \t\n")) != NULL){
				if(strlen(elemento)>0){
					argumento[narg] = elemento;
					narg ++;
					numficheros++;
				}
			}
			// Se tiene un número incorrecto de ficheros
			if(numficheros != 1){
				redireccion[0] = -1;
			}
		}
	} else {
		// No tenemos redireccion
		while((elemento=strsep(&linea, " \t\n")) != NULL){
			if(strlen(elemento)>0){
				argumento[narg] = elemento;
				narg++;
			}
		}
	}
	return narg;
}
/*************************************************************************************************/
/*
 * Función que ejecuta el comando recibido en argumento[0]
 *
 * Argumento: Cada uno de los argumentos que utilizara el comando
 * Narg: Numero de argumentos
 * Redireccion: Se utiliza para indicar si es necesario realizar redirección (redireccion != 0) o no(redireccion == 0)
 *				Contiene un -1 si se encuentra más de una redireccion o si no se tienen los argumentos necesarios
 * Narg: Devuelve el número de argumentos que se tienen en la línea
 * Error_message: Mensaje de error utilizado en caso de que se produzca algún error
 */
void ejecutarComando(char** argumento, int narg, int redireccion, char* error_message){
	// Variables
	char* args[narg+1];

	FILE* ficheroredireccion;
	int stdsalida = dup(STDOUT_FILENO);
	int stderror = dup(STDERR_FILENO);


	if(strcmp("cd", argumento[0]) == 0){
		if(narg != 2){
			fprintf(stderr, "%s", error_message);
		} else {
			if(chdir(argumento[1])!=0){
				fprintf(stderr, "%s", error_message);
			}
		}
	} else {
		// Tenemos que ejecutar el comando
		if(redireccion > 0){
			ficheroredireccion = fopen(argumento[redireccion], "w");
			if(ficheroredireccion == NULL){
				fprintf(stderr, "%s", error_message);
				redireccion = 0;
			} else {
				// Se necesita que el fichero sea un entero
				int fredireccion = fileno(ficheroredireccion);

				if(dup2(fredireccion, STDOUT_FILENO) == -1){
					fprintf(stderr, "%s", error_message);
				}
				if(dup2(fredireccion, STDERR_FILENO) == -1){
					fprintf(stderr, "%s", error_message);
				}
			}
			// Ya no se necesita ["fichero.txt"]
			narg = narg-1;
		}

		// Se ejecuta el comando en un proceso hijo
		if(redireccion != -1){
			pid_t pid = fork();
			if(pid == -1){
				fprintf(stderr, "%s", error_message);
			} else if(pid == 0){
				// Proceso hijo

				// Se colocan los argumentos
				for(int i = 0; i<narg; i++){
					args[i] = argumento[i];
				}
				args[narg] = NULL;

				if(execvp(argumento[0], args)==-1){
					fprintf(stderr, "%s", error_message);
				}
				exit(0);
			} else {
				// Proceso padre
				wait(NULL);

				if(redireccion != 0){
					if(dup2(stdsalida, STDOUT_FILENO) == -1){
						fprintf(stderr, "%s", error_message);
					}
					if(dup2(stderror, STDERR_FILENO) == -1){
						fprintf(stderr, "%s", error_message);
					}
					fclose(ficheroredireccion);
				}
			}
		} else {
			// Existe un error en la redireccion
			fprintf(stderr, "%s", error_message);
		}

	}
}
/*************************************************************************************************/
/*
 * Función que se encarga de procesar una entrada y ejecutarla
 *
 * Entrada: Cada línea que contiene el comando a ejecutar
 * Len: Longitud de la entrada
 */
void UVash(char* entrada, int len){
	//Variables
	char error_message[30] = "An error has occurred\n";
	int redireccion = 0;
	int numeroargumentos = 0;
	char* argumento[MAX_ARG];

	if(strspn(entrada, " \n\t") != len){
		numeroargumentos = procesarEntrada(entrada, argumento, &redireccion);

		// Comandos
		if(strcmp("exit", argumento[0]) == 0){
			if(numeroargumentos != 1){
				fprintf(stderr, "%s", error_message);
			} else {
				free(entrada);
				exit(0);
			}
		} else {
				ejecutarComando(argumento, numeroargumentos, redireccion, error_message);
		}
	}
}
/*************************************************************************************************/
/*
 * Función que ejecuta todos los comandos recibidos por la entrada que se encuentren separados por &
 *
 * Entrada: Cada línea que contiene los comandos a ejecutar
 * Error_message: Mensaje de error utilizado en caso de que se produzca un error
 */
void ejecutarComandoParalelo(char* entrada, char* error_message){
	char* elemento;
	int numerocomandos = 0;
	char* comando[MAX_ARG];
	int lencomando[MAX_ARG];
	pid_t pid;
	int i = 0;

	// Guardamos cada uno de los comandos
	while((elemento=strsep(&entrada, "&\n")) != NULL){
		if(i == 0){
			if((strlen(elemento) == 0) || (strlen(elemento) == strspn(elemento, " \n\t"))){
				fprintf(stderr, "%s", error_message);
			}
			i++;
		}
		// No contamos elementos vacios
		if(strlen(elemento) > 0){
			comando[numerocomandos] = elemento;
			lencomando[numerocomandos] = strlen(elemento);
			numerocomandos++;
		}
	}

	// Crear hijos para cada comando
	for(int i=0; i<numerocomandos;i++){
		pid = fork();
		if(pid == -1){
			fprintf(stderr, "%s", error_message);
			exit(1);
		} else if(pid == 0){
			// Cada hijo ejecuta su parte del código
			UVash(comando[i], lencomando[i]);
			exit(0);
		}
	}

	if(pid!=0) {
		// Esperamos a los hijos
		while(wait(NULL)!=-1);
	}
}
