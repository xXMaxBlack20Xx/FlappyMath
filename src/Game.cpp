// src/Game.cpp

#include "Game.h"
#include "ScoreManager.h"
#include "PipeManager.h"
#include "UIManager.h"
#include "raylib.h"

#include <cstdlib>
#include <ctime>

// Constantes de juego
#define GRAVITY 0.5f
#define JUMP_SPEED -8.0f
#define PIPE_SPEED 5
#define PIPE_GAP 400

Game *Game::instance = nullptr;

Game &Game::GetInstance()
{
    if (!instance)
        instance = new Game();
    return *instance;
}

Game::Game()
    : screenWidth(1525), screenHeight(830), player({0, 0}, Texture2D{}), puntuacion(0), puntuacionMax(0), juegoIniciado(false), juegoTerminado(false), pasarZona(false)
{
}

void Game::Init()
{
    std::srand((unsigned)std::time(nullptr));

    InitAudioDevice();
    InitWindow(screenWidth, screenHeight, "FlappyMath Patrones Software");
    SetTargetFPS(60);

    // Carga sprites
    fondo = LoadTexture("assets/sprites/fondo.png");
    titulo = LoadTexture("assets/sprites/Titulo.png");
    jugar = LoadTexture("assets/sprites/activar.png");
    creadores = LoadTexture("assets/sprites/creadores.png");
    gover = LoadTexture("assets/sprites/FINJUEGO.png");
    pajaro = LoadTexture("assets/sprites/redbird-upflap.png");
    pipeTex = LoadTexture("assets/sprites/pipe-green.png");
    pipeUpside = LoadTexture("assets/sprites/pipe-green-upside.png");
    halfPipeTex = LoadTexture("assets/sprites/half.pipe.png");

    // Carga sonido
    wingSnd = LoadSound("assets/audio/wing.wav");
    hitSnd = LoadSound("assets/audio/hit.wav");
    dieSnd = LoadSound("assets/audio/die.wav");
    pointSnd = LoadSound("assets/audio/point.wav");
    bgMusic = LoadMusicStream("assets/audio/musicita.mp3");

    // Inicializa jugador en centro
    Vector2 startPos = {screenWidth / 2.0f - 100.0f, screenHeight / 2.0f};
    player = Player(startPos, pajaro);

    // Carga high score
    puntuacion = 0;
    puntuacionMax = ScoreManager::LoadHighScore();

    // Prepara tuberías y primera operación
    PipeManager::GetInstance().InitPipes(screenWidth, screenHeight, pipeTex.height);
    operacion.Generate(screenWidth, screenHeight);
}

void Game::Run()
{
    Init();
    while (!WindowShouldClose())
    {
        Update();
        Draw();
    }
    Unload();
    CloseWindow();
    CloseAudioDevice();
}

void Game::Update()
{
    UpdateMusicStream(bgMusic);

    // Pantalla de inicio
    if (!juegoIniciado)
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            juegoIniciado = true;
            PlayMusicStream(bgMusic);
        }
        return;
    }
    // Game Over
    if (juegoTerminado)
    {
        if (IsKeyPressed(KEY_R))
        {
            // Reset completo
            puntuacion = 0;
            juegoTerminado = false;
            pasarZona = false;

            // Reposiciona jugador
            Vector2 startPos = {screenWidth / 2.0f - 100.0f, screenHeight / 2.0f};
            player = Player(startPos, pajaro);

            // Reset tuberías y operación
            PipeManager::GetInstance().Reset();
            operacion.Generate(screenWidth, screenHeight);

            PlayMusicStream(bgMusic);
        }
        else
        {
            StopMusicStream(bgMusic);
        }
        return;
    }

    // Salto
    if (IsKeyPressed(KEY_SPACE))
    {
        player.Jump();
        PlaySound(wingSnd);
    }

    // Física jugador
    player.Update();

    // Actualiza tuberías y operación
    PipeManager::GetInstance().Update(PIPE_SPEED);
    operacion.UpdateHitboxes(PIPE_SPEED);

    // Posiciona respuestas en el gap
    float px = PipeManager::GetInstance().GetCurrentPipeX();
    float gy = PipeManager::GetInstance().GetCurrentGapY();
    operacion.PositionInGap(px, gy, PIPE_GAP);

    // Colisiones
    Rectangle playerBox = player.GetCollisionBox();
    bool hitPipe = PipeManager::GetInstance().CheckCollision(playerBox);
    bool hitWrong = CheckCollisionRecs(playerBox, operacion.GetIncorrectHitbox());
    if (hitPipe || hitWrong || playerBox.y < 0 || playerBox.y + playerBox.height > screenHeight)
    {
        juegoTerminado = true;
        PlaySound(hitSnd);
        PlaySound(dieSnd);
    }

    // Puntuar si toca la respuesta correcta
    if (CheckCollisionRecs(playerBox, operacion.GetCorrectHitbox()) && !pasarZona)
    {
        puntuacion++;
        pasarZona = true;
        PlaySound(pointSnd);
    }

    // Permite puntuar de nuevo al pasar la tubería
    if (PipeManager::GetInstance().PassedFirstPipe())
    {
        pasarZona = false;
        operacion.Generate(screenWidth, screenHeight);
    }

    // Guarda high score
    if (juegoTerminado && puntuacion > puntuacionMax)
    {
        puntuacionMax = puntuacion;
        ScoreManager::SaveHighScore(puntuacionMax);
    }
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTextureV(fondo, {0, 0}, WHITE);

    if (!juegoIniciado)
    {
        UIManager::DrawStartScreen(fondo, titulo, jugar, puntuacionMax);
    }
    else if (juegoTerminado)
    {
        UIManager::DrawGameOverScreen(fondo, gover, puntuacion);
    }
    else
    {
        player.Draw();

        // Dibuja tuberías con sus texturas
        for (auto &r : PipeManager::GetInstance().GetTopPipes())
            DrawTexturePro(pipeUpside, {0, 0, (float)pipeUpside.width, (float)pipeUpside.height}, r, {0, 0}, 0, WHITE);
        for (auto &r : PipeManager::GetInstance().GetBottomPipes())
            DrawTexturePro(pipeTex, {0, 0, (float)pipeTex.width, (float)pipeTex.height}, r, {0, 0}, 0, WHITE);
        for (auto &r : PipeManager::GetInstance().GetMiddlePipes())
            DrawTexturePro(halfPipeTex, {0, 0, (float)halfPipeTex.width, (float)halfPipeTex.height}, r, {0, 0}, 0, WHITE);

        // Dibuja la operación en el gap
        operacion.Draw(false);

        // HUD de puntuación
        DrawText(TextFormat("PUNTUACIÓN: %d", puntuacion),
                 screenWidth / 2 - MeasureText("PUNTUACIÓN: 0", 40) / 2,
                 10, 40, BLACK);
    }

    EndDrawing();
}

void Game::Unload()
{
    UnloadTexture(fondo);
    UnloadTexture(titulo);
    UnloadTexture(jugar);
    UnloadTexture(creadores);
    UnloadTexture(gover);
    UnloadTexture(pajaro);
    UnloadTexture(pipeTex);
    UnloadTexture(pipeUpside);
    UnloadTexture(halfPipeTex);

    UnloadSound(wingSnd);
    UnloadSound(hitSnd);
    UnloadSound(dieSnd);
    UnloadSound(pointSnd);
    UnloadMusicStream(bgMusic);
}
