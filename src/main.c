#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://broker.emqx.io:1883"
#define CLIENTID    "emqx_tictac"
#define TOPIC       "esp32/tictac"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

char * curmsg;
char gameBoard[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
int count = 0;
bool ai = false;
bool player = false;
bool quit = false;
volatile MQTTClient_deliveryToken deliveredtoken;
MQTTClient client;

void showGameboard()
{
    printf("-------\n");
    for(int i = 0; i < 9; i++)
    {
        if(i == 0||i==3||i==6)
        {
            printf("|");
        }
        printf("%c|", gameBoard[i]);
        if(i==2||i==5)
        {
            printf("\n");
        }
        else if(i==8)
        {
            printf("\n");
            printf("-------\n");
        }
    }
}

void checkWin()
{
    int count = 0;
    for(int i = 0; i < 9; i++)
    {
        if(gameBoard[i] == 'X' || gameBoard[i] == 'O')
        {
            count++;
        }
    }

    if(gameBoard[0] == gameBoard[1] && gameBoard[0] == gameBoard[2])
    {
        if (gameBoard[0] == 'X')
        {
            printf("Player X wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
        else
        {
            printf("Player O wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
    }
    else if(gameBoard[3] == gameBoard[4] && gameBoard[3] == gameBoard[5])
    {
        if (gameBoard[3] == 'X')
        {
            printf("Player X wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
        else
        {
            printf("Player O wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
    }
    else if(gameBoard[6] == gameBoard[7] && gameBoard[6] == gameBoard[8])
    {
        if (gameBoard[6] == 'X')
        {
            printf("Player X wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
        else
        {
            printf("Player O wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
    }
    if(gameBoard[0] == gameBoard[3] && gameBoard[0] == gameBoard[6])
    {
        if (gameBoard[0] == 'X')
        {
            printf("Player X wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
        else
        {
            printf("Player O wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
    }
    else if(gameBoard[1] == gameBoard[4] && gameBoard[1] == gameBoard[7])
    {
        if (gameBoard[1] == 'X')
        {
            printf("Player X wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
        else
        {
            printf("Player O wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
    }
    else if(gameBoard[2] == gameBoard[5] && gameBoard[2] == gameBoard[8])
    {
        if (gameBoard[2] == 'X')
        {
            printf("Player X wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
        else
        {
            printf("Player O wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
    }
    else if(gameBoard[0] == gameBoard[4] && gameBoard[0] == gameBoard[8])
    {
        if (gameBoard[0] == 'X')
        {
            printf("Player X wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
        else
        {
            printf("Player O wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
    }
    else if(gameBoard[6] == gameBoard[4] && gameBoard[6] == gameBoard[2])
    {
        if (gameBoard[6] == 'X')
        {
            printf("Player X wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
        else
        {
            printf("Player O wins\n");
            for(int i = 0; i < 9; i++)
            {
                gameBoard[i] = i+0;
            }
        }
    }
}


void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    curmsg = message->payload;
    printf("%s", curmsg);

    if (*curmsg == '0')
    {
        printf("Quitting...");
        quit = true;
    }

    if(ai == false && player == false)
    {
        if(*curmsg == 'a')
        {
            printf("\nGame set to Player VS AI...\n");
            ai = true;
        }
        else if (*curmsg == 'p')
        {
            printf("\nGame set to Player VS Player");
            player = true;
        }
    }
    else if(count == 0)
    {
        if(*curmsg == '1')
        {
            if(gameBoard[0] == 'X' || gameBoard[0] == 'O')
            {
                printf("Space already taken...");
            }
            else
            {
                gameBoard[0] = 'X';
                printf("%c", gameBoard[0]);
                count++;
            }
        }
        if(*curmsg == '2')
        {
            if(gameBoard[1] == 'X' || gameBoard[1] == 'O')
            {
                printf("Space already taken...");
            }
            else
            {
                gameBoard[1] = 'X';
                printf("%c", gameBoard[1]);
                count++;
            }
        }
        if(*curmsg == '3')
        {
            if(gameBoard[2] == 'X' || gameBoard[2] == 'O')
            {
                printf("Space already taken...");
            }
            else
            {
                gameBoard[2] = 'X';
                printf("%c", gameBoard[2]);
                count++;
            }
        }
        if(*curmsg == '4')
        {
            if(gameBoard[3] == 'X' || gameBoard[3] == 'O')
            {
                printf("Space already taken...");
            }
            else
            {
                gameBoard[3] = 'X';
                printf("%c", gameBoard[3]);
                count++;
            }
        }
        if(*curmsg == '5')
        {
            if(gameBoard[4] == 'X' || gameBoard[4] == 'O')
            {
                printf("Space already taken...");
            }
            else
            {
                gameBoard[4] = 'X';
                printf("%c", gameBoard[4]);
                count++;
            }
        }
        if(*curmsg == '6')
        {
            if(gameBoard[5] == 'X' || gameBoard[5] == 'O')
            {
                printf("Space already taken...");
            }
            else
            {
                gameBoard[5] = 'X';
                printf("%c", gameBoard[5]);
                count++;
            }
        }
        if(*curmsg == '7')
        {
            if(gameBoard[6] == 'X' || gameBoard[6] == 'O')
            {
                printf("Space already taken...");
            }
            else
            {
                gameBoard[6] = 'X';
                printf("%c", gameBoard[6]);
                count++;
            }
        }
        if(*curmsg == '8')
        {
            if(gameBoard[7] == 'X' || gameBoard[7] == 'O')
            {
                printf("Space already taken...");
            }
            else
            {
                gameBoard[7] = 'X';
                printf("%c", gameBoard[7]);
                count++;
            }
        }
        if(*curmsg == '9')
        {
            if(gameBoard[8] == 'X' || gameBoard[8] == 'O')
            {
                printf("Space already taken...");
            }
            else
            {
                gameBoard[8] = 'X';
                printf("%c", gameBoard[8]);
                count++;
            }
        }
    }
    else if (count == 1 && player == true)
    {
        printf("\nIt is not your turn...\n");
    }
    putchar('\n');
    showGameboard();
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;
    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);
    while (quit != true)
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
