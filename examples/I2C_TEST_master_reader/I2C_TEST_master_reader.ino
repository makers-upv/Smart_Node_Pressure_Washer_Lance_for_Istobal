// https://arduino.stackexchange.com/questions/60940/arduino-i2c-register-read
#include <Wire.h>

byte ADDRESS_SLAVE = 0x39;
byte REGISTER_XY = 0x92;
byte READ_LENGTH = 1;

void setup()
{
//  Wire.setSDA(4);
//  Wire.setSCL(5);
//  Wire.setClock(100000);
  Wire.begin();
  Serial.begin(115200);
}

void loop()
{
  Wire.beginTransmission(ADDRESS_SLAVE);
  Wire.write(REGISTER_XY);  // set register for read
  Wire.endTransmission(false); // false to not release the line

  Wire.requestFrom(ADDRESS_SLAVE, READ_LENGTH); // request bytes from register XY
  byte buff[READ_LENGTH];
  Wire.readBytes(buff, READ_LENGTH);
  for (int i = 0; i < READ_LENGTH; i++) {
    Serial.println(buff[i], HEX);
  }
  Serial.println();
  delay(1000);
}
