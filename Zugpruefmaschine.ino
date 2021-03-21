
#include <HX711_ADC.h>
#include <EEPROM.h>

//pins
const int led = 13;
const int stpPin = 6;
const int dirPin = 4;
const int stopOben = 2;
const int stopUnten = 3;
const int HX711_dout = 7; //mcu > HX711 dout pin
const int HX711_sck = 8; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

int x;
const int force_max = 500;
const int calVal_eepromAdress = 0;
unsigned long t = 0;


void setup() {                
  pinMode(led, OUTPUT);
  pinMode(stpPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stopOben, INPUT);
  pinMode(stopUnten, INPUT);
  
  Serial.begin(57600);
  Serial.setTimeout(1);
  delay(10);
  Serial.println();
  Serial.println("Start...");

  LoadCell.begin();
  float calibrationValue;
  EEPROM.get(calVal_eepromAdress, calibrationValue);

  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, bitte überprüfe die Verkabelung und die Pins");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue);
    Serial.println("Start in abgeschlossen");
  }

}

void loop() {
  while (!Serial.available());
  x = 0;
  x = Serial.readString().toInt();
  if (x == 0) {
    turn_to_start();
  }
  if (x == 1) {
    move(400, 1, 15);
  }
  if (x == 2) {
    move(400, 0, 15);
  }
  if (x == 3) {
    messung();
  }
  //Serial.println(digitalRead(stopUnten));
  //Serial.println(digitalRead(stopOben));
}

void move(int step_t, int dir, int speed_t) {
  int i = 0;
  if (dir == 1) {
    digitalWrite(dirPin, HIGH);
    while (i < step_t && digitalRead(stopOben) == 0) {
      digitalWrite(stpPin, HIGH);
      delay(speed_t);
      digitalWrite(stpPin, LOW);
      delay(speed_t);
      i++;
    }
  }
  if (dir == 0) {
    digitalWrite(dirPin, LOW);
    while (i < step_t && digitalRead(stopUnten) == 0) {
      digitalWrite(stpPin, HIGH);
      delay(speed_t);
      digitalWrite(stpPin, LOW);
      delay(speed_t);
      i++;
    }
  }
}

void turn_to_start() {
  int speed_t = 20;
  digitalWrite(dirPin, LOW);
  while (digitalRead(stopUnten) == 0) {
    digitalWrite(stpPin, HIGH);
    delay(speed_t);
    digitalWrite(stpPin, LOW);
    delay(speed_t);
  }
}

void messung() {
  int speed_t = 20;
  int i = 0;
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0;
  
  digitalWrite(dirPin, HIGH);
  if (digitalRead(stopUnten) == 0) {
    Serial.println("Schlitten befindet sich nicht in Nullposition!");
    return;
  }

  while (digitalRead(stopOben) == 0) {
    digitalWrite(stpPin, HIGH);
    delay(speed_t);
    digitalWrite(stpPin, LOW);
    delay(speed_t);

    if (LoadCell.update()) newDataReady = true;

    if(newDataReady) {
      if (millis() > t + serialPrintInterval) {
        float i = LoadCell.getData();
        Serial.print("Load_cell output val: ");
        Serial.println(i);

        if (i > force_max) {
          Serial.println("Maximal zulässige Zugkraft erreicht");
          break;
        }
        
        newDataReady = 0;
        t = millis();
      }
    }
    
    i++;
  }
  
}
