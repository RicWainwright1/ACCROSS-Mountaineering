#include <SPI.h>
#include <RH_RF69.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Adafruit_DRV2605.h"
#ifdef __AVR__
#include <avr/power.h>
#endif


const int  button = 9;    // the pin that the pushbutton is attached to
const int outPin = 13; 
const int  StateSetbuttonPin = 5;    // the pin that the pushbutton is attached to
const int Send = 6;     // the number of the pushbutton pin
const int StateLED = 11;


Adafruit_DRV2605 drv;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, 10, NEO_RGBW + NEO_KHZ800);

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 434.0

#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
#define RFM69_CS      8
#define RFM69_INT     7
#define RFM69_RST     4
#endif

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonpushnum = 0;
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
int recievedyesno = 0;
int StateSetbuttonPushCounter = 0;   // counter for the number of button presses
int StateSetbuttonState = 0;         // current state of the button
int StateSetlastButtonState = 0;     // previous state of the button
int stateLED = LOW;
boolean buttonActive = false;
boolean longPressActive = false;
long buttonTimer = 0;
long longPressTime = 250;
int count = 0;               // Count the button presses
int modecount = 0;               // Count the button presses
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
int statevalue = HIGH;      // the current state of the output pin
int statereading;           // the current reading from the input pin
int stateprevious = LOW;    // the previous reading from the input pin
int SendState = 0;         // variable for reading the pushbutton status

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers
long statedebounce = 200;   // the debounce time, increase if the output flickers

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);
String stringOne;

void setup() {
  // initialize the button pin as a input:
  pinMode(button, INPUT);
  pinMode(outPin, OUTPUT);
  pinMode(StateLED, OUTPUT);
    pinMode(Send, INPUT);
  // initialize the button pin as a input:
  pinMode(StateSetbuttonPin, INPUT);
  // initialize serial communication:
  Serial.begin(9600);
  strip.begin();
  drv.begin();

  drv.selectLibrary(1);

  // I2C trigger by sending 'go' command
  // default, internal trigger when sending GO command
  drv.setMode(DRV2605_MODE_INTTRIG);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 RX/TX Test!");

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(14, true);

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                  };
  rf69.setEncryptionKey(key);



  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
  delay(500);
  // I2C trigger by sending 'go' command
  // default, internal trigger when sending GO command
  drv.setMode(DRV2605_MODE_INTTRIG);
}

int str1 = "65";
int str2 = "66";
int str3 = "67";
int str4 = "68";
int str5 = "69";
int str6 = "70";

void loop() {
  reading = digitalRead(button);
  statereading = digitalRead(StateSetbuttonPin);

  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    modecount++;
    Serial.print("mode counter =");
    Serial.println(modecount);
    if (state == HIGH)
      state = LOW;
    else
      state = HIGH;
    time = millis();
  }
  digitalWrite(outPin, state);
  previous = reading;


  if (statereading == HIGH && stateprevious == LOW && millis() - time > statedebounce) {
    count++;
    Serial.print("state counter =");
    Serial.println(count);
    if (statevalue == HIGH)
      statevalue = LOW;
    else
      statevalue = HIGH;
    time = millis();
  }
  
  stateprevious = statereading;

  

  if(count>2){
    count=0;
  }
    if(modecount>=6){
    modecount=0;
  }
  if(count==1){
    digitalWrite(StateLED, LOW);
    selectmode();
  }
    if(count==2){
    digitalWrite(StateLED, HIGH);
    recieved();
    
  }
  
}

/************************recieved****************************/
void recieved() {
  if (rf69.waitAvailableTimeout(100)) {
    // Should be a message for us now
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    int str1 = "65";
    int str2 = "66";
    int str3 = "67";
    int str4 = "68";
    int str5 = "69";
    int str6 = "70";
    Serial.println("nearly there");
    stringOne = String(buf[7]);
    if (! rf69.recv(buf, &len)) {
      Serial.println("Receive failed");
      return;
    }
    rf69.printBuffer("Received: ", buf, len);
    buf[len] = 0;

    Serial.print("Got: "); Serial.println((char*)buf);
    Serial.print("RSSI: "); Serial.println(rf69.lastRssi(), DEC);
    Serial.println(stringOne);
    Serial.println(str1);
    Serial.println(buf[8]);
    Serial.println(buf[0]);
    Serial.println(buf[1]);
    Serial.println(buf[2]);
    Serial.println(buf[3]);
    Serial.println(buf[4]);
    Serial.println(buf[5]);
    Serial.println(buf[6]);
    Serial.println(buf[7]);

  }
  if (stringOne == str1)  {
    Serial.println("ok");
    OK();
  }
  if (stringOne == str2)  {
    Serial.println("ProblemHoldONLights");
    ProblemHoldON();
  }
  if (stringOne == str3)  {
    Serial.println("How Are You");
    HowAreYou(strip.Color(0, 0, 127));
  }
  if (stringOne == str4)  {
    Serial.println("stopLights");
    STP();
  }
  if (stringOne == str5)  {
    Serial.println("GO Lights");
    GO(strip.Color(127, 0, 0));
  }
  if (stringOne == str6)  {
    Serial.println("Send mode");
    strip.clear();
  }
}
//*****************************************************************************
//-----------------------------------MODES-------------------------------------
//*****************************************************************************


//Select Mode Lights
void selectmode() {
  SendState = digitalRead(Send);

  if (modecount == 1) {
    Serial.println("ok preview");
    OKPreview();
    strip.show();
    strip.clear();
  }
  if (modecount == 2) {
    ProblemHoldONPreview();
    Serial.println("problem hold on preview");
    strip.show();
    strip.clear();
  }
  if (modecount == 3) {
    HowAreYouPreview(strip.Color(0, 0, 127));
    Serial.println("how are you preview");
    strip.show();
    strip.clear();
  }
  if (modecount == 4) {
    STPPreview();
    Serial.println("stop preview");
    strip.show();
    strip.clear();
  }
  if (modecount == 5) {
    GOPreview(strip.Color(127, 0, 0));
    Serial.println("Go preview");
    strip.show();
    strip.clear();
  }
  if (modecount == 6) {
    strip.setPixelColor(5, 127, 0, 0);
    strip.show();
    strip.clear();
  }
  if ((SendState==HIGH) && modecount == 1) {
    char radiopacket[20] = "Button A";
    Serial.print("Sending "); Serial.println(radiopacket);
    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
    rf69.waitPacketSent();
  }
  if ((SendState==HIGH) && modecount == 2) {
    char radiopacket[20] = "Button B";
    Serial.print("Sending "); Serial.println(radiopacket);
    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
    rf69.waitPacketSent();
  }
  if ((SendState==HIGH) && modecount == 3) {
    char radiopacket[20] = "Button C";
    Serial.print("Sending "); Serial.println(radiopacket);
    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
    rf69.waitPacketSent();
  }
  if ((SendState==HIGH) && modecount == 4) {
    char radiopacket[20] = "Button D";
    Serial.print("Sending "); Serial.println(radiopacket);
    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
    rf69.waitPacketSent();
  }
  if ((SendState==HIGH) && modecount == 5) {
    char radiopacket[20] = "Button E";
    Serial.print("Sending "); Serial.println(radiopacket);
    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
    rf69.waitPacketSent();
  }
  if ((SendState==HIGH) && modecount == 6) {
    char radiopacket[20] = "Button F";
    Serial.print("Sending "); Serial.println(radiopacket);
    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
    rf69.waitPacketSent();
  }
}

// How Are You Function
static void HowAreYou(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i  , c); // Draw new pixel
    strip.show();
    // set the effect to play
    drv.setWaveform(0, 83);  // play effect
    drv.setWaveform(1, 0);       // end waveform

    // play the effect!
    drv.go();
    delay(100);
  }
  strip.clear();
}

void OK() {
  strip.setPixelColor(0, 127, 0, 0);
  strip.setPixelColor(1, 127, 0, 0);
  strip.setPixelColor(2, 127, 0, 0);
  strip.setPixelColor(3, 127, 0, 0);
  strip.setPixelColor(4, 127, 0, 0);
  strip.setPixelColor(5, 127, 0, 0);
  strip.setPixelColor(6, 127, 0, 0);
  strip.setPixelColor(7, 127, 0, 0);
  drv.setWaveform(0, 83);  // play effect
  drv.setWaveform(1, 0);       // end waveform

  // play the effect!
  drv.go();
  strip.show();
  strip.clear();
}

// Problem Hold On Function
void ProblemHoldON() {
  strip.setPixelColor(0, 127, 100, 0);
  strip.setPixelColor(1, 127, 100, 0);
  strip.setPixelColor(2, 127, 100, 0);
  strip.setPixelColor(3, 127, 100, 0);
  strip.setPixelColor(4, 127, 100, 0);
  strip.setPixelColor(5, 127, 100, 0);
  strip.setPixelColor(6, 127, 100, 0);
  strip.setPixelColor(7, 127, 100, 0);
  drv.setWaveform(0, 83);  // play effect
  drv.setWaveform(1, 0);       // end waveform

  // play the effect!
  drv.go();
  strip.show();
  strip.clear();
}

// Go Function
static void GO(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels() + 4; i++) {
    strip.setPixelColor(i  , c); // Draw new pixel
    strip.setPixelColor(i - 4, 0); // Erase pixel a few steps back
    drv.setWaveform(0, 83);  // play effect
    drv.setWaveform(1, 0);       // end waveform

    // play the effect!
    drv.go();
    strip.show();
    delay(100);
  }
}

//Recieved Function
void Recieved() {
  strip.setPixelColor(0, 0, 0, 100);
  strip.setPixelColor(7, 0, 0, 100);
  strip.show();
  drv.setWaveform(0, 17);  // play effect
  drv.setWaveform(1, 0);       // end waveform
  drv.go();// play the effect!
  delay(200);
  strip.setPixelColor(1, 0, 0, 100);
  strip.setPixelColor(6, 0, 0, 100);
  strip.show();
  drv.setWaveform(0, 17);  // play effect
  drv.setWaveform(1, 0);       // end waveform
  drv.go();// play the effect!
  delay(200);
  strip.setPixelColor(2, 0, 0, 100);
  strip.setPixelColor(5, 0, 0, 100);
  strip.show();
  drv.setWaveform(0, 17);  // play effect
  drv.setWaveform(1, 0);       // end waveform
  drv.go();// play the effect!
  delay(200);
  strip.setPixelColor(3, 0, 0, 100);
  strip.setPixelColor(4, 0, 0, 100);
  strip.show();
  drv.setWaveform(0, 17);  // play effect
  drv.setWaveform(1, 0);       // end waveform
  drv.go();// play the effect!
  delay(100);
  strip.clear();
}

//STOP Function
void STP() {

  for (int i = 0; i <= 10; i++)
  {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(0, 0, 100, 0);
    strip.setPixelColor(1, 0, 100, 0);
    strip.setPixelColor(2, 0, 100, 0);
    strip.setPixelColor(3, 0, 100, 0);
    strip.setPixelColor(4, 0, 100, 0);
    strip.setPixelColor(5, 0, 100, 0);
    strip.setPixelColor(6, 0, 100, 0);
    strip.setPixelColor(7, 0, 100, 0);
    strip.show();
    delay(200);
    strip.clear();
    strip.show();
    delay(150);
    strip.clear();
    strip.show();
    delay(150);
    strip.show();
    delay(150);
    strip.clear();
    strip.show();
    delay(150);
    strip.show();
    delay(150);
    strip.clear();
    strip.show();
    delay(150);
    drv.setWaveform(0, 17);  // play effect
    drv.setWaveform(1, 0);       // end waveform

    // play the effect!
    drv.go();
  }
}
/*********************************************/
/*******************PREVIEWS******************/
/*********************************************/
// How Are You Preview
static void HowAreYouPreview(uint32_t c) {
  for (uint16_t i = 0; i < 4; i++) {
    strip.setPixelColor(i  , c); // Draw new pixel
    strip.show();
    delay(100);
  }
  strip.clear();
}

void OKPreview() {
  strip.setPixelColor(0, 127, 0, 0);
  strip.setPixelColor(1, 127, 0, 0);
  strip.setPixelColor(2, 127, 0, 0);
  strip.setPixelColor(3, 127, 0, 0);
  strip.show();
  delay(500);
  strip.clear();
}

// Problem Hold On Function
void ProblemHoldONPreview() {
  strip.setPixelColor(0, 127, 100, 0);
  strip.setPixelColor(1, 127, 100, 0);
  strip.setPixelColor(2, 127, 100, 0);
  strip.setPixelColor(3, 127, 100, 0);
  strip.show();
  delay(500);
  strip.clear();
}

// Go Function
static void GOPreview(uint32_t c) {
  for (uint16_t i = 0; i < 4 ; i++) {
    strip.setPixelColor(i  , c); // Draw new pixel
    strip.setPixelColor(i - 4, 0); // Erase pixel a few steps back
    strip.show();
    delay(100);
  }
}

//Recieved Function
void RecievedPreview() {
  strip.setPixelColor(0, 0, 0, 100);
  strip.show();
  delay(200);
  strip.setPixelColor(1, 0, 0, 100);
  strip.show();
  delay(200);
  strip.setPixelColor(2, 0, 0, 100);
  strip.show();
  delay(200);
  strip.setPixelColor(3, 0, 0, 100);
  strip.show();
  delay(100);
  strip.clear();
}

//STOP Function
void STPPreview() {

  for (int i = 0; i <= 2; i++)
  {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(0, 0, 100, 0);
    strip.setPixelColor(1, 0, 100, 0);
    strip.setPixelColor(2, 0, 100, 0);
    strip.setPixelColor(3, 0, 100, 0);
    strip.show();
    delay(200);
//    strip.clear();
//    strip.show();
//    delay(150);
//    strip.clear();
//    strip.show();



  }
}
