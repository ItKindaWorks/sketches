#include <ChibiOS_ARM.h>


static THD_WORKING_AREA(waTh1, 100);
static THD_WORKING_AREA(waTh2, 100);

struct threadData{
  int _blinkTime;
  int _lightPin;
  int _fadeTime;
};




static THD_FUNCTION (blinkerThread, arg) {
  //setup thread vars
  threadData *thisData = (threadData*)arg;
  int lightPin = thisData->_lightPin;
  int blinkTime = thisData->_blinkTime;

  //set the LED pinMode
  pinMode(lightPin, OUTPUT);

  while(1){

    //blink
    digitalWrite(lightPin, HIGH);
    chThdSleep(blinkTime);
    digitalWrite(lightPin, LOW);
    chThdSleep(blinkTime);
  }
}



static THD_FUNCTION (fadeThread, arg){
  threadData *thisData = (threadData*)arg;
  int lightPin = thisData->_lightPin;
  int fadeTime = thisData->_fadeTime;

  pinMode(lightPin, OUTPUT);
  while(1){

    for(int i = 0; i < 255; i+=5){
      analogWrite(lightPin, i);
      chThdSleep(fadeTime);
    }
    for(int i = 255; i > 0; i-=5){
      analogWrite(lightPin, i);
      chThdSleep(fadeTime);
    }
  }
}










//------------------------------------------------------------------------------
void setup() {
  // initialize and start ChibiOS
  chBegin(chSetup);
  
  // should not return
  while(1);
}


//------------------------------------------------------------------------------
void chSetup() {
  threadData set1;
  set1._lightPin = 13;
  set1._blinkTime = 300;

  threadData set2;
  set2._lightPin = 20;
  set2._fadeTime = 10;


  //schedule thread 2 
  chThdCreateStatic(waTh1, sizeof(waTh1), NORMALPRIO, blinkerThread, (void*)&set1);

  //schedule thread 3 (fading)
  chThdCreateStatic(waTh2, sizeof(waTh2), NORMALPRIO, fadeThread, (void*)&set2);



  while(1){
    chThdSleep(10000);
  }
}
//------------------------------------------------------------------------------
void loop() {/* not used */}