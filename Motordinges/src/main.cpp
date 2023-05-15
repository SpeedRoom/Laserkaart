// code voor de esp's aan de motoren

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include "OTAlib.h"
#include <PubSubClient.h>
#include <esp_wifi.h>

//OTA
OTAlib ota("telenet-64BBF56", "hTHzysaudk73");







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

void setup() {
  Serial.begin(115200);

  // motoren
  pinMode(pin_pwm1, OUTPUT);
  pinMode(pin_dir1, OUTPUT);
  pinMode(pin_pwm2, OUTPUT);
  pinMode(pin_dir2, OUTPUT);


  //communicatie
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_AP_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  taskYIELD();
}

void loop() {
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
  taskYIELD();
}