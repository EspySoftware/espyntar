#pragma once
#include <string>

using std::string;

/**
 * Configuración global del juego Espyntar
 */
class GameConfig
{
private:
    static GameConfig *instance;
    string selectedTheme;
    bool useWordService;

    GameConfig()
    {
        selectedTheme = "General";
        useWordService = true;
    }

public:
    // Singleton pattern
    static GameConfig *GetInstance()
    {
        if (instance == nullptr)
        {
            instance = new GameConfig();
        }
        return instance;
    }

    // Gestión de temática
    void SetSelectedTheme(const string &theme) { selectedTheme = theme; }
    string GetSelectedTheme() const { return selectedTheme; }

    // Gestión del servicio de palabras
    void SetUseWordService(bool use) { useWordService = use; }
    bool GetUseWordService() const { return useWordService; }

    // Destructor
    ~GameConfig()
    {
        if (instance != nullptr)
        {
            delete instance;
            instance = nullptr;
        }
    }
};

// Declaración de la instancia estática
GameConfig *GameConfig::instance = nullptr;