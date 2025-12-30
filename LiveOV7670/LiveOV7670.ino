#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include "CameraOV7670.h"


static File myFile;
static bool isFileAvailable = true;
static bool isFirstShoot = true;
static short fileNumber = 0;
static char fileName[16];
static char dirName[4];
static short number;

void setup() {
  CLKPR = 0x80;
  CLKPR = 0;
  Serial.begin(19200);
  number = readIntFromEEPROM(18) + 1;
  
  pinMode(13, OUTPUT);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  if (!SD.begin(8, 10, 11, 9)) {
    Serial.println(F("SD NOT DETECTED"));
    
    while (!SD.begin(8, 10, 11, 9)) {
      for (byte i=0;i<3;i++) {
        digitalWrite(13, HIGH);
        delay(100);
        digitalWrite(13, LOW);
        delay(100);
      }
      delay(1000);
    }
    
    
  }
  Serial.println(F("SD Card initialized"));
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  pinMode(13, INPUT);

  initializeCamera();
  Serial.println(F("Camera Ready"));
}
//==========================================
void loop() {
  if (digitalRead(13) == HIGH) {
    if (isFirstShoot) {
      writeIntIntoEEPROM(18, number);
      sprintf(dirName, "%d", number);
      SD.mkdir(dirName);
      isFirstShoot = false;
    }
    while (isFileAvailable) {
      sprintf(fileName, "%d/P_%04d.txt", number, fileNumber);
      if (SD.exists(fileName)) {
        fileNumber += 1;
      }
      else {
        myFile = SD.open(fileName, FILE_WRITE);
        isFileAvailable = false;
      }
    }
    
    if (myFile) {
      Serial.println(F("Taking Picture"));
      pinMode(13, OUTPUT);
      digitalWrite(13, HIGH);
      processFrame();
      myFile.close();
      
      isFileAvailable = true;
      
      digitalWrite(13, LOW);
      pinMode(13, INPUT);
      Serial.println(F("... Done ..."));
    }
    else {
      Serial.println(F("Error writing to SD Card"));
      pinMode(13, OUTPUT);
      for (byte i=0;i<5;i++) {
        digitalWrite(13, HIGH);
        delay(100);
        digitalWrite(13, LOW);
        delay(100);
      }
      
      pinMode(13, INPUT); 
    }
    
  }
}
//==========================================


static const uint16_t lineLength = 320;
static const uint16_t lineCount = 240;

CameraOV7670 camera(CameraOV7670::RESOLUTION_QVGA_320x240, CameraOV7670::PIXEL_YUV422, 50);

void initializeCamera() {
  bool cameraInitialized = camera.init();

}

inline void pixelSendingDelay() __attribute__((always_inline));

void processFrame() {
  uint8_t pixelBuffer[lineLength];
  camera.waitForVsync();

  for (uint16_t y = 0; y < lineCount; y++) {
    for (uint16_t x = 0; x < lineLength; x++) {


      camera.waitForPixelClockRisingEdge();
      camera.waitForPixelClockRisingEdge();

      camera.readPixelByte(pixelBuffer[x]);
      
    }
    myFile.write(pixelBuffer, sizeof(pixelBuffer));
    

  }
  //Serial.println(availableMemory());
}

void writeIntIntoEEPROM(short address, short number) { 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

short readIntFromEEPROM(short address) {
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

/*short availableMemory() {
    // Use 1024 with ATmega168
    short size = 2048;
    byte *buf;
    while ((buf = (byte *) malloc(--size)) == NULL);
        free(buf);
    return size;
}*/
