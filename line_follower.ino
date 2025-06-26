// Line Follower Robot - single sensor
// Uses analog sensor on A0 to detect black line
// Controls 4-pin motor driver (L298N or similar)
// Jan Oleksiuk

// Motor pin definitions
const int LEFT_BACK = 5;    // LB
const int LEFT_FORWARD = 6; // LF
const int RIGHT_BACK = 9;   // RB
const int RIGHT_FORWARD = 10; // RF

// Sensor configuration
const int SENSOR_PIN = A0;
const int BLACK_THRESHOLD = 500; // Threshold for black line detection
const int WHITE_VALUE = 30;      // Approximate white surface reading
const int BLACK_VALUE = 1000;    // Approximate black line reading

// Movement timing constants
const int TURN_TIME = 180;       // Base turn duration (ms)
const int PAUSE_TIME = 50;       // Pause between movements (ms)
const int BACKUP_TIME = 500;     // Backup duration when line is lost

// Search pattern multipliers
const int MAX_SEARCH_ATTEMPTS = 3;

void setup() {
  // Initialize motor pins as outputs
  pinMode(LEFT_BACK, OUTPUT);
  pinMode(LEFT_FORWARD, OUTPUT);
  pinMode(RIGHT_BACK, OUTPUT);
  pinMode(RIGHT_FORWARD, OUTPUT);
  
  // Initialize serial for debugging (optional)
  Serial.begin(9600);
  Serial.println("Line Follower Robot Started");
}

void loop() {
  int sensorValue = analogRead(SENSOR_PIN);
  
  if (isOnLine(sensorValue)) {
    moveForward();
  } else {
    // Line lost - execute search pattern
    searchForLine();
  }
  
  delay(10); // Small delay for stability
}

// Check if sensor detects black line
bool isOnLine(int sensorValue) {
  return sensorValue > BLACK_THRESHOLD;
}

// Motor control functions
void moveForward() {
  digitalWrite(LEFT_BACK, LOW);
  digitalWrite(LEFT_FORWARD, HIGH);
  digitalWrite(RIGHT_BACK, LOW);
  digitalWrite(RIGHT_FORWARD, HIGH);
}

void turnLeft(int duration) {
  digitalWrite(LEFT_BACK, HIGH);
  digitalWrite(LEFT_FORWARD, LOW);
  digitalWrite(RIGHT_BACK, LOW);
  digitalWrite(RIGHT_FORWARD, HIGH);
  delay(duration);
}

void turnRight(int duration) {
  digitalWrite(LEFT_BACK, LOW);
  digitalWrite(LEFT_FORWARD, HIGH);
  digitalWrite(RIGHT_BACK, HIGH);
  digitalWrite(RIGHT_FORWARD, LOW);
  delay(duration);
}

void stopMotors() {
  digitalWrite(LEFT_BACK, LOW);
  digitalWrite(LEFT_FORWARD, LOW);
  digitalWrite(RIGHT_BACK, LOW);
  digitalWrite(RIGHT_FORWARD, LOW);
}

void backup() {
  digitalWrite(LEFT_BACK, HIGH);
  digitalWrite(LEFT_FORWARD, LOW);
  digitalWrite(RIGHT_BACK, HIGH);
  digitalWrite(RIGHT_FORWARD, LOW);
  delay(BACKUP_TIME);
}

// Enhanced search pattern when line is lost
void searchForLine() {
  // Try alternating left-right search with increasing distances
  for (int attempt = 1; attempt <= MAX_SEARCH_ATTEMPTS; attempt++) {
    
    // Turn left with increasing duration
    turnLeft(TURN_TIME * attempt);
    stopMotors();
    delay(PAUSE_TIME);
    
    if (checkForLine()) return;
    
    // Turn right with double the duration (to go past center)
    turnRight(TURN_TIME * attempt * 2);
    stopMotors();
    delay(PAUSE_TIME);
    
    if (checkForLine()) return;
    
    // Return closer to center for next attempt
    turnLeft(TURN_TIME * attempt);
    stopMotors();
    delay(PAUSE_TIME);
    
    if (checkForLine()) return;
  }
  
  // If line still not found after all attempts, backup and try again
  Serial.println("Line lost - backing up");
  backup();
  stopMotors();
  delay(PAUSE_TIME);
}

// Check sensor and continue forward if line is found
bool checkForLine() {
  int sensorValue = analogRead(SENSOR_PIN);
  
  if (isOnLine(sensorValue)) {
    moveForward();
    return true;
  }
  return false;
}

// Debug function to print sensor values (call in loop if needed)
void debugSensorValue() {
  int sensorValue = analogRead(SENSOR_PIN);
  Serial.print("Sensor: ");
  Serial.print(sensorValue);
  Serial.print(" | Status: ");
  Serial.println(isOnLine(sensorValue) ? "ON LINE" : "OFF LINE");
}