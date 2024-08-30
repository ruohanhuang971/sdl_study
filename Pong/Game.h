#include <SDL2/SDL.h>
#include <cmath>
#include <vector>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>

struct Vector2 {
    float x;
    float y;
};

struct Ball {
    Vector2 Pos;
    Vector2 Vec;
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
    Uint32 mTicksCount;
    
    //paddles
    Vector2 mPaddlePos1;
    int mPaddleDir1;
    Vector2 mPaddlePos2;
    int mPaddleDir2;

    //balls
    const int numBalls = 2;
    std::vector<Ball> balls;
};