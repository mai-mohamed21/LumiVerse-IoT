#include <Arduino.h>
// for speaker
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Keypad.h>
// for sensor and database
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
// for LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
// for HiveMQ
#include <WiFiClientSecure.h>
#include <PubSubClient.h>


#define button_dfplayer 23  // for speaker part
static const uint8_t PIN_MP3_TX = 26;
static const uint8_t PIN_MP3_RX = 27;
#define disgust_led 5
#define anger_led 4
#define sad_led 14
#define joy_led 19
#define fear_led_red 21
#define fear_led_blue 22

// define PWM channels for (purple and blue)
const int red_channel = 0;
const int green_channel = 1;
const int blue_channel = 2;
// define PWM frequency and resolution
const int freq = 5000;
const int resolution = 8;

void setColor(int red, int green,int blue){
  ledcWrite(red_channel, red);
  ledcWrite(green_channel, green);
  ledcWrite(blue_channel, blue);
}

#define button_sensors 15 // for sensor & database part
#define vibration_sensor 16
#define temp_sensor 34
#define ldr 35

// MQTT broker info (HiveMQ Cloud)
const char* mqtt_server = "our server";
const int mqtt_port = 8883;
const char* mqtt_user = "LumiVerse";
const char* mqtt_pass = "our pass";
const char* mqtt_topic = "esp32/character/emotion";
WiFiClientSecure espClient;
PubSubClient client(espClient);


void turnOffAllLEDs() {
  digitalWrite(joy_led, LOW);
  digitalWrite(anger_led, LOW);
  digitalWrite(disgust_led, LOW);
  digitalWrite(sad_led, LOW);
  setColor(0,0,0);
  lcd.clear();
}

// for speaker and mp3 player
HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;
// for keypad
const byte ROWS = 2, COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'}
};
byte rowPins[ROWS] = {13,12};
byte colPins[COLS] = {18, 25,33};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
//for wifi and supabase
const  char* ssid = "WEA594D9";
const  char* password = "pass";
const  char* baseURL = "our url"; 
const  char* baseAPIkey = "our api key";


bool dfplayer_enabled = false;
bool sensors_enabled = false;


//  MQTT callback 
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("MQTT Message [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if (msg.indexOf("joy") > 0) {
    turnOffAllLEDs();
    myDFPlayer.play(1);
    digitalWrite(joy_led, HIGH);
    lcd.setCursor(0,0);
    lcd.print("Joy from MQTT");
  }
  else if (msg.indexOf("sadness") > 0) {
    turnOffAllLEDs();
    myDFPlayer.play(3);
    digitalWrite(sad_led, HIGH);
    lcd.setCursor(0,0);
    lcd.print("Sad from MQTT");
  }
  else if (msg.indexOf("anger") > 0) {
    turnOffAllLEDs();
    myDFPlayer.play(7);
    digitalWrite(anger_led, HIGH);
    lcd.setCursor(0,0);
    lcd.print("Anger from MQTT");
  }
  else if (msg.indexOf("fear") > 0) {
    turnOffAllLEDs();
    myDFPlayer.play(9);
    setColor(80,0,80);
    lcd.setCursor(0,0);
    lcd.print("Fear from MQTT");
  }
  else if (msg.indexOf("disgust") > 0) {
    turnOffAllLEDs();
    myDFPlayer.play(5);
    digitalWrite(disgust_led, HIGH);
    lcd.setCursor(0,0);
    lcd.print("Disgust from MQTT");
  }

    // ===
  if (String(topic) == "flutter/sound") {
    DynamicJsonDocument doc(200);
    deserializeJson(doc, msg);
    String character = doc["character"];
    int sound = doc["sound"];

    turnOffAllLEDs();

    if (character == "joy") { myDFPlayer.play(sound); digitalWrite(joy_led,HIGH); lcd.setCursor(0,0); lcd.print("joy from Chat"); }
    else if (character == "sad") { myDFPlayer.play(sound); digitalWrite(sad_led,HIGH); lcd.setCursor(0,0); lcd.print("sad from Chat"); }
    else if (character == "anger") { myDFPlayer.play(sound); digitalWrite(anger_led,HIGH); lcd.setCursor(0,0); lcd.print("anger from Chat"); }
    else if (character == "fear") { myDFPlayer.play(sound); setColor(80,0,80); lcd.setCursor(0,0); lcd.print("fear from Chat"); }
    else if (character == "disgust") { myDFPlayer.play(sound); digitalWrite(disgust_led,HIGH); lcd.setCursor(0,0); lcd.print("disgust from Chat"); }

    delay(5000);
    turnOffAllLEDs();
  }
}



void setup() {
  
  Serial.begin(115200);
  pinMode(button_dfplayer, INPUT_PULLUP);
  pinMode(button_sensors, INPUT_PULLUP);

  // for LCD
  Wire.begin(17, 32); // SDA = GPIO17, SCL = GPIO32
  lcd.begin(16, 2);
  lcd.backlight();


  lcd.setCursor(0, 0);
  lcd.print("   LumiVerse");
  delay(2000);
  lcd.clear();




  // wifi and supabase and sensors
  WiFi.begin(ssid, password);
  pinMode(vibration_sensor, INPUT_PULLUP);
  pinMode(temp_sensor, INPUT);
  pinMode(ldr, INPUT);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Connecting to WIFI...");
  }
  Serial.println("WIFI Connected!");


  //  MQTT
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);     
  client.setCallback(callback); 


  // speaker
  mySerial.begin(9600, SERIAL_8N1, PIN_MP3_TX,PIN_MP3_RX);
  pinMode(joy_led, OUTPUT);
  pinMode(anger_led, OUTPUT);
  pinMode(disgust_led, OUTPUT);
  pinMode(sad_led, OUTPUT);
  // set up PWM channels
  ledcSetup(red_channel, freq, resolution);
  ledcSetup(green_channel, freq, resolution);
  ledcSetup(blue_channel, freq, resolution);
  // attach pins to PWM channels
  ledcAttachPin(fear_led_red, red_channel);
  ledcAttachPin(fear_led_blue, blue_channel);

  turnOffAllLEDs();

  delay(1000);
  while(!myDFPlayer.begin(mySerial)){
    Serial.println("DFPlayer Mini not detected!");
  }
  Serial.println("DFPlayer Mini ready");
  myDFPlayer.volume(10);
  


}

void runDFPlayerPart(){
  // Keypad handling
  char key = keypad.getKey();
  if (!key) return;

  Serial.print("Key: "); Serial.println(key);

  switch (key) {
    case '1':
      turnOffAllLEDs();
      myDFPlayer.volume(10);
      myDFPlayer.play(1); // joy
      digitalWrite(joy_led, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("      Joy");
      break;
    case '2':
      turnOffAllLEDs();
      myDFPlayer.volume(20);
      myDFPlayer.play(3); // sadness
      digitalWrite(sad_led, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("    Sadness");
      break;
    case '3':
      turnOffAllLEDs();
      myDFPlayer.volume(6);
      myDFPlayer.play(5); // disgust
      digitalWrite(disgust_led, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("    Disgust");
      break;
    case '4':
      turnOffAllLEDs();
      myDFPlayer.volume(15);
      myDFPlayer.play(7); // anger
      digitalWrite(anger_led, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("     Anger");
      break;
    case '5':
      turnOffAllLEDs();
      myDFPlayer.volume(6);
      myDFPlayer.play(9); // fear
      setColor(80,0,80);
      lcd.setCursor(0, 0);
      lcd.print("      Fear");
      break;
    default:
      Serial.println("Invalid key!");
      lcd.setCursor(0, 0);
      lcd.print("Invalid key!");
      break;
  }




  if (myDFPlayer.available()) {
    int type = myDFPlayer.readType();
    Serial.print("Status: ");
    Serial.println(type);

    if (type == DFPlayerPlayFinished) {
      int fileNumber = myDFPlayer.readCurrentFileNumber();
      Serial.print("Finished playing file number: ");
      Serial.println(fileNumber);
  }
}
}



void runSensorsPart(){
  int vib_value = digitalRead(vibration_sensor);
  int temp_value = analogRead(temp_sensor);
  float voltage = temp_value * (3.3 / 4095.0); 
  float temperature = voltage * 100.0; 
  int ldr_value = analogRead(ldr);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(vib_value);
  lcd.print("   T:");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("LDR:");
  lcd.print(ldr_value);


  Serial.print("Vibration value: ");
  Serial.println(vib_value);
  if(vib_value == 1){
    setColor(80,0,80);
  }else{
    setColor(0,0,0);
  }

  
  Serial.print("Temperature value: ");
  Serial.println(temperature);
  if(temperature > 0){
    digitalWrite(anger_led, HIGH);
  }else{
    digitalWrite(anger_led, LOW);
  }

  
  Serial.print("LDR: ");
  Serial.println(ldr_value);
  if(ldr_value < 2000){
    digitalWrite(sad_led, HIGH);
  }else{
    digitalWrite(sad_led, LOW);
  }

  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(baseURL);
    http.addHeader("apikey", baseAPIkey);
    http.addHeader("Authorization", "bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InF2cWJob3B0cGVjdmZsaWRpcWlrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTQ1OTU4MTUsImV4cCI6MjA3MDE3MTgxNX0.RMohPWr8VmsC5lvFmN6Ys27CWByySwdlsks09O9N97k");

    // Prepare JSON for VIBRATION
    int vib = (vib_value == HIGH) ? 1 : 0;
    String jsonData_vibration = "{\"sensor_name\":\"vibration\", \"value\":" + String(vib) + "}";
    int httpResponseCode_vib = http.POST(jsonData_vibration);

    if(httpResponseCode_vib > 0){
      Serial.println("Vibration Data sent successfully: "+ String(httpResponseCode_vib));
      Serial.println(http.getString());
    }else{
      Serial.println("Error sending Vibration Data: "+ String(httpResponseCode_vib));
    }

    // Prepare JSON for TEMPERATURE
    int temp = (temperature > 0) ? temperature : 0;
    String jsonData_temperature = "{\"sensor_name\":\"Temperature\",\"value\":" + String(temp) + "}";
    int httpResponseCode_temp = http.POST(jsonData_temperature);

    if(httpResponseCode_temp > 0){
      Serial.println("Temperature Data sent successfully: "+ String(httpResponseCode_temp));
      Serial.println(http.getString());
    }else{
      Serial.println("Error sending Temperature Data: "+ String(httpResponseCode_temp));
    }

    // Prepare JSON for LDR
    String jsonData_LDR = "{\"sensor_name\":\"LDR\",\"value\":" + String(ldr_value) + "}";
    int httpResponseCode_ldr = http.POST(jsonData_LDR);

    if(httpResponseCode_ldr > 0){
      Serial.println("LDR Data sent successfully: "+ String(httpResponseCode_ldr));
      Serial.println(http.getString());
    }else{
      Serial.println("Error sending LDR Data: "+ String(httpResponseCode_ldr));
    }


    
    http.end();

  }
  delay(200);
}


const unsigned long debounceTime = 20;

bool last_dfplayer_state = HIGH;
bool button_dfplayer_state = HIGH;
unsigned long last_dfplayer_debounce_time = 0;

bool last_sensors_state = HIGH;
bool button_sensors_state = HIGH;
unsigned long last_sensors_debounce_time = 0;



// 
int mode = 0; // 0 = DFPlayer, 1 = Sensors
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;


unsigned long lastSensorRun = 0;
const unsigned long sensorInterval = 1000;


//  MQTT reconnect 
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe("esp32/character/emotion");
      client.subscribe("flutter/sound");          
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {


//------------------------------
  bool currentButtonState = digitalRead(button_dfplayer);
  Serial.print("Button: ");
  Serial.println(currentButtonState);


  if (currentButtonState != lastButtonState) {
      lastDebounceTime = millis();
    }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
      if (currentButtonState == LOW && buttonState == HIGH) {
        mode = !mode; 
        turnOffAllLEDs(); 
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(mode == 0 ? "Mode: Sensors" : "Mode: Sound");
      }
      buttonState = currentButtonState;
  }


  lastButtonState = currentButtonState;

  if (mode == 0) {
    if (millis() - lastSensorRun > sensorInterval) {
      runSensorsPart();
      lastSensorRun = millis();
    }
  } else {
    runDFPlayerPart();
    // MQTT
    if (!client.connected()) {
      reconnect();
    }
    // HiveMQ
    client.loop();

  }



}
