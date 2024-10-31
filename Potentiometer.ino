#include <WiFi.h>
#include <PubSubClient.h>

// ขาอนาล็อกที่เชื่อมต่อกับตัวต้านทานปรับค่า
const int potPin = A0;

// ข้อมูลการเชื่อมต่อ WiFi
const char* ssid = "iqxemS <3";        // ใส่ชื่อ WiFi ของคุณ
const char* password = "192329358"; // ใส่รหัสผ่าน WiFi ของคุณ

// ข้อมูลการเชื่อมต่อ MQTT
const char* mqttServer = "phycom.it.kmitl.ac.th";
const int mqttPort = 1883;
const char* mqttClientID = "client_a8939f4f";
const char* topic = "66070109/light";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(115200);
  
  // เชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // ตั้งค่า MQTT server
  client.setServer(mqttServer, mqttPort);
}

void reconnect() {
  // พยายามเชื่อมต่อกับ MQTT broker
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(mqttClientID)) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // อ่านค่าจากตัวต้านทานปรับค่า
  int potValue = analogRead(potPin); // ค่าอ่านได้อยู่ในช่วง 0-1023

  // แปลงค่าเป็นข้อความ
  char potString[8];
  itoa(potValue, potString, 10); // แปลงค่าเป็นข้อความในฐานสิบ

  // ส่งค่าตัวต้านทานปรับค่าไปยัง MQTT topic
  Serial.print("Publishing pot value: ");
  Serial.println(potString);
  client.publish(topic, potString);

  delay(5000);  // ส่งข้อมูลทุกๆ 5 วินาที
}
