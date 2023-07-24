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
const int SCALE_POT = 25;
const int OFFSET_POT = 33;
const int THRESHOLD_POT = 32;

// Constants
const int DISTANCE_THRESHOLD = 10;

// Variables
volatile int moisture = 0;              // Moisture sensor
volatile float distance = 0;            // Ultrasonic sensor
volatile int mositure_threshold = 200;  // Potentiomiter
volatile int moisture_multiplier = 1;   // Potentiomiter
volatile int moisture_offset = 0;       // Potentiomiter


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

  pinMode(SCALE_POT, INPUT);
  pinMode(OFFSET_POT, INPUT);
  pinMode(THRESHOLD_POT, INPUT);

  // Setup LCD
  lcd.begin();
  lcd.noBacklight();
}

void loop() {
  // Check potentiomiter values
  moisture_multiplier = analogRead(SCALE_POT);
  moisture_offset = analogRead(OFFSET_POT);
  mositure_threshold = analogRead(THRESHOLD_POT);


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
    if (moisture < mositure_threshold) {
      // LED indicator
      digitalWrite(NEEDS_WATERING_INDICATOR, HIGH);

      // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
      lcd.clear();
      lcd.print(F("WATER ME"));


      // lcd.print("WATER ME");
    } else {
      // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
      lcd.clear();
      lcd.print(F("DON'T WATER"));
      // lcd.print("DON'T WATER");
    }

    lcd.print(moisture);
    lcd.setCursor(0, 1);
    lcd.print(moisture_multiplier);
    lcd.print(F(" "));
    lcd.print(moisture_offset);
    lcd.print(F(" "));
    lcd.print(mositure_threshold);

  } else {
    // turn off the LED
    digitalWrite(NEEDS_WATERING_INDICATOR, LOW);

    // Turn off the LCD message
    lcd.clear();
  }




  // Wait for half a second
  delay(500);
}