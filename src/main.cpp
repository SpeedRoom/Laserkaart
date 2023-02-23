// code voor de esp aan het laserpaneel

#include <arduino.h>
#include <esp_now.h>
#include <WiFi.h>


int sw1;  // selectie motor 1
int sw2;  // selectie motor 2
int sw3;  // selectie motor 3
int pin_sw1, pin_sw2, pin_sw3;  // digitale pin
int motor1, motor2, motor3; 

int vrx;  // reading joystick (zijwaarts draaien spiegels)
int vry;  // reading joystick (omhoog - omlaag draaien spiegels)
int pin_vrx, pin_vry;  // analoge pin

uint8_t broadcastAddress_motor1[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  //mac adress van esp verbonden met motor1
uint8_t broadcastAddress_motor2[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  //mac adress van esp verbonden met motor2
uint8_t broadcastAddress_motor3[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  //mac adress van esp verbonden met motor3


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
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

void setup(){
  // Init Serial Monitor
  Serial.begin(115200);

  // motoren en schakelaars
  pinMode(pin_sw1, INPUT);
  pinMode(pin_sw2, INPUT);
  pinMode(pin_sw3, INPUT);

  // joystick
  pinMode(pin_vrx, INPUT);
  pinMode(pin_vry, INPUT);

  //communicatie
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // motor1
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress_motor1, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // motor2
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress_motor2, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // motor3
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress_motor3, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
}

void loop(){
  // motoren en schakelaars
  sw1 = digitalRead(pin_sw1);
  sw2 = digitalRead(pin_sw2);
  sw3 = digitalRead(pin_sw3);

  if(sw1==1 & sw2==0 & sw3==0){ //prioriteiten van de motoren 1>2>3
    motor1 = 1;
    motor2 = 0;
    motor3 = 0;
  }
  else if(sw1==0 & sw2==1 & sw3==0){
    motor1 = 0;
    motor2 = 1;
    motor3 = 0;
  }
  else if(sw1==0 & sw2==0 & sw3==1){
    motor1 = 0;
    motor2 = 0;
    motor3 = 1;    
  }
  else{
    motor1 = 0;
    motor2 = 0;
    motor3 = 0;
  }

  // joystick
  vrx = analogRead(pin_vrx);
  vry = analogRead(pin_vry);

  joystick_readings.vrx_send = vrx;
  joystick_readings.vry_send = vry;

  // communicatie
  if(motor1==1){
    esp_err_t result = esp_now_send(broadcastAddress_motor1, (uint8_t *) &joystick_readings, sizeof(joystick_readings));
  }
  else if(motor2==1){
    esp_err_t result = esp_now_send(broadcastAddress_motor2, (uint8_t *) &joystick_readings, sizeof(joystick_readings));
  }
  else if(motor3==1){
    esp_err_t result = esp_now_send(broadcastAddress_motor3, (uint8_t *) &joystick_readings, sizeof(joystick_readings));
  }
  else{
    esp_err_t result = ESP_OK;
  }

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
 
  delay(200);  // elke 0.2s opnieuw meten en doorsturen

}