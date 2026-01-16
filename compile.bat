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
    call versionar.bat "%~1"
) else (
    echo ❌ Error en la compilacion
)
echo.