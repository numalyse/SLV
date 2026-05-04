; --- configuration ---
#define MyAppName "Numalyse Player (alpha version)"
#define MyAppVersion "1.0-alpha"

; --- Éléments générés par Qt/windeploy ---

; -- nom du .exe dans le CMakeLists --
#define MyAppExeName "NumalysePlayer.exe"
; -- nom du dossier de sortie --
#define MyAppFolder "NumalysePlayer"
; -- nom du dossier où se situe la version distribuée --
#define MySourceDir "..\build\dist"

; --- innosetup : nom de l'installer ---
#define MyOutputName "NumalysePlayer_" + MyAppVersion + "_Installer"

[Setup]
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName}
AppPublisher=ANR Numalyse
UsePreviousAppDir=no
DefaultDirName={autopf}\{#MyAppFolder}
DefaultGroupName={#MyAppName}
OutputDir=Output
OutputBaseFilename={#MyOutputName}
Compression=lzma
SolidCompression=yes

[Files]
Source: "{#MySourceDir}\*"; DestDir: "{app}"; Flags: recursesubdirs ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Lancer {#MyAppName}"; Flags: nowait postinstall skipifsilent