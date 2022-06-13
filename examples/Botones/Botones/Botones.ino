const int SWMonedas = A2, SWCarga = A3, t1 = 500;

void setup(){
  Serial.begin(115200);
  pinMode(SWMonedas, INPUT_PULLUP);
  pinMode(SWCarga, INPUT_PULLUP);
}

void loop(){
  delay(t1);
  if(digitalRead(SWMonedas)==HIGH) Serial.println("Monedas ON");
  else                          Serial.println("Monedas OFF");
  if(digitalRead(SWCarga)==HIGH)   Serial.println("Carga ON");
  else                          Serial.println("Carga OFF");
}
