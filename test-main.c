#include "raylib.h"

#define ARRLEN(X) (sizeof(X)/sizeof((X)[0]))

int main(void) {
	const int screenWidth = 800;
	const int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "plot sine");

    SetTargetFPS(2);

    Rectangle plot_bg = {
        .width = 600,
        .height = 350,
        .x = (screenWidth - 600) / 2,
        .y = (screenHeight - 350) / 2
    };

    int values[] = {3, 5, 2, 3, 7, 8, 8, 8, 3, 2, 4}; // 0 - 10

    int plot_width = 7;
    int plot_height = 10;

    int roll = 0;

    while(!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(WHITE);

            DrawRectangleRec(plot_bg, BLUE);


            if(roll < plot_width){
                roll++;
            }

            for(int i = 0; i <= roll; i++){
                int x = plot_bg.width - (roll - i) * (plot_bg.width / plot_width) + plot_bg.x;
                int y = plot_bg.height - values[i] * (plot_bg.height / plot_height) + plot_bg.y;

                if(i > 0) {

                    int x1 = plot_bg.width - (roll - i + 1) * (plot_bg.width / plot_width) + plot_bg.x;
                    int y1 = plot_bg.height - values[i - 1] * (plot_bg.height / plot_height) + plot_bg.y;
                    
                    DrawLine(x, y, x1, y1, WHITE);
                }

                DrawCircle(x, y, 3, RED);
            }

        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
