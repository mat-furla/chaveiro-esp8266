#include <stdlib.h>
#include <string.h>
#include <ESP8266WiFi.h>
#include "Firebase.h"
#include "config.h"

Firebase api(FIREBASE_HOST, REQUISICAO_URL, KEYHISTORY_URL);
unsigned long api_mtbs = 5000;
unsigned long api_lasttime = -5000;

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.print("\nConnecting to:");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("Wi-fi connected");
    Serial.println("IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);
}

void loop()
{
    if (millis() > api_lasttime + api_mtbs)
    {
        Serial.println("Loading...");

        if (api.getRequisitionKey())
        {
            api_lasttime = millis();
            Serial.print("Key Number: ");
            Serial.println(api.key.keyNumber);

            Serial.print("Requisition Id: ");
            Serial.println(api.key.requisitionId);

            if(api.key.keyNumber != 0){
                if (api.deleteRequisition())
                {
                    Serial.println("Document " + api.key.requisitionId + " deleted");
                }
                else
                {
                    Serial.println("Document " + api.key.requisitionId + " failed to delete");
                }

                if (api.updateKeyHistory())
                {
                    Serial.println("Document " + api.key.requisitionId + " updated");
                }
                else
                {
                    Serial.println("Document " + api.key.requisitionId + " failed to update");
                }
            }
        }
        else
        {
            delay(2000);
            api_lasttime = 0;
        }
    }
}