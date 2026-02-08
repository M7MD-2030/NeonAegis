#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <iostream>

// --- CONSTANTS & COLORS ---
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const Color BG_DEEP_SPACE = {15, 15, 22, 255};
const Color UI_TEXT_GREEN = {0, 228, 48, 255};
const Color UI_BORDER_CYAN = {43, 250, 250, 255};
const Color BUTTON_HOVER = {43, 250, 250, 50};

const Color CORE_INNER_WHITE = {240, 245, 255, 255};
const Color CORE_OUTER_GLOW = {43, 250, 250, 255};
const Color CORE_DAMAGE_FLASH = {255, 0, 50, 255};

const Color ENEMY_RED_STANDARD = {230, 41, 55, 255};
const Color ENEMY_GREEN_FAST = {0, 228, 48, 255};
const Color ENEMY_BLUE_TANK = {0, 121, 241, 255};
const Color BULLET_COLOR = {255, 255, 0, 255};

// --- ENUMS ---
enum GameScreen { LOGO, TITLE, GAMEPLAY, SHOP, PAUSE, SETTINGS, ENDING };
enum EnemyType { STANDARD, FAST, TANK };

// --- UTILITY ---
float GetRandomFloat(float min, float max) {
    return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

// --- CLASSES ---

// 1. Particle System for explosions
struct Particle {
    Vector2 position;
    Vector2 velocity;
    float life;
    Color color;
    float size;
};

// 2. Projectile Class
class Projectile {
public:
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    float damage;

    Projectile(Vector2 pos, Vector2 target, float speed, float dmg) {
        position = pos;
        active = true;
        radius = 4.0f;
        damage = dmg;

        // Calculate direction vector
        Vector2 dir = Vector2Subtract(target, pos);
        dir = Vector2Normalize(dir);
        velocity = Vector2Scale(dir, speed);
    }

    void update(float dt) {
        position = Vector2Add(position, Vector2Scale(velocity, dt));
        // Deactivate if off screen
        if (position.x < 0 || position.x > SCREEN_WIDTH || position.y < 0 || position.y > SCREEN_HEIGHT) {
            active = false;
        }
    }

    void draw() {
        DrawCircleV(position, radius, BULLET_COLOR);
    }
};

// 3. Enemy Class
class Enemy {
public:
    Vector2 position;
    float speed;
    Color color;
    EnemyType type;
    float health;
    float maxHealth;
    bool active;
    float radius;
    int moneyValue;

    Enemy(EnemyType t, int difficultyLevel) {
        type = t;
        active = true;
        
        // Spawn Logic: Pick a random edge
        int side = GetRandomValue(0, 3);
        if (side == 0) position = { GetRandomFloat(0, SCREEN_WIDTH), -20 }; // Top
        else if (side == 1) position = { GetRandomFloat(0, SCREEN_WIDTH), SCREEN_HEIGHT + 20 }; // Bottom
        else if (side == 2) position = { -20, GetRandomFloat(0, SCREEN_HEIGHT) }; // Left
        else position = { SCREEN_WIDTH + 20, GetRandomFloat(0, SCREEN_HEIGHT) }; // Right

        // Stats based on Type
        if (type == STANDARD) {
            speed = 80.0f + (difficultyLevel * 2);
            maxHealth = 20.0f + (difficultyLevel * 5);
            color = ENEMY_RED_STANDARD;
            radius = 12.0f;
            moneyValue = 10;
        } else if (type == FAST) {
            speed = 140.0f + (difficultyLevel * 3);
            maxHealth = 10.0f + (difficultyLevel * 2);
            color = ENEMY_GREEN_FAST;
            radius = 8.0f;
            moneyValue = 20;
        } else if (type == TANK) {
            speed = 40.0f + (difficultyLevel * 1);
            maxHealth = 60.0f + (difficultyLevel * 10);
            color = ENEMY_BLUE_TANK;
            radius = 18.0f;
            moneyValue = 30;
        }
        health = maxHealth;
    }

    void update(float dt, Vector2 target) {
        // Move towards center
        Vector2 dir = Vector2Subtract(target, position);
        dir = Vector2Normalize(dir);
        position = Vector2Add(position, Vector2Scale(dir, speed * dt));
    }

    void draw() {
        if (type == STANDARD)
            DrawCircleV(position, radius, color);
        else if (type == FAST) {
            Vector2 v1 = { position.x, position.y - radius };
            Vector2 v2 = { position.x - radius, position.y + radius };
            Vector2 v3 = { position.x + radius, position.y + radius };
            DrawTriangle(v1, v2, v3, color);
        }
        else if (type == TANK)
            DrawRectangle(position.x - radius, position.y - radius, radius*2, radius*2, color);
        
        // Health bar
        float hpPct = health / maxHealth;
        DrawRectangle(position.x - 10, position.y - radius - 8, 20 * hpPct, 4, GREEN);
    }
};

// 4. Core Class
class Core {
private:
    float radius;
    float maxHealth;
    float currentHealth;
    float hitFlashTimer;

public:
    Core(float r, float h) : radius(r), maxHealth(h), currentHealth(h), hitFlashTimer(0) {}

    float getRadius() const { return radius; }
    float getHealth() const { return currentHealth; }
    float getMaxHealth() const { return maxHealth; }
    
    void heal(float amount) {
        currentHealth += amount;
        if (currentHealth > maxHealth) currentHealth = maxHealth;
    }

    void upgradeMaxHealth(float amount) {
        maxHealth += amount;
        currentHealth += amount;
    }

    void takeDamage(float amount) {
        currentHealth -= amount;
        hitFlashTimer = 0.15f; // Flash for 0.15 seconds
    }

    void update(float dt) {
        if (hitFlashTimer > 0) hitFlashTimer -= dt;
    }

    void draw() {
        Color currentGlow = (hitFlashTimer > 0) ? CORE_DAMAGE_FLASH : CORE_OUTER_GLOW;
        
        // Draw Glow
        DrawCircleGradient(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, radius + 15, currentGlow, BG_DEEP_SPACE);
        // Draw Core
        DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, radius, CORE_INNER_WHITE);
        
        // Draw Turret Cannon
        Vector2 center = { (float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2 };
        Vector2 mouse = GetMousePosition();
        Vector2 dir = Vector2Subtract(mouse, center);
        float angle = atan2(dir.y, dir.x) * RAD2DEG;
        
        Rectangle turretRec = { center.x, center.y, radius + 10, 10 };
        DrawRectanglePro(turretRec, {0, 5}, angle, GRAY);
    }
};

// --- GAME MANAGER ---
class Game {
public:
    GameScreen currentScreen;
    Core core;
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;
    std::vector<Particle> particles;
    
    // Stats
    int money;
    int score;
    float damageMultiplier;
    float fireRate; // Shots per second
    float timeSinceLastShot;
    float enemySpawnTimer;
    float difficultyTimer;
    int difficultyLevel;
    
    // Settings
    bool screenShakeEnabled;
    float shakeIntensity;

    Game() : core(30.0f, 100.0f), currentScreen(LOGO) {
        resetGame();
        screenShakeEnabled = true;
    }

    void resetGame() {
        core = Core(30.0f, 100.0f);
        enemies.clear();
        projectiles.clear();
        particles.clear();
        money = 0;
        score = 0;
        damageMultiplier = 10.0f;
        fireRate = 0.2f;
        timeSinceLastShot = 0;
        enemySpawnTimer = 0;
        difficultyLevel = 1;
        shakeIntensity = 0;
    }

    void spawnParticle(Vector2 pos, Color col, int count) {
        for(int i=0; i<count; i++) {
            Particle p;
            p.position = pos;
            float angle = GetRandomFloat(0, 360) * DEG2RAD;
            float speed = GetRandomFloat(50, 150);
            p.velocity = { cos(angle)*speed, sin(angle)*speed };
            p.life = GetRandomFloat(0.3f, 0.8f);
            p.color = col;
            p.size = GetRandomFloat(2, 5);
            particles.push_back(p);
        }
    }

    void update(float dt) {
        // Handle Global Keys
        if (IsKeyPressed(KEY_M)) currentScreen = (currentScreen == GAMEPLAY) ? PAUSE : GAMEPLAY;
        
        if (shakeIntensity > 0) {
            shakeIntensity -= dt * 30.0f;
            if(shakeIntensity < 0) shakeIntensity = 0;
        }

        switch (currentScreen) {
            case LOGO:
                if (GetTime() > 2.0) currentScreen = TITLE;
                break;
            case TITLE:
                if (IsKeyPressed(KEY_ENTER)) resetGame(), currentScreen = GAMEPLAY;
                if (IsKeyPressed(KEY_S)) currentScreen = SETTINGS;
                break;
            case GAMEPLAY:
                updateGameplay(dt);
                break;
            case SHOP:
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_B)) currentScreen = GAMEPLAY;
                break;
            case PAUSE:
                if (IsKeyPressed(KEY_ENTER)) currentScreen = GAMEPLAY;
                break;
            case SETTINGS:
                 if (IsKeyPressed(KEY_ESCAPE)) currentScreen = TITLE;
                 if (IsKeyPressed(KEY_SPACE)) screenShakeEnabled = !screenShakeEnabled;
                 break;
            case ENDING:
                if (IsKeyPressed(KEY_ENTER)) {
                    resetGame();
                    currentScreen = TITLE;
                }
                break;
        }
    }

    void updateGameplay(float dt) {
        core.update(dt);
        if (core.getHealth() <= 0) currentScreen = ENDING;

        // Spawning
        enemySpawnTimer -= dt;
        if (enemySpawnTimer <= 0) {
            EnemyType type = STANDARD;
            int rng = GetRandomValue(0, 100);
            if(rng > 80) type = TANK;
            else if (rng > 60) type = FAST;
            
            enemies.push_back(Enemy(type, difficultyLevel));
            enemySpawnTimer = 2.0f - (difficultyLevel * 0.1f); // Spawns get faster
            if(enemySpawnTimer < 0.3f) enemySpawnTimer = 0.3f;
        }
        
        // Difficulty scaling
        difficultyTimer += dt;
        if(difficultyTimer > 15.0f) {
            difficultyLevel++;
            difficultyTimer = 0;
        }

        // Shooting
        timeSinceLastShot += dt;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && timeSinceLastShot >= fireRate) {
            Vector2 center = { (float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2 };
            projectiles.push_back(Projectile(center, GetMousePosition(), 600.0f, damageMultiplier));
            timeSinceLastShot = 0;
        }

        // Update Projectiles
        for (auto& p : projectiles) p.update(dt);

        // Update Enemies & Collisions
        Vector2 center = { (float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2 };
        
        for (auto& e : enemies) {
            e.update(dt, center);
            
            // Enemy vs Core
            if (CheckCollisionCircles(e.position, e.radius, center, core.getRadius())) {
                core.takeDamage(10.0f);
                e.active = false;
                if(screenShakeEnabled) shakeIntensity = 10.0f;
                spawnParticle(e.position, RED, 10);
            }

            // Projectile vs Enemy
            for (auto& p : projectiles) {
                if (p.active && CheckCollisionCircles(e.position, e.radius, p.position, p.radius)) {
                    e.health -= p.damage;
                    p.active = false; // Bullet destroyed
                    spawnParticle(p.position, WHITE, 2);
                    
                    if (e.health <= 0) {
                        e.active = false;
                        money += e.moneyValue;
                        score += e.moneyValue * 10;
                        spawnParticle(e.position, e.color, 15);
                    }
                }
            }
        }

        // Update Particles
        for(auto& p : particles) {
            p.position = Vector2Add(p.position, Vector2Scale(p.velocity, dt));
            p.life -= dt;
        }

        // Cleanup
        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& p){ return !p.active; }), projectiles.end());
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e){ return !e.active; }), enemies.end());
        particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p){ return p.life <= 0; }), particles.end());
        
        // Shop Trigger
        if (IsKeyPressed(KEY_B)) currentScreen = SHOP;
    }

    void draw() {
        BeginDrawing();
        ClearBackground(BG_DEEP_SPACE);

        // Apply Screen Shake
        Camera2D cam = { 0 };
        cam.zoom = 1.0f;
        if(screenShakeEnabled && shakeIntensity > 0) {
            cam.offset = { GetRandomFloat(-shakeIntensity, shakeIntensity), GetRandomFloat(-shakeIntensity, shakeIntensity) };
        }
        
        BeginMode2D(cam);

        switch (currentScreen) {
            case LOGO:
                DrawText("NEON STUDIOS", 280, 280, 40, UI_TEXT_GREEN);
                break;
            case TITLE:
                DrawText("NEON AEGIS", 220, 150, 60, UI_BORDER_CYAN);
                DrawText("PRESS [ENTER] TO START", 250, 300, 20, WHITE);
                DrawText("PRESS [S] FOR SETTINGS", 260, 350, 20, GRAY);
                break;
            case GAMEPLAY:
                core.draw();
                for (auto& p : particles) DrawCircleV(p.position, p.size, Fade(p.color, p.life));
                for (auto& e : enemies) e.draw();
                for (auto& p : projectiles) p.draw();
                
                // HUD
                DrawText(TextFormat("HP: %.0f", core.getHealth()), 20, 20, 20, GREEN);
                DrawText(TextFormat("CREDITS: $%d", money), 20, 50, 20, YELLOW);
                DrawText(TextFormat("SCORE: %d", score), 20, 80, 20, WHITE);
                DrawText("[B] SHOP  [M] PAUSE", SCREEN_WIDTH - 220, 20, 20, LIGHTGRAY);
                break;
            case SHOP:
                drawShop();
                break;
            case PAUSE:
                core.draw(); // Draw background game
                DrawRectangle(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, {0,0,0, 150}); // Dim
                DrawText("PAUSED", 300, 200, 50, UI_TEXT_GREEN);
                DrawText("PRESS [ENTER] TO RESUME", 250, 300, 20, WHITE);
                break;
            case SETTINGS:
                DrawText("SETTINGS", 300, 100, 40, UI_BORDER_CYAN);
                DrawText(TextFormat("SCREEN SHAKE: %s", screenShakeEnabled ? "ON" : "OFF"), 250, 250, 20, WHITE);
                DrawText("PRESS [SPACE] TO TOGGLE", 250, 280, 15, GRAY);
                DrawText("PRESS [ESC] TO RETURN", 250, 400, 20, WHITE);
                break;
            case ENDING:
                DrawText("GAME OVER", 240, 150, 60, RED);
                DrawText(TextFormat("FINAL SCORE: %d", score), 280, 250, 30, WHITE);
                DrawText("PRESS [ENTER] TO RETRY", 260, 350, 20, UI_TEXT_GREEN);
                break;
        }
        
        EndMode2D();
        EndDrawing();
    }

    void drawShop() {
        DrawRectangle(100, 50, 600, 500, {15, 15, 22, 250});
        DrawRectangleLines(100, 50, 600, 500, UI_BORDER_CYAN);
        DrawText("UPGRADE SHOP", 280, 80, 30, UI_TEXT_GREEN);
        DrawText(TextFormat("CREDITS: $%d", money), 280, 120, 20, YELLOW);

        // Buttons (Simple immediate mode GUI)
        drawShopButton(150, 200, "REPAIR CORE ($50)", 50, [this](){
            if(money >= 50 && core.getHealth() < core.getMaxHealth()) { money-=50; core.heal(30); }
        });

        drawShopButton(150, 270, "UPGRADE DAMAGE ($100)", 100, [this](){
            if(money >= 100) { money-=100; damageMultiplier += 5.0f; }
        });

        drawShopButton(150, 340, "INCREASE FIRE RATE ($150)", 150, [this](){
            if(money >= 150 && fireRate > 0.05f) { money-=150; fireRate -= 0.02f; }
        });
        
        DrawText("PRESS [B] TO RESUME", 280, 480, 20, GRAY);
    }

    template<typename Func>
    void drawShopButton(int x, int y, const char* text, int cost, Func action) {
        Rectangle btn = { (float)x, (float)y, 500, 50 };
        bool hover = CheckCollisionPointRec(GetMousePosition(), btn);
        Color col = hover ? BUTTON_HOVER : BLACK;
        
        DrawRectangleRec(btn, col);
        DrawRectangleLinesEx(btn, 2, UI_BORDER_CYAN);
        DrawText(text, x + 20, y + 15, 20, WHITE);
        
        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            action();
        }
    }
};

// --- MAIN ---
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NeonAegis - Circular Defense");
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        game.update(dt);
        game.draw();
    }

    CloseWindow();
    return 0;
}