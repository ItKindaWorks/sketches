// example of memory pool and mailboxes with two senders and one receiver
#include <ChibiOS_ARM.h>

MUTEX_DECL(pcSerialMutex);

typedef struct threadInit { 
  int pin;
  int id;
};


//mailbox creation steps:
//  1. (mailMessage) - create struct for mail to be stored/structured
//  2. (myMessageCount) - determine box size (how many messages it can hold) and create const to hold the count
//  3. (myMessages) - create array of structs the size of the mailbox
//  4. (myMessagePool) - declare memory pool with a name and message count (box size from (2))
//  5. (message) - create msg_t array of boxsize
//  6. (myMail) - create the mailbox with a name, msg_t array from (5), and box size


///////////Mailbox start setup

typedef struct mailMessage { //(1) type for a memory pool object
  int id;
  int msg;
  int timeStamp;
};


const size_t myMessageCount = 5;  //(2) mailbox size and memory pool object count

//create the mem pool for the messages
mailMessage myMessages[myMessageCount];  //(3) array of memory pool objects
MEMORYPOOL_DECL(myMessagePool, myMessageCount, 0); //(4) memory pool structure

//create the mailbox to push and pull messages from
msg_t message[myMessageCount];  //(5) slots for mailbox messages
MAILBOX_DECL(myMail, &message, myMessageCount); //(6) mailbox structure
//------------------------------------------------------------------------------------------------------------------------------------------------------------

///////////Mailbox end setup




///////////Mailbox functions

//allocates the next message in the pool and returns a void pointer to that location
//the calling function must know what the expected data type is
void* allocNextMessage(memory_pool_t* messagePool){    
  void* p = (void*)chPoolAlloc(messagePool);    //allocate the next message in the pool
  if (!p) {   //if the alloc fails, print and halt
    Serial.println("chPoolAlloc failed");
    while(1);
  }
  return p; //else return pointer to message (the caller must know the expected data type as we return a void*)
}

//posts the given message to the given mailbox
//the calling function MUST post the correct message type to the mailbox
msg_t postMessage(mailbox_t* mailbox, void* messageToSend){

  msg_t s = chMBPost(mailbox, (msg_t)messageToSend, TIME_IMMEDIATE);

  if (s != MSG_OK) {  //if post retunrs anything other than all good, print and halt
    Serial.println("chMBPost failed");
    while(1);  
  }

  return s; //else return the operation status (should always return RDY_OK)
}

void initMailboxes(){
    // fill pool with mailMessage array pointers
    for (int i = 0; i < myMessageCount; i++) {chPoolFree(&myMessagePool, &myMessages[i]);}
}


///////////Mailbox functions







//basic heartbeat thread to tell us whether weve crashed
static THD_WORKING_AREA(waTh5, 16);   //heartbeat thread
static THD_FUNCTION (heartbeat, arg){
    pinMode(13, OUTPUT);

    while(1){
        digitalWrite(13, HIGH);
        chThdSleep(10);
        digitalWrite(13, LOW);
        chThdSleep(300);
        digitalWrite(13, HIGH);
        chThdSleep(10);
        digitalWrite(13, LOW);
        chThdSleep(1000);
    }

}



static THD_WORKING_AREA(waTh1, 64);

static THD_FUNCTION (counter, arg) {
    threadInit* thisThread = (threadInit*) arg;
    int msg = 0;
    int id = thisThread->id;

    int incVal = 1;

    while (1) {
        
        //flip the incrementor at 100 and -1
        if(msg == 100 || msg == -1){    
            incVal = -incVal;
        }

        // get object from memory pool
        mailMessage* p = (mailMessage*)allocNextMessage(&myMessagePool);

        // form message
        p->id = id;
        p->msg = msg += incVal;
        p->timeStamp = chVTGetSystemTime();

        // send message
        postMessage(&myMail, (void*)p);

        //sleep
        chThdSleep(50);    
    }
}

static THD_WORKING_AREA(waTh2, 64);
static THD_WORKING_AREA(waTh3, 64);

static THD_FUNCTION (ADCreader, arg) {
    threadInit* thisThread = (threadInit*) arg;

    int msg = 0;
    int id = thisThread->id;
    int pin = thisThread->pin;

    while (1) {
        //get data
        msg = analogRead(pin);

        // get object from memory pool
        mailMessage* p = (mailMessage*)allocNextMessage(&myMessagePool);

        // form message
        p->id = id;
        p->msg = msg;
        p->timeStamp = chVTGetSystemTime();

        // send message
        postMessage(&myMail, (void*)p);

        //sleep
        chThdSleep(50);    
    }
}


static THD_WORKING_AREA(waTh4, 64);

static THD_FUNCTION (reciever, arg){
    mailMessage *p;

    int msg1Val = -1;
    int msg2Val = -1;
    int msg3Val = -1;


    while (1) {

        // get mail
        if(chMBFetch(&myMail, (msg_t*)&p, 1) == MSG_OK){  //clarify (msg_t*)&p:  cast &p as a msg_t pointer

            if(p->id == 1 && msg1Val == -1){
                msg1Val = p->msg;
            }
            else if(p->id == 2 && msg2Val == -1){
                msg2Val = p->msg;
            }
            else if(p->id == 3 && msg3Val == -1){
                msg3Val = p->msg;
            }

            // free the message back into pool
            chPoolFree(&myMessagePool, p);
        }



        //print data
        if(msg1Val != -1 && msg2Val != -1 && msg3Val != -1){
            //Lock the serial lines
            chMtxLock(&pcSerialMutex);

            //print the data
            Serial.print(msg1Val);
            Serial.print(",");
            Serial.print(msg2Val);
            Serial.print(",");
            Serial.println(msg3Val);

            //unlock the serial line
            chMtxUnlock(&pcSerialMutex);

            msg1Val = -1;
            msg2Val = -1;
            msg3Val = -1;
        }

        //sleep
        chThdSleep(10);
    }
}




//Thread setup

void mainThread() {
    mySetup();
    while(1){myLoop();} //main thread loop
}
void mySetup(){

    //incrementor thread setup
    threadInit sender1;
    sender1.id = 1;

    //ADC thread setup
    threadInit sender2;
    sender2.id = 2;
    sender2.pin = 0;

    threadInit sender3;
    sender3.id = 3;
    sender3.pin = 1;


    initMailboxes();

    //schedule printer thread
    chThdCreateStatic(waTh5, sizeof(waTh5), LOWPRIO, heartbeat, (void*)NULL); //heartbeat thread

    //start the serial line
    while (!Serial) {chThdSleep(10);}

    //schedule counter thread
    chThdCreateStatic(waTh1, sizeof(waTh2), NORMALPRIO, counter, (void*)&sender1);

    //schedule ADC threads
    chThdCreateStatic(waTh2, sizeof(waTh2), NORMALPRIO, ADCreader, (void*)&sender2);
    chThdCreateStatic(waTh3, sizeof(waTh3), NORMALPRIO, ADCreader, (void*)&sender3);

    //schedule printer thread
    chThdCreateStatic(waTh4, sizeof(waTh4), NORMALPRIO, reciever, (void*)NULL);

    while(1){chThdSleep(100);}
}
void myLoop(){
    chThdSleep(10);
}

void setup() {
    Serial.begin(9600);
    chBegin(mainThread);
    while(1);
}
void loop() {
  // not used
}