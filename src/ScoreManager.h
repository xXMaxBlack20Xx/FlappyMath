#pragma once

#include <string>

// Clase Singleton para gestionar carga y guardado de la puntuación más alta
class ScoreManager
{
public:
    // Carga la puntuación más alta desde un archivo (puntuacion.bin por defecto)
    static int LoadHighScore(const std::string &filename = "puntuacion.bin");

    // Guarda la nueva puntuación más alta en el archivo
    static void SaveHighScore(int score, const std::string &filename = "puntuacion.bin");

private:
    ScoreManager() = default;
};
