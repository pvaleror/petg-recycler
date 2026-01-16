REM Leer el puerto desde platformio.ini
for /f "tokens=2 delims== " %%a in ('findstr "upload_port" platformio.ini') do set UPLOAD_PORT=%%a

REM Si no se encuentra el puerto, usar COM5 por defecto
if "%UPLOAD_PORT%"=="" set UPLOAD_PORT=COM5

REM Leer el baudrate desde platformio.ini
for /f "tokens=2 delims== " %%a in ('findstr "monitor_speed" platformio.ini') do set MON_SPEED=%%a

REM Si no se encuentra el puerto, usar COM5 por defecto
if "%MON_SPEED%"=="" set MON_SPEED=115200

echo Iniciando monitor
pio device monitor -p %UPLOAD_PORT% -b %MON_SPEED%