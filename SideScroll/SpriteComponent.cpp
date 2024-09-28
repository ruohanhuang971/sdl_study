#include "SpriteComponent.h"

void SpriteComponent::SetTexture(SDL_Texture* texture) {
    mTexture = texture;
    // Get width/height of texture
    SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
}