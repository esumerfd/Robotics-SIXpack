#include "esp_camera.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems
#include "esp_http_server.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Credential for the Wifi access point
//feel free to change that
const char* ssid = "SIXpackOrig";
const char* password = "aaaaaaaa";
//Instatntiation of communication variables, this was made for Alpha 
//most of them are not used for ESPiderman and can be deleted carefully
bool lightOn=false;
bool lightStrobe=false;
bool strobeOn=false;
bool arm=false;
bool wave=false;
int ch_1=1500;
int ch_2=1500;
int ch_3=1500;
int ch_4=1500;
int ch_5=1000;
int ch_6=1000;
int ch_7=1000;
int ch_7_old=1500;
int ch_8=1000;
int ch_9=0; 
//Instantiation of feedback variables, same as for the communication variables
char dataToClient[70];
String feedBackToClient;
String light="1000";
String armed="1000";
String height="1000";
String mode="1000";
String footFL="1000";
String footFR="1000";
String footBL="1000";
String footBR="1000";
String temp=" ";
String rotX=" ";
String rotY=" ";
String rotZ=" ";
String distFr=" ";
String distFrL=" ";
String distFrR=" ";
String distBott=" ";

//Tell the program which pins shall be used for the I2C communication
#define i2c_sda 1
#define i2c_scl 3

//Instantiation of varables used for ESPiderman walking gaits
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
//a0-a7= target angles, a0r-a7r= current angles
int a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a0r, a1r, a2r, a3r, a4r, a5r, a6r, a7r, a8r, a9r, a10r, a11r; 

//Angle of the outerLegParts
int legIdle = 30; //  for calibaration 0 for usage 30
//value that can be used to manipulate the idle posution after arming
int korr = 25; //0for calibration 25 for usage
//Offset to manipulate the center of gravity for each height


//Compensation values to calibrate the servos
int comp0=5;
int comp1=0;
int comp2=-3;
int comp3=-5;
int comp4=-8;
int comp5=-10;
int comp6=-8;
int comp7=-5;
int comp8=0;
int comp9=0;
int comp10=0;
int comp11=0;
//negative fullscale and positive fullscale,  this is used to tell the PCA9685 at which pulse 
//a servo is set to 0° and at which to 180°, maybe you need to change the values if other servosw
//are used
int pos0 = 150;
int pos180 = 650;
//Delay to decrease the Speed. This is in milli seconds so almost zero, if you want to speed up the gaits
//you have to do it at the ramped controller at the end
int v = 3;
int vFast = 3;
//Different speed for locomotion
int vSlow = 30;
//degrees that are set in each iteration of the controller
int t = 4;
//deadband that is drawn around stall position of servo to avoid jittering servos
int eps = t+3; 

//Delay for liftLeg() and lowerLeg() functions, is used to wait until the movement is done
int y = 75;
//counter for steps in each movement sequenz
int step = 0;

//height and width of a step, this is set for every heigt seperate
int stepLift = 0;
int stepLiftLow = 35;
int stepLiftMid = 40;
int stepLiftHigh = 45;

int stepLength = 0;
int stepLengthLow = 40; 
int stepLengthMid = 40; 
int stepLengthHigh = 40;    


//Section to define different cameramodels and chip types
#define PART_BOUNDARY "123456789000000000000987654321"
#define LED_BUILTIN 4
#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM_B
//#define CAMERA_MODEL_WROVER_KIT

#if defined(CAMERA_MODEL_WROVER_KIT)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    21
  #define SIOD_GPIO_NUM    26
  #define SIOC_GPIO_NUM    27
  
  #define Y9_GPIO_NUM      35
  #define Y8_GPIO_NUM      34
  #define Y7_GPIO_NUM      39
  #define Y6_GPIO_NUM      36
  #define Y5_GPIO_NUM      19
  #define Y4_GPIO_NUM      18
  #define Y3_GPIO_NUM       5
  #define Y2_GPIO_NUM       4
  #define VSYNC_GPIO_NUM   25
  #define HREF_GPIO_NUM    23
  #define PCLK_GPIO_NUM    22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_M5STACK_WITHOUT_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       17
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM_B)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     22
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#else
  #error "Camera model not selected"
#endif
//instatiation of heandler constants, neede for the communication between server and client
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

//here the HTML code of the webserver starts, if you want to change something for the webserve 
//you can make it here. I personally copy paste it to VSCode with HTML addon to see the change in realtime
//don't forget to copy paste it back here afterwards ;D
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<html>
  <head>
    <title>delta3Robotics</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      :root{
        --back: #858080;
        --backLog: darkgrey;
        --activeData: purple;
        --font: black;
        --BtnBackground: gray;
        --btnBorder: black;
        --BtnBackgroundDis: rgb(87, 86, 86);
        --BtnLegAct: purple; 
      }
      body { 
        font-family: Arial; 
        text-align: center; 
        background-color: var(--back);
        position: absolute;
        margin: 0px;
        overflow: hidden;
        top: 0px;
        bottom: 0px;
        width: 100%;
        height: 100%;
    }
      table { 
        margin-left: 0%; 
        margin-right: 0px; 
    }

      td { 
        padding:  1px; 
        align-items:"center";
    }

    .log {
        color: var(--activeData);
        font-size: 13px;
        text-align: left;
        margin-left: 5px;
    }
    .var {
        color: var(--font);
        font-size: 13px;
        text-align: left;
        margin-left: 5px;
    }

    .height{
      color: var(--activeData);
      font-size: 17px;
      text-align: left;
      margin-left: 5px;
    }

      .button {
        color: var(--font);
        background-color: var(--BtnBackground);
        border: solid;
        border-color: var(--btnBorder);
        border-radius: 100%;
        width: 60px;
        height: 60px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 18px;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }
      .button:active {
        border-color: var(--activeData);
      }

      .buttonDisabled {
        color: black;
        background-color: var(--back);
        border: none;
        border-radius: 100%;
        width: 60px;
        height: 60px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 18px;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }
      .buttondisabled:active {
        background-color: var(--back);
      }

      .buttonLeg {
        color: var(--font);
        background-color: var(--BtnLegAct);
        border: solid;
        border-radius: 100%;
        width: 40px;
        height: 40px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 18px;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }
      .buttonLeg:active {
        background-color: var(--activeData);
      }

    .containerA {
        display: grid;
        width: 100%;
        height: 100%;
        grid-template-columns: [col0] 20% [col1] 60% [col2] 20% [col3];
        grid-template-rows: [row0] 10% [row1] 20% [row2] 35% [row3] 30% [row4]; 
        column-gap: 5px;
        row-gap: 5px;   
        justify-self: start;
        align-self: start;
        justify-items: center;
        align-items: center;
    }

    .item-info1 {
        padding-left: 10px;
        grid-column-start: col0;
        grid-column-end: col1;
        grid-row-start: row0;
        grid-row-end: row2;
        justify-self: stretch;
        align-self: stretch;
        line-height: 40%;
        justify-content: left;
    }

    .item-info2 {
        grid-column-start: col0;
        grid-column-end: col1;
        grid-row-start: row2;
        grid-row-end: row3;
        align-self: stretch;
        justify-self: stretch;
        line-height: 40%;
        align-items: center;
        justify-items: center;
    }  
    
    .item-bt1 {
        grid-column-start: col0;
        grid-column-end: col1;
        grid-row-start: row3;
        grid-row-end: row4;
    }

    .item-heading {
        grid-column-start: col1;
        grid-column-end: col2;
        grid-row-start: row0;
        grid-row-end: row1;
        justify-self: center;
        align-self: center;
    }

    .item-stream {
        grid-column-start: col1;
        grid-column-end: col2;
        grid-row-start: row1;
        grid-row-end: row4;
        justify-self: stretch;
        align-self: stretch;
        
    }
    .item-bt3 {
        grid-column-start: col2;
        grid-column-end: col3;
        grid-row-start: row0;
        grid-row-end: row2;

    }
    .item-info3 {
        grid-column-start: col2;
        grid-column-end: col3;
        grid-row-start: row2;
        grid-row-end: row3;

    }
    .item-bt2 {
        grid-column-start: col2;
        grid-column-end: col3;
        grid-row-start: row3;
        grid-row-end: row4;
    }

    .containerB {
        display: grid;
        width: 100%;
        height: 100%;
        grid-template-columns: [col0] 20% [col1] 60% [col2] 20% [col3];
        grid-template-rows: [row0] 10% [row1] 20% [row2] 35% [row3] 30% [row4]; 
        column-gap: 5px;
        row-gap: 5px;   
        justify-self: start;
        align-self: start;
        justify-items: center;
        align-items: center;
    }
    .item-FL {
        grid-column-start: col0;
        grid-column-end: col1;
        grid-row-start: row0;
        grid-row-end: row2;
    }
    .item-FR {
        grid-column-start: col2;
        grid-column-end: col3;
        grid-row-start: row0;
        grid-row-end: row2;
    }
    .item-Height {
        grid-column-start: col1;
        grid-column-end: col2;
        grid-row-start: row1;
        grid-row-end: row4;
        justify-self: center;
        align-self: center;  
    }
    .item-BL {
        grid-column-start: col0;
        grid-column-end: col1;
        grid-row-start: row3;
        grid-row-end: row4;
    }
    .item-BR {
        grid-column-start: col2;
        grid-column-end: col3;
        grid-row-start: row3;
        grid-row-end: row4;
    }
    img {  
      width: 100%;
      align-self: stretch;
    }


    </style>
  </head>
  <body>
    <div class="containerA">
        <div class="item-info1">
          <div><h1 class=log id="Row11" ></h1></div>
          <div><h1 class=log id="Row10" ></h1></div>
          <div><h1 class=log id="Row9" ></h1></div>
          <div><h1 class=log id="Row8" ></h1></div>
          <div><h1 class=log id="Row7" ></h1></div>
          <div><h1 class=log id="Row6" ></h1></div>
          <div><h1 class=log id="Row5" ></h1></div>
          <div><h1 class=log id="Row4" ></h1></div>
          <div><h1 class=log id="Row3" ></h1></div>
          <div><h1 class=log id="Row2" ></h1></div>
          <div><h1 class=log id="Row1" ></h1></div>
          <div><h1 class=log id="Row0" ></h1></div>
        </div>
        <div calss="item-info2">
          <table>
            <tr>
              <td><h1 class="var" ></h1></td><td><h1 class="log" id="logTemp"></h1></td><td><h1 class="var" ></h1></td>
            </tr>
            <tr>
              <td><h1 class="var" ></h1></td><td><h1 class="log" id="logRotX"></h1></td><td><h1 class="var"></h1></td>
            </tr>
            <tr>
              <td><h1 class="var" ></h1></td><td><h1 class="log" id="logRotY"></h1></td><td><h1 class="var"></h1></td>
            </tr>
            <tr>
              <td><h1 class="var" ></h1></td><td><h1 class="log" id="logRotZ"></h1></td><td><h1 class="var"></h1></td>
            </tr>
            <tr>
              <td><h1 class="var" ></h1></td><td><h1 class="log" id="logDistFr"></h1></td><td><h1 class="var"></h1></td>
            </tr>
            <tr>
              <td><h1 class="var" ></h1></td><td><h1 class="log" id="logDistFrL"></h1></td><td><h1 class="var"></h1></td>
            </tr>
            <tr>
              <td><h1 class="var" ></h1></td><td><h1 class="log" id="logDistFrR"></h1></td><td><h1 class="var"></h1></td>
            </tr>
            <tr>
              <td><h1 class="var" ></h1></td><td><h1 class="log" id="logDistBot"></h1></td><td><h1 class="var"></h1></td>
            </tr>
          </table>
        </div>
        <div class="item-bt1">
            <table>
                <tr>
                  <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('A');" ontouchend="toggleCheckbox('d');"></button></td>
                  <td><button class="button" ontouchstart="toggleCheckbox('W');" ontouchend="toggleCheckbox('w');">W</button></td>
                  <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('D');" ontouchend="toggleCheckbox('d');"></button></td></tr>
                </tr>
                <tr>
                  <td><button class="button" ontouchstart="toggleCheckbox('TL');" ontouchend="toggleCheckbox('tl');">TL</button></td>
                  <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('');" ontouchend="toggleCheckbox('');" style="background-color: gray;"></button></td>
                  <td><button class="button" ontouchstart="toggleCheckbox('TR');" ontouchend="toggleCheckbox('tl');">TR</button></td>
                <tr>
                  <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('');" ontouchend="toggleCheckbox('');"></button></td>
                  <td><button class="button" ontouchstart="toggleCheckbox('S');" ontouchend="toggleCheckbox('w');">S</button></td>
                  <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('');" ontouchend="toggleCheckbox('');"></button></td>
                </tr>                   
            </table>
        </div>
        <div class="item-heading" style="font-size: 60px" >delta3Robotics</div>
        <div class="item-stream" >
            <img src="" id="photo" >
        </div>
        <div class="item-bt3">
            <table>    
                <tr>
                  <td><button id="wave" class="button" ontouchstart="toggleCheckbox('WAVE');" ontouchend="toggleCheckbox('wave');" >wave</button></td>
                  <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('');" ontouchend="toggleCheckbox('');"></button></td>
                  <td><button id="ARM" class="button" onclick="toggleCheckbox('ARM');" >ARM</button></td>
                </tr>
                <tr>
                  <td><button id="LIGHT" class="button" ontouchstart="toggleCheckbox('LIGHT'); toggleLIGHT();" style="font-size: 15px;" >LIGHT</button></td>
                  <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('');" ontouchend="toggleCheckbox('');"></button></td>
                  <td><button id="HEIGHT" class="button" ontouchstart="toggleCheckbox('HEIGHT');" style="font-size: 11px;" >HEIGHT</button></td>
                </tr>
                <tr>
                  <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('HORN');" ontouchend="toggleCheckbox('horn');" style="font-size: 15px;"></button></td>
                  <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('');" ontouchend="toggleCheckbox('');"></button></td>
                  <td><button id="MODE" class="buttonDisabled" ontouchstart="toggleCheckbox('MODE');"style="font-size: 13px;" ></button></td>
                </tr>
            </table>
        </div>
        <div class="item-info3">
          <div class="containerB">
            <div class="item-FL">
              <button class="buttonDisabled" id="FrontLeft"></button>
            </div>
            <div class="item-FR">
              <button class="buttonDisabled" id="FrontRight"></button>
            </div>
            <div class="item-Height">
              <h1 class="height" id="height" >Height 1</h1>
            </div>
            <div class="item-BL">
              <button class="buttonDisabled" id="BackLeft"></button>
            </div>
            <div class="item-BR">
              <button class="buttonDisabled" id="BackRight"></button>
            </div>
          </div>
        </div>
        <div class="item-bt2">
          <table>    
            <tr>
              <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('');" ontouchend="toggleCheckbox('');"></button></td>
              <td><button class="button" ontouchstart="toggleCheckbox('U');" ontouchend="toggleCheckbox('u');">U</button></td>
              <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('');" ontouchend="toggleCheckbox('');"></button></td>
            </tr>
            <tr>
              <td><button class="button" ontouchstart="toggleCheckbox('L');" ontouchend="toggleCheckbox('r');">L</button></td>
              <td><button class="button" ontouchstart="toggleCheckbox('RSTR');">RST</button></td>
              <td><button class="button" ontouchstart="toggleCheckbox('R');" ontouchend="toggleCheckbox('r');">R</button></td>
            </tr>
            <tr>
              <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('');" ontouchend="toggleCheckbox('');"></button></td>
              <td><button class="button" ontouchstart="toggleCheckbox('DO');" ontouchend="toggleCheckbox('u');">D</button></td>
              <td><button class="buttonDisabled" ontouchstart="toggleCheckbox('');" ontouchend="toggleCheckbox('');"></button></td>
            </tr>
        </table>
        </div>
    </div>

   <script>
    
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/action?go=" + x, true);
      xhr.onload = function() {
        if (xhr.status == 200 && xhr.responseText !== "") {
          var Row0 = document.getElementById("Row0");
          var Row1 = document.getElementById("Row1");
          var Row2 = document.getElementById("Row2");
          var Row3 = document.getElementById("Row3");
          var Row4 = document.getElementById("Row4");
          var Row5 = document.getElementById("Row5");
          var Row6 = document.getElementById("Row6");
          var Row7 = document.getElementById("Row7");
          var Row8 = document.getElementById("Row8");
          var Row9 = document.getElementById("Row9");
          var Row10 = document.getElementById("Row10");
          var Row11 = document.getElementById("Row11");
          Row11.innerText = Row10.innerText;
          Row10.innerText = Row9.innerText;
          Row9.innerText = Row8.innerText;
          Row8.innerText = Row7.innerText;
          Row7.innerText = Row6.innerText;
          Row6.innerText = Row5.innerText;
          Row5.innerText = Row4.innerText;
          Row4.innerText = Row3.innerText;
          Row3.innerText = Row2.innerText;
          Row2.innerText = Row1.innerText;
          Row1.innerText = Row0.innerText;
          Row0.innerText = xhr.responseText;
        } 
      };
     xhr.send();
   }


let timer = setInterval(getFeedBack, 200);
function getFeedBack() {
  var xhfr = new XMLHttpRequest();
    xhfr.open("GET", "/action?go=" + "feedBack", true);
      xhfr.onload = function(){
        if (xhfr.status == 200){
          var text = xhfr.responseText;
          var textarray = text.split(",");
          var lightBtn = document.getElementById("LIGHT");
            if(textarray[0]=="1000"){
              lightBtn.style.backgroundColor= "gray";
            }
            if(textarray[0]=="1500"){
              lightBtn.style.backgroundColor= "purple";
            }
            if(textarray[0]=="2000"){
              lightBtn.style.backgroundColor= "green";
            }
          var armedBtn = document.getElementById("ARM");
            if(textarray[1]=="1000"){
              armedBtn.style.backgroundColor= "gray";
            }
            if(textarray[1]=="2000"){
              armedBtn.style.backgroundColor= "purple";
            }
          var heightLog = document.getElementById("height");
          var heightBtn = document.getElementById("HEIGHT");
            if(textarray[2]=="1000"){
              heightLog.innerText = "Height 1";
              heightBtn.style.backgroundColor= "gray";
            }
            if(textarray[2]=="1500"){
              heightLog.innerText = "Height 2";
              heightBtn.style.backgroundColor= "purple";
            }
            if(textarray[2]=="2000"){
              heightLog.innerText = "Height 3";
              heightBtn.style.backgroundColor= "green";
            }
          var modeBtn = document.getElementById("MODE");
            if(textarray[3]=="1000"){
              modeBtn.style.backgroundColor= "gray";
            }
            if(textarray[3]=="1500"){
              modeBtn.style.backgroundColor= "purple";
            }
            if(textarray[3]=="2000"){
              modeBtn.style.backgroundColor= "green";
            }
          var legFLBtn = document.getElementById("FrontLeft");
            if(textarray[4]=="1000"){
              legFLBtn.style.backgroundColor= "gray";
            }
            if(textarray[4]=="2000"){
              legFLBtn.style.backgroundColor= "purple";
            }
          var legFRBtn = document.getElementById("FrontRight");
            if(textarray[5]=="1000"){
              legFRBtn.style.backgroundColor= "gray";
            }
            if(textarray[5]=="2000"){
              legFRBtn.style.backgroundColor= "purple";
            }
          var legBLBtn = document.getElementById("BackLeft");
            if(textarray[6]=="1000"){
              legBLBtn.style.backgroundColor= "gray";
            }
            if(textarray[6]=="2000"){
              legBLBtn.style.backgroundColor= "purple";
            }
          var legBRBtn = document.getElementById("BackRight");
            if(textarray[7]=="1000"){
              legBRBtn.style.backgroundColor= "gray";
            }
            if(textarray[7]=="2000"){
              legBRBtn.style.backgroundColor= "purple";
            }
          var logTempTxt = document.getElementById("logTemp");
            logTempTxt.innerText=textarray[8];
          var logRotXTxt = document.getElementById("logRotX");
            logRotXTxt.innerText=textarray[9];
          var logRotYTxt = document.getElementById("logRotY");
            logRotYTxt.innerText=textarray[10];
          var logRotZTxt = document.getElementById("logRotZ");
            logRotZTxt.innerText=textarray[11];
          var logDistFrTxt = document.getElementById("logDistFr");
            logDistFrTxt.innerText=textarray[12];
          var logDistFrLTxt = document.getElementById("logDistFrL");
            logDistFrLTxt.innerText=textarray[13];
          var logDistFrRTxt = document.getElementById("logDistFrR");
            logDistFrRTxt.innerText=textarray[14];
          var logDistBotTxt = document.getElementById("logDistBot");
            logDistBotTxt.innerText=textarray[15];       
        }
    };
    xhfr.send();
}
window.onload = document.getElementById("photo").src = window.location.href.slice(0, -1) + ":81/stream";
  </script>
  </body>
</html>
)rawliteral";
//Instatiation of different HTTP hander
static esp_err_t index_handler(httpd_req_t *req){
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

static esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK){
    return res;
  }

  while(true){
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if(fb->width > 400){
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if(res != ESP_OK){
      break;
    }
    //Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
  }
  return res;
}

static esp_err_t cmd_handler(httpd_req_t *req){
  char*  buf;
  size_t buf_len;
  char variable[70] = {0,};
  
  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if(!buf){
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      if (httpd_query_key_value(buf, "go", variable, sizeof(variable)) == ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
      }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  sensor_t * s = esp_camera_sensor_get();
  //flip the camera vertically
  //s->set_vflip(s, 1);          // 0 = disable , 1 = enable
  // mirror effect
  //s->set_hmirror(s, 1);          // 0 = disable , 1 = enable

  int res = 0;

//the following parts are used to process the incomming data and pack the communication
//which is send back to the client. variable is a char which is different for all Buttons 
//on client side. It is send at the end of the URL. 
//feedBack #############################
if(!strcmp(variable,"feedBack")){
  strcpy(variable,dataToClient);
}
//LIGHT#################################
if(!strcmp(variable,"LIGHT")){
  if(lightOn==false){
  lightOn=true;
  strcpy(variable,"Light switched on");
  light="1500";
  }
}
if(!strcmp(variable,"LIGHT")){
  if(lightOn==true){
  lightOn=false;
  strcpy(variable,"Light switched off");
  light="1000";
  }
}
//###################################################
//Channel 1 W/S #####################################
if(!strcmp(variable,"W")){
  ch_1=2000;
  strcpy(variable,"");
}
if(!strcmp(variable,"w")){
  ch_1=1500;
  strcpy(variable,"");
}
if(!strcmp(variable,"S")){
  ch_1=1000;
  strcpy(variable,"");
}
//###################################################
//Channel 2 L/R #####################################
if(!strcmp(variable,"R")){
  if(ch_2<=1724){
    ch_2=ch_2+25;
  }
  else{
    ch_2=1749;
  }
strcpy(variable,"");
}
if(!strcmp(variable,"L")){
  if(ch_2>=1276){
    ch_2=ch_2-25;
  }
  else{
    ch_2=1251;
  }
strcpy(variable,"");
}
if(!strcmp(variable,"TL")){
  ch_2=1000;
  strcpy(variable,"");
}
if(!strcmp(variable,"TR")){
  ch_2=2000;
  strcpy(variable,"");
}
if(!strcmp(variable,"tl")){
  ch_2=1500;
  strcpy(variable,"");
}
if(!strcmp(variable,"r")){
  strcpy(variable,"");
}
//###################################################
//Channel 3 U/D #####################################
if(!strcmp(variable,"U")){
  if(ch_3<=1998){
    ch_3=ch_3+50;
  }
  else{
    ch_3=2000;
  }
strcpy(variable,"");
}
if(!strcmp(variable,"DO")){
  if(ch_3>=1002){
    ch_3=ch_3-50;
  }
  else{
    ch_3=1000;
  }
  strcpy(variable,"");
}
if(!strcmp(variable,"u")){
  strcpy(variable,"");
}
//###################################################
//Reset for Ch_2 and Ch_3 ###########################
if(!strcmp(variable,"RSTR")){
  ch_2=1500;
  ch_3=1500;
  strcpy(variable,"Reset View");
}
//###################################################

//Channel 4 A/D #####################################
if(!strcmp(variable,"D")){
  ch_4=2000;
  strcpy(variable,"");
}
if(!strcmp(variable,"d")){
  ch_4=1500;
  strcpy(variable,"");
}
if(!strcmp(variable,"A")){
  ch_4=1000;
  strcpy(variable,"");
}
//###################################################
//Channel 5 ARM #####################################
if(!strcmp(variable,"ARM")){
  if(arm==true){
    ch_5=1000;
    strcpy(variable,"Robot disarmed");
    arm=false;
    armed="1000";
  }
}
if(!strcmp(variable,"ARM")){
  if(arm==false){
    ch_5=2000;
    strcpy(variable,"Robot armed");
    arm=true;
    armed="2000";
  }
}
//###################################################
//Channel 6 HORN ####################################
if(!strcmp(variable,"HORN")){
  ch_6=2000;
  strcpy(variable,"");
}
if(!strcmp(variable,"horn")){
  ch_6=1000;
  strcpy(variable,"");
}
//###################################################
//Channel 7 HEIGHT ##################################
//transition from HEIGHT1 to HEIGHT2
if(!strcmp(variable,"HEIGHT")){ 
  if(ch_7_old==1500 && ch_7==1000){
    ch_7=1500;
    ch_7_old=1000;
  strcpy(variable,"Changed from HEIGHT1 to HEIGHT2");
  height="1500";
  }
}
//transition from HEIGHT2 to HEIGHT3
if(!strcmp(variable,"HEIGHT")){ 
  if(ch_7_old==1000 && ch_7==1500){
    ch_7=2000;
    ch_7_old=1500;
  strcpy(variable,"Changed from HEIGHT2 to HEIGHT3");
  height="2000";
  }
}
//transition from HEIGHT3 to HEIGHT2
if(!strcmp(variable,"HEIGHT")){ 
  if(ch_7_old==1500 && ch_7==2000){
    ch_7=1500;
    ch_7_old=2000;
  strcpy(variable,"Changed from HEIGHT3 to HEIGHT2");
  height="1500";
  }
}
//transition from HEIGHT2 to HEIGHT1
if(!strcmp(variable,"HEIGHT")){ 
  if(ch_7_old==2000 && ch_7==1500){
    ch_7=1000;
    ch_7_old=1500;
  strcpy(variable,"Changed from HEIGHT2 to HEIGHT1");
  height="1000";
  }
}
//###################################################
//Channel 8 MODE ####################################
if(!strcmp(variable, "MODE")) {
  ch_8=ch_8+500;
  if(ch_8>=2001){
    ch_8=1000;
  }
  if (ch_8==1000){
   strcpy(variable,"MODE set to MANUAL");
   mode="1000";
  }
  if (ch_8==1500){
   strcpy(variable,"MODE set to DANCE");
   mode="1500";
  }
  if (ch_8==2000){
   strcpy(variable,"MODE set to AUTONOMOUS");
   mode="2000";
  }
}
//###################################################
//Channel 9 EARS ####################################
if(!strcmp(variable, "WAVE")) {
  wave=true;
  strcpy(variable,"Waving");
}
if(!strcmp(variable, "wave")) {
  wave=false;
  strcpy(variable,"");
}
//###################################################

  if(res){
    return httpd_resp_send_500(req);
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_sendstr(req, variable);
}

void startCameraServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t cmd_uri = {
    .uri       = "/action",
    .method    = HTTP_GET,
    .handler   = cmd_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &cmd_uri);
  }
  config.server_port += 1;
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

void setup() {
  //start the I2C connection to communicate with the PCA-board
  Wire.begin(i2c_sda,i2c_scl);
  pwm.begin();
  pwm.setPWMFreq(1600); 
  pwm.setPWMFreq(60);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  //make the built-in LED usable
  pinMode (LED_BUILTIN, OUTPUT);
  //The serial connection can be used to debug things by printing variables 
  //to the serial monitor, with the current setup you must comment out 
  //everything that is related to I2C and pwm, since the rx and tx pins are 
  //defined as the I2C pins and the I2C communication breaks the serial communication
  //currently I am looking for a way to bypass the I2C pins to other ones so we don't 
  //have to do this commenting-out-procedure
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  // Wi-Fi connection
  WiFi.softAP(ssid, password);
  
  // Start streaming web server
  startCameraServer();

}

void loop() {
  //Start and stop the LED
  if(lightOn==true){
    digitalWrite(LED_BUILTIN, HIGH);
  } else{
    digitalWrite(LED_BUILTIN, LOW);
  }

//here the movment skript starts
//arming = activate
  if(arm==true){
    //select height
    if(height.equals("1000") && ch_1==1500 && ch_2==1500 && wave==false){
      stepLength = stepLengthLow; 
      stepLift = stepLiftLow;
      a1=a3=40+map(ch_3, 1000, 2000, -25, 25);
      a5=a7=40;
      a9=a11=40-map(ch_3, 1000, 2000, -25, 25);
    }
      if(height.equals("1500") && ch_1==1500 && ch_2==1500 && wave==false){
      stepLength = stepLengthMid;
      stepLift = stepLiftMid;
      a1=a3=60+map(ch_3, 1000, 2000, -25, 25);
      a5=a7=60;
      a9=a11=60-map(ch_3, 1000, 2000, -25, 25);
    }
      if(height.equals("2000") && ch_1==1500 && ch_2==1500 && wave==false){
      stepLength = stepLengthHigh;
      stepLift = stepLiftHigh;
      a1=a3=90;
      if(a11<=90){
        a5=a7=90-(0.5*map(ch_3, 1000, 2000, -40, 40));
        a9=a11=90-(0.5*map(ch_3, 1000, 2000, -40, 40));
      }
    }
//stand if not actuated
    if(ch_1==1500 && ch_2!=2000 && ch_2!=1000 && wave==false){
      v=vFast;
      a0=map(ch_2, 1000, 2000, -85, 85);
      a2=-map(ch_2, 1000, 2000, -85, 85);
      a4=map(ch_2, 1000, 2000, -85, 85);
      a6=-map(ch_2, 1000, 2000, -85, 85);
      a8=map(ch_2, 1000, 2000, -85, 85);
      a10=-map(ch_2, 1000, 2000, -85, 85);
      step=0;

    }
//wave
    if(wave==true){
      a3=-85;
      if(a2r==stepLength && a3r==-85){
        a2=0;
      }
      if(a2r==0 && a3r==-85){
        a2=stepLength;
      }
    }
//Walk forward
    if(ch_1>=1600){
      ch_2=1500;
      ch_3=1500;
      if(a0r==0 && a2r==0 && a4r==0 && a6r==0 && a8r==0 && a10r==0 && step==0){
        liftLeg(1);
        liftLeg(7);
        liftLeg(9);
        a0=stepLength;
        a2=0; 
        a4=0; 
        a6=stepLength; 
        a8=stepLength; 
        a10=0; 
        step=1;
      }
      if(a0r==stepLength && a2r==0 && a4r==0 && a6r==stepLength && a8r==stepLength && a10r==0){
        lowerLeg(1);
        lowerLeg(7);
        lowerLeg(9);
        v=vSlow;
        a0=0;
        a2=-stepLength; 
        a4=-stepLength; 
        a6=0; 
        a8=0; 
        a10=-stepLength; 
        step=2;
      }
      if(a0r==0 && a2r==-stepLength && a4r==-stepLength && a6r==0 && a8r==0 && a10r==-stepLength){
        v=vFast;
        liftLeg(3);
        liftLeg(5);
        liftLeg(11);
        a0=0;
        a2=stepLength; 
        a4=stepLength; 
        a6=0; 
        a8=0; 
        a10=stepLength; 
        step=3;
      }
      if(a0r==0 && a2r==stepLength && a4r==stepLength && a6r==0 && a8r==0 && a10r==stepLength){
        lowerLeg(3);
        lowerLeg(5);
        lowerLeg(11);
        v=vSlow;
        a0=-stepLength;
        a2=0; 
        a4=0; 
        a6=-stepLength; 
        a8=-stepLength; 
        a10=0; 
        step=4;
      }
      if(a0r==-stepLength && a2r==0 && a4r==0 && a6r==-stepLength && a8r==-stepLength && a10r==0){
        v=vFast;
        liftLeg(1);
        liftLeg(7);
        liftLeg(9);
        a0=stepLength;
        a2=0; 
        a4=0; 
        a6=stepLength; 
        a8=stepLength; 
        a10=0; 
        step=5;
      }
    }
//Walk backwards
    if(ch_1<=1400){
      ch_2=1500;
      ch_3=1500;
      if(a0r==0 && a2r==0 && a4r==0 && a6r==0 && a8r==0 && a10r==0 && step==0){
        liftLeg(1);
        liftLeg(7);
        liftLeg(9);
        a0=-stepLength;
        a2=0; 
        a4=0; 
        a6=-stepLength; 
        a8=-stepLength; 
        a10=0; 
        step=1;
      }
      if(a0r==-stepLength && a2r==0 && a4r==0 && a6r==-stepLength && a8r==-stepLength && a10r==0){
        lowerLeg(1);
        lowerLeg(7);
        lowerLeg(9);
        v=vSlow;
        a0=0;
        a2=stepLength; 
        a4=stepLength; 
        a6=0; 
        a8=0; 
        a10=stepLength; 
        step=2;
      }
      if(a0r==0 && a2r==stepLength && a4r==stepLength && a6r==0 && a8r==0 && a10r==stepLength){
        v=vFast;
        liftLeg(3);
        liftLeg(5);
        liftLeg(11);
        a0=0;
        a2=-stepLength; 
        a4=-stepLength; 
        a6=0; 
        a8=0; 
        a10=-stepLength; 
        step=3;
      }
      if(a0r==0 && a2r==-stepLength && a4r==-stepLength && a6r==0 && a8r==0 && a10r==-stepLength){
        lowerLeg(3);
        lowerLeg(5);
        lowerLeg(11);
        v=vSlow;
        a0=stepLength;
        a2=0; 
        a4=0; 
        a6=stepLength; 
        a8=stepLength; 
        a10=0; 
        step=4;
      }
      if(a0r==stepLength && a2r==0 && a4r==0 && a6r==stepLength && a8r==stepLength && a10r==0){
        v=vFast;
        liftLeg(1);
        liftLeg(7);
        liftLeg(9);
        a0=-stepLength;
        a2=0; 
        a4=0; 
        a6=-stepLength; 
        a8=-stepLength; 
        a10=0; 
        step=5;
      }
    }

//Turn Left
    if(ch_2<=1400){
      if(a0r==0 && a2r==0 && a4r==0 && a6r==0 && a8r==0 && a10r==0 && step==0){
        a0=-stepLength;
        a2=stepLength; 
        a4=-stepLength; 
        a6=stepLength; 
        a8=-stepLength;
        a10=stepLength;
        step=1;
      }
      if(a0r==-stepLength && a2r==stepLength && a4r==-stepLength && a6r==stepLength && a8r==-stepLength && a10r==stepLength){
        liftLeg(1);
        liftLeg(7);
        liftLeg(9);
        a0=0;
        a2=stepLength; 
        a4=-stepLength; 
        a6=0; 
        a8=0;
        a10=stepLength;
        step=2;
      }
      if(a0r==0 && a2r==stepLength && a4r==-stepLength && a6r==0 && a8r==0 && a10r==stepLength){
        lowerLeg(1);
        lowerLeg(7);
        lowerLeg(9);
        step=3;
      }
      if(a0r==0 && a2r==stepLength && a4r==-stepLength && a6r==0 && a8r==0 && a10r==stepLength && step==3){
        liftLeg(3);
        liftLeg(5);
        liftLeg(11);
        a0=0;
        a2=0; 
        a4=0; 
        a6=0; 
        a8=0;
        a10=0;
        step=4;
      }
      if(a0r==0 && a2r==0 && a4r==0 && a6r==0 && a8r==0 && a10r==0 && step==4){
        lowerLeg(3);
        lowerLeg(5);
        lowerLeg(11);
        step=0;
      }
    }
//Turn Right
    if(ch_2>=1600){
      if(a0r==0 && a2r==0 && a4r==0 && a6r==0 && a8r==0 && a10r==0 && step==0){
        a0=stepLength;
        a2=-stepLength; 
        a4=stepLength; 
        a6=-stepLength; 
        a8=stepLength;
        a10=-stepLength;
        step=1;
      }
      if(a0r==stepLength && a2r==-stepLength && a4r==stepLength && a6r==-stepLength && a8r==stepLength && a10r==-stepLength){
        liftLeg(1);
        liftLeg(7);
        liftLeg(9);
        a0=0;
        a2=-stepLength; 
        a4=stepLength; 
        a6=0; 
        a8=0;
        a10=-stepLength;
        step=2;
      }
      if(a0r==0 && a2r==-stepLength && a4r==stepLength && a6r==0 && a8r==0 && a10r==-stepLength){
        lowerLeg(1);
        lowerLeg(7);
        lowerLeg(9);
        step=3;
      }
      if(a0r==0 && a2r==-stepLength && a4r==stepLength && a6r==0 && a8r==0 && a10r==-stepLength && step==3){
        liftLeg(3);
        liftLeg(5);
        liftLeg(11);
        a0=0;
        a2=0; 
        a4=0; 
        a6=0; 
        a8=0;
        a10=0;
        step=4;
      }
      if(a0r==0 && a2r==0 && a4r==0 && a6r==0 && a8r==0 && a10r==0 && step==4){
        lowerLeg(3);
        lowerLeg(5);
        lowerLeg(11);
        step=0;
      }
    }
  }
//Pose for disarmed
  else{
  a0=a2=a4=a6=a8=a10=0;
  a1=a3=a5=a7=a9=a11=legIdle;
  }

  //pack all the data into one very long char to send it back to the client
  feedBackToClient= light + "," + armed + "," + height + "," + mode + "," + footFL + "," + footFR + "," + footBL + "," + footBR + "," + temp + "," + rotX + "," + rotY + "," + rotZ + "," + distFr + "," + distFrL + "," + distFrL + "," + distBott;
  feedBackToClient.toCharArray (dataToClient, 70);
  //Serial.println (dataToClient);


//Simple ramped controller to set the speed of the servos, control speed via delay (int v) and eps is used to implement deadband and avoid jittering 
if(a0r<=a0-eps){
  setServo(0,a0r+90+korr+comp0); 
  a0r = a0r+t;
}
else if(a0r>=a0+eps){
  setServo(0,a0r+90+korr+comp0);
  a0r = a0r-t; 
}
else{
  a0r=a0;
  setServo(0,a0r+90+korr+comp0);
}

if(a1r<=a1-eps){
  setServo(1,180-a1r-90+comp1); 
  a1r = a1r+t;
}
else if(a1r>=a1+eps){
  setServo(1,180-a1r-90+comp1); 
  a1r = a1r-t;
}
else{
  a1r=a1;
  setServo(1,180-a1r-90+comp1);
}

if(a2r<=a2-eps){
  setServo(2,90-a2r-korr+comp2); 
  a2r = a2r+t;
}
else if(a2r>=a2+eps){
  setServo(2,90-a2r-korr+comp2);  
  a2r = a2r-t;
}
else{
  a2r=a2;
  setServo(2,90-a2r-korr+comp2); 
}

if(a3r<=a3-eps){
  setServo(3,a3r+90+comp3); 
  a3r = a3r+t;
}
else if(a3r>=a3+eps){
  setServo(3,a3r+90+comp3);  
  a3r = a3r-t;
}
else{
  a3r=a3;
  setServo(3,a3r+90+comp3); 
}

if(a4r<=a4-eps){
  setServo(4,a4r+90+comp4);
  a4r = a4r+t;
}
else if(a4r>=a4+eps){
  setServo(4,a4r+90+comp4);
  a4r = a4r-t;
}
else{
  a4r=a4;
  setServo(4,a4r+90+comp4);
}

if(a5r<=a5-eps){
  setServo(5,a5r+90+comp5); 
  a5r = a5r+t; 
}
else if(a5r>=a5+eps){
  setServo(5,a5r+90+comp5); 
  a5r = a5r-t;
}
else{
  a5r=a5;
  setServo(5,a5r+90+comp5);
}  

if(a6r<=a6-eps){
  setServo(6,90-a6r+comp6); 
  a6r = a6r+t;
}
else if(a6r>=a6+eps){
  setServo(6,90-a6r+comp6); 
  a6r = a6r-t;
}
else{
  a6r=a6;
  setServo(6,90-a6r+comp6);
}

if(a7r<=a7-eps){
  setServo(7,180-a7r-90+comp7); 
  a7r = a7r+t;
}
else if(a1r>=a1+eps){
  setServo(7,180-a7r-90+comp7); 
  a7r = a7r-t;
}
else{
  a7r=a7;
  setServo(7,180-a7r-90+comp7);
}
if(a8r<=a8-eps){
  setServo(8,a8r+90-korr+comp8);
  a8r = a8r+t;
}
else if(a8r>=a8+eps){
  setServo(8,a8r+90-korr+comp8);
  a8r = a8r-t;
}
else{
  a8r=a8;
  setServo(8,a8r+90-korr+comp8);
}

if(a9r<=a9-eps){
  setServo(9,a9r+90+comp9); 
  a9r = a9r+t; 
}
else if(a9r>=a9+eps){
  setServo(9,a9r+90+comp9); 
  a9r = a9r-t;
}
else{
  a9r=a9;
  setServo(9,a9r+90+comp9);
}  

if(a10r<=a10-eps){
  setServo(10,90-a10r+korr+comp10); 
  a10r = a10r+t;
}
else if(a10r>=a10+eps){
  setServo(10,90-a10r+korr+comp10); 
  a10r = a10r-t;
}
else{
  a10r=a10;
  setServo(10,90-a10r+korr+comp10);
}

if(a11r<=a11-eps){
  setServo(11,180-a11r-90+comp11); 
  a11r = a11r+t;
}
else if(a1r>=a1+eps){
  setServo(11,180-a11r-90+comp11); 
  a11r = a11r-t;
}
else{
  a11r=a11;
  setServo(11,180-a11r-90+comp11);
}

delay(v);
Serial.print(ch_1);
Serial.print("/");
Serial.println(a6r);
}
//function to set a servo to a defined position
void setServo(int servo, int angle) {
  int duty;
  duty = map(angle, 0, 180, pos0, pos180);
  pwm.setPWM(servo, 0, duty);
}
//Funktion to lift a leg only servos 1,3,5 and 7 are applicable, remenber you start to count on 0
void liftLeg(int servo){
  int duty;
  if(servo==1){
    a1=a1-stepLift;
    a1r=a1;
    setServo(1,180-a1-90+comp1);
    delay(y);
  }
  else if(servo==3){
    a3=a3-stepLift;
    a3r=a3;
    setServo(3,a3+90+comp3);
    delay(y);
  }
  else if(servo==5){
    a5=a5-stepLift;
    a5r=a5;
    setServo(5,a5+90+comp5);
    delay(y);
  }
  else if(servo==7){
    a7=a7-stepLift;
    a7r=a7;
    setServo(7,180-a7-90+comp7); 
    delay(y);
  }
    else if(servo==9){
    a9=a9-stepLift;
    a9r=a9;
    setServo(9,a9+90+comp9);
    delay(y);
  }
  else if(servo==11){
    a11=a11-stepLift;
    a11r=a11;
    setServo(11,180-a11-90+comp11); 
    delay(y);
  }
}
//Lower a leg
void lowerLeg(int servo){
  int duty;
  if(servo==1){
    a1=a1+stepLift;
    a1r=a1;
    setServo(1,180-a1-90+comp1);
    delay(y);
  }
  else if(servo==3){
    a3=a3+stepLift;
    a3r=a3;
    setServo(3,a3+90+comp3);
    delay(y);
  }
  else if(servo==5){
    a5=a5+stepLift;
    a5r=a5;
    setServo(5,a5+90+comp5);
    delay(y);
  }
  else if(servo==7){
    a7=a7+stepLift;
    a7r=a7;
    setServo(7,180-a7-90+comp7);
    delay(y);
  }
    else if(servo==9){
    a9=a9+stepLift;
    a9r=a9;
    setServo(9,a9+90+comp9);
    delay(y);
  }
  else if(servo==11){
    a11=a11+stepLift;
    a11r=a11;
    setServo(11,180-a11-90+comp11);
    delay(y);
  }

}

