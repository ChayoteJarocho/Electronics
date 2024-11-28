#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Reminder that the board pin number does not match the actual int value, use the GPIO value:
// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
#define D6 12 // For controlling the relay

const char *wifi_ssid = "TheSSIDName";
const char *wifi_password = "TheSSIDPassword";

const char *mqtt_server = "192.XXX.XXX.XXX"; // The IP of your Home Assistant instance
const char *mqtt_topic_garage_trigger = "homeassistant/garage/trigger";
const char *mqtt_topic_garage_ping = "homeassistant/garage/ping";
const char *mqtt_topic_garage_ack = "homeassistant/garage/ack";
const char *mqtt_online = "online";
const char *mqtt_username = "myusername"; // This must be an existing user in Home Assistant
const char *mqtt_password = "mypassword";
const char *divider = "-----------------------";
const int mqtt_port = 1883; // Default port used by Home Assistant MQTT integration

enum ListenedTopic
{
  Unrecognized,
  Trigger,
  Ping
};

String clientId = "MQTTGarage"; // The unique id to identify this device in the broker log
const char *mqtt_clientId = clientId.c_str();

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void reconnect_wifi()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return;
  }

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
  Serial.println(divider);
  delay(50);
}

void reconnect_mqtt_broker()
{
  while (!mqttClient.connected())
  {
    if (!mqttClient.connect(mqtt_clientId, mqtt_username, mqtt_password))
    {
      Serial.printf("Attempting MQTT connection to '%s' as '%s'... ", mqtt_server, mqtt_clientId);
      Serial.print(" Failed! state: ");
      Serial.println(mqttClient.state());
      Serial.println("Trying again in 5 seconds.");
      
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("Wifi is connected.");
      }
      else
      {
        Serial.println("Wifi is disconnected.");
      }

      delay(5000);
    }
    Serial.println("MQTT connected!");
    Serial.printf("Subscribing to '%s'\n", mqtt_topic_garage_trigger);
    mqttClient.subscribe(mqtt_topic_garage_trigger);
    Serial.printf("Subscribing to '%s'\n", mqtt_topic_garage_ping);
    mqttClient.subscribe(mqtt_topic_garage_ping);
    Serial.println("Publishing that I'm online.");
    mqttClient.publish(mqtt_topic_garage_ack, mqtt_online);
    delay(50);
    Serial.println(divider);
  }
  // Do not add code outside of the while
}

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
  ListenedTopic listenedTopic = ListenedTopic::Unrecognized;

  if (strcmp(topic, mqtt_topic_garage_trigger) == 0)
  {
    listenedTopic = ListenedTopic::Trigger;
  }
  else if (strcmp(topic, mqtt_topic_garage_ping) == 0)
  {
    listenedTopic = ListenedTopic::Ping;
  }

  if (listenedTopic == ListenedTopic::Unrecognized)
  {
    return;
  }

  Serial.printf("Recieved message via topic: '%s'\n", topic);
  switch (listenedTopic)
  {
    case ListenedTopic::Trigger:
      if (len > 0)
      {
        if ((char)payload[0] == 'g')
        {
          Serial.println("Opening/closing garage.");
          digitalWrite(D6, HIGH);
          delay(1500);
          digitalWrite(D6, LOW);
        }
        else
        {
          Serial.printf("Unrecognized payload for topic '%s'.\n", topic);
        }
      }
      else
      {
        Serial.printf("Null payload for topic '%s'.\n", topic);
      }
      break;
    case ListenedTopic::Ping:
      Serial.println("Someone asked if I'm online.");
      mqttClient.publish(mqtt_topic_garage_ack, mqtt_online);
      break;
    default:
      break;
  }

  Serial.println();
  Serial.println(divider);
}

void setup()
{
  pinMode(D6, OUTPUT);
  Serial.flush();
  Serial.begin(115200);
  reconnect_wifi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  delay(50);
}

void loop()
{
  reconnect_wifi();
  reconnect_mqtt_broker();
  mqttClient.loop();
  delay(500);
}
