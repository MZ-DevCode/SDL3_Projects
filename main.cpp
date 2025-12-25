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

// Function to update game logic, movement and collisions
void Update(Entity& player, Entity& enemy, float deltaTime, int winW, int winH, float& gameTime, float& highScore) {
    // Get the current state of the keyboard
    const bool* keystate = SDL_GetKeyboardState(NULL);

    // Player movement based on arrow keys and delta time
    if (keystate[SDL_SCANCODE_UP])    player.rect.y -= player.speed * deltaTime;
    if (keystate[SDL_SCANCODE_DOWN])  player.rect.y += player.speed * deltaTime;
    if (keystate[SDL_SCANCODE_LEFT])  player.rect.x -= player.speed * deltaTime;
    if (keystate[SDL_SCANCODE_RIGHT]) player.rect.x += player.speed * deltaTime;

    // Boundary Checks: Keep player inside the window dimensions
    if (player.rect.x < 0) player.rect.x = 0;
    if (player.rect.y < 0) player.rect.y = 0;
    if (player.rect.x + player.rect.w > winW) player.rect.x = (float)winW - player.rect.w;
    if (player.rect.y + player.rect.h > winH) player.rect.y = (float)winH - player.rect.h;

    // Enemy AI: Move towards player's X and Y coordinates
    if (enemy.rect.x < player.rect.x) enemy.rect.x += enemy.speed * deltaTime;
    if (enemy.rect.x > player.rect.x) enemy.rect.x -= enemy.speed * deltaTime;
    if (enemy.rect.y < player.rect.y) enemy.rect.y += enemy.speed * deltaTime;
    if (enemy.rect.y > player.rect.y) enemy.rect.y -= enemy.speed * deltaTime;

    // Collision detection between player and enemy
    if (SDL_HasRectIntersectionFloat(&player.rect, &enemy.rect) == true) {
        gameTime = 0.0f; // Reset current survival time
        int corner = rand() % 4; // Pick a random corner to respawn player
        switch (corner) {
        case 0: player.rect.x = 0; player.rect.y = 0; break;
        case 1: player.rect.x = (float)winW - player.rect.w; player.rect.y = 0; break;
        case 2: player.rect.x = 0; player.rect.y = (float)winH - player.rect.h; break;
        case 3: player.rect.x = (float)winW - player.rect.w; player.rect.y = (float)winH - player.rect.h; break;
        default: player.rect.x = (float)winW / 2; player.rect.y = (float)winH / 2; break;
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL video subsystem
    SDL_Init(SDL_INIT_VIDEO);

    // Create a window that can be resized
    SDL_Window* window = SDL_CreateWindow("SDL3 Refactored", 800, 600, SDL_WINDOW_RESIZABLE);

    // Create a renderer to draw on the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    // Seed the random number generator
    srand((unsigned int)time(NULL));

    // Initialize player starting state
    Entity player;
    player.rect = { 400.0f, 300.0f, 50.0f, 50.0f };
    player.speed = 300.0f;
    player.color = { 255, 50, 50, 255 };

    // Initialize enemy starting state
    Entity enemy;
    enemy.rect = { 100.0f, 100.0f, 50.0f, 50.0f };
    enemy.speed = 150.0f;
    enemy.color = { 50, 50, 255, 255 };

    float gameTime = 0.0f, highScore = 0.0f; // Score tracking
    bool running = true;                    // Loop control variable
    SDL_Event event;                        // Event container
    Uint64 last_time = SDL_GetTicks();      // Store time of the first frame

    // Main Game Loop
    while (running) {
        // Calculate Delta Time in seconds
        Uint64 current_time = SDL_GetTicks();
        float deltaTime = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        // Ensure deltaTime is never zero to avoid logic issues
        if (deltaTime <= 0) deltaTime = 0.001f;

        // Process all pending events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        // Get window size for responsive logic
        int winW, winH;
        SDL_GetWindowSizeInPixels(window, &winW, &winH);

        // Update score and high score
        gameTime += deltaTime;
        if (gameTime > highScore) highScore = gameTime;

        // Run movement and collision logic
        Update(player, enemy, deltaTime, winW, winH, gameTime, highScore);

        // Clear screen with black color
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw Player
        SDL_SetRenderDrawColor(renderer, player.color.r, player.color.g, player.color.b, 255);
        SDL_RenderFillRect(renderer, &player.rect);

        // Draw Enemy
        SDL_SetRenderDrawColor(renderer, enemy.color.r, enemy.color.g, enemy.color.b, 255);
        SDL_RenderFillRect(renderer, &enemy.rect);

        // Draw UI Text (Time and Best Score)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        char buffer[64];
        SDL_snprintf(buffer, sizeof(buffer), "TIME: %.2f s | BEST: %.2f s", gameTime, highScore);
        SDL_RenderDebugText(renderer, 10, 10, buffer);

        // Swap buffers to display the new frame
        SDL_RenderPresent(renderer);

        // Small delay to prevent high CPU usage
        SDL_Delay(1);
    }

    // Cleanup resources and quit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
