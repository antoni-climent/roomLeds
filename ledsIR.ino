#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


#define IR_INPUT_PIN    23
#define LED_PIN    22
#define LED_COUNT 287

#include "TinyIRReceiver.cpp.h"
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Global variables for switching pattern
bool changed;
uint8_t last_com = 100; 

//Globals for multyFlashTail
int numTails;
int currentNumTails;

//Color definition
uint32_t myGreen = strip.gamma32(strip.Color(87,   239,   70));
uint32_t myPurple = strip.gamma32(strip.Color(223,   56,   229));
uint32_t myWhite = strip.gamma32(strip.Color(200,   200,   200));
uint32_t myRed = strip.gamma32(strip.Color(200,   0,   0));
uint32_t myBlue = strip.gamma32(strip.Color(0,   111,   255));
uint32_t myPink = strip.gamma32(strip.Color(255,   0,   247));
uint32_t myBlack = strip.gamma32(strip.Color(0,   0,   0));
uint32_t mySoftBlue = strip.gamma32(strip.Color(48,   221,   230));
uint32_t myYellow = strip.gamma32(strip.Color(230,   230,   48));


void setup()
{
    
    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(20); // Set BRIGHTNESS to about 1/5 (max = 255)
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    delay(200);
    Serial.println(F("START " __FILE__ " from " __DATE__));
    initPCIInterruptForTinyReceiver();
    changed = false;
}

void rainbow(int wait) {
  while(not changed){
    for(long firstPixelHue = 0; firstPixelHue < 5*65536 and not changed; firstPixelHue += 256) {
      for(int i = 0; i< strip.numPixels() ; ++i){
        int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

void heartBeat() {
  Serial.println("Has entered heartBeat");
  uint32_t color = strip.Color(200,   0,   0); //Red Color
  int shortBeat = 25;
  int longBeat = 25;
  while(not changed){
    //Short beat
    int intens = 5;
    while(intens < 255) {  //compleated heard beat cycle
      color = strip.Color(intens,   0,   0); //Red Color
      strip.fill(color);
      strip.show(); // Update strip with new contents
      intens += 10;
      delay(shortBeat);  // Pause for a moment
    }
    while(intens > 5) {  //compleated heard beat cycle
      color = strip.Color(intens,   0,   0); //Red Color
      strip.fill(color);
      strip.show(); // Update strip with new contents
      intens -= 10;
      delay(shortBeat);  // Pause for a moment
    }
    //Seccond beat
    while(intens < 250) {  //compleated heard beat cycle
      color = strip.Color(intens,   0,   0); //Red Color
      strip.fill(color);
      strip.show(); // Update strip with new contents
      intens += 5;
      delay(longBeat);  // Pause for a moment
    }
    while(intens > 0) {  //compleated heard beat cycle
      color = strip.Color(intens,   0,   0); //Red Color
      strip.fill(color);
      strip.show(); // Update strip with new contents
      intens -= 5;
      delay(longBeat);  // Pause for a moment
    }
    delay(400);
  }
}

void flashTail(){
  int flashLen = 20;
  while(not changed){
    for(int i = 0; i < strip.numPixels() and not changed; ++i){
      strip.clear();
      int bright = 50;
      for(int j = i; j < i + flashLen; ++j){
        uint32_t myWhite = strip.gamma32(strip.Color(bright,bright,bright));
        strip.setPixelColor(j,myWhite);
        bright += 10;
      }
      strip.show();
      delay(10);
    }
  }
}


void removeFirstEl(int posits[]){
  int elRem = posits[0];
  for(int i = 0; i < numTails-1; i++){
    posits[i] = posits[i+1];
  }
  posits[currentNumTails-1] = -1;
  currentNumTails--;
  Serial.print("Removed: ");
  Serial.println(elRem);
}

void sumOneAndRemove(int posits[]){
  for(int i = 0; i < currentNumTails; i++) posits[i]++;
  if (posits[0] >= strip.numPixels()){ 
    removeFirstEl(posits);
  }
}

void addTail(int posits[]){
  posits[currentNumTails] = 0;
  currentNumTails++;
  for(int i = 0; i < 20; i++) {
    Serial.print(posits[i]);
    Serial.print(" ");
  }
  Serial.println("");
}

void multyFlashTail(int creationSpeed, int tailSpeed){
  numTails = 20;
  currentNumTails = 0;
  int flashLen = 20;
  int posits[numTails] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  long unsigned int lastTime = millis();
  long unsigned int creationTime = millis();
  while(not changed){
    if(millis() - creationTime > creationSpeed){
      addTail(posits);      
      Serial.println("TAIL CREATED");
      creationTime = millis();
    }
    if(millis() - lastTime > tailSpeed){
      for(int indPos = 0; indPos < currentNumTails; ++indPos){
        strip.numPixels();
        int i = posits[indPos];
        strip.setPixelColor(i-1,(0,0,0));
        int bright = 50;
        for(int j = i; j < i + flashLen; ++j){
          uint32_t myWhite = strip.gamma32(strip.Color(bright,bright,bright));
          strip.setPixelColor(j,myWhite);
          bright += 10;
        }
      }
      strip.show();
      sumOneAndRemove(posits);
      lastTime = millis();
    }
  }
}

void multyColorFlashTail(int creationSpeed, int tailSpeed, int r, int g, int b){
  numTails = 20;
  currentNumTails = 0;
  int flashLen = 20;
  int posits[numTails] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  long unsigned int lastTime = millis();
  long unsigned int creationTime = millis();
  while(not changed){
    if(millis() - creationTime > creationSpeed){
      addTail(posits);      
      Serial.println("TAIL CREATED");
      creationTime = millis();
    }
    if(millis() - lastTime > tailSpeed){
      for(int indPos = 0; indPos < currentNumTails; ++indPos){
        strip.numPixels();
        int i = posits[indPos];
        strip.setPixelColor(i-1,(0,0,0));
        int bright = flashLen*2.;
        for(int j = i; j < i + flashLen; ++j){
          uint32_t myColor = strip.gamma32(strip.Color(max(0,r-bright),max(0,g-bright),max(0,b-bright)));
          strip.setPixelColor(j,myColor);
          bright -= 2;
        }
      }
      strip.show();
      sumOneAndRemove(posits);
      lastTime = millis();
    }
  }
}

void twoColors(uint32_t firstColor, uint32_t sedcondColor) {
  strip.clear();
  delay(500);
  int wait = 100;
  int myStep = 10;
    
  while(not changed) {
    for(int i=0; i<myStep*2; i++) { 
      strip.clear();
           
      for(int j=i; j<strip.numPixels(); j += myStep*2) {
        strip.fill(firstColor, j, myStep); // Set pixel 'c' to value 'color'
        strip.fill(sedcondColor, j+myStep, myStep); // Set pixel 'c' to value 'color'
      }
      
      //Set up first leds
      if (i > 0){
        if (i <= myStep){
          strip.fill(sedcondColor, 0, i);
        }
        else{
          strip.fill(firstColor, 0, i-myStep);
          strip.fill(sedcondColor, i-myStep, myStep);
        }
      }
      
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

void theaterChase(uint32_t color, int wait) {
  while(not changed) { 
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

void myPattern(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      if(b==2){
        strip.setBrightness(200);
      }
      else{
        strip.setBrightness(20);
      }
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  while(not changed) { 
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void solid(uint32_t color){
  strip.fill(color);
  strip.show();
}

void firstBreathing(){
  int i = 140;
  bool up = true;
  uint32_t color = strip.gamma32(strip.ColorHSV(54612,255,i));
  long unsigned int lastTime = millis();
  int dist = 0;
  while(not changed){
    //Serial.println(i);
    color = strip.gamma32(strip.ColorHSV(18200,255,i)); //hue pink: 54612
    strip.fill(color);
    strip.show();
    if (i <= 130 or i >= 250) up = not up;
    if(up) i = i + 5;
    else i = i - 5;
    delay(50);
  }
}

void breathing(){
  uint32_t color = strip.gamma32(strip.Color(212,54,218));
  strip.fill(color);
  strip.show();
  int i = 60;
  bool up = true;
  while(not changed){
     strip.setBrightness(i);
     strip.show();
      if (i <= 20 or i >= 190){
        up = not up;
        delay(300);
      }
      if(up) i = i + 10;
      else i = i - 10;
      delay(100);
  }
}

void loop()
{
  changed = false;
  switch(last_com){
    case 6: //EQ
    rainbow(50);
    case 7: //0
    heartBeat();
    break;
    case 10: //1
    twoColors(myPurple,myGreen);
    break;
    case 27: //2
    twoColors(myPurple,myBlue);
    break;
    case 31: //3
    twoColors(myPurple,myBlack);
    break;
    case 12: //4
    multyColorFlashTail(2000, 10,225,0,247); //myPink
    break;
    case 13: //5
    multyColorFlashTail(2000, 10, 48,   221,   230); //mySoftBlue
    break;
    case 14: //6
    multyColorFlashTail(2000, 10, 87, 239, 70); //myGreen
    break;
    case 0:  //7
    multyFlashTail(500,1);
    break;
    case 15: //8
    multyFlashTail(1000,5); //creation speed and tail speed (the higher the slower)
    break;
    case 25: //9
    multyFlashTail(2000, 10);
    break;
    case 18: //ch-
    solid(myWhite);
    break;
    case 26: //ch
    solid(myRed);
    break;
    case 30: //ch+
    solid(myPink);
    break;
    case 1: //<<
    solid(myGreen);
    break;
    case 2: //>>
    solid(myBlue);
    break;
    case 3: //>||
    solid(mySoftBlue);
    break;
    default:
    //Serial.println("Command not recognized");
    breathing();
    break;
  }
}

void IRAM_ATTR handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat)
{
    Serial.print(F(" N="));
    Serial.println(aCommand);
    if (aCommand == 4) strip.setBrightness(strip.getBrightness()-10);
    else if (aCommand == 5) strip.setBrightness(strip.getBrightness()+10);
    else if(last_com != aCommand){
      changed = true;
      last_com = aCommand;
    }
    
}
