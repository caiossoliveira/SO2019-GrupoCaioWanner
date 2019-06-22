//declaração das bibliotecas
#include <stdio.h>
#include <time.h>
#include <gmp.h>
#define PREC 33219280  /* Precisão utilizada */

const int MAXIT = 1000000000;

void piBorweinSequencial();

main() {
	piBorweinSequencial();
}

void piBorweinSequencial(){

    mpf_t y0, y1, a0, a1, pi, pi_old;
    mpf_t aux1, aux2, aux3, aux4;
    time_t begin, end;
    int i = 0;

    time(&begin);

    // Indica qual será a precisão utilizada para as variaveis
    mpf_set_default_prec(PREC);

    // Inicializa as variaveis
    mpf_inits(y0,y1,a0,a1,pi,pi_old,aux1,aux2,aux3,aux4, NULL);

    // Pi "old" é 0, pi é 1 para que a comparação seja diferente na primeira vez
    mpf_set_d(pi_old,0);
    mpf_set_d(pi,1);

    // y0 = (sqrt(2) - 1.0); 
    mpf_set_d(y0,2);
    mpf_sqrt(y0, y0);
    mpf_sub_ui(y0, y0, 1);

    // a0 = (6.0 - (4.0*sqrt(2)));
    mpf_set_d(a0,2);
    mpf_sqrt(a0, a0);
    mpf_mul_ui(a0, a0, 4);
    mpf_ui_sub(a0, 6, a0);

    while(mpf_cmp(pi_old,pi) != 0){
        // pi_old agora é igual ao pi da iteração anterior
        mpf_set(pi_old,pi);

        // Calcula y1 = ( (1 - pow((1 - pow(y0,4)),0.25)) / (1 + pow((1 - pow(y0,4)),0.25))
        mpf_pow_ui(y0, y0, 4);
        mpf_ui_sub(y0, 1, y0);
        mpf_sqrt(y0, y0);
        mpf_sqrt(y0, y0);
        mpf_ui_sub(y1,1,y0);
        mpf_add_ui(y0,y0,1);
        mpf_div(y1,y1,y0);

        // Calcula a1 = a0 * pow(1+y1,4) - y1*pow(2,2*i+3)*(1+y1+(y1*y1));
        mpf_add_ui(aux1,y1,1);
        mpf_pow_ui(aux1, aux1, 4);
        mpf_mul(aux1, a0, aux1);
        mpf_add_ui(aux2, y1, 1);
        mpf_pow_ui(aux3, y1, 2);
        mpf_add(aux2, aux2, aux3);
        mpf_mul(aux2, aux2, y1);
        mpf_set_d(aux3,2);
        mpf_pow_ui(aux4, aux3, (2*i+3));
        mpf_mul(aux2, aux2, aux4);
        mpf_sub(a1, aux1, aux2);

        // No algoritmo, pi = 1/a1
        mpf_ui_div(pi, 1, a1);
        //gmp_printf("%da Interacao: %.Ff\n\n",(i+1), pi);

        mpf_set(y0, y1);
        mpf_set(a0, a1);
        i++;
    }

    time(&end);

    printf("   --------------------------------------------------------------------\n");
    printf("   |                 Pi por Borwein - Sequencial                      |\n");
gmp_printf("   |            Pi por Borwein: %.6Ff                              |\n", pi);
    printf("   --------------------------------------------------------------------\n");

    // Clear em todas as variaveis utilizadas
    mpf_clears(y0,y1,a0,a1,pi,pi_old,aux1,aux2,aux3,aux4, NULL);

}

