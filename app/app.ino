/*
  Web Server interface to control 4 pins tied to remote of RC car
*/
#include <SPI.h>
#include <Ethernet.h>

// network settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 177 };
byte gateway[] = { 192, 168, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
const int PORT = 80;

// pins
const int F_PIN = 7;
const int B_PIN = 6;
const int L_PIN = 5;
const int R_PIN = 4;

// input (reads http requests)
String input;

// server / port
EthernetServer server(PORT);

void setup() {
  pinMode(F_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  pinMode(L_PIN, OUTPUT);
  pinMode(R_PIN, OUTPUT);

  digitalWrite(F_PIN, HIGH);
  digitalWrite(B_PIN, HIGH);
  digitalWrite(L_PIN, HIGH);
  digitalWrite(R_PIN, HIGH);

  // start ethernet and server
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.begin(9600);

  // debugging
  Serial.println("server started");
}

void loop() {
  // wait until server is available
  EthernetClient client = server.available();
  if (client) {
    // as long as client is connected and available
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        // read char by char HTTP request
        if (input.length() < 100) {
          // store characters to string
          input += c;
        }

        // if HTTP request has ended
        if (c == '\n') {

          // debugging
          Serial.println(input);

          // headers
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");

          // print empty line to separate headers from response
          client.println();

          // response
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("<meta charset=\"utf-8\">");
          client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<title>Test</title>");
          client.println("<style>");
          client.println("ul{float:left;margin:0;padding:0;width:50%;}");
          client.println("li{display:block;margin-bottom:10px;}");
          client.println("a{background:#f2f2f2;color:#333;display:block;padding:10px;text-align:center;text-decoration:none;}");
          client.println("</style>");
          client.println("</head>");
          client.println("<body>");
          client.println("<ul>");
          client.println("<li><a href=\"?forward\">Forward</a></li>");
          client.println("<li><a href=\"?back\">Back</a></li>");
          client.println("<li><a href=\"?stop\">STOP</a></li>");
          client.println("</ul>");
          client.println("<ul>");
          client.println("<li><a href=\"?left\">Left</a></li>");
          client.println("<li><a href=\"?right\">Right</a></li>");
          client.println("<li><a href=\"?straight\">STRAIGHT</a></li>");
          client.println("</ul>");
          client.println("</body>");
          client.println("</html>");

          // needed to ensure request/response happens properly
          delay(1);

          // stop client (send response)
          client.stop();

          // write to pins based on query string
          if (input.indexOf("?stop") > 0) {
            digitalWrite(F_PIN, HIGH);
            digitalWrite(B_PIN, HIGH);
          } else {
            if (input.indexOf("?forward") > 0) {
              digitalWrite(F_PIN, LOW);
              digitalWrite(B_PIN, HIGH);
            }

            if (input.indexOf("?back") > 0) {
              digitalWrite(B_PIN, LOW);
              digitalWrite(F_PIN, HIGH);
            }
          }

          if (input.indexOf("?straight") > 0) {
            digitalWrite(L_PIN, HIGH);
            digitalWrite(R_PIN, HIGH);
          } else {
            if (input.indexOf("?left") > 0) {
              digitalWrite(L_PIN, LOW);
              digitalWrite(R_PIN, HIGH);
            }

            if (input.indexOf("?right") > 0) {
              digitalWrite(R_PIN, LOW);
              digitalWrite(L_PIN, HIGH);
            }
          }

          // reset input
          input = "";
        }
      }
    }
  }
}
