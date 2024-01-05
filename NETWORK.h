
// --------------------- NETWORK SETINGS ---------------------
#define WiFiname "aalto open"                                               //This is the name of the WiFi
#define WiFipw ""                                                           //This is the password of the WiFi
#define NEPPI_SOCKET "wss://elecdesign.org.aalto.fi/neppi/ws/hex/"           //This is the URL of the Websocket

//This is the https certificate for the Websocket
#define NEPPI_CERTIFICATE  \
  "-----BEGIN CERTIFICATE-----\n" \
  "!!!Introduce the certificate here!!!\n" \
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
