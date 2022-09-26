#ifndef WEB_H
#define WEB_H
#include <Arduino.h>
extern "C" {
    #include "crypto/base64.h"
}
#include <channels.h>
#include <location.h>
#include <animations.h>
#include <storage.h>
#include <signals.h>
#include <messagequeue.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>


#define AP_SSID "LightControllerConfig"
#define AP_PASS "password"

static State* GlobalState;

static DNSServer dnsServer;
static AsyncWebServer server(80);
static short clientsConnected = 0;
static uint8_t uploadBuffer[1024*14];

void initializeAP(State* state);

void OnWiFiEvent(WiFiEvent_t event);

String utilsProcessor(const String& var);

String viewProcessor(const String& var);

String animationProcessor(const String& var);

String switchesProcessor(const String& var);

String channelProcessor(const String& var);

AwsTemplateProcessor getChannelLocProcessor(int channelIndex);

AwsTemplateProcessor getChannelProcessor(int channelIndex);

int getClientCount();

#endif