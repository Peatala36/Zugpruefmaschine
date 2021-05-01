
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


// Steigung der Spindel
const int steigung = 8;

// Schritte pro Umdrehung
const int schritte = 200;

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
    move(1250, 1, 12);
  }
  if (x == 2) {
    move(400, 0, 12);
  }
  if (x == 3) {
    kraft_weg(790, 1);
  }
  if (x == 4) {
    anfangszugkraft(7.9);
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
  int speed_t = 12;
  digitalWrite(dirPin, LOW);
  while (digitalRead(stopUnten) == 0) {
    digitalWrite(stpPin, HIGH);
    delay(speed_t);
    digitalWrite(stpPin, LOW);
    delay(speed_t);
  }
  Serial.println("Schlitten befindet sich in Nullposition.");
}

void kraft_weg(int schritte, int richtung) {
  int speed_t = 12;
  int i = 0;
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0;
  
  digitalWrite(dirPin, HIGH);
  //if (digitalRead(stopUnten) == 0) {
  //  Serial.println("Schlitten befindet sich nicht in Nullposition!");
  //  return;
  //}

  while (i < schritte) {
    //move(1, richtung, speed_t);
    delay(1000);

    if (LoadCell.update()) newDataReady = true;

    if(newDataReady) {
      if (millis() > t + serialPrintInterval) {
        float kraft = LoadCell.getData();
        Serial.print("Load_cell output val: ");
        Serial.println(kraft);

        if (kraft > force_max) {
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

void anfangszugkraft(float prueflaenge) {
  int speed_t = 12;
  int x = round(prueflaenge * schritte / steigung);

  const int serialPrintInterval = 0;
  static boolean newDataReady = 0;

  digitalWrite(dirPin, HIGH);
  if (digitalRead(stopUnten) == 0) {
    Serial.println("Schlitten befindet sich nicht in Nullposition!");
    return;
  }

  Serial.println("Beginn der Messung Anfangszugkraft");
  Serial.print("Prüflänge: ");
  Serial.println(prueflaenge);
  Serial.println("Schlitten wird auf Prüflänge bewegt");
  //move(x, 1, speed_t);
  kraft_weg(x, 1);
  delay(1000);
  Serial.println("Elastic wird auf vierfache Prüflänge gedehnt");
  //move(3*x, 1, speed_t);
  kraft_weg(3*x, 1);
  Serial.println("Warte 5 sek");
  for (int n = 0; n < 5; n++) {
    Serial.print("x");
    delay(1000);
  }
  Serial.println("Entspanne den Elastic wieder auf dreifache Prüflänge");
  //move(x, 0, speed_t);
  kraft_weg(x, 0);
  Serial.println("Warte 30 sek");
  for (int n = 0; n < 30; n++) {
    Serial.print("x");
    delay(1000);
  }
  Serial.println("");
  Serial.println("Messung");
  
  float kraft = LoadCell.getData();
  delay(1000);
  Serial.print("Anfangszugkraft: ");
  Serial.println(kraft);
  
//  if (LoadCell.update()) newDataReady = true;
//  
//  if(newDataReady) {
//    if (millis() > t + serialPrintInterval) {
//      float kraft = LoadCell.getData();
//      Serial.print("Anfangszugkraft: ");
//      Serial.println(kraft);
//
//      newDataReady = 0;
//      t = millis();
//    }
//  }
  
  Serial.println("Fahre wieder in Nullposition");
  turn_to_start();
  
}
