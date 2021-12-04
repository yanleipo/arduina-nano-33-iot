/*
  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.
  This sketch will create a new access point (with no password).
  It will then launch a new server and print out the IP address
  to the Serial Monitor. From there, you can open that address in a web browser
  to turn on and off the LED on pin 13.

  If the IP address of your board is yourAddress:
    http://yourAddress/H turns the LED on
    http://yourAddress/L turns it off

  created 25 Nov 2012
  by Tom Igoe
  adapted to WiFi AP by Adafruit
 */

#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key index number (needed only for WEP)

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

int PIN =  12;  // D12
int POUT =  11; // D11
int PINState = 0;        
int lastPINState = 0; 

int ledStatus = 1;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  */
  
  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);      // set the LED pin mode
  pinMode(PIN, INPUT);
  pinMode(POUT, OUTPUT);

  // turn off LED as no client connected yet
  LEDOn(0);
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}


void loop() {
  // Check whether there are client conncted to the AP and set LED accordingly.
  // Note: The WiFi.status() API seems to have a bug. Even after client is powered off, the status will not change. 
  // There is currently no way to find whether a client is connected.
  if (status != WiFi.status()) {
      status = WiFi.status();
      if(status == WL_AP_CONNECTED) {
        // a device has connected to the AP
        Serial.println("Device connected to AP");
        LEDOn(1);
      } else {
        // a device has disconnected from the AP, and we are back in listening mode
        Serial.println("Device disconnected from AP");
        LEDOn(0);
      } 
  }

  /* Detect whether there is a PIN change. If so, send to all clients */
  PINState = digitalRead(PIN);
  if(PINState != lastPINState) {
    if(PINState==1) {
        server.write('H');
    }
    else {
        server.write('L');
    }
    Serial.println("Input PIN value changed:");
    Serial.println(PINState); 
    lastPINState = PINState;   
  }
  
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    
    Serial.println("Check whether client sent data");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    
    if (client.available()) {             // if there's bytes to read from the client,
      char c = client.read();             // read a byte, then
      Serial.print("Received input: [");
      Serial.write(c);                    // print it out the serial monitor
      Serial.println("]");

      // Check to see if the client request was "GET /H" or "GET /L":
      if (c == 'H') {
        //digitalWrite(led, HIGH);               // GET /H turns the LED on
        digitalWrite(POUT, HIGH);
      }
      if (c == 'L') {
        //digitalWrite(led, LOW);                // GET /L turns the LED off
        digitalWrite(POUT, LOW);
      }
    }
  }
  
  delay(500);
}

void listWiFiStatus() 
{
  Serial.print("WL_CONNECTED: ");
  Serial.println(WL_CONNECTED);
  Serial.print("WL_AP_CONNECTED: ");
  Serial.println(WL_AP_CONNECTED);
  Serial.print("WL_AP_LISTENING: ");
  Serial.println(WL_AP_LISTENING);
  Serial.print("WL_NO_SHIELD: ");
  Serial.println(WL_NO_SHIELD);
  Serial.print("WL_NO_MODULE: ");
  Serial.println(WL_NO_MODULE);
  Serial.print("WL_IDLE_STATUS: ");
  Serial.println(WL_IDLE_STATUS);
  Serial.print("WL_NO_SSID_AVAIL: ");
  Serial.println(WL_NO_SSID_AVAIL);
  Serial.print("WL_SCAN_COMPLETED: ");
  Serial.println(WL_SCAN_COMPLETED);
  Serial.print("WL_CONNECT_FAILED: ");
  Serial.println(WL_CONNECT_FAILED);
  Serial.print("WL_CONNECTION_LOST: ");
  Serial.println(WL_CONNECTION_LOST);
  Serial.print("WL_DISCONNECTED: ");
  Serial.println(WL_DISCONNECTED);
}
  
void LEDOn(int On)
{
  // Only change the LED when there is a difference and set it according to input
  if(ledStatus == 1 && On == 0 ) {
    digitalWrite(led, LOW); 
    ledStatus = 0;
  } 
  else if( ledStatus == 0 && On == 1) {
    digitalWrite(led, HIGH); 
    ledStatus = 1;
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}
