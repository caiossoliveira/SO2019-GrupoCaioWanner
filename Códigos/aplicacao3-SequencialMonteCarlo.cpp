//declaração das bibliotecas
#include <stdio.h>
#include <math.h>

const int MAXIT = 1000000000;//1000000000; 

//protótipo das funções
void piMonteCarloSequencial();

main() {
	piMonteCarloSequencial();
}

void piMonteCarloSequencial(){
	double x,y;
	int count=0;
	long int i;
	double z;
	double pi;

	srand(time_t(NULL));
	count=0;

	//Calcula o Pi
	for ( i=0; i<MAXIT; i++) {
    	x = (double)rand()/RAND_MAX;
    	y = (double)rand()/RAND_MAX;
    	z = x*x+y*y;
    	if (z<=1) count++;
    }
   	pi=(double)count/MAXIT*4;
   	printf("\n");
   	printf("   --------------------------------------------------------------------\n");
	printf("   |                   Pi por Monte Carlo - Sequencial                |\n");
   	printf("   |            Pi por Monte Carlo: %.6f                          |\n", pi);
   	printf("   --------------------------------------------------------------------\n");
}