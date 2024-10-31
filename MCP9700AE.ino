#include <WiFi.h>
#include <PubSubClient.h>

const int tempPin = A0;  // ขาอนาล็อกที่เชื่อมต่อกับ MCP9700AE

// ข้อมูลการเชื่อมต่อ WiFi
const char* ssid = "iqxemS <3";       // ใส่ชื่อ WiFi ของคุณ
const char* password = "";  // ใส่รหัสผ่าน WiFi ของคุณ

// ข้อมูลการเชื่อมต่อ MQTT
const char* mqttServer = "phycom.it.kmitl.ac.th";
const int mqttPort = 1883; // ใช้พอร์ต 1883 ถ้าไม่ใช้ SSL/TLS
const char* mqttClientID = "client_6959fa78";
const char* topic = "testtopic/109";

WiFiClient wifiClient; // ใช้ WiFiClient
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

  // อ่านค่าอนาล็อกจาก MCP9700AE
  int analogValue = analogRead(tempPin);
  float voltage = analogValue * (5.0 / 1023.0);     // แปลงค่าเป็นโวลต์
  float temperatureC = (voltage - 0.5) * 100;       // แปลงโวลต์เป็นอุณหภูมิ (เซลเซียส)

  // แปลงอุณหภูมิเป็นข้อความ
  char tempString[8];
  dtostrf(temperatureC, 1, 2, tempString);

  // ส่งค่าอุณหภูมิไปยัง MQTT topic
  Serial.print("Publishing temperature: ");
  Serial.println(tempString);
  client.publish(topic, tempString);

  delay(5000);  // ส่งข้อมูลทุกๆ 5 วินาที

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
