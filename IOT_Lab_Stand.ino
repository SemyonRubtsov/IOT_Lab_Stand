//Main file of repository

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include "driver/gpio.h"

const char* ssid = "Redmi";  
const char* password = "sr505505";

hw_timer_t *My_timer = NULL;
hw_timer_t *SW_timer = NULL;
int Step=0;

const int DIR = 12;
const int STEP = 14;
const int LEDD = 4;
const int ENDSW = 5;

int pos;
WebServer server(80);

bool but1State = false;
bool but2State = false;
bool but3State = false;
bool SWOld=false;
bool SWState=false;

void IRAM_ATTR SWTimer(){
  
  //if (SWState and !SWRead){
    SWOld=SWState;
    SWState=digitalRead(ENDSW);

    if (SWOld==0 and SWState==1) gpio_set_level(GPIO_NUM_12, !gpio_get_level(GPIO_NUM_12));
    //but1State = digitalRead(ENDSW);
    //SWState=false;
  //}
  //else if (SWState){
    //digitalWrite(DIR, !digitalRead(DIR));
    //but1State = digitalRead(DIR);
    //Serial.print("--------------TimerEnd--------------");
    //SWState=true;
    //timerStop(SW_timer);
  //}
  //SWRead=true;
  
}

void IRAM_ATTR onTimer(){
  //if (Step>=200) timerStop(My_timer);

  //check state of ENDSW
  


  if (but2State)
  {
    Step++;
    digitalWrite(STEP, !digitalRead(STEP));
  }
  //Serial.println(Step);
}

//void IRAM_ATTR release() {
//  if (SWRead){
//  timerAlarmWrite(SW_timer, 1500, true);
//  timerAlarmEnable(SW_timer);
//  SWRead=false;
//  SWState=false;
//  Serial.println("----------RELEASED-----");
//  }
  
 	//but1State = !but1State;
  //digitalWrite(DIR, but1State);
//}

void IRAM_ATTR press() {
  //if (SWRead){
  timerAlarmWrite(SW_timer, 5000, true);
  timerAlarmEnable(SW_timer);
  //SWRead=false;
  //SWState=false;
  //Serial.println("----------PRESSED-----");
  //}
  
 	//but1State = !but1State;
  //digitalWrite(DIR, but1State);
}

void setup() {
 
  Serial.begin(9600);

  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(LEDD, OUTPUT);

  pinMode(ENDSW,INPUT_PULLUP);
	attachInterrupt(ENDSW, press, RISING);
  //attachInterrupt(ENDSW, release, FALLING);

  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 500, true);
  timerAlarmEnable(My_timer);

  SW_timer = timerBegin(1, 80, true);
  timerAttachInterrupt(SW_timer, &SWTimer, true);
  

  //pinMode(output26, OUTPUT);
  //digitalWrite(output26, LOW);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  Serial.print(ssid);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", sendHtml);

  server.on(UriBraces("/toggle/{}"), []() {
    String led = server.pathArg(0);
    Serial.print("Toggle LED #");
    Serial.println(led);

    switch (led.toInt()) {
      case 1:
        but1State = !but1State;
        digitalWrite(DIR, but1State);
        break;
      case 2:
        but2State = !but2State;
        //digitalWrite(STEP, but2State);
        break;
      case 3:
        but3State = !but3State;
        digitalWrite(LEDD, but3State);
        break;
    }

    sendHtml();
  });

  server.begin();
  Serial.println("HTTP server started");
}



void loop()
{
  Serial.print(SWState);

  //Switch case

  //Serial.print(SWRead);
  Serial.println(digitalRead(DIR));
  server.handleClient();
  delay(1);
}



void sendHtml() {
  String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>ESP32 Web Server Demo</title>
        
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          html { font-family: sans-serif; text-align: center; }
          body { display: inline-flex; flex-direction: column; }
          h1 { margin-bottom: 1.2em; } 
          h2 { margin: 0; }
          div { display: grid; grid-template-columns: 1fr 1fr; grid-template-rows: auto auto; grid-auto-flow: column; grid-gap: 1em; }
          .btn { background-color: #5B5; border: none; color: #fff; padding: 0.5em 1em;
                 font-size: 2em; text-decoration: none }
          .btn.OFF { background-color: #333; }
        </style>
      </head>
            
      <body>
        <h1>ESP32 Web Server</h1>

        <div>
          <h2>DIR</h2>
          <a href="/toggle/1" class="btn LED1_TEXT">LED1_TEXT</a>
          <h2>MOT</h2>
          <a href="/toggle/2" class="btn LED2_TEXT">LED2_TEXT</a>
          <h2>BTN</h2>
          <a href="/toggle/3" class="btn BTN3_TEXT">BTN3_TEXT</a>
        </div>
      </body>
    </html>
  )";
  response.replace("LED1_TEXT", but1State ? "ON" : "OFF");
  response.replace("LED2_TEXT", but2State ? "ON" : "OFF");
  response.replace("BTN3_TEXT", but3State ? "ON" : "OFF");
  server.send(200, "text/html", response);
}
