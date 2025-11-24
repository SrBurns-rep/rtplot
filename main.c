#include "raylib.h"
#include "ring.h"

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#define ARRLEN(X) (sizeof(X)/sizeof((X)[0]))

#define BUFFLEN 256

void draw_ring_plot_samples(SampleRing *ring, Rectangle rect, Color bg, Color fg) {

    int x0 = 0, x1 = 0, y0 = 0, y1 = 0;

    int xt = 0, yt = 0;

    DrawRectangle(rect.x - 1, rect.y, rect.width, rect.height + 1, bg);

    for(size_t i = 0; i < ring->head; i++) {

        if(i > 0) {
            x0 = ((ring->head - i + 1) * rect.width) / ring->size;
            x1 = ((ring->head - i) * rect.width) / ring->size;
            x0 = rect.width - x0;
            x1 = rect.width - x1;
            x0 += rect.x;
            x1 += rect.x;

            // Invert Y axis
            y0 = 1023 - ring->samples[i - 1].raw;
            y1 = 1023 - ring->samples[i].raw;
            y0 = (y0 * rect.height) / 1024;
            y1 = (y1 * rect.height) / 1024;
            y0 += rect.y;
            y1 += rect.y;

            DrawLine(x0, y0, x1, y1, fg);

            if(i == 1) {
                xt = x0;
                yt = y0;
            }
        }
    }
    
    for(size_t i = ring->head; i < ring->size; i++) {

        if(i > ring->head + 1) {
            x0 = ((ring->size - i + 1 + ring->head) * rect.width) / ring->size;
            x1 = ((ring->size - i + ring->head) * rect.width) / ring->size;
            x0 = rect.width - x0;
            x1 = rect.width - x1;
            x0 += rect.x;
            x1 += rect.x;

            // Invert Y axis
            y0 = 1023 - ring->samples[i - 1].raw;
            y1 = 1023 - ring->samples[i].raw;
            y0 = (y0 * rect.height) / 1024;
            y1 = (y1 * rect.height) / 1024;
            y0 += rect.y;
            y1 += rect.y;
                        
            DrawLine(x0, y0, x1, y1, fg);
        }
    }

    float dist = sqrt((xt - x1) * (xt - x1) + (yt - y1) * (yt - y1));

    if(dist <= rect.height) {
        DrawLine(xt, yt, x1, y1, fg);    
    }
    
}

int main(void) {

    Color bg_plot_color = (Color) {0x2C, 0x2C, 0x2C, 0xFF};
    Color bg_color =      (Color) {0x33, 0x33, 0x33, 0xFF};

    SampleRing *ring = ring_init(500);

	const int screenWidth = 800;
	const int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "plot sine");

	SetTargetFPS(60);

    srand(time(0));

	int i = 0;

	int vals[1000] = {0};

	for(size_t j = 0; j < ARRLEN(vals); j++) {
	    vals[j] = j;
	}

	while(!WindowShouldClose()) {

        i++;

        Sample s = {0};

        // s.raw = (int)(600/2) * asinf(sinf((float)i*M_PI/50.f)) + (1000/2);

        s.raw = vals[i % ARRLEN(vals)];
        
        s.is_valid = 1;
        
        ring_push_sample(ring, s);

        Rectangle rect = {
            .width = 600,
            .height = 300,
            .x = (screenWidth - 600) / 2,
            .y = (screenHeight - 300) / 2
            };

		BeginDrawing();
		{
			ClearBackground(bg_color);
		    draw_ring_plot_samples(ring, rect, bg_plot_color, RAYWHITE);
		}
		EndDrawing();
	}

	free(ring);
	
	CloseWindow();
	return 0;	
}
