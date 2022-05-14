# Overview



The clapper is meant to be a device that listens to a user's clap and changes the screen color based on the number of claps. When the screen is black, two claps should turn the screen red, and three claps should turn the screen blue. When the screen is red, three claps should turn it blue, and two claps should turn it black(off). When the screen is blue, two claps should turn it red, and three claps should turn if black(off).

To record my sound values, I had an averaging filter that took an average of the past 35 sound values recorded by the microphone. I chose this high number to avoid outlier numbers affecting my clap state machine. This allowed for a consitent, almost horizontal line on my serial plotter when I was not clapping. I also took the absolute value of this value to avoid negative sound values. This recording the number of claps easier.
```cpp
float avg = abs(averaging_filter(analogRead(A0) - 1800, values, 35, &indx));
```

In order to track the number of claps, I had a state machine with four states. OFF or IDlE, DOWN(which indicated a clap had been recognized, or in other words, a high sound output over a certain threshold had been recorded), UP(the sound output had gone below a certain level), and Finish(after all claps have been recorded). After a clap is heard in the IDLE state(or the sound value goes above 140), we move to the down state and wait untill the sound output dips below the threshold 90. This is crucial in ensuring that continous sounds do not register as multiple claps. We wait untill the sound value dips below 90 to record a clap. Once the sound value has dipped below 90, we start a timer and move to the up state. In the up state, while the timer is between 500 milliseconds and 1 second(extra insurance that multiple claps close together do not register as multiple claps, and to ensure that claps more than a second apart are not in the same clap sequence), we check if our sound value goes above the clap threshold of 140 again. If it does, we move to the down state and repeat the process. If we exceed this timer and no clap is heard, we move to the finish state. In the finish state, we wait for 2 seconds before moving to the IDLE state again. This ensures a two second buffer after a screen color change.
```cpp

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
```





After getting the number of claps from my clapping state machine, I used that value in my screen change state machine. To transition states, I checked for the number of claps while in my finish state. This ensured that I only changed screen states based on the number of claps after I had finished clapping. As I explained earlier, in the off state, two claps takes me to red, and 3 claps takes me to blue. In the red state, two claps takes me to off, and three to blue. In the blue state, three claps takes me to off, and two to red.
```cpp
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
```

Notes:
While my design functioned correctly, I had more consistent results when tapping the microphone compared to clapping.



[demonstration video](https://www.youtube.com/watch?v=HWrs1Dd-yFU&ab_channel=AbeE)









