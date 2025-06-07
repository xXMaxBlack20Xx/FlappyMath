#include "Operation.h"
#include "raylib.h"
#include <cstdlib>
#include <cstdio>

Operation::Operation()
    : number1(0), number2(0), op('+'), correctAnswer(0.0f), incorrectAnswer(0.0f),
      correctBox{0, 0, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT},
      incorrectBox{0, 0, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT},
      swapFlag(false)
{
}

void Operation::Generate(int screenWidth, int screenHeight)
{
    // Generar números y operación
    number1 = GetRandomValue(1, 12); // Números más pequeños para mejor jugabilidad
    number2 = GetRandomValue(1, 12);
    int type = GetRandomValue(0, 3);

    switch (type)
    {
    case 0:
        op = '+';
        correctAnswer = number1 + number2;
        break;
    case 1:
        op = '-';
        // Asegurar que el resultado no sea negativo
        if (number1 < number2)
        {
            int temp = number1;
            number1 = number2;
            number2 = temp;
        }
        correctAnswer = number1 - number2;
        break;
    case 2:
        op = '*';
        correctAnswer = number1 * number2;
        break;
    case 3:
        op = '/';
        // Para división, hacer que number1 sea múltiplo de number2
        number1 = number2 * GetRandomValue(1, 5);
        correctAnswer = (float)number1 / (float)number2;
        break;
    }

    // Generar respuesta incorrecta que no sea igual a la correcta
    do
    {
        if (op == '/' || correctAnswer < 1)
        {
            // Para divisiones o números pequeños, variar por decimales
            incorrectAnswer = correctAnswer + GetRandomValue(-10, 10) * 0.1f;
        }
        else
        {
            // Para enteros, variar por unidades
            incorrectAnswer = correctAnswer + GetRandomValue(-5, 5);
        }
    } while (abs(incorrectAnswer - correctAnswer) < 0.1f);

    // Inicialmente colocar fuera de pantalla a la derecha
    float x = (float)screenWidth + 100;
    float midY = screenHeight / 2.0f;

    correctBox = {x, midY - ANSWER_SPACE_HEIGHT, (float)ANSWER_SPACE_WIDTH, (float)ANSWER_SPACE_HEIGHT};
    incorrectBox = {x, midY + 20, (float)ANSWER_SPACE_WIDTH, (float)ANSWER_SPACE_HEIGHT};

    // Alternar posiciones cada vez
    swapFlag = !swapFlag;
}

void Operation::UpdateHitboxes(float pipeSpeed)
{
    correctBox.x -= pipeSpeed;
    incorrectBox.x -= pipeSpeed;
}

void Operation::PositionInGap(float pipeX, float gapY, float gapSize)
{
    // Asegurar que las cajas quepan en el gap con margen
    float totalHeight = ANSWER_SPACE_HEIGHT * 2 + 30; // 30px de separación entre cajas

    // Si el gap es muy pequeño, ajustar el tamaño de las cajas
    if (totalHeight > gapSize - 40)
    { // 40px de margen total
        // Usar cajas más pequeñas si es necesario
        float availableHeight = gapSize - 40;
        float boxHeight = (availableHeight - 20) / 2; // 20px separación entre cajas

        if (swapFlag)
        {
            incorrectBox = {pipeX + 10, gapY + 20, ANSWER_SPACE_WIDTH, boxHeight};
            correctBox = {pipeX + 10, gapY + 20 + boxHeight + 20, ANSWER_SPACE_WIDTH, boxHeight};
        }
        else
        {
            correctBox = {pipeX + 10, gapY + 20, ANSWER_SPACE_WIDTH, boxHeight};
            incorrectBox = {pipeX + 10, gapY + 20 + boxHeight + 20, ANSWER_SPACE_WIDTH, boxHeight};
        }
    }
    else
    {
        // Usar tamaño normal
        float centerY = gapY + gapSize / 2.0f;
        float spacing = 15.0f;

        if (swapFlag)
        {
            incorrectBox = {pipeX + 10, centerY - ANSWER_SPACE_HEIGHT - spacing / 2, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT};
            correctBox = {pipeX + 10, centerY + spacing / 2, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT};
        }
        else
        {
            correctBox = {pipeX + 10, centerY - ANSWER_SPACE_HEIGHT - spacing / 2, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT};
            incorrectBox = {pipeX + 10, centerY + spacing / 2, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT};
        }
    }
}

void Operation::Draw(bool swapAnswers) const
{
    const int fontSize = 24; // Tamaño más pequeño para mejor legibilidad

    // Posición del texto de la operación (a la izquierda de las cajas)
    float operationX = correctBox.x - 140;
    float operationY = (correctBox.y + incorrectBox.y + incorrectBox.height) / 2.0f - fontSize / 2.0f;

    // Dibujar la operación
    DrawText(TextFormat("%d %c %d =", number1, op, number2),
             (int)operationX, (int)operationY, fontSize, WHITE);

    // Preparar las cadenas de respuesta
    char correctText[32], incorrectText[32];

    if (op == '/' && correctAnswer != (int)correctAnswer)
    {
        snprintf(correctText, sizeof(correctText), "%.1f", correctAnswer);
        snprintf(incorrectText, sizeof(incorrectText), "%.1f", incorrectAnswer);
    }
    else
    {
        snprintf(correctText, sizeof(correctText), "%.0f", correctAnswer);
        snprintf(incorrectText, sizeof(incorrectText), "%.0f", incorrectAnswer);
    }

    // Calcular posiciones centradas para el texto
    int correctTextWidth = MeasureText(correctText, fontSize);
    int incorrectTextWidth = MeasureText(incorrectText, fontSize);

    // Determinar qué texto va en cada caja
    const char *topText;
    const char *bottomText;
    int topTextWidth, bottomTextWidth;
    Color topColor, bottomColor;

    if (swapAnswers)
    {
        // Intercambiar visualmente las respuestas
        topText = (swapFlag) ? correctText : incorrectText;
        bottomText = (swapFlag) ? incorrectText : correctText;
        topTextWidth = (swapFlag) ? correctTextWidth : incorrectTextWidth;
        bottomTextWidth = (swapFlag) ? incorrectTextWidth : correctTextWidth;
        topColor = (swapFlag) ? GREEN : RED;
        bottomColor = (swapFlag) ? RED : GREEN;
    }
    else
    {
        // Mostrar respuestas en su posición normal
        topText = (swapFlag) ? incorrectText : correctText;
        bottomText = (swapFlag) ? correctText : incorrectText;
        topTextWidth = (swapFlag) ? incorrectTextWidth : correctTextWidth;
        bottomTextWidth = (swapFlag) ? correctTextWidth : incorrectTextWidth;
        topColor = (swapFlag) ? RED : GREEN;
        bottomColor = (swapFlag) ? GREEN : RED;
    }

    // Dibujar el texto en las cajas (la caja superior es la que está más arriba)
    Rectangle topBox = (correctBox.y < incorrectBox.y) ? correctBox : incorrectBox;
    Rectangle bottomBox = (correctBox.y < incorrectBox.y) ? incorrectBox : correctBox;

    // Texto en caja superior
    DrawText(topText,
             (int)(topBox.x + (topBox.width - topTextWidth) / 2.0f),
             (int)(topBox.y + (topBox.height - fontSize) / 2.0f),
             fontSize, WHITE);

    // Texto en caja inferior
    DrawText(bottomText,
             (int)(bottomBox.x + (bottomBox.width - bottomTextWidth) / 2.0f),
             (int)(bottomBox.y + (bottomBox.height - fontSize) / 2.0f),
             fontSize, WHITE);

    // Dibujar bordes de las cajas (verde para correcta, rojo para incorrecta)
    DrawRectangleLines((int)correctBox.x, (int)correctBox.y,
                       (int)correctBox.width, (int)correctBox.height, BLANK);
    DrawRectangleLines((int)incorrectBox.x, (int)incorrectBox.y,
                       (int)incorrectBox.width, (int)incorrectBox.height, BLANK);
}