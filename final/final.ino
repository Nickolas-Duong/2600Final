#include <WiFi.h>
#include <PubSubClient.h>
#include <Keypad.h>

#define RED 4
#define GREEN 2
#define BLUE 15
#define YELLOW 5

// define the symbols on the buttons of the keypad
char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[4] = {14, 27, 26, 25}; // connect to the row pinouts of the keypad
byte colPins[4] = {13, 21, 22, 23};   // connect to the column pinouts of the keypad

// initialize an instance of class NewKeypad
Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);


// WiFi
const char *ssid = "jinzo"; // Enter your WiFi name
const char *password = "dearbornsix";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "esp32/tictac";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;
int count = 0;
int player = 0;
char gameBoard [9][1];
bool inPlay = true;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);

  boardSetup();
 // Set software serial baud to 115200;
 Serial.begin(115200);
 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 // publish and subscribe
 client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
 String res;
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    res += (char) payload[i];
  }
  // Serial.println(res);
  if (res == "0")
  {
    inPlay = false;
  }

  if(count == 0 && res != "Catscratch" && count == 0 && res != "Game End" && inPlay == true)
  {
    if(gameBoard[res.toInt()-1][0] == 'X' || gameBoard[res.toInt()-1][0] == 'O')
    {
      Serial.print("Already taken by ");
      Serial.print(gameBoard[res.toInt()-1][0]);
      showUnallow();
    }
    else
    {
      Serial.print("X at ");
      Serial.print(res);
      gameBoard[res.toInt()-1][0] = 'X';
      showAllow();
      count++;
    }
  }
  else if (count == 1 && res != "Catscratch" && count == 1 && res != "Game End" && inPlay == true)
  {
    if(gameBoard[res.toInt()-1][0] == 'X' || gameBoard[res.toInt()-1][0] == 'O')
    {
      Serial.print("Already taken by ");
      Serial.print(gameBoard[res.toInt()-1][0]);
      showUnallow();
    }
    else
    {
      Serial.print("O at ");
      Serial.print(res);
      gameBoard[res.toInt()-1][0] = 'O';
      showAllow();
      count = 0;
    }
  }
  else if (res == "Catscratch" && inPlay == true)
  {
    Serial.print("New Game...");
    showScratch();
    count = 0;
  }
  else if (res == "Game End" && inPlay == true)
  {
    Serial.print("New Game...");
    showEnd();
    count = 0;
  }
  else if (inPlay != true)
  {
    Serial.print("Someone has quit... game has ended...");
  }
 Serial.println();
 showBoard();
 gameCheck();
 Serial.println("-----------------------");
}

void loop() {
  char keyPressed = myKeypad.getKey();
  if (count == 0 && inPlay == true)
  {
    if (keyPressed == '1')
    {
      client.publish(topic, "1");  
    }
    else if (keyPressed == '2')
    {
      client.publish(topic, "2");
    }
    else if (keyPressed == '3')
    {
      client.publish(topic, "3");
    }
    else if (keyPressed == '4')
    {
      client.publish(topic, "4");
    }
    else if (keyPressed == '5')
    {
      client.publish(topic, "5");
    }
    else if (keyPressed == '6')
    {
      client.publish(topic, "6");
    }
    else if (keyPressed == '7')
    {
      client.publish(topic, "7");
    }
    else if (keyPressed == '8')
    {
      client.publish(topic, "8");
    }
    else if (keyPressed == '9')
    {
      client.publish(topic, "9");
    }
    else if (keyPressed == '0');
    {
      client.publish(topic, "0");
    }
  }

 client.loop();
}

void gameCheck()
{
  int tempC = 0;  
  for(int i = 0; i < 9; i++)
  {
    if(gameBoard[i][0] == 'X' || gameBoard[i][0] == 'O')
    {
      tempC++;
    }
  }

    if(tempC == 9)
    {
      for(int i = 0; i < 9; i++)
      {
        gameBoard[i][0] = (char) i;
      }

      Serial.println("Catscratch... resetting...");
      client.publish(topic, "Catscratch");

      count = 0;

    }
    else if(gameBoard[0][0] == gameBoard[1][0] && gameBoard[0][0] == gameBoard[2][0])
    {
      Serial.print("Player ");
      Serial.print(gameBoard[0][0]);
      Serial.print(" Wins! ");
      Serial.println("Resetting...");

      for(int i = 0; i < 9; i++)
      {
        gameBoard[i][0] = (char) i;
      }
      count = 0;
      client.publish(topic, "Game End");

    }
    else if(gameBoard[3][0] == gameBoard[4][0] && gameBoard[3][0] == gameBoard[5][0])
    {
      Serial.print("Player ");
      Serial.print(gameBoard[3][0]);
      Serial.print(" Wins! ");
      Serial.println("Resetting...");

      for(int i = 0; i < 9; i++)
      {
        gameBoard[i][0] = (char) i;
      }
      count = 0;
      client.publish(topic, "Game End");

    }
    else if(gameBoard[6][0] == gameBoard[7][0] && gameBoard[6][0] == gameBoard[8][0])
    {
      Serial.print("Player ");
      Serial.print(gameBoard[6][0]);
      Serial.print(" Wins! ");
      Serial.println("Resetting...");

      for(int i = 0; i < 9; i++)
      {
        gameBoard[i][0] = (char) i;
      }
      count = 0;
      client.publish(topic, "Game End");

    }
    else if(gameBoard[0][0] == gameBoard[3][0] && gameBoard[0][0] == gameBoard[6][0])
    {
      Serial.print("Player ");
      Serial.print(gameBoard[0][0]);
      Serial.print(" Wins! ");
      Serial.println("Resetting...");

      for(int i = 0; i < 9; i++)
      {
        gameBoard[i][0] = (char) i;
      }
      count = 0;
      client.publish(topic, "Game End");

    }
    else if(gameBoard[1][0] == gameBoard[4][0] && gameBoard[1][0] == gameBoard[7][0])
    {
      Serial.print("Player ");
      Serial.print(gameBoard[1][0]);
      Serial.print(" Wins! ");
      Serial.println("Resetting...");

      for(int i = 0; i < 9; i++)
      {
        gameBoard[i][0] = (char) i;
      }
      count = 0;
      client.publish(topic, "Game End");

    }
    else if(gameBoard[2][0] == gameBoard[5][0] && gameBoard[2][0] == gameBoard[8][0])
    {
      Serial.print("Player ");
      Serial.print(gameBoard[6][0]);
      Serial.print(" Wins! ");
      Serial.println("Resetting...");

      for(int i = 0; i < 9; i++)
      {
        gameBoard[i][0] = (char) i;
      }
      count = 0;
      client.publish(topic, "Game End");

    }
    else if(gameBoard[0][0] == gameBoard[4][0] && gameBoard[0][0] == gameBoard[8][0])
    {
      Serial.print("Player ");
      Serial.print(gameBoard[0][0]);
      Serial.print(" Wins! ");
      Serial.println("Resetting...");

      for(int i = 0; i < 9; i++)
      {
        gameBoard[i][0] = (char) i;
      }
      count = 0;
      client.publish(topic, "Game End");

    }
    else if(gameBoard[6][0] == gameBoard[4][0] && gameBoard[6][0] == gameBoard[2][0])
    {
      Serial.print("Player ");
      Serial.print(gameBoard[6][0]);
      Serial.print(" Wins! ");
      Serial.println("Resetting...");

      for(int i = 0; i < 9; i++)
      {
        gameBoard[i][0] = (char) i;
      }

      count = 0;
      client.publish(topic, "Game End");

    }
}

void boardSetup()
{
  showAllow();
  showUnallow();
  showScratch();
  showEnd();
  for(int i = 0; i < 9; i++)
  {
    gameBoard[i][0] = (char) i;
  }
}

void showBoard()
{
  Serial.println("-------");
  Serial.print("|");
  Serial.print(gameBoard[0][0]);
  Serial.print("|");
  Serial.print(gameBoard[1][0]);
  Serial.print("|");
  Serial.print(gameBoard[2][0]);
  Serial.println("|");
  Serial.print("|");
  Serial.print(gameBoard[3][0]);
  Serial.print("|");
  Serial.print(gameBoard[4][0]);
  Serial.print("|");
  Serial.print(gameBoard[5][0]);
  Serial.println("|");
  Serial.print("|");
  Serial.print(gameBoard[6][0]);
  Serial.print("|");
  Serial.print(gameBoard[7][0]);
  Serial.print("|");
  Serial.print(gameBoard[8][0]);
  Serial.println("|");
  Serial.println("-------");
}

void showAllow()
{
  digitalWrite(GREEN, HIGH);
  delay(500);
  digitalWrite(GREEN, LOW);
}

void showUnallow()
{
  digitalWrite(RED, HIGH);
  delay(500);
  digitalWrite(RED, LOW);
}

void showEnd()
{
  digitalWrite(BLUE, HIGH);
  delay(500);
  digitalWrite(BLUE, LOW);
}

void showScratch()
{
  digitalWrite(YELLOW, HIGH);
  delay(500);
  digitalWrite(YELLOW, LOW);
}