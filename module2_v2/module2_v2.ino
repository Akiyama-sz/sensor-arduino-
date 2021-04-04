#include "TimerOne.h"
#include "Arduino_SensorKit.h"


volatile bool switchFunction = false; // a boolean value for switching between different sensors
const int BUTTON_PIN = 4; // connect button pin to control the oled display


// read and show humidity, temperature and air pressure data
void AHT() { 
  switchFunction =! switchFunction;
  float humid = Environment.readHumidity(); 
  float temp = Environment.readTemperature(); 
  float pres = Pressure.readPressure(); 
  if (switchFunction){
      Oled.clearDisplay(); // update data
      Oled.setCursor(0,10); // text position
      Oled.setFont(u8x8_font_chroma48medium8_r); //text font
      Oled.println("Air Pressure:"); //show air pressure
      Oled.println("");
      Oled.print (pres);
      Oled.println ("pa");
  }else{
      Oled.clearDisplay(); // update data
      Oled.setCursor(0,0); // text position
      Oled.setFont(u8x8_font_chroma48medium8_r); //text font
      Oled.println ("");
      Oled.println ("Humidity:"); //show humidity
      Oled.print (humid);
      Oled.println (" %");
      Oled.println ("");
      Oled.println ("Temperature:"); //show temperature on Celsius scale
      Oled.print (temp);
      Oled.print (" C");
      Oled.println ("");
   }
}

void setup() {
    Timer1.initialize(5000000); // set the display interval to 5 sec
    Oled.begin(); // initiate oled board
    Oled.clearDisplay(); // clear previous content if there is any
    Oled.setFlipMode(true); // flip the direction of texts
    Pressure.begin(); // initiate air pressure sensor
    Environment.begin(); // initiate humidity and temperature sensor
    Timer1.attachInterrupt(AHT);
    pinMode(BUTTON_PIN,INPUT);  // connect button's pin
}

void loop() {
    //read button input
    int value = digitalRead(BUTTON_PIN); 
    if (value == 1){
       Oled.clearDisplay(); // press button to clear(turn off) the oled board
    }
}
