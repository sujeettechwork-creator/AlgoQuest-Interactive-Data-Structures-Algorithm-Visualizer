# Run from the AlgoQuest folder in PowerShell:  .\build.ps1
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
$src = @("main.c") + (Get-ChildItem graphics\*.c, structures\*.c).FullName
gcc -g -std=c11 -Wall -Wextra -O1 @src -o AlgoQuest.exe -lraylib -lopengl32 -lgdi32 -lwinmm
if ($LASTEXITCODE -eq 0) { Write-Host "Build OK" -ForegroundColor Green; .\AlgoQuest.exe }
