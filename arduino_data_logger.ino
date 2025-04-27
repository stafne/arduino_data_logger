#include <SPI.h>
#include <SD.h>

const int chipSelect = 22;               // CS pin for SD card
const int analogPin0 = A0;               // First analog input
const int analogPin1 = A1;               // Second analog input
const unsigned long sampleRate = 250;    // Hz
const unsigned long sampleInterval = 1000 / sampleRate; // ms
const int totalSamples = 2000;

File dataFile;
unsigned long lastSampleTime = 0;
int sampleCount = 0;
bool logging = true;

void setup() {
  delay(2000);  // Allow USB to stabilize
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  Serial.println("SD card initialized.");
  dataFile = SD.open("data.csv", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Failed to open file for writing.");
    return;
  }

  // Write header
  dataFile.println("Time_ms,A0,A1");
  dataFile.flush();
  lastSampleTime = millis();
}

void loop() {
  if (!logging) return;

  unsigned long currentTime = millis();

  if (currentTime - lastSampleTime >= sampleInterval) {
    int value0 = analogRead(analogPin0);
    int value1 = analogRead(analogPin1);
    unsigned long timestamp = currentTime;

    dataFile.print(timestamp);
    dataFile.print(',');
    dataFile.print(value0);
    dataFile.print(',');
    dataFile.println(value1);

    sampleCount++;
    lastSampleTime += sampleInterval;

    if (sampleCount % 25 == 0) {
      dataFile.flush();
    }

    if (sampleCount >= totalSamples) {
      dataFile.flush();
      dataFile.close();
      Serial.println("Sampling complete. File closed.");
      logging = false;
    }
  }
}
