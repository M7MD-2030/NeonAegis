#include "raylib.h"


class core
{
private:
    float radius;
    float Health;
public:
    core(float r, float h) : radius(r), Health(h)
    {
    }
    ~core()
    {
    }
    float getRadius() { return radius; }
    float getHealth() { return Health; }
    void setRadius(float r) { radius = r; }
    void setHealth(float h) { Health = h; }
    void takeDamage(float dmg) { Health -= dmg; }
    void heal(float amt) { Health += amt; }
    void displayStatus()
    {
        DrawText(TextFormat("Core Health: %.2f", Health), 10, 30, 20, RED);
    }
    void displayCore()
    {
        DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, radius, RAYWHITE);
    }
};




int main() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    InitWindow(screenWidth, screenHeight, "Core Status Display");
    SetTargetFPS(60);
    core myCore(50.0f, 100.0f);
    Color bgColor = { 40,28,60,255 };

    
    while (!WindowShouldClose()) {        
        BeginDrawing();
            ClearBackground(bgColor);
            DrawFPS(10, 10);
            myCore.displayStatus();
            myCore.displayCore();
        EndDrawing();
    }

    
    CloseWindow();

    return 0;
}
