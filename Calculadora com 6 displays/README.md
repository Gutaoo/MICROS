Esse trabalho tinha o objetivo de implementar a multiplexação de hardware em um projeto envolvendo o stm32, minha idéia foi uma calculadora simples
(soma, subtração, multiplicação e divisão). Para a realização foram usados 6 displays de 7 segmentos, que foram multiplexados, ou seja, a transmissão para estes 
divide o mesmo meio físico, fazendo as apropriações no software para organizar a informação e o receptor da mensagem. Além dos displays, um teclado matricial 4x4
foi usado para uso da calculadora, ele foi multiplexado em formato de matriz, como pode ser observado no esquemático do hardware. 

A calculadora trabalha com todos os números inteiros. O usuário pode digitar livremente para formar o número que quiser nos 6 displays, quando é apertado a 
operação a ser realizada há uma indicação visual da operação e o primeiro número é salvo em uma variável, o usuário pode escrever o número que quiser e quando 
aperta o '=' a calculadora mostra o resultado, já possibilitando o resultado como novo fator.

O código do projeto está bem comentado e auto-explicativo.
