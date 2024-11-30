#include <ESP8266WiFi.h>
#include <Servo.h>

const char *ssid = "Bogdan's S22";
const char *password = "BogdanWifi";
const int servoPin = 12;
Servo myServo;
const int infraredSensorPin = 4;

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

        button:disabled {
            background-color: #cccccc;
            cursor: not-allowed;
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
    <script>
        async function updateButtonStates() {
            const response = await fetch('/status');
            const { infrared } = await response.json();
            document.getElementById('closeBtn').disabled = infrared;
        }

        async function controlDoor(action) {
            await fetch(`/${action}`);
            updateButtonStates();
        }

        // Attach event listeners after the page loads
        window.onload = function () {
            document.getElementById('openBtn').addEventListener('click', function () {
                controlDoor('Open');
            });
            document.getElementById('closeBtn').addEventListener('click', function () {
                controlDoor('Close');
            });
            updateButtonStates();
        };
    </script>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
</head>
<body>
    <div class="app">
        <h1>Garage Door Control</h1>
        <button id="openBtn">Open Garage Door</button>
        <button id="closeBtn">Close Garage Door</button>
        <h1 id="text-content"></h1>
    </div>
</body>
</html>
)";

WiFiServer server(80);  // Create a web server on port 80

void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT);  // Set the LED pin mode
  myServo.attach(servoPin);

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
            // Handle specific endpoints
            if (currentLine.startsWith("GET /Open")) {
              Serial.println("Opening Garage Door...");
              myServo.write(90);
              delay(2000);
              myServo.write(0);
            } else if (currentLine.startsWith("GET /Close")) {
              if (!digitalRead(infraredSensorPin)) {
                Serial.println("Closing Garage Door...");
                myServo.write(0);
              } else {
                Serial.println("Cannot close the garage door due to obstruction.");
              }
            } else if (currentLine.startsWith("GET /status")) {
              bool infrared = digitalRead(infraredSensorPin);
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println();
              client.printf("{\"infrared\": %s}", infrared ? "true" : "false");
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }


    client.stop();
    Serial.println("Client disconnected.");
  }
}
