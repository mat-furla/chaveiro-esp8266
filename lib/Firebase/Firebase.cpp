#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "Firebase.h"

Firebase::Firebase(String firebase_host, String requisicao_url, String keyhistory_url)
{
    int strLenFirebaseHost = firebase_host.length() + 1;
    char tempFirebaseHost[strLenFirebaseHost];
    firebase_host.toCharArray(tempFirebaseHost, strLenFirebaseHost);

    Firebase(tempFirebaseHost, requisicao_url, keyhistory_url);
}

Firebase::Firebase(char *firebase_host, String requisicao_url, String keyhistory_url)
{
    _firebase_host = firebase_host;
    _requisicao_url = requisicao_url;
    _keyhistory_url = keyhistory_url;
}

bool Firebase::getRequisitionKey()
{
    bool wasSuccessful = false;

    HTTPClient https;
    BearSSL::WiFiClientSecure client;

    client.setInsecure();
    https.begin(client, _firebase_host, 443, _requisicao_url, true);
    https.addHeader("Content-Type", "application/json");
    int httpCode = https.GET();

    if (httpCode >= 200 && httpCode <= 299)
    {
        const size_t capacity = JSON_ARRAY_SIZE(1) + 3 * JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(4) + 250;
        DynamicJsonDocument doc(capacity);

        DeserializationError error = deserializeJson(doc, https.getString());
        if (!error)
        {
            if (doc["documents"][0]["name"])
            {
                key.requisitionId = doc["documents"][0]["name"].as<String>();
                key.requisitionId = key.requisitionId.substring(63, 99);
                key.keyNumber = doc["documents"][0]["fields"]["chave"]["stringValue"].as<int>();
            }
            else
            {
                key.requisitionId = "";
                key.keyNumber = 0;
            }
            wasSuccessful = true;
        }
        else
        {
            Serial.print(F("deserializeJson() failed with code: "));
            Serial.println(error.c_str());
        }
    }
    else
    {
        Serial.print("Unexpected HTTP Status Code: ");
        Serial.println(httpCode);
    }
    https.end();
    client.stop();

    return wasSuccessful;
}

bool Firebase::deleteRequisition()
{
    bool wasSuccessful = false;

    HTTPClient https;
    BearSSL::WiFiClientSecure client;

    client.setInsecure();
    https.begin(client, _firebase_host, 443, _requisicao_url + "/" + key.requisitionId, true);

    int httpCode = https.sendRequest("DELETE");

    if (httpCode >= 200 && httpCode <= 299)
    {
        wasSuccessful = true;
    }
    https.end();
    client.stop();

    return wasSuccessful;
}

bool Firebase::updateKeyHistory()
{
    bool wasSuccessful = false;

    HTTPClient https;
    BearSSL::WiFiClientSecure client;

    client.setInsecure();
    https.begin(client, _firebase_host, 443, _keyhistory_url + "/" + key.requisitionId, true);
    https.addHeader("content-type", "application/x-www-form-urlencoded");
    String body = "booleanValue=false";

    int httpCode = https.POST(body);

    if (httpCode >= 200 && httpCode <= 299)
    {
        wasSuccessful = true;
    }
    https.end();
    client.stop();

    return wasSuccessful;
}