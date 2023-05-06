#include <Arduino.h>
#include <rn2xx3.h>
#include <SoftwareSerial.h>

#define FREQ 866000000  //this frequency allows a duty cicle of 1%
#define WATCHDOG 60000  //set to 1 min, in this way we wait the boat device to open the communication for more than what it needs for a full loop
SoftwareSerial LoRaSerial(4, 5); //Lora module connections: 4 = D2 -> Tx, 5 = D1 -> Rx
String strFromLora;
const int led = D6; //led connected to D6 pin
const int waterPin= A0; //water sensor data pin connected to A0, remember to power it with the Vin pin
int waterLevel; //variable that stores the water level detected by the sensor
bool ackReceived=false; //false if we do not receive a confirmation of reception from the boat device
void initialize_LoRaP2P(void);
void checkOK(bool *allOk);
bool boatListening();
void sendMOBsignal();
bool MOBAckReceived();
void ledSOS();

void setup() {
  Serial.begin(9600);
  initialize_LoRaP2P();
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
}

void loop() {
  int waterLevel=analogRead(waterPin); //read values from the water sensor
  Serial.println(waterLevel);  // See the Value In Serial Monitor
  if(waterLevel>100){ //if there is water on the sensor, the value will be over 100
    digitalWrite(led,1); //the led stays on while we try to send the Man Overboard SOS signal
    do{
      Serial.println("Waiting for the boat to open P2P comunication");
      if(boatListening()==true){ //we wait for the boat device to open P2P communication, when this happens we send the MOB signal
        delay(500);
        sendMOBsignal(); //sends the MOB SOS signal to the boat device
      }
      LoRaSerial.println("radio set wdt 1000"); //we change the watchdog time to wait less for the acknowlodgement
      strFromLora = LoRaSerial.readStringUntil('\n');
      ackReceived=MOBAckReceived(); //we check if we got the ack from the boat

      LoRaSerial.print("radio set wdt "); //we set the whatcdog timer back to the initial value (1 min)
      LoRaSerial.println(WATCHDOG);
      strFromLora = LoRaSerial.readStringUntil('\n');

    }while(ackReceived==false); //if we did not receive an ack, wait for the next time the P2P communication is initiated and try again
    digitalWrite(led,0);
    Serial.println("entering SOS mode, reset the board to exit");
    while(true){ 
      ledSOS(); //the led will continue to blink with a morse SOS pattern to make the person overboard easier to find
    } 
  }
  delay(1000); //we check the value of the water sensor every 1s
}

//this function sets all the parameters needed for LoRa P2P communication. 
//the LoRa module send an 'ok' after each command if everything worked correctly. Using allOk and checkOk, we check that the setup is done correctly
void initialize_LoRaP2P() {
  Serial.println("Initializing LoraP2P module");
  LoRaSerial.begin(57600);
  bool allOk= true;
  strFromLora = String("");
  LoRaSerial.setTimeout(1000);

  LoRaSerial.listen();
  
  LoRaSerial.println("sys get ver");
  strFromLora = LoRaSerial.readStringUntil('\n');

  LoRaSerial.println("mac pause"); //necessary before radio commands
  strFromLora = LoRaSerial.readStringUntil('\n');

  LoRaSerial.println("radio set mod lora");
  checkOK(&allOk);

  LoRaSerial.print("radio set freq "); //sets the frequency, with this frequency the duty cicle is 1%
  LoRaSerial.println(FREQ);
  checkOK(&allOk);

  LoRaSerial.println("radio set pwr 3");  //max power 14 dBm,  -3 is the min, 3 good for power saving
  checkOK(&allOk);

  LoRaSerial.println("radio set sf sf7"); //min range (we do not need more, the boat will always be closer than 2km), fast data rate (lower TOA), minimum battery impact
  checkOK(&allOk);
  
  LoRaSerial.println("radio set afcbw 41.7"); //sets the value used by the automatic frequency correction bandwidth
  checkOK(&allOk);
  
  LoRaSerial.println("radio set rxbw 125"); //Lower receiver BW equals better link budget / SNR (less noise)
  checkOK(&allOk);

  LoRaSerial.println("radio set prlen 8"); //sets the preamble lenght
  checkOK(&allOk);
  
  LoRaSerial.println("radio set crc on"); //able to correct single-bit errors and detect many multiple-bit errors
  checkOK(&allOk);
  
  LoRaSerial.println("radio set iqi off"); //we do not need the invert IQ
  checkOK(&allOk);
  
  LoRaSerial.println("radio set cr 4/5"); //every 4 useful bits are going to be encoded by 5 transmission bits
  checkOK(&allOk);
  
  LoRaSerial.print("radio set wdt "); //set the whatch dog timer, we set it to 1 minute
  LoRaSerial.println(WATCHDOG);
  checkOK(&allOk);
  
  LoRaSerial.println("radio set sync 12"); //set the sync word used
  checkOK(&allOk);
  
  //For a fixed SF, a narrower bandwidth will increase sensitivity as the bit rate is reduced
  //this means more time-on-air, so more battery consumption, but it's easier to receive
  LoRaSerial.println("radio set bw 125");
  checkOK(&allOk);

  //if checkOk never sets allOk to false, it means that the setup worked correctly
  if(allOk==true){
    Serial.println("LoraP2P setup completed correctly");
  }
  else{
    Serial.println("Error during LoraP2P setup");
  }
}

//used to check that the setup worked properly
void checkOK(bool *allOk) {
  strFromLora = LoRaSerial.readStringUntil('\n');
  if ( strFromLora.indexOf("ok") != 0 ){ //checks if the string that the Lora module sents back after we write a command is different from ok
    *allOk=false;                        //if this is the case, sets allOk to false, in this way if one or more of the steps of the setup fails, we detect it
  }
}

//waits for a message from the boat device that initialize the P2P communication and allows us to send the Man Overboard SOS
//after an rx or tx command, the LoRa module sends two strings
//the function returns true if we receive the message, false if we don't receive anything or the LoRa module has some issues
bool boatListening(){
  LoRaSerial.println("radio rx 0"); //wait to receive until the watchdogtime (1 minute)
  strFromLora = LoRaSerial.readStringUntil('\n');
  delay(20);
  if ( strFromLora.indexOf("ok") == 0 ) //check the first string to see if the parameters are correct, and we are in rx mode
  {
    strFromLora = String("");
    while(strFromLora=="") //waits for the second message that the LoRa module sends back after the rx command
    {
      strFromLora = LoRaSerial.readStringUntil('\n');
      delay(100);
      
    }
    if ( strFromLora.indexOf("radio_rx  01") == 0 )  //checking if the message we are expecting (03) is received 
    {
      Serial.println("Boat is listening, send the SOS");
      return true;
    }
    else
    {
      Serial.println("Waiting for the boat to open P2P comunication");
      return false;
    }
  }
  else
  {
    Serial.println("radio not going into receive mode"); //if the first string is not 'ok', something is wrong with the LoRa module
    return false;
  }
}

//sends the Man Overboard SOS signal
//after an rx or tx command, the LoRa module sends two strings
void sendMOBsignal() {
  Serial.println("sending MOB SOS signal to the boat");
  LoRaSerial.print("radio tx ");
  LoRaSerial.println("2"); //2 is the Man OverBoard SOS signal
  strFromLora = LoRaSerial.readStringUntil('\n');
  delay(20);
  if ( strFromLora.indexOf("ok") == 0 ) //check the first string to see if the parameters are correct, and we are in tx mode
  {
    strFromLora = String("");
    while(strFromLora=="") //waits for the second message that the LoRa module sends back after the tx command
    {
      strFromLora = LoRaSerial.readStringUntil('\n');
    }
    if ( strFromLora.indexOf("radio_tx_ok") == 0 )  //checking the second string received from the module to see if the message was sent correctly
    {
      Serial.println("message sent correctly");
    }
    else
    {
      Serial.println("transmission failed");
    }
  }
  else
  {
    Serial.println("radio not going into transmission mode"); //if the first string is not 'ok', something is wrong with the LoRa module
  }
}


//listens for an acknowledgment message from the boat that confirms that it received our SOS
//returns true if we receive the message, false if we don't receive anything or the LoRa module has some issues
bool MOBAckReceived(){
  LoRaSerial.println("radio rx 0"); //wait to receive until the watchdogtime
  strFromLora = LoRaSerial.readStringUntil('\n');
  delay(20);
  if ( strFromLora.indexOf("ok") == 0 ) //check the first string received to see if the parameters are correct, and we are in rx mode
  {
    strFromLora = String(""); //waits for the second message that the LoRa module sends back after the rx command
    while(strFromLora=="")
    {
      strFromLora = LoRaSerial.readStringUntil('\n');
      delay(100);
      
    }
    if ( strFromLora.indexOf("radio_rx  03") == 0 )  //checking if the acknowledgment message (03) is received 
    {
      Serial.println("acknowledgment message received, help is on the way!");
      return true;
    }
    else
    {
      Serial.println("Received nothing");
      return false;
    }
  }
  else
  {
    Serial.println("radio not going into receive mode"); //if the first string is not 'ok', something is wrong with the LoRa module
    return false;
  }
}

//makes the led blink as a morse SOS signal
void ledSOS(){
  digitalWrite(led,1);
  delay(1000);
  digitalWrite(led,0);
  delay(1000);
  digitalWrite(led,1);
  delay(1000);
  digitalWrite(led,0);
  delay(1000);
  digitalWrite(led,1);
  delay(1000);
  digitalWrite(led,0);
  delay(1000);

  digitalWrite(led,1);
  delay(2000);
  digitalWrite(led,0);
  delay(1000);
  digitalWrite(led,1);
  delay(2000);
  digitalWrite(led,0);
  delay(1000);
  digitalWrite(led,1);
  delay(2000);
  digitalWrite(led,0);
  delay(1000);

  digitalWrite(led,1);
  delay(1000);
  digitalWrite(led,0);
  delay(1000);
  digitalWrite(led,1);
  delay(1000);
  digitalWrite(led,0);
  delay(1000);
  digitalWrite(led,1);
  delay(1000);
  digitalWrite(led,0);
  delay(4000);
}