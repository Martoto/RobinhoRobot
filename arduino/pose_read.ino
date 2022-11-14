#include <SoftwareSerial.h>

SoftwareSerial myserial(12, 13);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myserial.begin(9600);
  myserial.flush();
  Serial.println("oi");
}

void loop() {
  char teste[10];
  float pose[3];
  if(Serial.available())
  {
    char tes = Serial.read();
    myserial.write(tes);
  }

  if(myserial.available())
  {
    char tes = myserial.read();
    Serial.println(tes, BIN);
    if(tes == 0b00000000)
    {
      for(int j = 0; j<3; j++)
      {
        myserial.write('1');
        while(myserial.available() != 10){}
        for(int i=0; i<10; i++)
        {
          while(!myserial.available()){}
          teste[i] = myserial.read();
        }
        pose[j] = atof(teste);
      }
      myserial.write('1');
      Serial.println(pose[0]);
      Serial.println(pose[1]);
      Serial.println(pose[2]);
    }
  }
}
