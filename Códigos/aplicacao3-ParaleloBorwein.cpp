//declaração das bibliotecas
#include <stdio.h>
#include<pthread.h>
#include <time.h>
#include <gmp.h>
#define PREC 33219280  // Precisão utilizada

// Variaveis utilizadas para sincronia das threads
pthread_mutex_t mutex_func = PTHREAD_MUTEX_INITIALIZER;

// Variaveis que serão usadas pelas threads, declaração global
mpf_t a0, a1, y_0, y_1, y_next, aux1, aux2, aux3, aux4, pi, pi_old;

// variavel que conta as iterações
int i = 0;

void *calcula_a(void *arg);

void piBorweinParalelo();
void *calcula_y(void *arg);

main() {
	piBorweinParalelo();
}

void piBorweinParalelo(){

    time_t begin, end;
	pthread_t thread[2];

	time(&begin);

	// Indica qual será a precisão utilizada para as variaveis 
	mpf_set_default_prec(PREC);

	// Inicializa as variaveis
	mpf_inits(y_0,y_1,y_next,a0,a1,pi,pi_old,aux1,aux2,aux3,aux4, NULL);

	// Pi "old" é 0, pi é 1 para que a comparação seja diferente na primeira vez
	mpf_set_d(pi_old,0);
	mpf_set_d(pi,1);

	// y0 = (sqrt(2) - 1.0);
	mpf_set_d(y_0,2);
	mpf_sqrt(y_0, y_0);
	mpf_sub_ui(y_0, y_0, 1);

	// a0 = (6.0 - (4.0*sqrt(2))); 
	mpf_set_d(a0,2);
	mpf_sqrt(a0, a0);
	mpf_mul_ui(a0, a0, 4);
	mpf_ui_sub(a0, 6, a0);

	// Calculamos y1 antes de começar o loop
	pthread_create(&thread[0], NULL, calcula_y, NULL);
	pthread_join(thread[0], NULL);

	while(mpf_cmp(pi_old,pi) != 0){
		// pi_old agora é igual ao pi da iteração anterior
		mpf_set(pi_old,pi);

		// Calcula y(n+1) e a(n)
		pthread_create(&thread[0], NULL, calcula_a, NULL);
		pthread_create(&thread[1], NULL, calcula_y, NULL);
		pthread_join(thread[0], NULL);
		pthread_join(thread[1], NULL);

		// No algoritmo, pi = 1/a1 
		mpf_ui_div(pi, 1, a1);
		//gmp_printf("%da Interacao: %.Ff\n\n",(i+1), pi);
		i++;
	}

	time(&end);

    printf("   --------------------------------------------------------------------\n");
    printf("   |                 Pi por Borwein - Paralelo                      |\n");
    printf("   |            Pi por Borwein: \t%.6Lf                      |\n", pi);
    printf("   --------------------------------------------------------------------\n");

    // Clear em todas as variaveis utilizadas
    mpf_clears(y_0,y_1,y_next,a0,a1,pi,pi_old,aux1,aux2,aux3,aux4, NULL);
}

// Função que calcula a variavel an do algoritmo
void *calcula_a(void *arg){
	pthread_mutex_lock(&mutex_func);

	mpf_add_ui(aux1,y_next,1);
	mpf_pow_ui(aux1, aux1, 4);
	mpf_mul(aux1, a0, aux1);
	mpf_add_ui(aux2, y_next, 1);
	mpf_pow_ui(aux3, y_next, 2);
	mpf_add(aux2, aux2, aux3);
	mpf_mul(aux2, aux2, y_next);

	pthread_mutex_unlock(&mutex_func);

	mpf_set_d(aux3,2);
	mpf_pow_ui(aux4, aux3, (2*i+3));
	mpf_mul(aux2, aux2, aux4);
	mpf_sub(a1, aux1, aux2);

	mpf_set(a0, a1);
}

// Função que calcula a variavel yn do algoritmo
void *calcula_y(void *arg){
	mpf_pow_ui(y_0, y_0, 4);
	mpf_ui_sub(y_0, 1, y_0);
	mpf_sqrt(y_0, y_0);
	mpf_sqrt(y_0, y_0);
	mpf_ui_sub(y_1,1,y_0);
	mpf_add_ui(y_0,y_0,1);
	mpf_div(y_1,y_1,y_0);

	pthread_mutex_lock(&mutex_func);
	mpf_set(y_next,y_1);
	pthread_mutex_unlock(&mutex_func);

	mpf_set(y_0, y_1);
}

