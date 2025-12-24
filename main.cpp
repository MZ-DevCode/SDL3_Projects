#include <SDL3/SDL.h>          // Main SDL3 functions and structures
#include <SDL3/SDL_main.h>     // Required for SDL3 entry point abstraction
#include <algorithm>           // Common algorithms
#include <iostream>            // Input/Output stream
#include <ctime>               // Used for seeding the random number generator

// Entity structure to represent game objects (player and enemy)
struct Entity {
    SDL_FRect rect;      // Floating point rectangle for position and size
    float speed;         // Movement speed (pixels per second)
    SDL_Color color;     // RGBA color of the entity
};

int main(int argc, char* argv[]) {
    // Initialize SDL video subsystem
    SDL_Init(SDL_INIT_VIDEO);

    // Create a resizable window
    SDL_Window* window = SDL_CreateWindow("SDL3 Delta Time", 800, 600, SDL_WINDOW_RESIZABLE);

    // Create a 2D rendering context for the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    // Seed the random generator using current time
    srand(time(NULL));

    // Initialize player starting properties
    Entity player;
    player.rect = { 400.0f, 300.0f, 50.0f, 50.0f }; // X, Y, Width, Height
    player.speed = 300.0f;                          // Pixels per second
    player.color = { 255, 50, 50, 255 };            // Red color

    // Initialize enemy starting properties
    Entity enemy;
    enemy.rect = { 100.0f, 100.0f, 50.0f, 50.0f };
    enemy.speed = 150.0f;
    enemy.color = { 50, 50, 255, 255 };             // Blue color

    float gameTime = 0.0f;    // Current survival time
    float highScore = 0.0f;   // Best survival time recorded

    bool running = true;      // Main loop flag
    SDL_Event event;          // Structure to store event data

    // Get initial ticks for delta time calculation
    Uint64 last_time = SDL_GetTicks();

    // Main Game Loop
    while (running) {
        // Get current window dimensions
        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        // Delta Time calculation: time passed since last frame in seconds
        Uint64 current_time = SDL_GetTicks();
        float deltaTime = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        // Prevent division by zero or negative delta (safety check)
        if (deltaTime <= 0) deltaTime = 0.001f;

        // Update score and high score
        gameTime += deltaTime;
        if (gameTime > highScore) {
            highScore = gameTime;
        };

        // Event Handling: Process window events (like closing the window)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        // Get window size in pixels for accurate collision and boundaries
        int winW, winH;
        SDL_GetWindowSizeInPixels(window, &winW, &winH);

        // Input Handling: Check keyboard state for player movement
        const bool* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_UP])    player.rect.y -= player.speed * deltaTime;
        if (keystate[SDL_SCANCODE_DOWN])  player.rect.y += player.speed * deltaTime;
        if (keystate[SDL_SCANCODE_LEFT])  player.rect.x -= player.speed * deltaTime;
        if (keystate[SDL_SCANCODE_RIGHT]) player.rect.x += player.speed * deltaTime;

        // Boundary Checks: Keep player inside the window
        if (player.rect.x < 0) player.rect.x = 0;
        if (player.rect.y < 0) player.rect.y = 0;
        if (player.rect.x + player.rect.w > winW) player.rect.x = (float)winW - player.rect.w;
        if (player.rect.y + player.rect.h > winH) player.rect.y = (float)winH - player.rect.h;

        // Enemy AI: Move enemy towards the player's position
        if (enemy.rect.x < player.rect.x) enemy.rect.x += enemy.speed * deltaTime;
        if (enemy.rect.x > player.rect.x) enemy.rect.x -= enemy.speed * deltaTime;
        if (enemy.rect.y < player.rect.y) enemy.rect.y += enemy.speed * deltaTime;
        if (enemy.rect.y > player.rect.y) enemy.rect.y -= enemy.speed * deltaTime;

        // Collision Check: Reset game if enemy touches the player
        if (SDL_HasRectIntersectionFloat(&player.rect, &enemy.rect)) {
            gameTime = 0.0f; // Reset current score

            // Randomize player respawn position in one of the 4 corners
            int corner = rand() % 4;
            switch (corner) {
            case 0: player.rect.x = 0; player.rect.y = 0; break;
            case 1: player.rect.x = (float)windowWidth - player.rect.w; player.rect.y = 0; break;
            case 2: player.rect.x = 0; player.rect.y = (float)windowHeight - player.rect.h; break;
            case 3: player.rect.x = (float)windowWidth - player.rect.w; player.rect.y = (float)windowHeight - player.rect.h; break;
            default: player.rect.x = 450; player.rect.y = 450; break;
            }
            SDL_Log("Collision detected! Respawning player.");
        }

        // Rendering Section 

        // Clear screen with black color
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw Player
        SDL_SetRenderDrawColor(renderer, player.color.r, player.color.g, player.color.b, 255);
        SDL_RenderFillRect(renderer, &player.rect);

        // Draw Enemy
        SDL_SetRenderDrawColor(renderer, enemy.color.r, enemy.color.g, enemy.color.b, 255);
        SDL_RenderFillRect(renderer, &enemy.rect);

        // Draw UI (Time and Best Score) using SDL3's debug text feature
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White text
        char buffer[64];

        // Format and render current time string
        SDL_snprintf(buffer, sizeof(buffer), "TIME: %.2f s", gameTime);
        SDL_RenderDebugText(renderer, 10, 10, buffer);

        // Format and render best score string
        SDL_snprintf(buffer, sizeof(buffer), "BEST: %.2f s", highScore);
        SDL_RenderDebugText(renderer, 10, 30, buffer);

        // Present the rendered frame to the window
        SDL_RenderPresent(renderer);

        // Small delay to prevent 100% CPU usage
        SDL_Delay(1);
    }

    // Cleanup: Destroy resources to prevent memory leaks
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); // Shutdown SDL systems

    return 0;
}
