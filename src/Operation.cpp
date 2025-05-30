#include "Operation.h"
#include "raylib.h"
#include <cstdlib>
#include <cstdio>

Operation::Operation()
    : number1(0), number2(0), op('+'), correctAnswer(0.0f), incorrectAnswer(0.0f), correctBox{0, 0, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT}, incorrectBox{0, 0, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT}, swapFlag(false)
{
}

void Operation::Generate(int screenWidth, int screenHeight)
{
    // Generar números y operación
    number1 = GetRandomValue(1, 20);
    number2 = GetRandomValue(1, 20);
    int type = GetRandomValue(0, 3);
    switch (type)
    {
    case 0:
        op = '+';
        correctAnswer = number1 + number2;
        break;
    case 1:
        op = '-';
        correctAnswer = number1 - number2;
        break;
    case 2:
        op = '*';
        correctAnswer = number1 * number2;
        break;
    case 3:
        op = '/';
        if (number2 == 0)
            number2 = 1;
        correctAnswer = (float)number1 / (float)number2;
        break;
    }
    // Respuesta incorrecta
    incorrectAnswer = correctAnswer + GetRandomValue(1, 30);

    // Inicialmente colocarlo fuera de pantalla a la derecha
    float x = (float)screenWidth;
    float midY = screenHeight / 2.0f;
    if (swapFlag)
    {
        correctBox = {x, midY + ANSWER_SPACE_HEIGHT / 2.0f, (float)ANSWER_SPACE_WIDTH, (float)ANSWER_SPACE_HEIGHT};
        incorrectBox = {x, midY - ANSWER_SPACE_HEIGHT / 2.0f, (float)ANSWER_SPACE_WIDTH, (float)ANSWER_SPACE_HEIGHT};
    }
    else
    {
        correctBox = {x, midY - ANSWER_SPACE_HEIGHT / 2.0f, (float)ANSWER_SPACE_WIDTH, (float)ANSWER_SPACE_HEIGHT};
        incorrectBox = {x, midY + ANSWER_SPACE_HEIGHT / 2.0f, (float)ANSWER_SPACE_WIDTH, (float)ANSWER_SPACE_HEIGHT};
    }
    swapFlag = !swapFlag;
}

void Operation::UpdateHitboxes(float pipeSpeed)
{
    correctBox.x -= pipeSpeed;
    incorrectBox.x -= pipeSpeed;
}

void Operation::PositionInGap(float pipeX, float gapY, float gapSize)
{
    // Centra el gap verticalmente
    float centerY = gapY + gapSize / 2.0f;
    float yTop = centerY - ANSWER_SPACE_HEIGHT / 2.0f;
    float margin = 20.0f;

    if (swapFlag)
    {
        // la caja correcta va abajo
        incorrectBox = {pipeX, yTop, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT};
        correctBox = {pipeX, yTop + ANSWER_SPACE_HEIGHT + margin, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT};
    }
    else
    {
        // la caja correcta arriba
        correctBox = {pipeX, yTop, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT};
        incorrectBox = {pipeX, yTop + ANSWER_SPACE_HEIGHT + margin, ANSWER_SPACE_WIDTH, ANSWER_SPACE_HEIGHT};
    }
    swapFlag = !swapFlag;
}

void Operation::Draw(bool swapAnswers) const
{
    const int fontSize = 35;
    // Texto de la operación
    float tx = correctBox.x - 120;
    float ty = correctBox.y + (correctBox.height - fontSize) / 2.0f;
    DrawText(TextFormat("%d %c %d =", number1, op, number2), (int)tx, (int)ty, fontSize, WHITE);

    // Prepara cadenas
    char bufC[32], bufI[32];
    snprintf(bufC, sizeof(bufC), "%.2f", correctAnswer);
    snprintf(bufI, sizeof(bufI), "%.2f", incorrectAnswer);
    int twC = MeasureText(bufC, fontSize);
    int twI = MeasureText(bufI, fontSize);

    // Dibuja respuestas
    if (swapAnswers)
    {
        DrawText(bufI,
                 (int)(correctBox.x + (correctBox.width - twI) / 2.0f),
                 (int)(correctBox.y + (correctBox.height - fontSize) / 2.0f),
                 fontSize, WHITE);
        DrawText(bufC,
                 (int)(incorrectBox.x + (incorrectBox.width - twC) / 2.0f),
                 (int)(incorrectBox.y + (incorrectBox.height - fontSize) / 2.0f),
                 fontSize, WHITE);
    }
    else
    {
        DrawText(bufC,
                 (int)(correctBox.x + (correctBox.width - twC) / 2.0f),
                 (int)(correctBox.y + (correctBox.height - fontSize) / 2.0f),
                 fontSize, WHITE);
        DrawText(bufI,
                 (int)(incorrectBox.x + (incorrectBox.width - twI) / 2.0f),
                 (int)(incorrectBox.y + (incorrectBox.height - fontSize) / 2.0f),
                 fontSize, WHITE);
    }

    // Debug hitboxes
    DrawRectangleLines((int)correctBox.x, (int)correctBox.y,
                       (int)correctBox.width, (int)correctBox.height, GREEN);
    DrawRectangleLines((int)incorrectBox.x, (int)incorrectBox.y,
                       (int)incorrectBox.width, (int)incorrectBox.height, RED);
}
