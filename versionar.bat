REM Verificar si hay parámetro
if "%~1"=="" (
    set /p COMMIT_MSG="Ingrese descripción del commit: "
) else (
    set "COMMIT_MSG=%~1"
)

echo Versionado cambios en git...
git add .
git diff --staged --quiet
if errorlevel 1 (
    echo Haciendo commit: %COMMIT_MSG%
    git commit -m "%COMMIT_MSG%"
    git push
    echo Commit y push exitosos.
) else (
    echo No hay cambios para commit.
)