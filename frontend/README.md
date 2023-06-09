# Boat safety device

## Description
Project developed for 34346 Networking technologies and application development for Internet of Things (IoT) Spring 23 course. 
The web application 

## How to run
Project developed in React 18. 
Prerequisites:
* [Node](https://nodejs.org/en)
* [Yarn](https://yarnpkg.com/) package manager 
* The Things Stack application and API key (https://www.thethingsindustries.com/docs/integrations/cloud-integrations/azure-iot-hub/deployment/)
* Google Maps API key (https://developers.google.com/maps/documentation/javascript/get-api-key)

After cloning the repo, create a file src/secrets.js defining your THINGS_STACK_API_KEY and GOOGLE_MAPS_API_KEY

In the project directory, you can run:

`yarn` - installs node modules.

`yarn start`- runs the app in the development mode.\
Open [http://localhost:3000](http://localhost:3000) to view it in the browser.


## Functionalities
### Uplink data
Boat safety device is a single page web application presenting data gathered from the sensors on boat. Sensors data is sent to [The Thing Stack](https://www.thethingsindustries.com/stack/#:~:text=The%20Things%20Stack%20is%20a,gateways%20and%20end%2Duser%20applications.) via LoraWan and then the data is fetched using HTTP GET method. The request is done every minute.

The data contains:
- GPS coordinates and location marked on the map
- Water choppiness level
- Battery level

**Data flow** \
![web_app_GET](https://user-images.githubusercontent.com/38289305/236691170-5763a3ef-486a-46b8-883b-fa7089193cc8.png)


### Downlink messages
The application provides a functionality to send downlink message to the boat.
This is done using HTTP POST method. The user can select from the four fiven messages:
- Come in
- Storm warning
- Wind changing
- Boat clearance 

**Data flow** \
![web_app_POST](https://user-images.githubusercontent.com/38289305/236691177-b6139823-e158-4e44-bb86-c18f18b23263.png)


