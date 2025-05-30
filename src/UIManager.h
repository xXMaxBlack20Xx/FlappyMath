// src/UIManager.h
#pragma once
#include "raylib.h"

class UIManager
{
public:
    // Pantalla de inicio, con fondo, título, botón y creditos
    static void DrawStartScreen(
        const Texture2D &fondo,
        const Texture2D &titulo,
        const Texture2D &jugar,
        int highScore);

    // Pantalla de game over, con fondo, imagen de fin y texto
    static void DrawGameOverScreen(
        const Texture2D &fondo,
        const Texture2D &gover,
        int score);
};
