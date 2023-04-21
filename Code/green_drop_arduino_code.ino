// Including needed libraries
#include <Adafruit_LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

// Pin set up for sensors
const int lightPin = A0, tempPin = A1;
const int setSMS = A2, getSMS = A3;

// Sensor data variables
int light = 0, moisture = 0;
float temp = 0.00;

bool firstTime = true; // firstTime exception
bool neopixelOn = false; // Stores the state of the lights

// Pin set up for neopixel, servos and data
const int neopixelPin = 6, alarmPin = 7;
const int tempServoPin = 4, waterServoPin = 5;

int tempServoPos = 0, waterServoPos = 0; // Servo positions
const int numPixel = 24; // Number of pixels of Neopixel

Adafruit_LiquidCrystal lcd_1(0); // Adding LCD display pins
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(numPixel, neopixelPin, NEO_GRB + NEO_KHZ800);
Servo tempServo, waterServo; // Declaring servos

// Returns light sensor data
int getLight() {
  return map(analogRead(lightPin), 0, 1023, 0, 1023);
}

// Returns light type
String lightType(int lightValue){
  String type;
  if (light < 10) {
    type = "Dark";
  } else if (light < 200) {
    type = "Dim";
  } else if (light < 500) {
    type = "Light";
  } else if (light < 800) {
    type = "Bright";
  } else {
    type = "Very bright";
  }
  return type;
}

// Returns temperature sensor data
float getTemp() {
  return map(((analogRead(tempPin) - 20) * 3.04), 0, 1023, -40, 125);
}

// Returns moisture sensor data
int getMoisture() {
  digitalWrite(setSMS, HIGH);
  delay(10);
  int tmp = analogRead(getSMS);
  digitalWrite(setSMS, LOW);
  tmp = map(tmp, 0, 1023 , 10, 550);
  return map(tmp, 10, 550, 0, 100);
}
/*
void beep(){
  tone(alarmPin, 440);
  delay(200);
  noTone(alarmPin);
}
*/


void setup()
{
  // Setting up pins
  pinMode(lightPin, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(setSMS, OUTPUT);
  pinMode(getSMS, INPUT);

  pinMode(alarmPin, OUTPUT); // Setting up alarm pin

  lcd_1.begin(16, 2); // Setting up LCD rows and columns

  // Setting up Neopixels
  pixels.begin();
  pixels.setBrightness(100);
  pixels.show(); // Set all pixels to "off"

  // Setting up servo connections and setting minimum and maximum data values
  tempServo.attach(tempServoPin, 500, 2500);
  tempServo.write(tempServoPos);
  waterServo.attach(waterServoPin, 500, 2500);
  waterServo.write(waterServoPos);
}

void loop()
{
  // Getting data from sensors
  int lightTmp = getLight();
  float tempTmp = getTemp();
  int moistureTmp = getMoisture();
  
  // Temporary variable
  int tmp;

  // Light sensor change check
  if (light != lightTmp || firstTime) {
    lcd_1.setCursor(0, 0); // Setting cursor to the begining
  
    light = lightTmp; // Light sensor data
  
    // Printing light sensor data to the LCD display
    lcd_1.print(lightType(light));
    lcd_1.print(" ");
    lcd_1.print(light);

    lcd_1.print("         "); // Cleaning leftovers from old data
  }
  
  // Temperature sensor change check
  if (temp != tempTmp || firstTime) {
    lcd_1.setCursor(0, 1); // Setting cursor to the begining of the second row

    temp = tempTmp; // Temperature sensor data

    // Printing temperature sensor data to the LCD display
    lcd_1.print(temp);
    lcd_1.print("*C");
    lcd_1.print(" ");
  }
  
  // Moisture sensor change check 
  if (moisture != moistureTmp || firstTime) {
    lcd_1.setCursor(8, 1); // Setting cursor after temperature data

    moisture = moistureTmp; // Moisture sensor data
  
    // Printing moisture sensor data to the LCD display
    lcd_1.print(" Wet:");
    lcd_1.print(moisture);
    lcd_1.print("%");

    lcd_1.print("  "); // Cleaning leftovers from old data
  }

  if (temp > 35 || temp < 10 || light > 820 || light < 100 || moisture > 70 || moisture < 40) digitalWrite(alarmPin, HIGH);
  else digitalWrite(alarmPin, LOW);

  // Checks if the light is low enough to turn on the lights
  bool lightStateTmp = (light < 520);

  // Based on the light level and the power state of the Neopixel is determined if the Neopixel will be switched on/off
  if (lightStateTmp && !neopixelOn) {
      for (tmp = 0; tmp < numPixel; tmp++) {
        // Turns on pixel
        pixels.setPixelColor(tmp, pixels.Color(255, 255, 0));

        // Sends the updated pixel color to the hardware.
        pixels.show();

        // Delay for a period of time (in milliseconds).
        delay(100);
      }
      //pixels.show();
      neopixelOn = true;
    }
  else if (neopixelOn && !lightStateTmp) {
      for (tmp = 0; tmp < numPixel; tmp++) {
        // Turns off pixel
        pixels.setPixelColor(tmp, pixels.Color(0, 0, 0));

        // Sends the updated pixel color to the hardware.
        pixels.show();

        // Delay for a period of time (in milliseconds).
        delay(100);
      }
      //pixels.show();
      neopixelOn = false;
    }
  
  // Translates temperature data to degrees for the servo to use
  tempTmp = map(temp, -40, 150, 0, 104) / 1;

  // Checks if there is a change in the position
  if (tempServoPos != tempTmp) {
    if (tempServoPos < tempTmp) {
      for (; tempServoPos <= tempTmp; tempServoPos++) {
        // tell servo to go to position in variable 'tempServoPos'
        tempServo.write(tempServoPos);
        // wait 15 ms for servo to reach the position
        delay(15); // Wait for 15 millisecond(s)
      }
    }
    else {
      for (; tempServoPos >= tempTmp; tempServoPos--) {
        // tell servo to go to position in variable 'tempServoPos'
        tempServo.write(tempServoPos);
        // wait 15 ms for servo to reach the position
        delay(15); // Wait for 15 millisecond(s)
      }
    }
  }

  // Translates moisture data to degrees for the servo to use
  moistureTmp = map(moisture, 0, 85, 0, 90);

  // Checks if there is a change in the position
  if (waterServoPos != moistureTmp) {
    if (waterServoPos < moistureTmp) {
      for (; waterServoPos <= moistureTmp; waterServoPos++) {
        // tell servo to go to position in variable 'waterServoPos'
        waterServo.write(waterServoPos);
        // wait 15 ms for servo to reach the position
        delay(15); // Wait for 15 millisecond(s)
      }
    }
    else {
      for (; waterServoPos >= moistureTmp; waterServoPos--) {
        // tell servo to go to position in variable 'waterServoPos'
        waterServo.write(waterServoPos);
        // wait 15 ms for servo to reach the position
        delay(15); // Wait for 15 millisecond(s)
      }
    }
  }

  firstTime = false; // Disabling firstTime exception
  delay(1000); // Waiting 1 second before refreshing data
}
