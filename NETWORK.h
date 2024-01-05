
// --------------------- NETWORK SETINGS ---------------------
#define WiFiname "aalto open"                                               //This is the name of the WiFi
#define WiFipw ""                                                           //This is the password of the WiFi
#define NEPPI_SOCKET "wss://elecdesign.org.aalto.fi/neppi/ws/hex/"           //This is the URL of the Websocket

//This is the https certificate for the Websocket
#define NEPPI_CERTIFICATE  \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw\n" \
  "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
  "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw\n" \
  "WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n" \
  "RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n" \
  "AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP\n" \
  "R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx\n" \
  "sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm\n" \
  "NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg\n" \
  "Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG\n" \
  "/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC\n" \
  "AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB\n" \
  "Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA\n" \
  "FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw\n" \
  "AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw\n" \
  "Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB\n" \
  "gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W\n" \
  "PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl\n" \
  "ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz\n" \
  "CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm\n" \
  "lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4\n" \
  "avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2\n" \
  "yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O\n" \
  "yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids\n" \
  "hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+\n" \
  "HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv\n" \
  "MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX\n" \
  "nLRbwHOoq7hHwg==\n" \
  "-----END CERTIFICATE-----\n" \

// -----------------------------------------------------------

#include <WiFi.h>
#include <ArduinoWebsockets.h>

using namespace websockets;
WebsocketsClient client;

bool network_connection = false; bool game_started = false;

int network_LED = 12; // LED-PIN for network indicator (blinking every 1000ms -> Wifi contection, 'OFF' no wifi- or socket conection, 'on' and blinking every 100ms sending 'ping')
bool led_12_state = false;

int current_gate = 0;
long gate_1_last_activated = 0; long gate_2_last_activated = 0;
// int button_1_state = 0; int button_2_state = 0;

long pingTimer = 0; long pollTimer = 0;                             // Timer for sending 'ping' every 10s to NodeRed and poll every 100ms 

//This is the https certificate for the Websocket
const char* ssid = WiFiname;
const char* password = WiFipw;
const char* websockets_connection_string = NEPPI_SOCKET;
const char echo_org_ssl_ca_cert[] PROGMEM = NEPPI_CERTIFICATE;

void vote(int result);

// Blinks once with the Network LED (GIPIO_12) at "delayTime" interval
void blink_Network_LED(int delayTime) {
  digitalWrite(network_LED, HIGH);
  delay(delayTime);
  digitalWrite(network_LED, LOW);
}

// Restart all parameters
void restart_game() {
  game_started = false;
  current_gate = 0;
}

//This two functions are used to react to incoming messages and events
void onMessageCallback(WebsocketsMessage message) { 
  String msg = message.data();
  Serial.println("Call back: " + msg);
  
  blink_Network_LED(1);

  if (msg == "start") {
    restart_game();
    game_started = true;
  }

  if (msg == "end") { restart_game(); }                         // The message "end" means the game is over -> restart all parameters

  if (msg.substring(0, msg.indexOf(",")) == String(placeID)) {  // Checking if a message is the resilt of a vote (placeID, a/b)
    String result = msg.substring(msg.indexOf(",") + 1);
    Serial.println("result: " + result);
    if (current_gate == 0 && result == "a") { 
      vote(gate_1_output_1);
    } else if (current_gate == 0 && result == "b") {
      vote(gate_1_output_2);
    } else if (current_gate == 1 && result == "a") {
      vote(gate_2_output_1);
    } else if (current_gate == 1 && result == "b") {
      vote(gate_2_output_2);
    }
  }
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connection Opened");
        led_12_state = true;
        network_connection = true;
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
        led_12_state = false;
        network_connection = false;
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }

    digitalWrite(network_LED, led_12_state);
}

void network_setup() {
  pinMode(network_LED, OUTPUT); digitalWrite(network_LED, LOW);     // Initializing the GPIO_12 pin as an output for "blinking" the network_LED
  pinMode(gate_1, INPUT); pinMode(gate_2, INPUT); // Initializing the gate PINs for reading button state

  // The ESP32 connects to the wifi and informs the serial monitor
  WiFi.begin(ssid,password);
  for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        digitalWrite(network_LED, !led_12_state);
        delay(1000);
  }
  if(WiFi.status() != WL_CONNECTED) { 
    Serial.println("No WiFi!");
    digitalWrite(network_LED, LOW);
  }

  // The ESP32 will start listening for messages and events, using the functions defined above
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  //The ESP32 connects to the websocket and sends the certificate
  client.setCACert(echo_org_ssl_ca_cert);
  client.connect(websockets_connection_string);
  client.ping();
  // The ESP32 sends the booting message, letting the websocket know its MAC address for identification
  String bootmsg = WiFi.macAddress() + ";" + String(placeID) + ";" + String(gate_1_output_1) + "," + String(gate_1_output_2) + ";" + String(gate_2_output_1) + "," + String(gate_2_output_2);

  if(start_hex == true) { bootmsg = bootmsg + ";0"; }

  client.send(bootmsg);
}

void check_start_sensors(bool button_1_state, bool button_2_state) {
  // button_1_state = analogRead(gate_1);
  
  //if (button_1_state == HIGH && (millis() - gate_1_last_activated >= 30000)){
  if (button_1_state && (millis() - gate_1_last_activated >= 300)){
    client.send(String(placeID) + ",0");
    current_gate = 0;
    gate_1_last_activated = millis();
  }
  //if (button_2_state == HIGH && (millis() - gate_2_last_activated >= 30000)){
  if (gate_2) {
    // button_2_state = analogRead(gate_2);
    if (button_2_state && (millis() - gate_2_last_activated >= 300)){
      client.send(String(placeID) + ",1");
      current_gate = 1;
      gate_2_last_activated = millis();
    }
  } 
}

// Network code that is called in the main loop-function.
void network_loop(bool s1 = false, bool s2 = false) {
  if (millis() - pollTimer >= 100) {
    client.poll();          //The ESP32 pings the websocket to maintain conneciton
    pollTimer = millis();
  }
  
  if (network_connection == true && (millis() - pingTimer >= 10000)) { // Sending an 'I am alive' message to NodeRed every 10 seconds
    client.send("ping," + String(placeID));
    pingTimer = millis();
    blink_Network_LED(1);
  }

  if(game_started == true) { check_start_sensors(s1, s2); }   // Check button/sensor state only if game is running
}
