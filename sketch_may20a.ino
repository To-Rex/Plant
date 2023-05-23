#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#define FIREBASE_HOST "https://plant-94aaf-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "ieeBwqphrAySk078qlHmpMYkWzDedwuh7xbGVk6g"
#define WIFI_SSID ":)"
#define WIFI_PASSWORD "azamat2018$"
#include <Wire.h>
#include <time.h>
#include <LiquidCrystal_I2C.h>
#include <dht11.h>

FirebaseData firebaseData;

int count = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DHT11PIN 16
#define solPin A0 // порт для подключения датчика
dht11 sensor;

int timezone = 5 * 3600;
int dst = 0;

int sol;
int led = 1;
int rele01 = 12;
int rele02 = 13;
int rele03 = 0;
int rele04 = 2;
int hello = 1;
byte degree[8] = // кодируем символ градуса
    {
        B00111,
        B00101,
        B00111,
        B00000,
        B00000,
        B00000,
        B00000,
};
void setup()
{
  pinMode(solPin, INPUT);
  pinMode(rele01, OUTPUT);
  pinMode(rele02, OUTPUT);
  pinMode(rele03, OUTPUT);
  pinMode(rele04, OUTPUT);
  digitalWrite(rele01, HIGH);
  digitalWrite(rele02, HIGH);
  digitalWrite(rele03, HIGH);
  digitalWrite(rele04, HIGH);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.reconnectWiFi(true);
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");

  lcd.init();
  lcd.backlight();
  lcd.createChar(1, degree);
}
void loop(){

  time_t now = time(nullptr);
  struct tm *p_tm = localtime(&now);
  sol = analogRead(solPin);
  int chk = sensor.read(DHT11PIN);
  int suuu = analogRead(14);
  lcd.setCursor(0, 0);
  lcd.print("Hum:  %");
  lcd.setCursor(4, 0);
  lcd.print(sensor.humidity);
  lcd.setCursor(0, 1);
  lcd.print("temp:  C|");
  lcd.setCursor(5, 1);
  lcd.print(sensor.temperature);
  lcd.setCursor(8, 0);
  lcd.print("|Wa:");
  lcd.setCursor(12, 0);
  lcd.print(sol);
  lcd.setCursor(9, 1);
  lcd.print("Wa:");
  if (led == 1)
  {
    lcd.setCursor(13, 1);
    lcd.print("HIGH");
  }
  else
  {
    lcd.setCursor(13, 1);
    lcd.print("LOW");
  }

  Firebase.set(firebaseData, "/plant/dhd_h", sensor.humidity);
  Firebase.set(firebaseData, "/plant/dhd_t", sensor.temperature);
  Firebase.set(firebaseData, "/plant/plantd", sol);
  Firebase.set(firebaseData, "/plant/led", led);

  Firebase.get(firebaseData, "/time");

  if (suuu > HIGH && hello == 1)
  {
    Firebase.set(firebaseData, "/plant/water", "true");
    hello = 0;
  }

  if (suuu < HIGH && hello == 0)
  {
    Firebase.set(firebaseData, "/plant/water", "false");
    hello = 1;
  }

  Firebase.get(firebaseData, "/plantSettings/humidity");
  if (sensor.humidity < firebaseData.intData()){
    digitalWrite(rele04, LOW);
  }else{
    digitalWrite(rele04, HIGH);
  }

  Firebase.get(firebaseData, "/plantSettings/temperature");
  if (sensor.temperature > firebaseData.intData()){
    digitalWrite(rele03, LOW);
  }else{
    digitalWrite(rele03, HIGH);
  }

  Firebase.get(firebaseData, "/plantSettings/plants");
  if (sol > firebaseData.intData()){
    digitalWrite(rele02, LOW);
  }else{
    digitalWrite(rele02, HIGH);
  }

  Firebase.get(firebaseData, "/Usimliklar/usimlik1");
  if (firebaseData.boolData() == true){
    int targetHour = 6;   // Maqsadli soat
    int targetMinute = 19; // Maqsadli minut

  // Hozirgi soat va minutlar
    int currentHour = p_tm->tm_hour;
    int currentMinute = p_tm->tm_min;

  // Davomiylikni hisoblash
    int targetTime = targetHour * 60 + targetMinute;
    int currentTime = currentHour * 60 + currentMinute;

    Serial.print("Maqsadli vaqtni minutda: ");
    Serial.println(targetTime);
    Serial.print("Hozirgi vaqtni minutda: ");
    Serial.println(currentTime);

  // Releni 10 minut oralig`ida yoqib turish yoqish/ochish
    if (currentTime >= targetTime && currentTime <= targetTime + 3){ //mano 3 bu minut agar sizga boshqa minut kerak boladigan bo`lsa shu soatni 60 ga kopaytirsez minut kelib chiqadi shu minutni  3ni orniga yozilsa masala xal
      digitalWrite(rele01, LOW);
      Serial.println("Rele yoqildi");
      Serial.println(currentTime);
    }else{
      digitalWrite(rele01, HIGH);
      Serial.println("Rele ochildi");
      Serial.println(currentTime);
    }
  }

  Firebase.get(firebaseData, "/Usimliklar/usimlik2");
  if (firebaseData.boolData() == true){
    int targetHour = 6;   // Maqsadli soat
    int targetMinute = 19; // Maqsadli minut

  // Hozirgi soat va minutlar
    int currentHour = p_tm->tm_hour;
    int currentMinute = p_tm->tm_min;

  // Davomiylikni hisoblash
    int targetTime = targetHour * 60 + targetMinute;
    int currentTime = currentHour * 60 + currentMinute;

    Serial.print("Maqsadli vaqtni minutda: ");
    Serial.println(targetTime);
    Serial.print("Hozirgi vaqtni minutda: ");
    Serial.println(currentTime);

  // Releni 10 minut oralig`ida yoqib turish yoqish/ochish
    if (currentTime >= targetTime && currentTime <= targetTime + 3){ //mano 3 bu minut agar sizga boshqa minut kerak boladigan bo`lsa shu soatni 60 ga kopaytirsez minut kelib chiqadi shu minutni  3ni orniga yozilsa masala xal
      digitalWrite(rele01, LOW);
      Serial.println("Rele yoqildi");
      Serial.println(currentTime);
    }else{
      digitalWrite(rele01, HIGH);
      Serial.println("Rele ochildi");
      Serial.println(currentTime);
    }
  }

  Firebase.get(firebaseData, "/Usimliklar/usimlik3");
  if (firebaseData.boolData() == true){
    int targetHour = 14;   // Maqsadli soat
    int targetMinute = 31; // Maqsadli minut
    //bu 14:31 ga ti`g`ri keladi

  // Hozirgi soat va minutlar
    int currentHour = p_tm->tm_hour;
    int currentMinute = p_tm->tm_min;

  // Davomiylikni hisoblash
    int targetTime = targetHour * 60 + targetMinute;
    int currentTime = currentHour * 60 + currentMinute;

  // Releni 10 minut oralig`ida yoqib turish yoqish/ochish
    if (currentTime >= targetTime && currentTime <= targetTime + 3){ //mano 3 bu minut agar sizga boshqa minut kerak boladigan bo`lsa shu soatni 60 ga kopaytirsez minut kelib chiqadi shu minutni  3ni orniga yozilsa masala xal
      digitalWrite(rele01, LOW);
      Serial.println("Rele yoqildi");
      Serial.println(currentTime);
    }else{
      digitalWrite(rele01, HIGH);
      Serial.println("Rele ochildi");
      Serial.println(currentTime);
    }
  }

  Firebase.get(firebaseData, "/Usimliklar/usimlik4");
  if (firebaseData.boolData() == true){
    int targetHour = 6;   // Maqsadli soat
    int targetMinute = 19; // Maqsadli minut

  // Hozirgi soat va minutlar
    int currentHour = p_tm->tm_hour;
    int currentMinute = p_tm->tm_min;

  // Davomiylikni hisoblash
    int targetTime = targetHour * 60 + targetMinute;
    int currentTime = currentHour * 60 + currentMinute;

    Serial.print("Maqsadli vaqtni minutda: ");
    Serial.println(targetTime);
    Serial.print("Hozirgi vaqtni minutda: ");
    Serial.println(currentTime);

  // Releni 10 minut oralig`ida yoqib turish yoqish/ochish
    if (currentTime >= targetTime && currentTime <= targetTime + 3){ //mano 3 bu minut agar sizga boshqa minut kerak boladigan bo`lsa shu soatni 60 ga kopaytirsez minut kelib chiqadi shu minutni  3ni orniga yozilsa masala xal
      digitalWrite(rele01, LOW);
      Serial.println("Rele yoqildi");
      Serial.println(currentTime);
    }else{
      digitalWrite(rele01, HIGH);
      Serial.println("Rele ochildi");
      Serial.println(currentTime);
    }
  }

  lcd.clear();
}