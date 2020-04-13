; Inno script
[Setup]
AppId={{6A8627FB-DC01-4E7A-BA0E-C949729C4CE9}
AppName=DeepTags
AppVersion=0.5
AppVerName=DeepTags 0.5
AppPublisher=SZinedine
AppPublisherURL=http://www.github.com/SZinedine/DeepTags
AppSupportURL=http://www.github.com/SZinedine/DeepTags
AppUpdatesURL=http://www.github.com/SZinedine/DeepTags
DefaultDirName={pf}\DeepTags
DisableProgramGroupPage=yes
LicenseFile=C:\projects\DeepTags\packaging\DeepTags\LICENSE
OutputDir=C:\projects\DeepTags\packaging
OutputBaseFilename=DeepTags_setup-32bit
SetupIconFile=C:\projects\DeepTags\packaging\DeepTags\DeepTags.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "C:\projects\DeepTags\packaging\DeepTags\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs


[Icons]
Name: "{commonprograms}\DeepTags"; Filename: "{app}\deeptags.exe"
Name: "{commondesktop}\DeepTags"; Filename: "{app}\deeptags.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\DeepTags"; Filename: "{app}\deeptags.exe"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\deeptags.exe"; Description: "{cm:LaunchProgram,DeepTags}"; Flags: nowait postinstall skipifsilent

