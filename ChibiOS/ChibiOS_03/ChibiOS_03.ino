// Example of using mutexes
#include <ChibiOS_ARM.h>

// data structures and stack for thread 2
static THD_WORKING_AREA(waTh2, 100);

// data structures and stack for thread 3
static THD_WORKING_AREA(waTh3, 100);

// data structures and stack for thread 3
static THD_WORKING_AREA(waThHB, 64);


//create a mutex that we will use to control access to the serial line
MUTEX_DECL(serialMtx);



static THD_FUNCTION(thdTLF1, arg) {

  while(1){
    chMtxLock(&serialMtx);  //lock access to the serial line to only this thread

    for(int i = 0; i < 10; i ++){ 
      Serial.println(i);
      chThdSleep(100);
    }

    chMtxUnlock(&serialMtx);  //release access to the serial line
  }

}




static THD_FUNCTION (thdTLF2, arg) {
  
  while(1){
    chMtxLock(&serialMtx);  //lock access to the serial line to only this thread

    Serial.println("My Turn!");
    chThdSleep(10);

    chMtxUnlock(&serialMtx);  //release access to the serial line
  }

}



//basic heartbeat thread to tell us whether weve crashed
static THD_FUNCTION(heartbeat, arg){
  int ledPin = 13;

  pinMode(ledPin, OUTPUT);
  while(1){
    digitalWrite(ledPin, HIGH);
    chThdSleep(10);
    digitalWrite(ledPin, LOW);
    chThdSleep(300);
    digitalWrite(ledPin, HIGH);
    chThdSleep(10);
    digitalWrite(ledPin, LOW);
    chThdSleep(1000);
  }
}





//------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  
  // initialize and start ChibiOS
  chBegin(chSetup);
  
  // should not return
  while(1);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void chSetup() {
  // schedule thread 2
  chThdCreateStatic(waTh2, sizeof(waTh2), NORMALPRIO, thdTLF1, NULL);

  // schedule thread 3
  chThdCreateStatic(waTh3, sizeof(waTh3), NORMALPRIO + 1, thdTLF2, NULL);

  // schedule heartbeat thread
  chThdCreateStatic(waThHB, sizeof(waThHB), NORMALPRIO - 1, heartbeat, NULL);

  while(1){
    chThdSleep(1000);
  }

  // main thread is thread 1 at NORMALPRIO
}
//------------------------------------------------------------------------------
void loop() {/* not used */}