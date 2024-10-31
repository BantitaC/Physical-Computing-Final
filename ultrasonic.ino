#include <WiFi.h>
#include <PubSubClient.h>

// ขาเชื่อมต่อ Ultrasonic Sensor
const int trigPin = 9;  // ขาที่เชื่อมต่อกับ Trigger pin
const int echoPin = 10; // ขาที่เชื่อมต่อกับ Echo pin

// ข้อมูลการเชื่อมต่อ WiFi
const char* ssid = "iqxemS <3";        // ใส่ชื่อ WiFi ของคุณ
const char* password = "192329358"; // ใส่รหัสผ่าน WiFi ของคุณ

// ข้อมูลการเชื่อมต่อ MQTT
const char* mqttServer = "phycom.it.kmitl.ac.th";
const int mqttPort = 1883;
const char* mqttClientID = "client_3d592a95";
const char* topic = "66070109/food";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(115200);
  
  // ตั้งค่าขา Ultrasonic
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

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

long measureDistance() {
  // ส่ง Pulse เพื่อเริ่มการวัดระยะ
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // อ่านค่าจาก Echo pin
  long duration = pulseIn(echoPin, HIGH);
  long distance = (duration * 0.034) / 2; // แปลงเป็นเซนติเมตร
  return distance;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // วัดระยะทาง
  long distance = measureDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // ตรวจสอบระยะทางและส่งข้อมูล
  if (distance > 20) {
    // ส่งคำว่า "off" ไปยัง MQTT topic
    Serial.println("Publishing: off");
    client.publish(topic, "off");
  }
  else{
    Serial.println("Publishing: on");
    client.publish(topic, "on");
  }

  delay(5000);  // วัดระยะทุกๆ 5 วินาที
}
