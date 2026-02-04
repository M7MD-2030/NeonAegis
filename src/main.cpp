#include "raylib.h"
#include <vector>
#include <cmath>

// --- COLOR PALETTE ---
// UI & Background
const Color BG_DEEP_SPACE     = { 15, 15, 22, 255 };
const Color UI_TEXT_GREEN     = { 0, 228, 48, 255 };
const Color UI_BORDER_CYAN    = { 43, 250, 250, 255 };

// Core Colors
const Color CORE_INNER_WHITE  = { 240, 245, 255, 255 };
const Color CORE_OUTER_GLOW   = { 43, 250, 250, 255 };
const Color CORE_DAMAGE_FLASH = { 255, 0, 255, 255 };

// Enemy Colors (Descriptive Names)
const Color ENEMY_RED_STANDARD = { 230, 41, 55, 255 };   // Standard Enemy: Bright Red
const Color ENEMY_GREEN_FAST   = { 0, 228, 48, 255 };    // Fast Enemy: Neon Green
const Color ENEMY_BLUE_TANK    = { 0, 121, 241, 255 };   // Tank Enemy: Solid Blue


class Core
{
private:
    float radius;
    float Health;
public:
    Core(float r, float h) : radius(r), Health(h) {}
    
    // Getters / Setters
    float getRadius() { return radius; }
    
    void displayStatus()
    {
        const char* statusText = TextFormat("CORE INTEGRITY: %.0f%%", Health);
        int fontSize = 20;
        int textWidth = MeasureText(statusText, fontSize);
        int padding = 8; // Slightly larger padding looks cleaner

        DrawText(statusText, 20, 20, fontSize, UI_TEXT_GREEN);
        DrawRectangleLines(20 - padding, 20 - padding, textWidth + (padding * 2), fontSize + (padding * 2), UI_BORDER_CYAN);
    }

    void displayCore()
    {
        // Simple visual effect: Glow changes if mouse is over it (simulating damage/interaction)
        bool isHovered = CheckCollisionPointCircle(GetMousePosition(), { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 }, radius);
        Color currentGlow = isHovered ? CORE_DAMAGE_FLASH : CORE_OUTER_GLOW;

        DrawCircleGradient(GetScreenWidth() / 2, GetScreenHeight() / 2, radius + 8, currentGlow, BG_DEEP_SPACE);
        DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, radius, CORE_INNER_WHITE);
    }
};

// Simple Enemy Struct for visualization
struct Enemy {
    Vector2 pos;
    Color color;
    float size;
};

int main() {
    InitWindow(800, 600, "NeonAegis");
    SetTargetFPS(60);

    Core myCore(40.0f, 100.0f);

    // Create 3 dummy enemies to test colors
    std::vector<Enemy> enemies;
    enemies.push_back({ {200, 200}, ENEMY_RED_STANDARD, 20 });
    enemies.push_back({ {600, 150}, ENEMY_GREEN_FAST, 15 });   // Smaller & Green
    enemies.push_back({ {400, 500}, ENEMY_BLUE_TANK, 30 });    // Larger & Blue

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BG_DEEP_SPACE);
            
            // Draw Enemies
            for (const auto& enemy : enemies) {
                DrawCircleV(enemy.pos, enemy.size, enemy.color);
            }

            // Draw Core
            myCore.displayCore();
            myCore.displayStatus();
            
            // Draw Legend (Just to help you see which is which)
            DrawText("RED: Standard", 10, 500, 10, ENEMY_RED_STANDARD);
            DrawText("GREEN: Fast", 10, 520, 10, ENEMY_GREEN_FAST);
            DrawText("BLUE: Tank", 10, 540, 10, ENEMY_BLUE_TANK);

            DrawFPS(GetScreenWidth() - 80, 10);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}