//declaração das bibliotecas
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct{
	int size;
	double result;
}monteCarloStruct;

struct BoxMullerState{
    double x1, x2, w, y1, y2;
    int useLast;
    struct drand48_data random;
};

const int MAXIT = 1000000000;//1000000000; 

//protótipo das funções
void piMonteCarloParalelo();
void *itself(void *ptr);
void initBoxMullerState(struct BoxMullerState* state);
int boxMullerRandom(struct BoxMullerState* state);

main() {
	piMonteCarloParalelo();
}

void piMonteCarloParalelo(){
	#define NTHREADS 2

	pthread_t thread[NTHREADS];

	double result = 0.0;

	int ret[NTHREADS], i;

	monteCarloStruct *mCS[NTHREADS];

	//Aloca as structs para enviar para as threads 
	for(i = 0; i < NTHREADS; i++)
	{	
		mCS[i] = (monteCarloStruct *)calloc(1, sizeof(monteCarloStruct));
		mCS[i]->size = (MAXIT/NTHREADS);
		ret[i] = pthread_create(&thread[i], NULL, itself, (void*) mCS[i]);

		if(ret[i]){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",ret[i]);
			exit(EXIT_FAILURE);
		}
	}

	for(i = 0; i < NTHREADS; i++)
	{
		pthread_join(thread[i], NULL);
	}

	//Soma os resultados
	for(i = 0; i < NTHREADS; i++)
	{
		result+=mCS[i]->result;
		free(mCS[i]);
	}
	
	//Dá o resultado pela média da soma do resultado de cada thread
	result = result/NTHREADS;

	printf("   --------------------------------------------------------------------\n");
	printf("   |                 Pi por Monte Carlo - Paralelo                    |\n");
   	printf("   |            Pi por Monte Carlo: %.6f                          |\n", result);
   	printf("   --------------------------------------------------------------------\n");

}

void *itself(void *ptr){
	monteCarloStruct *mCS = (monteCarloStruct*) ptr;

	int i,count=0;

	struct BoxMullerState state;

	initBoxMullerState(&state);
	//calcula o Pi usando o método de Monte Carlo
	count=0;
	for ( i=0; i<mCS->size; i++) {
    	if(!boxMullerRandom(&state))
    		count++;
    }
   	
   	mCS->result = (count/(double)mCS->size)*4;

   	return NULL;
}

void initBoxMullerState(struct BoxMullerState* state){
        state->random.__init = 0;
        state->useLast = 0;
        
        struct timeval now;
	gettimeofday(&now, NULL);
	state->random.__x[0] = now.tv_usec;
}

int boxMullerRandom(struct BoxMullerState* state){
        drand48_r(&state->random, &state->x1);
        state->x1 = 2.0 * state->x1 - 1.0;
        drand48_r(&state->random, &state->x2);
        state->x2 = 2.0 * state->x2 - 1.0;
        state->w = state->x1 * state->x1 + state->x2 * state->x2;
        if(state->w>1.0)
                return 1;
        else
                return 0;
}