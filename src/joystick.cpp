// #include <Arduino.h>
// #include <driver/adc.h>

// int vrx;
// int vry;
// int val;

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);

//   // adc1_config_width();
//   // adc1_config_channel_atten();
//   pinMode(32, INPUT); //vrx
//   pinMode(35, INPUT); //vry
  
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   val = adc1_get_raw(ADC1_CHANNEL_4);
//   vrx = analogRead(32);
//   vry = analogRead(35);
//   Serial.print("vrx = ");
//   Serial.println(val);
//   //Serial.print("vry = ");
//   Serial.println(vry);
//   delay(2000);
// }



// #include <Arduino.h>

// int switch1;

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   pinMode(19, INPUT); //switch1
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   switch1 = digitalRead(19);
//   Serial.print("switch1 = ");
//   Serial.println(switch1);
//   delay(500);
// }