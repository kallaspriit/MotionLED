const int PIR_PIN = 4;
const int LED_PIN = 10;
const int OUTPUT_STATUS_PIN = 6;
const int PIR_STATUS_PIN = 9;
const int BRIGHTNESS_STATUS_PIN = 11;
const int LIGHT_SENSOR_PIN = 14;

float targetBrightness = 0.0f;
float currentBrightness = 0.0f;
float fadeAmount = 0.2f;
float onPower = 255.0f;
float offPower = 0.0f;
int lightLevelMin = 1023;
int lightLevelMax = 150;
int lightLevelThreshold = 20;
unsigned long fadeInterval = 10;
unsigned long onDuration = 120000;

unsigned long lastBrightnessUpdateTime = 0;
unsigned long lastMotionTime = 0;

void setup()  {
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(OUTPUT_STATUS_PIN, OUTPUT);
  pinMode(PIR_STATUS_PIN, OUTPUT);
  pinMode(BRIGHTNESS_STATUS_PIN, OUTPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  
  Serial.begin(9600);
} 

void loop()  {
  unsigned long currentTime = millis();
  
  int lightSensorValue = analogRead(LIGHT_SENSOR_PIN);
  int lightPercentage = map(lightSensorValue, lightLevelMax, lightLevelMin, 100, 0);
  boolean pirMotionDetected = digitalRead(PIR_PIN) == HIGH;
  boolean lightLevelLow = lightPercentage < lightLevelThreshold;
  
  if (pirMotionDetected && lightLevelLow) {
    lastMotionTime = currentTime;
  }
  
  digitalWrite(PIR_STATUS_PIN, pirMotionDetected);
  digitalWrite(BRIGHTNESS_STATUS_PIN, lightLevelLow);
  
  if (lastMotionTime != 0 && currentTime - lastMotionTime < onDuration) {
    targetBrightness = onPower;
  } else {
    targetBrightness = offPower;
  }
  
  if (lastBrightnessUpdateTime - currentTime > fadeInterval) {
    if (currentBrightness > 1.0f) {
      analogWrite(LED_PIN, (int)(255.0f - currentBrightness));
    } else {
      pinMode(LED_PIN, OUTPUT);
      digitalWrite(LED_PIN, HIGH);
    }
    
    analogWrite(OUTPUT_STATUS_PIN, (int)currentBrightness);
    
    Serial.print("Target: ");
    Serial.print(targetBrightness);
    Serial.print(", current: ");
    Serial.print(currentBrightness);
    Serial.print(", light: ");
    Serial.print(lightPercentage);
    Serial.println();
  
    if (currentBrightness < targetBrightness) {
      currentBrightness += fadeAmount;
    } else if (currentBrightness > targetBrightness) {
      currentBrightness -= fadeAmount;
    }
    
    if (fabs(currentBrightness - targetBrightness) < 1.0f) {
      currentBrightness = targetBrightness;
    }
    
    lastBrightnessUpdateTime = currentTime;
  }

  delay(1);  
}

