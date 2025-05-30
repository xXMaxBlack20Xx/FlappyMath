// Player.cpp
#include "Player.h"

Player::Player(Vector2 initPos, Texture2D tex)
{
    position = initPos;
    radius = 18;
    velocityY = 0;
    texture = tex;
}

void Player::Update()
{
    velocityY += 0.5f;
    position.y += velocityY;
}

void Player::Jump()
{
    velocityY = -8.0f;
}

void Player::Draw()
{
    DrawTextureV(texture, position, WHITE);
}

Rectangle Player::GetCollisionBox() const
{
    return {position.x, position.y, (float)texture.width, (float)texture.height};
}
