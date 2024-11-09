# NTP-Server-for-ESP8266
ESP8266 NTP Server with DS3231 RTC
This program sets up an ESP8266 microcontroller to act as an NTP server, using a DS3231 RTC module to maintain accurate time. The ESP8266 connects to a WiFi network, synchronizes time with an NTP server, and responds to NTP requests with the current time.

## Recommended Configuration
Set the NTP Server IP to be the local subnet broadcast address for example 192.168.1.255/24 

Configure your NTP Client configuration to make the NTP request to that address. In reality you can have many standard network service running.

## Features
-  WiFi Connectivity: Connects to a specified WiFi network.

-  NTP Client: Synchronizes time with an NTP server.

-  RTC Module: Uses a DS3231 RTC module to keep track of time.

-  NTP Server: Listens for NTP requests and responds with the current time.

## Setup
1. Network Credentials: Replace YourWiFiAPN and YourWiFiPassword with your WiFi network's SSID and password.
2. I2C Pins: Configure the I2C pins for the DS3231 module (SDA and SCL).
3. *If necessary*: uncomment the three lines to make the fuction Check if RTC lost power and set the time if necessary otherwise it check every time the program starts.

## How It Works
1. Setup:
    -  Initializes I2C communication for the DS3231 RTC module.

    -  Connects to the specified WiFi network.

    -  Initializes the NTP client and sets the time from the NTP server at startup.

    -  Begins listening for UDP packets on the specified local port.

2. Main Loop:
    -  Periodically updates the time from the NTP server every hour.

    -  Listens for incoming NTP requests and responds with the current time from the RTC module.

3.  Time Synchronization:
    -  The setTimeFromNTP() function updates the RTC module with the current time from the NTP server.

    -  If the NTP update fails, an error message is printed, and the program restarts.

4. NTP Request Handling:
    -  When an NTP request is received, the program reads the request, prepares a response with the current time, and sends it back to the requester.

Programmer Manager: Professor. Damian A. James Williamson Grad.  
"Assisted by Microsoft Copilot"
  
