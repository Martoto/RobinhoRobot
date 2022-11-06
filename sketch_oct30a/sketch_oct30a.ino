#include <SoftwareSerial.h>
#include <Servo.h>
#define SERVO 3
#define M1P1 5
#define M1P2 6
#define M2P1 10
#define M2P2 9

SoftwareSerial espSerial(13,12); //RTX, TX
Servo myservo;
void setup() {
  // put your setup code here, to run once:
  espSerial.begin(9600);
  Serial.begin(9600);

  pinMode(M1P1, OUTPUT);
  pinMode(M1P2, OUTPUT);
  pinMode(M2P1, OUTPUT);
  pinMode(M2P2, OUTPUT);

  myservo.attach(SERVO);
  myservo.write(0);
  
  delay(10);
  espSerial.print("a");
}

void setMotor(char mov)
{
  
}

void loop() {
  char teste;
  if(espSerial.available())
  {
    teste = espSerial.read();
    switch (teste)
    {
    case 'a':
      digitalWrite(M1P1, LOW);
      digitalWrite(M1P2, HIGH);
      digitalWrite(M2P1, HIGH);
      digitalWrite(M2P2, LOW);
      myservo.write(0);
      delay(1000);
      espSerial.write('w');
      
      Serial.print(teste);
      Serial.print(" -> ");
      Serial.println("w");
      break;
    case 'w':
      digitalWrite(M1P1, HIGH);
      digitalWrite(M1P2, LOW);
      digitalWrite(M2P1, HIGH);
      digitalWrite(M2P2, LOW);
      myservo.write(0);
      delay(1000);
      espSerial.write('s');
      
      Serial.print(teste);
      Serial.print(" -> ");
      Serial.println("s");
      break;
    case 's':
      digitalWrite(M1P1, LOW);
      digitalWrite(M1P2, HIGH);
      digitalWrite(M2P1, LOW);
      digitalWrite(M2P2, HIGH);
      myservo.write(0);
      delay(1000);
      espSerial.write('d');
      
      Serial.print(teste);
      Serial.print(" -> ");
      Serial.println("d");
      break;
    case 'd':
      digitalWrite(M1P1, HIGH);
      digitalWrite(M1P2, LOW);
      digitalWrite(M2P1, LOW);
      digitalWrite(M2P2, HIGH);
      myservo.write(0);
      delay(1000);
      espSerial.write('a');
      
      Serial.print(teste);
      Serial.print(" -> ");
      Serial.println("a");
      break;
    case 'x':
      digitalWrite(M1P1, LOW);
      digitalWrite(M1P2, LOW);
      digitalWrite(M2P1, LOW);
      digitalWrite(M2P2, LOW);
      myservo.write(180);
      delay(1000);
      espSerial.write('a');
      
      Serial.print(teste);
      Serial.print(" -> ");
      Serial.println("a");
      break;
    
    default:
      espSerial.write('x');
      Serial.print(teste);
      Serial.print(" -> ");
      Serial.println("x");
      delay(1000);
      break;
    }      
  }
}
