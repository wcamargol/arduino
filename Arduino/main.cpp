/**
* Programa que le e interpreta os comandos enviados pelo servidor e tambem monitora os estados
* de sensores e dispositivos
*/

/**
* Varivaveis globais
*/
int  pino;
char comando;                     /** variavel que recebera o comando L para liga ou D para desliga*/
float R0;                         /** variavel que define RO para calculo da relacao RS/R0*/
float relacaoGases;               /** varivale que recebe o valor da leitura do sensor RS_gas divido por RO */
float ppmGLP;

#define pinoA0 A0                  /**define o pino da placa que recebera os valores da concentracao de gas*/
#define qtdLeituraCalibracao 1   /**define a quantidade de leituras feitas durannte a calibracao*/

#define liga 'L'                  /**define o comando liga como L*/
#define desliga  'D'              /**define o comando desliga como D*/
#define pulsar 'P'                /**define o comando pulsar com P*/

/**
* No caso de placas de reles, o arduino define o estado HIGH (1) para desligado e o estado LOW (0) para ligado
*/
#define ligado  0 
#define desligado  1

/**
* Metodo que retorna o valor da resistencia de sensoreamento
* dado o valor lido na entrada analgica
*/

float resistorSensorGas(int dadoAnalogico){ 
  return (float)1023/dadoAnalogico-1;
}

/**
* Metodo para calibrar o sensor e definir R0 e RS
*/
void calibrarSensorGas(){
  int i, dadoAnalogico = 0;
  R0 = 0.0;
  for(i=0; i < qtdLeituraCalibracao; i++){
    dadoAnalogico += analogRead(pinoA0);
    delay(500);
  }
  dadoAnalogico = dadoAnalogico/qtdLeituraCalibracao;
  R0 = resistorSensorGas(dadoAnalogico)/9.8;
}

float concentracaoGLP(){
  /**
  * calcula a relacao de entre a resistencia do sensor Rs e Ro
  */
  relacaoGases = resistorSensorGas(analogRead(pino))/R0; 
  
  /**
  * Calcula a concentracao de gas GLP no ambiente
  */
  return pow(10,(1.27-log10(relacaoGases))/0.47);
}

/**
* Procedimento para (re)inicializar as variaveis globais de comando
*/
void limparComando() {
  pino = -1;            /**nenhum pino*/  
  comando = NULL;       /**nenhum comando a ser executado*/
}

/**
 * Procedimento para ler uma string da porta serial
 * verificar o dispositivo, o comando e o pino e 
 * atribuir os valores as variaveis corretas
 */
void lerString(){
  /** 
  * variavel local que recebera temporarimente
  * a string (comando) da porta serial
  */
  String str;
  
  /** Enquanto houver caracteres na porta serial*/
  while (Serial.available()){
    /**delay necessario para chegada do byte pela porta*/
    delay(1);
    /** le  o caracter da porta serial*/
    char c = Serial.read();
    /** 
      * Concatena os caracteres na variavel str
      * formando a string enviada pelo servidor
      */
    str += c;
    /**muda as letras para maiusculo*/
    str.toUpperCase();
    /**retira os espacos em branco*/
    str.trim();
  }
  
  if (str.charAt(0) == '?'){
    pino = str.substring(1).toInt();
    Serial.println(concentracaoGLP());
  }else{
    /** 
    * Pega o dispositivo que sera usado na operacao
    * em seguida pega o comando e o numero do pino a ser operado
    */
    comando = str.charAt(0);
    pino = str.substring(1).toInt();    
    executarComando();
  }
}

/**
* Procedimento que executa o comando enviado para o arduino
* e retorna um feedback da operacao
*/
void executarComando() {
  /** 
  * variavel local que verificara o estado ligado ou desligado do pino 
  */
  int estado;          
  /** 
  * variavel local que armazena o feedback dado ao usuario 
  * inicializada com uma mensagem de comando no executado
  */
  String feedbackOperacao;
  
  pinMode(pino,OUTPUT);
  
  /** 
  * Executa o comando conforme solicitado liga, desliga ou pulso
  */  
  if (comando == liga) {  
    digitalWrite(pino, LOW);    
  } else if (comando == desliga) {
    digitalWrite(pino, HIGH);
  } else if (comando == pulsar){
    digitalWrite(pino, LOW);
    delay(100);
    digitalWrite(pino,HIGH);
  }
  /**
  * Le o estado do pino apos a execucao do comando
  */
  estado = digitalRead(pino);
  /**
  * escreve o feedback para o usuario conforme o estado e a operacao 
  * que foi solicitada e coloca o feedback na porta serial
  */
  if (estado == ligado && comando == liga) {
    feedbackOperacao = "L";
    Serial.println(feedbackOperacao);   
  }else if (estado == desligado && comando == desliga){
    feedbackOperacao = "D";
    Serial.println(feedbackOperacao);
  }
  delay(100);
  /**
  * reinicializa as variaveis e le a operacao (comando)
  */    
  limparComando();
}



void setup() {  
  /**
  Faz a calibracao do sensor de gas
  */
  calibrarSensorGas();
  
  /**
  Inicializa as variaveis que atuam sobre as saidas do Arduino
  */
  limparComando();
  
  /**
  Define a velocidade da comunicacao serial em 9600bps
  */
  Serial.begin(9600);   
}
  
void loop(){
  /**Se receber algo pela serial*/
  while (Serial.available() > 0){ 
    /*
    * le e executa o comando
    */   
    lerString();        
 }
}

