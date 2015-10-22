/**
* Programa que le e interpreta os comandos enviados pelo servidor e tambem monitora os estados
* de sensores e dispositivos
*
*Codigo adaptado de http://www.seeedstudio.com/wiki/Grove_-_Gas_Sensor%28MQ2%29
*
*/

/**
* Varivaveis globais
*/
int  pino;                /** variavel que recebera o numero do pino indo ate 999*/ 
char comando;             /** variavel que recebera o comando L para liga ou D para desliga*/
float tensaoSensor;         /** variavel que recebe a leitura do sensor convertido para valor de tensao*/
float R0;                 /** variavel que define RO para calculo da relacao RS/R0*/
float RS_gas;             /** variavel que recebe o valor em tempo de execucao da leitura do sensor */
float rs_R0;              /** varivale que recebe o valor da leitura do sensor RS_gas divido por RO */
float valorSensor;
float ppmGLP;

#define pinoSensorGas A0

#define liga 'L'          /**define o comando liga como L*/
#define desliga  'D'      /**define o comando desliga como D*/
#define pulsar 'P'        /**define o comando pulsar com P*/

/**
* No caso de placas de reles, o arduino define o estado HIGH (1) para desligado e o estado LOW (0) para ligado
*/
#define ligado  0 
#define desligado  1

/**
 Metodo para calibrar o sensor e definir R0 e RS
*/
void calibrarSensorGas(){
  int i;
  valorSensor = 0.0;
  for(i=0; i < 50; i++){
    valorSensor = valorSensor + analogRead(pinoSensorGas);
    delay(500);
  }
  valorSensor = valorSensor/50;
  tensaoSensor = (float)valorSensor/1023*4.37;
  R0 = ((4.37-tensaoSensor)/tensaoSensor)/9.4;
}

/**
* Procedimento para (re)inicializar as variaveis globais
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
void lerComando(){
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
  
  /** 
  * Pega o dispositivo que sera usado para o feedback da operacao
  * em seguida pega o comando e o numero do pino a ser operado
  */
  comando = str.charAt(0);
  pino = str.substring(1).toInt();
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
  * Executa o comando conforme solicitado liga ou desliga
  */  
  if (comando == liga) {  
    digitalWrite(pino, LOW);    
  } else if (comando == desliga) {
    digitalWrite(pino, HIGH);
  } else if (comando == pulsar){
    digitalWrite(pino, HIGH);
    delay(100);
    digitalWrite(pino,LOW);
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
  
  valorSensor = analogRead(pinoSensorGas);
  
  tensaoSensor=(float)valorSensor/1023*4.4;
  RS_gas = (4.4-tensaoSensor)/tensaoSensor;
  
  rs_R0 = RS_gas/R0; 
  
  ppmGLP = pow (10, (1.26 - log(rs_R0))/0.47);
  Serial.print(ppmGLP);
  Serial.println(" ppm");
  
  /**Se receber algo pela serial*/
  if (Serial.available() > 0){    
    lerComando();
    executarComando();
  }
  delay(500);  
}