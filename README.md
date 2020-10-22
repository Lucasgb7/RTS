# Sistemas em Tempo Real (RTS - Real Time Systems)

## Threads, concorrência e paralelismo
### Problema
Uma Indústria de Alimentos de Santa Catarina chamada FoodSec S.A. possui a
tarefa de escanear alimentos por meio de câmeras e verificar se os mesmos estão corretos. Os
alimentos podem passar por uma das três esteiras disponíveis. As três esteiras são controladas
por um por um único computador centralizado. Esse computador recebe dados de um sensor
em cada uma das esteiras que captura a contagem de itens que são identificados como
seguros. A contagem é exibida em um display perto das esteiras (todos os displays são
controlados pela mesma função, é apenas uma replicação). Diante disso, a empresa precisa
que vocês implementem, por meio de aplicação para distribuição Linux, uma solução que
consiga realizar as contagens nas três esteiras e exiba o resultado total (contagem esteira 1 +
contagem esteira 2 + contagem esteira 3) em tempo real. A empresa comprou o computador
com processador com 4 núcleos, possui uma distribuição Linux e não aceita atualizar sistema 
ou equipamento, mas aceita carregar novas aplicações. Além disso, os pesos dos itens que
passão por cada uma das esteiras são armazenados em um único vetor de dados. A cada 1.500
unidades de produtos, das três esteiras, é necessário atualizar o peso total de itens
processados. Sendo assim, a empresa aceita uma pausa na quantidade de itens sendo
contados e pesados para realizar a pesagem total, mas ela necessita saber quanto tempo é
necessário para isso.
Diante da problemática apresentada, vocês terão que implementar uma aplicação (em nível de
MVP) que possa lidar com tal situação usando duas abordagens: Pthreads e OpenMP. Deverá
ser feitos um relatório especificando qual a taxa de atualização (capacidade das esteiras em
conseguir identificar um novo item por meio do sensor), tempo de processamento da contagem,
tempo que consegue atualizar o display. Além disso, como a abordagem utiliza seção crítica, é
necessário implementar o uso de mutex na biblioteca Pthread. Como forma de estudar
possíveis soluções, avalie o mutex dessa biblioteca usando os protocolos Herança de
Prioridade e Teto de Prioridade, os quais podem ser configurados para uso na biblioteca
Pthread. Avalie a abordagem single thread e multithread, especifique temporalmente a solução
com, por exemplo 1 ou 2 núcleos. Além disso, ambas as bibliotecas oferecem a implementação
de Barreira (problema da barreira) e será dado pontuação adicional na prova para quem utilizar
de maneira apropriada (+1).