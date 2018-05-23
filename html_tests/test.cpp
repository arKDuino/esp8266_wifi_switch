#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "page_generator.h"

void dumpSwitchPage()
{
	//limit title length
	const int TITLE_MAX_LENGTH = 32;
	char _title[TITLE_MAX_LENGTH];
	strncpy(_title, "bedrooom lights", TITLE_MAX_LENGTH - 1);
	_title[TITLE_MAX_LENGTH] = '\0';
		
	std::string html = getSwitchPage(_title);
	std::ofstream out("switchPage.html");
	out << html;
	out.close();
}

void dumpConfigPage()
{
	uint8_t mac[6];
	char* ip = strdup("192.168.10.4");
	mac[0] = 15;
	mac[1] = 128;
	mac[2] = 200;
	mac[3] = 254;
	mac[4] = 211;
	mac[5] = 51;
	std::string html = getConfigPage(mac, ip);
	std::ofstream out("configPage.html");
	out << html;
	out.close();
	free(ip);
}

void dumpConfigurationSavedPage()
{
	std::string html = getConfigurationSavedPage();
	std::ofstream out("configurationSaved.html");
	out << html;
	out.close();
}

void dumpConfigurationInvalidPASSWD()
{
	std::string html = getConfigurationInvalidPASSWD();
	std::ofstream out("ConfigurationInvalidPASSWD.html");
	out << html;
	out.close();
}

void dumpConfigurationInvalidSSID()
{
	std::string html = getConfigurationInvalidSSID();
	std::ofstream out("ConfigurationInvalidSSID.html");
	out << html;
	out.close();
}

void dumpConfigurationInvalidIP()
{
	std::string html = getConfigurationInvalidIP();
	std::ofstream out("ConfigurationInvalidIP.html");
	out << html;
	out.close();
}

int main()
{
	printf("\nCreating HTML pages...\n");
	dumpSwitchPage();
	dumpConfigPage();
	dumpConfigurationSavedPage();
	dumpConfigurationInvalidPASSWD();
	dumpConfigurationInvalidSSID();
	dumpConfigurationInvalidIP();
	
	printf("\nHTML pages created sucessfully!\n");
	return 0;
}