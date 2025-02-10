#include <LittleFS.h>
#include <string>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

// Which pin on the ESP32 is connected to the NeoPixels?
#define PIN          D3

// How many NeoPixels LEDs are attached to the ESP32?
#define NUMPIXELS   64

// We define birghtness of NeoPixel LEDs
#define BRIGHTNESS  20

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Set SSID
const char* ssid     = "esp32";

// Set password
const char* password = "12345678";

// Set Access Point IP
IPAddress   apIP(10, 10, 10, 1);

// flag for running the game
bool running = false;

// Set web server to listen port 80
WiFiServer server(80);

void setup()
{
    // Set Access Point configuration
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00
    WiFi.softAP(ssid,password);

    // Set NeoPixel configuration
    matrix.setBrightness(BRIGHTNESS);

    // Start NeoPixel library with all LEDs off
    matrix.begin();

    // Show settings of LEDs in NeoPixel array
    matrix.show();

    Serial.begin(115200);
    if (!LittleFS.begin()) {
      Serial.println("LittleFS mount failed");
      return;
    }

    // Start web server
    server.begin();

    for (int i = 0; i < 64; i++) {
      matrix.setPixelColor(i, matrix.Color(139, 0, 0));
    }
    matrix.show();
}


void respondToClient(WiFiClient& client) {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    client.println("HTTP/1.1 500 Internal Server Error");
    return;
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  while (file.available()) {
    client.write(file.read());
  }
  file.close();
  client.println();
}


std::array<std::array<int, 8>, 8> game_map = {{
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
}};

void print_matrix() {
  int x = 0;
  int y = 0;
  int i;
  while(y < 8)
  {
    x = 0;
    while(x < 8)
    {
      if(game_map[y][x] == 1)
        matrix.setPixelColor(i, matrix.Color(0, 207, 209));
      if(game_map[y][x] == 0)
        matrix.setPixelColor(i, matrix.Color(139, 0, 0));
      i++;
      x++;
    }
    y++;
  }
  matrix.show();
}

int get_value(int y, int x)
{
  std::array<std::array<int, 2>, 8> indices = {{{x+1, y+1}, {x+1, y}, {x+1, y-1}, {x, y+1}, {x, y-1}, {x-1, y+1}, {x-1, y}, {x-1, y-1}}};
  for (int i = 0; i < 8; i++) {
    std::array<int, 2> index = indices[i];
    if (index[0] < 0)
      indices[i][0] = 7;
    if (index[0] > 7)
      indices[i][0] = 0;
    if (index[1] < 0)
      indices[i][1] = 7;
    if (index[1] > 7)
      indices[i][1] = 0;
  }

  int sum = 0;
  for (int i = 0; i < 8; i++) {
    int x_ = indices[i][0];
    int y_ = indices[i][1];
    sum += game_map[y_][x_];
  }
  if (sum < 2)
    return 0;
  if (sum == 2)
    return game_map[y][x];
  if (sum == 3)
    return 1;
  return 0;
}

void game_of_life(void)
{
	std::array<std::array<int, 8>, 8> new_map;
  int x = 0;
  int y = 0;
  while(y < 8)
  {
    x = 0;
    while(x < 8)
    {
			// logic
			new_map[y][x] = get_value(y, x);
      x++;
    }
    y++;
  }
  game_map = new_map;
  print_matrix();
}

void handleClientRequest(const String &request) {
	if (request.startsWith("GET /game")){
		running = true;
		return;
	}
  if (!request.startsWith("GET /"))
    return;

	if (request.startsWith("GET /home")) {
		reset_matrix();
    print_matrix();
		return;
	}

  if(!std::isdigit(request.c_str()[5]))
    return;

  int index = std::stoi(request.c_str() + 5) - 1;
  uint32_t color = matrix.getPixelColor(index);
	game_map[index / 8][index % 8] = !game_map[index / 8][index % 8];
	print_matrix();
}

void reset_matrix() {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			game_map[y][x] = 0;
		}
	}
}

int prev_mil = 0;

void loop(){
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {
		running = false;
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        // checking if byte is either a newline of a carriage return
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          // carriage returns and newlines are not added to the current line
          if (request.length() == 0)
          {
            respondToClient(client);
            break;
          } else
            request = "";
        } else if (c != '\r') {
          request += c;
        }
        else {
          Serial.println(request);
          handleClientRequest(request);
        }
      }
    }
    // close the connection:
    client.stop();
  }
  int cur_mil = millis();
	if (running && cur_mil - prev_mil >= 500)
	{
		prev_mil = cur_mil;
		game_of_life();
	}
}
