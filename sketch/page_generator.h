//generates html pages for the main program
//separated .h file for tests uses

#ifndef page_generator_h
#define page_generator_h

#include <string>
#include "constants_lang_pt.h"

std::string getConfigurationSavedPage()
{
  std::string buf;
  buf += "<!DOCTYPE html>\n";
  buf += "<html lang=\"en\">\n";
  buf += "<head>\n";  
  buf += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n";
  buf += "<meta charset=\"utf-8\" />\n";
  buf += "<title>ESP8266</title>\n";
  buf += "</head>\n";
  buf += "<h3>";
  buf += text_config_saved_success;
  buf += "</h3>";
  buf += "</html>\n";

  return buf;  
}

std::string getConfigurationInvalidSSID()
{
  std::string buf;
  buf += "<!DOCTYPE html>\n";
  buf += "<html lang=\"en\">\n";
  buf += "<head>\n";  
  buf += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n";
  buf += "<meta charset=\"utf-8\" />\n";
  buf += "<title>ESP8266</title>\n";
  buf += "</head>\n";
  buf += "<h3>";
  buf += text_invalid_ssid;
  buf += "</h3>";
  buf += "</html>\n";

  return buf;  
}

std::string getConfigurationInvalidIP()
{
  std::string buf;
  buf += "<!DOCTYPE html>\n";
  buf += "<html lang=\"en\">\n";
  buf += "<head>\n";  
  buf += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n";
  buf += "<meta charset=\"utf-8\" />\n";
  buf += "<title>ESP8266</title>\n";
  buf += "</head>\n";
  buf += "<h3>";
  buf += text_invalid_ip;
  buf += "</h3>";
  buf += "</html>\n";

  return buf;  
}

std::string getConfigurationInvalidSubnet()
{
  std::string buf;
  buf += "<!DOCTYPE html>\n";
  buf += "<html lang=\"en\">\n";
  buf += "<head>\n";  
  buf += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n";
  buf += "<meta charset=\"utf-8\" />\n";
  buf += "<title>ESP8266</title>\n";
  buf += "</head>\n";
  buf += "<h3>";
  buf += text_invalid_subnet;
  buf += "</h3>";
  buf += "</html>\n";

  return buf;  
}


std::string getConfigurationInvalidGateway()
{
  std::string buf;
  buf += "<!DOCTYPE html>\n";
  buf += "<html lang=\"en\">\n";
  buf += "<head>\n";  
  buf += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n";
  buf += "<meta charset=\"utf-8\" />\n";
  buf += "<title>ESP8266</title>\n";
  buf += "</head>\n";
  buf += "<h3>";
  buf += text_invalid_gateway;
  buf += "</h3>";
  buf += "</html>\n";

  return buf;  
}

std::string getConfigurationInvalidPASSWD()
{
  std::string buf;
  buf += "<!DOCTYPE html>\n";
  buf += "<html lang=\"en\">\n";
  buf += "<head>\n";  
  buf += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n";
  buf += "<meta charset=\"utf-8\" />\n";
  buf += "<title>ESP8266</title>\n";
  buf += "</head>\n";
  buf += "<h3>";
  buf += text_invalid_password;
  buf += "</h3>";  
  buf += "</html>\n";

  return buf;  
}

std::string getConfigPage(uint8_t* mac)
{
  std::string buf;
  char hex[2];  
  
  buf += "<!DOCTYPE html>\n";
  buf += "<html lang=\"en\">\n";
  buf += "<head>\n";  
  buf += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n";
  buf += "<meta charset=\"utf-8\" />\n";
  buf += "<title>Configuracao</title>\n";

buf += 
"<style> \
table { \
    font-family: arial, sans-serif; \
    border-collapse: collapse; \
    width: 100%; \
} \
td, th { \
    border: 1px solid #dddddd; \
    text-align: left; \
    padding: 8px; \
} \
tr:nth-child(even) { \
    background-color: #dddddd; \
} \
</style>";
  
  buf += "</head>\n";
  buf += "<body>\n";
  buf += "<form action=\"setConfig\" method=\"POST\">\n";
  buf += "<table>\n";
  buf += "<tr>";
  buf += "<td>MAC Address</td>";
  buf += "<td>";
  sprintf(hex, "%X", mac[0]);
  buf += hex;
  buf += ":";
  sprintf(hex, "%X", mac[1]);
  buf += hex;
  buf += ":";  
  sprintf(hex, "%X", mac[2]);
  buf += hex;
  buf += ":";  
  sprintf(hex, "%X", mac[3]);
  buf += hex;
  buf += ":";  
  sprintf(hex, "%X", mac[4]);
  buf += hex;  
  buf += ":";  
  sprintf(hex, "%X", mac[5]);
  buf += hex;    
  buf += "</td>";
  buf += "<tr>";
  buf += "<td>";
  buf += text_device_name;
  buf += "</td>";
  buf += "<td><input type=\"text\" name=\"txtTitle\" id=\"txtTitle\" maxlength=15 onkeypress=\"return blockKeys();\" ></td>"; 
  buf += "</tr>";  
  buf += "<tr>";
  buf += "<td>";
  buf += text_ssid_name;
  buf += "</td>";
  buf += "<td><input type=\"text\" name=\"txtSSID\" ></td>"; 
  buf += "</tr>";
  buf += "<tr>";
  buf += "<td>";
  buf += text_wifi_passowrd;
  buf += "</td>";
  buf += "<td><input type=\"text\" name=\"txtPASSWORD\" ></td>"; 
  buf += "</tr>";  
  buf += "<tr>";
  buf += "<td>";
  buf += text_enable_dhcp;
  buf += "</td>";
  buf += "<td><input onclick=\"toggleIPConfiguration();\" type=\"checkbox\" name=\"chkDHCP\" id=\"chkDHCP\" ></td>"; 
  buf += "</tr>";
  buf += "<tr>";  
  buf += "<td>IP</td>";
  buf += "<td><input type=\"text\" name=\"txtIP\" id=\"txtIP\" ></td>"; 
  buf += "</tr>";
  buf += "<tr>";  
  buf += "<td>Subnet</td>";
  buf += "<td><input type=\"text\" name=\"txtSubnet\" id=\"txtSubnet\" ></td>"; 
  buf += "</tr>";  
  buf += "<tr>"; 
  buf += "<td>Gateway</td>";
  buf += "<td><input type=\"text\" name=\"txtGateway\" id=\"txtGateway\" ></td>"; 
  buf += "</tr>";  
  buf += "</table>";
  buf += "<br/>";  
  buf += "<div>";
  buf += "<input style=\"display: block; margin: 0 auto;\" type=\"submit\" value=\"SALVAR\">";
  buf += "</div>";
  buf += "</form>\n";
  buf += "</body>\n";
  
  buf += "<script>"; //only simple chars, without spaces
  buf += "var allowed_chars = [48, 49, 50, 51, 52, 53, 54, 55, 56,"; 
  buf += " 57, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,";
  buf += " 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 97,";
  buf += " 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,";
  buf += "  110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122];";
  buf += "function blockKeys() {";
  buf += "var x = event.which || event.keyCode;";
  buf += "if (allowed_chars.indexOf(x) > -1) {";
  buf += "return true;";
  buf += "} else { return false; }";
  buf += "}";
  
  buf += "\n\n";
  buf += "function toggleIPConfiguration() {";
  buf += "var chkDHCP = document.getElementById(\"chkDHCP\");";
  buf += "var DHCP_marcado = chkDHCP.checked;";
  buf += "if (DHCP_marcado) {";
  buf += "document.getElementById(\"txtIP\").value = \"\";";
  buf += "document.getElementById(\"txtIP\").disabled = true;";
  buf += "document.getElementById(\"txtSubnet\").value = \"\";";
  buf += "document.getElementById(\"txtSubnet\").disabled = true;";
  buf += "document.getElementById(\"txtGateway\").value = \"\";";
  buf += "document.getElementById(\"txtGateway\").disabled = true;";  
  buf += "}"; //if end
  buf += "else {";
  buf += "document.getElementById(\"txtIP\").disabled = false;";
  buf += "document.getElementById(\"txtSubnet\").disabled = false;";
  buf += "document.getElementById(\"txtGateway\").disabled = false;";
  buf += "}"; //else end
  buf += "}"; //function end
  buf += "</script>";
  
  buf += "</html>\n";
  
  return buf;
}

std::string getSwitchPage(char* page_title)
{
	std::string buf;
	
	buf += "<!DOCTYPE html>\n";
	buf += "<html lang=\"en\">\n";
	buf += "<head>\n";
	buf += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n";
	buf += "<meta charset=\"utf-8\" />\n";
	buf += "<title>";
	buf += page_title;
	buf += "</title>\n";
	buf += "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:80%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;}</style>\n";
	buf += "</head>\n";
	buf += "<h4>";
	buf += page_title;
	buf += "</h4>";
	buf += "<div>\n";
	buf += "<form action=\"setRelayOn\" method=\"POST\"><button>" + text_button_turn_on + "</button></form>\n";
	buf += "</div>\n";
	buf += "<div>\n";
	buf += "<form action=\"setRelayOff\" method=\"POST\"><button>" + text_button_turn_off + "</button></form>\n";
	buf += "</div>\n";
	buf += "<h5>Bróine Electronics</h5>\n";
	buf += "</html>";
	
	return buf;
}


#endif
