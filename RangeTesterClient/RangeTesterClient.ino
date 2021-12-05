/*
  Repeating WiFi Web Client

 This sketch connects to a a web server and makes a request
 using a WiFi equipped Arduino board.

 created 23 April 2012
 modified 31 May 2012
 by Tom Igoe
 modified 13 Jan 2014
 by Federico Vanzati

 http://www.arduino.cc/en/Tutorial/WifiWebClientRepeating
 This code is in the public domain.
 */

#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
int count=0;

int PIN =  12;  // D12
int POUT =  11; // D11
int PINState = 0;        
int lastPINState = 0; 

int led =  LED_BUILTIN;
int ledStatus = 1;

// Initialize the WiFi client library
WiFiClient client;

// server address:
char server[] = "192.168.4.1";
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 3L * 1000L; // delay between updates, in milliseconds

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  */

  pinMode(led, OUTPUT);      // set the LED pin mode
  pinMode(PIN, INPUT);
  pinMode(POUT, OUTPUT);

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

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  //LEDOn(1);
  
  // you're connected now, so print out the status:
  printWifiStatus();

  if (client.connect(server, 80)) {
    Serial.println("Connected to server ...");
  } else {
    Serial.println("Failed to connect to server.");
  }
}

void loop() {
  /* Check whether WiFi is connected */
  // attempt to connect to WiFi network:
  if (WL_CONNECTED != WiFi.status()) {
    //LEDOn(0);
    while ( WL_CONNECTED != WiFi.status()) {
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
      status = WiFi.begin(ssid, pass);
  
      // wait 10 seconds for connection:
      delay(10000);
    }
    //LEDOn(1);
  }
  
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  /*
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  */

  /* If client is disconnected, reconnect to server */
  if (!client.connected()) {
      // close any connection before send a new request.
      // This will free the socket on the NINA module
      client.stop();
      if (client.connect(server, 80)) {
        Serial.println("Connected to server ...");
      } else {
        Serial.println("Failed to connect to server.");
      }
  }
  else {
    /*
    PINState = digitalRead(PIN);
    if(PINState != lastPINState) {
      if(PINState==1) {
          client.println("H");
      }
      else {
          client.println("L");
      }
      Serial.println("Input PIN value changed:");
      Serial.println(PINState);  
      lastPINState = PINState;  
    }
    */
    if (count % 2 == 0) {
      client.write('L');
    } else {
      client.write('H');
    }
  
    // Check whether the server has sent any data
    if (client.available()) {
      char c = client.read();
      Serial.print("Received input: [");
      Serial.write(c);
      Serial.println("]");
  
      // Check to see if the client request was "GET /H" or "GET /L":
      if (c == 'H') {
        LEDOn(1);
        digitalWrite(POUT, HIGH);
      }
      if (c == 'L') {
        LEDOn(0);
        digitalWrite(POUT, LOW);
      }
    }
  }
  count+=1;
  delay(1000);
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

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
