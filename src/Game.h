// src/Game.h
#pragma once

#include "raylib.h"
#include "Player.h"
#include "Operation.h"
#include "PipeManager.h"
#include "UIManager.h"
#include "ScoreManager.h"

/// Clase principal que controla el ciclo de vida del juego (Singleton).
class Game
{
private:
    static Game *instance;

    // Dimensiones de la ventana
    int screenWidth;
    int screenHeight;

    // Recursos gráficos
    Texture2D fondo;
    Texture2D titulo;
    Texture2D jugar;
    Texture2D creadores;
    Texture2D gover;
    Texture2D pajaro;
    Texture2D pipeTex;
    Texture2D pipeUpside;
    Texture2D halfPipeTex;

    // Recursos de audio
    Sound wingSnd;
    Sound hitSnd;
    Sound dieSnd;
    Sound pointSnd;
    Music bgMusic;

    // Entidades del juego
    Player player;
    Operation operacion;

    // Estado y puntaje
    int puntuacion;
    int puntuacionMax;
    bool juegoIniciado;
    bool juegoTerminado;
    bool pasarZona;

    // Constructor privado para Singleton
    Game();

public:
    /// Obtiene la instancia única de Game
    static Game &GetInstance();

    /// Inicializa todo (ventana, recursos, puntuación, entidades)
    void Init();

    /// Bucle principal: Update + Draw hasta cerrar
    void Run();

    /// Lógica y actualizaciones de cada frame
    void Update();

    /// Dibujado de cada frame
    void Draw();

    /// Libera todos los recursos cargados
    void Unload();

    /// Destructor (por si hay que manejar cleanup adicional)
    ~Game();
};
