#pragma once

#include <WiFi.h>
#include <lwip/sockets.h>

#define WIFI_CLIENT_DEF_CONN_TIMEOUT_MS (3000)
#define WIFI_CLIENT_MAX_WRITE_RETRY     (20)
#define WIFI_CLIENT_SELECT_TIMEOUT_US   (1000000)

class WiFiClientExt : public WiFiClient
{
    public:
    size_t writeBlocking(const uint8_t *buf, size_t size)
    {
        int res =0;
        int retry = WIFI_CLIENT_MAX_WRITE_RETRY;
        int socketFileDescriptor = fd();
        size_t totalBytesSent = 0;
        size_t bytesRemaining = size;

        if(!_connected || (socketFileDescriptor < 0)) {
            return 0;
        }

        while(retry) {
            //use select to make sure the socket is ready for writing
            fd_set set;
            struct timeval tv;
            FD_ZERO(&set);        // empties the set
            FD_SET(socketFileDescriptor, &set); // adds FD to the set
            tv.tv_sec = 0;
            tv.tv_usec = WIFI_CLIENT_SELECT_TIMEOUT_US;
            retry--;

            if(select(socketFileDescriptor + 1, NULL, &set, NULL, &tv) < 0) {
                return 0;
            }

            if(FD_ISSET(socketFileDescriptor, &set)) {
                res = send(socketFileDescriptor, (void*) buf, bytesRemaining, MSG_DONTWAIT);
                if(res > 0) {
                    totalBytesSent += res;
                    if (totalBytesSent >= size) {
                        //completed successfully
                        Serial.print("+");
                        retry = 0;
                    } else {
                        Serial.print("/");
                        buf += res;
                        bytesRemaining -= res;
                        retry = WIFI_CLIENT_MAX_WRITE_RETRY;
                        delay(1000);
                    }
                }
                else if(res < 0) {
                    Serial.print("-");
                    delay(1000);
                }
                else {
                    Serial.print("0");
                    // Try again
                }
            }
        }
        Serial.print("t:");
        Serial.print(totalBytesSent);
        return totalBytesSent;
    }
};