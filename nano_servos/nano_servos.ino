#include <Servo.h>
#include <SoftwareSerial.h>

// Configuração da porta serial por software para comunicar com o ESP
// Conecte o TX do ESP no RX (D2) do Nano, e RX do ESP no TX (D3) do Nano
// Lembre-se de usar um divisor de tensão no RX do ESP se o Nano for 5V!
SoftwareSerial ESPSerial(2, 3); // RX (Pino 2), TX (Pino 3)

// Instanciando os objetos Servo
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void setup() {
  // Serial hardware para debug no monitor serial do PC
  Serial.begin(9600);
  
  // Inicializando a porta SoftwareSerial com 9600 baud rate (mesmo do ESPHome)
  ESPSerial.begin(9600);
  
  // Configurando os pinos dos servos
  servo1.attach(6);
  servo2.attach(7);
  servo3.attach(8);
  servo4.attach(9);
  
  // Inicializa na posição 0 graus
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);

  Serial.println("Arduino Nano iniciado. Aguardando comandos do ESP...");
}

void loop() {
  // Verifica se há dados disponíveis da serial do ESP
  if (ESPSerial.available()) {
    // Lê a string até encontrar a quebra de linha '\n'
    String comando = ESPSerial.readStringUntil('\n');
    comando.trim(); // Remove espaços e quebras de linha invisíveis
    
    if (comando.length() > 0) {
      Serial.print("Comando recebido: ");
      Serial.println(comando);
      
      // Mapeamento dos comandos
      if (comando == "SERVO1_ON") {
        servo1.write(180); // Move para a posição acionada
      } 
      else if (comando == "SERVO1_OFF") {
        servo1.write(0);   // Retorna à posição inicial
      }
      else if (comando == "SERVO2_ON") {
        servo2.write(180);
      } 
      else if (comando == "SERVO2_OFF") {
        servo2.write(0);
      }
      else if (comando == "SERVO3_ON") {
        servo3.write(180);
      } 
      else if (comando == "SERVO3_OFF") {
        servo3.write(0);
      }
      else if (comando == "SERVO4_ON") {
        servo4.write(180);
      } 
      else if (comando == "SERVO4_OFF") {
        servo4.write(0);
      }
    }
  }
}
