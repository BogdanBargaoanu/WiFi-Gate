#include <WiFi.h>
#include <Servo.h>

const char *ssid = "YourSSID";
const char *password = "YourPassword";
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
            try {
                const response = await fetch('/status');
                if (!response.ok) {
                    console.error('Failed to fetch status');
                    return;
                }
                const { infrared } = await response.json();
                document.getElementById('closeBtn').disabled = infrared;
            } catch (error) {
                console.error('Error updating button states:', error);
            }
        }

        async function controlDoor(action) {
            try {
                if (action === 'Open') {
                  const response = await fetch(`/${action}`);
                  if (!response.ok) {
                    console.error(`Failed to send ${action} command`);
                    return;
                  }
                  setTimeout(() => controlDoor('Close'), 5000);
                }
                else {
                  const response = await fetch(`/${action}`);
                  if (!response.ok) {
                    console.error(`Failed to send ${action} command`);
                    return;
                  }
                }
                updateButtonStates();
            } catch (error) {
                console.error(`Error controlling door (${action}):`, error);
            }
        }

        setInterval(updateButtonStates, 1000);

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

NetworkServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT);  // set the LED pin mode
  myServo.attach(servoPin);
  pinMode(infraredSensorPin, INPUT);
 
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                                // if you get a client,
    Serial.println("New Client connected.");   // print a message out the serial port
    String currentLine = "";                   // make a String to hold incoming data from the client
    String request = "";

    while (client.connected()) {               // loop while the client's connected
      if (client.available()) {                // if there's bytes to read from the client,
        char c = client.read();                // read a byte, then
        Serial.write(c);                       // print it out the serial monitor
        request += c;

        // End of the HTTP request headers
        if (c == '\n' && currentLine.length() == 0) {
          if (request.startsWith("GET /status")) {
            // Handle /status endpoint
            bool infrared = digitalRead(infraredSensorPin);
            Serial.print("Infrared Sensor State: ");
            Serial.println(infrared);

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close");
            client.println();
            client.printf("{\"infrared\": %s}", infrared ? "false" : "true");
          } else if (request.startsWith("GET /Open")) {
            // Handle /Open endpoint
            Serial.println("Opening Garage Door...");
            myServo.write(180);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.println("Garage door opened.");
          } else if (request.startsWith("GET /Close")) {
            // Handle /Close endpoint
            if (digitalRead(infraredSensorPin)) {
              Serial.println("Closing Garage Door...");
              myServo.write(0);
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Garage door closed.");
            } else {
              Serial.println("Cannot close the garage door due to obstruction.");
              client.println("HTTP/1.1 400 Bad Request");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Obstruction detected. Cannot close the door.");
            }
          } else {
            // Serve the HTML page
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.print(HTML_CONTENT);
          }
          break;
        }

        // Keep track of the current line
        if (c == '\n') {
          currentLine = "";
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // Close the client connection
    client.stop();
    Serial.println("Client disconnected.");
  }

}
