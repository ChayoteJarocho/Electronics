#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define D9 99
#define D10 1010

#ifndef STASSID
#define STASSID "Set your WiFi SSID"
#define STAPSK  "Set your WiFi Password"
#define HOSTNAME "shelflights"
#endif

const char* ssid      = STASSID;
const char* password  = STAPSK;
const String hostName = HOSTNAME;
String ip = "";

ESP8266WebServer server(80);

String HtmlButton(String action, String number)
{
  String msg = "<div class='col-xs-6 col-sm-6 col-md-6 col-lg-6'><input type='button' class='btn btn-primary' value='" + action + " " + number + "'  onclick='" + action + "(" + number + ")' /></div>";
  return msg;
}

String HtmlButtonRow(String number)
{
  String msg = "";
  msg += "<div class='row'>";
  msg += HtmlButton("ON", number);
  msg += HtmlButton("OFF", number);
  msg += "<div class='col-xs-6 col-sm-6 col-md-6 col-lg-6' id='Result" + number + "'>&nbsp;</div>";
  msg += "</div>";
  return msg;
}

String AjaxFunction(String action, String resultID)
{
  String msg = "function " + action + "(n){";
  msg +=  "$.ajax({";
  msg += "method: 'GET',";
  msg += "url: '/" + action + "' + n,";
  msg += "success: function(msg){ $('#" + resultID + "' + n).html('Success: ' + msg); },";
  msg += "error: function(msg){ $('#" + resultID + "' + n).html('Error: ' + msg); }";
  msg += "});";
  msg += "}";
  return msg;
}

String GetInstructionsHtml()
{
  String msg = "<html><head><title>" + hostName + "</title>";
  msg += "<meta charset='utf-8'>";
  msg += "<meta http-equiv='X-UA-Compatible' content='IE=edge'>";
  msg += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  msg += "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js'></script>";
  msg += "<script type='text/javascript'>";
  msg += AjaxFunction("ON", "Result");
  msg += AjaxFunction("OFF", "Result");
  msg += "</script>";
  msg += "<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css' integrity='sha384-HSMxcRTRxnN+Bdg0JdbxYKrThecOKuH5zCYotlSAcp1+c8xmyTe9GYg1l9a69psu' crossorigin='anonymous'>";
  msg += "<script src='https://stackpath.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js' integrity='sha384-aJ21OjlMXNL5UyIl/XNwTMqvzeRMZH2w8c5cRVpzpU8Y5bApTppSuUkhZXN0VxHd' crossorigin='anonymous'></script>";
  msg += "</head><body><div class='container-fluid'>";
  msg += "<div class='row'><div class='col-xs-12 col-sm-12 col-md-12 col-lg-12'><b>Hostname:</b> " + hostName + "</div></div>";
  msg += "<div class='row'><div class='col-xs-12 col-sm-12 col-md-12 col-lg-12'><b>IP:</b> " + ip + "</div></div>";
  msg += HtmlButtonRow("4");
  msg += "</div></body></html>";
  return msg;
}

String GetInstructionsPlain()
{
  String msg = "";
  msg += " - Hostname: " + hostName + "\n";
  msg += " - IP:       " + ip + "\n";
  msg += " - Options:\n";
  msg += " - " + hostName + "/ON" + "\n";
  msg += " - " + hostName + "/OFF" + "\n";
  msg += "\n";

  return msg;
}

void reply(int responseNumber, String responseType, String message)
{
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Response number: " + String(responseNumber));
  Serial.println("Response type:" + responseType);
  server.send(responseNumber, responseType, message);
  digitalWrite(LED_BUILTIN, LOW);
}

void replyPlain(String message)
{
  reply(200, "text/plain", message);
}

void replyHtml(String message)
{
  reply(200, "text/html", message);
}

void handleRoot()
{
  replyHtml(GetInstructionsHtml());
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  reply(404, "text/plain", message);
}

void handlePin(int pin, int action)
{
  digitalWrite(pin, action);
  String strAction = action == 0 ? "OFF" : "ON";
  String message = strAction + String(pin);
  reply(200, "text/html", message);
}

void handleOn(int pin)
{
  handlePin(pin, HIGH);
}

void handleOff(int pin)
{
  handlePin(pin, LOW);
}

void On4(){ handleOn(D4); }
void Off4(){ handleOff(D4); }

String GetWifiStatusString(int wifistatus)
{
  switch (wifistatus)
  {
    case WL_CONNECTED:
      return "WL_CONNECTED";
      
    case WL_NO_SHIELD:
      return "WL_NO_SHIELD";
      
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";
      
    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";
      
    case WL_SCAN_COMPLETED:
      return "WL_SCAN_COMPLETED";
      
    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";
      
    case WL_CONNECTION_LOST:
      return "WL_CONNECTION_LOST";
      
    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";
      
    default:
      return "UNRECOGNIZED WIFI STATUS";
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D4, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.flush();
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\nConnecting to ssid: " + String(ssid));

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int wifistatus = WiFi.status();
  while (wifistatus != WL_CONNECTED)
  {
    Serial.println("WiFi status: " + GetWifiStatusString(wifistatus));
    delay(1000);
    wifistatus = WiFi.status();
  }

  if (MDNS.begin(hostName))
  {
    Serial.println("MDNS responder started.");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.on("/ON4",  On4);
  server.on("/OFF4", Off4);

  server.begin();
  Serial.println("Server started.");

  ip = WiFi.localIP().toString();
  Serial.println(GetInstructionsPlain());

  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  server.handleClient();
  MDNS.update();
}