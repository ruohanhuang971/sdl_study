#pragma once
#include "Actor.h"
#include ""
#include <SDL2/SDL.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unordered_map>

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
    SDL_Window* mWindow;
    // draws graphics
    SDL_Renderer* mRenderer;
    // Game should continue to run
    bool mIsRunning;
    Uint32 mTicksCount;

    bool mUpdatingActors; // if currently updating all mActors
    // add/remove actor to mPendingActors or mActors
    void AddActor(Actor* actor);
    void RemoveActor(Actor* actor);

    SDL_Texture* GetTexture(const char* fileName);
    void AddSprite(SpriteComponent* sprite);

    // if looping over mActors a new actor is created, it is added to mPendingActor
    // cannot add it to mActors because it is being iterated over
    std::vector<Actor*> mActors;        // active actors
    std::vector<Actor*> mPendingActors; // pending actors

    // All the sprite components drawn
	std::vector<class SpriteComponent*> mSprites;

    // map of filenames to SDL_Texture pointers
    std::unordered_map<std::string, SDL_Texture*> mTextures;
};