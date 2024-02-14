#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

const char* ssid = "Akubhi";
//const char* ssid = "PhayouneWIFI";
const char* wifi_password = "PhayounE2014";

const int EEPROM_SIZE = 64;

const int RELAY_PIN = 26;
const int RELAY_PIN_2 = 27;

WebServer server(80);

String username = "admin";
String password = "1234";

bool eepromInitialized = false;
bool isAuthenticated = false;

void initializeEEPROM() {
  // Check if EEPROM has been initialized
  char c = EEPROM.read(0);
  if (c == '1') {
    // EEPROM already initialized
    return;
  }

  // Save default username and password to EEPROM
  for (int i = 0; i < username.length(); ++i) {
    EEPROM.write(i + 1, username[i]);
  }
  for (int i = 0; i < password.length(); ++i) {
    EEPROM.write(username.length() + i + 2, password[i]);
  }
  EEPROM.write(0, '1'); // Mark EEPROM as initialized
  EEPROM.commit();

  eepromInitialized = true;
}

void printEEPROMValues() {
  Serial.println("EEPROM Values:");
  Serial.print("Username: ");
  for (int i = 0; i < 32; ++i) {
    char c = EEPROM.read(i + 1);
    if (c == '\0') break;
    Serial.print(c);
  }
  Serial.println();
  Serial.print("Password: ");
  for (int i = 0; i < 32; ++i) {
    char c = EEPROM.read(33 + i);
    if (c == '\0') break;
    Serial.print(c);
  }
  Serial.println();
}


void handleRoot() {
    printEEPROMValues(); // Print EEPROM values when accessing the login page

  server.send(200, "text/html", 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<style>"
    "body { font-family: Arial, sans-serif; margin: 0; padding: 0; }"
    ".container { max-width: 800px; margin: auto; padding: 20px; }"
    "input[type=text], input[type=password], button { width: 100%; padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; box-sizing: border-box; }"
    "button { background-color: #4CAF50; color: white; border: none; cursor: pointer; }"
    "button:hover { opacity: 0.8; }"
    "</style>"
    "</head>"
    "<body>"
    "<div class='container'>"
    "<h1>Login Page</h1>"
    "<form action='/login' method='post'>"
    "<label for='username'><b>Username</b></label>"
    "<input type='text' placeholder='Enter Username' name='username' required>"
    "<label for='password'><b>Password</b></label>"
    "<input type='password' placeholder='Enter Password' name='password' required>"
    "<button type='submit'>Login</button>"
    "</form>"
    "</div>"
    "</body>"
    "</html>"
  );
}

// Define other handlers...
void handleUnauthorized() {
  server.send(401, "text/html", 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<style>"
    "body { font-family: Arial, sans-serif; margin: 0; padding: 0; text-align: center; }"
    ".container { max-width: 800px; margin: auto; padding: 20px; }"
    "h1 { color: #FF5733; }"
    "p { color: #333; }"
    "a.button {"
    "  background-color: #4CAF50;"
    "  border: none;"
    "  color: white;"
    "  padding: 15px 32px;"
    "  text-align: center;"
    "  text-decoration: none;"
    "  display: inline-block;"
    "  font-size: 16px;"
    "  margin: 4px 2px;"
    "  cursor: pointer;"
    "}"
    "a.button:hover {"
    "  background-color: #45a049;"
    "}"
    "</style>"
    "</head>"
    "<body>"
    "<div class='container'>"
    "<h1>Unauthorized</h1>"
    "<p>Incorrect username or password.</p>"
    "<a class='button' href='/'>Go back to Login</a>"
    "</div>"
    "</body>"
    "</html>"
  );
}
void handleHome() {
      if (!isAuthenticated) {
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "");
        return;
    }
  server.send(200, "text/html", 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<style>"
    "body { font-family: Arial, sans-serif; margin: 0; padding: 0; text-align: center; }"
    ".container { max-width: 800px; margin: auto; padding: 20px; }"
    "h1 { color: #333; }"
    "p { color: #333; }"
    "a.button {"
    "  background-color: #4CAF50;"
    "  border: none;"
    "  color: white;"
    "  padding: 15px 32px;"
    "  text-align: center;"
    "  text-decoration: none;"
    "  display: inline-block;"
    "  font-size: 16px;"
    "  margin: 4px 2px;"
    "  cursor: pointer;"
    "}"
    "a.button:hover {"
    "  background-color: #45a049;"
    "}"
    "</style>"
    "</head>"
    "<body>"
"<div class='container'>"
"<h1>Welcome to the Home Page!</h1>"
// Add a <span> element to display the date and time
    "<p id='datetime'></p>"
"<button id='relayButton' onclick='toggleRelay()' class='button relay-button'>Toggle Relay 1</button>"
"<button id='relayButton2' onclick='toggleRelay2()' class='button relay-button'>Toggle Relay 2</button><br>"
"<a href='/update' class='button update-button'>Update Username/Password</a>"
"<a href='/logout' class='button logout-button'>Logout</a>"
"</div>"
"<style>"
".button {"
"  background-color: #4CAF50;"
"  border: none;"
"  color: white;"
"  padding: 15px 32px;"
"  text-align: center;"
"  text-decoration: none;"
"  display: inline-block;"
"  font-size: 16px;"
"  margin: 4px 2px;"
"  cursor: pointer;"
"}"
".button:hover {"
"  opacity: 0.8;"
"}"
".relay-button {"
"  background-color: #008CBA;"
"}"
".update-button {"
"  background-color: #f44336;"
"}"
".logout-button {"
"  background-color: #555555;"
"}"
"</style>"
"<script>"
"function toggleRelay() {"
"  var xhr = new XMLHttpRequest();"
"  xhr.open('GET', '/toggle_relay', true);"
"  xhr.onload = function () {"
"    if (xhr.status === 200) {"
"      var button = document.getElementById('relayButton');"
"      if (xhr.responseText === 'on') {"
"        button.style.backgroundColor = 'red';"
"        button.innerText = 'Relay 1 ON';"
"      } else {"
"        button.style.backgroundColor = '';"
"        button.innerText = 'Relay 1 OFF';"
"      }"
"    }"
"  };"
"  xhr.send();"
"}"
"function toggleRelay2() {"
"  var xhr = new XMLHttpRequest();"
"  xhr.open('GET', '/toggle_relay_2', true);"
"  xhr.onload = function () {"
"    if (xhr.status === 200) {"
"      var button = document.getElementById('relayButton2');"
"      if (xhr.responseText === 'on') {"
"        button.style.backgroundColor = 'red';"
"        button.innerText = 'Relay 2 ON';"
"      } else {"
"        button.style.backgroundColor = '';"
"        button.innerText = 'Relay 2 OFF';"
"      }"
"    }"
"  };"
"  xhr.send();"
"}"
"</script>"

"<script>"
    // JavaScript code to update the date and time in real-time
    "function updateDateTime() {"
    "  var datetimeElement = document.getElementById('datetime');"
    "  var now = new Date();"
    "  var timezoneOffset = 7 * 60;" // UTC+7 timezone offset in minutes
    "  var utcTime = now.getTime() + (now.getTimezoneOffset() * 60000);"
    "  var adjustedTime = new Date(utcTime + (timezoneOffset * 60000));"
    "  var formattedDateTime = adjustedTime.toLocaleString();"
    "  datetimeElement.innerHTML = 'Current Date and Time (UTC+7): ' + formattedDateTime;"
    "}"
    "updateDateTime();" // Call the function initially
    "setInterval(updateDateTime, 1000);" // Update every second
    "</script>"

    "</body>"
    "</html>"
  );
}


void handleLogin() {
  String req_username = server.arg("username");
  String req_password = server.arg("password");

  // Read stored username and password from EEPROM
  String stored_username;
  String stored_password;
  for (int i = 0; i < 32; ++i) {
    char c = EEPROM.read(i + 1);
    if (c == '\0') break; // Stop reading if null terminator is encountered
    stored_username += c;
  }
  for (int i = 0; i < 32; ++i) {
    char c = EEPROM.read(33 + i);
    if (c == '\0') break; // Stop reading if null terminator is encountered
    stored_password += c;
  }
/*
  if (req_username.equals(stored_username) && req_password.equals(stored_password)) {
    server.sendHeader("Location", "/home");
    server.send(302, "text/plain", "");
  } else {
    server.sendHeader("Location", "/unauthorized");
    server.send(302, "text/plain", "");
  }
*/
if (req_username.equals(stored_username) && req_password.equals(stored_password)) {
    isAuthenticated = true;
    server.sendHeader("Location", "/home");
    server.send(302, "text/plain", "");
  } else {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  }

}


void handleLogout() {
  isAuthenticated = false; // Reset authentication
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}


/*
void handleLogin() {
  String req_username = server.arg("username");

  // Read stored username from EEPROM
  String stored_username;
  for (int i = 0; i < 32; ++i) {
    char c = EEPROM.read(i + 1);
    if (c == '\0') break; // Stop reading if null terminator is encountered
    stored_username += c;
  }

  if (req_username.equals(stored_username)) {
    server.sendHeader("Location", "/home");
    server.send(302, "text/plain", "");
  } else {
    server.sendHeader("Location", "/unauthorized");
    server.send(302, "text/plain", "");
  }
}
*/


void handleUpdatePage() {
  if (!isAuthenticated) {
    // User is not authenticated, redirect to login page
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
    return;
  }
  server.send(200, "text/html", 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<style>"
    "body { font-family: Arial, sans-serif; margin: 0; padding: 0; text-align: center; }"
    ".container { max-width: 800px; margin: auto; padding: 20px; }"
    "h1 { color: #333; }"
    "p { color: #333; }"
    "label { display: block; margin-bottom: 10px; }"
    "input[type='text'], input[type='password'], button { width: 100%; padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; box-sizing: border-box; }"
    "button { background-color: #4CAF50; color: white; border: none; cursor: pointer; }"
    "button:hover { opacity: 0.8; }"
    "</style>"
    "</head>"
    "<body>"
    "<div class='container'>"
    "<h1>Update Username/Password</h1>"
    "<form action='/update' method='post'>"
    "<label for='new_username'><b>New Username</b></label>"
    "<input type='text' placeholder='Enter New Username' name='new_username' required>"
    "<label for='new_password'><b>New Password</b></label>"
    "<input type='password' placeholder='Enter New Password' name='new_password' required>"
    "<button type='submit'>Update</button>"
    "</form>"
    "</div>"
    "</body>"
    "</html>"
  );
}

void handleUpdate() {
  // Retrieve new username and password from the request
  String new_username = server.arg("new_username");
  String new_password = server.arg("new_password");

  // Perform validation and update logic (replace this with your own logic)
  if (new_username.length() > 0 && new_password.length() > 0) {
    // Update username and password (for example, save to EEPROM)
    for (int i = 0; i < new_username.length(); ++i) {
      EEPROM.write(i + 1, new_username[i]);
    }
    EEPROM.write(new_username.length() + 1, '\0'); // Null-terminate username
    for (int i = 0; i < new_password.length(); ++i) {
      EEPROM.write(33 + i, new_password[i]); // Write password starting from address 33
    }
    EEPROM.write(33 + new_password.length(), '\0'); // Null-terminate password
    bool success = EEPROM.commit(); // Commit changes to EEPROM

    if (success) {
      // Redirect to the update success page after successful update
      server.sendHeader("Location", "/update_success");
      server.send(302, "text/plain", "");
    } else {
      // If EEPROM write failed, handle error or redirect back to update page
      server.sendHeader("Location", "/update"); // Redirect back to update page
      server.send(302, "text/plain", "");
    }
  } else {
    // If new username or password is empty, redirect back to update page
    server.sendHeader("Location", "/update");
    server.send(302, "text/plain", "");
  }
}



void handleUpdateSuccess() {
    printEEPROMValues(); // Print EEPROM values when accessing the login page
  server.send(200, "text/html",
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<style>"
    "body { font-family: Arial, sans-serif; margin: 0; padding: 0; text-align: center; background-color: #f2f2f2; }"
    ".container { max-width: 800px; margin: auto; padding: 20px; }"
    "h1 { color: #333; }"
    "p { color: #333; }"
    "button { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; }"
    "button:hover { background-color: #45a049; }"
    "</style>"
    "</head>"
    "<body>"
    "<div class='container'>"
    "<h1>Success!</h1>"
    "<p>Username and password updated successfully.</p>"
    "<a href='/home'><button>Go to Home</button></a>"
    "</div>"
    "</body>"
    "</html>"
  );
}



void handleNotFound() {
  if (!isAuthenticated && server.uri() != "/") {
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "");
        return;
    }
  if (server.method() == HTTP_GET && server.uri() == "/update") {
    handleUpdatePage();
  } else {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  }
}


void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);

  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // Print the local IP address

  EEPROM.begin(EEPROM_SIZE);

  // Check if EEPROM has been initialized
  char c = EEPROM.read(0);
  if (c == '1') {
    eepromInitialized = true;
  }

  // Initialize EEPROM if not already initialized
  if (!eepromInitialized) {
    initializeEEPROM();
  }

  server.on("/", handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/home", handleHome);
  server.on("/unauthorized", handleUnauthorized);
  server.on("/update", HTTP_GET, handleUpdatePage); // Serve update page for GET requests
  server.on("/update", HTTP_POST, handleUpdate);   // Handle update logic for POST requests
  server.on("/update_success", HTTP_GET, handleUpdateSuccess);
  server.onNotFound(handleNotFound);
  server.on("/toggle_relay", HTTP_GET, handleToggleRelay);
  server.on("/toggle_relay_2", HTTP_GET, handleToggleRelay2);
  server.on("/logout", handleLogout); // Add this line to handle logout
  server.begin();
  Serial.println("HTTP server started");
}


void toggleRelay() {
  static bool relayState = false;
  relayState = !relayState;
  digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
}

void handleToggleRelay() {
  // Toggle relay state
  toggleRelay();

  // Send response to indicate relay state
  server.send(200, "text/plain", digitalRead(RELAY_PIN) == HIGH ? "on" : "off");
}

void toggleRelay2() {
  static bool relayState2 = false;
  relayState2 = !relayState2;
  digitalWrite(RELAY_PIN_2, relayState2 ? HIGH : LOW);
}

void handleToggleRelay2() {
  // Toggle relay 2 state
  toggleRelay2();

  // Send response to indicate relay state
  server.send(200, "text/plain", digitalRead(RELAY_PIN_2) == HIGH ? "on" : "off");
}


void loop() {
  server.handleClient();
}
