#ifndef _BOMBER_H_
#define _BOMBER_H_
#define MAKE_VER(o1,o2,o3,o4) ((DWORD)(((o1&255)<<24)|((o2&255)<<16)|((o3&255)<<8)|(o4&255)))

#ifndef DX3
int InitLobbySession(DPLCONNECTION*);
void GereLobby(void);
void SendLobbyScores(DWORD);
#endif
DWORD FileCRC(FILE*);
DWORD MemoryCRC(BYTE*,DWORD);
void ConsoleExecute(const char*);
void EnableConsole(void);
void DisableConsole(void);
void GereUpdateScreen(void);
int GetAsyncKState(BYTE);
void GereBombers(void);
void BltBlast(int,int);
void GereBlasts(void);
void NewBlast(int,int,int,BOOL);
void ExploseBomb(int,int);
void BltBomb(int,int);
void GereBombs(void);
void PoseBomb(int,int);
BOOL EstBloque(int,int);
void UpdateScreen(RECT);
void UpdateBuffer(RECT);
void BltBomber(int);
void InitBomber(int);
void ResetBomber(int);
void DeplaceBomber(int,float,float);
void ExtrapoleBomber(int,float,float);
void GetKeys(int);
void InitBlocs(int);
void SetAllBlocsSol(void);
//void BltBlocs(void);
void CleanUp(BOOL);
BOOL InitApplication(void);
BOOL InitInstance(int);
void ProcheMine(int);
int DirectDist(int,int,int,int);
void PurgeKeyboard(void);
int ChooseBonus(void);
void ClearSurface(const void*,DWORD);
DWORD TickCount(void);
DWORD fTickCount(void);
void AddPlayerToScores(int);
void AddTeamToScores(int);
void ResetAll(void);
void ResetGame(void);
void ResetMulti(void);
void ShowBack(void);
void ShowText(HDC,const char*,int,int,int,int);
void AffMessage(void);
void AddMessage(const char*,DWORD,DWORD);
void GereTextMessages(void);
void GereAffScore(void);
void MsgBoxScores(void);
void SendMail(void);
void SetDefaultOptions(struct SETTINGS*);
void SetDefaultControls(struct Controls*);
void SetDefaultGameOptions(struct GameOptions*);
void SetDefaultTextMessages(struct TextMessages*);
void InitDDraw(void);
void InitDInput(void);
void InitDSound(void);
BOOL CheckOSAndDX(void);
BOOL LoadRegistry(void);
int InitGameSession(void);
int InitDemoSession(void);
int InitLocalSession(void);
int InitClientSession(void);
int InitHostSession(void);
int InitMulticastSession(void);
int InitDedicatedSession(void);
void InitFileSystem(void);
BOOL TestColorControl(void);
void SaveSettings(const struct SETTINGS*);
void SaveConfigFile(const char*,const struct SETTINGS*);
BOOL LoadConfigFile(const char*,struct SETTINGS*);
BOOL LoadCurConfigFile(struct SETTINGS*);
BOOL CreateNewConfigFile(const char*);
int GetTypeSol(int,int);
void SendDir(int,const char*);
void SendFile(int,const char*);
void EnumAll(void);
void LoadSprites(void);
BOOL InitStartup(const char*);
void GameLoop(void);
BYTE atob(const char*);
DWORD atodw(const char*);
void ShowNow(const char*);
void SetOptionsDialog(HWND,const struct SETTINGS*);
void GetOptionsDialog(HWND,struct SETTINGS*);
DWORD GetBomberColor(BYTE);
int MainGame(void);
int	Dist(int,int,int,int);
void LoadSkin(const char*);
char* KeyName(BYTE);
void FinalShutdown(void);
void FlipToGDI(void);
int GetUpdateCount(void);
BOOL MemeTeam(int,int);
DWORD GetRealNumPlayers(void);
void UpdateAll(void);
void GereDT(BOOL);
void GereInput(void);
void GereGeneral(void);
void GereMulti(void);
void SetPause(void);
void ResetPause(void);
void ShowScores(void);
int GetFileLength(const char*);
void GereLastFirst(void);
int GetMaxNameLength(void);
void AllocScoreSurface(void);
void InitVersionInfo(void);
void GereSuddenDeath(void);
void SuddenAdd(DWORD,DWORD,BOOL);
void GereDemoMode(void);
void LoadMapFile(const char*);
void TryRestoreAll(void);
void ReInitDDraw(void);
void FinDDraw(void);
void FinDInput(void);
void FinDPlay(void);
void GetDisplayModes(void);
void RestoreSurfaces(void);
void SetDefaultModes(void);
void CreateDDObject(void);
void ReleaseDDObject(void);
int Equipe(int);
int RemainingTeams(void);
int GetNbEntities(void);
void AfficheTeams(HWND,BYTE,BYTE,BYTE);
int GetTeamPop(BYTE);
void DrawMapPreview(HDC,const char*);
void SetDefaultBlocs(void);
void SetDefaultStartPos(struct GameOptions*);
void GereChatMessages(void);
void DeleteChatChar(void);
void AddChatChar(int);
void ShowChatText(void);
void UpdateBombCount(void);
void UpdateBlastCount(void);
int TPOINTS(BYTE);
int TFRAGS(BYTE);
int TAK(BYTE);
int TPARTIES(BYTE);
void GereStats(void);

#if	0 //Tmporisation init/clean ?
#define Temporize() Sleep(15)
#else
#define Temporize()
#endif

BOOL CALLBACK DlgProcWaitSession(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK DlgProcWaitSPlaying(HWND,UINT,WPARAM,LPARAM);

const int BOT_REVTIME=2000;
const BYTE NUM_TEAMS=2;

const BYTE VOID_TEAM=0;
const BYTE RED_TEAM=1;
const BYTE BLUE_TEAM=2;

const int B_GAUCHEX=320;
const int B_DROITEX=0;
const int B_HAUTX=0;
const int B_BASX=160;

const int B_GAUCHEY=224;
const int B_DROITEY=224;
const int B_HAUTY=256;
const int B_BASY=224;

const int C_LIBRE=4;
const int MURX=0;
const int MURY=0;

const int SOLX=0;
const int SOLY=64;

const int CAISSEX=0;
const int CAISSEY=128;

const int BONUSX=0;
const int BONUSY=192;

const int	V=6;
const float	V1=48.0;

const int SLEEP_TIME=50;

const int	HIT_DIST=24;
const int	SDDEATH_PITCH=25;
const int	SDDEATH_SWITCH=500;

const int X_PER_SCORE=96;
const int Y_PER_SCORE=64;

const int CTRL_KB=1;
const int CTRL_NET=2;
const int MAX_DIST=10000;

const int CST_SND=5;

const int BN_BLAST=3;
const int BN_SIMUL=4;
const int BN_RAD=5;
const int BN_CRT=6;
const int BN_STD=7;
const int BN_LNG=8;
const int BN_RIG=9;
const int BN_RET=10;
const int BN_NUCL=11;
const int BN_ALLEXPL=12;
const int BN_TELEPORT=14;
const int BN_MINE=15;
const int BN_REVERSE=16;

const int STATS_TOUT=1500;
const int NSTBIAS_TOUT=500;
const int LOBBY_TOUT=0;
const int NPCHECK_TOUT=5000;
const int DEMO_TOUT=60000;
const int CTRL_TOUT=250;
const int MINE_TOUT=4000;
const int MV_TOUT=80;
const int BLAST_START_TOUT=90;
const int BLAST_TOUT=17;
const int MORT_TOUT=3000;
const int FRAME_TIME=500;

const int BB_TEAMCOLORX=32;
const int BB_TEAMCOLORY=0;

const int BB_BLASTX=0;
const int BB_BLASTY=96;

const int BB_BOMBX=0;
const int BB_BOMBY=32;

const int BB_STD=1;
const int BB_STDSPR=5;

const int BB_LNG=2;
const int BB_LNGSPR=0;

const int BB_CRT=4;
const int BB_CRTSPR=9;

const int BB_RET=8;
const int BB_RETSPR=12;

const int BB_MINE=16;
const int BB_MINESPR=13;

const int BB_RIG=32;

const int B_REALX=24;
const int B_REALY=28;

const int B_DISABLED=0;
const int B_PLAYING=1;

const int B_GAUCHE=1;
const int B_DROITE=2;
const int B_HAUT=3;
const int B_BAS=4;
const int B_MORT=5;

const int RIEN=0;
const int MUR=1;
const int CAISSE=2;
const int SOL=3;

const int Y_SCORE_BIAS=4;

const int BLQ_BOMB=1;
const int BLQ_BLAST=2;

const int MAX_AFFMSG=3;

//const int KEY_REPEAT_DELAY=200;
const DWORD BOMBER_COLOR[]={RGB(200,20,20),
							RGB(20,200,20),
							RGB(20,20,200),
							RGB(20,200,200)};

const DWORD BOMBER_DEFCOLOR=RGB(200,200,200);
#endif
