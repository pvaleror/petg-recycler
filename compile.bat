@echo off
echo ========================================
echo  PET Recycler - Compilacion
echo ========================================
echo.
echo Compilando proyecto...
pio run -e esp32c3
echo.
if %ERRORLEVEL% EQU 0 (
    echo ✅ Compilacion exitosa!
    echo.
    echo Ejecutando upload automaticamente...
    call upload.bat
    echo Versionado cambios en git...
    git add .
    git commit -m "$1"
    git push
) else (
    echo ❌ Error en la compilacion
)
echo.