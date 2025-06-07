// src/PipeManager.cpp

#include "PipeManager.h"
#include "raylib.h"
#include <cstdlib>

// Constantes internas que replican tu configuración original
static constexpr int DEFAULT_PIPE_WIDTH = 100;
static constexpr int DEFAULT_PIPE_GAP = 400;

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

    // Crear middle pipe en el centro del gap, pero con espacios más grandes para las respuestas
    // Dejamos 120px de espacio de navegación cada uno (240px total)
    float navigationSpaceEach = 120.0f;                      // Espacio para cada respuesta (arriba y abajo)
    float totalNavigationSpace = navigationSpaceEach * 2.0f; // 240px total

    float middlePipeHeight = (float)pipeGap - totalNavigationSpace;
    float middlePipeY = currentGapY + navigationSpaceEach; // Centrado con margen de 120px arriba

    // Solo crear middle pipe si queda espacio suficiente
    if (middlePipeHeight > 20.0f) // Mínimo 20px de altura para el pipe central
    {
        middlePipes.push_back({currentPipeX,
                               middlePipeY,
                               (float)pipeWidth,
                               middlePipeHeight});
    }

    // Zonas de respuesta más grandes (sin kill zones, solo para navegación)
    // Zona superior del gap (espacio entre top pipe y middle pipe) - RESPUESTA 1
    Rectangle topAnswerZone = {currentPipeX, currentGapY, (float)pipeWidth, navigationSpaceEach};

    // Zona inferior del gap (espacio entre middle pipe y bottom pipe) - RESPUESTA 2
    Rectangle bottomAnswerZone = {currentPipeX,
                                  middlePipeY + middlePipeHeight,
                                  (float)pipeWidth,
                                  navigationSpaceEach};

    // NO agregamos estas zonas como kill zones - son para las respuestas
    // Las respuestas se colocarán en estos espacios desde el código principal

    // Zona de puntuación: el área completa del gap para detectar cuando el jugador pasa
    Rectangle scoreZone = {currentPipeX, currentGapY, (float)pipeWidth, (float)pipeGap};
    scoreZones.push_back(scoreZone);
}

void PipeManager::Update(float pipeSpeed)
{
    // Actualizar currentPipeX para las operaciones
    currentPipeX -= pipeSpeed;

    // Mueve todas las rects en X
    for (auto &r : topPipes)
    {
        r.x -= pipeSpeed;
    }
    for (auto &r : bottomPipes)
    {
        r.x -= pipeSpeed;
    }
    for (auto &r : middlePipes)
    {
        r.x -= pipeSpeed;
    }
    for (auto &r : killZones)
    {
        r.x -= pipeSpeed;
    }
    for (auto &r : scoreZones)
    {
        r.x -= pipeSpeed;
    }

    // CAMBIO: Solo reiniciar después de que la zona de score haya pasado completamente
    // y haya algo de distancia extra para evitar conflictos
    if (!scoreZones.empty() && scoreZones[0].x + scoreZones[0].width < -50.0f)
    {
        InitPipes(screenWidth, screenHeight, pipeHeight);
    }
}

// También necesitas actualizar el Draw para mostrar el middle pipe
void PipeManager::Draw() const
{
    // Dibujar todas las tuberías (top, bottom y middle)
    for (auto &r : topPipes)
        DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, DARKGREEN);
    for (auto &r : bottomPipes)
        DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, DARKGREEN);
    for (auto &r : middlePipes)
        DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, DARKGREEN);

    
    // Debug: mostrar kill zones en rojo (ahora debería estar vacío)
    for (auto &r : killZones)
        DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, BLANK);

    // Debug: mostrar score zone en azul
    for (auto &r : scoreZones)
        DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, BLANK);


    // Debug: mostrar las zonas de respuesta en verde claro
    if (!middlePipes.empty())
    {
        // Zona superior para respuesta 1
        DrawRectangleLines((int)currentPipeX, (int)currentGapY, pipeWidth, 120, BLANK);

        // Zona inferior para respuesta 2
        float bottomZoneY = middlePipes[0].y + middlePipes[0].height;
        DrawRectangleLines((int)currentPipeX, (int)bottomZoneY, pipeWidth, 120, BLANK);
    }

}

void PipeManager::Reset()
{
    // Reinicia exactamente igual que Init
    InitPipes(screenWidth, screenHeight, pipeHeight);
}

// Actualizar CheckCollision para incluir middle pipes
bool PipeManager::CheckCollision(const Rectangle &playerBox) const
{
    // Colisión con tuberías sólidas (top, bottom y middle)
    for (auto &r : topPipes)
        if (CheckCollisionRecs(playerBox, r))
            return true;
    for (auto &r : bottomPipes)
        if (CheckCollisionRecs(playerBox, r))
            return true;
    for (auto &r : middlePipes)
        if (CheckCollisionRecs(playerBox, r))
            return true;

    // Ya no hay kill zones en el gap - solo las tuberías sólidas
    for (auto &r : killZones)
        if (CheckCollisionRecs(playerBox, r))
            return true;

    return false;
}
// Nueva función para verificar si el jugador tocó una respuesta
bool PipeManager::CheckAnswerCollision(const Rectangle &playerBox, const Rectangle &correctBox, const Rectangle &incorrectBox, bool &hitCorrect) const
{
    // Solo verificar respuestas si el jugador está en el área de score
    bool inScoreZone = false;
    for (const auto &zone : scoreZones)
    {
        if (CheckCollisionRecs(playerBox, zone))
        {
            inScoreZone = true;
            break;
        }
    }

    if (!inScoreZone)
        return false;

    // Verificar colisión con respuestas
    if (CheckCollisionRecs(playerBox, correctBox))
    {
        hitCorrect = true;
        return true;
    }

    if (CheckCollisionRecs(playerBox, incorrectBox))
    {
        hitCorrect = false;
        return true;
    }

    return false;
}

bool PipeManager::PassedFirstPipe() const
{
    if (scoreZones.empty())
        return false;

    // Detecta cuando el jugador ha pasado el centro de la zona de score
    float playerPassPoint = screenWidth * 0.2f; // Asume que el jugador está en ~20% de la pantalla

    if (!passedFirst && scoreZones[0].x + scoreZones[0].width / 2 < playerPassPoint)
    {
        const_cast<PipeManager *>(this)->passedFirst = true;
        return true;
    }

    // Reset flag cuando la pipe vuelve a aparecer
    if (scoreZones[0].x > screenWidth)
    {
        const_cast<PipeManager *>(this)->passedFirst = false;
    }

    return false;
}

// Funciones auxiliares para posicionar las respuestas
float PipeManager::GetTopAnswerY() const
{
    return currentGapY + 20.0f; // Un poco hacia abajo del borde superior
}

float PipeManager::GetBottomAnswerY() const
{
    if (!middlePipes.empty())
    {
        return middlePipes[0].y + middlePipes[0].height + 20.0f; // Un poco hacia abajo del middle pipe
    }
    return currentGapY + pipeGap - 120.0f + 20.0f; // Fallback
}

float PipeManager::GetAnswerWidth() const
{
    return (float)pipeWidth - 20.0f; // Un poco más estrecho que el pipe
}

float PipeManager::GetAnswerHeight() const
{
    return 80.0f; // Altura fija para las cajas de respuesta
}