#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_LINES   2
const int triggerPin = 3;
const int echoPin = 4;
const int buzzerPin = 9;
const int ledRed = 8;
const int ledGreen = 7;
const int ledBlue = 6;

long durasi;  // Sensor
float jarak_cm, jarak_inch; // Sensor

unsigned long previousMillis1 = 0; // Menyimpan waktu terakhir LED berubah
unsigned long previousMillis = 0; // Menyimpan waktu terakhir LED dan Buzzer kedip di loop()
unsigned long interval = 0; // Interval LED dan Buzzer di loop()

unsigned long lcdMillis = 0; // Variabel untuk kontrol animasi LCD
int dotCount = 0;  // Counter untuk titik animasi
bool ledState = false;  // Status LED
bool buzzerState = false; // Status Buzzer

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

void setup() {
  pinMode(triggerPin, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Parking Alarm");
  lcd.setCursor(0, 1);
  lcd.print("Asteria  Academy");
  welcomeTone();
  delay(1000); // Kalau kurang
  lcd.clear();
  ledBiruMati();

  // Program animasi LED dan LCD
  unsigned long startMillis = millis(); // Waktu mulai animasi
  lcd.setCursor(0, 0);
  lcd.print("Loading");
  lcd.setCursor(0, 1);
  lcd.print("Please wait");
  loadingTone();
  while (millis() - startMillis < 3000) {  // Animasi selama 3 detik
    // LED berkedip setiap interval
    if (millis() - previousMillis1 >= 250) {
      previousMillis1 = millis();  // Menyimpan waktu saat ini
      ledState = !ledState;  // Membalikkan status LED
      digitalWrite(ledBlue, ledState);  // Update LED
    }
    // Animasi loading LCD dengan titik-titik
    if (millis() - lcdMillis >= 250) {  // Update setiap 250ms
      lcdMillis = millis();  // Menyimpan waktu saat ini
      if (dotCount < 5) {
        lcd.setCursor(11 + dotCount, 1);
        lcd.print(".");
        dotCount++;
      }
      else {
        lcd.setCursor(11, 1);
        lcd.print("     ");  // Hapus titik-titik setelah 5
        dotCount = 0;  // Reset counter titik
      }
    }
  }
  lcd.clear();
  ledBiruMati();
}

void loop() {
  unsigned long currentMillis = millis();

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  durasi = pulseIn(echoPin, HIGH);

  jarak_cm = (durasi * 0.0343) / 2; // Konversi waktu ke jarak dalam cm
  jarak_inch = jarak_cm / 2.54; // Konversi cm ke inch

  if (jarak_cm > 20 && jarak_cm <= 30) {
    cetakJarak();
    ledHijauMati();
    interval = 1000;
  } else if (jarak_cm > 15 && jarak_cm <= 20) {
    cetakJarak();
    ledHijauMati();
    interval = 500;
  } else if (jarak_cm > 10 && jarak_cm <= 15) {
    cetakJarak();
    ledHijauMati();
    interval = 300;
  } else if (jarak_cm > 7 && jarak_cm <= 10) {
    cetakJarak();
    ledHijauMati();
    interval = 200;
  } else if (jarak_cm >= 5 && jarak_cm <= 7) {
    cetakJarak();
    ledHijauMati();
    interval = 100;
  } else if (jarak_cm < 5) {
    cetakBerhenti();
    ledHijauMati();
    ledMerahNyala();
    tone(buzzerPin, 1175);
    interval = 0;
  } else {
    cetakJarak();
    ledHijauNyala();
    ledMerahMati();
    noTone(buzzerPin);
    interval = 0;
  }

  if (interval > 0 && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    buzzerState = !buzzerState;
    if (buzzerState) {
      tone(buzzerPin, 1175, 100);
      ledMerahNyala();
    } else {
      noTone(buzzerPin);
      ledMerahMati();
    }
  }

  delay(100);
}

void ledMerahNyala() {
  digitalWrite(ledRed, HIGH);
}

void ledMerahMati() {
  digitalWrite(ledRed, LOW);
}

void ledHijauNyala() {
  digitalWrite(ledGreen, HIGH);
}

void ledHijauMati() {
  digitalWrite(ledGreen, LOW);
}

void ledBiruNyala() {
  digitalWrite(ledBlue, HIGH);
}

void ledBiruMati() {
  digitalWrite(ledBlue, LOW);
}

void welcomeTone() {
  tone(buzzerPin, 1047, 250); // Nada pertama (C6)
  ledBiruNyala();
  delay(300);
  tone(buzzerPin, 1319, 250); // Nada kedua (E6)
  ledBiruMati();
  ledHijauNyala();
  delay(300);
  tone(buzzerPin, 2349, 250); // Nada ketiga (D7)
  ledHijauMati();
  ledMerahNyala();
  delay(300);
  tone(buzzerPin, 1976, 200); // Nada ketiga (B6)
  ledMerahMati();
  ledHijauNyala();
  delay(200);
  tone(buzzerPin, 2093, 300); // Nada ketiga (C7)
  ledHijauMati();
  ledBiruNyala();
  delay(400);
  noTone(buzzerPin);
}

void loadingTone() {
  tone(buzzerPin, 1760, 100);
  delay(200);
  tone(buzzerPin, 1760, 100);
  delay(200);
  noTone(buzzerPin);
}

void cetakJarak() {
  lcd.setCursor(0, 0);
  lcd.print("Jarak:          ");
  lcd.setCursor(0, 1);
  lcd.print(jarak_cm);
  lcd.print(" cm         ");
}

void cetakBerhenti() {
  lcd.setCursor(0, 0);
  lcd.print("  Jarak < 5 cm  ");
  lcd.setCursor(0, 1);
  lcd.print("     STOP!!     ");
}
