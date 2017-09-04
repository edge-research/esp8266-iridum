#define DEFAULT_TIMEOUT_MS 1000

#include <SoftwareSerial.h>

// Create a SoftwareSerial or Hardware Serial where communicacion will be stablish
#define RxPin 14
#define TxPin 12

char replybuffer[255];

char signalStrength[4];

//SoftwareSerial  cmdSerial(RxPin, TxPin);
SoftwareSerial cmdSerial(RxPin, TxPin, false, 256);

void setup() {

  Serial.begin(9600);
  Serial.println(F("Iridium basic test"));


  cmdSerial.begin(19200);

  //disable echo
  getReply("ATE0",DEFAULT_TIMEOUT_MS,true);
  
}

void loop() {

  getReply("AT\r",DEFAULT_TIMEOUT_MS,false);

  getReply("AT+CSQ\r",DEFAULT_TIMEOUT_MS,false);

  char * pch = strstr(replybuffer,"CSQ:");
  if (pch) {

  signalStrength[1];
  strncpy(signalStrength, &replybuffer[5], 1);
  //signalStrength[1] = '\0';
  int signalInt = atoi(signalStrength);
//Serial.println(signalStrength);
Serial.print("Signal strength = ");
Serial.println(signalInt);

if (signalInt > 0) {

Serial.println("Sending ...");
getReply("AT&K0\r",DEFAULT_TIMEOUT_MS,false);
delay(1000);
getReply("AT+SBDWT=Hello World\r",DEFAULT_TIMEOUT_MS,false);
delay(1000);
getReply("AT+SBDIX\r",DEFAULT_TIMEOUT_MS,false);
delay(1000);

delay(60000); // wait a minute
  
}
  }
  

  delay(1000);
}

uint8_t readline(uint16_t timeout, boolean multiline) {
  uint16_t replyidx = 0;

  while (timeout--) {
    if (replyidx >= 254) {
      //DEBUG_PRINTLN(F("SPACE"));
      break;
    }

    while(cmdSerial.available()) {
      char c =  cmdSerial.read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0)   // the first 0x0A is ignored
          continue;

        if (!multiline) {
          timeout = 0;         // the second 0x0A is the end of the line
          break;
        }
      }
      replybuffer[replyidx] = c;
      //DEBUG_PRINT(c, HEX); DEBUG_PRINT("#"); DEBUG_PRINTLN(c);
      replyidx++;
    }

    if (timeout == 0) {
      //Serial.println(F("\nTIMEOUT"));
      break;
    }
    delay(1);
  }
  replybuffer[replyidx] = 0;  // null term
  return replyidx;
}

void flushInput() {
    // Read all available serial input to flush pending data.
    uint16_t timeoutloop = 0;
    while (timeoutloop++ < 40) {
        while(cmdSerial.available()) {
            cmdSerial.read();
            timeoutloop = 0;  // If char was received reset the timer
        }
        delay(1);
    }
}

uint8_t getReply(char *send, uint16_t timeout, bool multipleLines) {
  flushInput();


  Serial.println(F("Sending:")); Serial.println(send);


  cmdSerial.println(send);

  uint8_t l = readline(timeout,multipleLines); // false -- only get one line from reply

  Serial.println(F("Receiving: ")); Serial.println(replybuffer);

  Serial.println("-------");

  return l;
}

