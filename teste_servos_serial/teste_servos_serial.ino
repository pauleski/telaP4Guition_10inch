#include <Servo.h>

// Instanciando os objetos Servo
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void setup() {
  // Inicializa a comunicação serial com o PC via cabo USB
  Serial.begin(9600);
  
  // Configurando os pinos dos servos
  servo1.attach(9);
  servo2.attach(8);
  servo3.attach(7);
  servo4.attach(6);
  
  // Inicializa na posição 0 graus
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);

  Serial.println("==================================================");
  Serial.println("     Teste de Servos - Controle pelo Monitor");
  Serial.println("==================================================");
  Serial.println("Formato do comando: S<numero_servo>:<angulo>");
  Serial.println("Exemplos: ");
  Serial.println("  S1:90  -> Move o Servo 1 para 90 graus");
  Serial.println("  S2:180 -> Move o Servo 2 para 180 graus");
  Serial.println("  S3:0   -> Move o Servo 3 para 0 graus");
  Serial.println("==================================================");
}

void loop() {
  // Verifica se o usuário enviou algum dado pelo Monitor Serial
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim(); // Remove espaços extras e quebra de linha
    comando.toUpperCase(); // Padroniza para maiúsculo (permite aceitar 's1:90')
    
    if (comando.length() > 0) {
      // Verifica o formato esperado, ex: "S1:180"
      if (comando.startsWith("S") && comando.indexOf(':') > 0) {
        
        // Pega o número do servo (texto entre o 'S' e os dois pontos ':')
        int servoNum = comando.substring(1, comando.indexOf(':')).toInt();
        
        // Pega o ângulo (texto após os dois pontos ':')
        int angulo = comando.substring(comando.indexOf(':') + 1).toInt();
        
        // Garante que o ângulo esteja nos limites físicos do servo
        if (angulo >= 0 && angulo <= 180) {
          switch (servoNum) {
            case 1:
              servo1.write(angulo);
              Serial.print(">>> Servo 1 movido para ");
              Serial.println(angulo);
              break;
            case 2:
              servo2.write(angulo);
              Serial.print(">>> Servo 2 movido para ");
              Serial.println(angulo);
              break;
            case 3:
              servo3.write(angulo);
              Serial.print(">>> Servo 3 movido para ");
              Serial.println(angulo);
              break;
            case 4:
              servo4.write(angulo);
              Serial.print(">>> Servo 4 movido para ");
              Serial.println(angulo);
              break;
            default:
              Serial.println("Erro: Servo inválido! Escolha entre S1, S2, S3 ou S4.");
              break;
          }
        } else {
          Serial.println("Erro: Ângulo inválido! Escolha um valor entre 0 e 180.");
        }
      } else {
        Serial.println("Erro: Formato inválido! Tente enviar no formato 'S1:90'.");
      }
    }
  }
}
