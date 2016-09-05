// Example of syncronous messages
#include <ChibiOS_ARM.h>

// data structures and stack for thread 2
static THD_WORKING_AREA(waTh2, 1000);
thread_t *thread2Pointer;

// data structures and stack for thread 3
static THD_WORKING_AREA(waTh3, 1000);
thread_t *thread3Pointer;

// data structures and stack for thread 3
static THD_WORKING_AREA(waThHB, 64);



struct messageData{   //simple struct to hold message data
  int timeStamp;
  char* message;
};




static THD_FUNCTION(thdTLF1, arg) {
  messageData* myMessage;     //create an empty pointer of type messageData
  static int lastTimeStamp = 0;
  
  while(1){
    chMsgWait();    //wait for a new message
    myMessage = (messageData*)chMsgGet(thread3Pointer); //set our message pointer to the location of the message
    int receieveTime = chVTGetSystemTime();                     //get the current tick

    Serial.println("Message Recieved:");
    Serial.print("Message: ");
    Serial.println(myMessage->message);

    chMsgRelease(thread3Pointer, (msg_t)&myMessage);    //release the message with a pointer to the original message as our ping back message
    lastTimeStamp = myMessage->timeStamp;
    
  }
}




static THD_FUNCTION (thdTLF2, arg) {
  messageData myMessage;    //create a new message with type messageData
  
  while(1){
    myMessage.message = "Hello World";    //set the payload of the message (in this case, the name)
    myMessage.timeStamp = chVTGetSystemTime();    //and the timestamp in ticks  just before sending
    chMsgSend(thread2Pointer, (msg_t)&myMessage); //send the message and wait for reply (syncronous)

    chThdSleep(100);
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
  thread2Pointer = chThdCreateStatic(waTh2, sizeof(waTh2), NORMALPRIO, thdTLF1, NULL);

  // schedule thread 3
  thread3Pointer = chThdCreateStatic(waTh3, sizeof(waTh3), NORMALPRIO, thdTLF2, NULL);

  // schedule heartbeat thread
  chThdCreateStatic(waThHB, sizeof(waThHB), NORMALPRIO - 1, heartbeat, NULL);

  while(1){
    chThdSleep(1000);
  }

  // main thread is thread 1 at NORMALPRIO
}
//------------------------------------------------------------------------------
void loop() {/* not used */}