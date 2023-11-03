Este projeto tinha o objetivo de realizar o controle de um portão com apenas um botão de acionamento. Como pode ser visto no código, ao pressionar o botão, caso o portão esteja fechado ele abre, caso esteja aberto ele 
fecha, caso esteja em movimento ele para, e caso esteja parado ele retorna para sua ultima posição. Além disso, foi usado o sensor ultrasonico HC-SR04 para medição de distância, caso alguma coisa interrompa o caminho do 
portão o mesmo para. Foi usado um led RGB para indicação visual do estado do portão. 

Foi usado o módulo de ponte H L298N para realizar o controle de rotação do motor do portão (motor DC de 5V). As peças mecânicas para funcionalidade do portão, e o próprio portão, foram impressas em uma impressora 3D.
O projeto foi feito usando a linha de microcontroladores stm32f411. Importante resaltar o uso dos periféricos TIM, Input Capture (IC) e Output Compare (OC), para realizar o acionamento e leitura do sensor HC-SR04, e o 
o uso da função de geração de PWM para controle da cor do LED RGB.
