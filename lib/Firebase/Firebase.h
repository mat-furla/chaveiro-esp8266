#ifndef Firebase_h
#define Firebase_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

struct keyStruct
{
    int keyNumber;
    String requisitionId;
};

class Firebase
{
    private:
        char *_firebase_host;
        String _requisicao_url;
        String _keyhistory_url;

    public:
        Firebase(char *firebase_host, String requisicao_url, String keyhistory_url);
        Firebase(String firebase_host, String requisicao_url, String keyhistory_url);
        bool getRequisitionKey();
        bool deleteRequisition();
        bool updateKeyHistory();
        keyStruct key;
};

#endif