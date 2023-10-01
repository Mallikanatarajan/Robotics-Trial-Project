#include <Wire.h>
#include <Servo.h>

// Constants for MPU6050 register addresses
const int MPU6050_ADDRESS = 0x68; // MPU6050 I2C address
const int ACCEL_XOUT_H = 0x3B;   // Accelerometer data registers

// Servo object
Servo servo;
const int servoPin = 9;

// Variables for orientation and offset
int servoAngle = 90; // Initial servo angle 
int offsetAngle = 0; // Offset angle, change in the Serial MOntior 

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize I2C communication
  Wire.begin();

  // Initialize MPU6050
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(0x6B); // PWR_MGMT_1 register??
  Wire.write(0);    // Set to zero to wake up MPU6050
  Wire.endTransmission(true);

  // Attach servo to pin
  servo.attach(servoPin); 
}

void loop() {
  // Read from MPU6050
  int16_t accelerometerX, accelerometerY, accelerometerZ;
  readMPU6050Data(accelerometerX, accelerometerY, accelerometerZ);

  // Calculate pitch angle (adjust according to orientation
  float angle = atan2(-accelerometerX, accelerometerZ) * 180 / PI;

  // Apply offset 
  angle += offsetAngle;

  // Map angle to servo position 
  servoAngle = map(angle, -90, 90, 0, 180);

  // Set servo position
  servo.write(servoAngle);

  // Print data to Serial Monitor
  Serial.print("Angle: ");
  Serial.println(angle);
  Serial.print("Servo Angle: ");
  Serial.println(servoAngle);

  // Check for offset input from the Serial Monitor
  if (Serial.available() > 0) {
    int offsetInput = Serial.parseInt();
    if (!isnan(offsetInput)) {
      offsetAngle = offsetInput;
      Serial.print("Offset set to ");
      Serial.println(offsetAngle);
    }
  }

  // delay to avoid excessive servo movement? 
  delay(70);
}

void readMPU6050Data(int16_t &ax, int16_t &ay, int16_t &az) {
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDRESS, 6, true);

  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
}


