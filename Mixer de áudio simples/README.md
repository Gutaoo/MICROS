Este projeto tinha o objetivo de criar um mixer de áudio de 2 canais, usando o ADC (Analog to Digital Converter) e DAC (Digital to Analog Converter) do microcontrolador stm32f411. Foi usado o DMA (Direct Memory Access) 
nos dois periféricos, para a leitura e escrita ser feita de forma independente do processador do dispositivo.

De forma geral, o mixer de áudio tem a função de combinar diferentes canais de entrada de áudio em uma só saída. Este projeto implementa uma versão simplificada do dispositivo, os 2 sinais de entrada são apenas somados na
saída. Para isso ser possível, convertemos o sinal de áudio (analógico) em um sinal digital correspontende, somamos os dois sinais digitais, e por fim convertemos a soma para um sinal analógico novamente, assim temos o 
sinal de áudio correspondente aos 2 canais juntos.
