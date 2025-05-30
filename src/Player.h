// Player.h
#pragma once
#include "raylib.h"

class Player
{
private:
    Vector2 position;
    float radius;
    float velocityY;
    Texture2D texture;

public:
    Player(Vector2 initPos, Texture2D tex);
    void Update();
    void Draw();
    void Jump();
    Rectangle GetCollisionBox() const;
};
