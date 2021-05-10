#include "TimerOne.h"
#include "Arduino_SensorKit.h"
#include "Pitches.h"
#include "MsTimer2.h"
#include "analogComp.h"
#include "U8g2lib.h"

// set map size to 16:8
const uint8_t SCREEN_WIDTH = 16;
const uint8_t SCREEN_HEIGHT = 8;

// map for printing a leaf
char mapLayout[SCREEN_HEIGHT][SCREEN_WIDTH+1] = {
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '*', ' ', ' ', ' ','\0'},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '*', ' ', ' ', ' ', ' ','\0'},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', '*', '*', '*', '*', ' ', ' ', ' ', ' ', ' ','\0'},
  {' ', ' ', ' ', ' ', ' ', ' ', '*', ' ', ' ', '*', '*', ' ', ' ', ' ', ' ', ' ','\0'},
  {' ', ' ', ' ', ' ', ' ', '*', ' ', ' ', '*', ' ', '*', ' ', ' ', ' ', ' ', ' ','\0'},
  {' ', ' ', ' ', ' ', '*', ' ', ' ', '*', ' ', ' ', '*', ' ', ' ', ' ', ' ', ' ','\0'},
  {' ', ' ', ' ', ' ', '*', ' ', '*', ' ', ' ', '*', ' ', ' ', ' ', ' ', ' ', ' ','\0'},
  {' ', ' ', ' ', '*', '*', '*', '*', '*', '*', ' ', ' ', ' ', ' ', ' ', ' ', ' ','\0' }};

uint8_t leaf[8] = {
 0x0C, 0x0C, 0x0A,0x3F, 0x38, 0x08, 0x38, 0x3C
};

uint8_t x = 255;
uint8_t y = 255;

//module1 starts
//notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

const int soundSensorPin = A2;
const int lightSensorPin = A3; 
const int LED_PIN = 6;
volatile bool led_state = false;
int lightValue = 0;  
int soundValue = 0;
volatile boolean enableLed = true; //check if the interrupt has raised
int buttonPressed = 0; // for checking if the button is pressed / pressed again

//interrupt to be raised by the analog comparator
void changeStatus() {
    enableLed = false; //let's inform the main loop that the condition has been reached by the analog comparator
}

int melody1(){
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    if(analogRead(lightSensorPin) < 400){
      break;
    }
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(5, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(5);
  } 
 return 0;
}

void toggleLED(){ // blink the LED if sound volume is higher than designated value
  led_state = !led_state;
  digitalWrite(LED_PIN, led_state);
}
//module1 ends

//module2 starts
const int BUTTON_PIN = 4; // connect button pin to control the oled display
float humid,temp,pres; // define humidity, temperature and air pressure variables
boolean switchFunction = true; // a boolean value for switching between different sensors
int flag = 0; // set a 0/1 value to control the frequency of humidity and air pressure sensors' appearance

// read and show humidity, temperature and air pressure data
void AHT() { 
  switchFunction =! switchFunction; //control the switch between different sensors
  flag = 1; // manage flashing problems when display on OLED
}
//module2 ends

//module3 starts
const uint8_t POT_PIN = A0;


int pot_value;
int led_value;


int melody_2[] = {
  NOTE_D6,NOTE_D7,NOTE_DH1,NOTE_D7,NOTE_DH1,NOTE_DH3,NOTE_D7,NOTE_D7,NOTE_D7,NOTE_D3,NOTE_D3,
  NOTE_D6,NOTE_D5,NOTE_D6,NOTE_DH1,NOTE_D5,NOTE_D5,NOTE_D5,NOTE_D3,NOTE_D4,NOTE_D3,NOTE_D4,NOTE_DH1,
  NOTE_D3,NOTE_D3  ,NOTE_DH1,NOTE_DH1,NOTE_DH1,NOTE_D7,NOTE_D4,NOTE_D4,NOTE_D7,NOTE_D7,NOTE_D7
};

double noteDurations2[] = {
  0.5,0.5,1+0.5,0.5,1,1,1,1,1,0.5,0.5,
  1+0.5,0.5,1,1,1,1,1,1,1+0.5,0.5,1,1, 
  1,1,0.5,0.5,0.5,1+0.5,0.5,1,1,1,1
};

void melody2(){
  for(int x=0;x<sizeof(melody_2)/sizeof(melody_2[0]);x++) 
  {  
    if(Pressure.readTemperature()< 27){
      break;
    }
    tone(5,melody_2[x]); // play the notes in the matrix one by one 
    delay(400*noteDurations2[x]); // control the speed when playing the melody
    noTone(5);// stop playing current note and start the next one
  } 
}
//module3 ends

void setup() {
  //module1 starts
    pinMode(lightSensorPin, INPUT);
    pinMode(soundSensorPin, INPUT);

    digitalWrite(lightSensorPin,LOW);
    digitalWrite(soundSensorPin,LOW);
    
    Serial.begin(9600);
    analogComparator.setOn(AIN0, AIN1); //we instruct the lib to use voltages on the pins
  //module1 ends
  
  //module2 starts
    pinMode(BUTTON_PIN,INPUT);  // connect the button's pin
    Environment.begin(); // initiate humidity and temperature sensor
    Pressure.begin(); // initiate air pressure sensor
    Timer1.initialize(5000000); // set the display interval to 5 sec
    Timer1.attachInterrupt(AHT);
    Oled.begin();
    Oled.clearDisplay(); // clear previous data if there is any
    Oled.setFlipMode(true); 
    //module2 ends

  //module3 starts 
    pinMode(LED_PIN, OUTPUT);
    pinMode(POT_PIN, INPUT);
  //module3 ends
   
}

void loop() {
    //module1 starts
    float light = analogRead(lightSensorPin);
    float lightControl = pot_value = analogRead(POT_PIN);
    lightValue = light + lightControl;
    if(lightValue > 600){
      melody1();
      }else{
        noTone(5);
      }
  
    soundValue = analogRead(soundSensorPin);
    if(soundValue > 450){
      // if the sound volume is higher than the threshold, the LED flashes
      MsTimer2::set(600,toggleLED);
      MsTimer2::start();
      delay(5000);
    }else{
        // if the volume is lower than the threshold, stop flashing the LED 
      led_state = false;
      MsTimer2::stop();
    }
      Serial.println(soundValue);
    //module1 ends
  
    //module2 starts
    int value = digitalRead(BUTTON_PIN); //read button input
    humid = Environment.readHumidity(); 
    temp = Pressure.readTemperature(); 
    pres = Pressure.readPressure(); 
    if (value == 1){
       if (buttonPressed == 0){
         Oled.clearDisplay(); // press button to clear data and load leaf image
         for (int i=0; i<SCREEN_HEIGHT; i++) {
            Oled.setCursor(0, i);
            Oled.print(mapLayout[i]);    
         }
         buttonPressed = 1;
       }else if (buttonPressed == 1){
         buttonPressed = 0;
       }
    }
    if(switchFunction == true && buttonPressed == 0){
      if(flag == 1){
        Oled.clearDisplay();
        Oled.setCursor(0,10); // text position
        Oled.setFont(u8x8_font_chroma48medium8_r); //text font
        Oled.println("Air Pressure:"); //show air pressure
        Oled.println("");
        Oled.print (pres);
        Oled.println ("pa");
        flag = 0;
      }
    }else if( switchFunction == false && buttonPressed == 0){
      if(flag == 1){
        Oled.clearDisplay();
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
        flag = 0;
      }
    }
  
    //module2 ends

    //module3 starts
    //if temperature is higher than 27C, paly melody2 as an alarm
    if(temp>= 27){
        melody2();
    }else{
      //change the lightness of LED by rotating the potentiometer
        pot_value = analogRead(POT_PIN);
        led_value = map(pot_value, 0, 1023, 0, 255);
        analogWrite(LED_PIN, led_value);
    }
    delay(1);
    //module3 ends
}
