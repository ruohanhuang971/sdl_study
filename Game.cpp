#include "Game.h"

Game::Game() {
    mWindow = nullptr;
    mIsRunning = true;
}

bool Game::Initialize() {
    // that SDL_Init returns int
    // if this int != 0, initialization failed
    int sdlResult = SDL_Init(SDL_INIT_VIDEO); // SDL_INIT_AUDIO is the subsystem for Audio devices
    if (sdlResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // create window
    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (Chapter 1)", // Window title
        100,                                   // Top left x-coordinate of window
        100,                                   // Top left y-coordinate of window
        1024,                                  // Width of window
        768,                                   // Height of window
        0                                      // Flags (0 for no flags set)
    );
    // if create window failed, mWindow would be nullptr
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    // both init and window create successful
    return true;
}

void Game::RunLoop() {
    // run iterations of gameloop until mIsRunning == false
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::Shutdown() {
    SDL_DestroyWindow(mWindow); // destory window
    SDL_Quit();                 // closes SDL
}

void Game::ProcessInput() {
    SDL_Event event;
    // While there are still events in the queue
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        // Handle different event types here
        case SDL_QUIT:
            mIsRunning = false;
            break;
        }
    }

    // Get state of keyboard
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    // If escape is pressed, also stop program
    if (state[SDL_SCANCODE_ESCAPE]) {
        mIsRunning = false;
    }
}

void Game::UpdateGame() {
}

void Game::GenerateOutput() {
}
