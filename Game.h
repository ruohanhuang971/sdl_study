#include <SDL2/SDL.h>

struct Vector2 {
    float x;
    float y;
};

class Game {
public:
    Game();
    // Initialize the game
    bool Initialize();
    // Runs the game loop until the game is over
    void RunLoop();
    // Shutdown the game
    void Shutdown();

private:
    // Helper functions for the game loop
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
    // Window created by SDL
    SDL_Window *mWindow;
    // draws graphics
    SDL_Renderer *mRenderer;
    // Game should continue to run
    bool mIsRunning;
    Vector2 mPaddlePos1;
    int mPaddleDir1;
    Vector2 mPaddlePos2;
    int mPaddleDir2;
    Vector2 mBallPos;
    Vector2 mBallVel;
    Uint32 mTicksCount;
};