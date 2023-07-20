// Libraries
#include <HCSR04.h>

// Pins
const int MOISTURE_SENSOR_SIGNAL = 14;
const int MOISTURE_SENSOR_POWER = 12;
const int NEEDS_WATERING_INDICATOR = 13;
const int ULTRASONIC_TRIGGER = 27;
const int ULTRASONIC_ECHO = 26;
const int LCD_SDA = 25;
const int LCD_SCL = 33;
const int LCD_POWER = 32;

// Constants
const int NEEDS_WATERING_THRESHOLD = 200;
const int DISTANCE_THRESHOLD = 10;

// Variables
volatile int moisture = 0;
volatile float distance = 0;

// Inititalise libraries
UltraSonicDistanceSensor distanceSensor(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO);


void setup() {
  // For debugging
  Serial.begin(9600);

  // Setup pins
  pinMode(MOISTURE_SENSOR_POWER, OUTPUT);
  pinMode(MOISTURE_SENSOR_SIGNAL, INPUT);
  pinMode(NEEDS_WATERING_INDICATOR, OUTPUT);
  pinMode(LCD_POWER, OUTPUT);
}

void loop() {
  // Check distance on ultrasonic
  distance = distanceSensor.measureDistanceCm();

  digitalWrite(NEEDS_WATERING_INDICATOR, LOW);
  digitalWrite(LCD_POWER, LOW);


  // Check threshold before measing soil mostire to slow corrosion.
  if (distance > 0 && distance < DISTANCE_THRESHOLD) {

    // Turn on the soil moisture sensor
    digitalWrite(MOISTURE_SENSOR_POWER, HIGH);
    // Wait for 10 millisecond(s)
    delay(10);
    moisture = analogRead(MOISTURE_SENSOR_SIGNAL);
    // Turn off the sensor to reduce metal corrosion over time
    digitalWrite(MOISTURE_SENSOR_POWER, LOW);

    // Temporay LED indicator
    if (moisture < NEEDS_WATERING_THRESHOLD) {
      digitalWrite(NEEDS_WATERING_INDICATOR, HIGH);
      digitalWrite(LCD_POWER, HIGH);
    }
  }


  // Wait for a second
  delay(1000);
}