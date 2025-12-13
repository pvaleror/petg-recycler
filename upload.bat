@echo off
echo ========================================
echo  PET Recycler - Upload
echo ========================================
echo.

REM Leer el puerto desde platformio.ini
for /f "tokens=2 delims== " %%a in ('findstr "upload_port" platformio.ini') do set UPLOAD_PORT=%%a

REM Si no se encuentra el puerto, usar COM5 por defecto
if "%UPLOAD_PORT%"=="" set UPLOAD_PORT=COM5

echo Subiendo firmware al ESP32-C3...
echo Puerto: %UPLOAD_PORT%
echo.
pio run -e esp32c3 -t upload
echo.
if %ERRORLEVEL% EQU 0 (
    echo ✅ Upload exitoso!
) else (
    echo ❌ Error en el upload
    echo.
    echo Verifica que:
    echo - El ESP32-C3 este conectado al puerto %UPLOAD_PORT%
    echo - No haya otros programas usando el puerto
    echo - Presiona BOOT durante el upload si es necesario
)
echo.