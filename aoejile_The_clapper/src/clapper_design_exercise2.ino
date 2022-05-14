#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

const int UPDATE_PERIOD = 20;
const uint8_t PIN_1 = 1; //button 1
// const uint8_t PIN_2 = 39; //button 2
uint32_t primary_timer;
float sample_rate = 2000; //Hz
float sample_period = (int)(1e6 / sample_rate);


uint8_t state;
uint8_t state2;
int count = 0;
int clap_count;
const uint8_t OFF = 0;
// const uint8_t RED = 1;
// const uint8_t BLUE = 2;
const uint8_t DOWN = 1;
const uint8_t UP = 2;
const uint8_t finish = 3;
const uint8_t IDLE = 4;
const uint8_t RED = 5;
const uint8_t BLUE = 6;
unsigned long timer;
unsigned long timer2;
float values[50];
int indx = 0;
int count2 = 0;


void setup() {
  Serial.begin(115200);               // Set up serial port
  // pinMode(PIN_1, INPUT_PULLUP);
  analogReadResolution(12);       // initialize the analog resolution

  primary_timer = millis();
  render_counter = 0;
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set col
  for (int i=0; i<50; i++){ 
    values[i] = 0.0;
  }
  clap_count = 0;
  state = IDLE;
  state2 = OFF;
}

void loop() {
      float avg = abs(averaging_filter(analogRead(A0) - 1800, values, 35, &indx));
      fsm2(avg);
      color_fsm();
      char output[70];
      sprintf(output, "color state:  %d, state: %d, clap count: %d, avg:  %f", state2, state, clap_count, avg);
      Serial.println(output);
    // while (micros() > primary_timer && micros() - primary_timer < sample_period); //prevents rollover glitch every 71 minutes...not super needed
    // primary_timer = micros();
      while (millis() - primary_timer < 10);
      primary_timer = millis();
  }

float averaging_filter(float input, float* stored_values, int order, int* index) {
   float multiplier = 1.0 /(order+1);
   stored_values[*index] = input;
   float output = 0;
   for (int i = 0; i <= order; i++){
     if (order == 0){
     return input;
     }
     else{
     output += stored_values[i];
     }
   }

   *index += 1;
   if (*index > order){
    *index = 0;
  }
   return multiplier*output;
}

void fsm2(float input){
  uint8_t haha = input;
    char output2[50];
    sprintf(output2, " input: %f  ", haha);
    Serial.println(output2);
  switch(state){
    case IDLE:
    if (input > 140){
      state = DOWN;
    }
  break;
  case DOWN:
    if (input < 90){
      count = 0;
      state = UP;
      timer = millis();
    }
  break;
  case UP:
    if (count == 0){
    count = 1;
    clap_count += 1;
    }
    if (millis() - timer < 1000 && millis() - timer > 500){
      if (input > 140){
        state = DOWN;
      }
      // else{
      //   state = finish;
      //   timer2 = millis();
      // }
    }
    else if (millis()-timer > 1000){
      state = finish;
      timer2 = millis();
    }
    break;
  case finish:
    if (millis() - timer2 > 2000){
    state = IDLE;
    clap_count = 0;
    }
  break;
}
}


void color_fsm(){
switch(state2){
  case OFF:
  tft.fillScreen(TFT_BLACK);
  count2 = 0;
  if (clap_count == 2 && state == finish && count2 == 0){
    state2 = RED;
    tft.fillScreen(TFT_RED);
    count2 = 1;
  }
  else if (clap_count == 3 && state == finish && count2 == 0){
    state2 = BLUE;
    tft.fillScreen(TFT_BLUE);
    count2 = 1;
  }
  break;
  case RED:
   count2 = 0;
   if (clap_count == 2 && state == finish && count2 == 0 ){
    state2 = OFF;
    tft.fillScreen(TFT_BLACK);
    count2 = 1;
  }
   else if (clap_count == 3 && state == finish && count2 == 0){
    state2 = BLUE;
    tft.fillScreen(TFT_BLUE);
    count2 = 1;
  }
  break;
  case BLUE:
   count2 = 0;
   if (clap_count == 3 && state == finish && count2 == 0){
    state2 = OFF;
    tft.fillScreen(TFT_BLACK);
    count2 = 1;
  }
   else if (clap_count == 2 && state == finish && count2 == 0){
    state2 = RED;
    tft.fillScreen(TFT_RED);
    count2 = 1;
  }
  break;
}
}
  


