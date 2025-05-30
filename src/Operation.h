// src/Operation.h
#pragma once

#include "raylib.h"

/// Clase que gestiona la generación, posicionamiento y dibujo de operaciones matemáticas
class Operation
{
public:
    Operation();

    /// Genera una nueva operación aleatoria (suma, resta, mult., división)
    /// Debe llamarse al iniciar y cada vez que sale una tubería nueva.
    void Generate(int screenWidth, int screenHeight);

    /// Mueve las casillas de respuesta horizontalmente junto con las tuberías
    void UpdateHitboxes(float pipeSpeed);

    /// Posiciona las casillas de respuesta centradas en el gap actual
    /// pipeX: coordenada X de la tubería
    /// gapY: coordenada Y de inicio del gap
    /// gapSize: altura del gap
    void PositionInGap(float pipeX, float gapY, float gapSize);

    /// Dibuja la operación y las dos respuestas (correcta/incorrecta).
    /// swapAnswers: si true intercambia cuál casilla muestra la respuesta correcta.
    void Draw(bool swapAnswers) const;

    /// Accesores a las zonas de colisión
    Rectangle GetCorrectHitbox() const { return correctBox; }
    Rectangle GetIncorrectHitbox() const { return incorrectBox; }

    /// Obtiene el valor de la respuesta correcta (para puntuar)
    float GetCorrectAnswer() const { return correctAnswer; }

private:
    int number1;
    int number2;
    char op; // '+', '-', '*', '/'
    float correctAnswer;
    float incorrectAnswer;

    Rectangle correctBox;   // hitbox de la respuesta correcta
    Rectangle incorrectBox; // hitbox de la respuesta incorrecta

    bool swapFlag; // alterna la posición de correcta/incorrecta

    static constexpr int ANSWER_SPACE_WIDTH = 50;
    static constexpr int ANSWER_SPACE_HEIGHT = 200;
};
