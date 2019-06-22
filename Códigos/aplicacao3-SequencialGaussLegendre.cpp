//declaração das bibliotecas
#include <stdio.h>
#include <math.h>

//variáveis para a simulação de Monte Carlo
double valorAcao, precoOpcao, taxaJuros, volatividadeAcao, tempoValidade, nIteracoes;
const int MAXIT = 1000000000;//1000000000; 

void piGaussSequencial();

main() {

	piGaussSequencial();
}

void piGaussSequencial(){
	int i;
	double a, an, b, t, pi;
    long long int p;

    //Inicia as variáveis do algoritmo de Gauss
	a = 1;
	b = 1.0/sqrt( 2 );
	t = 0.25;
	p = 1;

    //Calcula o pi
	for( i = 0 ; i <= MAXIT ; i++ )
    {
        an = a;
        a = ( an+b )/2.0;
        b = sqrt( an*b );
        t = t - p*pow( (an - a), 2.0 );
        p = 2*p;
    }
    
    pi = pow( (a + b), 2.0 )/(4.0*t);
    
    printf("   --------------------------------------------------------------------\n");
	printf("   |                 Pi por Gauss-Legendre - Sequencial               |\n");
    printf("   |            Pi por Gauss-Legendre:  %.6lf                      |\n", pi );
    printf("   --------------------------------------------------------------------\n");

}