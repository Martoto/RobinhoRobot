#include <SoftwareSerial.h>

SoftwareSerial myserial(12, 13);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myserial.begin(9600);
  Serial.println("oi");
}

void loop() {
  char teste[5];
  float pose;
  if(Serial.available())
  {
    char tes = Serial.read();
    myserial.write(tes);
  }

  if(myserial.available())
  {
    char tes = myserial.read();
    Serial.println(tes);
    if(tes == 0b00000000)
    {
      for(int i=0; i<5; i++)
      {
        while(!myserial.available()){}
        teste[i] = myserial.read();
      }
      pose = atof(teste);
      Serial.println(pose);
    }    
  }
}
