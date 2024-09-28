#include "Game.h"
#include "SDL/SDL_image.h"
#include <algorithm>

const float windowWidth = 1024;
const float windowHeight = 700;

Game::Game() {
    mWindow = nullptr;
    mIsRunning = true;
    mTicksCount = 0;
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
        "Game Programming in C++ (Chapter 2)", // Window title
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

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    LoadData();

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
    const Uint8* state = SDL_GetKeyboardState(NULL);
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
    // Compute delta time
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    // Clamp maximum delta time value (never jump ahead too far)
    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }

    // Update all actors
    mUpdatingActors = true;
    for (auto actor : mActors) {
        actor->Update(deltaTime);
    }

    mUpdatingActors = false;
    // Move any pending actors to mActors
    for (auto pending : mPendingActors) {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    // Add any dead actors to a temp vector
    std::vector<Actor*> deadActors;
    for (auto actor : mActors) {
        if (actor->GetState() == Actor::EDead) {
            deadActors.emplace_back(actor);
        }
    }
    // Delete dead actors (which removes them from mActors)
    for (auto actor : deadActors) {
        delete actor;
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

    for (auto& i : mSprites) {
        i.Draw(mRenderer);
    }
   
    // swap the front and back buffers
    SDL_RenderPresent(mRenderer);
}

void Game::AddActor(Actor* actor) {
    // If updating actors, need to add to pending
    if (mUpdatingActors) {
        mPendingActors.emplace_back(actor);
    } else {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor) {
    // search in updating actors
    auto pos = std::find(mActors.begin(), mActors.end(), actor);
    if (pos != mActors.end()) {
        mActors.erase(pos);
    } else {
        pos = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
        mPendingActors.erase(pos);
    }
}

SDL_Texture* Game::GetTexture(const char* fileName) {
    auto pos = mTextures.find(fileName);
    if (pos != mTextures.end()) { // if found
        return pos->second;
    }

    // if not found load from file
    SDL_Surface* surf = IMG_Load(fileName);
    if (!surf) {
        SDL_Log("Failed to load texture file %s", fileName);
        return nullptr;
    }
    // Create texture from surface
    SDL_Texture* text = SDL_CreateTextureFromSurface(mRenderer, surf);
    SDL_FreeSurface(surf);
    if (!text) {
        SDL_Log("Failed to convert surface to texture for %s", fileName);
        return nullptr;
    }

    mTextures.emplace(fileName, text);

    return text;
}

void Game::AddSprite(SpriteComponent* sprite) {
    // Find the insertion point in the sorted vector
    // (The first element with a higher draw order than me)
    int myDrawOrder = sprite->GetDrawOrder();
    auto iter = mSprites.begin();
    for (; iter != mSprites.end(); ++iter) {
        if (myDrawOrder < (*iter)->GetDrawOrder()) {
            break;
        }
    }
    // Inserts element before position of iterator
    mSprites.insert(iter, sprite);
}
