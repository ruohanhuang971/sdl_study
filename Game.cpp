#include "Game.h"
#include <cmath>

const int thickness = 15;
const float paddleH = 100.0f;
const float windowWidth = 1024;
const float windowHeight = 700;

Game::Game() {
    mWindow = nullptr;
    mIsRunning = true;
    mTicksCount = 0;
    mPaddleDir = 0;
    mPaddlePos.x = 10.0f;
    mPaddlePos.y = windowHeight / 2.0f;
    mBallPos.x = windowWidth / 2.0f;
    mBallPos.y = windowHeight / 2.0f;
    mBallVel.x = -80.0f;
    mBallVel.y = 83.0f;
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
        25,                                    // Top left y-coordinate of window
        windowWidth,                           // Width of window
        windowHeight,                          // Height of window
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

    // updates mPaddleDir based on input
    // add/subtract from mPaddleDir ensures that if both keys pressed mPaddleDir is zero
    mPaddleDir = 0;
    if (state[SDL_SCANCODE_W]) {
        mPaddleDir -= 1;
    }
    if (state[SDL_SCANCODE_S]) {
        mPaddleDir += 1;
    }
}

void Game::UpdateGame() {
    // Wait until 16ms has elapsed since last frame
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
        ;
    // Delta time is the difference in ticks from last frame
    // (converted to seconds)
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    // Clamp maximum delta time value (never jump ahead too far)
    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }

    // Update paddle
    if (mPaddleDir != 0) {
        mPaddlePos.y += mPaddleDir * 200.0f * deltaTime; // 300 pixels/second
        // keep within screen bounds
        if (mPaddlePos.y < (paddleH / 2.0f + thickness)) {
            mPaddlePos.y = paddleH / 2.0f + thickness;
        } else if (mPaddlePos.y > (windowHeight - paddleH / 2.0f - thickness)) {
            mPaddlePos.y = windowHeight - paddleH / 2.0f - thickness;
        }
    }

    // Update ball
    mBallPos.x += mBallVel.x * deltaTime;
    mBallPos.y += mBallVel.y * deltaTime;

    // check for collision and update velocity
    // (prevent the ball from not moving far enough and constantly negating velocity and getting stuck)
    if (mBallPos.y <= thickness && mBallVel.y < 0.0f) { // if collide with top wall && moving up
        mBallVel.y *= -1;
    } else if (mBallPos.y >= windowHeight - thickness && mBallVel.y > 0.0f) { // bottom wall
        mBallVel.y *= -1;
    }
    if (mBallPos.x >= windowWidth - thickness && mBallVel.x > 0.0f) { // right wall
        mBallVel.x *= -1;
    }
    // collision with paddle
    float diff = std::abs(mBallPos.y - mPaddlePos.y);
    if (
        // if absolute y distance from paddle is less than half the height of paddle
        diff <= paddleH / 2.0f &&
        // and ball is at the correct x-position
        mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
        // and ball is moving to the left
        mBallVel.x < 0.0f) {
        mBallVel.x *= -1.0f;
    }

    // end game if offscreen
    if (mBallPos.x < 0 || mBallPos.x > windowWidth) {
        mIsRunning = false;
    }
}

void Game::GenerateOutput() {
    // clear back buffer to a color
    SDL_SetRenderDrawColor( // specify a color (blue)
        mRenderer,          // pointer to renderer
        0,                  // R
        0,                  // G
        255,                // B
        255                 // A
    );
    SDL_RenderClear(mRenderer); // clear the back buffer to the current draw color

    // draw the entire game scene
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255); // set draw color to white
    // specify bounds of the rectangle for top wall
    SDL_Rect wallTop{
        0,                             // Top left x
        0,                             // Top left y
        static_cast<int>(windowWidth), // Width
        thickness                      // Height
    };
    SDL_RenderFillRect(mRenderer, &wallTop); // draw top wall
    // specify bounds of the rectangle for bottom wall
    SDL_Rect wallBot{
        0,                                          // Top left x
        static_cast<int>(windowHeight) - thickness, // Top left y
        static_cast<int>(windowWidth),              // Width
        thickness                                   // Height
    };
    SDL_RenderFillRect(mRenderer, &wallBot); // draw bottom wall
    // specify bounds of the rectangle for right wall
    SDL_Rect wallRight{
        static_cast<int>(windowWidth) - thickness, // Top left x
        0,                                         // Top left y
        thickness,                                 // Width
        static_cast<int>(windowWidth)              // Height
    };
    SDL_RenderFillRect(mRenderer, &wallRight); // draw bottom wall
    // draw paddle
    SDL_Rect paddle{
        static_cast<int>(mPaddlePos.x - thickness / 2),
        static_cast<int>(mPaddlePos.y - paddleH / 2),
        thickness,
        static_cast<int>(paddleH)};
    SDL_RenderFillRect(mRenderer, &paddle); // draw ball
    // draw ball
    SDL_Rect ball{
        static_cast<int>(mBallPos.x - thickness / 2),
        static_cast<int>(mBallPos.y - thickness / 2),
        thickness,
        thickness};
    SDL_RenderFillRect(mRenderer, &ball); // draw ball

    // swap the front and back buffers
    SDL_RenderPresent(mRenderer);
}
