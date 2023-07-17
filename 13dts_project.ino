const int MOISTURE_SENSOR_SIGNAL = 14;
const int MOISTURE_SENSOR_POWER = 12;

volatile int moisture = 69;

void setup() {
  Serial.begin(9600);
  pinMode(MOISTURE_SENSOR_POWER, OUTPUT);
  pinMode(MOISTURE_SENSOR_SIGNAL, INPUT);
}

void loop() {
  // Turn on the soil moisture sensor
  digitalWrite(MOISTURE_SENSOR_POWER, HIGH);
  // Wait for 10 millisecond(s)
  delay(10);
  moisture = analogRead(MOISTURE_SENSOR_SIGNAL);
  // Turn off the sensor to reduce metal corrosion over time
  digitalWrite(MOISTURE_SENSOR_POWER, LOW);

  Serial.println(moisture);


  delay(1000);
}