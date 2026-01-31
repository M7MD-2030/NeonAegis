#include "raylib.h"

int main() {
    
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "NeonAegis");
    SetTargetFPS(60);

    
    while (!WindowShouldClose()) {
        
        

        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawFPS(10, 10);
        EndDrawing();
    }

    
    CloseWindow();

    return 0;
}
