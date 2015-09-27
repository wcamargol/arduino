/**
* Programa que le e interpreta os comandos enviados pelo servidor e tambem monitora os estados
* de sensores e dispositivos
*
*
*
*
*/

/**
* Varivaveis globais
*/
int  pino;              /**variavel que recebera o numero do pino ate 999*/ 
char comando;           /** variavel que recebera o comando L para liga ou D para desliga*/
String dispositivo;     /**variavel que recebera o dispositivo (lampada, motor, etc) que sera desligado ou ligado*/
#define liga 'L'         /**define o comando liga como L*/
#define desliga  'D'      /**define o comando desliga como D*/
/**
* No caso de placas de reles, o arduino define o estado HIGH (1) para desligado e o estado LOW (0) para ligado
*/
#define ligado  0 
#define desligado  1

/**
* Procedimento para (re)inicializar as variaveis globais
*/
void limparComando() {
  dispositivo = "";     /**nenhum dispositivo ou equipamento*/
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
  String cmd;
  /** 
  * variavel local que recebera cada caracter que chega 
  * pela porta serial sera concatenado a string comando
  */  
  char c;            
  
  /** Enquanto houver caracteres na porta serial*/
  while(Serial.available() > 0) {
    /** le  o caracter da porta serial*/
    c = Serial.read();
    /** Nao le o caractere de quebra de linha*/
    if (c != '\n'){
      /** 
      * Concatena os caracteres na string comando
      * formando a string enviada pelo servidor
      */
      cmd.concat(c);
    }
    /** Aguarda a porta serial ler próximo caractere*/
    delay(10);
  }
  
  /** 
  * Pega o dispositivo que sera usado para o feedback da operacao
  * em seguida pega o comando e o numero do pino a ser operado
  */
  dispositivo = cmd.substring(0,4);
  comando = cmd.charAt(4);
  pino = cmd.substring(5).toInt();
}

/**
* Funcao que verfica se a operacao (comando) deve ser realizada
* e retorna verdadeiro ou falso. Esta funcao determina o estado 
* do pino e retorna verdadeiro (true) para a execucao do comando
* e falso(false) para a nao execucao do comando.
*/
boolean devoExCmd() {
  /**
  * Le o estado do pino
  */
  int estadoPino = digitalRead(pino);
  /**
  * se o estado do pino esta ligado
  */
  if (estadoPino == ligado) {
    /**
    * e se o comando for para ligar
    */
    if (comando == liga) {
      /**
      * retorne falso e nao executa o comando
      */
      Serial.println(dispositivo + " comando nao executado");      
      return false;      
      /**
      * senao retorne verdadeiro se o comando
      * for para desligar
      */
    } else if (comando == desliga) {
      return true;
    }
    /**
    * senao, se o estado do pino esta desligado
    */
  } else if (estadoPino == desligado) {
    /**
    * e se o comando for para desligar
    */
    if (comando == desliga) {
      /**
      * retorne falso e nao executa o comando
      */  
      Serial.println(dispositivo + " comando nao executado"); 
      return false;      
      /**
      * senao retorne verdadeiro se o comando
      * for para ligar
      */
    } else if (comando == liga) {       
      return true;
    }
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
  String feedback;
  /** 
  * Executa o comando conforme solicitado liga ou desliga
  */  
  if (comando == liga) {  
    digitalWrite(pino, LOW);    
  } else if (comando == desliga) {
    digitalWrite(pino, HIGH);
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
    feedback = dispositivo + " - Ligado";
    Serial.println(feedback);   
  }else if (estado == desligado && comando == desliga){
    feedback = dispositivo + " - Desligado";
    Serial.println(feedback);
  }
}



void setup() {
  /**
  Define o pino 8 como saida e inicializa a placa com o rele desligado
  HIGH : rele desligado
  LOW : rele ligado
  */
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH );
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH );
  
  /**
  dDefine o pino 7 como entrada que recebera o sinal do interruptor e
  faz a leitura do sinal, passando para o arduino o estado do
  interruptor
  */
  /**pinMode(7, INPUT);
  /**interruptor = digitalRead(7);
  /*
  Define a velocidade da comunicacao serial em 9600bps
  */
  Serial.begin(9600);
  limparComando();  
}
  
void loop() {
  /**Se receber algo pela serial*/
  if (Serial.available() > 0){
    /**
    * reinicializa as variaveis e le a operacao (comando)
    */
    limparComando();    
    lerComando(); 
    if (devoExCmd()){       
      executarComando();
    }
  }
}