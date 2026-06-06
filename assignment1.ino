#include <LiquidCrystal_I2C.h>

#define BOTTLE_HEIGHT 23.0       // Bottle height in cm
#define BOTTLE_CAPACITY 1500      // Bottle capacity in ml

#define TRIG_PIN 5
#define ECHO_PIN 6
#define buzzerPin 9
#define ledPin 13
#define tempPin A0

#define REMINDER_HOT 2000        // 2 seconds
#define REMINDER_MODERATE 5000   // 5 seconds
#define REMINDER_COOL 10000       // 10 seconds

//#define REMINDER_HOT 1200000      // 20 minutes
//#define REMINDER_MODERATE 2400000 // 40 minutes
//#define REMINDER_COOL 3600000     // 60 minutes

unsigned long lastReminderTime = 0;
unsigned long reminderInterval = REMINDER_COOL;

long duration;
float distance, percentage, waterLevel, mlPerCm, volumeML;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight(); // Enable LCD backlight
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Temperature reading and conversion
  int analogValue = analogRead(tempPin);
  float voltage = analogValue * (5.0 / 1023.0);
  float temperatureC = voltage * 100;

  // Adjust reminder interval based on temperature
  if (temperatureC > 30) {
    reminderInterval = REMINDER_HOT;
  } else if (temperatureC > 20) {
    reminderInterval = REMINDER_MODERATE;
  } else {
    reminderInterval = REMINDER_COOL;
  }

  // Ultrasonic sensor reading
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration * 0.0343) / 2;
  waterLevel = BOTTLE_HEIGHT - distance;
  mlPerCm = BOTTLE_CAPACITY / BOTTLE_HEIGHT;
  volumeML = waterLevel * mlPerCm;
  percentage = (volumeML / BOTTLE_CAPACITY) * 100;

  if (volumeML > BOTTLE_CAPACITY || volumeML < 0) {
    Serial.println("Water Volume: Out Of Range");
    lcd.setCursor(0, 0);
    lcd.print("Water Level:");
    lcd.setCursor(0, 1);
    lcd.print("Out Of Range  ");
  } else {
    serial_display();
    lcd_display();
  }

  checkDrinkingReminder(temperatureC);
  delay(1000);
}

void serial_display() {
  Serial.print("Water Volume: ");
  Serial.print(volumeML);
  Serial.print(" ml / ");
  Serial.print(BOTTLE_CAPACITY);
  Serial.println(" ml");
  Serial.print(" || ");
  Serial.print(percentage);
  Serial.println("% full");
}

void lcd_display() {
  lcd.setCursor(0, 0);
  lcd.print("Water:");
  lcd.print(volumeML);
  lcd.print("ml   ");

  lcd.setCursor(0, 1);
  lcd.print("Fill:");
  lcd.print(percentage);
  lcd.print("%     ");
}

void checkDrinkingReminder(float temp) {
  if (millis() - lastReminderTime >= reminderInterval) {
    lastReminderTime = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Drink Water Now!");

    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print("C ");

    tone(buzzerPin, 1000);
    digitalWrite(ledPin, HIGH);
    delay(2000);
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
  }
}