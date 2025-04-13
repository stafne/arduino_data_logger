#include <SPI.h>
#include <SD.h>

const int chipSelect = 22;            // CS pin for SD card
const int analogPin = A0;             // Analog input
const unsigned long sampleRate = 250; // Hz
const unsigned long sampleInterval = 1000 / sampleRate; // milliseconds
const int totalSamples = 2000;

File dataFile;
unsigned long lastSampleTime = 0;
int sampleCount = 0;
bool logging = true;

void setup() {
  delay(2000);  // Allow USB to settle
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

  dataFile.println("Time_ms,AnalogValue");
  dataFile.flush();  // Ensure header is saved

  lastSampleTime = millis();
}

void loop() {
  if (!logging) return;

  unsigned long currentTime = millis();

  if (currentTime - lastSampleTime >= sampleInterval) {
    int value = analogRead(analogPin);
    unsigned long timestamp = currentTime;

    dataFile.print(timestamp);
    dataFile.print(',');
    dataFile.println(value);

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
