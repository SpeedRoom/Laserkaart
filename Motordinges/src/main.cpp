// code voor de esp's aan de motoren
// naam esp: espmotor
// wachtwoord esp: espmotor

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "OTAlib.h"
#include <PubSubClient.h>
#include <esp_wifi.h>

//OTA
OTAlib ota("NETGEAR68", "excitedtuba713");

//MQTT -
#define SSID1          "NETGEAR68"
#define PWD           "excitedtuba713"
#define MQTT_SERVER   "192.168.1.61"  
#define MQTT_PORT     1883
#define topic  "esp_motor/output"

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi..");
  WiFi.begin(SSID1, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // creat unique client ID
    // in Mosquitto broker enable anom. access
    if (client.connect("ESP32Client"))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe(topic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 second");
      vTaskDelay(1000/portTICK_RATE_MS);
    }
    taskYIELD();
  }
}
//- MQTT

#define JOYSTICK_LAAG 500
#define JOYSTICK_HOOG 1500

//motoren
int pin_pwm1 = 25;  // pwm motor 1 (horizontale beweging)
int pin_dir1 = 23;  // direction motor 1 (horizontale beweging)
int pin_pwm2 = 26;  // pwm motor 2 (verticale beweging)
int pin_dir2 = 13;  // direction motor 2 (verticale beweging)

//communicatie
//Structure example to receive data
//Must match the send structure
typedef struct struct_message {  
    int vrx_send;
    int vry_send;
} struct_message;

int incoming_vrx;
int incoming_vry;

struct_message joystick_readings;
esp_now_peer_info_t peerInfo;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&joystick_readings, incomingData, sizeof(joystick_readings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incoming_vrx = joystick_readings.vrx_send;
  incoming_vry = joystick_readings.vry_send;
  Serial.println(incoming_vrx);
}

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}

void setup() {


  Serial.begin(115200);

  //MQTT -
  setup_wifi();
  // client.setServer(MQTT_SERVER, MQTT_PORT);
  // - MQTT

  // OTA
  ota.setHostname("espmotor1");  
  ota.setPassword("espmotor1");
  ota.begin();

  // motoren
  pinMode(pin_pwm1, OUTPUT);
  pinMode(pin_dir1, OUTPUT);
  pinMode(pin_pwm2, OUTPUT);
  pinMode(pin_dir2, OUTPUT);
  
  digitalWrite(pin_pwm1, HIGH);  // motor 1 draait niet
  digitalWrite(pin_pwm2, HIGH);  // motor 2 draait niet
  Serial.println("motoren draaien niet");
  //communicatie
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_AP_STA);
  int32_t channel =getWiFiChannel(SSID1);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
  }

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

}

void loop() {

  //MQTT -
  // if (!client.connected()){
  //   reconnect();
  // }
  // client.loop();
  //- MQTT
  //client.publish(topic, "loop");
  client.publish(topic, (char*) incoming_vrx);
  client.publish(topic, (char*) incoming_vry);
  // client.publish(topic, (char*) WiFi.macAddress);

  // motor 1 besturen met vrx
  if(incoming_vrx < JOYSTICK_LAAG){
    digitalWrite(pin_dir1, HIGH);  // motor 1 draait in ene richting
    digitalWrite(pin_pwm1, LOW);  // motor 1 draait
  }
  else if(incoming_vrx > JOYSTICK_HOOG){
    digitalWrite(pin_dir1, LOW);  // motor 1 draait in andere richting
    digitalWrite(pin_pwm1, LOW);  // motor 1 draait
  }
  else{
    digitalWrite(pin_pwm1, HIGH);  // motor 1 draait niet
  }

  // motor 2 besturen met vry
  if(incoming_vry < JOYSTICK_LAAG){
    digitalWrite(pin_dir2, HIGH);  // motor 2 draait in ene richting
    digitalWrite(pin_pwm2, LOW);  // motor 2 draait
  }
  else if(incoming_vry > JOYSTICK_HOOG){
    digitalWrite(pin_dir2, LOW);  // motor 2 draait in andere richting
    digitalWrite(pin_pwm2, LOW);  // motor 2 draait
  }
  else{
    digitalWrite(pin_pwm2, HIGH);  // motor 2 draait niet
  }

  delay(200);  // elke 0.2s opnieuw meten en doorsturen
  //taskYIELD();

}