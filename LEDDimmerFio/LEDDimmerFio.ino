/*
 Fade
 
 This example shows how to fade an LED on pin 9
 using the analogWrite() function.
 
 This example code is in the public domain.
 
 */
int brightness = 0;    // how bright the LED is

enum serialState {
  WAITING_MSG_HEAD1,
  WAITING_MSG_HEAD2,
  WAITING_LED_VALUE,
  WAITING_NL,
  WAITING_CR
};

enum controlMode {
  POT_CONTROLLED,
  SERIAL_CONTROLLED
};
const int ledPin = 9;
const int msg_head1 = 109;//m
const int msg_head2 = 115;//s
const int this_id = 49;//ascii '1'
const int msg_nl = 13;//newline
const int msg_cr = 10;//carriage return
serialState currentSerialState = WAITING_MSG_HEAD1;
controlMode currentControlMode = SERIAL_CONTROLLED;
int serialLedValue = 0;
int incomingSerialLedValue = 0;
int lastValue;


void setup()  { 
  // declare pin 9 to be an output:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
} 

void loop()  { 
  static int value, inByte;
  //value = analogRead(0) / 4;
  
  if(Serial.available()) {
    inByte = Serial.read();
    switch(currentSerialState) {
      case WAITING_MSG_HEAD1:
        //Serial.println("head1");
        if(inByte == msg_head1)
          { currentSerialState = WAITING_MSG_HEAD2; }
          else {
            currentSerialState = WAITING_MSG_HEAD1;
            //Serial.println("backTo");
          };
        break;
      case WAITING_MSG_HEAD2:
        //Serial.println("head2");
        if(inByte == msg_head2)
          { currentSerialState = WAITING_LED_VALUE; }
          else {
            currentSerialState = WAITING_MSG_HEAD1;
            //Serial.println("backTo");
          };
        break;
      case WAITING_LED_VALUE:
        //Serial.println("led_value");
        incomingSerialLedValue = inByte;
        currentSerialState = WAITING_NL;
        break;
      case WAITING_NL:
        //Serial.println("newline");
        if(inByte == msg_nl) {
          currentSerialState = WAITING_CR;
        } else {
          //Serial.println("backTo");
          currentSerialState = WAITING_MSG_HEAD1;
        }
        break;
      case WAITING_CR:
        //Serial.println("creturn");
        if(inByte == msg_cr) {
          serialLedValue = incomingSerialLedValue;
          //Serial.print("LED to: ");
          //Serial.println(serialLedValue);
        } else {
          //Serial.println("backTo");
        };
        currentSerialState = WAITING_MSG_HEAD1;
        currentControlMode = SERIAL_CONTROLLED;
        break;
      default:
        //Serial.println("default");
        currentSerialState = WAITING_MSG_HEAD1;
    };
    //Serial.println(inByte);
  }
  
  
  if(currentControlMode == SERIAL_CONTROLLED) {
    int testValue = value >> 4;
    int testLastValue = lastValue >> 4;
    //if((testValue < testLastValue) || (testValue > testLastValue)) {
      //currentControlMode = POT_CONTROLLED;
      //Serial.println("backToPotControl");
    //};
    analogWrite(ledPin, serialLedValue);
  };   
  
  //if(currentControlMode == POT_CONTROLLED) {
    //analogWrite(9, value);
    //lastValue = value;
  //};
  delay(10);                            
}
