#include "stdlib.h"
#include "fxlib.h"
#include "stdio.h"
#include "string.h"
#include "revolution.h"
#include "dispbios.h"
#include "bitarray.h"

#define ME              0
#define OTHER           1

#define START           0xFF
#define EXIT            0xFE
#define COORDS          0xFD
#define CRASH           0xFC
#define HELLO_HOST      0xFB
#define HELLO_CLIENT    0xFA
#define SPEED           0xF9
#define READY           0xF8

unsigned char playerX = 85, playerY = 32, direction = 0, receivedX = 0, receivedY = 0, isHost = 0, isConnected = 0, wonLast=2, speed = 1, level = 0, levels = 6;
unsigned char videoBuffer[1024];
int score = 0, otherScore = 0;
int delays[3] = {65, 50, 35};

void DrawBox(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char* buffer, char color)
{
    DrawLine(x1, y1, x2, y1, buffer, color);
    DrawLine(x1, y1, x1, y2, buffer, color);
    DrawLine(x2, y1, x2, y2, buffer, color);
    DrawLine(x1, y2, x2, y2, buffer, color);
    PlotPixel(x1, y1, buffer, color);
}

void reset()
{
    receivedX = 0;
    receivedY = 0;

    memset(videoBuffer, 0, 1024);

    if(level == 0) {
        DrawBox(0, 0, 127, 63, videoBuffer, BLACK);

        playerY = 32;
        direction = 0;
        if(isHost) {
            playerX = 42;
        } else {
            playerX = 85;
        }
    }
    if(level == 1) {
        playerY = 32;
        direction = 0;
        if(isHost) {
            playerX = 42;
        } else {
            playerX = 85;
        }
    }
    if(level == 2) {
        DrawBox(0, 0, 127, 63, videoBuffer, BLACK);
        DrawLine(64, 0, 64, 63, videoBuffer, BLACK);

        playerY = 32;
        direction = 0;
        if(isHost) {
            playerX = 32;
        } else {
            playerX = 96;
        }
    }
    if(level == 3) {
        DrawLine(0, 0, 127, 0, videoBuffer, BLACK);
        DrawLine(0, 63, 127, 63, videoBuffer, BLACK);
        DrawLine(64, 0, 64, 63, videoBuffer, BLACK);

        playerY = 32;
        direction = 0;
        if(isHost) {
            playerX = 42;
        } else {
            playerX = 85;
        }
    }
    if(level == 4) {
        DrawBox(0, 0, 127, 63, videoBuffer, BLACK);
        DrawLine(0, 23, 57, 23, videoBuffer, BLACK);
        DrawLine(0, 40, 57, 40, videoBuffer, BLACK);
        DrawLine(70, 23, 127, 23, videoBuffer, BLACK);
        DrawLine(70, 40, 127, 40, videoBuffer, BLACK);

        playerY = 32;
        if(isHost) {
            direction = 1;
            playerX = 5;
        } else {
            direction = 3;
            playerX = 122;
        }
    }
    if(level == 5) {
        DrawBox(0, 0, 127, 63, videoBuffer, BLACK);
        DrawBox(59, 5, 68, 58, videoBuffer, BLACK);
        DrawLine(8, 45, 30, 45, videoBuffer, BLACK);
        DrawLine(30, 45, 30, 63, videoBuffer, BLACK);
        DrawLine(119, 45, 97, 45, videoBuffer, BLACK);
        DrawLine(97, 45, 97, 63, videoBuffer, BLACK);

        playerY = 52;
        if(isHost) {
            direction = 3;
            playerX = 21;
        } else {
            direction = 1;
            playerX = 106;
        }
    }
}

int connect()
{
    unsigned char answer = 0;

    SerialTransmit(HELLO_HOST);
    Sleep(50);
    answer = SerialReceive();

    if(answer == HELLO_CLIENT) {
        return 1;
    }

    return 0;
}

void checkKeys()
{
    if(IsKeyDown(KEY_CTRL_EXIT) || IsKeyDown(KEY_CTRL_MENU)) {
        SerialTransmit(EXIT);
        Stop_SerialPort();
        Reset_Calc();
    }
    if((IsKeyDown(KEY_CTRL_UP) || IsKeyDown(KEY_CHAR_5) || IsKeyDown(KEY_CHAR_8)) && direction != 2) {
        direction = 0;
    }
    if((IsKeyDown(KEY_CTRL_RIGHT) || IsKeyDown(KEY_CHAR_3) || IsKeyDown(KEY_CHAR_6)) && direction != 3) {
        direction = 1;
    }
    if((IsKeyDown(KEY_CTRL_DOWN) || IsKeyDown(KEY_CHAR_2)) && direction != 0) {
        direction = 2;
    }
    if((IsKeyDown(KEY_CTRL_LEFT) || IsKeyDown(KEY_CHAR_1) || IsKeyDown(KEY_CHAR_4)) && direction != 1) {
        direction = 3;
    }
}

void move()
{
    if(direction == 0) {
        playerY--;
    }
    if(direction == 1) {
        playerX++;
    }
    if(direction == 2) {
        playerY++;
    }
    if(direction == 3) {
        playerX--;
    }
    if(playerX == 255) {
        playerX = 127;
    }
    if(playerX > 127) {
        playerX = 0;
    }
    if(playerY == 255) {
        playerY = 63;
    }
    if(playerY > 63) {
        playerY = 0;
    }
}

void startScreen()
{
    unsigned char answer;
    char scoreText[4], otherScoreText[4], statusText[22], speedText[22], levelText[22];
    int isReady = 0, is1Pressed = 0, is2Pressed = 0, is3Pressed = 0, isLeftPressed = 0, isRightPressed = 0;

    while(!isReady) {
        if(isHost) {
            if(isConnected) {
                strcpy(statusText, "Hosting");
            } else {
                strcpy(statusText, "Waiting");
            }
        } else {
            strcpy(statusText, "Connected");
        }

        sprintf(scoreText, "%d", score);
        sprintf(otherScoreText, "%d", otherScore);
        sprintf(speedText, "SPEED:  %d", speed + 1);
        sprintf(levelText, "LEVEL:  %d", level + 1);

        memset(videoBuffer, 0, 1024);

        DrawBox(0, 0, 127, 63, videoBuffer, BLACK);
        DrawLine(0, 8, 127, 8, videoBuffer, BLACK);
        DrawLine(0, 16, 127, 16, videoBuffer, BLACK);
        DrawLine(0, 24, 127, 24, videoBuffer, BLACK);
        DrawLine(64, 16, 64, 24, videoBuffer, BLACK);
        DrawLine(0, 32, 127, 32, videoBuffer, BLACK);
        DrawLine(0, 40, 127, 40, videoBuffer, BLACK);

        CoolText(32, 1, FONT_FIVE, "MTRON by rojet601", videoBuffer);
        CoolText(48, 9, FONT_FIVE, statusText, videoBuffer);
        CoolText(27, 17, FONT_FIVE, scoreText, videoBuffer);
        CoolText(91, 17, FONT_FIVE, otherScoreText, videoBuffer);
        if(wonLast == ME) {
            CoolText(48, 25, FONT_FIVE, "You won", videoBuffer);
        }
        if(wonLast == OTHER) {
            CoolText(48, 25, FONT_FIVE, "You lost", videoBuffer);   
        }
        if(isHost) {
            CoolText(2, 33, FONT_FIVE, speedText, videoBuffer);
            CoolText(65, 33, FONT_FIVE, levelText, videoBuffer);
            CoolText(2, 52, FONT_FIVE, "[1]-[3] SPEED", videoBuffer);
            CoolText(65, 52, FONT_FIVE, "         LEVEL", videoBuffer);
            DrawLine(65, 55, 71, 55, videoBuffer, BLACK);
            DrawLine(66, 55, 68, 53, videoBuffer, BLACK);
            DrawLine(66, 55, 68, 57, videoBuffer, BLACK);

            DrawLine(73, 55, 79, 55, videoBuffer, BLACK);
            DrawLine(79, 55, 77, 53, videoBuffer, BLACK);
            DrawLine(79, 55, 77, 57, videoBuffer, BLACK);
            if(isConnected) {
                CoolText(2, 43, FONT_FIVE, "[SHIFT] START", videoBuffer);
            }
        } else {
            CoolText(2, 43, FONT_FIVE, "Only the host can change", videoBuffer);
            CoolText(2, 52, FONT_FIVE, "the settings", videoBuffer);
        }

        DrawAll(videoBuffer);

        if(IsKeyDown(KEY_CTRL_EXIT) || IsKeyDown(KEY_CTRL_MENU)) {
            SerialTransmit(EXIT);
            Stop_SerialPort();
            Reset_Calc();
        }

        answer = SerialReceive();

        if(answer == EXIT) {
            Stop_SerialPort();
            Reset_Calc();
        }
        if(answer == START) {
            Sleep(50);
            speed = SerialReceive();
            level = SerialReceive();
            isReady = 1;
        }

        if(isHost) {
            if(!isConnected) {
                if(answer == HELLO_HOST) {
                    SerialTransmit(HELLO_CLIENT);
                    isConnected = 1;
                }
            } else {
                if(IsKeyDown(KEY_CTRL_SHIFT)) {
                    SerialTransmit(START);
                    SerialTransmit(speed);
                    SerialTransmit(level);
                    Sleep(50);
                    isReady = 1;
                }
            }
            if(!IsKeyDown(KEY_CHAR_1)) {
                is1Pressed = 0;
            }
            if(IsKeyDown(KEY_CHAR_1) && !is1Pressed) {
                is1Pressed = 1;
                speed = 0;
            }
            if(!IsKeyDown(KEY_CHAR_2)) {
                is2Pressed = 0;
            }
            if(IsKeyDown(KEY_CHAR_2) && !is2Pressed) {
                is2Pressed = 1;
                speed = 1;
            }
            if(!IsKeyDown(KEY_CHAR_3)) {
                is3Pressed = 0;
            }
            if(IsKeyDown(KEY_CHAR_3) && !is3Pressed) {
                is3Pressed = 1;
                speed = 2;
            }
            if(!IsKeyDown(KEY_CTRL_LEFT)) {
                isLeftPressed = 0;
            }
            if(IsKeyDown(KEY_CTRL_LEFT) && !isLeftPressed) {
                isLeftPressed = 1;
                level--;
                if(level == 255) {
                    level = levels - 1;
                }
            }
            if(!IsKeyDown(KEY_CTRL_RIGHT)) {
                isRightPressed = 0;
            }
            if(IsKeyDown(KEY_CTRL_RIGHT) && !isRightPressed) {
                isRightPressed = 1;
                level++;
                if(level > levels - 1) {
                    level = 0;
                }
            }
        }
    }
}

int AddIn_main(int isAppli, unsigned short OptionNum)
{
    int collided = 0;
    unsigned char answer;

    Bdisp_AllClr_DDVRAM();

    Init_SerialPort();
    Start_SerialPort();

    if(connect() == 0) {
        isHost = 1;
    }

    while(1) {
        startScreen();

        memset(videoBuffer, 0, 1024);
        DrawBox(0, 0, 127, 63, videoBuffer, BLACK);
        DrawBox(48, 27, 79, 37, videoBuffer, BLACK);
        CoolText(51, 29, FONT_FIVE, "Ready?", videoBuffer);
        DrawAll(videoBuffer);

        Sleep(1000);

        SerialTransmit(READY);
        while(SerialReceive() != READY) {
            ;
        }

        reset();

        while(1) {
            answer = SerialReceive();

            if(answer == CRASH) {
                score++;
                wonLast = ME;
                break;
            }
            if(answer == EXIT) {
                Reset_Calc();
            }
            if(answer == COORDS) {
                receivedX = SerialReceive();
                receivedY = SerialReceive();

                setBit(videoBuffer, 128, receivedX, receivedY, 1);
            }

            checkKeys();
            move();

            collided = getBit(videoBuffer, 128, playerX, playerY);
            if(collided) {
                otherScore++;
                wonLast = OTHER;
                SerialTransmit(CRASH);
                break;
            }

            setBit(videoBuffer, 128, playerX, playerY, 1);

            SerialTransmit(COORDS);
            SerialTransmit(playerX);
            SerialTransmit(playerY);

            DrawAll(videoBuffer);

            Sleep(delays[speed]);
        }
    }

    return 1;
}

#pragma section _BR_Size
unsigned long BR_Size;
#pragma section


#pragma section _TOP

int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section