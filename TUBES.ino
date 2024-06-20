
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NewPing.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Konfigurasi WiFi
const char* ssid = "OPPO-A92";
const char* password = "xfx89rvg";

// Konfigurasi server
const char* serverName = "http://192.168.115.139:5000/data";

// Konfigurasi sensor DHT11
#define DHTPIN D5  // Pin DHT11
#define DHTTYPE DHT11 // Jenis DHT

// Konfigurasi sensor HC-SR04
#define TRIGGER_PIN D3  // Pin Trigger HC-SR04 pada NodeMCU
#define ECHO_PIN D6  // Pin Echo HC-SR04 pada NodeMCU
#define MAX_DISTANCE 250 // Maksimum pembacaan jarak (cm)

DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Konfigurasi sensor cahaya
const int pinSensor = A0;
int data = 0;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Menghubungkan ke WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.init(); // Inisialisasi LCD I2C
  lcd.backlight();

  // Inisialisasi sensor
  dht.begin();
  pinMode(pinSensor, INPUT);
}

void lcd_set(float temperature, int distance, float light) {
  lcd.clear(); // Clear the display
  bool panas = temperature >= 36;
  bool terlalu_dekat = distance <= 25;
  bool gelap = light <= 16;

  if (gelap) {
    lcd.setCursor(0, 0);
    lcd.print("RUANGAN GELAP");
  } else if (terlalu_dekat) {
    lcd.setCursor(0, 0);
    lcd.print("TERLALU DEKAT");
    lcd.setCursor(0, 1);
    lcd.print("AWAS MATA MINUS");
  } else if (panas) {
     lcd.setCursor(0, 0);
    lcd.print("RUANGAN PANAS!!!");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("SITUASI AMAN :)");
  }
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int jarak = sonar.ping_cm();
  data = analogRead(pinSensor);
  float light = map(data, 1023, 0, 0, 100);

  lcd.clear();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Mengambil data dari sensor
    

    // Menampilkan data sensor pada Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.print(" %, Distance: ");
    Serial.print(jarak);
    Serial.print(" cm, Light: ");
    Serial.print(light);
    Serial.println(" %");

    // Membuat URL untuk mengirim data
    String postData = "temperature=" + String(temperature) +
                      "&humidity=" + String(humidity) +
                      "&distance=" + String(jarak) +
                      "&light=" + String(light);

    // Memulai koneksi HTTP
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Mengirim data ke server
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    // Menutup koneksi
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
  
  // Update the LCD display
  lcd_set(temperature, jarak, light);

  // Tunggu 5 detik sebelum mengirim data lagi
  delay(5000);
}
