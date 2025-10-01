@echo off
echo ====================================
echo   Servicio de Palabras - Espyntar
echo ====================================
echo.

:: Verificar si Python está instalado
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Python no está instalado o no está en el PATH
    echo    Instala Python desde https://www.python.org/
    pause
    exit /b 1
)

:: Verificar si pip está disponible
pip --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ pip no está disponible
    echo    Reinstala Python con pip incluido
    pause
    exit /b 1
)

echo ✅ Python encontrado
echo.

:: Instalar dependencias
echo 📦 Instalando dependencias...
pip install -r requirements.txt
if %errorlevel% neq 0 (
    echo ❌ Error instalando dependencias
    pause
    exit /b 1
)

echo ✅ Dependencias instaladas correctamente
echo.

:: Verificar archivo .env
if not exist .env (
    echo ⚠️  Archivo .env no encontrado
    echo    Copia .env.example a .env y configura tu API key de OpenAI
    copy nul .env.example >nul 2>&1
    echo # Configuración del Servicio de Palabras de Espyntar > .env.example
    echo. >> .env.example
    echo # Clave de API de OpenAI (obtén una en https://platform.openai.com/api-keys) >> .env.example
    echo OPENAI_API_KEY=tu_clave_de_api_aqui >> .env.example
    echo. >> .env.example
    echo # Configuración del servidor >> .env.example
    echo WORD_SERVICE_HOST=localhost >> .env.example
    echo WORD_SERVICE_PORT=8888 >> .env.example
    pause
    exit /b 1
)

echo ✅ Configuración encontrada
echo.

:: Iniciar servicio
echo 🚀 Iniciando servicio de palabras...
echo    Servidor ejecutándose en localhost:8888
echo    Presiona Ctrl+C para detener
echo.
python word_service.py