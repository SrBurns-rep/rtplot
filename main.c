#include "raylib.h"
#include "ring.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFFLEN 256

// how to plot this? 0 - 1023
void draw_ring_plot_samples(SampleRing *ring, Rectangle rect, Color bg, Color fg) {


    int x0, x1, y0, y1;

    DrawRectangle(rect.x, rect.y, rect.width, rect.height, bg);
    
    
    for(size_t i = ring->head; i < ring->size; i++) {

        if(i > ring->head + 1) {
            x0 = ((i - 1) * rect.width) / ring->size;
            x1 = (i * rect.width) / ring->size;
            x0 += rect.x;
            x1 += rect.x;
            
            y0 = (ring->samples[i - 1].raw * rect.height) / 1024;
            y1 = (ring->samples[i].raw * rect.height) / 1024;
            y0 += rect.y;
            y1 += rect.y;
                        
            DrawLine(x0, y0, x1, y1, fg);
        }
    }

    for(size_t i = 0; i < ring->head; i++) {

        if(i > 0) {
            x0 = ((i - 1) * rect.width) / ring->size;
            x1 = (i * rect.width) / ring->size;
            x0 += rect.x;
            x1 += rect.x;

            y0 = (ring->samples[i - 1].raw * rect.height) / 1024;
            y1 = (ring->samples[i].raw * rect.height) / 1024;
            y0 += rect.y;
            y1 += rect.y;

            DrawLine(x0, y0, x1, y1, fg);
        }
    }
}

int main(void) {

    SampleRing *ring = ring_init(100);

	const int screenWidth = 800;
	const int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "plot sine");

	SetTargetFPS(10);

	int i = 0;

	while(!WindowShouldClose()) {

        i++;

        Sample s = {0};
        s.raw = (int)(1022/2) * sin((float)i*M_PI/100.f) + (1022/2);
        s.is_valid = 1;
        
        ring_push_sample(ring, s);

        ring_print_samples(ring);

        Rectangle rect = {
            .width = 600,
            .height = 300,
            .x = (screenWidth - 600) / 2,
            .y = (screenHeight - 300) / 2
            };

		BeginDrawing();
		{
			ClearBackground(RED);
		    draw_ring_plot_samples(ring, rect, BLACK, RAYWHITE);
		}
		EndDrawing();
	}

	free(ring);
	
	CloseWindow();
	return 0;	
}
