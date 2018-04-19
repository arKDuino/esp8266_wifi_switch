#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <user_interface.h>
#include <ESP8266mDNS.h>
#include "page_generator.h"

#define PCF8574_ADDRESS 32 //PCF8574 pins A0, A1 and A2 wired to ground
#define PCF8574_RELAY_PIN 0b00000001 //PCF8574 RELAY PIN
#define PCF8574_STATUS_PIN 0b00000010 //PCF8574 LED PIN
#define PCF8574_RESET_PIN 0b00000100 //PCF8574 RESET BUTTON
#define GPIO_SDA 4 //esp8266 gpio4
#define GPIO_SCL 5 //esp8266 gpio5
#define EEPROM_SIZE 1024
#define MAX_SSID_LENGTH 32 //according to 802.11
#define MAX_PASSWD_LENGTH 64 //according to 802.11 - WPA specification

//uncomment this when using a new es8266
//#define ERASE_EEPROM_NEW_CHIP true

typedef struct
{
  char* title = NULL;
  char* ssid = NULL;
  char* passwd = NULL;
  char* lastIP = NULL; //format: 0.0.0.0
  bool use_DHCP = false;
  char* deviceIP = NULL; //format: 0.0.0.0
  char* deviceGateway = NULL; //format: 0.0.0.0
  char* deviceSubnet = NULL; //format: 0.0.0.0
} EEPROM_DATA_STRUCT;

const char* TEXT_HTML = "text/html";

ESP8266WebServer server(80);
uint8_t mac[6];
uint8_t pcf8574_pins = 0; //PCF8574 state
EEPROM_DATA_STRUCT wifi_data;
os_timer_t tEraserButton;
bool flag_checkbutton = false;
bool is_in_config_mode = false;
int no_connection_timeout = 0;

void seeEEPROM()
{
  Serial.println("Actual EEPROM content:");
  char content[EEPROM_SIZE];
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    content[i] = EEPROM.read(i);
  }

  Serial.println(content);
}

void loadDataFromEEPROM(EEPROM_DATA_STRUCT* buf)
{
  char content[EEPROM_SIZE];
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    content[i] = EEPROM.read(i);
  }

  Serial.println("Converting this data from EEPROM into JSON:");
  Serial.println(content);

  StaticJsonBuffer<EEPROM_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(content);

  //strdup will create another pointer dinamically alocated
  //so it will resides on memory forever
  buf->title = strdup(root["title"]);
  buf->ssid = strdup(root["ssid"]);
  buf->passwd = strdup(root["passwd"]);
  buf->lastIP = strdup(root["lastIP"]);
  buf->use_DHCP = root["DHCP"];
  buf->deviceIP = strdup(root["deviceIP"]);
  buf->deviceGateway = strdup(root["deviceGateway"]);
  buf->deviceSubnet = strdup(root["deviceSubnet"]);

}

void saveDataToEEPROM(EEPROM_DATA_STRUCT* buf)
{
  //convert to JSON
  StaticJsonBuffer<EEPROM_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["title"] = buf->title;
  root["ssid"] = buf->ssid;
  root["passwd"] = buf->passwd;
  root["lastIP"] = buf->lastIP;
  root["DHCP"] = buf->use_DHCP;
  root["deviceIP"] = buf->deviceIP;
  root["deviceGateway"] = buf->deviceGateway;
  root["deviceSubnet"] = buf->deviceSubnet;

  //get a char array
  char json_chars[EEPROM_SIZE];
  root.printTo(json_chars, EEPROM_SIZE);
  Serial.println("Saving this JSON into EEPROM:");
  Serial.println(json_chars);

  //write on EEPROM memory
  for (int i = 0; i < strlen(json_chars); i++)
  {
    EEPROM.write(i, json_chars[i]);
  }

  //null terminating character
  EEPROM.write(strlen(json_chars), '\0');

  //write null on the remaining space
  for (int i = strlen(json_chars) + 1; i < EEPROM_SIZE; i++)
  {
    EEPROM.write(i, 0);
  }

  EEPROM.commit();
}

void updatePCF8574()
{
  //send status to PCF8574
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(pcf8574_pins);
  Wire.endTransmission();
}

void blinkStatusLights(int times)
{
  //PCF8574 is wired to leds in current sinking mode
  for (int i = 0; i < times; i++)
  {
    turnLedOn();
    delay(300);

    turnLedOff();
    delay(300);
    yield();
  }
}

void setFlagCheckButton(void* argss)
{
  flag_checkbutton = true;
}

void turnLedOff()
{
  //PCF8574 is wired to led in current sinking mode, so 0 turn ON, and 1 turn OFF
  pcf8574_pins |= PCF8574_STATUS_PIN;
  updatePCF8574();
}

void turnLedOn()
{
  //PCF8574 is wired to led in current sinking mode, so 0 turn ON, and 1 turn OFF
  pcf8574_pins &= ~(PCF8574_STATUS_PIN);
  updatePCF8574();
}

void turnRelayOn()
{
  pcf8574_pins |= PCF8574_RELAY_PIN;
  updatePCF8574();
}

void turnRelayOff()
{
  pcf8574_pins &= ~(PCF8574_RELAY_PIN);
  updatePCF8574();
}

void setupServerConfigMode()
{
  server.on("/", HTTP_GET, handleConfigPage);
  server.on("/setConfig", HTTP_POST, handleSetConfig);
  server.on("/setConfig", HTTP_GET, handleConfigPage);
  server.begin();

  Serial.println("HTTP server started in config mode");
}

void handleSetConfig()
{
  char* _ssid = strdup(server.arg("txtSSID").c_str());
  //data validation
  if (strlen(_ssid) <= 1 || strlen(_ssid) > MAX_SSID_LENGTH - 1)
  {
    std::string html = getConfigurationInvalidSSID();
    String s_html = String(html.c_str());
    ESP.wdtFeed();
    server.send(200, TEXT_HTML, s_html);
    return;
  }

  char* _passwd = strdup(server.arg("txtPASSWORD").c_str());
  //data validation
  if (strlen(_passwd) <= 1 || strlen(_passwd) > MAX_PASSWD_LENGTH - 1)
  {
    std::string html = getConfigurationInvalidPASSWD();
    String s_html = String(html.c_str());
    ESP.wdtFeed();
    server.send(200, TEXT_HTML, s_html);
    return;
  }

  char* _dhcp = strdup(server.arg("chkDHCP").c_str());
  char* _ip = strdup(server.arg("txtIP").c_str());
  char* _subnet = strdup(server.arg("txtSubnet").c_str());
  char* _gateway = strdup(server.arg("txtGateway").c_str());

  //limit title length
  const int TITLE_MAX_LENGTH = 32;
  char _title[TITLE_MAX_LENGTH];
  strncpy(_title, server.arg("txtTitle").c_str(), TITLE_MAX_LENGTH - 1);
  _title[TITLE_MAX_LENGTH] = '\0';

  if (strcmp(_dhcp, "on") != 0) //validate ip adresses only if dhcp is off
  {
    IPAddress ip_validator;
    if (!ip_validator.fromString(_ip))
    {
      std::string html = getConfigurationInvalidIP();
      String s_html = String(html.c_str());
      ESP.wdtFeed();
      server.send(200, TEXT_HTML, s_html);
      return;
    }

    IPAddress subnet_validator;
    if (!subnet_validator.fromString(_subnet))
    {
      std::string html = getConfigurationInvalidSubnet();
      String s_html = String(html.c_str());
      ESP.wdtFeed();
      server.send(200, TEXT_HTML, s_html);
      return;
    }

    IPAddress gateway_validator;
    if (!gateway_validator.fromString(_gateway))
    {
      std::string html = getConfigurationInvalidGateway();
      String s_html = String(html.c_str());
      ESP.wdtFeed();
      server.send(200, TEXT_HTML, s_html);
      return;
    }
  }

  Serial.println("New settings received");
  Serial.print("Title:");
  Serial.println(_title);
  Serial.print("SSID:");
  Serial.println(_ssid);
  Serial.print("PASSWORD: ");
  Serial.println(_passwd);
  Serial.print("USE DHCP: ");
  Serial.println(_dhcp);
  Serial.print("IP: ");
  Serial.println(_ip);
  Serial.print("SUBNET: ");
  Serial.println(_subnet);
  Serial.print("GATEWAY: ");
  Serial.println(_gateway);

  wifi_data.title = _title;
  wifi_data.ssid = _ssid;
  wifi_data.passwd = _passwd;
  if (strcmp(_dhcp, "on") == 0)
  {
    wifi_data.use_DHCP = true;
  }
  else
  {
    wifi_data.use_DHCP = false;
  }
  wifi_data.deviceIP = _ip;
  wifi_data.deviceSubnet = _subnet;
  wifi_data.deviceGateway = _gateway;
  saveDataToEEPROM(&wifi_data);

  std::string html = getConfigurationSavedPage();
  String s_html = String(html.c_str());
  ESP.wdtFeed();
  server.send(200, TEXT_HTML, s_html);
}

void handleConfigPage()
{
  std::string html = getConfigPage(mac, wifi_data.lastIP);
  String s_html = String(html.c_str());
  ESP.wdtFeed();
  server.send(200, TEXT_HTML, s_html);
}

void setupServerNormalMode()
{
  server.on("/", handleMainPage);
  server.on("/setRelayOn", HTTP_GET, handleMainPage);
  server.on("/setRelayOff", HTTP_GET, handleMainPage);
  server.on("/setRelayOn", HTTP_POST, handleTurnOn);
  server.on("/setRelayOff", HTTP_POST, handleTurnOff);
  server.begin();

  Serial.println("HTTP server started in normal mode");
}

void handleMainPage()
{
  std::string html = getSwitchPage(wifi_data.title);
  String s_html = String(html.c_str());
  ESP.wdtFeed();
  server.send(200, TEXT_HTML, s_html);
}

void handleTurnOn()
{
  turnRelayOn();
  Serial.println("handleTurnOn();");
  handleMainPage();
}

void handleTurnOff()
{
  turnRelayOff();
  Serial.println("handleTurnOff();");
  handleMainPage();
}

void setup() {
  //initialize Serial
  Serial.begin(115200);
  delay(100);

  //init I2C
  Wire.begin(GPIO_SDA, GPIO_SCL);
  Wire.setClock(100000); //ensure clock of 100kHz
  turnRelayOn();
  turnLedOff();

  //init timer to check button
  os_timer_setfn(&tEraserButton, setFlagCheckButton, NULL); //set flag function
  os_timer_arm(&tEraserButton, 2000, true); //repeat every x miliseconds

  //begin EEPROM
  Serial.println("Initializing EEPROM...");
  EEPROM.begin(EEPROM_SIZE);

#ifdef ERASE_EEPROM_NEW_CHIP
  Serial.println("ERASING FOR NEW CHIP...");
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();

  Serial.println("...ERASED!");

  wifi_data.title = "";
  wifi_data.ssid = "";
  wifi_data.passwd = "";
  wifi_data.lastIP = "";
  wifi_data.use_DHCP = false;
  wifi_data.deviceIP = "";
  wifi_data.deviceGateway = "";
  wifi_data.deviceSubnet = "";
  saveDataToEEPROM(&wifi_data);

  Serial.println("REMOVE #define ERASE_EEPROM_NEW_CHIP AND BURN FIRMWARE AGAIN!!");
  blinkStatusLights(255);
  while (true);
#endif

  loadDataFromEEPROM(&wifi_data);

  Serial.print("SSID:");
  Serial.println(wifi_data.ssid);

  Serial.print("PASSWORD:");
  Serial.println(wifi_data.passwd);

  //get MAC Address
  Serial.print("MAC: ");
  WiFi.macAddress(mac);
  Serial.print(mac[0], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.println(mac[5], HEX);

  //if there is no ssid, go to config mode
  if (strlen(wifi_data.ssid) <= 1)
  {
    is_in_config_mode = true;
    turnRelayOff();
    Serial.println("Entering in config mode...");
    WiFi.softAP("configuracao", "configuracao");
    setupServerConfigMode();
    return;
  }

  //Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_data.ssid);
  WiFi.mode(WIFI_STA);

  if (wifi_data.use_DHCP)
  {
    Serial.println("Using DHCP...");
    //nothing to do, DHCP is default
  }
  else
  {
    Serial.println("Using static IP...");
    // NETWORK: Static IP details...
    IPAddress ip;
    Serial.print("IP: ");
    Serial.println(wifi_data.deviceIP);
    ip.fromString(wifi_data.deviceIP);

    IPAddress subnet;
    Serial.print("Subnet: ");
    Serial.println(wifi_data.deviceGateway);
    subnet.fromString(wifi_data.deviceSubnet);

    IPAddress gateway;
    Serial.print("Gateway: ");
    Serial.println(wifi_data.deviceGateway);
    gateway.fromString(wifi_data.deviceGateway);

    WiFi.config(ip, gateway, subnet);
  }

  WiFi.begin(wifi_data.ssid, wifi_data.passwd);

  //try to connect to network, if cannot connect in x seconds,
  //turn main relay off, reset config and and blink status lights forever
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(10);
    no_connection_timeout++;
    if (no_connection_timeout > 1500)
    {      
      os_timer_disarm(&tEraserButton);

      Serial.println("Password/SSID/IP wrong, settings will be deleted...");
      wifi_data.title = "";
      wifi_data.ssid = "";
      wifi_data.passwd = "";
      wifi_data.use_DHCP = false;
      wifi_data.deviceIP = "";
      wifi_data.deviceGateway = "";
      wifi_data.deviceSubnet = "";
      saveDataToEEPROM(&wifi_data);
      turnRelayOff();
      blinkStatusLights(255);  
    }
  }

  no_connection_timeout = 0;

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("Server started on ");
  wifi_data.lastIP = strdup(WiFi.localIP().toString().c_str());
  Serial.println(wifi_data.lastIP);

  //setup MDNS
  if (!MDNS.begin(wifi_data.title, WiFi.localIP()))
  {
    Serial.println("Error setting up MDNS responder!");
  }

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);

  setupServerNormalMode();

  //blink lighst to tell everything is OK
  blinkStatusLights(2);
}

void loop()
{
  server.handleClient();

  //interrupt enabled flag, check if the reset button is pressed
  //if so, reset memory and restart
  if (flag_checkbutton)
  {
    flag_checkbutton = false;
    uint8_t _data = 0;
    //i2c communication, request 1 byte
    Wire.requestFrom(PCF8574_ADDRESS, 1);
    if (Wire.available())
    {
      _data = Wire.read();
    }
    Wire.endTransmission();

    if (_data & PCF8574_RESET_PIN)
    {
      //disable interrupt
      os_timer_disarm(&tEraserButton);

      //button pressed, clean data and save into eeprom
      wifi_data.title = "";
      wifi_data.ssid = "";
      wifi_data.passwd = "";
      wifi_data.use_DHCP = false;
      wifi_data.deviceIP = "";
      wifi_data.deviceGateway = "";
      wifi_data.deviceSubnet = "";
      saveDataToEEPROM(&wifi_data);
      turnRelayOff();
      Serial.println("Memory cleaned, will reset soon...");
      blinkStatusLights(10);
      ESP.reset();
    }
  }
}