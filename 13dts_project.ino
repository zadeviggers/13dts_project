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
const int LCD_WIDTH = 16;
const int LCD_HEIGHT = 2;
const int POTENTIOMITER_CHANGE_NOT_COUNT_THRESHOLD = 8;

// Custom LCD characters
byte GRAPH_SEGMENT_SHAPE[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
int GRAPH_SEGMENT = 0;  // Chacter ID

// Variables
volatile int moisture = 0;              // Moisture sensor
volatile float distance = 0;            // Ultrasonic sensor
volatile int mositure_threshold = 200;  // Potentiomiter
volatile int moisture_multiplier = 1;   // Potentiomiter
volatile int moisture_offset = 0;       // Potentiomiter
volatile bool potentiomiters_changing = false;

// Inititalise libraries
UltraSonicDistanceSensor distanceSensor(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO);
LCD_I2C lcd(0x27, LCD_WIDTH, LCD_HEIGHT);


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
  lcd.createChar(GRAPH_SEGMENT, GRAPH_SEGMENT_SHAPE);
}

bool is_pot_change_over_threshold(int a, int b) {
  return abs(a - b) > POTENTIOMITER_CHANGE_NOT_COUNT_THRESHOLD;
}

void loop() {
  // Check potentiomiter values
  int _moisture_multiplier = analogRead(SCALE_POT);
  int _moisture_offset = analogRead(OFFSET_POT);
  int _mositure_threshold = analogRead(THRESHOLD_POT);
  if (
    is_pot_change_over_threshold(_moisture_multiplier, moisture_multiplier)
    || is_pot_change_over_threshold(_moisture_offset, moisture_offset)
    || is_pot_change_over_threshold(_mositure_threshold, mositure_threshold)) {
    potentiomiters_changing = true;
  } else {
    potentiomiters_changing = false;
  }
  moisture_multiplier = _moisture_multiplier;
  moisture_offset = _moisture_offset;
  mositure_threshold = _mositure_threshold;


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
    lcd.clear();

    // If potentiomiters are changing, show them on the screen. Otherwise, show data.
    if (potentiomiters_changing) {
      lcd.setCursor(0, 0);
      lcd.print(moisture_multiplier);
      lcd.print(F(" "));
      lcd.print(moisture_offset);
      lcd.print(F(" "));
      lcd.print(mositure_threshold);
    } else {

      // Display message on LCD
      if (moisture < mositure_threshold) {
        // LED indicator
        digitalWrite(NEEDS_WATERING_INDICATOR, HIGH);

        // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
        lcd.print(F("WATER ME"));


        // lcd.print("WATER ME");
      } else {
        // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
        lcd.print(F("DON'T WATER"));
        // lcd.print("DON'T WATER");
      }
    }

    // Draw the mositure on the bottom line
    String space = " ";  // This is to make concatenation work
    String moisture_str = space + moisture;
    lcd.setCursor(LCD_WIDTH - moisture_str.length(), 1);
    lcd.print(moisture);

    // Draw the line graph
    int cells_avaiable = LCD_WIDTH - moisture_str.length();

    int cells_to_fill = mositure_threshold - moisture;

    for (int i = 0; i < cells_avaiable; i++) {
      lcd.setCursor(i, 1);
      if (cells_to_fill < i) {
        lcd.write(GRAPH_SEGMENT);
      }
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