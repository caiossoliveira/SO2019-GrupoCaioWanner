//declaração das bibliotecas
#include <stdio.h>
#include <math.h>

//ponteiro para abrir o arquivo do blackscholes
FILE *descritor;
char arquivo[] = {"entrada_blackscholes.txt"};

//variáveis para a simulação de Monte Carlo
double valorAcao, precoOpcao, taxaJuros, volatividadeAcao, tempoValidade, nIteracoes;
const int MAXIT = 1000000000;//1000000000; 

//protótipo das funções
void abrirArquivo();
void fecharArquivo();
void converterListaParaFloat();
void simulacaoMonteCarloSequencial();
void blackScholes(double S, double E, double r, double sigma, double T, int M);
double stdDev(double trials[], double mean, int M);

main(){
	abrirArquivo();
	converterListaParaFloat();
	simulacaoMonteCarloSequencial();
	fecharArquivo();
}

void abrirArquivo(){
    descritor = fopen(arquivo, "r");
}

void simulacaoMonteCarloSequencial(){
	converterListaParaFloat();
	blackScholes(valorAcao, precoOpcao, taxaJuros, volatividadeAcao, tempoValidade, MAXIT);
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

//Executa os cálculos da simulação de Monte Carlo
void blackScholes(double S, double E, double r, double sigma, double T, int M){
	int i;
	double t, mean = 0.0, stddev, confwidth, confmin, confmax;
	double *trials;
	
	trials = (double *) malloc(M*sizeof(double));
	
	//Calcula as iterações
	for(i = 0; i < M; i++){
		t = S*exp((r-((0.5)*pow(sigma, 2.0)))*T + sigma*sqrt(T) * (rand()%100));

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

//Calcula o devio padrão dos resultados da simulação de Monte Carlo
double stdDev(double trials[], double mean, int M){
	double stddev = 0.0;
	int i;
	for(i = 0; i < M; i++)
	{
		stddev += pow((trials[i] - mean),2);
	}
	stddev = sqrt(stddev/((double)M-1));
	return stddev;
}

void fecharArquivo(){
	 fclose(descritor);
}