//declaração das bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

//ponteiro para abrir o arquivo do blackscholes
FILE *descritor;
char arquivo[] = {"entrada_blackscholes.txt"};

struct BoxMullerState{
        double x1, x2, w, y1, y2;
        int useLast;
        struct drand48_data random;
};

typedef struct{
	int size;
	double S, E, r, sigma, T;
	double *trials;
	double sum;
}blkScholesStruct;

//variáveis para a simulação de Monte Carlo
double valorAcao, precoOpcao, taxaJuros, volatividadeAcao, tempoValidade, nIteracoes;
const int MAXIT = 1000000000;//1000000000; 

//protótipo das funções
void abrirArquivo();
void fecharArquivo();
void menu();
void converterListaParaFloat();
void simulacaoMonteCarloParalelo();

void blackScholes(double S, double E, double r, double sigma, double T, int M);

//protótipo das funções para simulação de monte carlo
void *blkScholesItself(void *ptr);
double stdDev(double trials[], double mean, int M);
double boxMullerRandom2(struct BoxMullerState* state);

main() {
	abrirArquivo();
	converterListaParaFloat();
	simulacaoMonteCarloParalelo();
	fecharArquivo();
}

void abrirArquivo(){
    descritor = fopen(arquivo, "r");
}

void simulacaoMonteCarloSequencial(){
	converterListaParaFloat();
	blackScholes(valorAcao, precoOpcao, taxaJuros, volatividadeAcao, tempoValidade, MAXIT);
}

void simulacaoMonteCarloParalelo(){

	converterListaParaFloat();

	#define NTHREADS 2
    int M;
	M = nIteracoes;

	pthread_t thread[NTHREADS];

	double result = 0.0, trials[M], mean, stddev, confwidth, confmin, confmax;

	int ret[NTHREADS], i, j, count;

	blkScholesStruct *blkSS[NTHREADS];

	//Cria as estruturas que serão passadas para as threads, inicializa suas variáveis e cria os threads
	for(i = 0; i < NTHREADS; i++)
	{	
		blkSS[i] = (blkScholesStruct *)calloc(1, sizeof(blkScholesStruct));
		
		blkSS[i]->S = valorAcao;
		blkSS[i]->E = precoOpcao;
		blkSS[i]->r = taxaJuros;
		blkSS[i]->sigma = volatividadeAcao;
		blkSS[i]->T = tempoValidade;
		blkSS[i]->size = (M/(double)NTHREADS);
		blkSS[i]->trials = (double*)calloc(blkSS[i]->size, sizeof(double));

		ret[i] = pthread_create(&thread[i], NULL, blkScholesItself, (void*) blkSS[i]);

		if(ret[i]){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",ret[i]);
			exit(EXIT_FAILURE);
		}
	}

	//Espera as threads acabarem
	for(i = 0; i < NTHREADS; i++)
	{
		pthread_join(thread[i], NULL);
	}

	count = 0;
	
	//soma todos os resultados das threads, coloca todos os testes em um vetor estático, libera a memória das estruturas
	for(i = 0; i < NTHREADS; i++)
	{
		result+=blkSS[i]->sum;
		for(j = 0; j < blkSS[i]->size; j++)
		{
			trials[count++] = blkSS[i]->trials[j];
		}
		free(blkSS[i]->trials);
		free(blkSS[i]);
	}
	
	//Cálculo final e resultado
	mean = result/(double)M;
	stddev = stdDev(trials, mean, M);
	confwidth = 1.96*stddev/sqrt(M);
	confmin = mean - confwidth;
	confmax = mean + confwidth;

	printf("\n     Valor da acao (S): \t%lf", valorAcao);
	printf("\n     Preco de exercicio da opcao (E): \t%lf", precoOpcao);
	printf("\n     Taxa de juros livre de risco (SELIC) (r): \t%lf", taxaJuros);
	printf("\n     Volatividade da acao (sigma): \t%lf", volatividadeAcao);
	printf("\n     Tempo de validade da opcao (T): \t%lf", tempoValidade);
	printf("\n     Numero de iteracoes (M): \t%d", M);
	printf("\n     Intervalo de confianca: (%.4f, %.4f)\n\n", confmin, confmax);

	printf("   |------------------------------------------------------------------|\n");
	
}

void converterListaParaFloat(){
	printf("\n");
	printf("   --------------------------------------------------------------------\n");
	printf("   |                        Conteudo do Arquivo                       |\n");

	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int n = 0;

    fp = fopen(arquivo, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("     %s", line);
        n++;
        switch(n){
        	case 1: valorAcao = atof(line);
        	break;
        	case 2: precoOpcao = atof(line);
        	break;
        	case 3: taxaJuros = atof(line);
        	break;
        	case 4: volatividadeAcao = atof(line);
        	break;
        	case 5: tempoValidade = atof(line);
        	break;
        	case 6: nIteracoes = atof(line); 
        	break;
        }
    }
    printf("   --------------------------------------------------------------------\n");

    printf("\n");

    printf("   --------------------------------------------------------------------\n");
	printf("   |                     Simulacao de Monte Carlo                     |\n");
	printf("   |                     Modelo de Black Scholes                      |\n");

    printf("\n     Valor da acao: %f", valorAcao);
    printf("\n     Preco de exercicio da apcao: %f", precoOpcao);
    printf("\n     Taxa de juros livre de risco: %f", taxaJuros);
    printf("\n     Volatividade da acao: %f", volatividadeAcao);
    printf("\n     Tempo de validade da opcao: %f", tempoValidade);
    printf("\n     Numero de interacoes: %f \n     ", nIteracoes);

    fclose(fp);
    if (line)
        free(line);
}

//Calcula o devio padrão dos resultados da simulação de Monte Carlo
double stdDev(double trials[], double mean, int M)
{
	double stddev = 0.0;
	int i;
	for(i = 0; i < M; i++)
	{
		stddev += pow((trials[i] - mean),2);
	}
	stddev = sqrt(stddev/((double)M-1));
	return stddev;
}

//Executa os cálculos da simulação de Monte Carlo
void blackScholes(double S, double E, double r, double sigma, double T, int M){
	int i;
	double t, mean = 0.0, stddev, confwidth, confmin, confmax;
	double *trials;
	struct BoxMullerState state;
	
	trials = (double *) malloc(M*sizeof(double));
	
	//Calcula as iterações de Black Scholes
	for(i = 0; i < M; i++){
		t = S*exp((r-((1.0/2.0)*pow(sigma, 2.0)))*T + sigma*sqrt(T)*boxMullerRandom2(&state));
		//t = S*exp((r-((0.5)*pow(sigma, 2.0)))*T + sigma*sqrt(T) * (rand()%100));

		if((t-E) > 0.0){
			trials[i] = exp((-r)*T)*(t-E);
		}
		else{
			trials[i] = 0.0;
		}
		mean += trials[i];
	}

	//Calcula média, desvio padrão e a confiança da simulação de Black Scholes
	mean = mean/(double)M;
	stddev = stdDev(trials, mean, M);
	confwidth = 1.96*stddev/sqrt(M);
	confmin = mean - confwidth;
	confmax = mean + confwidth;

	printf("\n     Valor da acao (S): \t%lf", S);
	printf("\n     Preco de exercicio da opcao (E): \t%lf", E);
	printf("\n     Taxa de juros livre de risco (SELIC) (r): \t%lf", r);
	printf("\n     Volatividade da acao (sigma): \t%lf", sigma);
	printf("\n     Tempo de validade da opcao (T): \t%lf", T);
	printf("\n     Numero de iteracoes (M): \t%d", M);
	printf("\n     Intervalo de confianca: (%.4f, %.4f)\n", confmin, confmax);

	printf("   |------------------------------------------------------------------|\n");
}

void fecharArquivo(){
	 fclose(descritor);
}

void initBoxMullerState(struct BoxMullerState* state){
    state->random.__init = 0;
    state->useLast = 0;
    struct timeval now;
	gettimeofday(&now, NULL);
	state->random.__x[0] = now.tv_usec;
}

void *blkScholesItself(void *ptr){
	//Converte o ponteiro recebido
	blkScholesStruct *blkSS = (blkScholesStruct*) ptr;

	int i;
	double t;
	struct BoxMullerState state;

	initBoxMullerState(&state);

	blkSS->sum = 0;

	//Faz os calculos do método Black Scholes
	for(i = 0; i < blkSS->size; i++)
	{
		t = blkSS->S*exp((blkSS->r-((1.0/2.0)*pow(blkSS->sigma, 2.0)))*blkSS->T + blkSS->sigma*sqrt(blkSS->T)*boxMullerRandom2(&state));

		if((t-blkSS->E) > 0.0)
			blkSS->trials[i] = exp((-blkSS->r)*blkSS->T)*(t-blkSS->E);
		else
			blkSS->trials[i] = 0.0;
		blkSS->sum += blkSS->trials[i];
	}
   	return NULL;
}

//Calcula um pseudo numero randomico
double boxMullerRandom2(struct BoxMullerState* state){
    if (state->useLast){
            state->y1 = state->y2;
            state->useLast = 0;
    }
    else{
        do{
            drand48_r(&state->random, &state->x1);
            state->x1 = 2.0 * state->x1 - 1.0;
            drand48_r(&state->random, &state->x2);
            state->x2 = 2.0 * state->x2 - 1.0;
            state->w = state->x1 * state->x1 + state->x2 * state->x2;
        }while (state->w >= 1.0);
        state->w = sqrt((-2.0 * log(state->w)) / state->w);
        state->y1 = state->x1 * state->w;
        state->y2 = state->x2 * state->w;
        state->useLast = 1;
    }
    return state->y1;
}