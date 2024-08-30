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

    // create render after window created
    mRenderer = SDL_CreateRenderer(
        mWindow,                                               // Window to create renderer for
        -1,                                                    // specifies which graphics driver to use; -1 only a single window [let SDL decide]
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // initialization flags

    if (!mRenderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    // both init/window/render create successful
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
    SDL_DestroyRenderer(mRenderer); // destory renderer
    SDL_DestroyWindow(mWindow);     // destory window
    SDL_Quit();                     // closes SDL
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
    // clear back buffer to a color
    SDL_SetRenderDrawColor( // specify a color
        mRenderer,          // pointer to renderer
        0,                  // R
        0,                  // G
        255,                // B
        255                 // A
    );
    SDL_RenderClear(mRenderer); // clear the back buffer to the current draw color

    // draw the entire game scene here

    // swap the front and back buffers
    SDL_RenderPresent(mRenderer);
}
