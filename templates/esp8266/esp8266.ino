// Example of how to run an esp8266
// Make sure to change the ssid, password and hostname

// TODO: Configure VSCode to compile and upload Arduino code:
// https://learn.sparkfun.com/tutorials/efficient-arduino-programming-with-arduino-cli-and-visual-studio-code/all

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
#define STASSID "SSID"
#define STAPSK  "PASSWORD"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;
const String hostName      = "esp8266";
String ip = "";

ESP8266WebServer server(80);

String HtmlButton(String action, String number)
{
  String msg = "<div class='col-xs-4 col-sm-4 col-md-4 col-lg-4'><input type='button' class='btn btn-primary' value='" + action + " " + number + "'  onclick='" + action + "(" + number + ")' /></div>";
  return msg;
}

String HtmlButtonRow(String number)
{
  String msg = "";
  msg += "<div class='row'>";
  msg += HtmlButton("ON", number);
  msg += HtmlButton("OFF", number);
  msg += "<div class='col-xs-4 col-sm-4 col-md-4 col-lg-4' id='Result" + number + "'>&nbsp;</div>";
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
  msg += "<div class='row'><div class='col-xs-12 col-sm-12 col-md-12 col-lg-12'>Hostname: " + hostName + "</div></div>";
  msg += "<div class='row'><div class='col-xs-12 col-sm-12 col-md-12 col-lg-12'>IP: " + ip + "</div></div>";
  for (int i = 0; i < 11; i++)
  {
    msg += HtmlButtonRow(String(i));
  }
  msg += "</div></body></html>";
  return msg;
}

String GetInstructionsPlain()
{
  String msg = "";
  msg += " - Hostname: " + hostName + "\n";
  msg += " - IP:       " + ip + "\n";
  msg += " - Options:\n";
  for (int i = 0; i < 11; i++)
  {
    msg += " - " + hostName + "/ON" + String(i) + "\n";
    msg += " - " + hostName + "/OFF" + String(i) + "\n";
  }
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

void On0(){ handleOn(D0); }
void On1(){ handleOn(D1); }
void On2(){ handleOn(D2); }
void On3(){ handleOn(D3); }
void On4(){ handleOn(D4); }
void On5(){ handleOn(D5); }
void On6(){ handleOn(D6); }
void On7(){ handleOn(D7); }
void On8(){ handleOn(D8); }
void On9(){ handleOn(D9); }
void On10(){ handleOn(D10); }

void Off0(){ handleOff(D0); }
void Off1(){ handleOff(D1); }
void Off2(){ handleOff(D2); }
void Off3(){ handleOff(D3); }
void Off4(){ handleOff(D4); }
void Off5(){ handleOff(D5); }
void Off6(){ handleOff(D6); }
void Off7(){ handleOff(D7); }
void Off8(){ handleOff(D8); }
void Off9(){ handleOff(D9); }
void Off10(){ handleOff(D10); }

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
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(D9, OUTPUT);
  pinMode(D10, OUTPUT);

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

  server.on("/ON0",  On0);
  server.on("/OFF0", Off0);
  server.on("/ON1",  On1);
  server.on("/OFF1", Off1);
  server.on("/ON2",  On2);
  server.on("/OFF2", Off2);
  server.on("/ON3",  On3);
  server.on("/OFF3", Off3);
  server.on("/ON4",  On4);
  server.on("/OFF4", Off4);
  server.on("/ON5",  On5);
  server.on("/OFF5", Off5);
  server.on("/ON6",  On6);
  server.on("/OFF6", Off6);
  server.on("/ON7",  On7);
  server.on("/OFF7", Off7);
  server.on("/ON8",  On8);
  server.on("/OFF8", Off8);
  server.on("/ON9",  On9);
  server.on("/OFF9", Off9);
  server.on("/ON10",  On10);
  server.on("/OFF10", Off10);

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
