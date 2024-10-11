#include <ESP8266WiFi.h> // Include the WiFi library
#include <WiFiUdp.h> // Include the UDP library
#include <NTPClient.h> // Include the NTP library to set RTC
#include <RTClib.h> // Include the library for DS3231 module
//"Assisted by Microsoft Copilot"

// Begin
// Replace with your network credentials
const char* ssid = "YourWiFiAPN";
const char* password = "YourWiFiPassword";
const long utcOffsetInSeconds = 0;  // 0 is UTC for NTP Server

// Configure NTPClient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.windows.com", utcOffsetInSeconds);

// Define delay for NTPClient update
unsigned long previousMillis = 0;
const unsigned long interval = 3600000;  // 1 hour in milliseconds

// Configure localPort
WiFiUDP udp;
unsigned int localPort = 123;  // Local port to listen for UDP packets

// Configure SDL SDA for DS3231
const int D0 = 0; // SDL
const int D2 = 2; // SDA

// Configure DS3231
RTC_DS3231 rtc;

//Setup
void setup() {
// Initialize I2C communication
Wire.begin(D2, D0);  // SDA, SCL

// Enable Serial
Serial.begin(115200);

// Enable WiFi
WiFi.begin(ssid, password);

// Output MOTD (Message of the Day)
Serial.println("");
Serial.println("");
Serial.println("########");
Serial.println("");
Serial.println("Begin RTC Server.");

// Connect to WiFi
while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.println("Connecting to WiFi...");
}

Serial.println("Connected to WiFi");
Serial.print("IP Address: ");
Serial.println(WiFi.localIP());

// Enable NTPClient
timeClient.begin();

// Enable RTC DS3231
if (!rtc.begin()) {
Serial.println("Couldn't find RTC");
while (1);
}

// Check if RTC lost power and set the time if necessary
//if (rtc.lostPower()) {
//Serial.println("RTC lost power, setting the time!");
setTimeFromNTP();
//}

// Begin listening on localPort
udp.begin(localPort);
}


void setTimeFromNTP() {
if (timeClient.update()) {
// Get the current epoch time from NTP server
unsigned long epochTime = timeClient.getEpochTime();
// Adjust the RTC with the current time
rtc.adjust(DateTime(epochTime));

// Print the current time for debugging
Serial.print("Current time: ");
Serial.println(epochTime);
DateTime now = rtc.now();
Serial.print("Date & Time: ");
Serial.print(now.year(), DEC);
Serial.print('/');
Serial.print(now.month(), DEC);
Serial.print('/');
Serial.print(now.day(), DEC);
Serial.print(" ");
Serial.print(now.hour(), DEC);
Serial.print(':');
Serial.print(now.minute(), DEC);
Serial.print(':');
Serial.println(now.second(), DEC);
} else {
// Output error message if unable to update from NTP server
Serial.println("Failed to update time from NTP server.");
while (1);
}
}

// Main
void loop() {
// Get current millis
unsigned long currentMillis = millis();

// Check if it's time to update from NTP server again
if (currentMillis - previousMillis >= interval) {
previousMillis = currentMillis;
setTimeFromNTP();
}

// Wait for NTP request
int packetSize = udp.parsePacket();
if (packetSize) {
// Output IP of NTP request
Serial.print("Received NTP request from IP: ");
Serial.println(udp.remoteIP());

// Import packet
byte packetBuffer[48];
udp.read(packetBuffer, 48);

// Print the received packet for debugging
Serial.println("Received UDP packet:");
for (int i = 0; i < 48; i++) {
Serial.print(packetBuffer[i], HEX);
Serial.print(" ");
}
Serial.println();

// Get current RTC DS3231 time
unsigned long currentTime = rtc.now().unixtime();
unsigned long secsSince1900 = currentTime + 2208988800UL - utcOffsetInSeconds;

// Prepare the response packet
packetBuffer[0] = 0b00100100;  // LI, Version, Mode
packetBuffer[1] = 1;           // Stratum, or type of clock
packetBuffer[2] = 0;           // Polling Interval
packetBuffer[3] = 0xEC;        // Peer Clock Precision
memset(packetBuffer + 4, 0, 8); // Root Delay & Root Dispersion
memset(packetBuffer + 12, 0, 4); // Reference Identifier

// Reference Timestamp
packetBuffer[16] = (byte)(secsSince1900 >> 24);
packetBuffer[17] = (byte)(secsSince1900 >> 16);
packetBuffer[18] = (byte)(secsSince1900 >> 8);
packetBuffer[19] = (byte)(secsSince1900);
memset(packetBuffer + 20, 0, 4);

// Originate Timestamp (copied from request)
memcpy(packetBuffer + 24, packetBuffer + 40, 8);

// Receive Timestamp
packetBuffer[32] = (byte)(secsSince1900 >> 24);
packetBuffer[33] = (byte)(secsSince1900 >> 16);
packetBuffer[34] = (byte)(secsSince1900 >> 8);
packetBuffer[35] = (byte)(secsSince1900);
memset(packetBuffer + 36, 0, 4);

// Transmit Timestamp
packetBuffer[40] = (byte)(secsSince1900 >> 24);
packetBuffer[41] = (byte)(secsSince1900 >> 16);
packetBuffer[42] = (byte)(secsSince1900 >> 8);
packetBuffer[43] = (byte)(secsSince1900);
memset(packetBuffer + 44, 0, 4);

// Print the response packet for debugging
Serial.println("Response UDP packet:");
for (int i = 0; i < 48; i++) {
Serial.print(packetBuffer[i], HEX);
Serial.print(" ");
}
Serial.println();

// Send the response
udp.beginPacket(udp.remoteIP(), udp.remotePort());
udp.write(packetBuffer, 48);
udp.endPacket();
}
}
