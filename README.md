# Sailing Safety Device - IoT34346
## Contributors:
Niels Frederik Flemming Holm Frandsen
Natalie Valett
Adam Peter Balogh
Karolina Jadwiga Jablonska
Marco Persico

## Description
This sailing safety project was developed for sailing clubs and harbors who could benefit from a line of communication between the harbor and boats on the water. Our hope is that this communication channel will allow harbor staff to respond to sailing hazards like capsizing, drowning, and collisions much faster and more effectively. The on-boat device and man overboard devices allow for sailors to send SOS and man overboard alerts to the harbor, as well as passively transmitting crucial data like location and water choppiness to the harbor. Communication from the harbor to the boats is facilitated by sending a message on the web app, which will be displayed on the LCD on the boat.

## Hardware Requirements
This repo consists of software for the on-boat device, man-overboard device, and a backend service and user web interface. The on-boat device consists of a NodeMCU microcontroller, an accelerometer & gyroscope GY-521, WAVGAT GPS module, SOS button, and an LCD display for displaying sensor data and downlink messages. For communication with the backend and with the man overboard device there is a LoRaWAN module and LoRa P2P module. 
The man overboard device consists of a water level sensor SE045 to detect if a sailor has fallen overboard, and a LoRa P2P module to transmit the signal to the on-boat device. 

## Software Requirements
To replicate the on-boat device functionality, App EUI, Dev EUI, and App Keys need to be obtained from LoRa, and key values must be specified in /OnBoatDevice/src/main.cpp
For information on replicating and running the web app, see frontend/README.md