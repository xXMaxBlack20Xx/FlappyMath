// src/PipeManager.cpp

#include "PipeManager.h"
#include "raylib.h"
#include <cstdlib>

// Constantes internas que replican tu configuración original
static constexpr int DEFAULT_PIPE_WIDTH = 100;
static constexpr int DEFAULT_PIPE_GAP = 400;
static constexpr int DEFAULT_HALF_PIPE_HEIGHT = 150;

PipeManager *PipeManager::instance = nullptr;

PipeManager &PipeManager::GetInstance()
{
    if (!instance)
        instance = new PipeManager();
    return *instance;
}

PipeManager::PipeManager()
    : screenWidth(0), screenHeight(0), pipeWidth(DEFAULT_PIPE_WIDTH), pipeHeight(0), pipeGap(DEFAULT_PIPE_GAP), currentPipeX(0), currentGapY(0), passedFirst(false)
{
}

void PipeManager::InitPipes(int scrW, int scrH, int textureHeight)
{
    screenWidth = scrW;
    screenHeight = scrH;
    pipeHeight = textureHeight;
    pipeGap = DEFAULT_PIPE_GAP;
    pipeWidth = DEFAULT_PIPE_WIDTH;
    passedFirst = false;

    topPipes.clear();
    bottomPipes.clear();
    middlePipes.clear();
    killZones.clear();
    scoreZones.clear();

    // Elige aleatoriamente la Y del hueco, dejando márgenes
    currentGapY = GetRandomValue(100, screenHeight - 100 - pipeGap);
    currentPipeX = (float)screenWidth;

    // Tubería superior: desde y=0 hasta start of gap
    topPipes.push_back({currentPipeX,
                        0.0f,
                        (float)pipeWidth,
                        currentGapY});

    // Tubería inferior: empieza tras el gap hasta el bottom
    bottomPipes.push_back({currentPipeX,
                           currentGapY + pipeGap,
                           (float)pipeWidth,
                           (float)(screenHeight - (currentGapY + pipeGap))});

    // Middle pipe (half-pipe) centrada verticalmente en el gap
    float halfH = DEFAULT_HALF_PIPE_HEIGHT / 2.0f;
    middlePipes.push_back({currentPipeX,
                           currentGapY + pipeGap / 2.0f - halfH,
                           (float)pipeWidth,
                           (float)DEFAULT_HALF_PIPE_HEIGHT});

    // Zona de kill (colisión extra) y zona de score
    killZones.push_back({currentPipeX + pipeWidth / 4.0f - 50.0f,
                         currentGapY + pipeGap / 2.0f - halfH,
                         (float)pipeWidth,
                         (float)DEFAULT_HALF_PIPE_HEIGHT});
    scoreZones.push_back({currentPipeX + pipeWidth / 2.0f - 50.0f,
                          currentGapY + pipeGap / 2.0f - halfH,
                          (float)pipeWidth,
                          (float)DEFAULT_HALF_PIPE_HEIGHT});
}

void PipeManager::Update(float pipeSpeed)
{
    // Mueve todas las rects en X
    for (auto &r : topPipes)
        r.x -= pipeSpeed;
    for (auto &r : bottomPipes)
        r.x -= pipeSpeed;
    for (auto &r : middlePipes)
        r.x -= pipeSpeed;
    for (auto &r : killZones)
        r.x -= pipeSpeed;
    for (auto &r : scoreZones)
        r.x -= pipeSpeed;

    // Si la tubería principal sale de la pantalla, reinicia todo
    if (!topPipes.empty() && topPipes[0].x + pipeWidth < 0)
    {
        InitPipes(screenWidth, screenHeight, pipeHeight);
    }
}

void PipeManager::Draw() const
{
    // Si quieres ver outlines de debug:
    for (auto &r : topPipes)
        DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, DARKGREEN);
    for (auto &r : bottomPipes)
        DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, DARKGREEN);
    for (auto &r : middlePipes)
        DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, DARKGREEN);
}

void PipeManager::Reset()
{
    // Reinicia exactamente igual que Init
    InitPipes(screenWidth, screenHeight, pipeHeight);
}

bool PipeManager::CheckCollision(const Rectangle &playerBox) const
{
    for (auto &r : topPipes)
        if (CheckCollisionRecs(playerBox, r))
            return true;
    for (auto &r : bottomPipes)
        if (CheckCollisionRecs(playerBox, r))
            return true;
    for (auto &r : middlePipes)
        if (CheckCollisionRecs(playerBox, r))
            return true;
    for (auto &r : killZones)
        if (CheckCollisionRecs(playerBox, r))
            return true;
    return false;
}

bool PipeManager::PassedFirstPipe() const
{
    if (scoreZones.empty())
        return false;
    // Detecta la primera vez que la zona de score sale de la izquierda
    if (!passedFirst && scoreZones[0].x + scoreZones[0].width < 0.0f)
    {
        const_cast<PipeManager *>(this)->passedFirst = true;
        return true;
    }
    // Cuando la zona vuelve por la derecha, resetea el flag
    if (scoreZones[0].x > screenWidth)
    {
        const_cast<PipeManager *>(this)->passedFirst = false;
    }
    return false;
}
