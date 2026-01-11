#include <ArduinoBLE.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

byte frame_left[8][12] = {
  {0,0,0,0,0,0,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,0,0,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,0,0,0,0,0}
};
byte frame_down[8][12] = {
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,1,1,0,1,1,0,1,1,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0}
};
byte frame_right[8][12] = {
  {0,0,0,0,0,1,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,0,0,0,0,0,0}
};
byte frame_up[8][12] = {
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,1,1,0,1,1,0,1,1,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0}
};
byte frame_error[8][12] = {
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0}
};


void setup() {

  Serial.begin(9600);
  matrix.begin();
  matrix.loadFrame(LEDMATRIX_BLUETOOTH);
  if (!BLE.begin()) {
    matrix.renderBitmap(frame_error, 8, 12);

    while (1);
  }

  BLE.scan();
  
}

void loop() {
  BLEDevice peripheral = BLE.available();
  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    // see if peripheral is a LED
    if (peripheral.localName() == "Flipper Inqu0w") {
      // stop scanning
      BLE.stopScan();
      Serial.println("Connecting ...");

      if (peripheral.connect()) {
        Serial.println("Connected");
        matrix.loadFrame(LEDMATRIX_EMOJI_HAPPY);

        Serial.println("Discovering attributes ...");
        if (peripheral.discoverAttributes()) {
          Serial.println("Attributes discovered");
          Serial.println();
          Serial.print("Device name: ");
          Serial.println(peripheral.deviceName());
          Serial.print("Appearance: 0x");
          Serial.println(peripheral.appearance(), HEX);
          Serial.println();

          for (int i = 0; i < peripheral.serviceCount(); i++) {
            BLEService service = peripheral.service(i);

            exploreService(service);
          }
        } else {
          Serial.println("Attribute discovery failed!");
          peripheral.disconnect();
          matrix.renderBitmap(frame_error, 8, 12);
        }
      } else {
        Serial.println("Failed to connect!");
        matrix.renderBitmap(frame_error, 8, 12);
      }
    }
  }
}

void exploreService(BLEService service) {
  // print the UUID of the service
  Serial.print("Service ");
  Serial.println(service.uuid());
  if (strcmp(service.uuid(), "181C") == 0) {
    return;
  }
  Serial.println("Successfully found required service.");

  // loop the characteristics of the service and explore each
  for (int i = 0; i < service.characteristicCount(); i++) {
    BLECharacteristic characteristic = service.characteristic(i);

    if (strcmp(characteristic.uuid(), "2a9f") == 0) {
      Serial.print("Desired characteristic was found!");
    } else {
      Serial.println("Characteristic uuid is ");
      Serial.println(characteristic.uuid());
      Serial.println("passing to the next.");
      continue;
    }
    while (1) {
      if (characteristic.canRead()) {
        // read the characteristic value
        characteristic.read();

        if (characteristic.valueLength() > 0) {
          // print out the value of the characteristic
          Serial.print(", value 0x");
          printData(characteristic.value(), characteristic.valueLength());
          Serial.println();
          if (*(characteristic.value()) == 0x01) {
            matrix.renderBitmap(frame_left, 8, 12);
          }
          if (*(characteristic.value()) == 0x02) {
            matrix.renderBitmap(frame_right, 8, 12);
          }
          if (*(characteristic.value()) == 0x03) {
            matrix.renderBitmap(frame_up, 8, 12);
          }
          if (*(characteristic.value()) == 0x04) {
            matrix.renderBitmap(frame_down, 8, 12);
          }
          if (*(characteristic.value()) == 0x05) {
            matrix.beginDraw();
            matrix.stroke(0xFFFFFFFF);
            const char text[] = "OK";
            matrix.textFont(Font_4x6);
            matrix.beginText(0, 1, 0xFFFFFF);
            matrix.println(text);
            matrix.endText();
            matrix.endDraw();
          }
          delay(500);
        } else {
          Serial.println("characteristic valueLength is 0");
          delay(500);
        }

      } else {
        Serial.println("can't read characteristic!");
        delay(500);
      }
    }
  }
}

void printData(const unsigned char data[], int length) {
  for (int i = 0; i < length; i++) {
    unsigned char b = data[i];

    if (b < 16) {
      Serial.print("0");
    }

    Serial.print(b, HEX);
  }
}










;
