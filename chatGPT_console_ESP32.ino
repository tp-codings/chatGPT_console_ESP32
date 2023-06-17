#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//Keys
const char* WIFI_SSID;
const char* WIFI_PASSWORD;
const char* apiKey;

HTTPClient http;

//Initialisiere Schlüssel
void initKeys() {
  WIFI_SSID = "";
  WIFI_PASSWORD = "";
  apiKey = "";
}

void setup() {
  initKeys();
  Serial.begin(115200);

  // Mit WLAN verbinden
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Verbindungsaufbau ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  //Verbindungsaufbau zur API https://platform.openai.com/docs/plugins/getting-started
  String apiUrl = "https://api.openai.com/v1/completions";
  http.begin(apiUrl);

  //Header hinzufügen, die von der API gefordert werden
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(apiKey));
  http.setTimeout(30000);
}

void loop() {
  String input = "";

  //Eingabe Text
  input = Serial.readStringUntil('\n');

  if (input != "") {
    Serial.print("Du: ");
    Serial.println(input);

    //HTML Request an API mit Davinci Modell und einem maximalen Tokenverbrauch von 100
    String payload = "{\"prompt\":\"" + input + "\",\"max_tokens\":100, \"model\": \"text-davinci-003\"}";
    int httpResponseCode = http.POST(payload);

    //Abfrage des hhtp response Code, 200 bedeutet erfolgreich
    if (httpResponseCode == 200) {
      String response = http.getString();

      // Parsen der JSON Antwort
      DynamicJsonDocument jsonDoc(1024);
      deserializeJson(jsonDoc, response);
      String outputText = jsonDoc["choices"][0]["text"];

      //Ausgabe Text
      Serial.print("KI: ");
      Serial.println(outputText);

      input = "";
    } else {
      Serial.printf("Error %i \n", httpResponseCode);
    }
  }
}
