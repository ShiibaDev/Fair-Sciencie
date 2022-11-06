#include <Arduino.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <WiFi101.h>

#include <../lib/WiFiDef.h>

// char ssid[] = WIFI_SSID;
// char pass[] = WIFI_PASSWORD;

char ssid[] = PRIVATE_SCH_IDENTIFICATION;
char pass[] = PRIVATE_SCH_CONNECTION;

const char *mqtt_server = "10.12.12.240";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Led pins
const int warning = 13;
const int Failure = 11;
const int Safe = 12;
const int Ans = 14;

// Functions Definition
void RSSIPrint() {
  long rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.println(rssi);
}

void printData()
{
  Serial.println("Board Information: ");
  // Print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println();
  Serial.println("Network Information");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal strenght (RSSI):");
  Serial.println(rssi);

  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();

  int Status = WiFi.status();
  Serial.print("Wifi Status is: ");
  Serial.println(Status);
}

void setup_wifi() {
  delay(10);

  // Start connecting to the wifi

  while (WiFi.status() != WL_CONNECTED)
  {
    // Turn on the led, meaning is Attempting to connect to a network
    digitalWrite(Failure, HIGH);
    digitalWrite(warning, HIGH);

    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);

    // Connect to WPA/WPA2
    WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  digitalWrite(Failure, LOW);
  digitalWrite(Safe, HIGH);

  if (WiFi.status() == WL_CONNECTION_LOST)
  {
    digitalWrite(warning, HIGH);
    digitalWrite(Failure, HIGH);
    digitalWrite(Safe, LOW);
  } else {
    digitalWrite(warning, LOW);
    digitalWrite(Failure, LOW);
    digitalWrite(Safe, HIGH);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");

  Serial.println("================================");
  printData();
  Serial.println("================================");
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic MKR1000/ANS,
  // Changes the output state according to the message
  if (String(topic) == "MKR1000/ANS") {
    Serial.print("Changing to ");
    if (messageTemp == "on") {
      Serial.print("on");
      digitalWrite(Ans, HIGH);
    } else if (messageTemp == "off") {
      Serial.print("off");
      digitalWrite(Ans, LOW);
    }
  }
}

void WiFiBoardConnection() {
  // We gonna detect if the wifi connected or not

  if (WiFi.status() == WL_CONNECTED) {
    pinMode(Safe, OUTPUT);

    digitalWrite(Safe, HIGH);
    delay(1250);
    digitalWrite(Safe, LOW);
    delay(1250);
    digitalWrite(Safe, HIGH);
    delay(1250);
    digitalWrite(Safe, LOW);
    delay(1250);
    digitalWrite(Safe, HIGH);
  } else if (WiFi.status() != WL_CONNECTED) {
    pinMode(warning, OUTPUT);

    digitalWrite(warning, HIGH);
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(warning, LOW);
    }
  }
}

// Main()
void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  pinMode(warning, OUTPUT);
  pinMode(Failure, OUTPUT);
  pinMode(Safe, OUTPUT);
  pinMode(Ans, OUTPUT);

  // Functions
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  WiFiBoardConnection();


  // attempt to connect to WiFi network:
  // Upside is the Internet connection system
}

void loop() {
  // put your main code here, to run repeatedly:

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(warning, HIGH);
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(warning, LOW);
      digitalWrite(Safe, HIGH);
    }
  }
  client.loop();
}