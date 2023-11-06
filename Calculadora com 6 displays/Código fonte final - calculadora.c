/*
******************************************************************************
*@authors: Augusto Blauth Schneider
  * @date    setembro,2022
   * @info: 	calculadora com teclado matricial e 6 displays de 7 segmentos
   *

         	 PB7  PB5  PB3  PB1
               C0  C1  C2  C3
      PB6 L0    1   2   3   A
      PB4 L1    4   5   6   B
      PB2 L2    7   8   9   C
      PB0 L3    *   0   #   D

******************************************************************************
*/

#include "stm32f4xx.h"

#define coluna_0 2
#define coluna_1 3
#define coluna_2 0
#define coluna_3 1

//config timer multiplexação dos displays
#define PSC_11 1599
#define ARR_11 10

//config timer multiplicação do teclado
#define PSC_10 159
#define ARR_10 99

int mux_disp[]={
		0b0000010000000, //disp_1
		0b0000100000000, //disp_2
		0b0001000000000, //disp_3
		0b0010000000000, //disp_4
		0b0100000000000, //disp_5
		0b1000000000000, //disp_6
};

uint16_t numeros[]={
		0b1111110111111, //0.
		0b1111110000110, //1.
		0b1111111011011, //2.
		0b1111111001111, //3.
		0b1111111100110, //4.
		0b1111111101101, //5.
		0b1111111111101, //6.
		0b1111110000111, //7.
		0b1111111111111, //8.
		0b1111111100111, //9.
		0b1111111000000, //traço (10)
		0b1111110000000, //nada (11)
		0b1111111110000, //p formar o + (12)
		0b1111111110110, //p formar o X (13)
		0b1111111010010  //p formar o / (14)
};

#define DESATIVA 0x00aa      // 1010 1010
#define NCOLUNAS 4

//vetor com as saidas conectadas as colunas do teclado
const uint16_t acionaColuna[NCOLUNAS]={
						0xff7f,
						0xffdf,
						0xfff7,
						0xfffd
						};

// MAQUINA DE ESTADOS CALCULADORA
enum {Escrevendo,
	Somando,
	Subtraindo,
	Multiplicando,
	Dividindo,
	Resultado,
	Zerando
}STATE_calc;

//VARIAVEIS GLOBAIS
int disp_1_antes=0, disp_2_antes=0, disp_3_antes=0, disp_4_antes=0, disp_5_antes=0, disp_6_antes=11, disp_1_atual=0, disp_2_atual=0, disp_3_atual=0, disp_4_atual=0, disp_5_atual=0, disp_6_atual=11;
int coluna=0;
int total_1=0, total_2=0;

// FUNÇÂO ESCREVE NUMERO
void escreve_num (int num){
	//escreve no primeiro display
	disp_1_atual = num;
	//passa o numero para o display do lado
	disp_2_atual = disp_1_antes;
	disp_3_atual = disp_2_antes;
	disp_4_atual = disp_3_antes;
	disp_5_atual = disp_4_antes;
}

// FUNÇÂO ATUALIZA DISPLAYS
void atualiza_disp (){
	//atualiza o valor do disp_antes
	disp_1_antes = disp_1_atual;
	disp_2_antes = disp_2_atual;
	disp_3_antes = disp_3_atual;
	disp_4_antes = disp_4_atual;
	disp_5_antes = disp_5_atual;
	disp_6_antes = disp_6_atual;
}

//FUNÇÃO ZERA DISPLAYS
void zera_disp(){
	disp_1_atual = 0;
	disp_2_atual = 0;
	disp_3_atual = 0;
	disp_4_atual = 0;
	disp_5_atual = 0;
	disp_6_atual = 11;
}

//FUNÇÃO DETECTA NUMERO PARA TROCA DE ESTADO (OPERAÇÂO - ESCREVENDO)
void detecta_num(int col, int l0, int l1, int l2, int l3){
	if(col==coluna_0||col==coluna_1||col==coluna_2){
		if(!l0){
			zera_disp();
			atualiza_disp();
			STATE_calc = Escrevendo;
		}
		if(!l1){
			zera_disp();
			atualiza_disp();
			STATE_calc = Escrevendo;
		}
		if(!l2){
			zera_disp();
			atualiza_disp();
			STATE_calc = Escrevendo;
		}
	}
	if(col==coluna_0){
		if(!l3){
			STATE_calc = Zerando;
		}
	}
	if(col==coluna_1){
		if(!l3){
			zera_disp();
			atualiza_disp();
			STATE_calc = Escrevendo;
		}
	}
}

//FUNÇÃO SALVA FATOR
void salva_fator(int aux){
	if(aux==1){
		 if(disp_6_antes==10){ //se o numero for negativo
			 total_1 = (((disp_5_antes*10000)+(disp_4_antes*1000)+(disp_3_antes*100)+(disp_2_antes*10)+disp_1_antes)*-1); //salva primeiro fator
		 }
		 else{ //se o numero for positivo
			 total_1 = ((disp_5_antes*10000)+(disp_4_antes*1000)+(disp_3_antes*100)+(disp_2_antes*10)+disp_1_antes); //salva primeiro fator
		 }
	}
	else{
		total_2 = ((disp_5_antes*10000)+(disp_4_antes*1000)+(disp_3_antes*100)+(disp_2_antes*10)+disp_1_antes); //salva segundo fator
	}
}

//FUNÇÃO ESCREVE RESULTADO
void distribui_result(int result){
	 disp_1_atual = (((((result)%10000)%1000)%100)%10);
	 disp_2_atual = (((((result)%10000)%1000)%100)/10);
	 disp_3_atual = (((result)%10000)%1000/100);
	 disp_4_atual = (((result)%10000)/1000);
	 disp_5_atual = ((result)/10000);

	 atualiza_disp();
}

void mostra_soma(){
	disp_1_atual = 12;
	disp_2_atual = 10;
	disp_3_atual = 11;
	disp_4_atual = 11;
	disp_5_atual = 11;
	disp_6_atual = 11;
}
void mostra_sub(){
	disp_1_atual = 10;
	disp_2_atual = 11;
	disp_3_atual = 11;
	disp_4_atual = 11;
	disp_5_atual = 11;
	disp_6_atual = 11;
}
void mostra_mult(){
	disp_1_atual = 13;
	disp_2_atual = 11;
	disp_3_atual = 11;
	disp_4_atual = 11;
	disp_5_atual = 11;
	disp_6_atual = 11;
}
void mostra_div(){
	disp_1_atual = 14;
	disp_2_atual = 11;
	disp_3_atual = 11;
	disp_4_atual = 11;
	disp_5_atual = 11;
	disp_6_atual = 11;
}
int main(void)
{

	// Configurando o GPIO
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN;

	// Configurando entradas
	GPIOB->MODER &=~(GPIO_MODER_MODER6|GPIO_MODER_MODER4|GPIO_MODER_MODER2|GPIO_MODER_MODER0);
	// Colocando Pull-up
	GPIOB->PUPDR &=~(GPIO_PUPDR_PUPDR6|GPIO_PUPDR_PUPDR4|GPIO_PUPDR_PUPDR2|GPIO_PUPDR_PUPDR0);
	GPIOB->PUPDR |=(GPIO_PUPDR_PUPDR6_0|GPIO_PUPDR_PUPDR4_0|GPIO_PUPDR_PUPDR2_0|GPIO_PUPDR_PUPDR0_0);

	// Configurando saidas
	//saidas display
	GPIOC->MODER &=~(GPIO_MODER_MODER0|GPIO_MODER_MODER1|GPIO_MODER_MODER2|GPIO_MODER_MODER3|GPIO_MODER_MODER4|GPIO_MODER_MODER5|GPIO_MODER_MODER6|GPIO_MODER_MODER7|GPIO_MODER_MODER8|GPIO_MODER_MODER9|GPIO_MODER_MODER10|GPIO_MODER_MODER11|GPIO_MODER_MODER12);
	GPIOC->MODER |=(GPIO_MODER_MODER0_0|GPIO_MODER_MODER1_0|GPIO_MODER_MODER2_0|GPIO_MODER_MODER3_0|GPIO_MODER_MODER4_0|GPIO_MODER_MODER5_0|GPIO_MODER_MODER6_0|GPIO_MODER_MODER7_0|GPIO_MODER_MODER8_0|GPIO_MODER_MODER9_0|GPIO_MODER_MODER10_0|GPIO_MODER_MODER11_0|GPIO_MODER_MODER12_0);

	//saidas teclado
	GPIOB->MODER &=~(GPIO_MODER_MODER7|GPIO_MODER_MODER5|GPIO_MODER_MODER3|GPIO_MODER_MODER1);
	GPIOB->MODER |= (GPIO_MODER_MODER7_0|GPIO_MODER_MODER5_0|GPIO_MODER_MODER3_0|GPIO_MODER_MODER1_0);
	// Configurando saidas como open-drain
	GPIOB->OTYPER |= (GPIO_OTYPER_OT_7|GPIO_OTYPER_OT_5|GPIO_OTYPER_OT_3|GPIO_OTYPER_OT_1);


	// Configurando o TIM10 e o TIM11
	RCC->APB2ENR |= RCC_APB2ENR_TIM10EN|RCC_APB2ENR_TIM11EN;
	// Configurar o TIM10 - piscada do pont
	TIM10->PSC=PSC_10;
	TIM10->ARR=ARR_10;
	TIM10->CR1 |= TIM_CR1_CEN; // Habilitando a contagem

	//tim11 -> multiplexagem display
		TIM11->PSC=PSC_11;
		TIM11->ARR=ARR_11;
		TIM11->CR1|=TIM_CR1_CEN;

	//variáveis
	int L0_col0_atual=0, L1_col0_atual=0, L2_col0_atual=0, L3_col0_atual=0, L0_col0_antes=0, L1_col0_antes=0, L2_col0_antes=0;
	int L0_col1_atual=0, L1_col1_atual=0, L2_col1_atual=0, L3_col1_atual=0, L0_col1_antes=0, L1_col1_antes=0, L2_col1_antes=0, L3_col1_antes=0;
	int L0_col2_atual=0, L1_col2_atual=0, L2_col2_atual=0, L3_col2_atual=0, L0_col2_antes=0, L1_col2_antes=0, L2_col2_antes=0;
	int L0_col3_atual=0, L1_col3_atual=0, L2_col3_atual=0, L3_col3_atual=0;
	int ope=0, result=0;
	int cont_disp=-1;

	STATE_calc = Escrevendo; //estado inicial
  while (1)
  {
	  L0_col0_atual = L0_col1_atual = L0_col2_atual = L0_col3_atual = GPIOB->IDR &0x40;  //PB6
	  L1_col0_atual = L1_col1_atual = L1_col2_atual = L1_col3_atual = GPIOB->IDR &0x10;  //PB4
	  L2_col0_atual = L2_col1_atual = L2_col2_atual = L2_col3_atual = GPIOB->IDR &0x4;   //PB2
	  L3_col0_atual = L3_col1_atual = L3_col2_atual = L3_col3_atual = GPIOB->IDR &0x1;   //PB0


	  if(TIM11->SR & TIM_SR_UIF){
	 		  TIM11->SR &=~TIM_SR_UIF;
	 		  cont_disp++;

	 		 if(cont_disp==0){
	 			 GPIOC->ODR&=~0x1FFF; //limpa o display
	 			 GPIOC->ODR|=~(numeros[disp_1_atual])|(mux_disp[cont_disp]); //escreve no display da unidade (PC7 esta em 1, o resto em 0))
	 		 }
	 		 if(cont_disp==1){
	 			 GPIOC->ODR&=~0x1FFF; //limpa o display
	 		 	 GPIOC->ODR|=~(numeros[disp_2_atual])|(mux_disp[cont_disp]); //escreve no display da dezena (PC8 esta em 1, o resto em 0))
	 		 }
	 		 if(cont_disp==2){
	 			 GPIOC->ODR&=~0x1FFF; //limpa o display
	 			 GPIOC->ODR|=~(numeros[disp_3_atual])|(mux_disp[cont_disp]); //escreve no display da centena (PC9 esta em 1, o resto em 0)
	 		 }
	 		 if(cont_disp==3){
	 			 GPIOC->ODR&=~0x1FFF; //limpa o display
	 			 GPIOC->ODR|=~(numeros[disp_4_atual])|(mux_disp[cont_disp]); //escreve no display do milhar (PC10 esta em 1, o resto em 0))
	 		 }
	 		 if(cont_disp==4){
	 			 GPIOC->ODR&=~0x1FFF; //limpa o display
	 			 GPIOC->ODR|=~(numeros[disp_5_atual])|(mux_disp[cont_disp]); //escreve no display da dezena de milhar (PC11 esta em 1, o resto em 0))
	 		 }
	 		 if(cont_disp==5){
	 			 GPIOC->ODR&=~0x1FFF; //limpa o display
	 			 GPIOC->ODR|=~(numeros[disp_6_atual])|(mux_disp[cont_disp]); //escreve no display do traço para numeros negativos (PC12 esta em 1, o resto em 0))

	 			 cont_disp=-1;
	 		 }
	 	  }

	  if (TIM10->SR & TIM_SR_UIF)
	  {
		  TIM10->SR &=~TIM_SR_UIF;

		  //acionando uma coluna por vez
		 GPIOB->ODR |= DESATIVA;
		 GPIOB->ODR &= acionaColuna[coluna];

		 //mantem o ciclo
		 if (++coluna==NCOLUNAS)
		 {
			 coluna=0;
		 }

		 switch(STATE_calc){
		 	 case Escrevendo:{
		 		 //VERIFICANDO COLUNA 0
		 		 if(coluna==coluna_0){
		 			 if(L0_col0_atual!=L0_col0_antes){			//LINHA 0 || COLUNA 0 -> DIGITA 1
		 				 L0_col0_antes=L0_col0_atual;
		 				 	 if(!L0_col0_atual){
		 				 		 escreve_num(1);
		 				 	 }
		 			 }
		 			 if(L1_col0_atual!=L1_col0_antes){			//LINHA 1 || COLUNA 0 -> DIGITA 4
		 				 L1_col0_antes=L1_col0_atual;
		 				 	 if(!L1_col0_atual){
		 				 		 escreve_num(4);
		 				 	 }
		 			 }
		 			 if(L2_col0_atual!=L2_col0_antes){			//LINHA 2 || COLUNA 0 -> DIGITA 7
		 				 L2_col0_antes=L2_col0_atual;
		 				 	 if(!L2_col0_atual){
		 				 		 escreve_num(7);
		 				 	 }
		 			 }
		 			 if(!L3_col0_atual){			//MUDANÇA DE ESTADO PARA ZERANDO (*)
		 				 STATE_calc = Zerando;
		 			 }
		 			 //atualiza o valor do disp_antes
		 			 atualiza_disp();
		 		 }
		 		 //VERIFICANDO COLUNA 1
		 		 if(coluna==coluna_1){
		 			 if(L0_col1_atual!=L0_col1_antes){			//lINHA 0 || COLUNA 1 -> DIGITA 2
		 				 L0_col1_antes=L0_col1_atual;
		 				 	 if(!L0_col1_atual){
				 				 escreve_num(2);
		 				 	 }
		 			 }
		 			 if(L1_col1_atual!=L1_col1_antes){     		//lINHA 1 || COLUNA 1 -> DIGITA 5
		 				 L1_col1_antes=L1_col1_atual;
		 				 	 if(!L1_col1_atual){
		 				 		 escreve_num(5);
		 				 	 }
		 			 }
		 			 if(L2_col1_atual!=L2_col1_antes){          //lINHA 2|| COLUNA 1 -> DIGITA 8
		 				 L2_col1_antes=L2_col1_atual;
		 				 	 if(!L2_col1_atual){
		 				 		 escreve_num(8);
		 				 	 }
		 			 }
		 			 if(L3_col1_atual!=L3_col1_antes){     	    //lINHA 3 || COLUNA 1 -> DIGITA 0
		 				 L3_col1_antes=L3_col1_atual;
		 				 	 if(!L3_col1_atual){
		 				 		 escreve_num(0);
		 				 	 }
		 			 }
		 			 //atualiza o valor do disp_antes
		 			 atualiza_disp();
		 		 }
		 		 //VERIFICANDO COLUNA 2
		 		 if(coluna==coluna_2){
		 			 if(L0_col2_atual!=L0_col2_antes){			//LINHA 0 || COLUNA 2 -> DIGITA 3
		 				 L0_col2_antes=L0_col2_atual;
		 				 	 if(!L0_col2_atual){
		 				 		 escreve_num(3);
		 				 		 atualiza_disp();
		 				 	 }
		 			 }
		 			 if(L1_col2_atual!=L1_col2_antes){     		//lINHA 1 || COLUNA 2 -> DIGITA 6
		 				 L1_col2_antes=L1_col2_atual;
		 				 	 if(!L1_col2_atual){
		 				 		 escreve_num(6);
		 				 		 atualiza_disp();
		 				 	 }
		 			 }
		 			 if(L2_col2_atual!=L2_col2_antes){			//lINHA 2 || COLUNA 2 -> DIGITA 9
		 				 L2_col2_antes=L2_col2_atual;
		 				 	 if(!L2_col2_atual){
		 				 		 escreve_num(9);
		 				 		 atualiza_disp();
		 				 	 }
		 			 }
		 			 if(!L3_col2_atual){			//MUDANÇA DE ESTADO PARA RESULTADO (#)
		 				 STATE_calc = Resultado;
		 			 }
		 		 }
		 		 //VERIFICANDO COLUNA 3
		 		 if(coluna==coluna_3){
		 			 if(!L0_col3_atual){			//MUDANÇA DE ESTADO PARA SOMANDO (A)
		 				 STATE_calc = Somando;
		 			 }
		 			 if(!L1_col3_atual){			//MUDANÇA DE ESTADO PARA SUBTRAINDO (B)
		 				 STATE_calc = Subtraindo;
		 			 }
		 			 if(!L2_col3_atual){			//MUDANÇA DE ESTADO PARA MULTIPLICANDO (C)
		 				 STATE_calc = Multiplicando;
		 			 }
		 			 if(!L3_col3_atual){			//MUDANÇA DE ESTADO PARA DIVIDINDO (D)
		 				 STATE_calc = Dividindo;
		 			 }
		 		 }
		 		 break;
		 	 }

		 	 case Zerando:{
		 		 //zera todas as variáveis da calculadora
		 		 zera_disp();
		 		 total_1 = 0;
		 		 total_2 = 0;
		 		 ope = 0;
		 		 result = 0;
		 		 atualiza_disp();

		 		 //retorna ao estado de escrita
		 		 STATE_calc = Escrevendo;
		 		 break;
		 	 }

		 	 case Somando:{
		 		 ope = 1; //ope = 1 -> soma
		 		 salva_fator(1); //salvar primeiro fator
		 		 //escreve +
		 		 mostra_soma();

		 		 //mudança de estado
		 		 detecta_num(coluna, L0_col0_atual, L1_col0_atual, L2_col0_atual, L3_col0_atual);
		 		 break;
		 	 }

		 	 case Subtraindo:{
		 		 ope = 2; //ope = 2 -> subtração
		 		 salva_fator(1); //salvar primeiro fator
		 		 //escreve -
		 		 mostra_sub();

		 		 //mudança de estado
		 		 detecta_num(coluna, L0_col0_atual, L1_col0_atual, L2_col0_atual, L3_col0_atual);
		 		 break;
		 	 }

		 	 case Multiplicando:{
		 		 ope = 3; //ope = 3 -> multiplicação
		 		 salva_fator(1); //salvar primeiro fator
		 		 //escreve X
		 		 mostra_mult();

		 		 //mudança de estado
		 		 detecta_num(coluna, L0_col0_atual, L1_col0_atual, L2_col0_atual, L3_col0_atual);
		 		 break;
		 	 }

		 	 case Dividindo:{
		 		 ope = 4; //ope = 4 -> divisão
		 		 salva_fator(1); //salvar primeiro fator
		 		 //escreve /
		 		 mostra_div();

		 		 //mudança de estado
		 		 detecta_num(coluna, L0_col0_atual, L1_col0_atual, L2_col0_atual, L3_col0_atual);
		 		 break;
		 	 }

		 	 case Resultado:{
		 		 salva_fator(2); //salvar segundo fator

		 		 //se for soma
		 		 if(ope==1){
		 			 if((total_1+total_2)<0){  //se a soma resulta em um numero negativo
		 				   disp_6_atual = 10; //coloca sinal de negativo
		 				   result = (((total_1)*-1)-total_2); //refaz a conta com o resultado em módulo
		 			 }
		 			 else{
		 				   disp_6_atual = 11;
		 				   result = (total_1+total_2); //conta normal
		 			 }
		 			 distribui_result(result);
		 		 }
		 		 //se for subtração
		 		 if(ope==2){
		 			 if((total_1-total_2)<0){//se a subtração resulta em um numero negativo
		 				   disp_6_atual = 10; //coloca sinal de negativo

		 					if(total_1<0){ //se o primeiro fator for um numero negativo
		 					 	result = (((total_1)*-1)+total_2); //refaz a conta com o resultado em módulo
		 					}
		 					else{ //se forem dois numeros positivos com o segundo fator maio que o primeiro
		 					 	result = (total_2-total_1); //refaz a conta com o resultado em módulo
		 					}
		 			 }
		 			 else{
		 				   disp_6_atual = 11;
		 				   result = (total_1-total_2); //conta normal
		 			 }
		 			 distribui_result(result);
		 		 }
		 		 //se for multiplicação
		 		 if(ope==3){
		 			 if((total_1*total_2)<0){ //se a multiplicação resulta em um numero negativo
		 				   disp_6_atual = 10; //coloca sinal de negativo
		 				   result = (((total_1)*-1)*total_2); //refaz a conta com o resultado em módulo
		 			 }
		 			 else{
		 				   disp_6_atual = 11;
		 				   result = (total_1*total_2); //conta normal
		 			 }
		 			 distribui_result(result);
		 		 }
		 		 //se for divisão
		 		 if(ope==4){
		 			 if(total_2==0){//se o divisor for 0
		 				  disp_1_atual = 10;
		 				  disp_2_atual = 10;
		 				  disp_3_atual = 10;
		 				  disp_4_atual = 10;
		 				  disp_5_atual = 10;
		 			 }
		 			 else{
		 				  if((total_1/total_2)<0){ //se a divisão resulta em um numero negativo
		 					 	disp_6_atual = 10; //coloca sinal negativo
		 					 	result = (((total_1)*-1)/total_2); //refaz a conta com o resultado em módulo
		 				  }
		 				  else{
		 					 	disp_6_atual = 11;
		 					 	result = (total_1/total_2); //conta normal
		 				  }
				 		distribui_result(result);
		 			 }
		 		 }
	 			 //zera as variaveis
	 			 ope = 0;
	 			 result = 0;
	 			 total_1 = 0;
	 			 total_2 = 0;

	 			 atualiza_disp();
	 			 //mudança de estado
	 			 if(coluna==coluna_0){
	 				 if(!L3_col0_atual){
	 					 STATE_calc = Zerando;
	 				 }
	 			 }
	 			 if(coluna==coluna_3){
	 				 if(!L0_col3_atual){
	 					 STATE_calc = Somando;
	 				 }
	 				 if(!L1_col3_atual){
	 					 STATE_calc = Subtraindo;
	 				 }
	 				 if(!L2_col3_atual){
	 					 STATE_calc = Multiplicando;
	 				 }
	 				 if(!L3_col3_atual){
	 					 STATE_calc = Dividindo;
	 				 }
	 			 }
	 			 break;
		 	 }
		 }
	 }
  }
}
