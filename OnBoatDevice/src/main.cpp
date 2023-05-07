#include <Arduino.h>
#include <rn2xx3.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Adafruit_MPU6050.h> // Accelometer
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RESET 15
#define FREQ 866000000 //this frequency allows a duty cicle of 1%
#define WATCHDOG 1000
#define i2c_Address 0x27

SoftwareSerial GPSSerial(0, 2);  //GPS module connections2 = 3 = D3 -> Tx, D4 -> Rx
SoftwareSerial LoRaP2PSerial(3, 13); //LoraP2P module connections: Rx -> Tx, D7 -> Rx REMOVE RX TO ALLOW UPLOAD TO THE BOARD!!!
SoftwareSerial LoRaSerial(14, 12); //LoraWAN module connections: D5 -> Tx, D6 -> Rx

// DEFINE LORA CONNECTION KEYS
String App_EUI = "";
String Dev_EUI = "";
String App_Key = "";

String recv = "";
uint8_t tx[9];
String txStr;
uint8_t *pla;
uint8_t *plo;
float longitude, latitude;
int batteryLevel = 3;
TinyGPSPlus gps;
String strFromLora;
int sosSignal = 0;
String downlink="";
bool messageReceived=false;
//constants Adafruit_MPU6050 - ROTATION 

long const BAUD_RATE = 9600;
int const MAX_LEVEL = 7;
float const SENSITIVITY = 6;      //higher the value, easier it is to reach higher choppiness waveLevel
float const X_OFFSET = 0.83;      //eliminating gravitational acceleration
float const Y_OFFSET = -0.02;     //eliminating other constantly present acceleration
float const Z_OFFSET = 0.00;      //no constant 
// float NOISE_CONST = 0.10;  //eliminates small movements due to shaking hand etc

Adafruit_MPU6050 mpu;
float totalX = 0;
float totalY = 0;
float totalZ = 0;
int count = 0;            //number of measurments since last result
int waveLevel = 0;
long lastDisplayTime;     
long period = 20000;       //currently gives result every 3 sec, 30-60 sec would be more reasonable IRL

const int sosButton = 16;  // D0

// LCD params
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(i2c_Address, lcdColumns, lcdRows);

//FUNCTION INITS
bool manOverBoard = false;
void initialize_LoRaP2P();
void initialize_LoRaWAN();
void checkOK(bool *allOk);
void openP2PCommunication();
void sendMOBAcknowledgement(void);
bool MOBreceived(void);
void handleGPS();
void GPSLoop();
void locationDisplayHandler(float lat, float lng);
void messagesHandler();
void sosSignalDisplay(int isOn);
void displayBatteryAndWaveLevel();
void sosSignalHandler();
void displaySetup();
void initGY521();
void measureRotation();
void displayRotation(float x, float y, float z);


//String bytearray_to_string(uint8_t *tx);
String bytearray_to_base64(uint8_t* data, size_t len);
void construct_payload(float x, float y, int sos, bool mob, int acc, int bat);
//create an instance of the rn2xx3 library,
//giving the software UART as stream to use,
//and using LoRa WAN
rn2xx3 myLora(LoRaSerial);

// the setup routine runs once when you press reset:
void setup() {
  // LED pin is GPIO2 which is the ESP8266's built in LED
  pinMode(2, OUTPUT);
  pinMode(A0, INPUT);
  delay(5000);

  // Open serial communications and wait for port to open:
  //Be aware that all other serial communcations are only opened when needed, because
  //When all serial connections were opened at the same time, there would be a thread overload
  Serial.begin(9600);
  displaySetup();
  digitalWrite(sosButton, LOW);
  pinMode(sosButton, INPUT);  // SOS setup
  Serial.print("button level: ");
  Serial.println(digitalRead(sosButton));
  initGY521();                // Accelometer setup should be here **********

  delay(1000); //wait for the arduino ide's serial console to open
  initialize_LoRaWAN();
  initialize_LoRaP2P();
  LoRaP2PSerial.end();
  Serial.println("Startup");
  delay(2000);
}

void loop() {
  measureRotation();
  measureBattery();
  //LoRa P2P communication
  openP2PCommunication(); //send a message that allows the other device to send the SOS signal if it is needed
  manOverBoard=MOBreceived(); //waits 1s for the sos signal. If it receives it, sends back an acknowledgement
  Serial.println("P2P Communication closed");
  //end of P2P communication
  measureRotation();
  sosSignalHandler();
  Serial.println(sosSignal);
  measureRotation();

  float millisBefore = millis();
  GPSSerial.begin(9600);
  GPSSerial.listen();
  while(millis() - millisBefore < 10000){
    //Serial.print("GPS");
    GPSLoop();
  }
  measureRotation();
  delay(1000);
  measureRotation();
  GPSSerial.end();
  if(!messageReceived){
    displaySensorsData();
  }
  delay(10000); 

  Serial.println("TXing");
  construct_payload(latitude, longitude, sosSignal, manOverBoard, waveLevel, 2);
  txStr = bytearray_to_base64(tx, 9);
  
  //Write Hex to serial for debug purposes
  LoRaSerial.begin(57600);
  myLora.tx(txStr); //one byte, blocking function
  recv = myLora.getRx();
  Serial.print("received message ");
  Serial.println(recv);
  if(recv != ""){
    downlink = recv;
  }
  Serial.print("downlink message ");
  Serial.println(downlink);
  messagesHandler();
  LoRaSerial.end();
  measureRotation();
  delay(1000);
  measureRotation();
  delay(1000);
  measureRotation();
  delay(1000);  
}


/*
:param x: float - latitude coordinate
:param y: float - longitude coordinate
:param sos: int - 0-3
:param acc: int - 0-7
:param bat: int - 0-4
*/
void construct_payload(float x, float y, int sos, bool mob, int acc, int bat) {
  uint8_t *pla;
  uint8_t *plo;

  uint8_t extra = 0;
  extra = extra | sos;
  if (mob) {
    extra = extra | (1 << 1);
  } else {
    extra = extra | (0 << 1);
  }
  extra = extra | (acc << 2);
  extra = extra | (bat << 5);
  pla = (uint8_t *)&x;
  plo = (uint8_t *)&y;

  for (uint i = 0; i < sizeof(x); i++) {
    tx[i] = pla[i];
  }

  for (uint i = 0; i < (sizeof(y)); i++) {
    tx[i + 4] = plo[i];
  }
  tx[8] = extra;
  tx[9] = 0x00;
}

// Convert a byte array to a base64-encoded string
String bytearray_to_base64(uint8_t* data, size_t len) {
    static const String base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    String ret;
    int i = 0;
    int j = 0;
    uint8_t char_array_3[3];
    uint8_t char_array_4[4];

    while (len--) {
        char_array_3[i++] = *(data++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) +
                              ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) +
                              ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++) {
                ret += base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++) {
            char_array_3[j] = 0;
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) +
                          ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) +
                          ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < i + 1; j++) {
            ret += base64_chars[char_array_4[j]];
        }

        while (i++ < 3) {
            ret += '=';
        }
    }

    return ret;
}


/* ###################################################################################################### */

void initialize_LoRaWAN()
{
  LoRaSerial.begin(57600);
  //reset RN2xx3
  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);
  delay(1000);
  digitalWrite(D3, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  LoRaSerial.flush();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
    //reset RN2xx3
    pinMode(D3, OUTPUT);
    digitalWrite(D3, LOW);
    delay(1000);
    digitalWrite(D3, HIGH);
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(hweui);
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("- Trying to join TTN");
  bool join_result = false;
  join_result = myLora.initOTAA(App_EUI, App_Key, Dev_EUI);

  while(!join_result)
  {
    Serial.println("- No Connection Established");
    delay(1000); //delay 5s before retry
    join_result = myLora.initOTAA(App_EUI, App_Key, Dev_EUI);
  }
  
  Serial.println("- Successfully joined TTN");
  LoRaSerial.end();

}

//this function sets all the parameters needed for LoRa P2P communication. Using allOk and checkOk, we check that the setup is done correctly
void initialize_LoRaP2P() {
  LoRaP2PSerial.begin(57600);
  Serial.println("Initializing LoraP2P module");
  bool allOk= true; //this variable will detect if any step of the setup will fail 
  strFromLora = String(""); //string used to store the strings that the LoRa module prints
  LoRaP2PSerial.setTimeout(1000);

  LoRaP2PSerial.listen();
  
  LoRaP2PSerial.println("sys get ver");
  strFromLora = LoRaP2PSerial.readStringUntil('\n');

  LoRaP2PSerial.println("mac pause"); //necessary before radio commands
  strFromLora = LoRaP2PSerial.readStringUntil('\n');

  LoRaP2PSerial.println("radio set mod lora");
  checkOK(&allOk);

  LoRaP2PSerial.print("radio set freq "); //sets the frequency, with this frequency the duty cicle is 1%
  LoRaP2PSerial.println(FREQ);
  checkOK(&allOk);

  LoRaP2PSerial.println("radio set pwr 3");  //max power 14 dBm,  -3 is the min, 3 good for power saving
  checkOK(&allOk);

  LoRaP2PSerial.println("radio set sf sf7"); //min range (we do not need more, the boat will always be closer than 2km), fast data rate (lower TOA), minimum battery impact
  checkOK(&allOk);
  
  LoRaP2PSerial.println("radio set afcbw 41.7"); //sets the value used by the automatic frequency correction bandwidth
  checkOK(&allOk);
  
  LoRaP2PSerial.println("radio set rxbw 125"); //Lower receiver BW equals better link budget / SNR (less noise)
  checkOK(&allOk);

  LoRaP2PSerial.println("radio set prlen 8"); //sets the preamble lenght
  checkOK(&allOk);
  
  LoRaP2PSerial.println("radio set crc on"); //able to correct single-bit errors and detect many multiple-bit errors
  checkOK(&allOk);
  
  LoRaP2PSerial.println("radio set iqi off"); //we do not need the invert IQ
  checkOK(&allOk);
  
  LoRaP2PSerial.println("radio set cr 4/5"); //every 4 useful bits are going to be encoded by 5, transmission bits
  checkOK(&allOk);
  
  LoRaP2PSerial.print("radio set wdt "); //set the whatch dog timer
  LoRaP2PSerial.println(WATCHDOG);
  checkOK(&allOk);
  
  LoRaP2PSerial.println("radio set sync 12"); //set the sync word used
  checkOK(&allOk);
  
  //For a fixed SF, a narrower bandwidth will increase sensitivity as the bit rate is reduced
  //this means more time-on-air, so more battery consumption, but it's easier to receive
  LoRaP2PSerial.println("radio set bw 125");
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
  strFromLora = LoRaP2PSerial.readStringUntil('\n');
  if ( strFromLora.indexOf("ok") != 0 ){ //checks if the string that the Lora module sents back after we write a command is different from ok
    *allOk=false;                        //if this is the case, sets allOk to false, in this way if one or more of the steps of the setup fails, we detect it
  }
}

//sends message to let the other device know that the boat is listening, allows P2P communciation
//after an rx or tx command, the LoRa module sends two strings
void openP2PCommunication() {
  LoRaP2PSerial.begin(57600);
  Serial.println("Opening P2P communication");
  LoRaP2PSerial.print("radio tx ");
  LoRaP2PSerial.println("1"); //1 is the signal taht we are sending
  strFromLora = LoRaP2PSerial.readStringUntil('\n');
  delay(20);
  if ( strFromLora.indexOf("ok") == 0 ) //check the first string to see if the parameters are correct, and we are in tx mode
  {
    strFromLora = String("");
    while(strFromLora=="") //waits for the second message that the LoRa module sends back after the tx command
    {
      strFromLora = LoRaP2PSerial.readStringUntil('\n');
      delay(100);
    }
    if ( strFromLora.indexOf("radio_tx_ok") == 0 )  //checking if the message was sent correctly
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

//sends a confirmation to the man overboard (MOB) that we received the SOS signal
//after an rx or tx command, the LoRa module sends two strings
void sendMOBAcknowledgement() {
  Serial.println("sending acknowledgement to the man overboard");
  LoRaP2PSerial.print("radio tx ");
  LoRaP2PSerial.println("3"); //3 is the MOB acknowledgment signal
  strFromLora = LoRaP2PSerial.readStringUntil('\n');
  delay(200);
  if ( strFromLora.indexOf("ok") == 0 ) //check the first string to see if the parameters are correct, and we are in tx mode
  {
    strFromLora = String(""); 
    while(strFromLora=="") //waits for the second message that the LoRa module sends back after the tx command
    {
      strFromLora = LoRaP2PSerial.readStringUntil('\n');
      delay(100);
    }
    if ( strFromLora.indexOf("radio_tx_ok") == 0 )  //checking the second string to see if the message was sent correctly
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

//listens for a Man OverBoard message, sends back a confirmation if it's received
//after an rx or tx command, the LoRa module sends two strings
//the fucntion returns true if we receive a Man Overboard (MOB) SOS signal, false if we receive nothing
bool MOBreceived(){
  LoRaP2PSerial.println("radio rx 0"); //wait to receive until the watchdogtime
  strFromLora = LoRaP2PSerial.readStringUntil('\n');
  delay(200);
  if ( strFromLora.indexOf("ok") == 0 ) //check the first string to seeif the parameters are correct, and we are in rx mode
  {
    strFromLora = String("");
    while(strFromLora=="") //waits for the second message that the LoRa module sends back after the tx command
    {
      strFromLora = LoRaP2PSerial.readStringUntil('\n');
      delay(100);
    }
    if ( strFromLora.indexOf("radio_rx  02") == 0 )  //checking if the Man OverBoard message (01) is received in the second string
    {
      Serial.println("MOB message received");
      delay(500);
      sendMOBAcknowledgement(); //if we get a MOB message, we send back an acknowledgement
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
  LoRaP2PSerial.end();
}

void handleGPS()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    Serial.print(latitude, 6);
    Serial.print(F(","));
    Serial.print(longitude, 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();  
  }

  void GPSLoop(){
    while (GPSSerial.available() > 0){
      yield();
      if (gps.encode(GPSSerial.read())){
        handleGPS();
      }
    }

    if (millis() > 5000 && gps.charsProcessed() < 10){
      yield();
      Serial.println(F("No GPS detected: check wiring."));
    }
  }

void displaySetup() {
  delay(250);
  lcd.init();                    
  lcd.backlight();
}

void sosSignalHandler() {
  int sosButtonPressed = digitalRead(sosButton);
  if (sosButtonPressed == HIGH) {
    sosSignal = !sosSignal;
    Serial.print("SOS turned ");
    if(sosSignal){
      lcd.print("Sending SOS");
    } else {
      lcd.print("SOS cancelled");
    }
    Serial.println(sosSignal);
    delay(500);
  }
}

void displaySensorsData() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waves: ");
  lcd.setCursor(6, 0);
  lcd.print(waveLevel);
  lcd.setCursor(8, 0);
  lcd.print("Bat: ");
  lcd.setCursor(14, 0);
  lcd.print(batteryLevel + 1);
  lcd.setCursor(0, 1);
  lcd.print("GPS: ");
  lcd.setCursor(7, 1);
  lcd.print(latitude);
  lcd.setCursor(12, 1);
  lcd.print(longitude);
}

void measureBattery(){
  int batteryInput  = analogRead(A0); // Read the analog  value 
  batteryLevel = batteryInput * (3 / 1024.0);
  Serial.print("Voltage: ");
  Serial.print(batteryInput * (4.2 / 1024.0));
  Serial.println(" V");
}

void messagesHandler() {
  lcd.clear();
  lcd.setCursor(0, 0);  // set cursor to first column, first row
  if(downlink.indexOf("00") == 0) {
      lcd.print("! GO BACK !");
      messageReceived = true;
  }
  else if(downlink.indexOf("01") == 0){
    lcd.print("! STORM ALERT !");
    messageReceived = true;
  }
  else if(downlink.indexOf("02") == 0){
    lcd.print("! STRONG WIND !");
    messageReceived = true;
  }
  else if(downlink.indexOf("03") == 0){
    lcd.print("!BOAT CLEARANCE!");
    messageReceived = true;
  }
  else{
    lcd.print("NO MESSAGES");
    messageReceived = false;
  }
}

void initGY521(){
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    return;
  }
  Serial.println("MPU6050 Found!");

  //sensor setup
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
  lastDisplayTime = millis();
}

void measureRotation(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  count++;
  totalX += abs(g.gyro.x - X_OFFSET);
  totalY +=  abs(g.gyro.y - Y_OFFSET);
  totalZ += abs(g.gyro.z - Z_OFFSET);
  
  long currentTime = millis();
  if(currentTime - lastDisplayTime > period){
    waveLevel = SENSITIVITY * (totalX + totalY + totalZ) / count;
    waveLevel = waveLevel <= MAX_LEVEL ? waveLevel : MAX_LEVEL;   //caps waveLevel at 7(or whatever is given) (anything above is completely unreasonable for water movement)
    displayRotation(totalX / count, totalY / count, totalZ / count);  //displays data for testing
    
    // reseting values
    lastDisplayTime = currentTime;
    totalX = 0;
    totalY = 0;
    totalZ = 0;
    count = 0;
  }
}

void displayRotation(float x, float y, float z){
  Serial.print("LEVEL: ");
  Serial.print(waveLevel);
  Serial.print(" (");
  Serial.print(6 * (x + y + z));
  Serial.print(")   |   Rotation X: ");
  Serial.print(x);
  Serial.print(", Y: ");
  Serial.print(y);
  Serial.print(", Z: ");
  Serial.print(z);
  Serial.println(" rad/s");
}