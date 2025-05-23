#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid     = "****";
const char* password = "****";

// Create an instance of the WebServer on port 80
WebServer server(80);

// Motor A
int motor1Pin1 = 26; 
int motor1Pin2 = 27; 
int motor2Pin1 = 25;
int motor2Pin2 = 33;

int motor3Pin1 = 18;
int motor3Pin2 = 19;
int motor4Pin1 = 22;
int motor4Pin2 = 23;
// Setting PWM properties

int enable1Pin = 14; 
int enable2Pin = 32;
int enable3Pin = 5;
int enable4Pin = 21;

#define LEFT_CHANNEL 0
#define RIGHTCHANNEL 1

const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;
String valueString = String(100);


void handleRoot() {
  const char html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
      html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; }
      .button { -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; background-color: #4CAF50; border: none; color: white; padding: 12px 28px; text-decoration: none; font-size: 26px; margin: 1px; cursor: pointer; }
      .button2 {background-color: #555555;}
    </style>
    <script>
      function moveForward() { fetch('/forward'); }
      function moveLeft() { fetch('/left'); }
      function stopRobot() { fetch('/stop'); }
      function moveRight() { fetch('/right'); }
      function moveReverse() { fetch('/reverse'); }

      function updateMotorSpeed(pos) {
        document.getElementById('motorSpeed').innerHTML = pos;
        fetch(`/speed?value=${pos}`);
      }
    </script>
  </head>
  <body>
    <h1>ESP32 Motor Control</h1>
    <p><button class="button" onclick="moveForward()">FORWARD</button></p>
    <div style="clear: both;">
      <p>
        <button class="button" onclick="moveLeft()">LEFT</button>
        <button class="button button2" onclick="stopRwith
    <p><button class="button" onclick="moveReverse()">REVERSE</button></p>
    <p>Motor Speed: <span id="motorSpeed">100</span></p>
    <input type="range" min="0" max="100" step="25" id="motorSlider" oninput="updateMotorSpeed(this.value)" value="100"/>
  </body>
  </html>)rawliteral";
  server.send(200, "text/html", html);
}

void stop_motors() {
  // Stop the DC motor
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);

    digitalWrite(motor3Pin1, LOW);
    digitalWrite(motor3Pin2, LOW);
    digitalWrite(motor4Pin1, LOW);
    digitalWrite(motor4Pin2, LOW);

    digitalWrite(2, LOW);
}

void move_forward_left() {
  // Move the DC motor forward at maximum speed
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
}

void move_backward_left() {
  // Move the DC motor forward at maximum speed
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
}

void move_forward_right() {
  // Move the DC motor forward at maximum speed
    digitalWrite(motor3Pin1, LOW);
    digitalWrite(motor3Pin2, HIGH);
    digitalWrite(motor4Pin1, LOW);
    digitalWrite(motor4Pin2, HIGH);
}

void move_backward_right() {
  // Move the DC motor forward at maximum speed
    digitalWrite(motor3Pin1, HIGH);
    digitalWrite(motor3Pin2, LOW);
    digitalWrite(motor4Pin1, HIGH);
    digitalWrite(motor4Pin2, LOW);
}

void move_forward() {
  // Move the DC motor forward at maximum speed
    move_forward_left();
    move_forward_right();
    digitalWrite(2, HIGH);
}

void move_backward() {
  // Move the DC motor backwards at maximum speed
    move_backward_left();
    move_backward_right();
    digitalWrite(2, HIGH);

}

void rotate_left() {
  // Move the DC motor forward at maximum speed
    move_forward_left();
    move_backward_right();
    digitalWrite(2, HIGH);

}

void rotate_right() {
  // Move the DC motor forward at maximum speed
    move_backward_left();
    move_forward_right();
    digitalWrite(2, HIGH);

}

void handleForward() {
  Serial.println("Forward");
  move_forward();
  server.send(200);
}

void handleLeft() {
  Serial.println("Left");
  rotate_left();
  server.send(200);
}

void handleStop() {
  Serial.println("Stop");
  stop_motors();   
  server.send(200);
}

void handleRight() {
  Serial.println("Right");
  rotate_right();
  server.send(200);
}

void handleReverse() {
  Serial.println("Reverse");
  move_backward();          
  server.send(200);
}

void handleSpeed() {
  if (server.hasArg("value")) {
    valueString = server.arg("value");
    int value = valueString.toInt();
    if (value == 0) {
      ledcWrite(LEFT_CHANNEL, 0);
      ledcWrite(RIGHTCHANNEL, 0);
      
      digitalWrite(motor1Pin1, LOW); 
      digitalWrite(motor1Pin2, LOW); 
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, LOW);   
    } else { 
      dutyCycle = map(value, 25, 100, 200, 255);
      ledcWrite(LEFT_CHANNEL, dutyCycle);
      ledcWrite(RIGHTCHANNEL, dutyCycle);
    }
  }
  server.send(200);
}

void setup() {
  Serial.begin(115200);
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  pinMode(motor3Pin1, OUTPUT);
  pinMode(motor3Pin2, OUTPUT);
  pinMode(motor4Pin1, OUTPUT);
  pinMode(motor4Pin2, OUTPUT);

  pinMode(2, OUTPUT);

  // Configure PWM Pins
  ledcSetup(LEFT_CHANNEL, freq, resolution);
  ledcSetup(RIGHTCHANNEL, freq, resolution);
  
  ledcAttachPin(enable1Pin, LEFT_CHANNEL);
  ledcAttachPin(enable2Pin, LEFT_CHANNEL);
  ledcAttachPin(enable3Pin, RIGHTCHANNEL);
  ledcAttachPin(enable4Pin, RIGHTCHANNEL);
    
  // Initialize PWM with 255 duty cycle
  ledcWrite(LEFT_CHANNEL, 255);
  ledcWrite(RIGHTCHANNEL, 255);
  
  Serial.begin(115200);

  Serial.println("Done setting up pins");

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

  // Define routes
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/left", handleLeft);
  server.on("/stop", handleStop);
  server.on("/right", handleRight);
  server.on("/reverse", handleReverse);

  server.on("/speed", handleSpeed);

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();
}
