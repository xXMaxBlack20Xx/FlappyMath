// src/UIManager.cpp
#include "UIManager.h"
#include "raylib.h"
#include <cstdio>

void UIManager::DrawStartScreen(
    const Texture2D &fondo,
    const Texture2D &titulo,
    const Texture2D &jugar,
    int highScore)
{
    // Igual que en tu main original:
    ClearBackground(DARKGREEN);
    DrawTextureV(fondo, (Vector2){0, 0}, WHITE);
    DrawTextureV(titulo, (Vector2){325, 200}, WHITE);
    DrawTextureV(jugar, (Vector2){445, 530}, WHITE);

    // Texto de puntuación más alta
    const char *fmt = TextFormat("LA PUNTUACION MAS ALTA ES: %d", highScore);
    int tw = MeasureText(fmt, 30);
    DrawText(
        fmt,
        GetScreenWidth() / 2 - tw / 2,
        GetScreenHeight() / 2 + 210,
        30, BLACK);
}

void UIManager::DrawGameOverScreen(
    const Texture2D &fondo,
    const Texture2D &gover,
    int score)
{
    // Igual que en tu main original:
    ClearBackground(RAYWHITE);
    DrawTextureV(fondo, (Vector2){0, 0}, WHITE);
    DrawTextureV(gover, (Vector2){200, 200}, WHITE);

    // Texto de puntuación final
    char buf[64];
    snprintf(buf, sizeof(buf), "PUNTUACION: %d", score);
    int twScore = MeasureText(buf, 30);
    DrawText(
        buf,
        GetScreenWidth() / 2 - twScore / 2 + 50,
        GetScreenHeight() / 2 + 150,
        30, BLACK);

    // Texto de reinicio
    const char *reiniciar = "Presiona [R] para reiniciar";
    int twR = MeasureText(reiniciar, 40);
    DrawText(
        reiniciar,
        GetScreenWidth() / 2 - twR / 2 + 10,
        GetScreenHeight() / 2 + 215,
        40, BLACK);
}
