#include <SPI.h>
#include <TFT_eSPI.h>
#include <math.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h> 
#include <ArduinoJson.h>
Adafruit_BMP280 bmp; // I2C
TFT_eSPI tft = TFT_eSPI();

const char USER[] = "AbeE"; //CHANGE YOUR USER VARIABLE!!!
const char POST_URL[] = "POST http://608dev-2.net/sandbox/sc/aoejile/design/thermostat_request.py HTTP/1.1\r\n"; 
char network[] = "MIT";
char password[] = "";
// char network[] = "EECS_Labs";
// char password[] = "";

const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int POSTING_PERIOD = 6000; //ms to wait between posting step


const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response



const uint8_t OFF = 0;
#define red 1
const uint8_t RED = 1;
const uint8_t BLUE = 2;
#define blue 3
int end_count = 0;
const uint8_t IDLE11 = 12;
const uint8_t IDLE1 = 11;
const uint8_t IDLE = 0;
const uint8_t IDLE2 = 13;
const uint8_t DOWN = 1;
const uint8_t DOWN1 = 14;
const uint8_t UP = 2;
const uint8_t UP1 = 15;
const uint8_t RESET = 3;
const uint8_t RESET1 = 16;
const uint8_t FINISH = 4;
const uint8_t FINISH1 = 17;
const uint8_t DOWN2 = 5;
const uint8_t UP2 = 6;
const uint8_t FINISH2 = 7;
const uint8_t End = 8;
const uint8_t End2 = 9;
const uint8_t End1 = 18;
uint8_t state;
uint8_t state2;
uint8_t state3;
uint8_t state4 = 0;
const uint8_t BUTTON1 = 45;
const uint8_t BUTTON2 = 39;
const uint8_t BUTTON3 = 38;
const uint8_t BUTTON4 = 34;
int HOT = 1;
int COOL = 2;
int POST = 10;
char modechar[50];

int target_tempcount = 0;
int target_tempcount1 = 0;
int reset_counter = 0;
int reset_counter1 = 0;
int num_count;
int num_count1;
int temperature_counter = 0;
int temperature_offset = 0;

int end_thermo = 0;
int count = 0;
int count2 = 0;
int inter;
int inter1;
int mode_count = 0;
const int LOOP_SPEED = 10;
unsigned long primary_timer; //main loop timer
uint32_t posting_timer = 0;
unsigned long on_timer = 0;
unsigned long timer = 0;

float test_room_temp = 0;
int target_temp = 0;
char status1[50] = "";
int mode;
float temperature;
char tft_output[200];






void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //for debugging if needed.
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  state = IDLE1;
  state3 = IDLE11;
  primary_timer = millis();
  Serial.begin(115200); //begin serial comms
  while (!Serial);
  unsigned status;
  status = bmp.begin();
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(), 16);
    esp_restart();
  }
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  Serial.println("bmp280 setup done");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      uint8_t* cc = WiFi.BSSID(i);
      for (int k = 0; k < 6; k++) {
        Serial.print(*cc, HEX);
        if (k != 5) Serial.print(":");
        cc++;
      }
      Serial.println("");
    }
  }
  delay(100); //wait a bit (100 ms)

  //if using regular connection use line below:
  WiFi.begin(network, password);
  //if using channel/mac specification for crowded bands use the following:
  //WiFi.begin(network, password, channel, bssid);


  uint8_t count1 = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count1 < 12) {
    delay(500);
    Serial.print(".");
    count1++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  posting_timer = millis();
  state2 = 0;


}

void loop() {
  char output2[200];
  sprintf(output2, "state: %d, target_temp: %d, inter: %d, reset counter:  %d, target temp count: %d, numcount :  %d, modecount: %d, status:%s ", state, target_temp, inter, reset_counter, target_tempcount, num_count, mode_count, status1);
  Serial.println(output2);
  inter = target(digitalRead(BUTTON1), digitalRead(BUTTON2));
  inter1 = offset(digitalRead(BUTTON3), digitalRead(BUTTON4));
  char output3[200];
  sprintf(output3, "state: %d, target_temp_offset: %d, inter: %d, reset counter:  %d, target temp count: %d, numcount :  %d ", state3, temperature_offset, inter1, reset_counter1, target_tempcount1, num_count1);
  // Serial.println(output3);
  if (reset_counter1 == 1 && count2 == 0){
  count2 = 1;
  temperature_offset += 100* inter1;
  }
  else if (reset_counter1 == 2 && count2 == 1){
    count2 = 2;
    temperature_offset += 10* inter1;
  }
  else if (reset_counter1 == 3 && count2 == 2){
    count2 = 3;
    temperature_offset += inter1;
    char offset1[30];
    sprintf(offset1, "offset temp: %d", temperature_offset);
    tft.println(offset1);
  }

  if (reset_counter == 1 && count == 0){
    count = 1;
    target_temp += 100* inter;
  }
  else if (reset_counter == 2 && count == 1){
    count = 2;
    target_temp += 10* inter;
  }
  else if (reset_counter == 3 && count == 2){
    count = 3;
    target_temp += inter;
    char ttemp[50];
    sprintf(ttemp,"target temp: %d", target_temp );
    if (end_count == 0){
    tft.println(ttemp);
    }
  }
 
  else if (state == End || state3 == End1 && end_count == 0){
    // char output[20];
    // modechar[0] = '\0';
    end_count += 1;
    state3 = IDLE11;
    state = IDLE1;
    state2 = POST;
    if (mode == 1){
      modechar[0] = '\0';
      sprintf(modechar, "heat");
      reset_counter = 0;
      count = 0;
    }
    else if (mode == 2){
      modechar[0] = '\0';
      sprintf(modechar, "heat");
      reset_counter = 0;
      count = 0;
    }
    // sprintf(output, "temp: %d\n mode: %s", target_temp, modechar);
      // Serial.println(output);
  }
   //variable for temperature
  if (millis() - posting_timer > POSTING_PERIOD && state2 == POST) {
    temperature = bmp.readTemperature();
    if (mode == 1){
    test_room_temp = temperature + temperature_offset;  
    }
    else if (mode == 2){
       test_room_temp = temperature - temperature_offset;
    }
    posting_timer = millis();
    Serial.printf("temp: %f ", temperature);
    char body[100]; //for body
    // test_room_temp = temperature;
    sprintf(body, "target_temp=%d&current_temp=%f&mode=%d&test_room_temp=%f", target_temp, temperature, mode,test_room_temp); //generate body, posting temp, humidity to server
    int body_len = strlen(body); //calculate body length (for header reporting)
    sprintf(request_buffer, POST_URL);
    strcat(request_buffer, "Host: 608dev-2.net\r\n");
    strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
    sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
    strcat(request_buffer, "\r\n"); //new line from header to body
    strcat(request_buffer, body); //body
    strcat(request_buffer, "\r\n"); //new line
    Serial.println(request_buffer);
    do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
    Serial.println(response_buffer); //viewable in Serial Terminal
    request_buffer[0] = '\0'; //set 0th byte to null
    response_buffer[0] = '\0';
    int off = 0; //reset offset variable for sprintf-ing
    off += sprintf(request_buffer + off, "GET http://608dev-2.net/sandbox/sc/aoejile/design/thermostat_request.py?haha=why HTTP/1.1\r\n");
    off += sprintf(request_buffer + off, "Host:  608dev-2.net\r\n");
        // offset += sprintf(request + offset, "Content-Type: application/json\r\n");
        // offset += sprintf(request + offset, "cache-control: no-cache\r\n");
        // offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
    off += sprintf(request_buffer + off, "\r\n");
    do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
     DynamicJsonDocument doc(500);
        char* starting1 = strchr(response_buffer, '{');
        char* ending1 = strrchr(response_buffer, '}');
        *(ending1 + 1) = NULL;
        DeserializationError error = deserializeJson(doc, starting1);
        // Test if parsing succeeds.
        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.f_str());
        } else {
           target_temp = doc["target temp"];
           sprintf(modechar, doc["mode"]);
          //  test_room_temp = doc["test_room_temp"];
           strcat(status1, doc["status"]);
          //  Serial.println("status:----------");
          //  Serial.println(status1);
          //  Serial.println("status:----------");
          //  temperature = doc["current temp"];
        }    
    if (strcmp(status1, "ON") == 0){
      if (strcmp(modechar, "heat") ==0){
        Serial.println("hahaha");
        mode = 1;
        led(mode);
      }
      else if (strcmp(modechar, "cool") == 0){
        mode = 2;
        led(mode);
        
      }     
    }
    else{
      digitalWrite(red, LOW);
      digitalWrite(blue, LOW);
    }
    sprintf(tft_output, "current temp:%f \ntarget temp: %d\n test room temp: %f\nmode: %s\nstatus: %s ", temperature, target_temp, test_room_temp, modechar, status1);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0,0,2);
    tft.println(tft_output);
    status1[0] = '\0';
    modechar[0] = '\0';


    
  }

while (millis() - primary_timer < LOOP_SPEED); //wait for primary timer to increment
primary_timer = millis();
}

int target(uint8_t input, uint8_t input2){
  switch(state){
    case IDLE1:
    if (input == 0){
      target_tempcount = 0;
      state = DOWN;
      target_temp = 0;
    }
    case IDLE:
    if (input == 0){
      target_tempcount = 0;
      mode_count = 0;
      state = DOWN;
    }
    break;
    case DOWN:
      // state2 = 0;
      if (input == 1){
        state = UP;
      }
      break;
    case UP:
      if (input == 0){
        target_tempcount += 1;
        state = DOWN;
      }
      if (input2 == 0){
        state = RESET;
      }
      break;
    case RESET:
      if (reset_counter == 3){
        state = FINISH;
        num_count = target_tempcount;
      }
      else{
      reset_counter += 1;
      num_count = target_tempcount;
      state = IDLE;
      }
      break;
    case FINISH:
    Serial.println("Press 1 for hot mode and 2 for cool mode");
    if (input == 0){
      state = DOWN2;
    }
    break;
    case DOWN2:
      if (input == 1){
        state = UP2;
        mode_count += 1;
        if (mode_count == 2){
          state = FINISH2;
        }
      }
    break;
    case UP2:
      if (input == 0){
        state = DOWN2;
      }
      else if (input2 == 0 ){
        state = FINISH2;
      }
      // else if (input2 == 0 && end_thermo > 0){
      //   if (end_thermo < 3){
      //   end_thermo += 1;
      //   state = DOWN2;
      //   }
      //   else{
      //     end_thermo = 0;
      //     state = IDLE;
      //   }

      // }
    break;
    case FINISH2:
    if (mode_count == 1){
        mode = 1;
        state = End;
    }
    else if (mode_count == 2){
      mode = 2;
      state = End;
    }
    break;
    case End:
    if (end_count > 0){
    state = IDLE1;
    reset_counter = 0;
    }
    break;
  }

return num_count;
}

void do_http_request(const char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response,client.read(),OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}       

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

int offset(uint8_t input, uint8_t input2){
  switch(state3){
    case IDLE11:
    if (input == 0){
      target_tempcount1 = 0;
      state3 = DOWN1;
      temperature_offset = 0;
    }
    case IDLE2:
    if (input == 0){
      target_tempcount1 = 0;
      // mode_count = 0;
      state3 = DOWN1;
    }
    break;
    case DOWN1:
      // state2 = 0;
      if (input == 1){
        state3 = UP1;
      }
      break;
    case UP1:
      if (input == 0){
        target_tempcount1 += 1;
        state3 = DOWN1;
      }
      if (input2 == 0){
        state3 = RESET1;
      }
      break;
    case RESET1:
      if (reset_counter1 == 3){
        state3 = End1;
        num_count1 = target_tempcount1;
      }
      else{
      reset_counter1 += 1;
      num_count1 = target_tempcount1;
      state3 = IDLE2;
      }
      break;
    case End1:
    state3 = IDLE11;
    reset_counter1 = 0;
    break;
  }

return num_count1;
}

void led(int input){
  switch(state4){
    case OFF:
    if(input == 1){
      state4 = RED;
       on_timer = millis();
    }
    else if(input == 2){
      state4 = BLUE;
      on_timer = millis();
    }
    break;
    case RED:
    if (input == 2){
      on_timer = millis();
      state4 = BLUE;
    }
    else {
      if (millis() - on_timer < 30000){
        digitalWrite(red, HIGH);
        digitalWrite(blue, LOW);
    }
    else{
        digitalWrite(red, LOW);
        state4 = OFF;
      }
    }
    break;
    case BLUE:
    if (input == 1){
      on_timer = millis();
      state4 = RED;
    }
    else{
      if (millis() - on_timer < 30000){
        digitalWrite(blue, HIGH);
        digitalWrite(red, LOW);
      }
      else{
        digitalWrite(blue, LOW);
        state4 = OFF;
      }
    }
  break;    
  }
}



