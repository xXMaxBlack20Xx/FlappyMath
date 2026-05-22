// Flappy Math

#include "raylib.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace
{
constexpr int kInitialScreenWidth = 1525;
constexpr int kInitialScreenHeight = 830;
constexpr float kGravity = 0.45f;
constexpr float kJumpVelocity = -8.5f;
constexpr float kPipeSpeed = 5.0f;
constexpr float kPipeWidth = 100.0f;
constexpr float kPipeGap = 500.0f;
constexpr float kMiddlePipeHeight = 150.0f;
constexpr float kAnswerGateHeight = 135.0f;
constexpr float kGroundPadding = 20.0f;
constexpr char kHighScoreFile[] = "puntuacion.bin";
constexpr char kSettingsFile[] = "settings.bin";
constexpr int kAudioSettingCount = 2;

enum class Scene
{
    Start,
    Settings,
    Playing,
    GameOver
};

struct Player
{
    Vector2 position;
    float radius;
    float velocityY;
};

struct AnswerGate
{
    Rectangle bounds;
    int value;
    bool correct;
};

struct MathProblem
{
    int left;
    int right;
    char operatorSymbol;
    int correctAnswer;
    AnswerGate gates[2];
};

struct Obstacle
{
    Rectangle topPipe;
    Rectangle bottomPipe;
    Rectangle middlePipe;
    MathProblem problem;
    bool scored;
};

struct Assets
{
    Texture2D background;
    Texture2D title;
    Texture2D playButton;
    Texture2D creators;
    Texture2D gameOverTitle;
    Texture2D bird[3];
    Texture2D pipeBottom;
    Texture2D pipeTop;
    Texture2D middlePipe;
    Sound wing;
    Sound hit;
    Sound die;
    Sound point;
    Music music;
};

struct GameState
{
    Scene scene;
    Player player;
    Obstacle obstacle;
    int score;
    int highScore;
    float musicVolume;
    float sfxVolume;
    int selectedSetting;
};

int LoadHighScore();
void SaveHighScore(int score);
Assets LoadAssets();
void UnloadAssets(Assets *assets);
void ResetPlayer(Player *player);
void StartRun(GameState *game, Assets *assets);
void EndRun(GameState *game, Assets *assets);
void ResetObstacle(Obstacle *obstacle, int screenWidth, int screenHeight, int pipeTextureHeight);
void GenerateProblem(MathProblem *problem);
int GenerateWrongAnswer(int correctAnswer);
void MoveObstacle(Obstacle *obstacle);
bool PlayerHitsRectangle(const Player *player, Rectangle rectangle);
AnswerGate *GetTouchedGate(Obstacle *obstacle, const Player *player);
void UpdatePlaying(GameState *game, Assets *assets);
void DrawBackground(Texture2D background);
void DrawStartScreen(const GameState *game, const Assets *assets);
void DrawPlayingScreen(const GameState *game, const Assets *assets);
void DrawGameOverScreen(const GameState *game, const Assets *assets);
void DrawTextureCentered(Texture2D texture, Vector2 center, Color tint);
void DrawTextCentered(const char *text, int y, int fontSize, Color color);
void DrawTextWithShadow(const char *text, int x, int y, int fontSize, Color color);
void DrawPipes(const Obstacle *obstacle, const Assets *assets);
void DrawProblem(const Obstacle *obstacle);
void DrawPlayer(const Player *player, const Assets *assets);
bool WantsPrimaryAction();
void LoadSettings(GameState *game);
void SaveSettings(const GameState *game);
void ApplyVolumeSettings(const GameState *game, Assets *assets);
void DrawSettingsScreen(const GameState *game, const Assets *assets);
} // namespace

int main(void)
{
    srand((unsigned int)time(NULL));

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
    InitWindow(kInitialScreenWidth, kInitialScreenHeight, "Flappy Math");
    InitAudioDevice();
    SetTargetFPS(60);

    Assets assets = LoadAssets();
    GameState game = {};
    game.scene = Scene::Start;
    game.highScore = LoadHighScore();
    game.musicVolume = 0.5f;
    game.sfxVolume = 0.5f;
    game.selectedSetting = 0;
    LoadSettings(&game);
    ResetPlayer(&game.player);
    ResetObstacle(&game.obstacle, GetScreenWidth(), GetScreenHeight(), assets.pipeTop.height);
    ApplyVolumeSettings(&game, &assets);

    while (!WindowShouldClose())
    {
        if (game.scene == Scene::Start && WantsPrimaryAction())
        {
            StartRun(&game, &assets);
        }
        else if (game.scene == Scene::Start && IsKeyPressed(KEY_S))
        {
            game.scene = Scene::Settings;
            PlayMusicStream(assets.music);
        }
        else if (game.scene == Scene::Settings)
        {
            if (IsKeyPressed(KEY_ESCAPE) || WantsPrimaryAction())
            {
                StopMusicStream(assets.music);
                SaveSettings(&game);
                game.scene = Scene::Start;
            }

            if (IsKeyPressed(KEY_DOWN))
            {
                game.selectedSetting = (game.selectedSetting + 1) % kAudioSettingCount;
            }
            else if (IsKeyPressed(KEY_UP))
            {
                game.selectedSetting = (game.selectedSetting - 1 + kAudioSettingCount) % kAudioSettingCount;
            }

            float *volume = game.selectedSetting == 0 ? &game.musicVolume : &game.sfxVolume;
            if (IsKeyPressed(KEY_RIGHT))
            {
                *volume = *volume + 0.1f > 1.0f ? 1.0f : *volume + 0.1f;
                ApplyVolumeSettings(&game, &assets);
            }
            else if (IsKeyPressed(KEY_LEFT))
            {
                *volume = *volume - 0.1f < 0.0f ? 0.0f : *volume - 0.1f;
                ApplyVolumeSettings(&game, &assets);
            }

            UpdateMusicStream(assets.music);
        }
        else if (game.scene == Scene::GameOver && IsKeyPressed(KEY_R))
        {
            StartRun(&game, &assets);
        }

        if (game.scene == Scene::Playing)
        {
            UpdatePlaying(&game, &assets);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (game.scene)
        {
        case Scene::Start:
            DrawStartScreen(&game, &assets);
            break;
        case Scene::Settings:
            DrawSettingsScreen(&game, &assets);
            break;
        case Scene::Playing:
            DrawPlayingScreen(&game, &assets);
            break;
        case Scene::GameOver:
            DrawGameOverScreen(&game, &assets);
            break;
        }

        EndDrawing();
    }

    UnloadAssets(&assets);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

namespace
{
int LoadHighScore()
{
    int score = 0;
    FILE *file = fopen(kHighScoreFile, "rb");
    if (file != NULL)
    {
        size_t readCount = fread(&score, sizeof(score), 1, file);
        fclose(file);
        if (readCount != 1 || score < 0)
        {
            score = 0;
        }
    }
    return score;
}

void SaveHighScore(int score)
{
    FILE *file = fopen(kHighScoreFile, "wb");
    if (file != NULL)
    {
        fwrite(&score, sizeof(score), 1, file);
        fclose(file);
    }
}

void LoadSettings(GameState *game)
{
    FILE *file = fopen(kSettingsFile, "rb");
    if (file != NULL)
    {
        float musicVolume;
        float sfxVolume;
        size_t readCount = fread(&musicVolume, sizeof(float), 1, file);
        readCount += fread(&sfxVolume, sizeof(float), 1, file);
        fclose(file);
        if (readCount == 2)
        {
            game->musicVolume = musicVolume;
            game->sfxVolume = sfxVolume;
        }
    }
}

void SaveSettings(const GameState *game)
{
    FILE *file = fopen(kSettingsFile, "wb");
    if (file != NULL)
    {
        fwrite(&game->musicVolume, sizeof(float), 1, file);
        fwrite(&game->sfxVolume, sizeof(float), 1, file);
        fclose(file);
    }
}

void ApplyVolumeSettings(const GameState *game, Assets *assets)
{
    SetMusicVolume(assets->music, game->musicVolume);
    SetSoundVolume(assets->wing, game->sfxVolume);
    SetSoundVolume(assets->hit, game->sfxVolume);
    SetSoundVolume(assets->die, game->sfxVolume);
    SetSoundVolume(assets->point, game->sfxVolume);
}

Assets LoadAssets()
{
    Assets assets = {};
    assets.background = LoadTexture("assets/sprites/fondo.png");
    assets.title = LoadTexture("assets/sprites/Titulo.png");
    assets.playButton = LoadTexture("assets/sprites/activar.png");
    assets.creators = LoadTexture("assets/sprites/creadores.png");
    assets.gameOverTitle = LoadTexture("assets/sprites/FINJUEGO.png");
    assets.bird[0] = LoadTexture("assets/sprites/redbird-upflap.png");
    assets.bird[1] = LoadTexture("assets/sprites/redbird-midflap.png");
    assets.bird[2] = LoadTexture("assets/sprites/redbird-downflap.png");
    assets.pipeBottom = LoadTexture("assets/sprites/pipe-green.png");
    assets.pipeTop = LoadTexture("assets/sprites/pipe-green-upside.png");
    assets.middlePipe = LoadTexture("assets/sprites/half.pipe.png");

    assets.wing = LoadSound("assets/audio/wing.wav");
    assets.hit = LoadSound("assets/audio/hit.wav");
    assets.die = LoadSound("assets/audio/die.wav");
    assets.point = LoadSound("assets/audio/point.wav");
    assets.music = LoadMusicStream("assets/audio/musicita.mp3");

    return assets;
}

void UnloadAssets(Assets *assets)
{
    UnloadTexture(assets->background);
    UnloadTexture(assets->title);
    UnloadTexture(assets->playButton);
    UnloadTexture(assets->creators);
    UnloadTexture(assets->gameOverTitle);
    for (int i = 0; i < 3; ++i)
    {
        UnloadTexture(assets->bird[i]);
    }
    UnloadTexture(assets->pipeBottom);
    UnloadTexture(assets->pipeTop);
    UnloadTexture(assets->middlePipe);

    UnloadSound(assets->wing);
    UnloadSound(assets->hit);
    UnloadSound(assets->die);
    UnloadSound(assets->point);
    UnloadMusicStream(assets->music);
}

void ResetPlayer(Player *player)
{
    player->position = (Vector2){GetScreenWidth() * 0.35f, GetScreenHeight() * 0.5f};
    player->radius = 18.0f;
    player->velocityY = 0.0f;
}

void StartRun(GameState *game, Assets *assets)
{
    game->scene = Scene::Playing;
    game->score = 0;
    ResetPlayer(&game->player);
    ResetObstacle(&game->obstacle, GetScreenWidth(), GetScreenHeight(), assets->pipeTop.height);
    ApplyVolumeSettings(game, assets);
    StopMusicStream(assets->music);
    PlayMusicStream(assets->music);
}

void EndRun(GameState *game, Assets *assets)
{
    if (game->scene != Scene::Playing)
    {
        return;
    }

    game->scene = Scene::GameOver;
    StopMusicStream(assets->music);
    PlaySound(assets->hit);
    PlaySound(assets->die);

    if (game->score > game->highScore)
    {
        game->highScore = game->score;
        SaveHighScore(game->highScore);
    }
}

void ResetObstacle(Obstacle *obstacle, int screenWidth, int screenHeight, int pipeTextureHeight)
{
    float minCenterY = kPipeGap * 0.5f + kGroundPadding;
    float maxCenterY = screenHeight - kPipeGap * 0.5f - kGroundPadding;
    if (maxCenterY < minCenterY)
    {
        minCenterY = screenHeight * 0.5f;
        maxCenterY = screenHeight * 0.5f;
    }

    const float centerY = (float)GetRandomValue((int)minCenterY, (int)maxCenterY);
    const float pipeHeight = pipeTextureHeight > 0 ? (float)pipeTextureHeight : (float)screenHeight;
    const float pipeX = (float)screenWidth;
    const float topPipeBottom = centerY - kPipeGap * 0.5f;
    const float bottomPipeTop = centerY + kPipeGap * 0.5f;
    const float middleTop = centerY - kMiddlePipeHeight * 0.5f;
    const float middleBottom = centerY + kMiddlePipeHeight * 0.5f;
    const float laneHeight = (kPipeGap - kMiddlePipeHeight) * 0.5f;
    const float gateOffsetY = (laneHeight - kAnswerGateHeight) * 0.5f;

    obstacle->topPipe = (Rectangle){pipeX, topPipeBottom - pipeHeight, kPipeWidth, pipeHeight};
    obstacle->bottomPipe = (Rectangle){pipeX, bottomPipeTop, kPipeWidth, pipeHeight};
    obstacle->middlePipe = (Rectangle){pipeX, middleTop, kPipeWidth, kMiddlePipeHeight};
    obstacle->scored = false;

    GenerateProblem(&obstacle->problem);

    obstacle->problem.gates[0].bounds = (Rectangle){
        pipeX,
        topPipeBottom + gateOffsetY,
        kPipeWidth,
        kAnswerGateHeight};
    obstacle->problem.gates[1].bounds = (Rectangle){
        pipeX,
        middleBottom + gateOffsetY,
        kPipeWidth,
        kAnswerGateHeight};
}

void GenerateProblem(MathProblem *problem)
{
    const int type = GetRandomValue(0, 3);

    switch (type)
    {
    case 0:
        problem->left = GetRandomValue(1, 20);
        problem->right = GetRandomValue(1, 20);
        problem->operatorSymbol = '+';
        problem->correctAnswer = problem->left + problem->right;
        break;
    case 1:
        problem->left = GetRandomValue(5, 30);
        problem->right = GetRandomValue(1, problem->left);
        problem->operatorSymbol = '-';
        problem->correctAnswer = problem->left - problem->right;
        break;
    case 2:
        problem->left = GetRandomValue(1, 12);
        problem->right = GetRandomValue(1, 12);
        problem->operatorSymbol = 'x';
        problem->correctAnswer = problem->left * problem->right;
        break;
    default:
        problem->right = GetRandomValue(2, 12);
        problem->correctAnswer = GetRandomValue(1, 12);
        problem->left = problem->right * problem->correctAnswer;
        problem->operatorSymbol = '/';
        break;
    }

    const int correctGate = GetRandomValue(0, 1);
    const int wrongAnswer = GenerateWrongAnswer(problem->correctAnswer);

    for (int i = 0; i < 2; ++i)
    {
        problem->gates[i].correct = i == correctGate;
        problem->gates[i].value = problem->gates[i].correct ? problem->correctAnswer : wrongAnswer;
    }
}

int GenerateWrongAnswer(int correctAnswer)
{
    int wrongAnswer = correctAnswer;
    while (wrongAnswer == correctAnswer)
    {
        int delta = GetRandomValue(1, 9);
        if (GetRandomValue(0, 1) == 0)
        {
            delta = -delta;
        }
        wrongAnswer = correctAnswer + delta;
    }
    return wrongAnswer;
}

void MoveObstacle(Obstacle *obstacle)
{
    obstacle->topPipe.x -= kPipeSpeed;
    obstacle->bottomPipe.x -= kPipeSpeed;
    obstacle->middlePipe.x -= kPipeSpeed;
    for (int i = 0; i < 2; ++i)
    {
        obstacle->problem.gates[i].bounds.x -= kPipeSpeed;
    }
}

bool PlayerHitsRectangle(const Player *player, Rectangle rectangle)
{
    return CheckCollisionCircleRec(player->position, player->radius, rectangle);
}

AnswerGate *GetTouchedGate(Obstacle *obstacle, const Player *player)
{
    for (int i = 0; i < 2; ++i)
    {
        if (PlayerHitsRectangle(player, obstacle->problem.gates[i].bounds))
        {
            return &obstacle->problem.gates[i];
        }
    }
    return NULL;
}

void UpdatePlaying(GameState *game, Assets *assets)
{
    UpdateMusicStream(assets->music);

    if (WantsPrimaryAction())
    {
        game->player.velocityY = kJumpVelocity;
        PlaySound(assets->wing);
    }

    game->player.velocityY += kGravity;
    game->player.position.y += game->player.velocityY;

    MoveObstacle(&game->obstacle);

    if (game->obstacle.topPipe.x + game->obstacle.topPipe.width < 0.0f)
    {
        ResetObstacle(&game->obstacle, GetScreenWidth(), GetScreenHeight(), assets->pipeTop.height);
    }

    if (PlayerHitsRectangle(&game->player, game->obstacle.topPipe) ||
        PlayerHitsRectangle(&game->player, game->obstacle.bottomPipe) ||
        PlayerHitsRectangle(&game->player, game->obstacle.middlePipe))
    {
        EndRun(game, assets);
        return;
    }

    AnswerGate *touchedGate = GetTouchedGate(&game->obstacle, &game->player);
    if (touchedGate != NULL)
    {
        if (touchedGate->correct)
        {
            if (!game->obstacle.scored)
            {
                game->score++;
                game->obstacle.scored = true;
                PlaySound(assets->point);
            }
        }
        else
        {
            EndRun(game, assets);
            return;
        }
    }

    if (game->player.position.y + game->player.radius > GetScreenHeight() ||
        game->player.position.y - game->player.radius < 0)
    {
        EndRun(game, assets);
    }
}

void DrawBackground(Texture2D background)
{
    DrawTexturePro(
        background,
        (Rectangle){0.0f, 0.0f, (float)background.width, (float)background.height},
        (Rectangle){0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE);
}

void DrawStartScreen(const GameState *game, const Assets *assets)
{
    DrawBackground(assets->background);
    DrawTextureCentered(assets->title, (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.34f}, WHITE);
    DrawTextureCentered(assets->playButton, (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.68f}, WHITE);
    DrawTextureV(assets->creators, (Vector2){15.0f, (float)GetScreenHeight() - assets->creators.height - 15.0f}, WHITE);
    DrawTextCentered(TextFormat("LA PUNTUACION MAS ALTA ES: %d", game->highScore), (int)(GetScreenHeight() * 0.79f), 30, BLACK);
    DrawTextCentered("Presiona [S] para configuracion de audio", (int)(GetScreenHeight() * 0.89f), 24, GRAY);
}

void DrawSettingsScreen(const GameState *game, const Assets *assets)
{
    DrawBackground(assets->background);

    DrawTextCentered("CONFIGURACION DE AUDIO", (int)(GetScreenHeight() * 0.12f), 50, WHITE);

    const char *labels[kAudioSettingCount] = {"MUSICA", "EFECTOS"};
    const float volumes[kAudioSettingCount] = {game->musicVolume, game->sfxVolume};
    const int labelX = (int)(GetScreenWidth() * 0.25f);
    const int barY[kAudioSettingCount] = {(int)(GetScreenHeight() * 0.38f), (int)(GetScreenHeight() * 0.52f)};
    const int barWidth = GetScreenWidth() / 2;
    const int barHeight = 30;

    for (int i = 0; i < kAudioSettingCount; ++i)
    {
        const bool selected = i == game->selectedSetting;
        const Color labelColor = selected ? YELLOW : WHITE;
        const char *prefix = selected ? "-> " : "   ";

        DrawText(TextFormat("%s%s", prefix, labels[i]), labelX, barY[i] - 25, 36, labelColor);

        DrawRectangle(labelX, barY[i], barWidth, barHeight, Fade(BLACK, 0.5f));

        const int fillWidth = (int)(barWidth * volumes[i]);
        DrawRectangle(labelX, barY[i], fillWidth, barHeight, BLUE);

        DrawRectangleLines(labelX, barY[i], barWidth, barHeight, Fade(WHITE, 0.7f));

        const char *percentText = TextFormat("%d%%", (int)(volumes[i] * 100.0f));
        DrawText(percentText, labelX + barWidth + 15, barY[i], 30, WHITE);
    }

    DrawTextCentered("[FLECHA ARRIBA/ABAJO] Navegar    [FLECHA IZQ/DER] Ajustar    [ESC] Volver",
                     (int)(GetScreenHeight() * 0.78f), 28, Fade(WHITE, 0.7f));
}

void DrawPlayingScreen(const GameState *game, const Assets *assets)
{
    DrawBackground(assets->background);
    DrawPipes(&game->obstacle, assets);
    DrawProblem(&game->obstacle);
    DrawPlayer(&game->player, assets);
    DrawTextCentered(TextFormat("PUNTUACION: %d", game->score), 10, 40, BLACK);
}

void DrawGameOverScreen(const GameState *game, const Assets *assets)
{
    DrawBackground(assets->background);
    DrawTextureCentered(assets->gameOverTitle, (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.35f}, WHITE);
    DrawTextCentered(TextFormat("PUNTUACION: %d", game->score), (int)(GetScreenHeight() * 0.62f), 34, BLACK);
    DrawTextCentered(TextFormat("MEJOR PUNTUACION: %d", game->highScore), (int)(GetScreenHeight() * 0.69f), 30, BLACK);
    DrawTextCentered("Presiona [R] para reiniciar", (int)(GetScreenHeight() * 0.77f), 36, BLACK);
}

void DrawTextureCentered(Texture2D texture, Vector2 center, Color tint)
{
    DrawTextureV(
        texture,
        (Vector2){center.x - texture.width * 0.5f, center.y - texture.height * 0.5f},
        tint);
}

void DrawTextCentered(const char *text, int y, int fontSize, Color color)
{
    const int x = GetScreenWidth() / 2 - MeasureText(text, fontSize) / 2;
    DrawTextWithShadow(text, x, y, fontSize, color);
}

void DrawTextWithShadow(const char *text, int x, int y, int fontSize, Color color)
{
    DrawText(text, x + 2, y + 2, fontSize, Fade(WHITE, 0.7f));
    DrawText(text, x, y, fontSize, color);
}

void DrawPipes(const Obstacle *obstacle, const Assets *assets)
{
    DrawTexturePro(
        assets->pipeTop,
        (Rectangle){0.0f, 0.0f, (float)assets->pipeTop.width, (float)assets->pipeTop.height},
        obstacle->topPipe,
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE);

    DrawTexturePro(
        assets->pipeBottom,
        (Rectangle){0.0f, 0.0f, (float)assets->pipeBottom.width, (float)assets->pipeBottom.height},
        obstacle->bottomPipe,
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE);

    DrawTexturePro(
        assets->middlePipe,
        (Rectangle){0.0f, 0.0f, (float)assets->middlePipe.width, (float)assets->middlePipe.height},
        obstacle->middlePipe,
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE);
}

void DrawProblem(const Obstacle *obstacle)
{
    const MathProblem *problem = &obstacle->problem;
    const char *problemText = TextFormat("%d %c %d =", problem->left, problem->operatorSymbol, problem->right);
    DrawTextWithShadow(problemText, (int)(obstacle->topPipe.x - 245.0f), (int)(GetScreenHeight() * 0.48f), 36, WHITE);

    for (int i = 0; i < 2; ++i)
    {
        const AnswerGate *gate = &problem->gates[i];
        const char *answerText = TextFormat("%d", gate->value);
        DrawRectangleRec(gate->bounds, Fade(BLACK, 0.35f));
        DrawRectangleLinesEx(gate->bounds, 3.0f, Fade(WHITE, 0.85f));
        DrawTextWithShadow(
            answerText,
            (int)(gate->bounds.x + gate->bounds.width * 0.5f - MeasureText(answerText, 34) * 0.5f),
            (int)(gate->bounds.y + gate->bounds.height * 0.5f - 17.0f),
            34,
            WHITE);
    }
}

void DrawPlayer(const Player *player, const Assets *assets)
{
    const int frame = ((int)(GetTime() * 12.0)) % 3;
    Texture2D bird = assets->bird[frame];
    DrawTextureV(
        bird,
        (Vector2){player->position.x - bird.width * 0.5f, player->position.y - bird.height * 0.5f},
        WHITE);
}

bool WantsPrimaryAction()
{
    return IsKeyPressed(KEY_SPACE) ||
           IsKeyPressed(KEY_ENTER) ||
           IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
} // namespace
