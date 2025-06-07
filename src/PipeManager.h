// src/PipeManager.h
#pragma once

#include <vector>
#include "raylib.h"

// Singleton que gestiona la creación, actualización y dibujo de las tuberías
// así como la detección de colisiones y la posición del gap actual.
class PipeManager
{
public:
    // Obtiene la instancia única
    static PipeManager &GetInstance();

    // Inicializa una nueva tubería (o reinicia) con:
    //  screenWidth, screenHeight: dimensiones de la ventana
    //  pipeTextureHeight: altura de la textura de la tubería
    void InitPipes(int screenWidth, int screenHeight, int pipeTextureHeight);

    // Mueve todas las tuberías y zonas según pipeSpeed
    void Update(float pipeSpeed);

    // Dibuja las tuberías (si usas DrawRectangleLines, o texturas en Game::Draw)
    void Draw() const;

    // Reinicia a estado inicial (llama a InitPipes con los mismos parámetros)
    void Reset();

    /// Comprueba si playerBox colisiona con cualquier tubería o zona kill
    bool CheckCollision(const Rectangle &playerBox) const;

    /// Nueva función: Comprueba si el jugador tocó una respuesta en el gap
    /// Devuelve true si tocó alguna respuesta, hitCorrect indica si fue la correcta
    bool CheckAnswerCollision(const Rectangle &playerBox, const Rectangle &correctBox, const Rectangle &incorrectBox, bool &hitCorrect) const;

    /// Devuelve true cuando la tubería ha pasado por completo y aún no se había marcado
    bool PassedFirstPipe() const;

    /// Posición X actual de la tubería (para alinear respuestas, etc.)
    float GetCurrentPipeX() const { return currentPipeX; }
    /// Posición Y del comienzo del gap en la tubería
    float GetCurrentGapY() const { return currentGapY; }
    /// Tamaño del gap
    float GetGapSize() const { return (float)pipeGap; }

    // Nuevos métodos para posicionar respuestas en los espacios de navegación
    /// Obtiene la posición Y para la respuesta superior
    float GetTopAnswerY() const;
    /// Obtiene la posición Y para la respuesta inferior
    float GetBottomAnswerY() const;
    /// Obtiene el ancho recomendado para las cajas de respuesta
    float GetAnswerWidth() const;
    /// Obtiene la altura recomendada para las cajas de respuesta
    float GetAnswerHeight() const;

    // Accesores a las hitboxes de tubería (útil si dibujas con DrawTexturePro)
    const std::vector<Rectangle> &GetTopPipes() const { return topPipes; }
    const std::vector<Rectangle> &GetBottomPipes() const { return bottomPipes; }
    const std::vector<Rectangle> &GetMiddlePipes() const { return middlePipes; }
    const std::vector<Rectangle> &GetKillZones() const { return killZones; }
    const std::vector<Rectangle> &GetScoreZones() const { return scoreZones; }

private:
    PipeManager();
    static PipeManager *instance;

    int screenWidth;
    int screenHeight;
    int pipeWidth;
    int pipeHeight;
    int pipeGap;

    // Para posicionar respuestas en el hueco:
    float currentPipeX;
    float currentGapY;

    std::vector<Rectangle> topPipes;
    std::vector<Rectangle> bottomPipes;
    std::vector<Rectangle> middlePipes;
    std::vector<Rectangle> killZones;
    std::vector<Rectangle> scoreZones;

    // Bandera para marcar cuándo ya se puntuó esa tubería
    bool passedFirst;
};