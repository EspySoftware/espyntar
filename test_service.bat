@echo off
echo ====================================
echo    Probar Servicio de Palabras
echo ====================================
echo.

:: Verificar si Python está instalado
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Python no está instalado o no está en el PATH
    pause
    exit /b 1
)

echo 🧪 Ejecutando pruebas del servicio...
echo    Asegúrate de que el servicio esté ejecutándose (start_word_service.bat)
echo.

python test_word_service.py

echo.
echo ✅ Pruebas completadas
pause