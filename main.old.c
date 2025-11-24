#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFFLEN 256

int main(void) {
	const int screenWidth = 800;
	const int screenHeight = 450;

	int fd = open("/dev/ttyUSB0", O_RDWR | O_NONBLOCK);
	if(fd <= 0){
		perror("open");
		exit(EXIT_FAILURE);
	}

    char buff[256] = {0};

    char *device, *baud;

    snprintf(buff, "stty -F %s %s cs8 -cstopb -parity -icanon min 1 time 1", device, baud);

    //            \/ open and use this device                   
	system("stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parity -icanon min 1 time 1");

	InitWindow(screenWidth, screenHeight, "Move block with esp32");

	SetTargetFPS(60);

	char buff[BUFFLEN] = {0};

	const int rect_width = 100;
	const int rect_height = 100;

	int posX = (GetScreenWidth() / 2) - (rect_width / 2);
	int posY = (GetScreenHeight() / 2) - (rect_height / 2);

	int axis = 0;

	while(!WindowShouldClose()) {

		read(fd, buff, BUFFLEN);

		switch(buff[0]) {
			case '+':
				if(axis == 1) {
					posY += 10;
				} else {
					posX += 10;
				}
				break;
				
			case '-':
				if(axis == 1) {
					posY -= 10;
				} else {
					posX -= 10;
				}
				break;
				
			case '#':
				axis = 1 - axis;
				break;
		}

		buff[0] = 0;
	
		BeginDrawing();
		{
			ClearBackground(RAYWHITE);

			DrawRectangle(posX, posY, rect_width, rect_height, RED);

			if(axis == 1){
				DrawText("Y", 50, 50, 40, LIGHTGRAY);
			} else {
				DrawText("X", 50, 50, 40, LIGHTGRAY);
			}		
		}
		EndDrawing();
	}

	close(fd);
	
	CloseWindow();
	return 0;	
}
