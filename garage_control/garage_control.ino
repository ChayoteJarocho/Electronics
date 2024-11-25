#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Reminder that the board pin number does not match the actual int value, use the GPIO value:
// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
#define D6 12 // For controlling the relay

const char *wifi_ssid = "TheSSIDName";
const char *wifi_password = "TheSSIDPassword";

const char *mqtt_server = "192.XXX.XXX.XXX";
const char *mqtt_topic = "homeassistant/garage";
const char *mqtt_username = "UsernameForMQTTIntegration";
const char *mqtt_password = "PasswordForMQTTIntegration";
const int mqtt_port = 1883; // Default port used by HA MQTT integration

String clientId = "MQTTGarage"; // Change as desired
const char *mqtt_clientId = clientId.c_str();

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup_wifi()
{
  delay(100); // This delay helps ensure the first serial message shows up in the monitor
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt_broker()
{
  while (!mqttClient.connected())
  {
    Serial.printf("Attempting MQTT connection to '%s' as '%s'... ", mqtt_server, mqtt_clientId);
    if (mqttClient.connect(mqtt_clientId, mqtt_username, mqtt_password))
    {
      Serial.println("Connected!");
      Serial.printf("Subscribing to '%s'\n", mqtt_topic);
      mqttClient.subscribe(mqtt_topic);
    }
    else
    {
      Serial.print(" Failed! state: ");
      Serial.println(mqttClient.state());
      Serial.println("Trying again in 5 seconds.");
      delay(5000);
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
  Serial.printf("Topic: '%s'\n", topic);
  for (unsigned int i = 0; i < len; i++)
  {
      Serial.print((char) payload[i]);
  }

  // I chose the letter 'g' as the single payload character to indicate button press
  if (len > 0 && (char)payload[0] == 'g')
  {
    digitalWrite(D6, HIGH);
    delay(1500);
    digitalWrite(D6, LOW);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void setup()
{
  pinMode(D6, OUTPUT);
  Serial.flush();
  Serial.begin(115200); // Lower numbers show garbled text
  setup_wifi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  delay(50);
}

void loop()
{
  reconnect_mqtt_broker();
  mqttClient.loop();
  // Half a second between checks for MQTT published messages feels enough,
  // as the relay also has a bit of delay to react after the MQTT message is detected
  delay(500);
}
