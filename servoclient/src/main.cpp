#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ezButton.h>

const char* ssid = "ibrar";
const char* password = "ibrarahmad";

ezButton button(15); // Pin for the button

using namespace websockets;
WebsocketsServer server;
WebsocketsClient client;

bool buttonPressed = false;
unsigned long lastButtonPressTime = 0;
const unsigned long buttonCooldownTime = 5000; // Cooldown time in milliseconds

void setup() {
  Serial.begin(115200);
  button.setDebounceTime(50); // Set debounce time to 50 milliseconds
  button.setCountMode(COUNT_FALLING);

  Serial.println();
  Serial.println("Setting AP...");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP Address: ");
  Serial.println(IP);

  server.listen(8888);
}

void loop() {
  if (server.poll()) {
    client = server.accept();
  }

  if (client.available()) {
    client.poll();
    button.loop(); // Call the button's loop function to update its state

    if (button.isPressed() && !buttonPressed && (millis() - lastButtonPressTime >= buttonCooldownTime)) {
      buttonPressed = true;
      lastButtonPressTime = millis();

      // Send message to the client
      client.send("Button pressed!");
      Serial.println("Button pressed");
    }
  }

  if (buttonPressed && (millis() - lastButtonPressTime >= buttonCooldownTime)) {
    buttonPressed = false;
  }
}
