#include <ESP8266WiFi.h>

const char *ssid = "Bogdan's S22";
const char *password = "BogdanWifi";

const char *HTML_CONTENT = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Wireless Garage Door Control</title>
    <style>
        body {
            text-align: center;
            font-family: Verdana, Tahoma, sans-serif;
            overflow: hidden;
            background-color: #fff9da;
        }
        button {
            background-color: #3254a8; /* Blue */
            border: none;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            font-family: Verdana, Tahoma, sans-serif;
            margin: 4px 2px;
            cursor: pointer;
            border-radius: 2rem;
            box-shadow: 5px 5px 5px #3f3e3e;
            margin-top: 1rem;
        }

        .app {
            width: 100vw;
            height: 100vh;
            overflow: hidden;
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: column;
        }
    </style>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
</head>
<body>
    <div class="app">
        <h1>Garage Door Control</h1>
        <button onclick="window.location.href='/Open'">Open Garage Door</button>
        <button onclick="window.location.href='/Close'">Close Garage Door</button>
    </div>
</body>
</html>
)";

WiFiServer server(80);  // Create a web server on port 80

void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT);  // Set the LED pin mode
  
  delay(10);

  // Connect to Wi-Fi network
  Serial.println();
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();  // Start the server
}

void loop() {
  WiFiClient client = server.available();  // Check for incoming clients

  if (client) {
    Serial.println("New Client connected.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Send HTTP response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.print(HTML_CONTENT);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // Process HTTP GET requests for "Open" and "Close"
        if (currentLine.endsWith("GET /Open")) {
          Serial.println("Opening Garage Door...");
          client.println("Garage Door is Opening...");
          digitalWrite(5, HIGH);  // Example action
        }
        if (currentLine.endsWith("GET /Close")) {
          Serial.println("Closing Garage Door...");
          client.println("Garage Door is Closing...");
          digitalWrite(5, LOW);  // Example action
        }
      }
    }

    client.stop();
    Serial.println("Client disconnected.");
  }
}
