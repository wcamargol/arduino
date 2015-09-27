//variaveis globais
boolean isOn = false; //variavel global com o estado do led
int buttonState = 0;// variável para armazenar a leitura da porta digital

void setup() {                

  pinMode(2, OUTPUT); // inicializa a porta 4 como porta de saída / escrita.
  digitalWrite(2, HIGH); // escreve o estado da porta 4 como LOW / desligado
  pinMode(7, INPUT); // inicializa a porta 4 como porta de entrada / leitura.
  buttonState = digitalRead(7);   // le o estado da porta digital 7
    Serial.begin(9600); //inicializa a comunicação serial em uma vel. de 9600bps
}

void loop(){
   //lendo comandos da porta serial
   if (Serial.available() > 0) { //informa se tem bytes para serem lidos da porta serial
      char comando = 0;
      comando = (char) Serial.read(); //le um byte da porta serial
      if (comando == 'L')
        isOn = true; //liga o led
      if (comando == 'D')
        isOn = false; //desliga o led
      if (comando == 'I')
        isOn = !isOn; //inverte o valor do estado do LED
  }

  //lendo comando da porta digital
  int newButtonState = 0; // variável para armazenar a leitura da porta
  newButtonState = digitalRead(7); // le o estado da porta digital 7

  //checa se o estado do interruptor mudou
  if (newButtonState != buttonState) {
     buttonState = newButtonState; //atualiza o valor do estado do botao
     isOn = !isOn; //inverte o valor do estado do LED
  }

  if (isOn)
    Serial.println ("LIGADO"); //envia a string via porta serial
  else
    Serial.println ("DESLIGADO"); //envia a string via porta serial

  //liga o LED de acordo com o valor do seu estado.
  if (isOn) {     
    // liga o LED   
    digitalWrite(2, LOW);  
  } 
  else {
    // desliga o LED
    digitalWrite(2, HIGH); 
  }

  //delay (500); //pause de meio segundo 
}