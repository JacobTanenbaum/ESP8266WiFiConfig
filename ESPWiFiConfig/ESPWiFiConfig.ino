#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define AUTOCONNECT 1

const char WiFiAPPSK[] = "thereisnospoon";                // for demo perposes hardcode password
ESP8266WebServer server(80);                              // HTTP server will listen at port 


String form = 
"<!DOCTYPE html>"
"<html>"
"<body>"
"<p>"
"<center>"
"<h1>Jacob's SoftAP WiFi setup Demo </h1>"
"<form action=\"msg\">"
"SSID: <input type=\"text\" name=\"SSID\"><br>"
"PASSWORD: <input type=\"text\" name=\"Password\"><br>"
"<input type=\"submit\" value=\"Submit\">"
"</form>"

"<p>Click the \"Submit\" button and the ESP8266 will try to connect to SSID given.</p>"
"</body>"
"</html>";


int testWifi(void);     //autoconnect to last network
void setupAP(void);     //setup softAP
void handle_msg(void);  //handle_msg callback for dealing with html pages 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  delay(10);
  Serial.println("Startup");
  if ( AUTOCONNECT) {
    testWifi();
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("");
    Serial.println("Connection timed out or Autoconnect not enabled, opening AP");
    setupAP();
    
   }
       
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.localIP());     

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(BUILTIN_LED, LOW);
  delay(1000);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(1000);  
}


int testWifi(void) {
  long Timeout = (millis() + 10000);
  //WiFi.printDiag(Serial);             //Uncomment this line to see the printed network details
  
  Serial.print("Waiting for Wifi to connect. ");  
  while (millis() < Timeout){
    delay(500);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected.");
      return(1); 
    }
  }      
  return(0);
} 

void setupAP(void){
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.softAPmacAddress(mac);
  
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);

  macID.toUpperCase();

  macID.toUpperCase();
  String AP_NameString = "ESP8266-" + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
  server.on("/", []()
  {
    server.send(200, "text/html", form);
  });
  server.on("/msg", handle_msg);
  server.begin();
  while(WiFi.status() != WL_CONNECTED){
    server.handleClient();
  }
}

void handle_msg(void){
  String ssid = server.arg("SSID");
  String password = server.arg("Password");
  char __ssid[ssid.length() * sizeof(String)];
  char __password[password.length() * sizeof(String)];

  ssid.toCharArray(__ssid, sizeof(__ssid));
  password.toCharArray(__password, sizeof(__password));

  Serial.println("Connecting to local Wifi"); //Close the AP and connect with new SSID and P/W
  WiFi.softAPdisconnect(true);
  delay(500);
  WiFi.begin(__ssid,__password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println("");
}

