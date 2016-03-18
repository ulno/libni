#include <ESP8266WiFi.h>
#include "ulno_esp_utils.h"

void ulno_esp_init_wifi(const char *essid, const char* psk) {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(essid);

  WiFi.begin(essid, psk);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

}

void ulno_esp_init(const char* hello, const char *essid, const char* psk) {
//  Serial.begin(230400);
  Serial.begin(115200);
  delay(10);

  Serial.println(hello);
  ulno_esp_init_wifi( essid, psk);

}
