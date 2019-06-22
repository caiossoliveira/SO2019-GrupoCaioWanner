max=30

#avaliacao sequencial de Borwein
for (( i=1; i <= $max; ++i ))
do
	(time ./aplicacao3-SequencialBorwein) >> saidaTempoSequencialBorwein.txt 2>&1
done

#avaliacao sequencial de Gauss
for (( i=1; i <= $max; ++i ))
do
	(time ./aplicacao3-SequencialGaussLegendre) >> saidaTempoSequencialGauss.txt 2>&1
done

#avaliacao sequencial de Monte Carlo
for (( i=1; i <= $max; ++i ))
do
	(time ./aplicacao3-SequencialMonteCarlo) >> saidaTempoSequencialMonteCarlo.txt 2>&1
done

#avaliacao sequencial de Simulacao de Monte Carlo
for (( i=1; i <= $max; ++i ))
do
	(time ./aplicacao3-SequencialSimulacao) >> saidaTempoSequencialSimulacao.txt 2>&1
done


#avaliacao paralela de Borwein
for (( i=1; i <= $max; ++i ))
do
	(time ./aplicacao3-ParaleloBorwein) >> saidaTempoParaleloBorwein.txt 2>&1
done

#avaliacao paralela de Gauss
for (( i=1; i <= $max; ++i ))
do
	(time ./aplicacao3-ParaleloGaussLegendre) >> saidaTempoParaleloGauss.txt 2>&1
done

#avaliacao paralela de Monte Carlo
for (( i=1; i <= $max; ++i ))
do
	(time ./aplicacao3-ParaleloMonteCarlo) >> saidaTempoParaleloMonteCarlo.txt 2>&1
done

#avaliacao paralela de Simulacao de Monte Carlo
for (( i=1; i <= $max; ++i ))
do
	(time ./aplicacao3-ParaleloSimulacao) >> saidaTempoParaleloSimulacao.txt 2>&1
done

