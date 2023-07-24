// Libraries
#include <HCSR04.h>
#include <LCD_I2C.h>

// Pins
const int MOISTURE_SENSOR_SIGNAL = 14;
const int MOISTURE_SENSOR_POWER = 12;
const int NEEDS_WATERING_INDICATOR = 13;
const int ULTRASONIC_TRIGGER = 27;
const int ULTRASONIC_ECHO = 26;
const int LCD_ADDRESS = 0x27;

// Constants
const int NEEDS_WATERING_THRESHOLD = 200;
const int DISTANCE_THRESHOLD = 10;

// Variables
volatile int moisture = 0;
volatile float distance = 0;

// Inititalise libraries
UltraSonicDistanceSensor distanceSensor(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO);
LCD_I2C lcd(0x27, 16, 2);


void setup() {
  // For debugging
  Serial.begin(9600);

  // Setup pins
  pinMode(MOISTURE_SENSOR_POWER, OUTPUT);
  pinMode(MOISTURE_SENSOR_SIGNAL, INPUT);
  pinMode(NEEDS_WATERING_INDICATOR, OUTPUT);

  // Setup LCD
  lcd.begin();
  lcd.noBacklight();
}

void loop() {
  // Check distance on ultrasonic
  distance = distanceSensor.measureDistanceCm();

  // Check threshold before measing soil mostire to slow corrosion.
  if (distance > 0 && distance < DISTANCE_THRESHOLD) {

    // Turn on the soil moisture sensor
    digitalWrite(MOISTURE_SENSOR_POWER, HIGH);
    // Wait for 10 millisecond(s)
    delay(10);
    moisture = analogRead(MOISTURE_SENSOR_SIGNAL);
    // Turn off the sensor to reduce metal corrosion over time
    digitalWrite(MOISTURE_SENSOR_POWER, LOW);


    // Turn on the LCD backlight
    // lcd.backlight();


    // Display message on LCF
    if (moisture < NEEDS_WATERING_THRESHOLD) {
      // LED indicator
      digitalWrite(NEEDS_WATERING_INDICATOR, HIGH);

      // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
      lcd.clear();
      // lcd.print(F("WATER ME"));
      lcd.print("WATER ME");
    } else {
      // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
      lcd.clear();
      // lcd.print(F("DON'T WATER"));
      lcd.print("DON'T WATER");
    }

  } else {
    // turn off the LED
    digitalWrite(NEEDS_WATERING_INDICATOR, LOW);

    // Turn off the LCD message
    lcd.clear();
  }


  // Wait for half a second
  delay(500);
}