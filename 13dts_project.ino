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
const int MOSITURE_MAX_POT = 25;
const int MOISTURE_OFFSET_POT = 33;

// Constants
const int DISTANCE_THRESHOLD = 10;
const int LCD_WIDTH = 16;
const int LCD_HEIGHT = 2;
const int POTENTIOMITER_CHANGE_NOT_COUNT_THRESHOLD = 15;

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
volatile int moisture = 0;         // Moisture sensor
volatile float distance = 0;       // Ultrasonic sensor
volatile int moisture_max = 200;   // Potentiomiter
volatile int moisture_offset = 0;  // Potentiomiter
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

  pinMode(MOISTURE_OFFSET_POT, INPUT);
  pinMode(MOSITURE_MAX_POT, INPUT);

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
  int _moisture_min = analogRead(MOISTURE_OFFSET_POT);
  int _mositure_max = analogRead(MOSITURE_MAX_POT);

  potentiomiters_changing = false;
  if (
    is_pot_change_over_threshold(_moisture_min, moisture_offset)) {
    potentiomiters_changing = true;
    moisture_offset = _moisture_min;
  }
  if (is_pot_change_over_threshold(_mositure_max, moisture_max)) {
    potentiomiters_changing = true;
    moisture_max = _mositure_max;
  }



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

    // Caculate computed moisture
    int computed_moisture = moisture + moisture_offset;


    // Turn on the LCD backlight
    lcd.backlight();
    lcd.clear();

    // If potentiomiters are changing, show them on the screen. Otherwise, show data.
    if (potentiomiters_changing) {
      // Draw moisture offset on the left
      lcd.setCursor(0, 0);
      lcd.print(moisture_offset);

      // Draw moisture threshold next to offset
      lcd.setCursor(String(moisture_offset).length() + 1, 0);
      lcd.print(moisture_max);
    } else {
      // Display message on LCD
      if (computed_moisture < moisture_max) {
        // LED indicator
        digitalWrite(NEEDS_WATERING_INDICATOR, HIGH);

        // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
        lcd.print(F("Too dry"));


        // lcd.print("WATER ME");
      } else {
        // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
        lcd.print(F("Dampness OK"));
      }
    }

    // Display moisture on right
    lcd.setCursor(LCD_WIDTH - String(moisture).length(), 0);
    lcd.print(moisture);

    // Draw the line graph
    float moisture_percentage = (moisture + moisture_offset) / moisture_max + 1;  // Avoid dividing by zero
    int cells_to_fill = LCD_WIDTH * moisture_percentage;

    lcd.setCursor(0, 1);
    for (int i = 0; i < LCD_WIDTH; i++) {
      if (i < cells_to_fill) {
        // if ()
        lcd.write(GRAPH_SEGMENT);
      }
    }


  } else {
    // turn off the LED
    digitalWrite(NEEDS_WATERING_INDICATOR, LOW);

    // Turn off the LCD message
    // lcd.clear();
  }
  // Wait for half a second
  delay(500);
}