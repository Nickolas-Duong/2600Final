#include <WiFi.h>
#include <PubSubClient.h>
#include <Keypad.h>

#define ONE 4
#define TWO 2
#define THREE 15
#define FOUR 5
#define FIVE 34
#define SIX 35
#define SEVEN 32
#define EIGHT 33
#define NINE 18
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
  //set initial pins
  pinMode(ONE, OUTPUT);
  pinMode(TWO, OUTPUT);
  pinMode(THREE, OUTPUT);
  pinMode(FOUR, OUTPUT);
  pinMode(FIVE, OUTPUT);
  pinMode(SIX, OUTPUT);
  pinMode(SEVEN, OUTPUT);
  pinMode(EIGHT, OUTPUT);
  pinMode(NINE, OUTPUT);

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
  //if quit is selected
  if (res == "0")
  {
    inPlay = false;
  }

  //if the game is not a catscratch or game end
  if(count == 0 && res != "Catscratch" && count == 0 && res != "Game End" && inPlay == true)
  {
    //prevent if already taken
    if(gameBoard[res.toInt()-1][0] == 'X' || gameBoard[res.toInt()-1][0] == 'O')
    {
      Serial.print("Already taken by ");
      Serial.print(gameBoard[res.toInt()-1][0]);
    }
    //otherwise allow
    else
    {
      Serial.print("X at ");
      Serial.print(res);
      gameBoard[res.toInt()-1][0] = 'X';
      count++;
    }
  }
  //repeat for player 2
  else if (count == 1 && res != "Catscratch" && count == 1 && res != "Game End" && inPlay == true)
  {
    if(gameBoard[res.toInt()-1][0] == 'X' || gameBoard[res.toInt()-1][0] == 'O')
    {
      Serial.print("Already taken by ");
      Serial.print(gameBoard[res.toInt()-1][0]);
    }
    else
    {
      Serial.print("O at ");
      Serial.print(res);
      gameBoard[res.toInt()-1][0] = 'O';
      count = 0;
    }
  }
  //if scratch
  else if (res == "Catscratch" && inPlay == true)
  {
    Serial.print("New Game...");
    showScratch();
    count = 0;
  }
  //if game end
  else if (res == "Game End" && inPlay == true)
  {
    Serial.print("New Game...");
    count = 0;
  }
  //if quit
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
  //get keypad input
  char keyPressed = myKeypad.getKey();
  //if game is in play, get input and publish
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

//check game state
void gameCheck()
{
  int tempC = 0;

  //see if all spots are taken  
  for(int i = 0; i < 9; i++)
  {
    if(gameBoard[i][0] == 'X' || gameBoard[i][0] == 'O')
    {
      tempC++;
    }
  }

  //If match is made, state game is won, show end state, reset
  if(gameBoard[0][0] == gameBoard[1][0] && gameBoard[0][0] == gameBoard[2][0])
  {
    Serial.print("Player ");
    Serial.print(gameBoard[0][0]);
    Serial.print(" Wins! ");
    Serial.println("Resetting...");
    showEnd(gameBoard[0][0]);
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
    showEnd(gameBoard[3][0]);

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
    showEnd(gameBoard[6][0]);

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
    showEnd(gameBoard[0][0]);

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
    showEnd(gameBoard[1][0]);

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
    showEnd(gameBoard[2][0]);

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
    showEnd(gameBoard[0][0]);

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
    showEnd(gameBoard[6][0]);

    for(int i = 0; i < 9; i++)
    {
      gameBoard[i][0] = (char) i;
    }

    count = 0;
    client.publish(topic, "Game End");

  }
  //show catscratch if no matches
  else if(tempC == 9)
  {
    showScratch();

    Serial.println("Catscratch... resetting...");
    client.publish(topic, "Catscratch");
    for(int i = 0; i < 9; i++)
    {
      gameBoard[i][0] = (char) i;
    }
    count = 0;

  }
}

//set up initial game board
void boardSetup()
{
  //set all lights to off
  digitalWrite(ONE, LOW);
  digitalWrite(TWO, LOW);
  digitalWrite(THREE, LOW);
  digitalWrite(FOUR, LOW);
  digitalWrite(FIVE, LOW);
  digitalWrite(SIX, LOW);
  digitalWrite(SEVEN, LOW);
  digitalWrite(EIGHT, LOW);
  digitalWrite(NINE, LOW);

  //initialize board
  for(int i = 0; i < 9; i++)
  {
    gameBoard[i][0] = i + 0;
  }
}

//show game board
void showBoard()
{
  //show board, shine if X, blink if O
  Serial.println("-------");
  Serial.print("|");
  Serial.print(gameBoard[0][0]);
  if (gameBoard[0][0] == 'X')
  {
    digitalWrite(ONE, HIGH);
  }
  else if (gameBoard[0][0] == 'O')
  {
    digitalWrite(ONE, HIGH);
    delay(500);
    digitalWrite(ONE, LOW);
  }
  else
  {
    digitalWrite(ONE, LOW);
  }
  Serial.print("|");
  Serial.print(gameBoard[1][0]);
  if (gameBoard[1][0] == 'X')
  {
    digitalWrite(TWO, HIGH);
  }
  else if (gameBoard[1][0] == 'O')
  {
    digitalWrite(TWO, HIGH);
    delay(500);
    digitalWrite(TWO, LOW);
  }
  else
  {
    digitalWrite(TWO, LOW);
  }
  Serial.print("|");
  Serial.print(gameBoard[2][0]);
  if (gameBoard[2][0] == 'X')
  {
    digitalWrite(THREE, HIGH);
  }
  else if (gameBoard[2][0] == 'O')
  {
    digitalWrite(THREE, HIGH);
    delay(500);
    digitalWrite(THREE, LOW);
  }
  else
  {
    digitalWrite(THREE, LOW);
  }
  Serial.println("|");
  Serial.print("|");
  Serial.print(gameBoard[3][0]);
  if (gameBoard[3][0] == 'X')
  {
    digitalWrite(FOUR, HIGH);
  }
  else if (gameBoard[3][0] == 'O')
  {
    digitalWrite(FOUR, HIGH);
    delay(500);
    digitalWrite(FOUR, LOW);
  }
  else
  {
    digitalWrite(FOUR, LOW);
  }
  Serial.print("|");
  Serial.print(gameBoard[4][0]);
  if (gameBoard[4][0] == 'X')
  {
    digitalWrite(FIVE, HIGH);
  }
  else if (gameBoard[4][0] == 'O')
  {
    digitalWrite(FIVE, HIGH);
    delay(500);
    digitalWrite(FIVE, LOW);
  }
  else
  {
    digitalWrite(FIVE, LOW);
  }
  Serial.print("|");
  Serial.print(gameBoard[5][0]);
  if (gameBoard[5][0] == 'X')
  {
    digitalWrite(SIX, HIGH);
  }
  else if (gameBoard[5][0] == 'O')
  {
    digitalWrite(SIX, HIGH);
    delay(500);
    digitalWrite(SIX, LOW);
  }
  else
  {
    digitalWrite(SIX, LOW);
  }
  Serial.println("|");
  Serial.print("|");
  Serial.print(gameBoard[6][0]);
  if (gameBoard[6][0] == 'X')
  {
    digitalWrite(SEVEN, HIGH);
  }
  else if (gameBoard[6][0] == 'O')
  {
    digitalWrite(SEVEN, HIGH);
    delay(500);
    digitalWrite(SEVEN, LOW);
  }
  else
  {
    digitalWrite(SEVEN, LOW);
  }
  Serial.print("|");
  Serial.print(gameBoard[7][0]);
  if (gameBoard[7][0] == 'X')
  {
    digitalWrite(EIGHT, HIGH);
  }
  else if (gameBoard[7][0] == 'O')
  {
    digitalWrite(EIGHT, HIGH);
    delay(500);
    digitalWrite(EIGHT, LOW);
  }
  else
  {
    digitalWrite(EIGHT, LOW);
  }
  Serial.print("|");
  Serial.print(gameBoard[8][0]);
  if (gameBoard[8][0] == 'X')
  {
    digitalWrite(NINE, HIGH);
  }
  else if (gameBoard[8][0] == 'O')
  {
    digitalWrite(NINE, HIGH);
    delay(500);
    digitalWrite(NINE, LOW);
  }
  else
  {
    digitalWrite(NINE, LOW);
  }
  Serial.println("|");
  Serial.println("-------");
}

//Show end of game via LEDs
void showEnd(char letter)
{
  //If X, show in X shape, otherwise O shape
  if (letter == 'X')
  {
    digitalWrite(ONE, HIGH);
    digitalWrite(THREE, HIGH);
    digitalWrite(FIVE, HIGH);
    digitalWrite(SEVEN, HIGH);
    digitalWrite(NINE, HIGH);
    delay(500);
    digitalWrite(ONE, LOW);
    digitalWrite(THREE, LOW);
    digitalWrite(FIVE, LOW);
    digitalWrite(SEVEN, LOW);
    digitalWrite(NINE, LOW);
    delay(500);
    digitalWrite(ONE, HIGH);
    digitalWrite(THREE, HIGH);
    digitalWrite(FIVE, HIGH);
    digitalWrite(SEVEN, HIGH);
    digitalWrite(NINE, HIGH);
    delay(500);
    digitalWrite(ONE, LOW);
    digitalWrite(THREE, LOW);
    digitalWrite(FIVE, LOW);
    digitalWrite(SEVEN, LOW);
    digitalWrite(NINE, LOW);
  }
  else
  {
    digitalWrite(TWO, HIGH);
    digitalWrite(FOUR, HIGH);
    digitalWrite(SIX, HIGH);
    digitalWrite(EIGHT, HIGH);
    delay(500);
    digitalWrite(TWO, LOW);
    digitalWrite(FOUR, LOW);
    digitalWrite(SIX, LOW);
    digitalWrite(EIGHT, LOW);
    delay(500);
    digitalWrite(TWO, HIGH);
    digitalWrite(FOUR, HIGH);
    digitalWrite(SIX, HIGH);
    digitalWrite(EIGHT, HIGH);
    delay(500);
    digitalWrite(TWO, LOW);
    digitalWrite(FOUR, LOW);
    digitalWrite(SIX, LOW);
    digitalWrite(EIGHT, LOW);
    delay(500);
  }
}

//Show scratch via LED
void showScratch()
{
  //Blink all LEDs twice
  digitalWrite(ONE, HIGH);
  digitalWrite(TWO, HIGH);
  digitalWrite(THREE, HIGH);
  digitalWrite(FOUR, HIGH);
  digitalWrite(FIVE, HIGH);
  digitalWrite(SIX, HIGH);
  digitalWrite(SEVEN, HIGH);
  digitalWrite(EIGHT, HIGH);
  digitalWrite(NINE, HIGH);
  delay(500);
  digitalWrite(ONE, LOW);
  digitalWrite(TWO, LOW);
  digitalWrite(THREE, LOW);
  digitalWrite(FOUR, LOW);
  digitalWrite(FIVE, LOW);
  digitalWrite(SIX, LOW);
  digitalWrite(SEVEN, LOW);
  digitalWrite(EIGHT, LOW);
  digitalWrite(NINE, LOW);
  delay(500);
  digitalWrite(ONE, HIGH);
  digitalWrite(TWO, HIGH);
  digitalWrite(THREE, HIGH);
  digitalWrite(FOUR, HIGH);
  digitalWrite(FIVE, HIGH);
  digitalWrite(SIX, HIGH);
  digitalWrite(SEVEN, HIGH);
  digitalWrite(EIGHT, HIGH);
  digitalWrite(NINE, HIGH);
  delay(500);
  digitalWrite(ONE, LOW);
  digitalWrite(TWO, LOW);
  digitalWrite(THREE, LOW);
  digitalWrite(FOUR, LOW);
  digitalWrite(FIVE, LOW);
  digitalWrite(SIX, LOW);
  digitalWrite(SEVEN, LOW);
  digitalWrite(EIGHT, LOW);
  digitalWrite(NINE, LOW);
}