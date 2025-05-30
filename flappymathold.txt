// Juego de FlappyMath: flappymath

#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define GRAVITY 0.5f
#define JUMP_SPEED -8.0f
#define PIPE_SPEED 5
#define PIPE_GAP 400
#define PIPE_WIDTH 100
#define HALF_PIPE_HEIGHT 150
#define ANSWER_SPACE_HEIGHT 200
#define ANSWER_SPACE_WIDTH 50

Texture2D gover;

static bool intercambiarRespuestas = false; // Alterna entre true y false

typedef struct Jugador
{
    Vector2 posicion;
    float radio;
    float velocidadY;
    Texture2D textura;
} Jugador;

typedef struct Operacion
{
    int numero1;
    int numero2;
    char operador;
    float respuestaCorrecta;
    float respuestaIncorrecta;
    Rectangle espacioRespuestaCorrecta;
    Rectangle espacioRespuestaIncorrecta;
} Operacion;

void InicializarTubos(Rectangle *tuboSuperior, Rectangle *tuboInferior, Rectangle *halfPipe, Rectangle *halfPipeKill, Rectangle *halfPipeScore, int anchoPantalla, int altoPantalla, int alturaTuberia, Operacion *operacion);
void GenerarOperacion(Operacion *operacion, int anchoPantalla, int altoPantalla);
void DibujarOperacion(Operacion operacion, Jugador jugador, bool intercambiarRespuestas);
void DibujarPantallaInicio(int puntuacion);
void DibujarPantallaGameOver(int puntuacion);
void GuardarPuntuacion(int puntuacion);
int CargarPuntuacion();

int main(void)
{
    srand(time(NULL));

    const int anchoPantalla = 1525;
    const int altoPantalla = 830;
    InitAudioDevice();
    InitWindow(anchoPantalla, altoPantalla, "Flappy Math - Programacion Estructurada");
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);

    Texture2D fondo = LoadTexture("assets/sprites/fondo.png");
    Texture2D titulo = LoadTexture("assets/sprites/Titulo.png");
    Texture2D jugar = LoadTexture("assets/sprites/activar.png");
    Texture2D creadores = LoadTexture("assets/sprites/creadores.png");
    gover = LoadTexture("assets/sprites/FINJUEGO.png");
    Texture2D pajaro = LoadTexture("assets/sprites/redbird-upflap.png");
    Texture2D tuberiaInferior = LoadTexture("assets/sprites/pipe-green.png");
    Texture2D tuberiaSuperior = LoadTexture("assets/sprites/pipe-green-upside.png");
    Texture2D halfPipe = LoadTexture("assets/sprites/half.pipe.png");

    Sound sonido = LoadSound("assets/audio/wing.wav");
    Sound sonidod = LoadSound("assets/audio/hit.wav");
    Sound sonidod2 = LoadSound("assets/audio/die.wav");
    Sound sonidot = LoadSound("assets/audio/point.wav");
    Music musi_fond = LoadMusicStream("assets/audio/musicita.mp3");

    Jugador jugador = {0};
    jugador.posicion = (Vector2){anchoPantalla / 2.0f - 100, altoPantalla / 2.0f};
    jugador.radio = 18;
    jugador.velocidadY = 0;
    jugador.textura = pajaro;

    Rectangle tuboSuperior;
    Rectangle tuboInferior;
    Rectangle tuboMedio;
    Rectangle halfPipeKill;
    Rectangle halfPipeScore;

    Operacion operacion;

    int puntuacion = 0;
    int puntuacionMasAlta = CargarPuntuacion();
    bool juegoIniciado = false;
    bool juegoTerminado = false;
    bool pasarZona = false;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (!juegoIniciado)
        {
            BeginDrawing();
            ClearBackground(DARKGREEN);
            DrawTextureV(fondo, (Vector2){0, 0}, WHITE);
            DrawTextureV(titulo, (Vector2){325, 200}, WHITE);
            DrawTextureV(jugar, (Vector2){445, 530}, WHITE);
            DrawTextureV(creadores, (Vector2){15, 760}, WHITE);

            DibujarPantallaInicio(puntuacionMasAlta);
            EndDrawing();
            if (IsKeyPressed(KEY_SPACE))
            {
                juegoIniciado = true;
                PlayMusicStream(musi_fond); // Iniciar música al comenzar el juego
            }
            continue;
        }

        if (juegoTerminado)
        {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTextureV(fondo, (Vector2){0, 0}, WHITE);
            DibujarPantallaGameOver(puntuacion);
            EndDrawing();

            if (puntuacion > puntuacionMasAlta)
            {
                puntuacionMasAlta = puntuacion;
                GuardarPuntuacion(puntuacionMasAlta);
            }

            if (IsKeyPressed(KEY_R))
            {
                juegoIniciado = false;
                juegoTerminado = false;
                puntuacion = 0;
                jugador.posicion = (Vector2){anchoPantalla / 2.0f - 100, altoPantalla / 2.0f};
                InicializarTubos(&tuboSuperior, &tuboInferior, &tuboMedio, &halfPipeKill, &halfPipeScore, anchoPantalla, altoPantalla, tuberiaSuperior.height, &operacion);
                GenerarOperacion(&operacion, anchoPantalla, altoPantalla);
                PlayMusicStream(musi_fond); // Reiniciar música al reiniciar el juego
            }
            else
            {
                StopMusicStream(musi_fond); // Detener música si el juego ha terminado
            }
            continue;
        }

        UpdateMusicStream(musi_fond); // Actualizar el flujo de música

        if (IsKeyPressed(KEY_SPACE))
            jugador.velocidadY = JUMP_SPEED;

        if (IsKeyPressed(KEY_SPACE))
            PlaySound(sonido);

        jugador.velocidadY += GRAVITY;
        jugador.posicion.y += jugador.velocidadY;

        // Update within your main game loop:
        tuboSuperior.x -= PIPE_SPEED;
        tuboInferior.x -= PIPE_SPEED;
        tuboMedio.x -= PIPE_SPEED;
        halfPipeKill.x -= PIPE_SPEED;
        halfPipeScore.x -= PIPE_SPEED;

        // Actualiza la posición Y de las hitboxes de las respuestas
        float espacioEntreRespuestas = 50; // Espacio adicional entre las hitboxes
        float offset = 20;                 // Ajuste para acercar las hitboxes a los tubos

        operacion.espacioRespuestaCorrecta.y = tuboSuperior.y + tuboSuperior.height + offset;                                         // Alineado con el hueco superior
        operacion.espacioRespuestaIncorrecta.y = operacion.espacioRespuestaCorrecta.y + ANSWER_SPACE_HEIGHT + espacioEntreRespuestas; // Separación entre las hitboxes

        // Mueve las hitboxes de las respuestas
        operacion.espacioRespuestaCorrecta.x -= PIPE_SPEED;   // Mueve la hitbox correcta
        operacion.espacioRespuestaIncorrecta.x -= PIPE_SPEED; // Mueve la hitbox incorrecta

        if (tuboSuperior.x + PIPE_WIDTH < 0)
        {
            // Solo inicializa las tuberías y genera una nueva operación una vez
            InicializarTubos(&tuboSuperior, &tuboInferior, &tuboMedio, &halfPipeKill, &halfPipeScore, anchoPantalla, altoPantalla, (int)tuberiaSuperior.height, &operacion);
            GenerarOperacion(&operacion, anchoPantalla, altoPantalla);
            pasarZona = false; // Reinicia la zona de puntuación
        }

        // Verifica colisiones
        if (CheckCollisionCircleRec(jugador.posicion, jugador.radio, tuboSuperior) ||
            CheckCollisionCircleRec(jugador.posicion, jugador.radio, tuboInferior) ||
            CheckCollisionCircleRec(jugador.posicion, jugador.radio, tuboMedio) ||
            CheckCollisionCircleRec(jugador.posicion, jugador.radio, halfPipeKill) ||
            CheckCollisionCircleRec(jugador.posicion, jugador.radio, operacion.espacioRespuestaIncorrecta))
        {
            juegoTerminado = true;
            PlaySound(sonidod);
            PlaySound(sonidod2);
            StopMusicStream(musi_fond); // Detener música al morir
        }

        // Manejo de puntuación
        if (CheckCollisionCircleRec(jugador.posicion, jugador.radio, operacion.espacioRespuestaCorrecta) && !pasarZona)
        {
            puntuacion++;
            pasarZona = true;
            PlaySound(sonidot);
        }

        if (CheckCollisionCircleRec(jugador.posicion, jugador.radio, halfPipeScore) && !pasarZona)
        {
            puntuacion++;
            pasarZona = true;
            PlaySound(sonidot);
        }

        if (jugador.posicion.y + jugador.radio > altoPantalla || jugador.posicion.y - jugador.radio < 0)
        {
            juegoTerminado = true;
            PlaySound(sonidod);
            PlaySound(sonidod2);
            StopMusicStream(musi_fond); // Detener música al caer
        }

        BeginDrawing();

        DrawTextureV(fondo, (Vector2){0, 0}, WHITE);

        DrawTextureV(jugador.textura, jugador.posicion, WHITE);

        DrawTexturePro(tuberiaSuperior,
                       (Rectangle){0, 0, (float)tuberiaSuperior.width, (float)tuberiaSuperior.height},
                       (Rectangle){tuboSuperior.x, tuboSuperior.y, PIPE_WIDTH, tuboSuperior.height},
                       (Vector2){0, 0}, 0, WHITE);

        DrawTexturePro(tuberiaInferior,
                       (Rectangle){0, 0, (float)tuberiaInferior.width, (float)tuberiaInferior.height},
                       (Rectangle){tuboInferior.x, tuboInferior.y, PIPE_WIDTH, tuboInferior.height},
                       (Vector2){0, 0}, 0, WHITE);

        DrawTexturePro(halfPipe,
                       (Rectangle){0, 0, (float)halfPipe.width, (float)halfPipe.height},
                       (Rectangle){tuboMedio.x, tuboMedio.y, PIPE_WIDTH, tuboMedio.height},
                       (Vector2){0, 0}, 0, WHITE);

        DibujarOperacion(operacion, jugador, intercambiarRespuestas);

        DrawText(TextFormat("PUNTUACION: %d", puntuacion), anchoPantalla / 2 - MeasureText(TextFormat("PUNTUACION: %d", puntuacion), 40) / 2, 10, 40, BLACK);

        EndDrawing();
    }

    UnloadTexture(fondo);
    UnloadTexture(pajaro);
    UnloadTexture(gover);
    UnloadTexture(titulo);
    UnloadTexture(jugar);
    UnloadTexture(creadores);
    UnloadTexture(tuberiaInferior);
    UnloadTexture(tuberiaSuperior);
    UnloadTexture(halfPipe);

    UnloadSound(sonido);
    UnloadSound(sonidod);
    UnloadSound(sonidod2);
    UnloadSound(sonidot);
    UnloadMusicStream(musi_fond);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void InicializarTubos(Rectangle *tuboSuperior, Rectangle *tuboInferior, Rectangle *halfPipe, Rectangle *halfPipeKill, Rectangle *halfPipeScore, int anchoPantalla, int altoPantalla, int alturaTuberia, Operacion *operacion)
{
    int randomY = GetRandomValue(-150, 150);

    // Espaciado entre tuberías
    float pipeGap = 500.0f;                                  // Ajusta el tamaño del espacio entre las tuberías
    float pipeCenterY = (float)(altoPantalla / 2) + randomY; // Punto central de las tuberías

    // Tuberías superior e inferior
    *tuboSuperior = (Rectangle){(float)anchoPantalla, pipeCenterY - pipeGap / 2 - (float)alturaTuberia, PIPE_WIDTH, (float)alturaTuberia};
    *tuboInferior = (Rectangle){(float)anchoPantalla, pipeCenterY + pipeGap / 2, PIPE_WIDTH, (float)alturaTuberia};

    // Half pipe centrado
    *halfPipe = (Rectangle){(float)anchoPantalla, pipeCenterY - HALF_PIPE_HEIGHT / 2, PIPE_WIDTH, HALF_PIPE_HEIGHT};

    // Hitboxes ajustadas
    *halfPipeKill = (Rectangle){(float)anchoPantalla + PIPE_WIDTH / 4 - 50, pipeCenterY - HALF_PIPE_HEIGHT / 2, PIPE_WIDTH, HALF_PIPE_HEIGHT};  // Ajustado a la izquierda
    *halfPipeScore = (Rectangle){(float)anchoPantalla + PIPE_WIDTH / 2 - 50, pipeCenterY - HALF_PIPE_HEIGHT / 2, PIPE_WIDTH, HALF_PIPE_HEIGHT}; // Ajustado a la izquierda

    // Correct and incorrect answer hitboxes
    operacion->espacioRespuestaIncorrecta = (Rectangle){
        .x = (float)anchoPantalla + PIPE_WIDTH - 150,              // Ajustado a la izquierda
        .y = pipeCenterY + PIPE_GAP / 2 - ANSWER_SPACE_HEIGHT / 2, // Alineado con el hueco inferior
        .width = ANSWER_SPACE_WIDTH,
        .height = ANSWER_SPACE_HEIGHT};

    operacion->espacioRespuestaCorrecta = (Rectangle){
        .x = (float)anchoPantalla + PIPE_WIDTH - 150,              // Ajustado a la izquierda
        .y = pipeCenterY - PIPE_GAP / 2 - ANSWER_SPACE_HEIGHT / 2, // Alineado con el hueco superior
        .width = ANSWER_SPACE_WIDTH,
        .height = ANSWER_SPACE_HEIGHT};
}

// Genera la operacion a resolver en
void GenerarOperacion(Operacion *operacion, int anchoPantalla, int altoPantalla)
{
    operacion->numero1 = GetRandomValue(1, 20);
    operacion->numero2 = GetRandomValue(1, 20);

    // Cambiar la lógica para incluir multiplicación y división
    int tipoOperacion = GetRandomValue(0, 3); // 0: suma, 1: resta, 2: multiplicación, 3: división
    switch (tipoOperacion)
    {
    case 0: // Suma
        operacion->operador = '+';
        operacion->respuestaCorrecta = operacion->numero1 + operacion->numero2;
        break;
    case 1: // Resta
        operacion->operador = '-';
        operacion->respuestaCorrecta = operacion->numero1 - operacion->numero2;
        break;
    case 2: // Multiplicación
        operacion->operador = '*';
        operacion->respuestaCorrecta = operacion->numero1 * operacion->numero2;
        break;
    case 3: // División
        operacion->operador = '/';
        // Asegúrate de que no se divida por cero
        if (operacion->numero2 == 0)
            operacion->numero2 = 1; // Evitar división por cero
        operacion->respuestaCorrecta = (float)operacion->numero1 / (float)operacion->numero2;
        break;
    }

    // Generar una respuesta incorrecta
    operacion->respuestaIncorrecta = operacion->respuestaCorrecta + (float)GetRandomValue(1, 30);

    // Posiciones iniciales
    if (intercambiarRespuestas)
    {
        operacion->espacioRespuestaCorrecta = (Rectangle){
            (float)anchoPantalla,
            (float)(altoPantalla / 2 + ANSWER_SPACE_HEIGHT),
            (float)ANSWER_SPACE_WIDTH,
            (float)ANSWER_SPACE_HEIGHT};
        operacion->espacioRespuestaIncorrecta = (Rectangle){
            (float)anchoPantalla,
            (float)(altoPantalla / 2 - ANSWER_SPACE_HEIGHT),
            (float)ANSWER_SPACE_WIDTH,
            (float)ANSWER_SPACE_HEIGHT};
    }
    else
    {
        operacion->espacioRespuestaCorrecta = (Rectangle){
            (float)anchoPantalla,
            (float)(altoPantalla / 2 - ANSWER_SPACE_HEIGHT),
            (float)ANSWER_SPACE_WIDTH,
            (float)ANSWER_SPACE_HEIGHT};
        operacion->espacioRespuestaIncorrecta = (Rectangle){
            (float)anchoPantalla,
            (float)(altoPantalla / 2 + ANSWER_SPACE_HEIGHT),
            (float)ANSWER_SPACE_WIDTH,
            (float)ANSWER_SPACE_HEIGHT};
    }

    intercambiarRespuestas = !intercambiarRespuestas; // Cambiar para la próxima vez
}

// Dibuja los datos de la operacion que ya ha sido generada
void DibujarOperacion(Operacion operacion, Jugador jugador, bool intercambiarRespuestas)
{
    // Render operation text, posicionando el resultado más a la izquierda
    DrawText(TextFormat("%d %c %d =", operacion.numero1, operacion.operador, operacion.numero2),
             operacion.espacioRespuestaCorrecta.x - 300, operacion.espacioRespuestaCorrecta.y + 230, 35, WHITE); // Ajusta el valor -200 según sea necesario

    // Intercambio de posiciones basado en la variable intercambiarRespuestas
    if (intercambiarRespuestas)
    {
        // Incorrect answer in the space for correct answer
        DrawText(TextFormat("%.2f", operacion.respuestaIncorrecta),
                 operacion.espacioRespuestaCorrecta.x + (operacion.espacioRespuestaCorrecta.width - MeasureText(TextFormat("%.2f", operacion.respuestaIncorrecta), 30)) / 2,
                 operacion.espacioRespuestaCorrecta.y + (operacion.espacioRespuestaCorrecta.height - 80) / 2, 35, WHITE);

        // Correct answer in the space for incorrect answer
        DrawText(TextFormat("%.2f", operacion.respuestaCorrecta),
                 operacion.espacioRespuestaIncorrecta.x + (operacion.espacioRespuestaIncorrecta.width - MeasureText(TextFormat("%.2f", operacion.respuestaCorrecta), 30)) / 2,
                 operacion.espacioRespuestaIncorrecta.y + (operacion.espacioRespuestaIncorrecta.height - 15) / 2, 35, WHITE);
    }
    else
    {
        // Correct answer
        DrawText(TextFormat("%.2f", operacion.respuestaCorrecta),
                 operacion.espacioRespuestaCorrecta.x + (operacion.espacioRespuestaCorrecta.width - MeasureText(TextFormat("%.2f", operacion.respuestaCorrecta), 30)) / 2,
                 operacion.espacioRespuestaCorrecta.y + (operacion.espacioRespuestaCorrecta.height - 80) / 2, 35, WHITE);

        // Incorrect answer
        DrawText(TextFormat("%.2f", operacion.respuestaIncorrecta),
                 operacion.espacioRespuestaIncorrecta.x + (operacion.espacioRespuestaIncorrecta.width - MeasureText(TextFormat("%.2f", operacion.respuestaIncorrecta), 30)) / 2,
                 operacion.espacioRespuestaIncorrecta.y + (operacion.espacioRespuestaIncorrecta.height - 15) / 2, 35, WHITE);
    }

    // Debug: Draw hitboxes
    DrawRectangleLines(operacion.espacioRespuestaCorrecta.x, operacion.espacioRespuestaCorrecta.y,
                       operacion.espacioRespuestaCorrecta.width, operacion.espacioRespuestaCorrecta.height, GREEN);
    DrawRectangleLines(operacion.espacioRespuestaIncorrecta.x, operacion.espacioRespuestaIncorrecta.y,
                       operacion.espacioRespuestaIncorrecta.width, operacion.espacioRespuestaIncorrecta.height, RED);
}

void DibujarPantallaInicio(int puntuacion)
{
    DrawText(TextFormat("LA PUNTUACION MAS ALTA ES: %d", puntuacion), GetScreenWidth() / 2 - 75 - MeasureText(TextFormat("LA PUNTUACION MAS ALTA ES: %d", puntuacion), 20) / 2, GetScreenHeight() / 2 + 210, 30, BLACK);
}

void DibujarPantallaGameOver(int puntuacion)
{
    const char *reiniciar = "Presiona [R] para reiniciar";
    DrawTextureV(gover, (Vector2){200, 200}, WHITE);
    DrawText(TextFormat("PUNTUACION: %d", puntuacion), GetScreenWidth() / 2 - MeasureText(TextFormat("PUNTUACION: %d", puntuacion), 20) + 50, GetScreenHeight() / 2 + 150, 30, BLACK);
    DrawText(reiniciar, GetScreenWidth() / 2 - MeasureText(reiniciar, 20) + 10, GetScreenHeight() / 2 + 215, 40, BLACK);
}

void GuardarPuntuacion(int puntuacion)
{
    FILE *archivoPuntuacion;
    archivoPuntuacion = fopen("puntuacion.bin", "wb");
    if (archivoPuntuacion != NULL)
    {
        fwrite(&puntuacion, sizeof(int), 1, archivoPuntuacion);
        fclose(archivoPuntuacion);
    }
}

int CargarPuntuacion()
{
    FILE *archivoPuntuacion;
    int puntuacion = 0;
    archivoPuntuacion = fopen("puntuacion.bin", "rb");
    if (archivoPuntuacion != NULL)
    {
        fread(&puntuacion, sizeof(int), 1, archivoPuntuacion);
        fclose(archivoPuntuacion);
    }
    return puntuacion;
}
