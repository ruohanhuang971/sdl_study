#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;
const float windowWidth = 1024;
const float windowHeight = 700;

Game::Game() {
    mWindow = nullptr;
    mIsRunning = true;
    mTicksCount = 0;

    // paddle 1
    mPaddleDir1 = 0;
    mPaddlePos1.x = 10.0f;
    mPaddlePos1.y = windowHeight / 2.0f;

    // paddle2
    mPaddleDir2 = 0;
    mPaddlePos2.x = windowWidth - 10.0f;
    mPaddlePos2.y = windowHeight / 2.0f;

    // ball
    srand(time(NULL));
    float vecX, vecY;
    for (int i = 0; i < numBalls; i++) {
        // each ball start with the center with different velocity
        vecX = rand() % 40 + 70;
        vecY = rand() % 40 + 70;
        if (rand() % 2 == 0) {
            vecX *= -1;
        }
        if (rand() % 2 == 0) {
            vecY *= -1;
        }
        balls.push_back({windowWidth / 2.0f, windowHeight / 2.0f, vecX, vecY});
    }
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
    mPaddleDir1 = 0;
    if (state[SDL_SCANCODE_W]) {
        mPaddleDir1 -= 1;
    }
    if (state[SDL_SCANCODE_S]) {
        mPaddleDir1 += 1;
    }

    mPaddleDir2 = 0;
    if (state[SDL_SCANCODE_UP]) {
        mPaddleDir2 -= 1;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        mPaddleDir2 += 1;
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

    // Update paddle1
    if (mPaddleDir1 != 0) {
        mPaddlePos1.y += mPaddleDir1 * 200.0f * deltaTime; // 200 pixels/second
        // keep within screen bounds
        if (mPaddlePos1.y < (paddleH / 2.0f + thickness)) {
            mPaddlePos1.y = paddleH / 2.0f + thickness;
        } else if (mPaddlePos1.y > (windowHeight - paddleH / 2.0f - thickness)) {
            mPaddlePos1.y = windowHeight - paddleH / 2.0f - thickness;
        }
    }

    // Update paddle2
    if (mPaddleDir2 != 0) {
        mPaddlePos2.y += mPaddleDir2 * 200.0f * deltaTime; // 200 pixels/second
        // keep within screen bounds
        if (mPaddlePos2.y < (paddleH / 2.0f + thickness)) {
            mPaddlePos2.y = paddleH / 2.0f + thickness;
        } else if (mPaddlePos2.y > (windowHeight - paddleH / 2.0f - thickness)) {
            mPaddlePos2.y = windowHeight - paddleH / 2.0f - thickness;
        }
    }

    // Update ball
    for (int i = 0; i < numBalls; i++) {
        balls[i].Pos.x += balls[i].Vec.x * deltaTime;
        balls[i].Pos.y += balls[i].Vec.y * deltaTime;
    }

    // check for collision and update velocity
    // (prevent the ball from not moving far enough and constantly negating velocity and getting stuck)
    for (int i = 0; i < numBalls; i++) {
        if (balls[i].Pos.y <= thickness && balls[i].Vec.y < 0.0f) { // if collide with top wall && moving up
            balls[i].Vec.y *= -1;
        } else if (balls[i].Pos.y >= windowHeight - thickness && balls[i].Vec.y > 0.0f) { // bottom wall
            balls[i].Vec.y *= -1;
        }
    }
    // DEBUGGING: force ball to stay on screen
    // for (int i = 0; i < numBalls; i++) {
    //     if (balls[i].Pos.x <= thickness && balls[i].Vec.x < 0.0f) { // if collide with top wall && moving up
    //         balls[i].Vec.x *= -1;
    //     } else if (balls[i].Pos.x >= windowWidth - thickness && balls[i].Vec.x > 0.0f) { // bottom wall
    //         balls[i].Vec.x *= -1;
    //     }
    // }

    // collision with paddle1
    for (int i = 0; i < numBalls; i++) {
        float diff1 = std::abs(balls[i].Pos.y - mPaddlePos1.y);
        if (
            // if absolute y distance from paddle is less than half the height of paddle
            diff1 <= paddleH / 2.0f &&
            // and ball is at the correct x-position
            balls[i].Pos.x <= 25.0f && balls[i].Pos.x >= 20.0f &&
            // and ball is moving to the left
            balls[i].Vec.x < 0.0f) {
            balls[i].Vec.x *= -1.0f;
        }
    }
    // collision with paddle2
    for (int i = 0; i < numBalls; i++) {
        float diff2 = std::abs(balls[i].Pos.y - mPaddlePos2.y);
        if (
            // if absolute y distance from paddle is less than half the height of paddle
            diff2 <= paddleH / 2.0f &&
            // and ball is at the correct x-position
            balls[i].Pos.x >= (windowWidth - 25.0f) && balls[i].Pos.x <= (windowWidth - 20.0f) &&
            // and ball is moving to the right
            balls[i].Vec.x > 0.0f) {
            balls[i].Vec.x *= -1.0f;
        }
    }

    // // end game if offscreen
    for (int i = 0; i < numBalls; i++) {
        if ((balls[i].Pos.x - thickness / 2) < 0 || (balls[i].Pos.x + thickness / 2) > windowWidth) {
            mIsRunning = false;
        }
    }
}

void Game::GenerateOutput() {
    // clear back buffer to a color
    SDL_SetRenderDrawColor( // specify a color (blue)
        mRenderer,          // pointer to renderer
        20,                 // R
        100,                // G
        20,                 // B
        255                 // A
    );
    SDL_RenderClear(mRenderer); // clear the back buffer to the current draw color

    // draw the entire game scene
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255); // set draw color to white
    // specify bounds of the rectangle for top wall
    SDL_Rect mid{
        static_cast<int>(windowWidth) / 2 - 2, // Top left x
        0,                                     // Top left y
        4,                                     // Width
        static_cast<int>(windowWidth)          // Height
    };
    SDL_RenderFillRect(mRenderer, &mid); // draw top wall
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
    // draw paddle1
    SDL_Rect paddle1{
        static_cast<int>(mPaddlePos1.x - thickness / 2),
        static_cast<int>(mPaddlePos1.y - paddleH / 2),
        thickness,
        static_cast<int>(paddleH)};
    SDL_RenderFillRect(mRenderer, &paddle1); // draw paddle1
    // draw paddle2
    SDL_Rect paddle2{
        static_cast<int>(mPaddlePos2.x - thickness / 2),
        static_cast<int>(mPaddlePos2.y - paddleH / 2),
        thickness,
        static_cast<int>(paddleH)};
    SDL_RenderFillRect(mRenderer, &paddle2); // draw paddle1
    // draw ball
    for (int i = 0; i < numBalls; i++) {
        SDL_Rect ball{
            static_cast<int>(balls[i].Pos.x - thickness / 2),
            static_cast<int>(balls[i].Pos.y - thickness / 2),
            thickness,
            thickness};
        SDL_RenderFillRect(mRenderer, &ball); // draw ball
    }

    // swap the front and back buffers
    SDL_RenderPresent(mRenderer);
}
