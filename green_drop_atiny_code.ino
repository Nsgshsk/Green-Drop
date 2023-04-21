// Pin to which the speaker is connected
const int piezoPin = 2;

// Frequency of the tone (440 Hz for A4 pitch)
const int Tone_Frequency = 440;

// Duration of the tone in milliseconds
const int Tone_Duaration_Millis = 200;

void beep() {
  // Generate a square wave tone at the desired frequency for the desired duration
  unsigned long startTime = millis();
  unsigned long elapsedTime = 0;
  while (elapsedTime < Tone_Duaration_Millis) {
    digitalWrite(piezoPin, HIGH);
    delayMicroseconds(500000 / Tone_Frequency);
    digitalWrite(piezoPin, LOW);
    delayMicroseconds(500000 / Tone_Frequency);
    elapsedTime = millis() - startTime;
  }
  delay(100);
}

void setup() {
  // Set the speaker pin as an OUTPUT
  pinMode(piezoPin, OUTPUT);
}

void loop() {
  for(int i = 0; i < 3; i++)
      beep();
  delay(200);
}
