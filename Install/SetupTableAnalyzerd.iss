; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "TableAnalyzer"
#define MyAppVersion "1.0.0.1"#define MyAppPublisher "luo.kaikun@coriant.com"
#define MyAppExeName "TableAnalyzer.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{88F21BEF-106B-41AD-AC5C-FDECB64ED759}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputDir=F:\MyGarden\TableAnalyzer\Install
OutputBaseFilename=TableAnalyzerInstall
SetupIconFile=F:\MyGarden\TableAnalyzer\Install\Install.ico
Compression=lzma
SolidCompression=yes
Uninstallable=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "F:\MyGarden\TableAnalyzer\Install\Uninstall.ico"; Flags:solidbreak dontcopySource: "F:\MyGarden\TableAnalyzer\Install\UpdateIcon.dll"; Flags:solidbreak dontcopy
Source: "F:\MyGarden\TableAnalyzer\bin\TableAnalyzer.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "F:\MyGarden\TableAnalyzer\bin\tables\*"; DestDir: "{app}\tables"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "F:\MyGarden\TableAnalyzer\bin\readme.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "F:\MyGarden\TableAnalyzer\bin\Help.pdf"; DestDir: "{app}"; Flags: ignoreversion
Source: "F:\MyGarden\TableAnalyzer\install\uninstall.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "F:\MyGarden\TableAnalyzer\bin\Readme.txt"; DestDir: "{app}"; Flags: isreadme
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[code]

function UpdateIcon(const hWnd: Integer; const exeFileName, exeIcon, IcoFileName: String; wlangID: DWORD): Boolean;

external 'UpdateIcon@files:UpdateIcon.dll stdcall';


//替换卸载程序的图标

function UpdateUninstIcon(const IcoFileName: String): Boolean;

begin

//要替换图标的exe文件路径名称留空，则插件会自动替换掉Inno卸载程序的图标,其它参数类似！

Result:= UpdateIcon(MainForm.Handle, '', '', IcoFileName, 0);

//替换卸载图标

end;


procedure CurStepChanged(CurStep: TSetupStep);

var

sIcon: String;

begin

//注意:替换卸载程序的图标，必须是在卸载程序生成之前！

//安装图标与卸载图标的格式与大小一致，否则可能会导致卸载程序出错！

if CurStep=ssInstall then

begin

sIcon:= ExpandConstant('{tmp}\uninstall.ico'); //定义卸载图标

ExtractTemporaryFile(ExtractFileName(sIcon)); //释放卸载图标

//要替换图标的exe文件路径名称留空，则插件会自动替换掉Inno卸载程序的图标！

UpdateUninstIcon(sIcon);//替换卸载图标

end;

end;


[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\Help"; Filename: "{app}\Help.pdf"
Name: "{group}\uninstall"; Filename: "{uninstallexe}"; IconFilename: "{app}\uninstall.ico";
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

