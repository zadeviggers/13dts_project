// Pins
const int MOISTURE_SENSOR_SIGNAL = 14;
const int MOISTURE_SENSOR_POWER = 12;
const int NEEDS_WATERING_INDICATOR = 13;

// Constants
const int NEEDS_WATERING_THRESHOLD = 200;

// Variables
volatile int moisture = 0;



void setup() {
  // For debugging
  Serial.begin(9600);

  // Setup pins
  pinMode(MOISTURE_SENSOR_POWER, OUTPUT);
  pinMode(MOISTURE_SENSOR_SIGNAL, INPUT);
  pinMode(NEEDS_WATERING_INDICATOR, OUTPUT);
}

void loop() {
  // Turn on the soil moisture sensor
  digitalWrite(MOISTURE_SENSOR_POWER, HIGH);
  // Wait for 10 millisecond(s)
  delay(10);
  moisture = analogRead(MOISTURE_SENSOR_SIGNAL);
  // Turn off the sensor to reduce metal corrosion over time
  digitalWrite(MOISTURE_SENSOR_POWER, LOW);

  // For debugging
  Serial.println(moisture);

  // Temporay LED indicator
  if (moisture < NEEDS_WATERING_THRESHOLD) {
    digitalWrite(NEEDS_WATERING_INDICATOR, HIGH);
  } else {
    digitalWrite(NEEDS_WATERING_INDICATOR, LOW);
  }


  delay(1000);
}