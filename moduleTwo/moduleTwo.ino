#include "Arduino_SensorKit.h"

int timer = 0; //set initial time value
int switchFunction = 0; // a 0/1 variable for switching between different sensors
const int BUTTON_PIN = 4; // connect button pin to control the oled display

void setup() {
   Serial.begin(9600);
   Pressure.begin(); // initiate air pressure sensor
   Environment.begin(); // initiate humidity and temperature sensor
   Oled.begin(); // initiate oled board
   Oled.clearDisplay(); // clear previous content if there is any
   Oled.setFlipMode(true); // flip the direction of texts
   pinMode(BUTTON_PIN,INPUT);  // connect button's pin
}

void loop() {
   //   read humidity, temperature and air pressure data
   float humid = Environment.readHumidity(); 
   float temp = Environment.readTemperature(); 
   float pres = Pressure.readPressure(); 

   //read button input
   int value = digitalRead(BUTTON_PIN); 
   
   //start counting time
   countTime();

   // if button is pressed, clear board; if button is released, continue to show data
   if (value == 1){
       clearBoard(); // press button to clear(turn off) the oled board
       if (timer >= 10){
          timer = 0; // let the timer continue to work properly during turning off the board
       }
   }else if (value == 0){
      if ((timer > 0) && (timer <= 5) && (switchFunction == 0)){ 
          Oled.clearDisplay(); // update data
          Oled.setCursor(0,10); // text position
          Oled.setFont(u8x8_font_chroma48medium8_r); //text font
          Oled.println("Air Pressure: "); //show air pressure
          Oled.println("");
          Oled.print (pres);
          Oled.println ("pa");
          switchFunction = 1;
       }else if ((timer >= 6) && (timer < 10) && (switchFunction == 1)){
          Oled.clearDisplay(); // update data
          Oled.setCursor(0,0); // text position
          Oled.setFont(u8x8_font_chroma48medium8_r); //text font
          Oled.println ("");
          Oled.println ("Humidity: "); //show humidity
          Oled.print (humid);
          Oled.println ("%");
          Oled.println ("");
          Oled.println ("Temperature: "); //show temperature on Celsius scale
          Oled.print (temp);
          Oled.print (" C");
          Oled.println ("");
          switchFunction = 0;
       }else if (timer >= 10){
          timer = 0; //if time exceeds 10 seconds, count from 0 again
       }
   }
}
   

void countTime(){
     timer ++;
     delay(1000); //set a timer to count seconds
}   

void clearBoard(){
     Oled.clearDisplay(); // if button is pressed, clear board
}
