#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <algorithm>
#include <iostream>
#include <ctime>

// Entity structure: represents any object in the game world
struct Entity {
    SDL_FRect rect;  // Position and size (float for smooth movement)
    float speed;     // Movement speed in pixels per second
    SDL_Color color; // RGBA color structure
};

// Update function: handles input, movement, and collision logic
void Update(Entity& player, Entity& enemy, Entity& rock, float deltaTime, int winW, int winH, float& gameTime, float& highScore) {
    // Get pointer to the current keyboard state
    const bool* keystate = SDL_GetKeyboardState(NULL);

    // 1. STORE PREVIOUS POSITION
    // We save coordinates before moving to handle collisions (rollback technique)
    float oldX = player.rect.x;
    float oldY = player.rect.y;

    // 2. PLAYER MOVEMENT
    // Calculate new position based on speed and time passed (deltaTime)
    if (keystate[SDL_SCANCODE_UP])    player.rect.y -= player.speed * deltaTime;
    if (keystate[SDL_SCANCODE_DOWN])  player.rect.y += player.speed * deltaTime;
    if (keystate[SDL_SCANCODE_LEFT])  player.rect.x -= player.speed * deltaTime;
    if (keystate[SDL_SCANCODE_RIGHT]) player.rect.x += player.speed * deltaTime;

    // 3. COLLISION WITH OBSTACLE (ROCK)
    // If player overlaps with rock, reset position to the one before movement
    if (SDL_HasRectIntersectionFloat(&player.rect, &rock.rect)) {
        player.rect.x = oldX;
        player.rect.y = oldY;
    }

    // 4. SCREEN BOUNDARY CONSTRAINTS
    // SDL_clamp ensures the value stays between 0 and window edge
    player.rect.x = SDL_clamp(player.rect.x, 0, (float)winW - player.rect.w);
    player.rect.y = SDL_clamp(player.rect.y, 0, (float)winH - player.rect.h);

    // 5. ENEMY AI MOVEMENT
    // Simple "follow player" logic
    float oldEnX = enemy.rect.x;
    float oldEnY = enemy.rect.y;

    if (enemy.rect.x < player.rect.x) enemy.rect.x += enemy.speed * deltaTime;
    if (enemy.rect.x > player.rect.x) enemy.rect.x -= enemy.speed * deltaTime;
    if (enemy.rect.y < player.rect.y) enemy.rect.y += enemy.speed * deltaTime;
    if (enemy.rect.y > player.rect.y) enemy.rect.y -= enemy.speed * deltaTime;

    // Enemy also stops if it hits the rock
    if (SDL_HasRectIntersectionFloat(&enemy.rect, &rock.rect)) {
        enemy.rect.x = oldEnX;
        enemy.rect.y = oldEnY;
    }

    // 6. PLAYER-ENEMY COLLISION (GAME OVER LOGIC)
    if (SDL_HasRectIntersectionFloat(&player.rect, &enemy.rect)) {
        gameTime = 0.0f; // Reset survival timer
        player.rect.x = 0; player.rect.y = 0; // Teleport player to the start
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) == false) return -1;

    // Create window and renderer
    SDL_Window* window = SDL_CreateWindow("SDL3 Rock Game", 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    
    // Seed random for future use
    srand((unsigned int)time(NULL));

    // Initialize game entities
    Entity player = {{ 400, 300, 50, 50 }, 300.0f, { 255, 50, 50, 255 }};
    Entity enemy  = {{ 100, 100, 50, 50 }, 150.0f, { 50, 50, 255, 255 }};
    
    // The obstacle (Rock)
    Entity rock;
    rock.rect = { 200.0f, 160.0f, 150.0f, 150.0f };
    rock.speed = 0;
    rock.color = { 100, 100, 100, 255 }; // Gray

    float gameTime = 0.0f, highScore = 0.0f;
    bool running = true;
    SDL_Event event;
    Uint64 last_time = SDL_GetTicks();

    // MAIN LOOP
    while (running) {
        // Calculate Delta Time (time between current and previous frame)
        Uint64 current_time = SDL_GetTicks();
        float deltaTime = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        if (deltaTime <= 0) deltaTime = 0.001f;

        // EVENT HANDLING
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        // Window size might change, get actual values
        int winW, winH;
        SDL_GetWindowSizeInPixels(window, &winW, &winH);

        // Update score
        gameTime += deltaTime;
        if (gameTime > highScore) highScore = gameTime;

        // LOGIC UPDATE
        Update(player, enemy, rock, deltaTime, winW, winH, gameTime, highScore);

        // RENDERING
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Background: Black
        SDL_RenderClear(renderer);

        // Draw Rock
        SDL_SetRenderDrawColor(renderer, rock.color.r, rock.color.g, rock.color.b, 255);
        SDL_RenderFillRect(renderer, &rock.rect);

        // Draw Player
        SDL_SetRenderDrawColor(renderer, player.color.r, player.color.g, player.color.b, 255);
        SDL_RenderFillRect(renderer, &player.rect);

        // Draw Enemy
        SDL_SetRenderDrawColor(renderer, enemy.color.r, enemy.color.g, enemy.color.b, 255);
        SDL_RenderFillRect(renderer, &enemy.rect);

        // Draw UI
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        char buffer[64];
        SDL_snprintf(buffer, sizeof(buffer), "TIME: %.2f s | BEST: %.2f s", gameTime, highScore);
        SDL_RenderDebugText(renderer, 10, 10, buffer);

        // Show the frame on screen
        SDL_RenderPresent(renderer);
        SDL_Delay(1); // Prevent 100% CPU usage
    }

    // Cleanup resources
    SDL_Quit();
    return 0;
}
