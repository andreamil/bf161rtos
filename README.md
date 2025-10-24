PRIMEIRO TRABALHO PRÁTICO
1. Enunciado
Tendo como base o RTOS desenvolvido em sala de aula, faças as seguintes inclusões na API deste:
1. Implementação do mecanismo de sincronização de tarefas baseado em variáveis mutex.
2. Implementação do algoritmo de escalonamento de tarefas baseado em prioridade.
3. Alterar a estrutura de dados do PIPE para que a mesma seja alocada dinamicamente ao invés de
estaticamente.
4. Implementação de uma API de E/S para manipular os seguintes periféricos: PWM, ADC e
interrupção externa.
Após a implementação dos itens citados acima implemente uma aplicação embarcada para validar o
RTOS desenvolvido. A aplicação embarcada se refere ao controle de um drone do tipo quadrimotor
equipado com giroscópio, acelerômetro e medidor de carga de bateria. A Figura 1 ilustra um modelo de
drone quadrimotor.
Figura 1. Exemplo de drone quadrimotor.
O sistema de controle do drone é composto pelas seguintes tarefas:
• Controle dos motores: é a tarefa responsável pelo acionamento dos quatro motores em
velocidades específicas. Esta tarefa recebe uma mensagem no formato
|M1|VEL_M1|M2|VEL_M2|M3|VEL_M3|M4|VEL_M4| da tarefa de controle central. Esta
mensagem trata-se de uma variável compartilhada entre a tarefa de controle central e a tarefa
de controle dos motores. O acesso a variável compartilhada deve ser exclusivo, ou seja, há a
necessidade de estabelecer um protocolo de sincronização de acesso com semáforo ou variável
mutex.
• Leitura dos sensores: esta tarefa é responsável por ler os dados do giroscópio e do acelerômetro
e então enviá-los via fila de mensagens para a tarefa de controle central.
• Monitoramento da bateria: é a tarefa que fica lendo a tensão da bateria para verificar se a carga
é suficiente para concluir a missão. Caso a carga esteja abaixo do mínimo esta tarefa deverá
informar a tarefa de controle central para que esta última faça com que o drone retorne a base
de operação.
• Tarefa de controle central: esta é a principal tarefa do sistema de controle do drone. É ela quem
calcula a velocidade e altitude do drone. Esta tarefa recebe via fila de mensagens os dados da
tarefa de leitura dos sensores e então calcula as velocidades para os motores. As informações
sobre as velocidades dos motores são escritas em uma variável compartilhada com a tarefa
controle dos motores.
Implemente o sistema embarcado de acordo com as especificações acima. Utilize a API do RTOS
desenvolvido em aula para prover os recursos necessários para a execução da aplicação embarcada. Os
recursos a serem utilizado basicamente são: escalonamento baseado em prioridade, API de E/S (leitura
do periférico de ADC e geração de sinal PWM), sincronização de tarefas com semáforo ou variável
mutex e comunicação entre tarefas com pipe.
2. Informações
O trabalho poderá ser feito em dupla. Se atenha ao que foi especificado no enunciado. Para a
conclusão do trabalho com êxito não esqueça de implementar os quatro itens que foram listados no
início do enunciado. Você poderá utilizar os seguintes componentes: potenciômetro para simular o
giroscópio, o acelerômetro e o nível de carga da bateria, motores DC e leds para simular os quatro
motores do drone. O hardware poderá ser projetado no Proteus ou em protoboard. Há ainda a opção
de utilizar o PICsimlab disponível em https://github.com/lcgamboa/picsimlab.

3. Implementação realizada
- Sincronização: adicionada API de mutex (mutex_init, mutex_lock, mutex_unlock, mutex_trylock).
- Escalonamento: implementado escalonador por prioridade com round-robin entre tarefas de mesma prioridade; configurado como padrão.
- Pipe: buffer agora é alocado dinamicamente (SRAMalloc/SRAMfree) e adicionadas operações em bloco (pipe_write_block/pipe_read_block).
- API de E/S: ampliada com PWM por software (RC0..RC3) e interrupção externa INT0 com callback. ADC já existente foi mantido.
- Aplicação Drone: quatro tarefas (motores, sensores, bateria e controle central) usando mutex, pipe e PWM/ADC/INT0 conforme especificado.

Mapeamentos de I/O (PIC18F4550):
- PWM por software em RD1..RD4 (motores M1..M4).
- ADC: CH0 (gyro simulado), CH1 (acelerômetro simulado), CH2 (bateria simulada).
- INT0 (RB0): botão externo para alternar habilitação de voo.

Obs.: O PWM é implementado por software e atualizado a cada tick do Timer0; o período é configurado em pwm_init(255). Para simulação, utilize potenciômetros nos canais analógicos indicados e leds/motores conectados a RC0..RC3.