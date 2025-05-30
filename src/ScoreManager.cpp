// src/ScoreManager.cpp
#include "ScoreManager.h"
#include <fstream>

int ScoreManager::LoadHighScore(const std::string &filename)
{
    std::ifstream in{filename, std::ios::binary};
    int score = 0;
    if (in)
    {
        in.read(reinterpret_cast<char *>(&score), sizeof(score));
    }
    return score;
}

void ScoreManager::SaveHighScore(int score, const std::string &filename)
{
    std::ofstream out{filename, std::ios::binary};
    if (out)
    {
        out.write(reinterpret_cast<const char *>(&score), sizeof(score));
    }
}
