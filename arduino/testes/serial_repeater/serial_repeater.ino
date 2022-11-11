#include <SoftwareSerial.h>

SoftwareSerial myserial(12, 13);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myserial.begin(9600);
  Serial.println("oi");
}

void loop() {
  // put your main code here, to run repeatedly:
  char teste;
  if(Serial.available())
  {
    teste = Serial.read();
    myserial.write(teste);
  }

  if(myserial.available())
  {
    teste = myserial.read();
    Serial.print(teste);
  }
}
