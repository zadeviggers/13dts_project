// Libraries
#include <HCSR04.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>


// Pins
// Moisture
const int MOISTURE_SENSOR_SIGNAL = 14;
const int MOISTURE_SENSOR_POWER = 12;
// LED
const int NEEDS_WATERING_INDICATOR = 13;
// Ultrasonic
const int ULTRASONIC_TRIGGER = 27;
const int ULTRASONIC_ECHO = 26;
// Potentiomiters
const int MOISTURE_MAX_POT = 25;
const int MOISTURE_MIN_POT = 33;
// LCD
const int TFT_CS = 19;
const int TFT_RST = 5;
const int TFT_DC = 18;
const int TFT_SCLK = 16;
const int TFT_MOSI = 17;

// Constants
const int DISTANCE_THRESHOLD = 10;
const int POTENTIOMITER_CHANGE_NOT_COUNT_THRESHOLD = 15;
const int GRAPH_LINE_HEIGHT = 15;
const int POTENTIOMITER_DIVISOR = 4;

// Variables
volatile int moisture = 0;        // Moisture sensor
volatile float distance = 0;      // Ultrasonic sensor
volatile int moisture_max = 200;  // Potentiomiter
volatile int moisture_min = 0;    // Potentiomiter
volatile bool potentiomiters_changing = false;

// Inititalise libraries
// Ultrasonic
UltraSonicDistanceSensor distanceSensor(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO);
// LCD
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


void setup() {
  // For debugging
  Serial.begin(9600);

  // Setup pins
  pinMode(MOISTURE_SENSOR_POWER, OUTPUT);
  pinMode(MOISTURE_SENSOR_SIGNAL, INPUT);

  pinMode(NEEDS_WATERING_INDICATOR, OUTPUT);

  pinMode(MOISTURE_MIN_POT, INPUT);
  pinMode(MOISTURE_MAX_POT, INPUT);


  // Setup LCD
  tft.initR(INITR_BLACKTAB);
  tft.setTextColor(ST7735_WHITE);
  tft.fillScreen(ST7735_BLACK);
  tft.print(F("Started!"));

  delay(1000);
}

bool is_pot_change_over_threshold(int a, int b) {
  return abs(a - b) > POTENTIOMITER_CHANGE_NOT_COUNT_THRESHOLD;
}

void loop() {
  // Check potentiomiter values
  moisture_min = analogRead(MOISTURE_MIN_POT) / POTENTIOMITER_DIVISOR;
  moisture_max = analogRead(MOISTURE_MAX_POT) / POTENTIOMITER_DIVISOR;

  // Check distance on ultrasonic
  distance = distanceSensor.measureDistanceCm();
  Serial.println("Distance:" + String(distance));

  // Check threshold before measing soil mostire to slow corrosion.
  if (distance > 0 && distance < DISTANCE_THRESHOLD) {

    // Turn on the soil moisture sensor
    digitalWrite(MOISTURE_SENSOR_POWER, HIGH);
    // Wait for 10 millisecond(s)
    delay(10);
    moisture = analogRead(MOISTURE_SENSOR_SIGNAL);
    Serial.println("Moisture:" + String(moisture));

    // Turn off the sensor to reduce metal corrosion over time
    digitalWrite(MOISTURE_SENSOR_POWER, LOW);

    // Caculate computed moisture
    int computed_moisture = moisture + moisture_min;

    // Display message on LCD
    tft.setCursor(0, 0);
    // Characters are 7 pixels high
    tft.fillRect(0, 0, tft.width(), 20, ST7735_BLACK);
    if (computed_moisture < moisture_min) {
      // LED indicator
      digitalWrite(NEEDS_WATERING_INDICATOR, HIGH);

      // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
      tft.print(F("Too dry - water"));

    } else if (computed_moisture > moisture_max) {
      // LED indicator
      digitalWrite(NEEDS_WATERING_INDICATOR, LOW);

      // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
      tft.print(F("Too wet"));

    } else {
      // LED
      digitalWrite(NEEDS_WATERING_INDICATOR, LOW);

      // Print to LCD. `F()` macro makes the string get stored in flash memory rather than RAM.
      tft.print(F("Wetness OK"));
    }

    // Display moisture
    tft.setCursor(0, 10);
    tft.print("Moisture:" + String(moisture));

    // Draw the line graph
    if (moisture_max <= 0) {
      // If the threshold is 0, just don't draw any line
      tft.fillRect(0, tft.height() - GRAPH_LINE_HEIGHT, tft.width(), GRAPH_LINE_HEIGHT, ST7735_BLACK);
    } else {
      // Otherwise, calcualte stuff

      Serial.println("Moisture_total:" + String(moisture + moisture_min));
      Serial.println("Moisture_min:" + String(moisture_min));
      Serial.println("Moisture_max:" + String(moisture_max));
      float moisture_percentage = ((moisture * 100) / moisture_max);
      Serial.println("Moisture_percent:" + String(moisture_percentage));
      int pixels_wide = (tft.width() * moisture_percentage) / 100;
      Serial.println("Moisture_percent:" + String(pixels_wide));


      tft.fillRect(0, tft.height() - GRAPH_LINE_HEIGHT, pixels_wide, GRAPH_LINE_HEIGHT, ST7735_BLUE);
      // Fill in the old area with black
      tft.fillRect(pixels_wide, tft.height() - GRAPH_LINE_HEIGHT, tft.width(), GRAPH_LINE_HEIGHT, ST7735_BLACK);
    }
  } else {
    // turn off the LED
    digitalWrite(NEEDS_WATERING_INDICATOR, LOW);

    // Turn off the LCD message
    tft.fillRect(0, tft.height() - 20, tft.width(), 20, ST7735_BLACK);
    tft.setCursor(0, tft.height() - 8);
    tft.print(F("NO DATA - Move closer"));
  }

  int potentiomiter_height = int(tft.height() / 2);
  tft.fillRect(0, potentiomiter_height, tft.width(), 7, ST7735_BLACK);

  // If potentiomiters are changing, show them on the screen. Otherwise, show data.
  // Draw moisture min on the left
  tft.setCursor(0, potentiomiter_height - 10);
  tft.print(F("Min"));
  tft.setCursor(0, potentiomiter_height);
  tft.print(moisture_min);

  // Draw moisture threshold on right
  tft.setCursor(tft.width() - (3 * 6) - 1, potentiomiter_height - 10);
  tft.print(F("Max"));
  String moisture_max_str = String(moisture_max);
  // The width of a character is 5 pixels, with a one pixel gap between characters
  tft.setCursor(tft.width() - (moisture_max_str.length() * 6) - 1, potentiomiter_height);
  tft.print(moisture_max_str);

  // Wait for half a second
  delay(500);
}