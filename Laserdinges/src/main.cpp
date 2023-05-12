// code voor de esp aan het laserpaneel
// naam esp: esplaser
// wachtwoord esp: esplaser

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "OTAlib.h"
#include <esp_wifi.h>
#define SSID1          "NETGEAR68"
//OTA
OTAlib ota("NETGEAR68", "excitedtuba713");


int sw1, sw2, sw3;  // selectie motor 
int pin_sw1 = 19;
int pin_sw2 = 18; 
int pin_sw3 = 21;  
bool motor1, motor2, motor3; 

int vrx;  // reading joystick (zijwaarts draaien spiegels)
int vry;  // reading joystick (omhoog - omlaag draaien spiegels)
int pin_vrx = 33 ;  // adc pin
int pin_vry = 32;  // adc pin

uint8_t broadcastAddress_motor1[] = {0xAC, 0x67, 0xB2, 0x30, 0x29, 0xE8};  //mac adress van esp verbonden met motor1
uint8_t broadcastAddress_motor2[] = {0x40, 0x22, 0xD8, 0xE9, 0x11, 0xC8};  //mac adress van esp verbonden met motor2
uint8_t broadcastAddress_motor3[] = {0xB4, 0x8A, 0x0A, 0x46, 0xA6, 0x6C};  //mac adress van esp verbonden met motor3


// Variable to store if sending data was successful
String success;

esp_err_t result;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {  //enkel waarden van joystick doorsturen naar juiste motor
    int vrx_send;
    int vry_send;
} struct_message;

struct_message joystick_readings;
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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


void setup(){

  // Init Serial Monitor
  Serial.begin(115200);
  Serial.println("setup begin");

  // OTA
  ota.setHostname("esplaser");  
  ota.setPassword("esplaser");
  ota.begin();

  // motoren en schakelaars
  pinMode(pin_sw1, INPUT);
  pinMode(pin_sw2, INPUT);
  pinMode(pin_sw3, INPUT);

  // joystick
  pinMode(pin_vrx, INPUT);
  pinMode(pin_vry, INPUT);

  //communicatie
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_AP_STA);
  int32_t channel =getWiFiChannel(SSID1);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // motor1
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress_motor1, 6);

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 1");
  }
  else if (esp_now_add_peer(&peerInfo) == ESP_OK){
    Serial.println("peer 1 added");
  }

  // motor2
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress_motor2, 6);
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 2");
  }
  else if (esp_now_add_peer(&peerInfo) == ESP_OK){
    Serial.println("peer 2 added");
  }

  // motor3
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress_motor3, 6);
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 3");
  }
  else if (esp_now_add_peer(&peerInfo) == ESP_OK){
    Serial.println("peer 3 added");
  }
  Serial.println("Setup done");
}

void loop(){
  // motoren en schakelaars
  sw1 = digitalRead(pin_sw1);
  Serial.print("sw1 = ");
  Serial.println(sw1);
  sw2 = digitalRead(pin_sw2);
  Serial.print("sw2 = ");
  Serial.println(sw2);
  sw3 = digitalRead(pin_sw3);
  Serial.print("sw3 = ");
  Serial.println(sw3);

  if(sw1==1 & sw2==0 & sw3==0){ //prioriteiten van de motoren 1>2>3
    motor1 = true;
    motor2 = false;
    motor3 = false;
    Serial.println("motor 1 gekozen");
  }
  else if(sw1==0 & sw2==1 & sw3==0){
    motor1 = false;
    motor2 = true;
    motor3 = false;
    Serial.println("motor 2 gekozen");
  }
  else if(sw1==0 & sw2==0 & sw3==1){
    motor1 = false;
    motor2 = false;
    motor3 = true;
    Serial.println("motor 3 gekozen");    
  }
  else{
    motor1 = false;
    motor2 = false;
    motor3 = false;
  }

  // joystick
  vrx = analogRead(pin_vrx);
  vry = analogRead(pin_vry);

  joystick_readings.vrx_send = vrx;
  joystick_readings.vry_send = vry;

  // communicatie
  if(motor1){
    esp_err_t result = esp_now_send(broadcastAddress_motor1, (uint8_t *) &joystick_readings, sizeof(joystick_readings));
  }
  if(motor2){
    esp_err_t result = esp_now_send(broadcastAddress_motor2, (uint8_t *) &joystick_readings, sizeof(joystick_readings));
  }
  if(motor3){
    esp_err_t result = esp_now_send(broadcastAddress_motor3, (uint8_t *) &joystick_readings, sizeof(joystick_readings));
  }
  // else{
  //   esp_err_t result = ESP_ERR_INVALID_CRC ;  // hier mag hij niets versturen
  // }

  if (result == ESP_OK) {
    Serial.println("Sent with success");
    Serial.println(vrx);
    Serial.println(vry);
  }
  else {
    Serial.println("Error sending the data");
  }
 
  vTaskDelay(500);  // elke 0.2s opnieuw meten en doorsturen
  //taskYIELD();

}