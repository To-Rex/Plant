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

FirebaseData firebaseData; // object of FirebaseData class

#define DHT11PIN 16 // port for connecting the sensor
#define LandPin A0  // port for connecting the sensor

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

dht11 sensor; // object of dht11 class

int TimeZone = 5 * 3600; // Time zone
int Dst = 0;             // Daylight saving time

int Rele01 = 13; // port for connecting the sensor
int Rele02 = 12;// port for connecting the sensor
int Rele03 = 2;  // port for connecting the sensor
int Rele04 = 0;  // port for connecting the sensor
bool Falg = true;    // flag water
int Land; // port for connecting the sensor

void setup()
{
    pinMode(LandPin, INPUT);
    pinMode(Rele01, OUTPUT);
    digitalWrite(Rele01, HIGH);
    pinMode(Rele02, OUTPUT);
    digitalWrite(Rele02, HIGH);
    pinMode(Rele03, OUTPUT);
    digitalWrite(Rele03, HIGH); 
    pinMode(Rele04, OUTPUT);
    digitalWrite(Rele04, HIGH);
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to...");
    Serial.print(WIFI_SSID);

    lcd.init();
    lcd.backlight();

    while (WiFi.status() != WL_CONNECTED){
        lcd.setCursor(0, 0);
        lcd.print("Connecting to ");
        lcd.setCursor(0, 1);
        lcd.print(WIFI_SSID);
        Serial.print(".");
        delay(500);
    }

    Serial.println();                  //chop buni-------------------------------------------------------
    Serial.print("Connected to ");     //chop buni-------------------------------------------------------
    Serial.println(WIFI_SSID);
    Serial.print("IP Address is : ");  //chop buni-------------------------------------------------------
    Serial.println(WiFi.localIP());

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);
    configTime(TimeZone, Dst, "pool.ntp.org", "time.nist.gov");
    lcd.clear();
}

void loop(){
    Land = analogRead(LandPin);

    int chk = sensor.read(DHT11PIN);
    int Water = analogRead(14);
    
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
    lcd.print(Land);
    lcd.setCursor(9, 1);
    lcd.print("Wa:");

    Firebase.set(firebaseData, "/plant/dhd_h", sensor.humidity);
    Firebase.set(firebaseData, "/plant/dhd_t", sensor.temperature);
    Firebase.set(firebaseData, "/plant/plantd", Land);
    //Firebase.set(firebaseData, "/plant/led", led);

    if(Water > HIGH && Falg == true){
        Firebase.set(firebaseData, "/plant/water", true);
        Falg = false;
    }else if (Water < HIGH && Falg == false){
        Firebase.set(firebaseData, "/plant/water", false);
        Falg = true;
    }

    //Suv sensori
    Firebase.get(firebaseData, "/plantSettings/plants"); // get the int value from the database
    if (firebaseData.intData() < Land&& Falg == false){
        digitalWrite(Rele01, LOW); // turn on the Water pump
    }else{
        digitalWrite(Rele01, HIGH); // turn off the Water pump
    }
    
    //Namlik sensori
    Firebase.get(firebaseData, "/plantSettings/humidity"); // get the int value from the database
    if (sensor.humidity < firebaseData.intData()){
        digitalWrite(Rele03, LOW); // turn on the Humidifier
    }else{
        digitalWrite(Rele03, HIGH); // turn off the Humidifier
    }

    //Harorat sensori
    Firebase.get(firebaseData, "/plantSettings/temperature");
    if (sensor.temperature > firebaseData.intData()){
        digitalWrite(Rele04, LOW); // turn on the Cooler
    }else{
        digitalWrite(Rele04, HIGH); // turn off the Cooler
    }

    Firebase.get(firebaseData, "/Usimliklar/usimlik1");
    if (firebaseData.boolData() == true){
        TimeWork(6,0,780);
        return;
    }

    Firebase.get(firebaseData, "/Usimliklar/usimlik2");
    if (firebaseData.boolData() == true){
        TimeWork(9,0,900);
        return;
    }

    Firebase.get(firebaseData, "/Usimliklar/usimlik3");
    if (firebaseData.boolData() == true){
        TimeWork(18,0,1080);
        return;
    }

    Firebase.get(firebaseData, "/Usimliklar/usimlik4");
    if (firebaseData.boolData() == true){
        TimeWork(12,0,1040);
        return;
    }
    Firebase.set(firebaseData, "/plant/led", false);
    digitalWrite(Rele02, HIGH);
}

void TimeWork(int startHour, int startMinute, int TimeWatering){
    // Hozirgi soat va minutlar
    time_t now = time(nullptr);
    struct tm *p_tm = localtime(&now);

    int currentHour = p_tm->tm_hour;
    int currentMinute = p_tm->tm_min;

    // Davomiylikni hisoblash
    int targetTime = startHour * 60 + startMinute;
    int currentTime = currentHour * 60 + currentMinute;

    Serial.print("Maqsadli vaqtni minutda: ");
    Serial.println(targetTime);
    Serial.print("Hozirgi vaqtni minutda: ");
    Serial.println(currentTime);

    // Releni 10 minut oralig`ida yoqib turish yoqish/ochish
    if (currentTime >= targetTime && currentTime <= targetTime + TimeWatering){ //mano 3 bu minut agar sizga boshqa minut kerak boladigan bo`lsa shu soatni 60 ga kopaytirsez minut kelib chiqadi shu minutni  3ni orniga yozilsa masala xal
        digitalWrite(Rele02, LOW);
        Firebase.set(firebaseData, "/plant/led", true);
        Serial.println(currentTime);
    }else{
        digitalWrite(Rele02, HIGH);
        Firebase.set(firebaseData, "/plant/led", false);
        Serial.println(currentTime);
    }

}
