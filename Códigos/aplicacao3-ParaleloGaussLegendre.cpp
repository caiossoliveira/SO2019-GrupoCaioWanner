//declaração das bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct{
    double a;
    double an;
    double b;
    double t;
    long long int p;
}GaussLegStruct;

pthread_mutex_t mutex;

const int MAXIT = 1000000000;//1000000000; 

//protótipo das funções
void piGaussParalelo();
void *calcAB(void *ptr);
void *calcTP(void *ptr);

main() {
    piGaussParalelo();
}

void piGaussParalelo(){

    pthread_t threads[2];

	int ret[2];
	long int i;

	double pi;

	GaussLegStruct *glstruct;

	//Aloca as structs and inicializa as variaveis
	glstruct = (GaussLegStruct*)calloc(1, sizeof(GaussLegStruct));

	glstruct->a = 1;
	glstruct->an = 1;
	glstruct->b = 1.0/sqrt(2);
	glstruct->t = 0.25;
	glstruct->p = 1;

	//Cria as threads para calcular cada parte do algoritmo a cada interação, até o fim do algoritmo
	for(i = 0; i < MAXIT; i++){
		pthread_mutex_lock (&mutex);
		ret[0] = pthread_create(&threads[0], NULL, calcAB, (void*) glstruct);

		if(ret[0]){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",ret[0]);
			exit(EXIT_FAILURE);
		}
		ret[1] = pthread_create(&threads[1], NULL, calcTP, (void*) glstruct);

		if(ret[1]){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",ret[1]);
			exit(EXIT_FAILURE);
		}

		pthread_join(threads[0], NULL);
		pthread_join(threads[1], NULL);
	}

	//Calcula o Pi segundo a formula de GaussLegendre
	pi = pow( (glstruct->a + glstruct->b), 2.0 )/(4.0*glstruct->t);

	printf("   --------------------------------------------------------------------\n");
	printf("   |                 Pi por Gauss-Legendre - Paralelo                 |\n");
    printf("   |            Pi por Gauss-Legendre:  %.6lf                      |\n", pi );
    printf("   --------------------------------------------------------------------\n");

    free(glstruct);

    //pthread_exit(NULL);
}

void *calcAB(void *ptr){
    //calcula valores A e B
    GaussLegStruct *glstr = (GaussLegStruct*)ptr;
    glstr->an = glstr->a;
    glstr->a = (glstr->an+glstr->b)/2.0;
    glstr->b = sqrt( (glstr->an)*(glstr->b) );

    //libera a thread
    pthread_mutex_unlock (&mutex);
    
    return NULL;
}

void *calcTP(void *ptr){
    //tranca a thread para calcular os valores T e P
    pthread_mutex_lock (&mutex);

    GaussLegStruct *glstr = (GaussLegStruct*)ptr;
    glstr->t = (glstr->t) - (glstr->p)*pow( ((glstr->an) - (glstr->a)), 2.0 );
    glstr->p = 2*(glstr->p);

    //destranca a thread
    pthread_mutex_unlock (&mutex);

    return NULL;
}
