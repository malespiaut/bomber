//#define _WIN32_WINNT 0x400
#undef GOA
//#define GOA

#include "General.h"
//#include <mapi.h>
#include <shellapi.h>
#include <commctrl.h>
#include <ddraw.h>
#include <dinput.h>
#include <dplobby.h>

#include "Bomber.h"
#include "iBomber.h"
#include "Multi.h"
#include "Son.h"
#include "Voice.h"
#include "VoiceStack.h"
#include "DEBUG.h"
#include "ddutil.h"
#include "GetDXVer.h"
#include "Aleat.h"
#include "Skins.h"
#include "Maps.h"
#include "Configs.h"
#include "LoadFile.h"
#include "Crypte.h"
#include "IA.h"
#include "Font.h"
#include "WSDPlay.h"

#ifdef GCOMP_LZO
#include <lzo1x.h>
#elif defined(GCOMP_UCL)
#include <ucl/ucl.h>
#include "ucl/src/ucl_conf.h"
#elif defined(GCOMP_BZ)
#include <bzlib.h>
#else
#error Pas de compresseur !!
#endif
//#include <seer.h>

#define	PLAYSON(From,Pan,Num,Loop)	{SendGameMessage(From,-1,MSG_SNDFORME,Num,0,0,0);PlaySon(Pan,Num,Loop);}
#define SCPOINTS(a)	(Settings.GO.MORTPNT*int(Score[a].Frag)-Settings.GO.AKPNT*int(Score[a].AK))
#define SCSPOINTS(a)	(Settings.GO.MORTPNT*int((a).Frag)-Settings.GO.AKPNT*int((a).AK))
#define BPOINTS(a)	(Settings.GO.MORTPNT*int(Bombers[a].Frag)-Settings.GO.AKPNT*int(Bombers[a].AK))
#define CALCPOINTS(frag,ak) (Settings.GO.MORTPNT*int(frag)-Settings.GO.AKPNT*int(ak))
#define KEY_CODE(dik,vk) (Settings.USEDINPUT?(dik):(vk))

// {653B6270-031A-11d3-A9CE-A0ED51C10FE3}
static const GUID BOMBER_GUID =
 { 0x653b6270, 0x31a, 0x11d3, { 0xa9, 0xce, 0xa0, 0xed, 0x51, 0xc1, 0xf, 0xe3 } };

const BYTE dwSign[4]={'B','S','C','R'};//'B'<<24+'S'<<16+'C'<<8+'R';

const DWORD BLITFAST_WAIT=DDBLTFAST_WAIT;
const DWORD BLIT_WAIT=DDBLT_WAIT;
const DWORD FLIP_WAIT=DDFLIP_WAIT;
/*
const DWORD BLITFAST_WAIT=0;
const DWORD BLIT_WAIT=0;
const DWORD FLIP_WAIT=DDFLIP_DONOTWAIT;
*/

const DWORD dwBltFastCK=DDBLTFAST_SRCCOLORKEY;

const char* DEFAULT_FONTFACE="MS Sans Serif";
const char* DEFAULT_SCORE_FONTFACE="MS Sans Serif";
const int POINT_SIZE=10;
const int SCORE_POINT_SIZE=10;
int DEFAULT_FONT,DEFAULT_SCORE_FONT;

const int FRAME_SLEEP_TIME=5;
const DWORD SPR_SIZEX=512;
const DWORD SPR_SIZEY=512;

const char* szPAKFile="Data.pak";
const char* szSprFile="Sprites.bmp";
const char* szScoreFile="Scores.dat";
const char* szBPassword=NULL;

const char* TeamName[NUM_TEAMS+1]={LSZ_NOTEAM,LSZ_REDTEAM,LSZ_BLUETEAM};
const char szAppName[]="Bomber";
const char szTitle[]="Bomber";
char szSessionName[BMAX_NAME]="Session";

char szWorkingDirectory[BMAX_PATH];
struct SETTINGS Settings;
struct GameOptions	TmpGO,TmpGO1;
struct TextMessages	TmpTM,TmpTM1;
struct Controls		TmpC,TmpC1;
struct SndGlobals	Sons;
char tszName[256][BMAX_PATH];
char szGenericKey[64];
int LastF1,LastF2,LastF3,LastF4;
int LastF5,LastF6,LastF7,LastF8;

DWORD LastGereStats;
class WSDPlay* wsDP;

int	Depth[MAX_VIDMODES],NumModes;
HDC hDCLogo;
HBITMAP hBmpLogo;
HBITMAP hBmpOldLogo;

/*
const int	NumSysColors=9;
COLORREF	oldSysColors[NumSysColors];
int			SysID[NumSysColors]={COLOR_ACTIVECAPTION,
								COLOR_WINDOW,
								COLOR_BTNFACE,
								COLOR_ACTIVEBORDER,
								COLOR_3DDKSHADOW,
								COLOR_3DSHADOW,
								COLOR_3DLIGHT,
								COLOR_3DHILIGHT};
COLORREF	SysColors[NumSysColors]={RGB(150,150,100),
									RGB(150,150,100),
									RGB(150,150,100),
									RGB(150,150,100),
									RGB(120,120,70),
									RGB(120,120,70),
									RGB(180,180,130),
									RGB(180,180,130)};
*/

DWORD		SleepTime;

BOOL		bLockedSessionName;
BOOL		bChat;
BOOL		bChatFirst;
BOOL		bReturnPress;
char		ChatText[BMAX_PATH];
BYTE		ChatNum;

DWORD		dwPlays,dwLastNumPlayers,dwLastNetNumPlayers;
char		szCurMap[BMAX_PATH];
DWORD		dwVersion;
DWORD		dwProto;
char		Buff[BMAX_PATH];
char		Buff1[BMAX_PATH];
HINSTANCE	hInst;
HWND		hWndWin,hWndCon;
int			TCount,FCount;
float		dt;
DWORD		lGTC,TimerRef;
DWORD		StartTime,LastGTC;
DWORD		LastNPCheck;
BYTE		Keyboard[256];

BOOL		UPlck[2*MAX_PLAYERS],DOWNlck[2*MAX_PLAYERS];
BOOL		LEFTlck[2*MAX_PLAYERS],RIGHTlck[2*MAX_PLAYERS];
BOOL		F1lck[2*MAX_PLAYERS],F2lck[2*MAX_PLAYERS],F2Down[2*MAX_PLAYERS];
BOOL		F3lck[2*MAX_PLAYERS];
BOOL		bWin_NT,bQuit;
BOOL		bClrCtrl,bScoring;
BOOL		bExit;

DWORD		F1GTC[4],F3GTC[4];

int		LastXDir[MAX_PLAYERS],LastYDir[MAX_PLAYERS];

int		MaxNameLength;
int		XperScore,YperScore;
BOOL	switchDemo,bDemoMode;
DWORD	LastDemo;
BOOL	bCheckRestore;
BOOL	bLastPing,bLastPause,bLastRestart;
BOOL	bShowPing,IsHost;
BOOL	bPause;
DWORD	StartTick,StartPauseTick;
struct	TEXTMESSAGE		txtMsg[TOT_TXTMSG];
struct	Blast			Blasts[MAX_PLAYERS][TOT_BLAST];
int		NumBlasts[MAX_PLAYERS];

struct	Bomb			Bombs[MAX_PLAYERS][TOT_BOMB];
int		NumBombs[MAX_PLAYERS];

struct	Bloc			Blocs[20][15];
struct	Bomber			Bombers[MAX_PLAYERS];
struct	ControlBomber	BCtrl[MAX_PLAYERS];
struct	BomberScore		Score[MAX_SCORES];
struct	RemotePlayerData	RemoteData[MAX_PLAYERS];
struct	DPlayPlayer		DPlayer[MAX_PLAYERS];
DPID	SpectatorID,MulticastID;
int NumPlayers;

RECT	Window;
DWORD	RefreshRate;

//DPID grp;

#ifdef DX7
DWORD	Buffer_pagelock_cnt;
DWORD	Sprites_pagelock_cnt;

LPDIRECTDRAWCOLORCONTROL lpDDCC;
LPDIRECTINPUT2			lpDI;
LPDIRECTINPUTDEVICE		lpDIDevice;
LPDIRECTDRAW7	 		lpDD;
LPDIRECTDRAWSURFACE7	lpDDSPrimary;
LPDIRECTDRAWSURFACE7	lpDDSBack;
LPDIRECTDRAWSURFACE7	lpDDSBuffer;
LPDIRECTDRAWSURFACE7	lpDDSprites;
LPDIRECTDRAWSURFACE7	lpDDSScore;
#ifndef WSDP
LPDIRECTPLAY4A			lpDP;
#endif
LPDIRECTPLAYLOBBY3A		lpDPL;

#elif defined(DX5)
DWORD	Buffer_pagelock_cnt;
DWORD	Sprites_pagelock_cnt;

LPDIRECTDRAWCOLORCONTROL lpDDCC;
LPDIRECTINPUT2			lpDI;
LPDIRECTINPUTDEVICE		lpDIDevice;
LPDIRECTDRAW4	 		lpDD;
LPDIRECTDRAWSURFACE4	lpDDSPrimary;
LPDIRECTDRAWSURFACE4	lpDDSBack;
LPDIRECTDRAWSURFACE4	lpDDSBuffer;
LPDIRECTDRAWSURFACE4	lpDDSprites;
LPDIRECTDRAWSURFACE4	lpDDSScore;
#ifndef WSDP
LPDIRECTPLAY4A			lpDP;
#endif
LPDIRECTPLAYLOBBY3A		lpDPL;

#else

LPDIRECTINPUT			lpDI;
LPDIRECTINPUTDEVICE		lpDIDevice;

LPDIRECTDRAW2	 		lpDD;
LPDIRECTDRAWSURFACE		lpDDSPrimary;
LPDIRECTDRAWSURFACE		lpDDSBack;
LPDIRECTDRAWSURFACE		lpDDSBuffer;
LPDIRECTDRAWSURFACE		lpDDSprites;
LPDIRECTDRAWSURFACE		lpDDSScore;
#ifndef WSDP
LPDIRECTPLAY3A			lpDP;
#endif
#endif
#ifdef WSDP
#define lpDP wsDP
#endif

int		AffScoreX;
BOOL	bScoreRedraw;

int		olddelta;
DWORD	xSudden;
DWORD	GameStartTime,SuddenStart;
BOOL	bSudden;

char	szLastFirst[BMAX_NAME];
RECT	rect;
RECT	updBuff;
int		i,j,k,l,m;
BOOL	bConnected,bSpectator;
BOOL	bDedicated;
DWORD	PerfFreq;
int		VORM,VORM1;
BOOL	bReset,bMustReset,bReseted;
BOOL	bResetReady[MAX_PLAYERS];
char	szCmdLine[BMAX_PATH];

char	SkinDir[MAX_SKINS][BMAX_PATH];
int		NbSkins;
char	szSkinsDir[BMAX_PATH];

char	MapDir[MAX_MAPS][BMAX_PATH];
int		NbMaps;
char	szMapsDir[BMAX_PATH];

char	ConfigName[MAX_CONFIGS][BMAX_PATH];
char	szConfigDir[BMAX_PATH];
int		NbConfigs;
char	szCurConfig[BMAX_PATH];

int	bUpdateAllScreen;
int	bUpdateCol[20];
int	bUpdateRow[15];
int	bUpdateCase[20][15];
BOOL bLobby;
DWORD NetSendTime,LastNSTBias;
DWORD cl_id;
DWORD dwPlayingSession;

BOOL LastCAO;
BOOL bConsole;

#ifndef DX3
DWORD LastLobbyCheck;
GUID PlayerGUID;
BOOL bSendLobbyMessages;
#endif
extern DWORD FtDSize,FtCSize;

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR     lpCmdLine,
					 int       nCmdShow)
{
	hInst=hInstance;
	if(!InitStartup(lpCmdLine))
	{
		FinalShutdown();
		return 0;
	}
	if (!hPrevInstance)
		if(!InitApplication())
			return 0;

	while(MainGame());

	FinalShutdown();
	return 0;
}

void InitTimers(void)
{
	StartTick=0;
	StartTick=TickCount();
	TCount=TimerRef=GameStartTime=StartTime=TickCount();
	FCount=0;
	dt=float(1.0/RefreshRate);
	LastGTC=fTickCount();
	return;
}

#ifdef _DEBUG
void DebugStart();
void DebugStart()
{return;}
#endif

int MainGame(void)
{
	MSG msg;

	switch(InitGameSession()) 
	{
	case 0:
		CleanUp(0);
		return 1;
		break;
	case -1:
		CleanUp(0);
		return 0;
		break;
	}

	for(i=0;i<MAX_PLAYERS;i++)
		ResetBomber(i);
	for(i=0;i<MAX_PLAYERS;i++)
		Bombers[i].Parties=0;

	//NetSendTime=Settings.NET_TOUT;
	if(Settings.NET_TOUT<=2)
		NetSendTime=200;
	else
		NetSendTime=150;

	SetCursor(LoadCursor(0,IDC_APPSTARTING));
	if(!InitInstance(SW_SHOW))
	{
		CleanUp(0);
		return 0;
	}
	InitFileSystem();
	strcpy(szCurMap,Settings.szSelMap);
	InitBlocs(Settings.GO.BLOC_COND);

	Temporize();
	if(!bDedicated)
	{
		InitDDraw();
		Temporize();
		InitDSound();
		Temporize();
		InitDInput();
		Temporize();
		LoadSprites();
		Temporize();
		if(Settings.VOICE&&Settings.ENABLESON&&bConnected&&!bDedicated)
		{
			InitVoiceCapture();
			Temporize();
			StartVoiceCapture();
			Temporize();
		}
	}

	InitTimers();
	ResetGame();
	UpdateAll();
	szLastFirst[0]='\0';
	MaxNameLength=0;

	SetWindowText(hWndWin,szTitle);

	SetCursor(LoadCursor(0,IDC_ARROW));
	if(!Settings.DBGMODE)
		ShowCursor(FALSE);

	for(int pl=0;pl<NumPlayers;pl++)
		Bombers[pl].StartTime=TickCount();

#ifdef _DEBUG
	DebugStart();
#endif

	InitMulti();

//**************************** DEBUT ******************************
	while(1) 
	{
		DWORD timer;
		timer=TickCount();

		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0))
			{
				bQuit=0;
				if(!Settings.GO.TEAMPLAY)
				{
					for(i=0;i<NumPlayers;i++)
						AddPlayerToScores(i);
				}
				else
				{
					for(i=0;i<NumPlayers;i++)
						if(Bombers[i].Team==VOID_TEAM)
							AddPlayerToScores(i);

					for(i=1;i<NUM_TEAMS+1;i++)
						if(GetTeamPop(i)>0)
							AddTeamToScores(i);
				}

				if(!Settings.DBGMODE)
					ShowCursor(TRUE);
				CleanUp(TRUE);
				if(!bLobby)
					return -1;
				else
					return 0;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		GameLoop();

		timer=TickCount()-timer;
		if(timer<SleepTime)
			Sleep(SleepTime-timer);
	}
	return -1;
}

void GereUpdateScreen()
{
	if(bCheckRestore)
		TryRestoreAll();
	RECT rct;
	if(bUpdateAllScreen<=0)
	{
		int col,row;
		for(col=0;col<NumPlayers;col++)
			for(row=0;row<NumBlasts[col];row++)
				if((Blasts[col][row].Etat==B_PLAYING)&&Blasts[col][row].BombBlast)
				{
					bUpdateCol[Blasts[col][row].X]=GetUpdateCount();
					bUpdateRow[Blasts[col][row].Y]=GetUpdateCount();
				}

		for(col=0;col<20;col++)
			if(bUpdateCol[col]>0)
			{
				rct.top=0;
				rct.bottom=480;
				rct.left=col*32;
				rct.right=rct.left+32;
				UpdateScreen(rct);
				bUpdateCol[col]--;
			}

		for(row=0;row<15;row++)
			if(bUpdateRow[row]>0)
			{
				rct.top=row*32;
				rct.bottom=rct.top+32;
				rct.left=0;
				rct.right=640;
				UpdateScreen(rct);
				bUpdateRow[row]--;
			}

		for(col=0;col<20;col++)
			for(row=0;row<15;row++)
				if(bUpdateCase[col][row]>0)
				{
					rct.top=row*32;
					rct.bottom=rct.top+32;
					rct.left=col*32;
					rct.right=rct.left+32;
					UpdateScreen(rct);
					bUpdateCase[col][row]--;
				}
	}
	else
	{
		bUpdateAllScreen--;
		rct.top=0;
		rct.bottom=480;
		rct.left=0;
		rct.right=640;
		UpdateScreen(rct);
	}

	return;
}

void GereMulti()
{
#ifdef WSDP
	wsDP->Update();
#endif
	/*
	if(bConnected)
		if(TickCount()-LastNSTBias>NSTBIAS_TOUT)
		{
			LastNSTBias=TickCount();
			DWORD dec_value=(DWORD)(NST_DEC_BIAS*(NSTBIAS_TOUT/1000.0));
			if(dec_value<NetSendTime)
			{
				NetSendTime-=dec_value;
				OutFileEx("NST dec a ",NetSendTime);
			}
			else
				if(NetSendTime!=0)
				{
					NetSendTime=0;
					OutFile("NST dec a zero");
				}
		}
	*/

	if(IsHost&&bConnected)
		if(TickCount()-LastNPCheck>NPCHECK_TOUT)
		{
			DPSESSIONDESC2 sessionDesc;
			lpDP->GetSessionDesc(0,&sessionDesc.dwSize); 
			DWORD* sDesc=new DWORD[(sessionDesc.dwSize+3)/4];
			sDesc[0]=sessionDesc.dwSize;
			lpDP->GetSessionDesc(sDesc,&sessionDesc.dwSize);
			sessionDesc=*(DPSESSIONDESC2*)sDesc;
			delete[] sDesc;
			DWORD np=GetRealNumPlayers();
			if(sessionDesc.dwUser4!=np)
			{
				OutFile(LSZ_NOTNUMP);
				UpdateSessionDesc();
			}
			/*
			sessionDesc.dwSize=sizeof(DPSESSIONDESC2);
			sessionDesc.lpszSessionNameA=szSessionName;
			lpDP->SetSessionDesc(&sessionDesc,0);
			*/
			LastNPCheck=TickCount();
		}

	char M;
	M=FlushReceiveQueue();
	if ((M==MSG_RESET)||bReset)
		ResetAll();

#ifndef DX3
	GereLobby();
#endif
	return;
}

#ifndef DX3
void GereLobby(void)
{
	if(lpDPL==NULL)
		return;
	if(!bConnected)
		return;
	if(TickCount()-LastLobbyCheck<LOBBY_TOUT)
		return;
	LastLobbyCheck=TickCount();

	BYTE* dplMsg=NULL;
	DWORD dwSize=0;
	DWORD dwMsgFlags=0;
	int hr=lpDPL->ReceiveLobbyMessage(0,0,&dwMsgFlags,NULL,&dwSize);
	if(hr!=DPERR_BUFFERTOOSMALL)
		return;
	dplMsg=new BYTE[dwSize];
	hr=lpDPL->ReceiveLobbyMessage(0,0,&dwMsgFlags,dplMsg,&dwSize);
	if(hr!=DP_OK)
	{
		SAFE_DELETE_ARRAY(dplMsg);
		return;
	}
	switch(((DPLMSG_GENERIC*)dplMsg)->dwType)
	{
	case DPLSYS_GETPROPERTYRESPONSE:

		DPLMSG_GETPROPERTYRESPONSE* dplResp;
		dplResp=(DPLMSG_GETPROPERTYRESPONSE*)dplMsg;
		if((dplResp->hr==DP_OK)&&(IsEqualGUID(DPLPROPERTY_MessagesSupported,dplResp->guidPropertyTag)))
			bSendLobbyMessages=*(BOOL*)(dplResp->dwPropertyData);

		if((dplResp->hr==DP_OK)&&(IsEqualGUID(DPLPROPERTY_PlayerGuid,dplResp->guidPropertyTag)))
			PlayerGUID=*(GUID*)(dplResp->dwPropertyData);
		break;
	}
	SAFE_DELETE_ARRAY(dplMsg);
	return;
}

void SendLobbyScores(DWORD Num)
{
	if((lpDPL==NULL)||!bConnected)
		return;
	if(bSendLobbyMessages&&!IsEqualGUID(PlayerGUID,GUID_NULL))
	{
		DPLMSG_SETPROPERTY* dplSP;
		dplSP=(DPLMSG_SETPROPERTY*)new BYTE[sizeof(DPLMSG_SETPROPERTY)+3*4];
		dplSP->dwType=DPLSYS_SETPROPERTY;
		dplSP->dwRequestID=DPL_NOCONFIRMATION;
		dplSP->guidPlayer=PlayerGUID;
		dplSP->guidPropertyTag=DPLPROPERTY_PlayerScore;
		dplSP->dwDataSize=3*4;
		dplSP->dwPropertyData[0]=BPOINTS(Num);
		dplSP->dwPropertyData[1]=Bombers[Num].Frag;
		dplSP->dwPropertyData[2]=Bombers[Num].AK;
		lpDPL->SendLobbyMessage(DPLMSG_STANDARD,0,dplSP,sizeof(DPLMSG_SETPROPERTY)+3*4);
		SAFE_DELETE_ARRAY(dplSP);
	}
	return;
}
#endif

void GameLoop()
{
	GereGeneral();
	GereStats();
	GereMulti();

	GereVoiceCapture();
	GereVoiceStacks();

	if(!bDedicated)
	{
		if(GetAsyncKState(KEY_CODE(DIK_RETURN,VK_RETURN)))
		{
			if(!bReturnPress)
			{
				bReturnPress=TRUE;
				if(GetAsyncKState(KEY_CODE(DIK_LALT,VK_LMENU)))
				{
					Settings.DBGMODE=!Settings.DBGMODE;
					if(!Settings.DBGMODE)
						ShowCursor(FALSE);
					else
						ShowCursor(TRUE);
					ReInitDDraw();
					SetWindowLong(hWndWin,GWL_EXSTYLE,(Settings.DBGMODE? 0:WS_EX_TOPMOST)|WS_EX_APPWINDOW);
					SetWindowPos(hWndWin,HWND_TOP,
						Window.left=(GetSystemMetrics(SM_CXSCREEN)-640)>>1,
						Window.top=(GetSystemMetrics(SM_CYSCREEN)-480)>>1,
						0,
						0,
						SWP_NOSIZE|SWP_SHOWWINDOW);

					Window.bottom=Window.top+480;
					Window.right=Window.left+640;
				}
			}
		}
		else
			bReturnPress=FALSE;
	}

	if(bPause)
	{
		HDC hdc;
		if(DD_OK==lpDDSBack->GetDC(&hdc))
		{
			ShowText(hdc,LSZ_PAUSE,320,240,DEFAULT_FONT,RGB(0,0,240));
			lpDDSBack->ReleaseDC(hdc);
		}
		GereInput();
		GereAffScore();
		ShowChatText();
		ShowBack();
		GereUpdateScreen();
		bUpdateAllScreen=GetUpdateCount();
		GereDT(TRUE);
		return;
	}

	GereDemoMode();
	GereSuddenDeath();
	GereInput();
	GereChatMessages();
	GereTextMessages();
	GereSynchro();
	GereBlasts();
	GereBombs();
	GereBombers();
	if(!bReseted&&!IsHost&&bSpectator)
		ClearSurface(lpDDSBack,RGB(0,0,0));
	AffMessage();
	GereAffScore();
	GereDT(0);
	ShowChatText();
	ShowBack();
	GereUpdateScreen();

	if(bDedicated&&(NumPlayers==0))
		Sleep(15);
	return;
}

void GereSuddenDeath(void)
{
	if(Settings.GO.SDDEATH_TOUT==0)
		return;
	if(bSudden)
	{
		bUpdateAllScreen=GetUpdateCount();

		if(TickCount()-SuddenStart>=SDDEATH_PITCH)
		{
			SuddenStart=TickCount();
			if(xSudden<10)
			{
				for(int ys=0;ys<15;ys++)
				{
					SuddenAdd(xSudden,ys,TRUE);
					SuddenAdd(19-xSudden,ys,TRUE);
				}
				xSudden++;
			}
		}
	}

	if(!bSudden)
	{
		if(TickCount()-GameStartTime>=Settings.GO.SDDEATH_TOUT)
		{
			SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_SDDEATH,0,0,0,0);
			PlaySon(0,Sons.SndBeep,0);
			int nb;
			for(nb=0;nb<NumPlayers;nb++)
				for(int bl=0;bl<TOT_BOMB;bl++)
					Bombs[nb][bl].Etat=B_DISABLED;
			UpdateBombCount();

			for(nb=0;nb<NumPlayers;nb++)
				for(int bl=0;bl<TOT_BLAST;bl++)
					Blasts[nb][bl].Etat=B_DISABLED;
			UpdateBlastCount();

			bSudden=TRUE;
			xSudden=0;
			SuddenStart=TickCount();
		}
		else
			if(5000+TickCount()-GameStartTime>Settings.GO.SDDEATH_TOUT)
			{
				int delta=5000-(int(TickCount()-GameStartTime)-(int(Settings.GO.SDDEATH_TOUT)-5000));
				delta=1+(delta/1000);
				if(delta!=olddelta)
				{
					PlaySon(0,Sons.SndBeep,FALSE);
					olddelta=delta;
				}
				if(delta!=1)
					sprintf(Buff,LSZ_ldSECS,delta);
				else
					sprintf(Buff,LSZ_ldSEC,delta);
				HDC hdc;
				if(DD_OK==lpDDSBack->GetDC(&hdc))
				{
					ShowText(hdc,Buff,320,240,DEFAULT_FONT,RGB(240,0,0));
					lpDDSBack->ReleaseDC(hdc);
				}
				bUpdateRow[7]=GetUpdateCount();
				bUpdateRow[8]=GetUpdateCount();
			}
	}

	return;
}

void SuddenAdd(DWORD xSudden,DWORD ySudden,BOOL snd)
{
	for(int pl=0;pl<NumPlayers;pl++)
	{
		if((Bombers[pl].Etat!=B_PLAYING)||(Bombers[pl].Sens==B_MORT))
			continue;
		if((xSudden==(DWORD)Bombers[pl].X/32)&&(ySudden==(DWORD)Bombers[pl].Y/32))
		{
			Bombers[pl].AK++;
			SendGameMessage(pl,-1,MSG_GETAK,0,0,0,0);
			sprintf(Buff,LSZ_sSUICIDE,Bombers[pl].Nom);
			AddMessage(Buff,2500,GetBomberColor(pl));
			if(IsLocalPlayer(pl))
				UpdateRemoteData(pl);
			Bombers[pl].Sens=B_MORT;
			if(snd)
				PLAYSON(pl,(Bombers[pl].X-320)/320.0,Bombers[pl].SndAK,0)
			bScoreRedraw=TRUE;
		}
	}
	Blocs[xSudden][ySudden].Type=MUR;
	Blocs[xSudden][ySudden].TypeBonus=0;
	RECT r={xSudden,ySudden,xSudden+1,ySudden+1};
	UpdateBuffer(r);
	bUpdateAllScreen=GetUpdateCount();
	return;
}


void GereGeneral()
{
	if(bDedicated)
	{
		if((GetAsyncKeyState(VK_CONTROL)&0x80000000)&&(GetAsyncKeyState(VK_MENU)&0x80000000)&&(GetAsyncKeyState('Q')&0x80000000))
			bExit=TRUE;
		if(bExit)
		{
			CleanUp(0);
			FinalShutdown();
			exit(EXIT_SUCCESS);
		}

		BOOL CAO;
		CAO=FALSE;
		/*
		if(bConsole)
		{
			INPUT_RECORD inpr;
			DWORD ninpr;

			HANDLE hConIn=GetStdHandle(STD_INPUT_HANDLE);
			GetNumberOfConsoleInputEvents(hConIn,&ninpr);
			if(ninpr>0)
			{
				ReadConsoleInput(hConIn,&inpr,1,&ninpr);
				CloseHandle(hConIn);
				if(ninpr==1)
					if(inpr.EventType==KEY_EVENT)
						if(inpr.Event.KeyEvent.wVirtualKeyCode=='O')
						CAO=(inpr.Event.KeyEvent.bKeyDown==TRUE)
							&&(inpr.Event.KeyEvent.wVirtualKeyCode=='O')
							&&((inpr.Event.KeyEvent.dwControlKeyState&LEFT_CTRL_PRESSED)!=0)
							&&((inpr.Event.KeyEvent.dwControlKeyState&LEFT_ALT_PRESSED)!=0);
			}
		}
		*/
		CAO=(GetAsyncKeyState(VK_CONTROL)&0x80000000)&&(GetAsyncKeyState(VK_MENU)&0x80000000)&&(GetAsyncKeyState('O')&0x80000000);

		if(CAO&&!LastCAO)
			if(bConsole)
				DisableConsole();
			else
			{
				EnableConsole();
				OutFile(LSZ_DED_STARTED);
				if(Settings.USEDPPROTO)
					OutFile(LSZ_DED_DPPROTOON);
				else
					OutFile(LSZ_DED_DPPROTOOFF);
			}

		LastCAO=CAO;
	}

	if(bMustReset)
	{
		int c=-1;
		for(int c1=0;c1<NumPlayers;c1++)
			if(!bResetReady[c1])
				c=c1;

		if(c==-1)
		{
			bReset=TRUE;
			bMustReset=0;
		}
	}

	if(!bDedicated)
	{
		if(GetAsyncKState(KEY_CODE(DIK_F12,VK_F12)))
		{
			if(!bLastPing)
			{
				SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_PING,0,0,0,0);
				bShowPing=TRUE;
			}
			bLastPing=TRUE;
		}
		else
			bLastPing=0;
	}

	if((GetAsyncKState(KEY_CODE(DIK_APPS,VK_APPS))!=0)&&(!bConnected||IsHost))
	{
		if(!bLastRestart)
			bReset=TRUE;
		bLastRestart=TRUE;
	}
	else
		bLastRestart=0;
#ifdef DIK_PAUSE
	if(GetAsyncKState(KEY_CODE(DIK_PAUSE,VK_PAUSE)))
#else
	if(GetAsyncKeyState(VK_PAUSE)
#endif
	{
		if(!bLastPause)
			if(!bPause)
				SetPause();
			else
				ResetPause();
		bLastPause=TRUE;
	}
	else
		bLastPause=0;

	if(!bDedicated)
		if((GetAsyncKeyState(VK_ESCAPE)!=0)&&!bQuit)
		{
			bQuit=TRUE;
			PostQuitMessage(0);
			SendGameMessage(FROM_ALLLOCALPLAYERS,-1,MSG_QUIT,0,0,0,0);
		}

	return;
}

void GereDemoMode(void)
{
	if(!bDemoMode)
		return;
	if(TickCount()-LastDemo>=1000)
	{
		switchDemo=!switchDemo;
		if(!switchDemo)
		{
			bUpdateRow[7]=GetUpdateCount();
			bUpdateRow[8]=GetUpdateCount();
		}
		LastDemo=TickCount();
	}
	if(switchDemo)
	{
		HDC hdc;
		if(DD_OK==lpDDSBack->GetDC(&hdc))
		{
			ShowText(hdc,LSZ_DEMO,320,240,DEFAULT_FONT,RGB(240,0,0));
			lpDDSBack->ReleaseDC(hdc);
		}
		bUpdateRow[7]=GetUpdateCount();
		bUpdateRow[8]=GetUpdateCount();
	}
	return;
}

void SetPause(void)
{
	if(bConnected||bPause)
		return;
	bPause=TRUE;
	StartPauseTick=TickCount();
	return;
}

void ResetPause(void)
{
	if(!bPause)
		return;
	bPause=0;
	StartTick+=TickCount()-StartPauseTick;
	return;
}

void GereInput()
{
	if(bDedicated)
		return;
	if(Settings.USEDINPUT)
	{
		int hr=lpDIDevice->GetDeviceState(256,Keyboard);
		if((hr==DIERR_INPUTLOST)||(hr==DIERR_NOTACQUIRED))
			lpDIDevice->Acquire();
		if(hr!=DI_OK)
			memset(Keyboard,0,256);
	}
	return;
}

void GereDT(BOOL bJustCalc)
{
	if(!bJustCalc)
	{
		FCount++;
		if(FCount>=FRAME_TIME)
		{
			TCount=TickCount()-TCount;
			//OutFileEx("Fps : ",(int)(1000/(TCount/(float)FCount)));
			OutFileEx2("Fps: %ld",(int)(1000/(TCount/(float)FCount)));
			TCount=TickCount();
			FCount=0;
		}
	}

	dt=float(fTickCount()-LastGTC)/float(PerfFreq);
	LastGTC=fTickCount();
	
	if(!bJustCalc)
		if((dt<1.0/RefreshRate)&&Settings.VSYNC)
//		{
//			BOOL status;
			//while((lpDD->GetVerticalBlankStatus(&status)==DD_OK)&&!status);
			lpDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN,0);
//			if(bWin_NT)
//				SwitchToThread();
//		}

	if (dt>0.1)
		dt=float(0.1);
	else
	if (dt<0.0001)
		dt=float(0.0001);
	return;
}

void InitVersionInfo(void)
{
	char nf[BMAX_PATH];
	DWORD h,s;
	GetModuleFileName(GetModuleHandle(0),nf,BMAX_PATH);
	s=GetFileVersionInfoSize(nf,&h);
	if(s!=0)
	{
		UINT s1;
		void *pblock;
		VS_FIXEDFILEINFO *psub;
		pblock=new char[s];
		GetFileVersionInfo(nf,h,s,pblock);
		h=VerQueryValue(pblock,"\\",(void**)&psub,&s1);
		if((psub!=0)&&(h!=0))
			dwVersion=dwProto=(psub->dwProductVersionLS&0xff)+((psub->dwProductVersionLS>>8)&0xff00)
					+((psub->dwProductVersionMS&0xff)<<16)+((psub->dwProductVersionMS&0xff0000)<<8);
		else
		{
			SAFE_DELETE_ARRAY(pblock);
			ErrorMsg(LSZ_EXECORRUPTED,LSZ_FATALERR);
		}
		SAFE_DELETE_ARRAY(pblock);
	}
	else
		ErrorMsg(LSZ_EXECORRUPTED,LSZ_FATALERR);

	return;
}

BOOL InitStartup(const char* lpCmdLine)
{
	/*
	for(i=0;i<NumSysColors;i++)
		oldSysColors[i]=GetSysColor(SysID[i]);

	SetSysColors(NumSysColors,SysID,SysColors);
	*/
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
	wsDP=new WSDPlay();

	//scInit_SeeR();
	InitIA();
	Temporize();
	FT_Start((long*)&FtCSize,(long*)&FtDSize);
	Temporize();
	bLobby=FALSE;

#ifdef GCOMP_LZO
	lzo_init();
#elif defined(GCOMP_UCL)
	ucl_init();
#endif
	srand(TickCount());
	InitCommonControls();
	Temporize();
	CoInitialize(NULL);
	Temporize();
    OSVERSIONINFO osVer;
    osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (GetVersionEx(&osVer))
		bWin_NT=(osVer.dwPlatformId==VER_PLATFORM_WIN32_NT);

	InitVersionInfo();
	Temporize();
#ifdef _DEBUG
	_getcwd(szWorkingDirectory,BMAX_PATH);
#else
	GetModuleFileName(GetModuleHandle(0),szWorkingDirectory,BMAX_PATH);
	for(i=strlen(szWorkingDirectory)-1;i>=0;i--)
		if(szWorkingDirectory[i]=='\\')
			{szWorkingDirectory[i+1]=0;break;}
#endif
	if(szWorkingDirectory[strlen(szWorkingDirectory)-1]!='\\')
		strcat(szWorkingDirectory,"\\");
	_chdir(szWorkingDirectory);
	_mkdir("Skins");
	_mkdir("Configs");
	_mkdir("Maps");
	strcpy(szSkinsDir,szWorkingDirectory);
	strcat(szSkinsDir,"Skins\\");
	strcpy(szMapsDir,szWorkingDirectory);
	strcat(szMapsDir,"Maps\\");
	EnumAll();
	Temporize();

	for(i=0;i<256;i++)
		GetKeyNameText(i<<16,tszName[i],BMAX_PATH);

	sprintf(Buff,"Bomber v%ld.%ld.%ld.%ld",dwVersion>>24,(dwVersion>>16)&255,(dwVersion>>8)&255,dwVersion&255);
	strcpy(szCmdLine,lpCmdLine);
	strtoupper(szCmdLine);

	LARGE_INTEGER lgi;
	if(QueryPerformanceFrequency(&lgi)==0) 
		ErrorMsg(LSZ_REQU586,LSZ_NORDTSC);
	PerfFreq=lgi.LowPart;
	
	if(strstr(szCmdLine,"-HEL"))
	{
		VORM=DDSCAPS_SYSTEMMEMORY;
		VORM1=DDSCAPS_SYSTEMMEMORY;
	}
	else
	{
		VORM=0;
		VORM1=0;
	}

	if(strstr(szCmdLine,"-FORCECOLOR"))
		bClrCtrl=TRUE;
	else
		if(strstr(szCmdLine,"-COLOR"))
			bClrCtrl=TestColorControl();
		else
			bClrCtrl=0;

	if(strstr(szCmdLine,"-ENUMMODES"))
		GetDisplayModes();
	else
		SetDefaultModes();

	Temporize();
	ClearOutFile();
	{
		i=j=-1;
		char *ptr=0,*ptr1=0;
		ptr=strstr(szCmdLine,"-CFG\"");
		if(ptr!=0)
			i=int(ptr-szCmdLine);
		if(i>=0)
			ptr1=strstr(&szCmdLine[i+5],"\"");
		if(ptr1!=0)
			j=int(ptr1-ptr);
	}

	if(j>0)
	{
		BOOL bS;
		memset(szCurConfig,0,BMAX_PATH);
		memcpy(szCurConfig,&szCmdLine[i+5],j-5);
		bS=LoadConfigFile(szCurConfig,&Settings);
		if(!bS)
		{
			memset(szCurConfig,0,BMAX_PATH);
			SetDefaultOptions(&Settings);
			SaveConfigFile("\0",&Settings);
		}
	}
	else
		if(!bWin_NT)
		{
			BOOL bS;
			FILE *F=0;
			F=fopen("Bomber.dat","rt");
			if(F!=0)
			{
				char szCfgc[BMAX_PATH];
				memset(szCfgc,0,BMAX_PATH);
				fscanf(F,"%s",szCfgc);
				strcpy(szCurConfig,szCfgc);
				bS=LoadConfigFile(szCfgc,&Settings);
				fclose(F);
			}
			if((F==0)||(bS==0))
			{
				memset(szCurConfig,0,BMAX_PATH);
				bS=LoadConfigFile("\0",&Settings);
			}

			if(!bS)
			{
				memset(szCurConfig,0,BMAX_PATH);
				SetDefaultOptions(&Settings);
				SaveConfigFile("\0",&Settings);
			}
		}
		else
		{
			BOOL bS;
			char szUsr[BMAX_PATH];
			DWORD strsze;
			strsze=BMAX_PATH;
			memset(szUsr,0,BMAX_PATH);
			GetUserName(szUsr,&strsze);
			strcpy(szCurConfig,szUsr);
			bS=LoadConfigFile(szUsr,&Settings);
			if(!bS)
			{
				SetDefaultOptions(&Settings);
				SaveConfigFile(szUsr,&Settings);
			}
		}

	SleepTime=FRAME_SLEEP_TIME;
	if(i=(int)strstr(szCmdLine,"-S"))
		if(isdigit(*(2+(char*)i)))
			SleepTime=atoi(2+(char*)i);


	bLockedSessionName=FALSE;
	if(i=(int)strstr(szCmdLine,"-SNAME\""))
	{
		j=(int)strstr(7+(char*)i,"\"");
		if(j!=0)
		{
			memcpy(szSessionName,7+(char*)i,j-(i+7));
			szSessionName[1+j-(i+7)]='\0';
			bLockedSessionName=TRUE;
		}
	}

	if(strstr(szCmdLine,"-DEBUG"))
		Settings.DEBUG=TRUE;
	else
		Settings.DEBUG=0;

#ifdef _DEBUG
	Settings.DEBUG=TRUE;
#endif

	if(!Settings.DEBUG)
		remove("Debug.txt");

	//OutFile((const char*)ucl_copyright());
	if(strstr(szCmdLine,"-CHECKDX"))
		if(!CheckOSAndDX())
			return FALSE;;

	Temporize();
#ifndef DX3
	if(strstr(szCmdLine,"-REGISTER"))
	{
		CoCreateInstance(CLSID_DirectPlayLobby,NULL,CLSCTX_INPROC_SERVER,IID_IDirectPlayLobby3A,(void**)&lpDPL);
		if(lpDPL)
		{
			DPAPPLICATIONDESC dpad;
			dpad.dwSize=sizeof(DPAPPLICATIONDESC);
			dpad.dwFlags=0;
			dpad.lpszApplicationNameA="Bomber";
			dpad.guidApplication=BOMBER_GUID;
			dpad.lpszFilenameA="Bomber.exe";
			dpad.lpszCommandLineA=NULL;
			dpad.lpszPathA=szWorkingDirectory;
			dpad.lpszCurrentDirectoryA=szWorkingDirectory;
			dpad.lpszDescriptionA="Bomber";
			dpad.lpszDescriptionW=NULL;
			lpDPL->RegisterApplication(0,&dpad);
		}
	}
	else
		if(strstr(szCmdLine,"-UNREGISTER"))
		{
			CoCreateInstance(CLSID_DirectPlayLobby,NULL,CLSCTX_INPROC_SERVER,IID_IDirectPlayLobby3A,(void**)&lpDPL);
			if(lpDPL)
				int hr=lpDPL->UnregisterApplication(0,BOMBER_GUID);
		}
#endif

	dwLastNumPlayers=2;
	dwLastNetNumPlayers=1;
	CreateDDObject();
	return TRUE;
}

void EnumAll()
{
	_chdir(szWorkingDirectory);
	EnumSkins(szSkinsDir);
	NbSkins=GetSkinCount();
	for(i=0;i<NbSkins;i++)
		GetSkinName(i,SkinDir[i]);

	EnumMaps(szMapsDir);
	NbMaps=GetMapCount();
	for(i=0;i<NbMaps;i++)
		GetMapName(i,MapDir[i]);

	strcpy(szConfigDir,szWorkingDirectory);
	strcat(szConfigDir,"Configs\\");
	EnumConfigs(szConfigDir);
	NbConfigs=GetConfigCount();
	for(i=0;i<NbConfigs;i++)
	{
		GetConfigName(i,ConfigName[i]);
		ConfigName[i][strlen(ConfigName[i])-4]=0;
	}
	return;
}

void InitFileSystem()
{
	if((strlen(Settings.szSelSkin)==0)||(!DoesSkinExist(Settings.szSelSkin)))
	{
		memset(Settings.szSelSkin,0,BMAX_PATH);
		if(!InitFileLoader(szWorkingDirectory,szWorkingDirectory,szPAKFile,szBPassword))
			ErrorMsg(LSZ_CANTINITFS,LSZ_INIT);
	}
	else
	{
		char szD[BMAX_PATH];
		strcpy(szD,szWorkingDirectory);
		strcat(szD,"Skins\\");
		strcat(szD,Settings.szSelSkin);
		if(!InitFileLoader(szWorkingDirectory,szD,szPAKFile,szBPassword))
			ErrorMsg(LSZ_CANTINITFS,LSZ_INIT);
	}

return;
}

int InitClientSession()
{
	DPSESSIONDESC2 sessionDesc;
	DPNAME dpName;
	int hr;
	int r,s;
	int NumLocalPlayers;
	int Me;

	bReseted=FALSE;
	r=DialogBox(0,(LPCSTR)IDD_SESSIONS,NULL,(DLGPROC)DlgProcSessions);
	if (r==-1) return 0;
	if(r==0) 
	{
		WarningMsg(LSZ_DP_CANTJOIN,LSZ_DP_INIT);
		return 0;
	}

	if(!bSpectator)
	{
		NumLocalPlayers=NumPlayers=DialogBox(0,(LPCSTR)IDD_NUMLOCALPLAYERS,NULL,(DLGPROC)DlgProcNumLocalPlayers);
		if((NumLocalPlayers<1)||(NumLocalPlayers>3))
			return 0;
		if(!DialogBox(0,(LPCSTR)IDD_NOMS,NULL,(DLGPROC)DlgProcNoms))
			return 0;
		SaveConfigFile(szCurConfig,&Settings);
	}
	
	hr=lpDP->GetSessionDesc(0,&sessionDesc.dwSize); 
	DWORD* sDesc=new DWORD[sessionDesc.dwSize];
	sDesc[0]=sessionDesc.dwSize;
	hr=lpDP->GetSessionDesc(sDesc,&sessionDesc.dwSize);
	sessionDesc=*(DPSESSIONDESC2*)sDesc;
	SAFE_DELETE_ARRAY(sDesc);

	if(hr!=DP_OK)
	{
		WarningMsg(LSZ_DP_CANTRETINFO,LSZ_DP_INIT);
		return 0;
	}

	if(sessionDesc.dwUser2!=dwProto)
	{
		WarningMsg(LSZ_DP_BADPROTO,LSZ_DP_INIT);
		return 0;
	}

	if(!bSpectator)
	{
		if(sessionDesc.dwUser4+NumLocalPlayers>MAX_PLAYERS)
		{
			WarningMsg("Nombre maximum de joueurs actifs atteint","Mode Multijoueurs");
			return 0;
		}

		if((sessionDesc.dwUser3&BS_PLAYING)&&(sessionDesc.dwUser4>1))
		{
			dwPlayingSession=sessionDesc.dwUser1;
			r=DialogBox(0,(LPCSTR)IDD_WAITSPLAYING,NULL,(DLGPROC)DlgProcWaitSPlaying);
			if(r!=TRUE)
			{
				WarningMsg(LSZ_DP_FREEZEDSESSION,LSZ_DP_INIT);
				return 0;
			}
		}
	}

	hr=lpDP->GetSessionDesc(0,&sessionDesc.dwSize); 
	sDesc=new DWORD[sessionDesc.dwSize];
	sDesc[0]=sessionDesc.dwSize;
	hr=lpDP->GetSessionDesc(sDesc,&sessionDesc.dwSize);
	sessionDesc=*(DPSESSIONDESC2*)sDesc;
	SAFE_DELETE_ARRAY(sDesc);

	if((sessionDesc.dwUser3&BS_PLAYING)==0)
	{
		r=DialogBox(0,(LPCSTR)IDD_WAITSESSION,NULL,(DLGPROC)DlgProcWaitSession);
		if(r!=TRUE)
		{
			WarningMsg(LSZ_DP_FREEZEDSESSION,LSZ_DP_INIT);
			return 0;
		}
	}

	if(!bSpectator)
	{
		Me=sessionDesc.dwUser4;
		NumPlayers=Me+NumLocalPlayers;
		for(r=Me+NumLocalPlayers-1;r>=Me;r--)
		{
			BCtrl[r].Control=CTRL_KB;
			BCtrl[r].cl_id=cl_id;
			Bombers[r].CmdCtrl=r-Me; /***************** Numéro des contrôles en MULTI !!!!! ***************/
			Bombers[r].Etat=B_PLAYING;
			Bombers[r].Color=r;
			Bombers[r].Team=Bombers[r-Me].Team;

			strcpy(Bombers[r].Nom,Bombers[r-Me].Nom);
			if(Settings.IsIA[r-Me])
			{
				Bombers[r].CmdCtrl+=4;
				strcat(Bombers[r].Nom,LSZ_DP_IA);
			}
		}

		for(r=Me;r<Me+NumLocalPlayers;r++)
		{
			Sleep(SLEEP_TIME);
			memset(&dpName,0,sizeof(DPNAME));
			dpName.dwSize = sizeof(DPNAME);
			dpName.lpszShortNameA = Bombers[r].Nom;
			dpName.lpszLongNameA = NULL;
			RemoteData[r].Me=r;
			RemoteData[r].Frag=0;
			RemoteData[r].AK=0;
			RemoteData[r].Team=Bombers[r].Team;
			RemoteData[r].cl_id=cl_id;

			if(lpDP->CreatePlayer(&BCtrl[r].dpid, &dpName,0, &RemoteData[r],sizeof(struct RemotePlayerData),0)!=DP_OK) 
			{
				WarningMsg(LSZ_DP_CANTCREATEPLAYER,LSZ_DP_INIT);
				return 0;
			}
		}

		SendGameMessage(FROM_ALLLOCALPLAYERS,-1,MSG_RESETREADY,0,0,0,0);
	}
	else
	{
		NumPlayers=sessionDesc.dwUser4;

		if(lpDP->CreatePlayer(&SpectatorID, 0,0, 0,0,DPPLAYER_SPECTATOR)!=DP_OK)
		{
			WarningMsg(LSZ_DP_CANTCREATESPECT,LSZ_DP_INIT);
			return 0;
		}
	}

	r=0;
	if(lpDP->EnumPlayers(0,EnumPlayersCallback,&r,DPENUMPLAYERS_REMOTE)!=DP_OK) 
	{
		WarningMsg(LSZ_DP_CANTENUMPLAYERS,LSZ_DP_INIT);
		return 0;
	}
	if((DWORD)r!=sessionDesc.dwUser4) 
	{
		WarningMsg(LSZ_DP_BADPLNB,LSZ_DP_INIT);
		return 0;
	}

	char tcb[BMAX_PATH];
	sprintf(tcb,LSZ_DP_ldPLAYERldSPECT,sessionDesc.dwCurrentPlayers,sessionDesc.dwCurrentPlayers-sessionDesc.dwUser4);
	OutFile(tcb);
	for(s=0;s<r;s++)
	{
		DWORD T=sizeof(struct RemotePlayerData);
		if(lpDP->GetPlayerData(DPlayer[s].dpid,&RemoteData[s],&T,0)!=DP_OK) 
		{
			WarningMsg(LSZ_DP_CANTRETPLINFO,LSZ_DP_INIT);
			return 0;
		}
		DPlayer[s].Num=RemoteData[s].Me;
		BCtrl[DPlayer[s].Num].Connected=B_PLAYING;
		BCtrl[DPlayer[s].Num].Control=CTRL_NET;
		Bombers[DPlayer[s].Num].Etat=B_PLAYING;
		Bombers[DPlayer[s].Num].Frag=RemoteData[s].Frag;
		Bombers[DPlayer[s].Num].AK=RemoteData[s].AK;
		Bombers[DPlayer[s].Num].Team=RemoteData[s].Team;
		if(Bombers[DPlayer[s].Num].Team!=VOID_TEAM)
			Settings.GO.TEAMPLAY=TRUE;
		BCtrl[DPlayer[s].Num].dpid=DPlayer[s].dpid;
		BCtrl[DPlayer[s].Num].cl_id=RemoteData[s].cl_id;
		if(DPlayer[s].dpName.lpszShortNameA)
		{
			strcpy(Bombers[DPlayer[s].Num].Nom,DPlayer[s].dpName.lpszShortNameA);
			SAFE_DELETE_ARRAY(DPlayer[s].dpName.lpszShortNameA);
		}
	}
	UpdateOtherGroup();

return 1;
}

int InitHostSession()
{
	DPSESSIONDESC2 sessionDesc;
	DPNAME dpName;
	int r;

	bReseted=TRUE;
	for(int ct=0;ct<MAX_PLAYERS;ct++)
		bResetReady[ct]=TRUE;
	if(!bLockedSessionName)
		DialogBox(0,(LPCSTR)IDD_NAMESESSION,NULL,(DLGPROC)DlgProcNameSession);

	NumPlayers=DialogBox(0,(LPCSTR)IDD_NUMLOCALPLAYERS,NULL,(DLGPROC)DlgProcNumLocalPlayers);
	if((NumPlayers<1)||(NumPlayers>4))
		return 0;
	if(!DialogBox(0,(LPCSTR)IDD_NOMS,NULL,(DLGPROC)DlgProcNoms))
		return 0;
	SaveConfigFile(szCurConfig,&Settings);

	memset(&sessionDesc,0,sizeof(DPSESSIONDESC2));
	sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
	sessionDesc.dwFlags = DPSESSION_KEEPALIVE|DPSESSION_OPTIMIZELATENCY;
	if(Settings.MIGRATEHOST)
		sessionDesc.dwFlags|=DPSESSION_MIGRATEHOST;
	if(Settings.USEDPPROTO)
		sessionDesc.dwFlags|=DPSESSION_DIRECTPLAYPROTOCOL;

	sessionDesc.guidApplication = BOMBER_GUID;
	sessionDesc.dwMaxPlayers = MAX_SESSION_PLAYERS;
	sessionDesc.lpszSessionNameA = szSessionName;
	sessionDesc.dwUser1=0;
	sessionDesc.dwUser2=dwProto;
	sessionDesc.dwUser3=BS_PLAYING;
	sessionDesc.dwUser4=NumPlayers;

	if(lpDP->Open(&sessionDesc, DPOPEN_CREATE)!=DP_OK)
	{
		WarningMsg(LSZ_DP_CANTCREATE,LSZ_DP_INIT);
		return 0;
	}

	for(r=0;r<NumPlayers;r++)
	{
		BCtrl[r].Control=CTRL_KB;
		BCtrl[r].cl_id=cl_id;
		Bombers[r].Etat=B_PLAYING;
		if(Settings.IsIA[r])
		{
			Bombers[r].CmdCtrl+=4;
			strcat(Bombers[r].Nom,LSZ_DP_IA);
		}

		memset(&dpName,0,sizeof(DPNAME));
		dpName.dwSize = sizeof(DPNAME);
		dpName.lpszShortNameA = Bombers[r].Nom;
		dpName.lpszLongNameA = NULL;
		RemoteData[r].Me=r;
		RemoteData[r].Frag=0;
		RemoteData[r].AK=0;
		RemoteData[r].Team=Bombers[r].Team;
		RemoteData[r].cl_id=cl_id;
		if(lpDP->CreatePlayer(&BCtrl[r].dpid, &dpName,0, &RemoteData[r],sizeof(struct RemotePlayerData),0)!=DP_OK)
		{
			WarningMsg(LSZ_DP_CANTCREATEPLAYER,LSZ_DP_INIT);
			return 0;
		}
	}
return 1;
}

int InitMulticastSession(void)
{
	DPSESSIONDESC2 sessionDesc;
	DPNAME dpName;
	int r;

	cl_id=rand();
	bReseted=TRUE;
	for(int ct=0;ct<MAX_PLAYERS;ct++)
		bResetReady[ct]=TRUE;
	if(!bLockedSessionName)
		DialogBox(0,(LPCSTR)IDD_NAMESESSION,NULL,(DLGPROC)DlgProcNameSession);

	NumPlayers=DialogBox(0,(LPCSTR)IDD_NUMLOCALPLAYERS,NULL,(DLGPROC)DlgProcNumLocalPlayers);
	if((NumPlayers<1)||(NumPlayers>4))
		return 0;
	if(!DialogBox(0,(LPCSTR)IDD_NOMS,NULL,(DLGPROC)DlgProcNoms))
		return 0;
	SaveConfigFile(szCurConfig,&Settings);

	memset(&sessionDesc,0,sizeof(DPSESSIONDESC2));
	sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
	sessionDesc.dwFlags = DPSESSION_KEEPALIVE|DPSESSION_OPTIMIZELATENCY;
	if(Settings.USEDPPROTO)
		sessionDesc.dwFlags|=DPSESSION_DIRECTPLAYPROTOCOL;

	sessionDesc.guidApplication = BOMBER_GUID;
	sessionDesc.dwMaxPlayers = MAX_SESSION_PLAYERS;
	sessionDesc.lpszSessionNameA = szSessionName;
	sessionDesc.dwUser1=0;
	sessionDesc.dwUser2=dwProto;
	sessionDesc.dwUser3=BS_PLAYING|BS_MULTICAST;
	sessionDesc.dwUser4=NumPlayers;

	if(lpDP->Open(&sessionDesc, DPOPEN_CREATE)!=DP_OK)
	{
		WarningMsg(LSZ_DP_CANTCREATE,LSZ_DP_INIT);
		return 0;
	}

	for(r=0;r<NumPlayers;r++)
	{
		BCtrl[r].Control=CTRL_KB;
		BCtrl[r].cl_id=cl_id;
		Bombers[r].Etat=B_PLAYING;
		if(Settings.IsIA[r])
		{
			Bombers[r].CmdCtrl+=4;
			strcat(Bombers[r].Nom,LSZ_DP_IA);
		}

		memset(&dpName,0,sizeof(DPNAME));
		dpName.dwSize = sizeof(DPNAME);
		dpName.lpszShortNameA = Bombers[r].Nom;
		dpName.lpszLongNameA = NULL;
		RemoteData[r].Me=r;
		RemoteData[r].Frag=0;
		RemoteData[r].AK=0;
		RemoteData[r].Team=Bombers[r].Team;
		RemoteData[r].cl_id=cl_id;
		if(lpDP->CreatePlayer(&BCtrl[r].dpid, &dpName,0, &RemoteData[r],sizeof(struct RemotePlayerData),0)!=DP_OK)
		{
			WarningMsg(LSZ_DP_CANTCREATEPLAYER,LSZ_DP_INIT);
			return 0;
		}
	}
	MulticastID=BCtrl[0].dpid;
return 1;
}

int InitDedicatedSession(void)
{
	DPSESSIONDESC2 sessionDesc;
	bReseted=TRUE;
	for(int ct=0;ct<MAX_PLAYERS;ct++)
		bResetReady[ct]=TRUE;
	SaveConfigFile(szCurConfig,&Settings);
	if(!bLockedSessionName)
		DialogBox(0,(LPCSTR)IDD_NAMESESSION,NULL,(DLGPROC)DlgProcNameSession);
	NumPlayers=0;

	memset(&sessionDesc,0,sizeof(DPSESSIONDESC2));
	sessionDesc.dwSize = sizeof(DPSESSIONDESC2);

//	sessionDesc.dwFlags = DPSESSION_MULTICASTSERVER|DPSESSION_KEEPALIVE|DPSESSION_OPTIMIZELATENCY;
	sessionDesc.dwFlags = DPSESSION_KEEPALIVE|DPSESSION_OPTIMIZELATENCY;
	if(Settings.USEDPPROTO)
		sessionDesc.dwFlags|=DPSESSION_DIRECTPLAYPROTOCOL;

	sessionDesc.guidApplication = BOMBER_GUID;
	sessionDesc.dwMaxPlayers = MAX_SESSION_PLAYERS;
	sessionDesc.lpszSessionNameA = szSessionName;
	sessionDesc.dwUser1=0;
	sessionDesc.dwUser2=dwProto;
	sessionDesc.dwUser3=BS_PLAYING|BS_DEDICATED;
	sessionDesc.dwUser4=NumPlayers;

	if(lpDP->Open(&sessionDesc, DPOPEN_CREATE)!=DP_OK)
	{
		WarningMsg(LSZ_DP_CANTCREATE,LSZ_DP_INIT);
		return 0;
	}

	if(lpDP->CreatePlayer(&SpectatorID, 0,0, 0,0,DPPLAYER_SPECTATOR)!=DP_OK)
	{
		WarningMsg(LSZ_DP_CANTCREATESPECT,LSZ_DP_INIT);
		return 0;
	}
	MulticastID=SpectatorID;
return 1;
}

int InitLocalSession()
{
	int r;
	NumPlayers=DialogBox(0,(LPCSTR)IDD_NUMPLAYERS,NULL,(DLGPROC)DlgProcNumPlayers);
	if((NumPlayers<2)||(NumPlayers>4))
		return 0;
	if(!DialogBox(0,(LPCSTR)IDD_NOMS,NULL,(DLGPROC)DlgProcNoms))
		return 0;
	SaveConfigFile(szCurConfig,&Settings);
	for(r=0;r<NumPlayers;r++)
	{
		BCtrl[r].Control=CTRL_KB;
		Bombers[r].Etat=B_PLAYING;
		if(Settings.IsIA[r])
		{
			Bombers[r].CmdCtrl+=4;
			strcat(Bombers[r].Nom,LSZ_DP_IA);
		}
	}
return 1;
}

int InitDemoSession()
{
	int r;
	NumPlayers=randInt(2,4);
	for(r=0;r<NumPlayers;r++)
	{
		BCtrl[r].Control=CTRL_KB;
		Bombers[r].Etat=B_PLAYING;
		Bombers[r].CmdCtrl=r+4;
	}

	strcpy(Bombers[0].Nom,LSZ_DP_IANAME1);
	strcpy(Bombers[1].Nom,LSZ_DP_IANAME2);
	strcpy(Bombers[2].Nom,LSZ_DP_IANAME3);
	strcpy(Bombers[3].Nom,LSZ_DP_IANAME4);
	Settings.SAVESCORES=FALSE;
	switchDemo=bDemoMode=TRUE;
	LastDemo=TickCount();
return 1;
}

#ifndef DX3
int InitLobbySession(DPLCONNECTION* conn)
{
	int hr,r,s,Me;
	int NumLocalPlayers;
	DPSESSIONDESC2 sessionDesc;

	if(strstr(szCmdLine,"-DEDICATED")||(conn->lpSessionDesc->dwUser3&BS_DEDICATED))
	{
		NumPlayers=0;
		IsHost=TRUE;
		bDedicated=TRUE;
		conn->lpSessionDesc->dwUser3=BS_PLAYING|BS_DEDICATED;
		lpDPL->SetConnectionSettings(0,0,conn);
	}
	else
	{
		NumPlayers=1;
		if(conn->dwFlags&DPLCONNECTION_CREATESESSION)
		{
			IsHost=TRUE;
			conn->lpSessionDesc->dwUser1=0;
			conn->lpSessionDesc->dwUser2=dwProto;
			conn->lpSessionDesc->dwUser3=BS_PLAYING;
			conn->lpSessionDesc->dwUser4=NumPlayers;
			lpDPL->SetConnectionSettings(0,0,conn);
		}
	}

	SAFE_RELEASE(lpDP);

	hr=lpDPL->ConnectEx(0,IID_IDirectPlay4A,(void**)&lpDP,NULL);
	if((lpDP==NULL)||(hr!=DP_OK))
		ErrorMsg(LSZ_DP_CANTCONNECTLOBBY,LSZ_DP_INIT);
/*	{
		return 0;
	}
*/
	if(IsHost)
	{
		if(bDedicated)
		{
			if(lpDP->CreatePlayer(&SpectatorID, 0,0, 0,0,DPPLAYER_SPECTATOR)!=DP_OK)
			{
				WarningMsg(LSZ_DP_CANTCREATESPECT,LSZ_DP_INIT);
				return 0;
			}
			MulticastID=SpectatorID;

			if(strstr(szCmdLine,"-NOCONSOLE")==0)
				EnableConsole();
			OutFile(LSZ_DED_STARTED);
			if(Settings.USEDPPROTO)
				OutFile(LSZ_DED_DPPROTOON);
			else
				OutFile(LSZ_DED_DPPROTOOFF);
		}
		else
		{
			for(r=0;r<NumPlayers;r++) //1 !!
			{
				BCtrl[r].Control=CTRL_KB;
				BCtrl[r].cl_id=cl_id;
				Bombers[r].Etat=B_PLAYING;

				strcpy(Bombers[r].Nom,conn->lpPlayerName->lpszShortNameA);
				RemoteData[r].Me=r;
				RemoteData[r].Frag=0;
				RemoteData[r].AK=0;
				RemoteData[r].Team=VOID_TEAM;
				RemoteData[r].cl_id=cl_id;
				if(lpDP->CreatePlayer(&BCtrl[r].dpid, conn->lpPlayerName,0, &RemoteData[r],sizeof(struct RemotePlayerData),0)!=DP_OK)
				{
					WarningMsg(LSZ_DP_CANTCREATEPLAYER,LSZ_DP_INIT);
					return 0;
				}
			}
		}
	}
	else
	{
		NumLocalPlayers=1;

		hr=lpDP->GetSessionDesc(0,&sessionDesc.dwSize); 
		DWORD* sDesc=new DWORD[sessionDesc.dwSize];
		sDesc[0]=sessionDesc.dwSize;
		hr=lpDP->GetSessionDesc(sDesc,&sessionDesc.dwSize);
		sessionDesc=*(DPSESSIONDESC2*)sDesc;
		SAFE_DELETE_ARRAY(sDesc)

		if(hr!=DP_OK)
		{
			WarningMsg(LSZ_DP_CANTRETINFO,LSZ_DP_INIT);
			return 0;
		}

		if(sessionDesc.dwUser2!=dwProto)
		{
			WarningMsg(LSZ_DP_BADPROTO,LSZ_DP_INIT);
			return 0;
		}

		if(!bSpectator)
		{
			if(sessionDesc.dwUser4+NumLocalPlayers>MAX_PLAYERS)
			{
				WarningMsg("Nombre maximum de joueurs actifs atteint","Mode Multijoueurs");
				return 0;
			}

			if(sessionDesc.dwUser3&BS_PLAYING&&(sessionDesc.dwUser4>1))
			{
				dwPlayingSession=sessionDesc.dwUser1;
				r=DialogBox(0,(LPCSTR)IDD_WAITSPLAYING,NULL,(DLGPROC)DlgProcWaitSPlaying);
				if(r!=TRUE)
				{
					WarningMsg(LSZ_DP_FREEZEDSESSION,LSZ_DP_INIT);
					return 0;
				}
			}
		}

		hr=lpDP->GetSessionDesc(0,&sessionDesc.dwSize); 
		sDesc=new DWORD[sessionDesc.dwSize];
		sDesc[0]=sessionDesc.dwSize;
		hr=lpDP->GetSessionDesc(sDesc,&sessionDesc.dwSize);
		sessionDesc=*(DPSESSIONDESC2*)sDesc;
		SAFE_DELETE_ARRAY(sDesc);

		if((sessionDesc.dwUser3&BS_PLAYING)==0)
		{
			r=DialogBox(0,(LPCSTR)IDD_WAITSESSION,NULL,(DLGPROC)DlgProcWaitSession);
			if(r!=TRUE)
			{
				WarningMsg(LSZ_DP_FREEZEDSESSION,LSZ_DP_INIT);
				return 0;
			}
		}

		if(!bSpectator)
		{
			Me=sessionDesc.dwUser4;
			NumPlayers=Me+NumLocalPlayers;
			for(r=Me+NumLocalPlayers-1;r>=Me;r--)
			{
				BCtrl[r].Control=CTRL_KB;
				BCtrl[r].cl_id=cl_id;
				Bombers[r].CmdCtrl=r-Me; /***************** Numéro des contrôles en MULTI !!!!! ***************/
				Bombers[r].Etat=B_PLAYING;
				Bombers[r].Color=r;
				Bombers[r].Team=VOID_TEAM;

				strcpy(Bombers[r].Nom,conn->lpPlayerName->lpszShortNameA);
			}

			for(r=Me;r<Me+NumLocalPlayers;r++)
			{
				Sleep(SLEEP_TIME);
				RemoteData[r].Me=r;
				RemoteData[r].Frag=0;
				RemoteData[r].AK=0;
				RemoteData[r].Team=VOID_TEAM;
				RemoteData[r].cl_id=cl_id;

				if(lpDP->CreatePlayer(&BCtrl[r].dpid, conn->lpPlayerName,0, &RemoteData[r],sizeof(struct RemotePlayerData),0)!=DP_OK) 
				{
					WarningMsg(LSZ_DP_CANTCREATEPLAYER,LSZ_DP_INIT);
					return 0;
				}
			}

			SendGameMessage(FROM_ALLLOCALPLAYERS,-1,MSG_RESETREADY,0,0,0,0);
		}
		else
		{
			NumPlayers=sessionDesc.dwUser4;

			if(lpDP->CreatePlayer(&SpectatorID, 0,0, 0,0,DPPLAYER_SPECTATOR)!=DP_OK)
			{
				WarningMsg(LSZ_DP_CANTCREATESPECT,LSZ_DP_INIT);
				return 0;
			}
		}

		r=0;
		if(lpDP->EnumPlayers(0,EnumPlayersCallback,&r,DPENUMPLAYERS_REMOTE)!=DP_OK) 
		{
			WarningMsg(LSZ_DP_CANTENUMPLAYERS,LSZ_DP_INIT);
			return 0;
		}
		if((DWORD)r!=sessionDesc.dwUser4) 
		{
			WarningMsg(LSZ_DP_BADPLNB,LSZ_DP_INIT);
			return 0;
		}

		char tcb[BMAX_PATH];
		sprintf(tcb,LSZ_DP_ldPLAYERldSPECT,sessionDesc.dwCurrentPlayers,sessionDesc.dwCurrentPlayers-sessionDesc.dwUser4);
		OutFile(tcb);
		for(s=0;s<r;s++)
		{
			DWORD T=sizeof(struct RemotePlayerData);
			if(lpDP->GetPlayerData(DPlayer[s].dpid,&RemoteData[s],&T,0)!=DP_OK) 
			{
				WarningMsg(LSZ_DP_CANTRETPLINFO,LSZ_DP_INIT);
				return 0;
			}
			DPlayer[s].Num=RemoteData[s].Me;
			BCtrl[DPlayer[s].Num].Connected=B_PLAYING;
			BCtrl[DPlayer[s].Num].Control=CTRL_NET;
			Bombers[DPlayer[s].Num].Etat=B_PLAYING;
			Bombers[DPlayer[s].Num].Frag=RemoteData[s].Frag;
			Bombers[DPlayer[s].Num].AK=RemoteData[s].AK;
			Bombers[DPlayer[s].Num].Team=RemoteData[s].Team;
			if(Bombers[DPlayer[s].Num].Team!=VOID_TEAM)
				Settings.GO.TEAMPLAY=TRUE;
			BCtrl[DPlayer[s].Num].dpid=DPlayer[s].dpid;
			BCtrl[DPlayer[s].Num].cl_id=RemoteData[s].cl_id;
			if(DPlayer[s].dpName.lpszShortNameA)
			{
				strcpy(Bombers[DPlayer[s].Num].Nom,DPlayer[s].dpName.lpszShortNameA);
				SAFE_DELETE_ARRAY(DPlayer[s].dpName.lpszShortNameA);
			}
		}
	}

	DPLMSG_GETPROPERTY dplMsg;
	dplMsg.dwType=DPLSYS_GETPROPERTY;
	dplMsg.dwRequestID=1;
	dplMsg.guidPlayer=GUID_NULL;
	dplMsg.guidPropertyTag=DPLPROPERTY_MessagesSupported;
	lpDPL->SendLobbyMessage(DPLMSG_STANDARD,0,&dplMsg,sizeof(DPLMSG_GETPROPERTY));

	dplMsg.dwType=DPLSYS_GETPROPERTY;
	dplMsg.dwRequestID=2;
	dplMsg.guidPlayer=GUID_NULL;
	dplMsg.guidPropertyTag=DPLPROPERTY_PlayerGuid;
	lpDPL->SendLobbyMessage(DPLMSG_STANDARD,0,&dplMsg,sizeof(DPLMSG_GETPROPERTY));

	PlayerGUID=GUID_NULL;
	bSendLobbyMessages=FALSE;
	return 1;
}
#endif

int InitGameSession(void)
{
	int hr;
	bDedicated=bScoring=bPause=bConnected=bSpectator=0;
	MulticastID=0;
	lGTC=0;
	bChat=0;
	strcpy(ChatText,"\0");
	bReseted=TRUE;
	bDemoMode=FALSE;
	SAFE_RELEASE(lpDP);
	for(i=0;i<MAX_PLAYERS;i++)
		InitBomber(i);
	for(i=0;i<MAX_SCORES;i++)
		Score[i].bSet=0;
	for(i=0;i<TOT_TXTMSG;i++)
		txtMsg[i].Valide=0;
	cl_id=rand();

#ifndef DX3
	DWORD dwCSSize=0;
	char *connSettings;

	if(lpDPL==NULL)
		CoCreateInstance(CLSID_DirectPlayLobby,NULL,CLSCTX_INPROC_SERVER,IID_IDirectPlayLobby3A,(void**)&lpDPL);

	if(lpDPL)
	{
		hr=lpDPL->GetConnectionSettings(0,0,&dwCSSize);
		if(hr!=DPERR_NOTLOBBIED)
		{
			connSettings=new char[dwCSSize];
			hr=lpDPL->GetConnectionSettings(0,connSettings,&dwCSSize);
			if(hr==DP_OK)
			{
				bConnected=TRUE;
				hr=InitLobbySession((DPLCONNECTION*)connSettings);
				SAFE_DELETE_ARRAY(connSettings);
				if(hr==1)
					bLobby=TRUE;
				return hr;
			}
			SAFE_DELETE_ARRAY(connSettings);
		}
		else
			SAFE_RELEASE(lpDPL);
	}
#endif
/*
	FILE* FC=NULL;
	if(strstr(szCmdLine,"-DEDTCP"))
		FC=fopen("dedicated.tcp","rb");
	else
	if(strstr(szCmdLine,"-DEDIPX"))
		FC=fopen("dedicated.ipx","rb");
*/
#ifndef GOA
	if(strstr(szCmdLine,"-DEDTCP")||strstr(szCmdLine,"-DEDIPX"))
	{
		/*
		fread(&i,4,1,FC);
		BYTE* lpCSettings=new BYTE[i];
		fread(lpCSettings,i,1,FC);
		fclose(FC);
		*/

		if(lpDP==NULL)
		{
#ifdef WSDP
			wsDP=new WSDPlay();
#else
#ifndef DX3
			CoCreateInstance( CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER,
							 IID_IDirectPlay4A, (LPVOID*)&lpDP);
#else
			CoCreateInstance( CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, 
							 IID_IDirectPlay3A, (LPVOID*)&lpDP);
#endif
#endif
		}

		if(lpDP==NULL)
		{
			WarningMsg(LSZ_DP_CANTCREATEDP,LSZ_DP_INIT);
			return -1;
		}
/*
		hr=lpDP->Initialize(0);
		if(hr!=DP_OK)
		{
			WarningMsg(LSZ_DP_CANTINIT,LSZ_DP_INIT);
			return -1;
		}
*/
		*(DWORD*)Buff=0;
		if(strstr(szCmdLine,"-DEDTCP"))
			lpDP->EnumConnections(&BOMBER_GUID,DedicatedEnumConnectionsCallback, (void*)&DPSPGUID_TCPIP, DPCONNECTION_DIRECTPLAY);
		else
			if(strstr(szCmdLine,"-DEDIPX"))
				lpDP->EnumConnections(&BOMBER_GUID,DedicatedEnumConnectionsCallback, (void*)&DPSPGUID_IPX, DPCONNECTION_DIRECTPLAY);

		i=*(DWORD*)Buff;
		if(i==0)
		{
			WarningMsg(LSZ_DP_NOSP,LSZ_DP_INIT);
			return -1;
		}

		hr=lpDP->InitializeConnection(Buff+4,NULL);
		if(hr!=DP_OK)
		{
			WarningMsg(LSZ_DP_CANTINITCONN,LSZ_DP_INIT);
			return -1;
		}

//		if(!bLockedSessionName)
//			sprintf(szSessionName,"Session #%ld",rand()%30000);

		NumPlayers=0;
		IsHost=bDedicated=bConnected=TRUE;

		DPSESSIONDESC2 sessionDesc;
		memset(&sessionDesc,0,sizeof(DPSESSIONDESC2));
		sessionDesc.dwSize = sizeof(DPSESSIONDESC2);

		sessionDesc.dwFlags = DPSESSION_KEEPALIVE|DPSESSION_OPTIMIZELATENCY;
		if(Settings.USEDPPROTO)
			sessionDesc.dwFlags|=DPSESSION_DIRECTPLAYPROTOCOL;

		sessionDesc.guidApplication = BOMBER_GUID;
		sessionDesc.dwMaxPlayers = MAX_SESSION_PLAYERS;
		sessionDesc.lpszSessionNameA = szSessionName;
		sessionDesc.dwUser1=0;
		sessionDesc.dwUser2=dwProto;
		sessionDesc.dwUser3=BS_PLAYING|BS_DEDICATED;
		sessionDesc.dwUser4=NumPlayers;

		if(lpDP->Open(&sessionDesc, DPOPEN_CREATE)!=DP_OK)
		{
			WarningMsg(LSZ_DP_CANTCREATE,LSZ_DP_INIT);
			return -1;
		}

		if(lpDP->CreatePlayer(&SpectatorID, 0,0, 0,0,DPPLAYER_SPECTATOR)!=DP_OK)
		{
			WarningMsg(LSZ_DP_CANTCREATESPECT,LSZ_DP_INIT);
			return -1;
		}
		MulticastID=SpectatorID;

		if(strstr(szCmdLine,"-NOCONSOLE")==0)
			EnableConsole();
		OutFile(LSZ_DED_STARTED);
		if(Settings.USEDPPROTO)
			OutFile(LSZ_DED_DPPROTOON);
		else
			OutFile(LSZ_DED_DPPROTOOFF);
		return 1;
	}
#endif

	if(strstr(szCmdLine,"-DEMO")==0)
	{
#ifndef GOA
		i=DialogBox(0,(LPCSTR)IDD_START,NULL,(DLGPROC)DlgProcStart);
		if(i==-1)
			return -1;
#else
		struct SETTINGS SAVSettings;
		SAVSettings=Settings;
		i=DialogBox(0,(LPCSTR)IDD_OPTIONS,NULL,(DLGPROC)DlgProcOptions);
		if(i)
		{
			Settings=SAVSettings;
			return -1;
		}
		i=1;
#endif
	}
	else
		i=2; //Mode démo

	if(i!=2)
	{
		if(i==0) 
		{
			IsHost=TRUE;
			j=DialogBox(0,(LPCSTR)IDD_PROVIDER,NULL,(DLGPROC)DlgProcProvider);
			if (j==-1) return 0;
		}//i=CreateGame();
		else
			if(i==1)
			{
				IsHost=0;
				j=DialogBox(0,(LPCSTR)IDD_PROVIDER,NULL,(DLGPROC)DlgProcProvider);
				if (j==-1) return 0;
			}//i=JoinGame();
			else
				return 0;

		if(j==1)
		{
			bConnected=TRUE;
			InitMulti();
			if(!IsHost)
				hr=InitClientSession();
			else
				switch(Settings.SERVER_TYPE)
				{
				case ST_DEDICATED:
					bDedicated=TRUE;
					hr=InitDedicatedSession();
					if(hr==1)
					{
						if(strstr(szCmdLine,"-NOCONSOLE")==0)
							EnableConsole();
						OutFile(LSZ_DED_STARTED);
						if(Settings.USEDPPROTO)
							OutFile(LSZ_DED_DPPROTOON);
						else
							OutFile(LSZ_DED_DPPROTOOFF);
					}
					break;

				case ST_MULTICAST:
					hr=InitMulticastSession();
					break;

				case ST_NORMAL:
					hr=InitHostSession();
					break;
				}
		}
		else
			hr=InitLocalSession();
	}
	else
		hr=InitDemoSession();

	if(hr!=1)
		return hr;

return 1;
}

void EnableConsole(void)
{
	if(bConsole)
		return;
	hWndCon=CreateDialog(hInst,(LPCSTR)IDD_CONSOLE,NULL,(DLGPROC)DlgProcConsole);
	sprintf(Buff,LSZ_DED_TITLE,szSessionName);
	SetWindowText(hWndCon,Buff);
	bConsole=TRUE;
	/*
	AllocConsole();
	Sleep(100);
	SetConsoleCtrlHandler(CtrlHandler,TRUE);
	Sleep(100);
	sprintf(Buff,LSZ_DED_TITLE,szSessionName);
	SetConsoleTitle(Buff);
	*/
	return;
}

void DisableConsole(void)
{
	if(!bConsole)
		return;
	/*
	SetConsoleCtrlHandler(CtrlHandler,FALSE);
	FreeConsole();
	*/
	DestroyWindow(hWndCon);
	bConsole=FALSE;
	return;
}

BOOL WINAPI CtrlHandler(DWORD mode)
{
	bQuit=TRUE;
	PostQuitMessage(0);
	OutFile(LSZ_DED_STOPPING);
	CleanUp(0);
	FinalShutdown();
	exit(EXIT_SUCCESS);
	return 0;
}

void SaveConfigFile(const char* szCfg,const struct SETTINGS* pSettings)
{
	struct SETTINGS sSettings;
	FILE* F;
	char FileName[BMAX_PATH];
	_chdir(szConfigDir);
	sSettings=*pSettings;
	sSettings.cksum=127;
	sSettings.cksum=CheckSumSettings(&sSettings);
	if(strlen(szCfg)!=0)
	{
		_chdir(szConfigDir);
		strcpy(FileName,szCfg);
		strcat(FileName,".cfg");
		F=fopen(FileName,"wb");
		if(F!=0)
		{
			fwrite(&sSettings,sizeof(struct SETTINGS),1,F);
			fclose(F);
		}
		_chdir(szWorkingDirectory);
		F=fopen("Bomber.","wb");
		if(F!=0)
		{
			fwrite(&sSettings,sizeof(struct SETTINGS),1,F);
			fclose(F);
		}
	}
	else
	{
		_chdir(szWorkingDirectory);
		F=fopen("Bomber.","wb");
		if(F!=0)
		{
			fwrite(&sSettings,sizeof(struct SETTINGS),1,F);
			fclose(F);
		}
	}
	_chdir(szWorkingDirectory);
	F=fopen("Bomber.dat","wt");
	if(F!=0)
	{
		fprintf(F,"%s",szCurConfig);
		fclose(F);
	}
	return;
}

BOOL CreateNewConfigFile(const char* szFN)
{
	char FileName[BMAX_PATH];
	struct SETTINGS stmp;
	_chdir(szConfigDir);
	SetDefaultOptions(&stmp);
	strcpy(stmp.szConfigName,szFN);
	strcpy(FileName,szFN);
	strcat(FileName,".cfg");
	if(DoesFileExist(FileName))
		return 0;
	FILE*F;
	F=fopen(FileName,"wb");
	if(F!=0)
	{
		fwrite(&stmp,sizeof(struct SETTINGS),1,F);
		fclose(F);
	}
	return TRUE;
}

BOOL LoadConfigFile(const char* szCfg,struct SETTINGS* pSettings)
{
	FILE* F;
	char FileName[BMAX_PATH];
	_chdir(szConfigDir);
	strcpy(FileName,szCfg);
	if(strlen(FileName)!=0)
	{
		strcat(FileName,".cfg");
		F=fopen(FileName,"rb");
		if(F==0)
			return 0;
	}
	else
	{
		_chdir(szWorkingDirectory);
		F=fopen("Bomber.","rb");
		if(F==0)
			return 0;
	}
	if(fread(pSettings,sizeof(struct SETTINGS),1,F)==1)
	{
		fclose(F);
		BYTE cksum=pSettings->cksum;
		pSettings->cksum=127;
		if(CheckSumSettings(pSettings)!=cksum)
			return 0;
		return TRUE;
	}
	else
	{
		fclose(F);
		return 0;
	}
}

void InitDInput(void)
{
	if(Settings.USEDINPUT)
	{
#ifndef DX3
		CoCreateInstance(CLSID_DirectInput,
			NULL, CLSCTX_INPROC_SERVER, IID_IDirectInput2, (void**)&lpDI);
#else
		CoCreateInstance(CLSID_DirectInput,
			NULL, CLSCTX_INPROC_SERVER, IID_IDirectInput, (void**)&lpDI);
#endif
		if(lpDI==0) ErrorMsg(LSZ_DI_FAILED,LSZ_DI_INIT);

		if(lpDI->Initialize(hInst,DIRECTINPUT_VERSION)!=DI_OK) 
			ErrorMsg(LSZ_DI_CANTINIT,LSZ_DI_INIT);
		lpDI->CreateDevice(GUID_SysKeyboard,&lpDIDevice,0);
		if(lpDIDevice==0) 
			ErrorMsg(LSZ_DI_CANTCREATEKB,LSZ_DI_INIT);
		lpDIDevice->SetDataFormat(&c_dfDIKeyboard);
		if(Settings.DBGMODE)
			lpDIDevice->SetCooperativeLevel(hWndWin,DISCL_BACKGROUND|DISCL_NONEXCLUSIVE);
		else
			lpDIDevice->SetCooperativeLevel(hWndWin,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE);
		lpDIDevice->Acquire();
	}
	return;
}
/*
BOOL CALLBACK DIEnumDeviceObjectsProc(
    LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	if(lpddoi->guidType!=GUID_Key) 
		return DIENUM_CONTINUE;
	if(lpddoi->dwOfs>255) 
		return DIENUM_CONTINUE;
	strcpy(tszName[lpddoi->dwOfs],lpddoi->tszName);

	return DIENUM_CONTINUE;
}
*/
BOOL TestColorControl()
{
#ifndef DX3
	int hr;
	DDCAPS HALCaps,HELCaps;
	HALCaps.dwSize=HELCaps.dwSize=sizeof(DDCAPS);
	CreateDDObject();
	hr=lpDD->GetCaps(&HALCaps,&HELCaps);
	if(hr!=DD_OK)
		return FALSE;
	if((hr==DD_OK)&&((HALCaps.dwCaps2&DDCAPS2_COLORCONTROLPRIMARY)||
		(HELCaps.dwCaps2&DDCAPS2_COLORCONTROLPRIMARY)))
		return TRUE;
	else
		return FALSE;
	return FALSE;
#else
	return FALSE;
#endif
}

void GetDisplayModes(void)
{
	int hr;

	CreateDDObject();
#ifndef DX3
	DDSURFACEDESC2 ddsded;
	ddsded.dwSize=sizeof(DDSURFACEDESC2);
#else
	DDSURFACEDESC ddsded;
	ddsded.dwSize=sizeof(DDSURFACEDESC);
#endif
	ddsded.dwFlags=DDSD_WIDTH|DDSD_HEIGHT;
	ddsded.dwWidth=640;
	ddsded.dwHeight=480;
	NumModes=0;
	hr=lpDD->EnumDisplayModes(0,&ddsded,NULL,EnumsModesCallback);

	if(hr!=DD_OK)
	{
		WarningMsg(LSZ_CANTENUMMODES,LSZ_VMENUM);
		SetDefaultModes();
	}

	if(NumModes==0)
	{
		WarningMsg(LSZ_NOVM,LSZ_VMENUM);
		SetDefaultModes();
	}
	return;
}

void SetDefaultModes(void)
{
	NumModes=4;
	Depth[0]=8;
	Depth[1]=16;
	Depth[2]=24;
	Depth[3]=32;
	return;
}

#ifndef DX3
HRESULT WINAPI EnumsModesCallback(LPDDSURFACEDESC2 lpDDSurfaceDesc,
								  LPVOID lpContext)
#else
HRESULT WINAPI EnumsModesCallback(LPDDSURFACEDESC lpDDSurfaceDesc,
								  LPVOID lpContext)
#endif
{
	if(NumModes<MAX_VIDMODES)
	{
		Depth[NumModes]=lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount;
		NumModes++;
		return DDENUMRET_OK;
	}
	else
	{
		OutFile(LSZ_TOOMANYVM);
		return DDENUMRET_CANCEL;
	}
	return DDENUMRET_OK;
}

void ReleaseDDObject()
{
	SAFE_RELEASE(lpDD)
	return;
}
void CreateDDObject()
{
	if(lpDD!=0)
		return;
#ifdef DX7
	CoCreateInstance(CLSID_DirectDraw,
		NULL, CLSCTX_INPROC_SERVER, IID_IDirectDraw7, (void**)&lpDD);
#elif defined DX5
	CoCreateInstance(CLSID_DirectDraw,
		NULL, CLSCTX_INPROC_SERVER, IID_IDirectDraw4, (void**)&lpDD);
#else
	CoCreateInstance(CLSID_DirectDraw,
		NULL, CLSCTX_INPROC_SERVER, IID_IDirectDraw2, (void**)&lpDD);
#endif
	if(lpDD==0) ErrorMsg(LSZ_DD_FAILED,LSZ_DD_INIT);

	if(strstr(szCmdLine,"-HEL"))
	{
		if(lpDD->Initialize((LPGUID)DDCREATE_EMULATIONONLY)!=DD_OK) 
			ErrorMsg(LSZ_DD_CANTINIT,LSZ_DD_INIT);
	}
		else
		if(strstr(szCmdLine,"-HAL"))
		{
			if(lpDD->Initialize((LPGUID)DDCREATE_HARDWAREONLY)!=DD_OK) 
				ErrorMsg(LSZ_DD_CANTINIT,LSZ_DD_INIT);
		}
			else
				if(lpDD->Initialize(0)!=DD_OK) 
					ErrorMsg(LSZ_DD_CANTINIT,LSZ_DD_INIT);
	return;
}

void InitDDraw()
{
#ifndef DX3
	DDSURFACEDESC2	ddsd;
	DDSCAPS2		ddscaps;
#else
	DDSURFACEDESC	ddsd;
	DDSCAPS			ddscaps;
#endif
	int hr;

	CreateDDObject();
	memset(&ddsd,0,sizeof(ddsd));
	if(!Settings.DBGMODE)
	{
		hr=lpDD->SetCooperativeLevel(hWndWin,DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT);
		if(hr!=DD_OK)
			ErrorMsg(LSZ_DD_CANTSETCOOP,LSZ_DD_INIT);

		hr=lpDD->SetDisplayMode(640,480,Settings.Depth,0,0);
		if(hr!=DD_OK)
			ErrorMsg(LSZ_DD_UNSUPPVM,LSZ_DD_INIT);
/*
	SIZE sdm={640,480};
	hr=lpDD->StartModeTest(&sdm,1,5000);
	OutFileInt(DDERR_CURRENTLYNOTAVAIL);
	OutFileInt(DDERR_INVALIDOBJECT);
	OutFileInt(DDERR_NOEXCLUSIVEMODE);
	OutFileInt(DDERR_NOTFOUND);
	OutFileInt(DDERR_TESTFINISHED);
	OutFileInt(DDERR_NODRIVERSUPPORT);
	OutFileInt(DDERR_NOMONITORINFORMATION);
	OutFileInt(hr);
	DWORD dw=0;
	hr=lpDD->EvaluateMode(DDEM_MODEPASSED,&dw);
*/

	OutFileInt(hr);

		if(Settings.USEFLIP)
		{
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_COMPLEX|DDSCAPS_FLIP;
			ddsd.dwBackBufferCount=Settings.USETBUFFER? 2:1;
			lpDD->CreateSurface( &ddsd,&lpDDSPrimary, NULL );
			if(lpDDSPrimary==0)
				ErrorMsg(LSZ_DD_CANTCREATESP,LSZ_DD_INIT);

			memset(&ddscaps,0,sizeof(ddscaps));
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
			lpDDSPrimary->GetAttachedSurface(&ddscaps,&lpDDSBack);
			if(lpDDSBack==0)
				ErrorMsg(LSZ_DD_CANTGETATTACHED,LSZ_DD_INIT);
		}
		else
		{
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_CAPS;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			lpDD->CreateSurface( &ddsd,&lpDDSPrimary, NULL );
			if(lpDDSPrimary==0)
				ErrorMsg(LSZ_DD_CANTCREATESP,LSZ_DD_INIT);

			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
			ddsd.dwWidth=640;
			ddsd.dwHeight=480;
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | VORM;
			lpDD->CreateSurface( &ddsd,&lpDDSBack, NULL );
			if(lpDDSBack==0)
				ErrorMsg(LSZ_DD_CANTCREATESS,LSZ_DD_INIT);
		}
		memset(&ddsd,0,sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;// | DDSD_PIXELFORMAT;
		ddsd.dwWidth=SPR_SIZEX;
		ddsd.dwHeight=SPR_SIZEY;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | Settings.SPRVIDEO? 0:DDSCAPS_SYSTEMMEMORY;
		lpDD->CreateSurface( &ddsd, &lpDDSprites, NULL );
		if(lpDDSprites==0)
			ErrorMsg(LSZ_DD_CANTCREATESPR,LSZ_DD_INIT);
		ClearSurface(lpDDSprites,0);

		memset(&ddsd,0,sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;// | DDSD_PIXELFORMAT;
		ddsd.dwWidth=640;
		ddsd.dwHeight=480;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|VORM;
		lpDD->CreateSurface( &ddsd,&lpDDSBuffer, NULL );
		if(lpDDSBuffer==0)
			ErrorMsg(LSZ_DD_CANTCREATEBUFF,LSZ_DD_INIT);
	}
	else
	{
		lpDD->SetCooperativeLevel(hWndWin,DDSCL_NORMAL);
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		lpDD->CreateSurface( &ddsd,&lpDDSPrimary, NULL );
		if(lpDDSPrimary==0)
			ErrorMsg(LSZ_DD_CANTCREATESP,LSZ_DD_INIT);

		LPDIRECTDRAWCLIPPER lpClipper;
		lpDD->CreateClipper( 0, &lpClipper, NULL );
		if(lpClipper==0)
			ErrorMsg(LSZ_DD_CANTCREATECLIP,LSZ_DD_INIT);

		lpClipper->SetHWnd( 0, hWndWin );
		lpDDSPrimary->SetClipper( lpClipper );
		lpClipper->Release();

		memset(&ddsd,0,sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.dwWidth=640;
		ddsd.dwHeight=480;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | VORM1;
		lpDD->CreateSurface( &ddsd,&lpDDSBack, NULL );
		if(lpDDSBack==0)
			ErrorMsg(LSZ_DD_CANTCREATESS,LSZ_DD_INIT);

		memset(&ddsd,0,sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;// | DDSD_PIXELFORMAT;
		ddsd.dwWidth=SPR_SIZEX;
		ddsd.dwHeight=SPR_SIZEY;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | VORM1;
		lpDD->CreateSurface( &ddsd, &lpDDSprites, NULL );
		if(lpDDSprites==0)
			ErrorMsg(LSZ_DD_CANTCREATESPR,LSZ_DD_INIT);

		memset(&ddsd,0,sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;// | DDSD_PIXELFORMAT;
		ddsd.dwWidth=640;
		ddsd.dwHeight=480;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | VORM1;
		lpDD->CreateSurface( &ddsd,&lpDDSBuffer, NULL );
		if(lpDDSBuffer==0)
			ErrorMsg(LSZ_DD_CANTCREATEBUFF,LSZ_DD_INIT);

		HDC hDC;
		if(DD_OK==lpDDSBack->GetDC(&hDC))
		{
			DEFAULT_FONT=NewFont(hDC,DEFAULT_FONTFACE,POINT_SIZE);
			DEFAULT_SCORE_FONT=NewFont(hDC,DEFAULT_SCORE_FONTFACE,SCORE_POINT_SIZE);
			lpDDSBack->ReleaseDC(hDC);
		}
	}
	DDSetColorKey(lpDDSBack,RGB(255,0,255));

#ifndef DX3
	lpDDSprites->PageLock(0);
	lpDDSBuffer->PageLock(0);
	Buffer_pagelock_cnt=Sprites_pagelock_cnt=1;

	if((!Settings.DBGMODE)&&bClrCtrl)
	{
		lpDDSPrimary->QueryInterface(IID_IDirectDrawColorControl,(void**)&lpDDCC);
		if(lpDDCC!=0)
		{
			DDCOLORCONTROL ddcc;
			OutFile("DirectDrawColorControl OK");
			ddcc.dwSize=sizeof(DDCOLORCONTROL);
			lpDDCC->GetColorControls(&ddcc);
			ddcc.dwFlags|=DDCOLOR_BRIGHTNESS|DDCOLOR_CONTRAST;
			ddcc.lBrightness=Settings.Brightness;
			ddcc.lContrast=Settings.Contrast;
			lpDDCC->SetColorControls(&ddcc);
		}
		else
			OutFile(LSZ_DD_NOCC);
	}
#endif

	AllocScoreSurface();
	ddsd.dwSize=sizeof(ddsd);
	memset(&ddsd,0,sizeof(ddsd));
	lpDD->GetDisplayMode(&ddsd);
	RefreshRate=ddsd.dwRefreshRate;
	if(RefreshRate==0)
		if(i=(int)strstr(szCmdLine,"-RATE"))
			RefreshRate=atoi((char*)i+5);

	if(RefreshRate==0)
		RefreshRate=100;

	DDCAPS HALcaps;
	DDCAPS HELcaps;
	HALcaps.dwSize=sizeof(DDCAPS);
	HELcaps.dwSize=sizeof(DDCAPS);
	lpDD->GetCaps(&HALcaps,&HELcaps);
	return;
}

int GetMaxNameLength(void)
{
	int ml=0;
	int l;
	for(int c=0;c<NumPlayers;c++)
		if((l=strlen(Bombers[c].Nom))>ml)
			ml=l;

	return ml;
}

void AllocScoreSurface(void)
{
	if(lpDDSBack==NULL)
		return;
	int ml=GetMaxNameLength();

	MaxNameLength=ml;
	ml=ml>12?ml:12;
	if(lpDDSScore!=0)
		lpDDSScore->Release();
	lpDDSScore=NULL;

	XperScore=X_PER_SCORE;
	YperScore=Y_PER_SCORE;
#ifndef DX3
	DDSURFACEDESC2 ddsd;
#else
	DDSURFACEDESC ddsd;
#endif
	HDC hdcb;
	TEXTMETRIC tm;
	if(DD_OK==lpDDSBack->GetDC(&hdcb))
	{
		AttachFont(hdcb,DEFAULT_SCORE_FONT);
		if(GetTextMetrics(hdcb,&tm)!=0)
		{
			XperScore=(ml+1)*tm.tmAveCharWidth;
			YperScore=4*tm.tmHeight;
		}
		lpDDSBack->ReleaseDC(hdcb);
	}

	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;// | DDSD_PIXELFORMAT;
	ddsd.dwWidth=XperScore;
	if(!Settings.GO.TEAMPLAY)
		ddsd.dwHeight=YperScore*NumPlayers;
	else
		ddsd.dwHeight=YperScore*GetNbEntities();
	if(ddsd.dwHeight==0)
		return;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | VORM1;
	lpDD->CreateSurface( &ddsd,&lpDDSScore, NULL );
	if(lpDDSScore==0)
		ErrorMsg(LSZ_DD_CANTCREATESC,LSZ_DD_INIT);
	DDSetColorKey(lpDDSScore,RGB(0,0,0));
	bScoreRedraw=TRUE;
	return;
}

void GereLastFirst(void)
{
	struct BomberScore plt[MAX_PLAYERS+NUM_TEAMS];
	int ns=0;

	if(!Settings.GO.TEAMPLAY)
	{
		for(i=0;i<NumPlayers;i++)
		{
			strcpy(plt[i].Nom,Bombers[i].Nom);
			plt[i].Frag=Bombers[i].Frag;
			plt[i].AK=Bombers[i].AK;
			plt[i].Parties=Bombers[i].Parties;
			ns++;
		}
	}
	else
	{
		for(i=0;i<NumPlayers;i++)
			if(Bombers[i].Team==VOID_TEAM)
			{
				strcpy(plt[ns].Nom,Bombers[i].Nom);
				plt[ns].Frag=Bombers[i].Frag;
				plt[ns].AK=Bombers[i].AK;
				plt[ns].Parties=Bombers[i].Parties;
				ns++;
			}

		for(int ix=1;ix<NUM_TEAMS+1;ix++)
			if(GetTeamPop(ix)>0)
			{
				strcpy(plt[ns].Nom,TeamName[ix]);
				plt[ns].Frag=TFRAGS(ix);
				plt[ns].AK=TAK(ix);
				plt[ns].Parties=TPARTIES(ix);
				ns++;
			}
	}
	if(ns==0)
		return;

	int ms,idx;
	DWORD mf;
	ms=SCSPOINTS(plt[0]);
	mf=plt[0].Frag;
	idx=0;

	for(i=0;i<ns;i++)
		if((SCSPOINTS(plt[i])>ms)
			||((SCSPOINTS(plt[i])==ms)
			&&(plt[i].Frag>mf)))
		{
			ms=SCSPOINTS(plt[i]);
			mf=plt[i].Frag;
			idx=i;
		}

	if(strcmp(plt[idx].Nom,szLastFirst))
	{
		strcpy(szLastFirst,plt[idx].Nom);
		int bp=SCSPOINTS(plt[idx]);
		if(bp!=1)
			sprintf(Buff,LSZ_ldWONWITHldPTS,plt[idx].Nom,bp);
		else
			sprintf(Buff,LSZ_ldWONWITHldPT,plt[idx].Nom,bp);
		AddMessage(Buff,2500,0);//GetBomberColor(idx));
	}
	return;
}

int GetTeamPop(BYTE team)
{
	if(!Settings.GO.TEAMPLAY)
		return 0;
	int pop=0;
	for(int vi=0;vi<NumPlayers;vi++)
		if((Bombers[vi].Etat==B_PLAYING)&&(Bombers[vi].Team==team))
			pop++;
	return pop;
}

int TPOINTS(BYTE team)
{
	int pnt=0;
	if(team==VOID_TEAM)
		return 0;
	for(int bc=0;bc<NumPlayers;bc++)
		if((Bombers[bc].Etat==B_PLAYING)&&(Bombers[bc].Team==team))
			pnt+=BPOINTS(bc);
	return pnt;
}

int TFRAGS(BYTE team)
{
	int pnt=0;
	if(team==VOID_TEAM)
		return 0;
	for(int bc=0;bc<NumPlayers;bc++)
		if((Bombers[bc].Etat==B_PLAYING)&&(Bombers[bc].Team==team))
			pnt+=Bombers[bc].Frag;
	return pnt;
}

int TAK(BYTE team)
{
	int pnt=0;
	if(team==VOID_TEAM)
		return 0;
	for(int bc=0;bc<NumPlayers;bc++)
		if((Bombers[bc].Etat==B_PLAYING)&&(Bombers[bc].Team==team))
			pnt+=Bombers[bc].AK;
	return pnt;
}

int TPARTIES(BYTE team)
{
	DWORD p=0;
	if(team==VOID_TEAM)
		return 0;
	for(int bc=0;bc<NumPlayers;bc++)
		if((Bombers[bc].Etat==B_PLAYING)&&(Bombers[bc].Team==team))
			p=max(p,Bombers[bc].Parties);
	return p;
}

void GereAffScore()
{
	if(GetAsyncKState(KEY_CODE(DIK_TAB,VK_TAB)))
	{
		bScoring=TRUE;
		bUpdateCol[0]=GetUpdateCount();
		int sc=int(400.0*dt);
		if(AffScoreX<XperScore-sc)
			AffScoreX+=sc;
		else
			AffScoreX=XperScore;
	}
	else
	{
		if(AffScoreX>0)
			for(int c=0;c<=XperScore/32;c++)
				bUpdateCol[c]=GetUpdateCount();
		if(!bScoring)
		{
			int sc=int(400.0*dt);
			if(AffScoreX>=sc)
				AffScoreX-=sc;
			else
				AffScoreX=0;
		}
		bScoring=0;
	}

	if((AffScoreX>0)&&(lpDDSScore!=NULL)&&(lpDDSBack!=NULL))
	{
		RECT txtRect;
		if(bScoreRedraw)
		{
			struct BomberScore plt[MAX_PLAYERS+NUM_TEAMS],TmpSC;
			int ns=0;

			ClearSurface(lpDDSScore,RGB(0,0,0));
			if(!Settings.GO.TEAMPLAY)
			{
				for(i=0;i<NumPlayers;i++)
				{
					strcpy(plt[i].Nom,Bombers[i].Nom);
					plt[i].Frag=Bombers[i].Frag;
					plt[i].AK=Bombers[i].AK;
					plt[i].Parties=Bombers[i].Parties;
					ns++;
				}
			}
			else
			{
				for(i=0;i<NumPlayers;i++)
					if(Bombers[i].Team==VOID_TEAM)
					{
						strcpy(plt[ns].Nom,Bombers[i].Nom);
						plt[ns].Frag=Bombers[i].Frag;
						plt[ns].AK=Bombers[i].AK;
						plt[ns].Parties=Bombers[i].Parties;
						ns++;
					}

				for(int ix=1;ix<NUM_TEAMS+1;ix++)
					if(GetTeamPop(ix)>0)
					{
						strcpy(plt[ns].Nom,TeamName[ix]);
						plt[ns].Frag=TFRAGS(ix);
						plt[ns].AK=TAK(ix);
						plt[ns].Parties=TPARTIES(ix);
						ns++;
					}
			}

			if(ns>1)
			{
				BOOL Xchg;
				do
				{
					Xchg=0;
					for(i=0;i<ns-1;i++)
						if((SCSPOINTS(plt[i+1])>SCSPOINTS(plt[i]))
							||((SCSPOINTS(plt[i+1])==SCSPOINTS(plt[i]))
							&&(plt[i+1].Frag>plt[i].Frag)))
						{
							Xchg=TRUE;
							TmpSC=plt[i+1];
							plt[i+1]=plt[i];
							plt[i]=TmpSC;
						}

				}
				while(Xchg);
			}

			memset(Buff,0,BMAX_PATH);
			for(i=0;i<ns;i++)
			{
				sprintf(Buff1,"%s\n%ld Points\n  %ld K/%ld AK\n\n",plt[i].Nom,SCSPOINTS(plt[i]),plt[i].Frag,plt[i].AK);
				strcat(Buff,Buff1);
			}
			HDC		hdc;

			if(DD_OK==lpDDSScore->GetDC(&hdc))
			{
				HBRUSH hb=0,hbr=0;
				AttachFont(hdc,DEFAULT_SCORE_FONT);
				hbr=CreateSolidBrush(RGB(0,0,128));
				hb=(HBRUSH)SelectObject(hdc,hbr);
				RoundRect(hdc,0,0,XperScore,YperScore*ns,40,40);
				SelectObject(hdc,hb);
				DeleteObject(hbr);

				SetBkMode(hdc,TRANSPARENT);
				SetTextColor(hdc, RGB(0,128,0));
				SetBkColor(hdc, RGB(0,0,0));

				txtRect.left=0;
				txtRect.top=Y_SCORE_BIAS;
				txtRect.right=XperScore;
				txtRect.bottom=YperScore*ns+Y_SCORE_BIAS;
				DrawText(hdc,Buff,-1,&txtRect,DT_CENTER|DT_NOPREFIX);

				lpDDSScore->ReleaseDC(hdc);
				bScoreRedraw=0;
			}
		}

		txtRect.left=XperScore-AffScoreX;
		txtRect.top=0;
		txtRect.right=XperScore;
		if(!Settings.GO.TEAMPLAY)
			txtRect.bottom=YperScore*NumPlayers;
		else
			txtRect.bottom=YperScore*GetNbEntities();
		lpDDSBack->BltFast(0,(480-txtRect.bottom)>>1,lpDDSScore,&txtRect,dwBltFastCK|BLITFAST_WAIT);
	}
	else
		bScoreRedraw=TRUE;

	return;
}

/*
void BltBlocs(void)
{
	if((lpDDSBack==NULL)||(lpDDSprites==NULL))
		return;
	for(i=0;i<20;i++)
		for(j=0;j<15;j++)
		if(Blocs[i][j].Type==CAISSE)
		{
			k=Blocs[i][j].ExplLevel;
			rect.top=CAISSEY;
			rect.left=32*k+CAISSEX;
			rect.right=32*k+CAISSEX+32;
			rect.bottom=CAISSEY+32;
			lpDDSBack->BltFast(i*32,j*32,lpDDSprites,&rect,DDBLTFAST_NOCOLORKEY | BLITFAST_WAIT);
		}


	return;
}
*/

BOOL InitApplication()
{
	WNDCLASSEX  wcex;

	wcex.cbSize=sizeof(WNDCLASSEX);
	wcex.style=CS_NOCLOSE;
	wcex.lpfnWndProc=(WNDPROC)WndProc;
	wcex.cbClsExtra=0;
	wcex.cbWndExtra=0;
	wcex.hInstance=hInst;
	wcex.hIcon=LoadIcon(GetModuleHandle(0),MAKEINTRESOURCE(IDI_ICON_MARBLE));
	wcex.hIconSm=wcex.hIcon;
	wcex.hCursor=LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground=(HBRUSH)COLOR_WINDOW;

	wcex.lpszMenuName=NULL;
	wcex.lpszClassName=szAppName;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(int nCmdShow)
{
	if(hWndWin)
		return FALSE;

	if(!bDedicated)
	{
		hWndWin = CreateWindowEx((Settings.DBGMODE? 0:WS_EX_TOPMOST)|WS_EX_APPWINDOW,
			szAppName,
			szTitle,
			WS_VISIBLE|WS_POPUP,
			Window.left=(GetSystemMetrics(SM_CXSCREEN)-640)>>1,
			Window.top=(GetSystemMetrics(SM_CYSCREEN)-480)>>1,
			640,
			480,
			NULL,
			NULL,
			hInst,
			NULL );

		ShowWindow(hWndWin, nCmdShow);
		UpdateWindow(hWndWin);
		SetFocus(hWndWin);
		SetActiveWindow(hWndWin);
		
		Window.bottom=Window.top+480;
		Window.right=Window.left+640;

	}
	else
	{
		hWndWin = CreateWindowEx(WS_EX_APPWINDOW,
			szAppName,
			szTitle,
			WS_BORDER|WS_CAPTION|
			WS_SYSMENU|WS_MINIMIZEBOX|WS_MINIMIZE,
			Window.left=(GetSystemMetrics(SM_CXSCREEN)-128)>>1,
			Window.top=(GetSystemMetrics(SM_CYSCREEN)-64)>>1,
			128,
			64,
			NULL,
			NULL,
			hInst,
			NULL );
	
		Window.bottom=Window.top+64;
		Window.right=Window.left+128;
	}

	
   return hWndWin!=0;
}

void DeleteChatChar(void)
{
	DWORD len=strlen(ChatText);
	if(len>0)
	{
		ChatText[len-1]='\0';
	}
	return;
}

void AddChatChar(int add_c)
{
	if(add_c>255)
		return;
	char sc[2];
	sc[0]=(char)add_c;
	sc[1]='\0';
	strcat(ChatText,sc);

/*	if(isalnum(add_c)||(add_c==' '))
	{
		bUpdateAllScreen=GetUpdateCount();
	}
*/
	return;
}

void ShowChatText(void)
{
	if(!bChat)
		return;
	char szChatText[BMAX_PATH];
	sprintf(szChatText,"%s: %s",Bombers[ChatNum].Nom,ChatText);
	DWORD len=strlen(szChatText);

	HDC hDC;
	lpDDSBack->GetDC(&hDC);
	SetBkMode(hDC,TRANSPARENT);
	SetBkColor(hDC, RGB(0,0,0));
	AttachFont(hDC,DEFAULT_FONT);
	RECT r;
	r.left=1;
	r.top=399;
	r.right=641;
	r.bottom=479;
	SetTextColor(hDC, GetBomberColor(ChatNum));
	DrawText(hDC,szChatText,len,&r,DT_CENTER|DT_SINGLELINE|DT_NOCLIP|DT_BOTTOM|DT_NOPREFIX);

	r.left=0;
	r.top=400;
	r.right=640;
	r.bottom=480;
	SetTextColor(hDC, 0);
	DrawText(hDC,szChatText,len,&r,DT_CENTER|DT_SINGLELINE|DT_NOCLIP|DT_BOTTOM|DT_NOPREFIX);
	lpDDSBack->ReleaseDC(hDC);
	return;
}

void GereChatMessages(void)
{
	if(!bConnected)
		return;
	if(!bChat)
	{
		if(((NumPlayers>0)&&!bConnected)||IsLocalPlayer(0))
			if(GetAsyncKState(KEY_CODE(DIK_1,'1')))
			{
				strcpy(ChatText,"\0");
				bChat=bChatFirst=TRUE;
				ChatNum=0;
			}
		
		if(((NumPlayers>1)&&!bConnected)||IsLocalPlayer(1))
			if(GetAsyncKState(KEY_CODE(DIK_2,'2')))
			{
				strcpy(ChatText,"\0");
				bChat=bChatFirst=TRUE;
				ChatNum=1;
			}
			
		if(((NumPlayers>2)&&!bConnected)||IsLocalPlayer(2))
			if(GetAsyncKState(KEY_CODE(DIK_3,'3')))
			{
				strcpy(ChatText,"\0");
				bChat=bChatFirst=TRUE;
				ChatNum=2;
			}
			
		if(((NumPlayers>3)&&!bConnected)||IsLocalPlayer(3))
			if(GetAsyncKState(KEY_CODE(DIK_4,'4')))
			{
				strcpy(ChatText,"\0");
				bChat=bChatFirst=TRUE;
				ChatNum=3;
			}
	}
	return;
}

void GereTextMessages(void)
{
	if(!bConnected||(NumPlayers<=1)) return;
	if((GetAsyncKState(KEY_CODE(DIK_F1,VK_F1)))&&(LastF1==0))
		ChatMessage(FROM_FIRSTLOCALPLAYER,-1,Settings.TM.F1TxtMsg,TRUE);
	LastF1=GetAsyncKState(KEY_CODE(DIK_F1,VK_F1));

	if((GetAsyncKState(KEY_CODE(DIK_F2,VK_F2)))&&(LastF2==0))
		ChatMessage(FROM_FIRSTLOCALPLAYER,-1,Settings.TM.F2TxtMsg,TRUE);
	LastF2=GetAsyncKState(KEY_CODE(DIK_F2,VK_F2));

	if((GetAsyncKState(KEY_CODE(DIK_F3,VK_F3)))&&(LastF3==0))
		ChatMessage(FROM_FIRSTLOCALPLAYER,-1,Settings.TM.F3TxtMsg,TRUE);
	LastF3=GetAsyncKState(KEY_CODE(DIK_F3,VK_F3));

	if((GetAsyncKState(KEY_CODE(DIK_F4,VK_F4)))&&(LastF4==0))
		ChatMessage(FROM_FIRSTLOCALPLAYER,-1,Settings.TM.F4TxtMsg,TRUE);
	LastF4=GetAsyncKState(KEY_CODE(DIK_F4,VK_F4));

	if((GetAsyncKState(KEY_CODE(DIK_F5,VK_F5)))&&(LastF5==0))
		ChatMessage(FROM_FIRSTLOCALPLAYER,-1,Settings.TM.F5TxtMsg,TRUE);
	LastF5=GetAsyncKState(KEY_CODE(DIK_F5,VK_F5));

	if((GetAsyncKState(KEY_CODE(DIK_F6,VK_F6)))&&(LastF6==0))
		ChatMessage(FROM_FIRSTLOCALPLAYER,-1,Settings.TM.F6TxtMsg,TRUE);
	LastF6=GetAsyncKState(KEY_CODE(DIK_F6,VK_F6));

	if((GetAsyncKState(KEY_CODE(DIK_F7,VK_F7)))&&(LastF7==0))
		ChatMessage(FROM_FIRSTLOCALPLAYER,-1,Settings.TM.F7TxtMsg,TRUE);
	LastF7=GetAsyncKState(KEY_CODE(DIK_F7,VK_F7));

	if((GetAsyncKState(KEY_CODE(DIK_F8,VK_F8)))&&(LastF8==0))
		ChatMessage(FROM_FIRSTLOCALPLAYER,-1,Settings.TM.F8TxtMsg,TRUE);
	LastF8=GetAsyncKState(KEY_CODE(DIK_F8,VK_F8));

	return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CHAR:
		if(!bChat)
			break;
		if(bChatFirst)
		{
			bChatFirst=FALSE;
			break;
		}
		char c;
		c=(TCHAR)wParam;
		if(c=='\b')
		{
			DeleteChatChar();
			bUpdateRow[14]=GetUpdateCount();
		}
		else
		if(c==13)
		{
			if(strlen(ChatText))
			{
				ChatMessage(ChatNum,-1,ChatText,TRUE);
			}
			strcpy(ChatText,"\0");
			bUpdateRow[14]=GetUpdateCount();
			bChat=FALSE;
		}
		else
		{
			AddChatChar(c);
			bUpdateRow[14]=GetUpdateCount();
		}
		break;

	case WM_KEYUP:
		if(bDemoMode)
			PostQuitMessage(0);
		break;

	case WM_ACTIVATE:
		if ((0!=HIWORD(wParam))||(WA_INACTIVE==LOWORD(wParam)))
		{
			//OutFile("WA_INACTIVATE");
#ifndef DX3
			if(lpDDSBuffer&&Buffer_pagelock_cnt)
			{
				lpDDSBuffer->PageUnlock(0);
				Buffer_pagelock_cnt--;
			}
			if(lpDDSprites&&Sprites_pagelock_cnt)
			{
				lpDDSprites->PageUnlock(0);
				Sprites_pagelock_cnt--;
			}
#endif
			if(!Settings.DBGMODE)
				SetPause();
			bCheckRestore=0;

		}
		else
		{
			//OutFile("WA_ACTIVATE");
			bCheckRestore=TRUE;
		}
		break;
/*
	case WM_ACTIVATEAPP:
		if (!(BOOL)wParam)
		{
			OutFile("WA_INACTIVATEAPP");
			if(!bPause&&!bConnected)
			{
				bPause=TRUE;
				StartPauseTick=TickCount();
			}
			bCheckRestore=0;
		}
		else
		{
			OutFile("WA_ACTIVATEAPP");
			bCheckRestore=TRUE;
		}
		break;
*/
	case WM_DISPLAYCHANGE:
		TryRestoreAll();
		break;

	case WM_PAINT:
		PAINTSTRUCT	pst;
		BeginPaint(hWndWin,&pst);
		EndPaint(hWndWin,&pst);
		//OutFile("WM_PAINT");
		bUpdateAllScreen=GetUpdateCount();
		if(bCheckRestore)
			TryRestoreAll();

#ifndef DX3
		if(lpDDSBuffer&&(Buffer_pagelock_cnt==0))
		{
			lpDDSBuffer->PageLock(0);
			Buffer_pagelock_cnt++;
		}
		if(lpDDSprites&&(Sprites_pagelock_cnt==0))
		{
			lpDDSprites->PageLock(0);
			Sprites_pagelock_cnt++;
		}
#endif
		break;


		case WM_CLOSE:
			OutFile("WM_CLOSE");
			PostQuitMessage(0);
		break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
void ReInitDDraw()
{
	FinDDraw();
	InitDDraw();
	LoadSprites();
	updBuff.top=updBuff.left=0;
	updBuff.right=20;
	updBuff.bottom=15;
	UpdateBuffer(updBuff);
	bScoreRedraw=TRUE;
	bUpdateAllScreen=GetUpdateCount();
	bCheckRestore=FALSE;
	return;
}

void RestoreSurfaces()
{
	if(bDedicated)
		return;
if(lpDDSPrimary!=0)
	if(lpDDSPrimary->IsLost()==DDERR_SURFACELOST)
	{
		bCheckRestore=TRUE;
		int hr;
#ifndef DX3
		hr=lpDD->RestoreAllSurfaces();
		if(hr!=DD_OK)
			if(Settings.DBGMODE)
			{
				ReInitDDraw();
				return;
			}
			else
				return;
#else
		if(lpDDSBack!=0)
			hr=lpDDSBack->Restore();
		if(lpDDSPrimary!=0)
			hr+=lpDDSPrimary->Restore();
		if(lpDDSBuffer!=0)
			hr+=lpDDSBuffer->Restore();
		if(lpDDSScore!=0)
			hr+=lpDDSScore->Restore();
		if(lpDDSprites!=0)
			hr+=lpDDSprites->Restore();
#endif

		if((hr!=DD_OK)&&(hr!=DDERR_IMPLICITLYCREATED))
			if(Settings.DBGMODE)
			{
				ReInitDDraw();
				return;
			}
			else
				return;

		LoadSprites();
		updBuff.top=updBuff.left=0;
		updBuff.right=20;
		updBuff.bottom=15;
		UpdateBuffer(updBuff);
		bUpdateAllScreen=GetUpdateCount();
		bScoreRedraw=TRUE;
		bCheckRestore=FALSE;
	}
	else
		bCheckRestore=FALSE;
	return;
}

void TryRestoreAll()
{
	if(lpDIDevice!=0)
		lpDIDevice->Acquire();

	if(bDedicated)
		return;
#ifndef DX3
	if(lpDD!=0)
	{
		HRESULT hr=lpDD->TestCooperativeLevel();
		if(hr!=DD_OK)
			bCheckRestore=TRUE;
		else
			bCheckRestore=0;

		if(Settings.DBGMODE&&(hr==DDERR_WRONGMODE))
			ReInitDDraw();

		if(Settings.DBGMODE&&(hr==DDERR_EXCLUSIVEMODEALREADYSET))
			While(DD_OK!=lpDD->TestCooperativeLevel())
				Sleep(SLEEP_TIME);

		if(hr!=0)
		{
			if(DD_OK==lpDD->TestCooperativeLevel())
				ReInitDDraw();
		}
		else
			RestoreSurfaces();
		return;
	}
	return;
#endif
	RestoreSurfaces();
	return;
}

void GereStats(void)
{
	if(!bDedicated||!bConnected)
		return;
	if(TickCount()-LastGereStats<STATS_TOUT)
		return;
	LastGereStats=TickCount();
	FILE* Freq=fopen("req","rt");

	if(Freq==NULL)
		return;

	char sname[BMAX_NAME];
	if(fread(sname,1,BMAX_NAME,Freq)==0)
	{
		fclose(Freq);
		return;
	}
	fclose(Freq);

	if(strstr(sname,szSessionName)==0)
		return;

	remove("req");
	FILE* Fans=fopen("ans","wt");
	if(Fans==NULL)
		return;
	fprintf(Fans,"Nom de la Session: %s\n",szSessionName);
	fprintf(Fans,"Nombre de joueurs: %ld\n",NumPlayers);
	for(i=0;i<NumPlayers;i++)
	{
		fprintf(Fans,"%s\n",Bombers[i].Nom);
		fprintf(Fans,"Frags: %ld\n",Bombers[i].Frag);
		fprintf(Fans,"AK: %ld\n",Bombers[i].AK);
		fprintf(Fans,"Points: %ld\n",BPOINTS(i));
	}
	fclose(Fans);
	return;
}

void CleanUp(BOOL shwScores)
{
	OutFile("Cleaning up...");
	remove("req");
	remove("ans");
	DisableConsole();
	OutFile("DestroyAllVoiceStacks...");
	DestroyAllVoiceStacks();
	Temporize();
	OutFile("StopVoiceCapture...");
	StopVoiceCapture();
	Temporize();
	OutFile("FinVoiceCapture...");
	FinVoiceCapture();
	Temporize();
	OutFile("FinSon...");
	FinSon();
	Temporize();
	OutFile("CloseFileLoader...");
	CloseFileLoader();
	Temporize();
	OutFile("FinDPlay...");
	FinDPlay();
	Temporize();
	OutFile("FinDInput...");
	FinDInput();
	Temporize();
	OutFile("FinDDraw...");
	FinDDraw();
	Temporize();

	//ShowWindow(hWndWin,SW_HIDE);
	DestroyWindow(hWndWin);
	hWndWin=0;

if(shwScores)
{
	ShowScores();

//  MAIL !!!!!!!!!!!!!
/*	if(strstr(szCmdLine,"-MAIL")!=0)
		SendMail();*/
}
	for(i=0;i<MAX_SCORES;i++)
		memset(&Score[i],0,sizeof(struct BomberScore));

	return;
}

void FinDPlay(void)
{
	CloseMulti();
	
	OutFile("Closing DP");
	if (lpDP!=0)
		lpDP->Close();

	OutFile("Releassing DP");
	SAFE_RELEASE(lpDP);
	SAFE_RELEASE(lpDPL);
	return;
}

void FinDInput(void)
{
	if(lpDIDevice!=0)
		lpDIDevice->Unacquire();

	SAFE_RELEASE(lpDIDevice)
	SAFE_RELEASE(lpDI)

	return;
}

void FinDDraw(void)
{
#ifndef DX3
	if(lpDDSBuffer!=NULL)
		lpDDSBuffer->PageUnlock(0);
	if(lpDDSprites!=NULL)
		lpDDSprites->PageUnlock(0);
#endif
	SAFE_RELEASE(lpDDSBuffer)
	SAFE_RELEASE(lpDDSprites)
	SAFE_RELEASE(lpDDSBack)
	SAFE_RELEASE(lpDDSScore)
	SAFE_RELEASE(lpDDSPrimary)

#ifndef DX3
	SAFE_RELEASE(lpDDCC)
#endif

	ReleaseDDObject();

	DeleteFont(DEFAULT_SCORE_FONT);
	DeleteFont(DEFAULT_FONT);
	return;
}

void FinalShutdown(void)
{
//	SetSysColors(NumSysColors,SysID,oldSysColors);
//	CleanUp(0);
	FT_Free();
#ifndef DX3
	SAFE_RELEASE(lpDPL);
#endif
	UninitIA();
	OutFileEx2(LSZ_RUNFOR,TickCount()-StartTime);
	OutFileLF();
	EndOutFile();
	CoUninitialize();
//	delete wsDP;
	return;
}

void ClearSurface(const void* vpdds,DWORD color)
{
#ifndef DX3
	LPDIRECTDRAWSURFACE4 pdds=(LPDIRECTDRAWSURFACE4)vpdds;
#else
	LPDIRECTDRAWSURFACE2 pdds=(LPDIRECTDRAWSURFACE2)vpdds;
#endif
	if(pdds==NULL)
		return;
	DDBLTFX ddbltfx;
	ddbltfx.dwSize=sizeof(DDBLTFX);
	ddbltfx.dwFillColor=color;
	pdds->Blt(0,0,0,DDBLT_WAIT|DDBLT_COLORFILL,&ddbltfx);
return;
}

void SetDefaultBlocs(void)
{
	for(i=0;i<20;i++)
		for(j=0;j<15;j++)
			Blocs[i][j].Type=SOL;
		
	for(i=1;i<10;i+=2)
		for(j=1;j<15;j+=2)
			Blocs[i][j].Type=MUR;
			
	for(i=10;i<20;i+=2)
		for(j=1;j<15;j+=2)
			Blocs[i][j].Type=MUR;
				return;
}

void LoadMapFile(const char* szCMap)
{
	char dn[BMAX_PATH];
	SetDefaultStartPos(&Settings.GO);

	if(szCMap[0]!='\0')
		sprintf(dn,"Maps\\%s\\BMap.txt",szCMap);
	else
		strcpy(dn,"BMap.txt");

	LoadRootFile(dn);
	FILE*F=fopen(dn,"rt");
	if(F!=0)
	{
		char c=0;
		BOOL br=0;
		for(j=0;j<15;j++)
			for(i=0;i<20;i++)
			{
				if(c==EOF)
				{
					if(!br)
					{
						br=TRUE;
						SetDefaultBlocs();
					}

					if(szCMap[0]!='\0')
						sprintf(Buff,LSZ_ERRINsMAP,szCMap);
					else
						strcpy(Buff,LSZ_ERRINDEFMAP);
					AddMessage(Buff,2500,RGB(0,0,200));
				}
				else
				{
					do
					{
						c=fgetc(F);
						if(c==';')
							fgets(Buff,BMAX_PATH,F);
					}
					while((c==10)||(c==13)||(c==';'));
					Blocs[i][j].Type=((c=='x')||(c=='X'))?MUR:SOL;
					if((c>='1')&&(c<='0'+MAX_PLAYERS))
					{
						Settings.GO.StartPos[2*(c-'1')]=i;
						Settings.GO.StartPos[2*(c-'1')+1]=j;
					}
				}
			}

		fclose(F);
	}
	else
	{
		if(szCMap[0]!='\0')
			sprintf(Buff,LSZ_CANTOPENsMAP,szCMap);
		else
			strcpy(Buff,LSZ_CANTOPENDEFMAP);
		AddMessage(Buff,2500,RGB(0,0,200));
		SetDefaultBlocs();
	}
	UnloadFile(dn);
	return;
}

void InitBlocs(int NBl)
{
	for(i=0;i<20;i++)
		for(j=0;j<15;j++)
		{
			Blocs[i][j].TypeBonus=0;
			Blocs[i][j].Type=SOL;
		}

	LoadMapFile(szCurMap);
	for(i=0;i<20;i++)
		for(j=0;j<15;j++)
			if(Blocs[i][j].Type!=MUR)
			{
				BOOL tp=FALSE;
				for(int p=0;p<MAX_PLAYERS;p++)
					if(abs(i-Settings.GO.StartPos[2*p])+abs(j-Settings.GO.StartPos[2*p+1])<C_LIBRE)
						tp=TRUE;

				if((rand()%99<=NBl-1)&&!tp)
				{
					Blocs[i][j].Type=CAISSE;
					Blocs[i][j].TypeBonus=ChooseBonus();
				}
				else
					Blocs[i][j].Type=SOL;

				Blocs[i][j].TypeSol=GetTypeSol(i,j);
				Blocs[i][j].Bloque=0;
			}

	SetAllBlocsSol();
	return;
}

void SetAllBlocsSol(void)
{
	for(i=0;i<20;i++)
		for(j=0;j<15;j++)
			if(Blocs[i][j].Type!=MUR)
				Blocs[i][j].TypeSol=GetTypeSol(i,j);
	return;
}

int GetTypeSol(int t1,int t2)
{
	BYTE b0,b1,b2,b3;
	BYTE index;
	int convert[]={
		15, //0
		12, //1
		13, //2
		3,  //3
		14, //4
		5,  //5
		0,  //6
		9,  //7
		11, //8
		2,  //9
		4,  //10
		8,  //11
		1,  //12
		10, //13
		7,  //14
		6   //15
	};

	//Sens des aiguilles d'une montre
	if(t2==0)
		b0=0;
	else
		b0=Blocs[t1][t2-1].Type!=MUR?1:0;

	if(t1==19)
		b1=0;
	else
		b1=Blocs[t1+1][t2].Type!=MUR?1:0;

	if(t2==14)
		b2=0;
	else
		b2=Blocs[t1][t2+1].Type!=MUR?1:0;

	if(t1==0)
		b3=0;
	else
		b3=Blocs[t1-1][t2].Type!=MUR?1:0;

	index=(b3<<3)|(b2<<2)|(b1<<1)|b0;
	return convert[index];

	/*
	if((t1==0)&&(t2==0))
		v=0;
	else
	if((t1==19)&&(t2==0))
		v=1;
	else
	if((t1==19)&&(t2==14))
		v=2;
	else
	if((t1==0)&&(t2==14))
		v=3;
	else
	if(t1&1)
		v=4;
	else
	if((t2==0)&&!(t1&1))
		v=7;
	else
	if((t2==14)&&!(t1&1))
		v=8;
	else
	if(t2&1)
		v=5;
	else
	if((t1==0)&&!(t2&1))
		v=9;
	else
		v=6;
	return v;
	*/
}

void GetKeys(int Num)
{
//	if(Settings.USEDINPUT)
//	{
		i=GetAsyncKState(KEY_CODE(Settings.C.DIKey_LEFT[Num],Settings.C.Key_LEFT[Num]));
		if(i&&(LEFTlck[Num]==0))
		{
			LastXDir[Num]=-V*Bombers[Num].Reverse;
			LastYDir[Num]=0;
		}
		if(i)
			LEFTlck[Num]=TRUE;
		else LEFTlck[Num]=0;

		i=GetAsyncKState(KEY_CODE(Settings.C.DIKey_RIGHT[Num],Settings.C.Key_RIGHT[Num]));
		if(i&&(RIGHTlck[Num]==0))
		{
			LastXDir[Num]=V*Bombers[Num].Reverse;
			LastYDir[Num]=0;
		}
		if(i)
			RIGHTlck[Num]=TRUE;
		else RIGHTlck[Num]=0;

		i=GetAsyncKState(KEY_CODE(Settings.C.DIKey_UP[Num],Settings.C.Key_UP[Num]));
		if(i&&(UPlck[Num]==0))
		{
			LastYDir[Num]=-V*Bombers[Num].Reverse;
			LastXDir[Num]=0;
		}
		if(i)
			UPlck[Num]=TRUE;
		else UPlck[Num]=0;

		i=GetAsyncKState(KEY_CODE(Settings.C.DIKey_DOWN[Num],Settings.C.Key_DOWN[Num]));
		if(i&&(DOWNlck[Num]==0))
		{
			LastYDir[Num]=V*Bombers[Num].Reverse;
			LastXDir[Num]=0;
		}
		if(i)
			DOWNlck[Num]=TRUE;
		else DOWNlck[Num]=0;

		i=GetAsyncKState(KEY_CODE(Settings.C.DIKey_FIRE1[Num],Settings.C.Key_FIRE1[Num]));
		if(((F1lck[Num]==0)||(TickCount()-F1GTC[Num]>CTRL_TOUT))&&i)
		{
			F1lck[Num]=TRUE;
			F1GTC[Num]=TickCount();
		}
		else F1lck[Num]=0;

		i=GetAsyncKState(KEY_CODE(Settings.C.DIKey_FIRE2[Num],Settings.C.Key_FIRE2[Num]));
		if((F2Down[Num]==TRUE)&&i)
			F2lck[Num]=0;
		else
		if(i)
		{
			F2lck[Num]=TRUE;
			F2Down[Num]=TRUE;
		}
		else if (i==0)
		{
			F2lck[Num]=0;
			F2Down[Num]=0;
		}

		i=GetAsyncKState(KEY_CODE(Settings.C.DIKey_FIRE3[Num],Settings.C.Key_FIRE3[Num]));
		if(((F3lck[Num]==0)||(TickCount()-F3GTC[Num]>CTRL_TOUT))&&i)
		{
			F3lck[Num]=TRUE;
			F3GTC[Num]=TickCount();
		}
		else F3lck[Num]=0;
	return;
}

void UpdateBuffer(RECT updrect)
{
	if((lpDDSBuffer==NULL)||(lpDDSprites==NULL))
		return;
	RECT urect;
	int x,y;
	for(x=updrect.left;x<updrect.right;x++)
		for(y=updrect.top;y<updrect.bottom;y++)
			if(Blocs[x][y].Type==MUR)
			{
				urect.top=MURY;
				urect.left=MURX;
				urect.right=MURX+32;
				urect.bottom=MURY+32;
				lpDDSBuffer->BltFast(x*32,y*32,lpDDSprites,&urect,DDBLTFAST_NOCOLORKEY|BLITFAST_WAIT);
			}
			else if(Blocs[x][y].Type==SOL)
			{
				k=Blocs[x][y].TypeSol;
				urect.left=32*k+SOLX;
				urect.right=32*k+SOLX+32;
				urect.top=SOLY;
				urect.bottom=SOLY+32;
				lpDDSBuffer->BltFast(x*32,y*32,lpDDSprites,&urect,DDBLTFAST_NOCOLORKEY|BLITFAST_WAIT);
				k=Blocs[x][y].TypeBonus;
				if (k!=0)
				{
					k--;
					urect.left=32*k+BONUSX;
					urect.right=32*k+BONUSX+32;
					urect.top=BONUSY;
					urect.bottom=BONUSY+32;
					lpDDSBuffer->BltFast(x*32,y*32,lpDDSprites,&urect,dwBltFastCK|BLITFAST_WAIT);
				}

			}
			else if(Blocs[x][y].Type==CAISSE)
			{
				k=Blocs[x][y].TypeSol;
				urect.left=32*k+SOLX;
				urect.right=32*k+SOLX+32;
				urect.top=SOLY;
				urect.bottom=SOLY+32;
				lpDDSBuffer->BltFast(x*32,y*32,lpDDSprites,&urect,DDBLTFAST_NOCOLORKEY|BLITFAST_WAIT);

				urect.left=CAISSEX;
				urect.right=CAISSEX+32;
				urect.top=CAISSEY;
				urect.bottom=CAISSEY+32;
				lpDDSBuffer->BltFast(x*32,y*32,lpDDSprites,&urect,dwBltFastCK|BLITFAST_WAIT);
			}

	return;
}

void InitBomber(int Num)
{
	Bombers[Num].X=float(Settings.GO.StartPos[Num*2]*32+16);
	Bombers[Num].Y=float(Settings.GO.StartPos[Num*2+1]*32+16);
	Bombers[Num].Sens=B_DROITE;
	Bombers[Num].LastGTC1=TickCount();
	Bombers[Num].Sprite=0;
	Bombers[Num].SimulBomb=Settings.GO.NBBOMB;
	Bombers[Num].NbBomb=Bombers[Num].SimulBomb;
	Bombers[Num].Blast=Settings.GO.NBBLAST;
	Bombers[Num].BType=BB_STD;
	Bombers[Num].Reverse=1;
	Bombers[Num].RetOpt=0;
	Bombers[Num].RigOpt=0;
	Bombers[Num].NbMine=Settings.GO.NBMINE;
	Bombers[Num].Color=Num;
	Bombers[Num].NetGTC=TickCount();
	Bombers[Num].Frag=0;
	Bombers[Num].AK=0;
	Bombers[Num].Parties=0;
	Bombers[Num].StartTime=0;
	Bombers[Num].oldSensX=0;
	Bombers[Num].oldSensY=0;
	strcpy(Bombers[Num].Nom,Settings.Nom[Num]);
	Bombers[Num].CmdCtrl=Num;
	memset(&BCtrl[Num],0,sizeof(ControlBomber));
	return;
}

void ResetBomber(int Num)
{
	Bombers[Num].X=float(Settings.GO.StartPos[Num*2]*32+16);
	Bombers[Num].Y=float(Settings.GO.StartPos[Num*2+1]*32+16);
	Bombers[Num].Sens=B_DROITE;
	Bombers[Num].LastGTC1=TickCount();
	Bombers[Num].Sprite=0;
	Bombers[Num].SimulBomb=Settings.GO.NBBOMB;
	Bombers[Num].NbBomb=Bombers[Num].SimulBomb;
	Bombers[Num].Blast=Settings.GO.NBBLAST;
	Bombers[Num].BType=BB_STD;
	Bombers[Num].Reverse=1;
	Bombers[Num].RetOpt=0;
	Bombers[Num].RigOpt=0;
	Bombers[Num].NbMine=Settings.GO.NBMINE;
	Bombers[Num].NetGTC=TickCount();
	Bombers[Num].Parties++;
	Bombers[Num].oldSensX=0;
	Bombers[Num].oldSensY=0;
	return;
}
void ExtrapoleBomber(int Num,float a,float b)
{
	float Ecart;
	if (Bombers[Num].Etat!=B_PLAYING) return;
	if(Bombers[Num].Sens==B_MORT) return;

	i=(int)Bombers[Num].X/32;
	j=(int)Bombers[Num].Y/32;

	if(a>0)
		if(Bombers[Num].Sens==B_DROITE) 
		{
			if(TickCount()-Bombers[Num].LastGTC1>=MV_TOUT) 
			{
				Bombers[Num].Sprite=++(Bombers[Num].Sprite)%5;
				Bombers[Num].LastGTC1=TickCount();
			}
		}
		else {
			Bombers[Num].Sens=B_DROITE;
			Bombers[Num].Sprite=0;
		}
	else if (a<0) 
		if(Bombers[Num].Sens==B_GAUCHE) 
		{
			if(TickCount()-Bombers[Num].LastGTC1>=MV_TOUT) 
			{
				Bombers[Num].Sprite=++(Bombers[Num].Sprite)%5;
				Bombers[Num].LastGTC1=TickCount();
			}
		}
		else {
			Bombers[Num].Sens=B_GAUCHE;
			Bombers[Num].Sprite=0;
		}
	else if (b>0)
		if(Bombers[Num].Sens==B_BAS) 
		{
			if(TickCount()-Bombers[Num].LastGTC1>=MV_TOUT) 
			{
				Bombers[Num].Sprite=++(Bombers[Num].Sprite)%5;
				Bombers[Num].LastGTC1=TickCount();
			}
		}
		else {
			Bombers[Num].Sens=B_BAS;
			Bombers[Num].Sprite=0;
		}
	else if (b<0)
		if(Bombers[Num].Sens==B_HAUT) 
		{
			if(TickCount()-Bombers[Num].LastGTC1>=MV_TOUT) 
			{
				Bombers[Num].Sprite=++(Bombers[Num].Sprite)%5;
				Bombers[Num].LastGTC1=TickCount();
			}
		}
		else {
			Bombers[Num].Sens=B_HAUT;
			Bombers[Num].Sprite=0;
		}

if (a)
{
	k=(int)_copysign(1.,a);
	if ((EstBloque(i+k,j)==0)||(int(Bombers[Num].X)*k<k*(i*32+16)))
	{
		if(int(Bombers[Num].Y)!=j*32+16)
		{
			Ecart=(float)j*32+16-Bombers[Num].Y;
			if (fabs(Ecart)>dt*V*V1) Ecart=(float)_copysign(dt*V*V1,Ecart);
			b=Ecart/(dt*V1);
			a=0;
		}
	}
	else
		a=0;
}

if (b)
{
	k=(int)_copysign(1.,b);
	if ((EstBloque(i,j+k)==0)||(int(Bombers[Num].Y)*k<k*(j*32+16)))
	{
		if(int(Bombers[Num].X)!=i*32+16)
		{
			Ecart=(float)i*32+16-Bombers[Num].X;
			if (fabs(Ecart)>dt*V*V1) Ecart=(float)_copysign(dt*V*V1,Ecart);
			a=Ecart/(dt*V1);
			b=0;
		}
	}
	else
		b=0;
}

float mx=float(a*dt*V1);
float my=float(b*dt*V1);
if(fabs(mx)>16.) mx=(float)_copysign(16.,mx);
if(fabs(my)>16.) my=(float)_copysign(16.,my);

Bombers[Num].X+=mx;
Bombers[Num].Y+=my;
if(Bombers[Num].X<16.) Bombers[Num].X=16.;
if(Bombers[Num].Y<16.) Bombers[Num].Y=16.;
if(Bombers[Num].X>640.-16.) Bombers[Num].X=640.-16.;
if(Bombers[Num].Y>480.-16.) Bombers[Num].Y=480.-16.;
return;
}

void DeplaceBomber(int Num,float a,float b)
{
	int x,y,p;
	float Ecart;
	if (Bombers[Num].Etat!=B_PLAYING) return;
	if(Bombers[Num].Sens==B_MORT) return;

	i=(int)Bombers[Num].X/32;
	j=(int)Bombers[Num].Y/32;

	if((a!=0)&&(b!=0))
	{
		if(LastXDir[Num]&&!EstBloque(i+(int)_copysign(1.,a),j))
			b=0;
		else
		if(LastYDir[Num]&&!EstBloque(i,j+(int)_copysign(1.,b)))
			a=0;
	}

	if(a>0)
		if(Bombers[Num].Sens==B_DROITE) 
		{
			if(TickCount()-Bombers[Num].LastGTC1>=MV_TOUT) 
			{
				Bombers[Num].Sprite=++(Bombers[Num].Sprite)%5;
				Bombers[Num].LastGTC1=TickCount();
			}
		}
		else {
			Bombers[Num].Sens=B_DROITE;
			Bombers[Num].Sprite=0;
		}
	else if (a<0) 
		if(Bombers[Num].Sens==B_GAUCHE) 
		{
			if(TickCount()-Bombers[Num].LastGTC1>=MV_TOUT) 
			{
				Bombers[Num].Sprite=++(Bombers[Num].Sprite)%5;
				Bombers[Num].LastGTC1=TickCount();
			}
		}
		else {
			Bombers[Num].Sens=B_GAUCHE;
			Bombers[Num].Sprite=0;
		}
	else if (b>0)
		if(Bombers[Num].Sens==B_BAS) 
		{
			if(TickCount()-Bombers[Num].LastGTC1>=MV_TOUT) 
			{
				Bombers[Num].Sprite=++(Bombers[Num].Sprite)%5;
				Bombers[Num].LastGTC1=TickCount();
			}
		}
		else {
			Bombers[Num].Sens=B_BAS;
			Bombers[Num].Sprite=0;
		}
	else if (b<0)
		if(Bombers[Num].Sens==B_HAUT) 
		{
			if(TickCount()-Bombers[Num].LastGTC1>=MV_TOUT) 
			{
				Bombers[Num].Sprite=++(Bombers[Num].Sprite)%5;
				Bombers[Num].LastGTC1=TickCount();
			}
		}
		else {
			Bombers[Num].Sens=B_HAUT;
			Bombers[Num].Sprite=0;
		}

	if (a)
	{
		k=(int)_copysign(1.,a);
		Ecart=(float)j*32+16-Bombers[Num].Y;
		if ((EstBloque(i+k,j)==0)||(int(Bombers[Num].X)*k<k*(i*32+16)))
		{
			if(int(Bombers[Num].Y)!=j*32+16)
			{
				Ecart=(float)j*32+16-Bombers[Num].Y;
				if (fabs(Ecart)>dt*V*V1) Ecart=(float)_copysign(dt*V*V1,Ecart);
				b=Ecart/(dt*V1);
				a=0;
			}
		}
		else
			a=0;
	}

	if (b)
	{
		k=(int)_copysign(1.,b);
		if ((EstBloque(i,j+k)==0)||(int(Bombers[Num].Y)*k<k*(j*32+16)))
		{
			if(int(Bombers[Num].X)!=i*32+16)
			{
				Ecart=(float)i*32+16-Bombers[Num].X;
				if (fabs(Ecart)>dt*V*V1) Ecart=(float)_copysign(dt*V*V1,Ecart);
				a=Ecart/(dt*V1);
				b=0;
			}
		}
		else
			b=0;
	}

	float mx=float(a*dt*V1);
	float my=float(b*dt*V1);
	if(fabs(mx)>16.) mx=(float)_copysign(16.,mx);
	if(fabs(my)>16.) my=(float)_copysign(16.,my);

	Bombers[Num].X+=mx;
	Bombers[Num].Y+=my;
	if(Bombers[Num].X<16.) Bombers[Num].X=16.;
	if(Bombers[Num].Y<16.) Bombers[Num].Y=16.;
	if(Bombers[Num].X>640.-16.) Bombers[Num].X=640.-16.;
	if(Bombers[Num].Y>480.-16.) Bombers[Num].Y=480.-16.;


	i=(int)Bombers[Num].X/32;
	j=(int)Bombers[Num].Y/32;
	if((Blocs[i][j].Type==SOL)&&(Blocs[i][j].TypeBonus!=0))
	{
		switch(Blocs[i][j].TypeBonus)
		{
			int t,u;
		case BN_TELEPORT:
			do
				p=rand()%NumPlayers;
			While(Bombers[p].Sens==B_MORT);

			BYTE px,py;
			do
			{
				px=randInt(0,19);
				py=randInt(0,14);
			}
			While(EstBloque(px,py)!=0);

			if(!bConnected)
			{
				Bombers[p].X=float(px*32+16);
				Bombers[p].Y=float(py*32+16);
			}
			else
			if(IsLocalPlayer(p))
			{
				Bombers[p].X=float(px*32+16);
				Bombers[p].Y=float(py*32+16);
				Bombers[p].NetGTC-=NetSendTime;
			}
			else
				SendGameMessage(Num,p,MSG_BN_TELEPORT,px,py,0,0);

			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndTeleport,0)
			break;

		case BN_RIG:
			Bombers[Num].RigOpt=1;
			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndBonus,0)
			break;

		case BN_RET:
			Bombers[Num].RetOpt=1;
			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndBonus,0)
			break;

		case BN_STD:
			Bombers[Num].BType=BB_STD;
			if(Settings.GO.CANREV)
				Bombers[Num].Reverse=1;
			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndBonus,0)
			break;

		case BN_LNG:
			if(!Settings.GO.TEAMPLAY)
			{
				for(t=u=0;t<NumPlayers;t++)
					if((t!=Num)&&(Bombers[t].Sens!=B_MORT)) u=1;
			}
			else
			{
				for(t=u=0;t<NumPlayers;t++)
					if((t!=Num)&&(Bombers[t].Sens!=B_MORT)&&!MemeTeam(Num,t)) u=1;
			}

			if(u==0) break;

			if(!Settings.GO.TEAMPLAY)
			{
				do
					p=rand()%NumPlayers;
				While((p==Num)||(Bombers[p].Sens==B_MORT));
			}
			else
			{
				do
					p=rand()%NumPlayers;
				While((p==Num)||MemeTeam(p,Num)||(Bombers[p].Sens==B_MORT));
			}

			Bombers[p].BType=BB_LNG;
			Bombers[p].TpsGTC=TickCount();
			SendGameMessage(Num,p,MSG_BN_LNG,TickCount(),0,0,0);
			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndVache,0)
			break;

		case BN_CRT:
			if(!Settings.GO.TEAMPLAY)
			{
				for(t=u=0;t<NumPlayers;t++)
					if((t!=Num)&&(Bombers[t].Sens!=B_MORT)) u=1;
			}
			else
			{
				for(t=u=0;t<NumPlayers;t++)
					if((t!=Num)&&(Bombers[t].Sens!=B_MORT)&&!MemeTeam(Num,t)) u=1;
			}

			if(u==0) break;

			if(!Settings.GO.TEAMPLAY)
			{
				do
					p=rand()%NumPlayers;
				While((p==Num)||(Bombers[p].Sens==B_MORT));
			}
			else
			{
				do
					p=rand()%NumPlayers;
				While((p==Num)||MemeTeam(p,Num)||(Bombers[p].Sens==B_MORT));
			}

			Bombers[p].BType=BB_CRT;
			Bombers[p].TpsGTC=TickCount();
			SendGameMessage(Num,p,MSG_BN_CRT,TickCount(),0,0,0);
			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndVache,0)
			break;

		case BN_REVERSE:
			if(!Settings.GO.TEAMPLAY)
			{
				for(t=u=0;t<NumPlayers;t++)
					if((t!=Num)&&(Bombers[t].Sens!=B_MORT)) u=1;
			}
			else
			{
				for(t=u=0;t<NumPlayers;t++)
					if((t!=Num)&&(Bombers[t].Sens!=B_MORT)&&!MemeTeam(Num,t)) u=1;
			}

			if(u==0) break;

			if(!Settings.GO.TEAMPLAY)
			{
				do
					p=rand()%NumPlayers;
				While((p==Num)||(Bombers[p].Sens==B_MORT));
			}
			else
			{
				do
					p=rand()%NumPlayers;
				While((p==Num)||MemeTeam(p,Num)||(Bombers[p].Sens==B_MORT));
			}

			Bombers[p].Reverse=-1;
			if(Bombers[p].CmdCtrl<4)
				Bombers[p].RevGTC=TickCount();
			else
				Bombers[p].RevGTC=BOT_REVTIME+TickCount()-Settings.GO.REV_TOUT;
			SendGameMessage(Num,p,MSG_BN_REVERSE,TickCount(),0,0,0);
			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndVache,0)
			break;

		case BN_ALLEXPL:
			for(x=0;x<NumPlayers;x++)
				for(y=0;y<NumBombs[x];y++)
					if(Bombs[x][y].Etat==B_PLAYING) 
						ExploseBomb(x,y);

			SendGameMessage(Num,-1,MSG_BN_ALLEXPL,0,0,0,0);
			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndVache,0)
			break;

		case BN_SIMUL:
			if(Bombers[Num].SimulBomb<Settings.GO.SIMULBOMB)
			{
				Bombers[Num].SimulBomb++;	
				Bombers[Num].NbBomb++;	
			}
		PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndBonus,0)
		break;

		case BN_MINE:
		if(Bombers[Num].NbMine<Settings.GO.SIMULBOMB)
			Bombers[Num].NbMine++;
		PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndBonus,0)
		break;

		case BN_BLAST:
			if(Bombers[Num].Blast<20)
				Bombers[Num].Blast++;
			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndBonus,0)
			break;

		case BN_NUCL:
			SendGameMessage(Num,-1,MSG_BN_NUCL,0,0,0,0);
			for(x=0;x<20;x++)
				for(y=0;y<15;y++)
				{
					if(Settings.GO.DISPRETNUCL)
						if((Blocs[x][y].Type==SOL)&&(Blocs[x][y].TypeBonus==BN_RET))
							Blocs[x][y].TypeBonus=0;

					if(Blocs[x][y].Type==CAISSE) {
						Blocs[x][y].Type=SOL;
						Blocs[x][y].TypeBonus=0;
					}
				}

					rect.top=0;
					rect.left=0;
					rect.right=20;
					rect.bottom=15;
					UpdateBuffer(rect);
					bUpdateAllScreen=GetUpdateCount();
					for(p=0;p<NumPlayers;p++)
					{
						Bombers[p].NbBomb+=Settings.GO.SIMULBOMB-Bombers[p].SimulBomb;
						Bombers[p].SimulBomb=Settings.GO.SIMULBOMB;
						Bombers[p].Blast=20;
						if(Settings.GO.RETNUCL)
							Bombers[p].RetOpt=0;
					}
			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndVache,0)
			break;

		case BN_RAD:
			Bombers[Num].Blast=20;
			PLAYSON(Num,(Bombers[Num].X-320)/320.0,Bombers[Num].SndBonus,0)
			break;

		default:
			break;
		}

		SendGameMessage(Num,-1,MSG_ERASEBONUS,i,j,0,0);
		Blocs[i][j].TypeBonus=0;
		rect.top=j;
		rect.left=i;
		rect.bottom=rect.top+1;
		rect.right=rect.left+1;
		UpdateBuffer(rect);
		bUpdateCase[i][j]=GetUpdateCount();
	}

	return;
}

void BltBomber(int Num)
{
	if((lpDDSBack==NULL)||(lpDDSprites==NULL))
		return;
	if (Bombers[Num].Etat!=B_PLAYING) return;

	int col=int(Bombers[Num].X+16)/32;
	int row=int(Bombers[Num].Y+16)/32;
	for(int c=col-2;c<col+1;c++)
		for(int r=row-2;r<row+1;r++)
			if((c>=0)&&(r>=0)&&(c<20)&&(r<15))
				bUpdateCase[c][r]=GetUpdateCount();

	i=Bombers[Num].Sprite;
	switch (Bombers[Num].Sens)
	{
	case B_DROITE:
		rect.left=i*32+B_DROITEX;
		rect.right=i*32+B_DROITEX+32;
		rect.top=B_DROITEY+64*Bombers[Num].Color;
		rect.bottom=B_DROITEY+32+64*Bombers[Num].Color;

		lpDDSBack->BltFast((int)Bombers[Num].X-16,(int)Bombers[Num].Y-16,lpDDSprites,&rect,dwBltFastCK | BLITFAST_WAIT);
		break;

	case B_GAUCHE:
		rect.left=i*32+B_GAUCHEX;
		rect.right=i*32+B_GAUCHEX+32;
		rect.top=B_GAUCHEY+64*Bombers[Num].Color;;
		rect.bottom=B_GAUCHEY+32+64*Bombers[Num].Color;;
		lpDDSBack->BltFast((int)Bombers[Num].X-16,(int)Bombers[Num].Y-16,lpDDSprites,&rect,dwBltFastCK | BLITFAST_WAIT);
		break;

	case B_HAUT:
		rect.left=i*32+B_HAUTX;
		rect.right=i*32+B_HAUTX+32;
		rect.top=B_HAUTY+64*Bombers[Num].Color;;
		rect.bottom=B_HAUTY+32+64*Bombers[Num].Color;;
		lpDDSBack->BltFast((int)Bombers[Num].X-16,(int)Bombers[Num].Y-16,lpDDSprites,&rect,dwBltFastCK | BLITFAST_WAIT);
		break;

	case B_BAS:
		rect.left=i*32+B_BASX;
		rect.right=i*32+B_BASX+32;
		rect.top=B_BASY+64*Bombers[Num].Color;;
		rect.bottom=B_BASY+32+64*Bombers[Num].Color;;
		lpDDSBack->BltFast((int)Bombers[Num].X-16,(int)Bombers[Num].Y-16,lpDDSprites,&rect,dwBltFastCK | BLITFAST_WAIT);
		break;

	case B_MORT:
		rect.left=5*32+B_HAUTX;
		rect.right=5*32+B_HAUTX+32;
		rect.top=B_HAUTY+64*Bombers[Num].Color;;
		rect.bottom=B_HAUTY+32+64*Bombers[Num].Color;;
		lpDDSBack->BltFast((int)Bombers[Num].X-16,(int)Bombers[Num].Y-16,lpDDSprites,&rect,dwBltFastCK | BLITFAST_WAIT);
		break;

	default:
		break;
	}
	if(Settings.GO.TEAMPLAY&&(Bombers[Num].Team!=VOID_TEAM)&&Settings.PLAYERTCOLOR)
	{
		rect.top=BB_TEAMCOLORY;
		rect.bottom=rect.top+32;
		rect.left=BB_TEAMCOLORX+32*(Bombers[Num].Team-1);
		rect.right=rect.left+32;
		lpDDSBack->BltFast((int)Bombers[Num].X-16,(int)Bombers[Num].Y-16,lpDDSprites,&rect,dwBltFastCK | BLITFAST_WAIT);
	}
	return;
}

void UpdateScreen(RECT upds)
{
	if((lpDDSBack==NULL)||(lpDDSBuffer==NULL))
		return;
	if (upds.top<0) upds.top=0;
	if (upds.left<0) upds.left=0;
	if (upds.right>640) upds.right=640;
	if (upds.bottom>480) upds.bottom=480;
	lpDDSBack->BltFast(upds.left,upds.top,lpDDSBuffer,&upds,DDBLTFAST_NOCOLORKEY|BLITFAST_WAIT);
	return;
}

BOOL EstBloque(int x,int y)
{
	if ((x<0)||(y<0)||(x>19)||(y>14)) return TRUE;
	return ((Blocs[x][y].Type==MUR)||(Blocs[x][y].Type==CAISSE)||Blocs[x][y].Bloque);
}

void PoseBomb(int Num,int Mode)
{
	if(Blocs[(int)Bombers[Num].X/32][(int)Bombers[Num].Y/32].Bloque) return;;
	if ((Bombers[Num].NbBomb==0)&&(Mode!=BB_MINE)&&(Mode!=BB_RIG)) return;
	i=0;
	While((Bombs[Num][i].Etat==B_PLAYING)&&(i<TOT_BOMB))
		i++;

	if(((Mode&BB_MINE)==0)&&((Mode&BB_RIG)==0)) 
		Bombers[Num].NbBomb--;
	Bombs[Num][i].X=(int)Bombers[Num].X/32;
	Bombs[Num][i].MustExpl=0;
	Bombs[Num][i].Y=(int)Bombers[Num].Y/32;
	Blocs[Bombs[Num][i].X][Bombs[Num][i].Y].Bloque|=BLQ_BOMB;

	Bombs[Num][i].TOut=500; //ms entre chaque sprite
	Bombs[Num][i].LastGTC=TickCount();
	Bombs[Num][i].Etat=B_PLAYING;
	Bombs[Num][i].Blast=Bombers[Num].Blast;

	if (Mode&BB_STD)
		Bombs[Num][i].BSpr=Bombs[Num][i].NSpr=BB_STDSPR;
	else if (Mode&BB_LNG)
		Bombs[Num][i].BSpr=Bombs[Num][i].NSpr=BB_LNGSPR;
	else if (Mode&BB_CRT)
		Bombs[Num][i].BSpr=Bombs[Num][i].NSpr=BB_CRTSPR;
	else if (Mode&BB_RET)
		Bombs[Num][i].BSpr=Bombs[Num][i].NSpr=BB_RETSPR;
	else if (Mode&BB_MINE)
	{
		Bombers[Num].NbMine--;
		Bombs[Num][i].BSpr=Bombs[Num][i].NSpr=BB_MINESPR;
		Bombs[Num][i].Sens=4;
		Bombs[Num][i].Blast=20;
	}
	else if(Mode&BB_RIG)
	{
		Bombs[Num][i].BSpr=Bombs[Num][i].NSpr=BB_STDSPR;
		Bombs[Num][i].Blast=0;
	}

	UpdateBombCount();
	SendGameMessage(Num,-1,MSG_NEWBOMB,i,0,0,0);
	return;
}

void GereBombs(void)
{
	for (i=0;i<NumPlayers;i++)
		if(Bombers[i].Etat==B_PLAYING)
			for(j=0;j<NumBombs[i];j++)
				if(Bombs[i][j].Etat==B_PLAYING)
				{
					bUpdateCase[Bombs[i][j].X][Bombs[i][j].Y]=GetUpdateCount();
					if(Bombs[i][j].MustExpl==2)
						ExploseBomb(i,j);
					else
					{
						if(Bombs[i][j].MustExpl==1)
						{
							Bombs[i][j].MustExpl=2;
							Bombs[i][j].NSpr=11;
						}
						else
						{
							if(Bombs[i][j].BSpr<BB_RETSPR)
							{
								Bombs[i][j].NSpr=(BYTE)((TickCount()-Bombs[i][j].LastGTC)/Bombs[i][j].TOut+Bombs[i][j].BSpr);
								if(Bombs[i][j].NSpr>11)
									ExploseBomb(i,j);
							}
							else if((Bombs[i][j].BSpr==BB_MINESPR)&&(TickCount()-Bombs[i][j].LastGTC>MINE_TOUT)&&(Bombs[i][j].NSpr==BB_MINESPR))
								Bombs[i][j].NSpr++;
						}
					}

					if(Bombs[i][j].Etat==B_PLAYING)
						BltBomb(i,j);
				}
	return;
}

void BltBomb(int a,int b)
{
	if((lpDDSBack==NULL)||(lpDDSprites==NULL))
		return;
	if(Blocs[Bombs[a][b].X][Bombs[a][b].Y].Type==MUR)
		return;

	int NSpr=Bombs[a][b].NSpr<=14?Bombs[a][b].NSpr:14;
	rect.top=BB_BOMBY;
	rect.bottom=rect.top+32;
	rect.left=NSpr*32+BB_BOMBX;
	rect.right=rect.left+32;
	lpDDSBack->BltFast(Bombs[a][b].X*32,Bombs[a][b].Y*32,lpDDSprites,&rect,dwBltFastCK | BLITFAST_WAIT);

	if(Settings.GO.TEAMPLAY&&(Bombers[a].Team!=VOID_TEAM)&&Settings.BOMBTCOLOR)
	{
		rect.top=BB_TEAMCOLORY;
		rect.bottom=rect.top+32;
		rect.left=BB_TEAMCOLORX+32*(Bombers[a].Team-1);
		rect.right=rect.left+32;
		lpDDSBack->BltFast(Bombs[a][b].X*32,Bombs[a][b].Y*32,lpDDSprites,&rect,dwBltFastCK | BLITFAST_WAIT);
	}
	return;
}

void ExploseBomb(int a,int b)
{
	PlaySon((Bombs[a][b].X-10)/10.0,Bombers[a].SndExplose,0);
 	Blocs[Bombs[a][b].X][Bombs[a][b].Y].Bloque&=~BLQ_BOMB;
	Bombs[a][b].Etat=B_DISABLED;
	if((Bombs[a][b].Blast!=0)&&(Bombs[a][b].BSpr<BB_MINESPR)) 
		Bombers[a].NbBomb++;

	NewBlast(a,Bombs[a][b].X,Bombs[a][b].Y,TRUE);
	for(l=Bombs[a][b].X-1;l>=Bombs[a][b].X-Bombs[a][b].Blast;l--)
	{
		if((l<0)||(l>19))
			break;
		if(Blocs[l][Bombs[a][b].Y].Bloque)
		{
			BOOL br=FALSE;
			for(m=0;m<NumPlayers;m++)
				for(k=0;k<NumBombs[m];k++)
					if((Bombs[m][k].Etat==B_PLAYING)&&(Bombs[m][k].X==l)&&(Bombs[m][k].Y==Bombs[a][b].Y)) 
					{
						Bombs[m][k].MustExpl=1;
						br=TRUE;
					}
			if(br)
				break;
		}
		NewBlast(a,l,Bombs[a][b].Y,0);
		if(EstBloque(l,Bombs[a][b].Y))
			break;
	}

	for(l=Bombs[a][b].X+1;l<=Bombs[a][b].X+Bombs[a][b].Blast;l++)
	{
		if((l<0)||(l>19))
			break;
		if(Blocs[l][Bombs[a][b].Y].Bloque)
		{
			BOOL br=FALSE;
			for(m=0;m<NumPlayers;m++)
				for(k=0;k<NumBombs[m];k++)
					if((Bombs[m][k].Etat==B_PLAYING)&&(Bombs[m][k].X==l)&&(Bombs[m][k].Y==Bombs[a][b].Y))
					{
						Bombs[m][k].MustExpl=1;
						br=TRUE;
					}
			if(br)
				break;
		}
		NewBlast(a,l,Bombs[a][b].Y,0);
		if(EstBloque(l,Bombs[a][b].Y))
			break;
	}

	for(l=Bombs[a][b].Y-1;l>=Bombs[a][b].Y-Bombs[a][b].Blast;l--)
	{
		if((l<0)||(l>14))
			break;
		if(Blocs[Bombs[a][b].X][l].Bloque)
		{
			BOOL br=FALSE;
			for(m=0;m<NumPlayers;m++)
				for(k=0;k<NumBombs[m];k++)
					if((Bombs[m][k].Etat==B_PLAYING)&&(Bombs[m][k].X==Bombs[a][b].X)&&(Bombs[m][k].Y==l))
					{
						Bombs[m][k].MustExpl=1;
						br=TRUE;
					}
			if(br)
				break;
		}
		NewBlast(a,Bombs[a][b].X,l,0);
		if(EstBloque(Bombs[a][b].X,l))
			break;
	}

	for(l=Bombs[a][b].Y+1;l<=Bombs[a][b].Y+Bombs[a][b].Blast;l++)
	{
		if((l<0)||(l>14))
			break;
		if(Blocs[Bombs[a][b].X][l].Bloque)
		{
			BOOL br=FALSE;
			for(m=0;m<NumPlayers;m++)
				for(k=0;k<NumBombs[m];k++)
					if((Bombs[m][k].Etat==B_PLAYING)&&(Bombs[m][k].X==Bombs[a][b].X)&&(Bombs[m][k].Y==l))
					{
						Bombs[m][k].MustExpl=1;
						br=TRUE;
					}
			if(br)
				break;
		}
		NewBlast(a,Bombs[a][b].X,l,0);
		if(EstBloque(Bombs[a][b].X,l))
			break;
	}
	UpdateBombCount();
	return;
}

void NewBlast(int Num,int a,int b,BOOL bBombBlast)
{
//	int x;
	int	y,t;
	if((a<0)||(b<0)||(a>19)||(b>14))
		return;
	if(Blocs[a][b].Type==MUR)
		return;
//	for(x=0;x<NumPlayers;x++)

		for(y=0;y<NumBlasts[Num];y++)
			if((Blasts[Num][y].Etat==B_PLAYING)&&(Blasts[Num][y].X==a)&&(Blasts[Num][y].Y==b)) 
			{
				if(Blasts[Num][y].Type==CAISSE)
					Blasts[Num][y].NSpr=1;
				else
					Blasts[Num][y].NSpr=0;
				Blasts[Num][y].LastGTC=TickCount();
				UpdateBlastCount();
				return;
			}

	t=0;
	while((Blasts[Num][t].Etat==B_PLAYING)&&(t<TOT_BLAST))
		t++;

	if(t>=TOT_BLAST)
		return;

	Blasts[Num][t].Etat=B_PLAYING;
	Blasts[Num][t].X=a;
	Blasts[Num][t].Y=b;
	Blasts[Num][t].LastGTC=TickCount();
	Blasts[Num][t].TOut=BLAST_TOUT;

	switch(Blocs[a][b].Type)
	{
	case CAISSE:
		Blasts[Num][t].NSpr=1;
		Blasts[Num][t].Type=CAISSE;
		Blocs[a][b].Type=SOL;
		rect.top=b;
		rect.left=a;
		rect.bottom=b+1;
		rect.right=a+1;
		UpdateBuffer(rect);
		Blocs[a][b].Bloque|=BLQ_BLAST;
		bUpdateCase[a][b]=GetUpdateCount();
		break;

	case SOL:
		Blasts[Num][t].NSpr=0;
		Blasts[Num][t].Etat=B_PLAYING;
		Blasts[Num][t].Type=SOL;
		if(Blocs[a][b].TypeBonus!=0)
		{
			Blocs[a][b].TypeBonus=0;
			rect.top=b;
			rect.left=a;
			rect.bottom=b+1;
			rect.right=a+1;
			UpdateBuffer(rect);
			bUpdateCase[a][b]=GetUpdateCount();
			SendGameMessage(Num,-1,MSG_ERASEBONUS,a,b,0,0);
		}
		break;
	}

	if(bBombBlast)
		Blasts[Num][t].BombBlast=TRUE;
	else
		Blasts[Num][t].BombBlast=0;
	UpdateBlastCount();
	return;
}

void GereBlasts(void)
{
	for(i=0;i<NumPlayers;i++)
		if(Bombers[i].Etat==B_PLAYING)
			for(j=0;j<NumBlasts[i];j++)
				if(Blasts[i][j].Etat==B_PLAYING)
				{
					if(Blasts[i][j].Type==CAISSE)
					{
						bUpdateCase[Blasts[i][j].X][Blasts[i][j].Y]=GetUpdateCount();
						if(TickCount()-Blasts[i][j].LastGTC>=BLAST_START_TOUT)
						{
							Blasts[i][j].NSpr=(BYTE)((TickCount()-BLAST_START_TOUT-Blasts[i][j].LastGTC)/Blasts[i][j].TOut);
							if(Blasts[i][j].NSpr>=6)
							{
								Blocs[Blasts[i][j].X][Blasts[i][j].Y].Bloque&=~BLQ_BLAST;
								Blasts[i][j].Etat=B_DISABLED;
								UpdateBlastCount();
								/*
								Blocs[Blasts[i][j].X][Blasts[i][j].Y].Type=SOL;
								rect.left=Blasts[i][j].X;
								rect.top=Blasts[i][j].Y;
								rect.right=rect.left+1;
								rect.bottom=rect.top+1;
								UpdateBuffer(rect);
								rect.left=Blasts[i][j].X*32;
								rect.top=Blasts[i][j].Y*32;
								rect.right=rect.left+32;
								rect.bottom=rect.top+32;
								UpdateScreen(rect);
								*/
								bUpdateCase[i][j]=GetUpdateCount();
							}
	//						else
	//							Blasts[i][j].NSpr++;
						}

						if(Blasts[i][j].Etat==B_PLAYING)
							BltBlast(i,j);
					}
					else
					{
						if(TickCount()-Blasts[i][j].LastGTC>=BLAST_START_TOUT)
						{
							Blasts[i][j].NSpr=(BYTE)((TickCount()-BLAST_START_TOUT-Blasts[i][j].LastGTC)/BLAST_TOUT);
							if(Blasts[i][j].NSpr>=4)
							{
								Blasts[i][j].Etat=B_DISABLED;
								UpdateBlastCount();
								if(Blasts[i][j].BombBlast)
								{
									bUpdateCol[Blasts[i][j].X]=GetUpdateCount();
									bUpdateRow[Blasts[i][j].Y]=GetUpdateCount();
								}
							}
						}
					}
					if(Blasts[i][j].Etat==B_PLAYING) 
						BltBlast(i,j);
				}
	return;
}

void BltBlast(int a,int b)
{
	if((lpDDSBack==NULL)||(lpDDSprites==NULL))
		return;
	if(Blasts[a][b].Type!=CAISSE)
	{
		rect.top=BB_BLASTY;
		rect.bottom=rect.top+32;
		rect.left=Blasts[a][b].NSpr*32+BB_BLASTX;
		rect.right=rect.left+32;
	}
	else
	{
		rect.top=CAISSEY;
		rect.bottom=rect.top+32;
		rect.left=Blasts[a][b].NSpr*32+CAISSEX;
		rect.right=rect.left+32;
	}
	lpDDSBack->BltFast(Blasts[a][b].X*32,Blasts[a][b].Y*32,lpDDSprites,&rect,dwBltFastCK | BLITFAST_WAIT);
	return;
}

void GereBombers(void)
{
	int RealGere,x,y;
	int	cnt,n,P,NumHitPlayers;
	BOOL	HitPlayer[MAX_PLAYERS];
	BOOL	AK;
	DWORD	T;

	RealGere=0;
	for(n=0;n<NumPlayers;n++)
	{
		if(Bombers[n].Etat!=B_PLAYING) 
			continue;
		switch(BCtrl[n].Control)
		{
		case CTRL_KB:
		if(Bombers[n].Sens!=B_MORT)
		{
			memset(HitPlayer,0,MAX_PLAYERS*sizeof(BOOL));
			NumHitPlayers=0;
			AK=FALSE;
			if(!Settings.GO.TEAMPLAY)
			{
				for(x=0;x<NumPlayers;x++)
					for(y=0;y<NumBlasts[x];y++)
						if(Blasts[x][y].Etat==B_PLAYING)
							if(Dist((int)Bombers[n].X,(int)Bombers[n].Y,Blasts[x][y].X*32+16,Blasts[x][y].Y*32+16)<HIT_DIST)
							{
								if(x!=n)
								{
									HitPlayer[x]=TRUE;
									NumHitPlayers++;
								}
								else 
									AK=TRUE;

							}
			}
			else
			{
				for(x=0;x<NumPlayers;x++)
					if((x==n)||!MemeTeam(x,n))
					{
						for(y=0;y<NumBlasts[x];y++)
							if(Blasts[x][y].Etat==B_PLAYING)
								if(Dist((int)Bombers[n].X,(int)Bombers[n].Y,Blasts[x][y].X*32+16,Blasts[x][y].Y*32+16)<HIT_DIST)
								{
									if(x!=n)
									{
										HitPlayer[x]=TRUE;
										NumHitPlayers++;
									}
									else 
										AK=TRUE;

								}
					}
			}

			if(NumHitPlayers>0)
			{
				for(int jo=0;jo<MAX_PLAYERS;jo++)
					if(HitPlayer[jo])
					{
						Bombers[jo].Frag++;
						SendGameMessage(n,-1,MSG_GETFRAG,jo,0,0,0);
						if(IsLocalPlayer(jo))
							UpdateRemoteData(jo);
						sprintf(Buff,LSZ_sKILLEDs,Bombers[jo].Nom,Bombers[n].Nom);
						AddMessage(Buff,2500,GetBomberColor(jo));
					}
				Bombers[n].NetGTC-=NetSendTime;
				Bombers[n].Sens=B_MORT;
				PLAYSON(n,(Bombers[n].X-320)/320.0,Bombers[n].SndMort,0)
				bScoreRedraw=TRUE;
				Bombers[n].NetGTC=TickCount();
			}
			else if(AK)
			{
				Bombers[n].AK++;
				SendGameMessage(n,-1,MSG_GETAK,0,0,0,0);
				sprintf(Buff,LSZ_sSUICIDE,Bombers[n].Nom);
				AddMessage(Buff,2500,GetBomberColor(n));
				if(IsLocalPlayer(n))
					UpdateRemoteData(n);
				Bombers[n].Sens=B_MORT;
				PLAYSON(n,(Bombers[n].X-320)/320.0,Bombers[n].SndAK,0)
				bScoreRedraw=TRUE;
				Bombers[n].NetGTC=TickCount();
			}

		}

		if(Bombers[n].Sens==B_MORT) 
		{
				if(TickCount()-Bombers[n].NetGTC>=NetSendTime)
				{
					Bombers[n].NetGTC=TickCount();
					SendGameMessage(n,-1,MSG_UPDATE,n,0,0,0);
				}
			break;
		}

		if(Bombers[n].CmdCtrl<4)
			GetKeys(Bombers[n].CmdCtrl);
		else
			ControleIA(n);

		RealGere++;
		if((Bombers[n].Reverse==-1)&&(TickCount()-Bombers[n].RevGTC>=Settings.GO.REV_TOUT)) Bombers[n].Reverse=1;
		if((Bombers[n].BType!=BB_STD)&&(TickCount()-Bombers[n].TpsGTC>=Settings.GO.MCH_TOUT)) Bombers[n].BType=BB_STD;

		if(F1lck[Bombers[n].CmdCtrl]) 
		{

			if (Bombers[n].RetOpt==0)
				PoseBomb(n,Bombers[n].BType);
			else
				PoseBomb(n,BB_RET);
		}
		else if(F2lck[Bombers[n].CmdCtrl])
		{
			T=TickCount();
			P=-1;
			for(cnt=0;cnt<NumBombs[n];cnt++)
				if((Bombs[n][cnt].Etat==B_PLAYING)&&(Bombs[n][cnt].NSpr==BB_RETSPR)&&(Bombs[n][cnt].LastGTC<T))
				{
					T=Bombs[n][cnt].LastGTC;
					P=cnt;
				}

			if(P>=0)
			{
				SendGameMessage(n,-1,MSG_EXPBOMB,n,Bombs[n][P].X,Bombs[n][P].Y,0);
				ExploseBomb(n,P);
			}
			else
		   if(Bombers[n].NbMine>0)
			   PoseBomb(n,BB_MINE);
		}
		else if(F3lck[Bombers[n].CmdCtrl])
			if(Bombers[n].RigOpt==1) 
				PoseBomb(n,BB_RIG);

/*
		updBuff.left=(int)Bombers[n].X/32-2;
		updBuff.right=(int)Bombers[n].X/32+2;
		updBuff.top=(int)Bombers[n].Y/32-2;
		updBuff.bottom=(int)Bombers[n].Y/32+2;
		UpdateBuffer(updBuff);
*/
		i=j=0;
		if(UPlck[Bombers[n].CmdCtrl]) 
			j=-V*Bombers[n].Reverse;
		else
		if(DOWNlck[Bombers[n].CmdCtrl]) j=V*Bombers[n].Reverse;

		if(LEFTlck[Bombers[n].CmdCtrl]) i=-V*Bombers[n].Reverse;
		else
		if(RIGHTlck[Bombers[n].CmdCtrl]) i=V*Bombers[n].Reverse;

		float mvx,mvy;
		mvx=(float)i;
		mvy=(float)j;
		DeplaceBomber(n,mvx,mvy);
		if((int(mvx)!=Bombers[n].oldSensX)||(int(mvy)!=Bombers[n].oldSensY))
			Bombers[n].NetGTC-=NetSendTime;

		Bombers[n].oldSensX=int(mvx);
		Bombers[n].oldSensY=int(mvy);

		ProcheMine(n);

		if(TickCount()-Bombers[n].NetGTC>=NetSendTime)
		{
			Bombers[n].NetGTC=TickCount();
			SendGameMessage(n,-1,MSG_UPDATE,n,(int)mvx,(int)mvy,0);
		}

		break;

		case	CTRL_NET:
			if(BCtrl[n].Connected&&(Bombers[n].Sens!=B_MORT))
			{
				ExtrapoleBomber(n,Bombers[n].dx,Bombers[n].dy);
				RealGere++;
				break;
			}

		break;
		}
		if(Bombers[n].Etat==B_PLAYING)
			BltBomber(n);
	}

	if(Settings.GO.TEAMPLAY)
		if(RemainingTeams()<=1)
			RealGere=1;

	if (((NumPlayers>1)||((NumPlayers==1)&&(RealGere==0)))&&((RealGere<=1)&&((!bConnected)||(IsHost))))
	{
		if(lGTC==0)
			lGTC=TickCount();
		else
			if(TickCount()-lGTC>=MORT_TOUT)
			{
				bReset=TRUE;
				lGTC=0;
			}
	}

	return;
}

int RemainingTeams(void)
{
	BOOL t[NUM_TEAMS+1];
	int ct=0;
	memset(t,0,(NUM_TEAMS+1)*sizeof(BOOL));
	for(int bc=0;bc<NumPlayers;bc++)
		if((Bombers[bc].Etat==B_PLAYING)&&(Bombers[bc].Sens!=B_MORT))
		{
			if((Bombers[bc].Team==VOID_TEAM)&&t[VOID_TEAM])
				ct++;
			t[Bombers[bc].Team]=TRUE;
		}

	for(int tc=0;tc<NUM_TEAMS+1;tc++)
		if(t[tc])
			ct++;
	return ct;
}

int GetNbEntities(void)
{
	BOOL t[NUM_TEAMS+1];
	int ct=0;
	memset(t,0,(NUM_TEAMS+1)*sizeof(BOOL));
	for(int bc=0;bc<NumPlayers;bc++)
		if(Bombers[bc].Etat==B_PLAYING)
		{
			if((Bombers[bc].Team==VOID_TEAM)&&t[VOID_TEAM])
				ct++;
			t[Bombers[bc].Team]=TRUE;
		}

	for(int tc=0;tc<NUM_TEAMS+1;tc++)
		if(t[tc])
			ct++;
	return ct;
}

DWORD GetBomberColor(BYTE Coloridx)
{
	DWORD color;
	if(Bombers[Coloridx].Color<4)
		return BOMBER_COLOR[Bombers[Coloridx].Color];
	else
		return BOMBER_DEFCOLOR;
	return color;
}

void ResetAll(void)
{
	StopVoiceCapture();
	DestroyAllVoiceStacks();

	olddelta=0;
	lGTC=0;
	bReset=0;
	bMustReset=0;
	bSudden=FALSE;
	//GameStartTime=TickCount();
	bScoreRedraw=TRUE;
	dwPlays++;
	if((Settings.MAPDELAY!=0)&&(IsHost||!bConnected))
	{
		if(dwPlays>=Settings.MAPDELAY)
		{
			dwPlays=0;
			char szNewMap[BMAX_PATH];
			do
			strcpy(szNewMap,MapDir[rand()%NbMaps]);
			while((NbMaps>1)&&!strcmp(szNewMap,szCurMap));
			strcpy(szCurMap,szNewMap);
			sprintf(Buff,LSZ_NEWMAPs,szCurMap);
			AddMessage(Buff,2500,RGB(0,0,200));
		}
	}
	else
		strcpy(szCurMap,Settings.szSelMap);

	if(IsHost||!bConnected)
		InitBlocs(Settings.GO.BLOC_COND);
	ResetMulti();
	SetAllBlocsSol();
	ResetGame();
	ResetIA();
	AllocScoreSurface();
	UpdateAll();
	GereLastFirst();
#ifdef _DEBUG
	DebugStart();
#endif
	if(Settings.VOICE&&Settings.ENABLESON&&bConnected&&!bDedicated)
		StartVoiceCapture();
 	return;
}

void UpdateAll(void)
{
	RECT urct;
	urct.left=urct.top=0;
	urct.right=20;
	urct.bottom=15;
	UpdateBuffer(urct);
	bUpdateAllScreen=GetUpdateCount();
	return;
}

int Dist(int x1,int y1,int x2,int y2)
{
	return (int)sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

BOOL CALLBACK DlgProcTxtMsg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hDlg,IDC_F1TXT),TmpTM1.F1TxtMsg);
		SetWindowText(GetDlgItem(hDlg,IDC_F2TXT),TmpTM1.F2TxtMsg);
		SetWindowText(GetDlgItem(hDlg,IDC_F3TXT),TmpTM1.F3TxtMsg);
		SetWindowText(GetDlgItem(hDlg,IDC_F4TXT),TmpTM1.F4TxtMsg);
		SetWindowText(GetDlgItem(hDlg,IDC_F5TXT),TmpTM1.F5TxtMsg);
		SetWindowText(GetDlgItem(hDlg,IDC_F6TXT),TmpTM1.F6TxtMsg);
		SetWindowText(GetDlgItem(hDlg,IDC_F7TXT),TmpTM1.F7TxtMsg);
		SetWindowText(GetDlgItem(hDlg,IDC_F8TXT),TmpTM1.F8TxtMsg);
		SetFocus(GetDlgItem(hDlg,IDOK));
		return(FALSE);
		break;

	case WM_COMMAND:
		switch(wParam)
		{
			case IDOK:
				GetWindowText(GetDlgItem(hDlg,IDC_F1TXT),TmpTM.F1TxtMsg,BMAX_PATH);
				GetWindowText(GetDlgItem(hDlg,IDC_F2TXT),TmpTM.F2TxtMsg,BMAX_PATH);
				GetWindowText(GetDlgItem(hDlg,IDC_F3TXT),TmpTM.F3TxtMsg,BMAX_PATH);
				GetWindowText(GetDlgItem(hDlg,IDC_F4TXT),TmpTM.F4TxtMsg,BMAX_PATH);
				GetWindowText(GetDlgItem(hDlg,IDC_F5TXT),TmpTM.F5TxtMsg,BMAX_PATH);
				GetWindowText(GetDlgItem(hDlg,IDC_F6TXT),TmpTM.F6TxtMsg,BMAX_PATH);
				GetWindowText(GetDlgItem(hDlg,IDC_F7TXT),TmpTM.F7TxtMsg,BMAX_PATH);
				GetWindowText(GetDlgItem(hDlg,IDC_F8TXT),TmpTM.F8TxtMsg,BMAX_PATH);

				EndDialog(hDlg,TRUE);
			break;

			case IDCANCEL:
					EndDialog(hDlg,0);
			break;
			case IDB_RESETTM:
				SetDefaultTextMessages(&TmpTM1);
				SendMessage(hDlg,WM_INITDIALOG,0,0);
				break;
		}
		break;

	}
	return (FALSE);
}

void SetOptionsDialog(HWND hDlg,const struct SETTINGS* pSettings)
{
		char szdt[BMAX_PATH];
		strcpy(szdt,LSZ_OPTMENU);
		if(strlen(szCurConfig)!=0)
			strcat(szdt,szCurConfig);
		else
			strcat(szdt,LSZ_DEFAULT);
		while(CB_ERR!=SendDlgItemMessage(hDlg,IDC_DEPTH,CB_DELETESTRING,0,0));
		while(CB_ERR!=SendDlgItemMessage(hDlg,IDC_NET_TOUT,CB_DELETESTRING,0,0));

		SetWindowText(hDlg,szdt);
		SendDlgItemMessage(hDlg,IDC_NET_TOUT,CB_ADDSTRING,0,(LPARAM)LSZ_CON9k6);
		SendDlgItemMessage(hDlg,IDC_NET_TOUT,CB_ADDSTRING,0,(LPARAM)LSZ_CON14k4);
		SendDlgItemMessage(hDlg,IDC_NET_TOUT,CB_ADDSTRING,0,(LPARAM)LSZ_CON33k6);
		SendDlgItemMessage(hDlg,IDC_NET_TOUT,CB_ADDSTRING,0,(LPARAM)LSZ_CON1m);
		SendDlgItemMessage(hDlg,IDC_NET_TOUT,CB_ADDSTRING,0,(LPARAM)LSZ_CONmax);

		int iIndex=CB_ERR;
		for(int x=0;x<NumModes;x++)
		{
			sprintf(Buff,"640x480x%ld",Depth[x]);
			SendDlgItemMessage(hDlg,IDC_DEPTH,CB_ADDSTRING,0,(LPARAM)Buff);
			SendDlgItemMessage(hDlg,IDC_DEPTH,CB_SETITEMDATA,x,(LPARAM)Depth[x]);
			if(pSettings->Depth==Depth[x])
				iIndex=x;
		}
		if(iIndex!=CB_ERR)
			SendDlgItemMessage(hDlg,IDC_DEPTH,CB_SETCURSEL,iIndex,0);
		else
			SendDlgItemMessage(hDlg,IDC_DEPTH,CB_SETCURSEL,0,0);

		SendDlgItemMessage(hDlg,IDC_VOLSLIDER,TBM_SETRANGE,TRUE,MAKELONG(85,100));
		SendDlgItemMessage(hDlg,IDC_BRIGHTSLIDER,TBM_SETRANGE,TRUE,MAKELONG(0,100));
		SendDlgItemMessage(hDlg,IDC_CONTRASTSLIDER,TBM_SETRANGE,TRUE,MAKELONG(0,200));

		SendDlgItemMessage(hDlg,IDC_VOLSLIDER,TBM_SETPOS,TRUE,pSettings->Volume/100);
		SendDlgItemMessage(hDlg,IDC_BRIGHTSLIDER,TBM_SETPOS,TRUE,pSettings->Brightness/100);
		SendDlgItemMessage(hDlg,IDC_CONTRASTSLIDER,TBM_SETPOS,TRUE,pSettings->Contrast/100);

		SendDlgItemMessage(hDlg,IDC_NET_TOUT,CB_SETCURSEL,pSettings->NET_TOUT,0);

		CheckDlgButton(hDlg,IDC_DBGMODE,pSettings->DBGMODE? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_DPPROTO,pSettings->USEDPPROTO? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_DINPUT,pSettings->USEDINPUT? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_TBUFFER,pSettings->USETBUFFER? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_DUPSBUFF,pSettings->DUPSBUFF? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_SPRVIDEO,pSettings->SPRVIDEO? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_ASYNCENUM,pSettings->ASYNCENUM? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_ENUMEVERY,pSettings->ENUMEVERY? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_ENABLESON,pSettings->ENABLESON? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_HISOUND,pSettings->HISOUND? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_TELCLNT,pSettings->TELCLNT? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_TELSVR,pSettings->TELSVR? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_SHOWPING,pSettings->SHOWPING? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_LOADSKIN,pSettings->LOADSKIN? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_ALLOWSPECTATORS,pSettings->ALLOWSPECTATORS? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_VSYNC,pSettings->VSYNC? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_MIGRATEHOST,pSettings->MIGRATEHOST? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_BOMBTCOLOR,pSettings->BOMBTCOLOR? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_PLAYERTCOLOR,pSettings->PLAYERTCOLOR? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_VOICE,pSettings->VOICE? BST_CHECKED:BST_UNCHECKED);

		CheckRadioButton(hDlg,IDC_RADIO1,IDC_RADIO2,pSettings->USEFLIP? IDC_RADIO1:IDC_RADIO2);

		EnableWindow(GetDlgItem(hDlg,IDC_RADIO1),!pSettings->DBGMODE);
		EnableWindow(GetDlgItem(hDlg,IDC_RADIO2),!pSettings->DBGMODE);
		EnableWindow(GetDlgItem(hDlg,IDC_DEPTH),!pSettings->DBGMODE);
		EnableWindow(GetDlgItem(hDlg,IDC_TBUFFER),pSettings->USEFLIP&&!pSettings->DBGMODE);
		EnableWindow(GetDlgItem(hDlg,IDC_SPRVIDEO),!pSettings->DBGMODE);
		EnableWindow(GetDlgItem(hDlg,IDC_DUPSBUFF),pSettings->ENABLESON);
		EnableWindow(GetDlgItem(hDlg,IDC_VOLSLIDER),pSettings->ENABLESON);
		EnableWindow(GetDlgItem(hDlg,IDC_HISOUND),pSettings->ENABLESON);
		EnableWindow(GetDlgItem(hDlg,IDC_BRIGHTSLIDER),!pSettings->DBGMODE);
		EnableWindow(GetDlgItem(hDlg,IDC_CONTRASTSLIDER),!pSettings->DBGMODE);
		if(bClrCtrl)
		{
			ShowWindow(GetDlgItem(hDlg,IDC_BRIGHTLBL),SW_SHOW);
			ShowWindow(GetDlgItem(hDlg,IDC_CONTRASTLBL),SW_SHOW);
			ShowWindow(GetDlgItem(hDlg,IDC_BRIGHTSLIDER),SW_SHOW);
			ShowWindow(GetDlgItem(hDlg,IDC_CONTRASTSLIDER),SW_SHOW);
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,IDC_BRIGHTLBL),SW_HIDE);
			ShowWindow(GetDlgItem(hDlg,IDC_CONTRASTLBL),SW_HIDE);
			ShowWindow(GetDlgItem(hDlg,IDC_BRIGHTSLIDER),SW_HIDE);
			ShowWindow(GetDlgItem(hDlg,IDC_CONTRASTSLIDER),SW_HIDE);
		}

		;
//		SendMessage(GetDlgItem(hDlg,IDC_TEVERY),WM_SETTEXT,0,(LPARAM)_ltoa(pSettings->TEVERY,Buff,10));
		SetWindowText(GetDlgItem(hDlg,IDC_FILE_BLOCK_SIZE),_ltoa(pSettings->FILE_BLOCK_SIZE,Buff,10));
		PostMessage(GetDlgItem(hDlg,IDC_FILE_BLOCK_SIZE),EM_SETLIMITTEXT,5,0);
		SetWindowText(GetDlgItem(hDlg,IDC_TEVERY),_ltoa(pSettings->TEVERY,Buff,10));
		PostMessage(GetDlgItem(hDlg,IDC_TEVERY),EM_SETLIMITTEXT,5,0);
		EnableWindow(GetDlgItem(hDlg,IDC_TEVERY),pSettings->ENUMEVERY);
#ifdef GOA
		EnableWindow(GetDlgItem(hDlg,IDC_DPPROTO),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_MIGRATEHOST),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDB_GAMEOPT),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDB_MAPSELECT),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_MAPDELAY),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_ALLOWSPECTATORS),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_FILE_BLOCK_SIZE),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_TELSVR),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_TELSVR),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDB_CFGMGR),FALSE);
		SetWindowText(GetDlgItem(hDlg,IDOK),"GOA!");
#ifdef LANG_FR
		SetWindowText(GetDlgItem(hDlg,IDCANCEL),"Quitter");
#else
		SetWindowText(GetDlgItem(hDlg,IDCANCEL),"Quit");
#endif
#endif
		SetWindowText(GetDlgItem(hDlg,IDC_MAPDELAY),_ltoa(pSettings->MAPDELAY,Buff,10));
		PostMessage(GetDlgItem(hDlg,IDC_MAPDELAY),EM_SETLIMITTEXT,2,0);
		SetFocus(hDlg);
return;
}

void GetOptionsDialog(HWND hDlg,struct SETTINGS* pSettings)
{
	int iIndex;
	iIndex=SendDlgItemMessage(hDlg,IDC_DEPTH,CB_GETCURSEL,0,0);
	pSettings->Depth=SendDlgItemMessage(hDlg,IDC_DEPTH,CB_GETITEMDATA,iIndex,0);

	iIndex=SendDlgItemMessage(hDlg,IDC_NET_TOUT,CB_GETCURSEL,0,0);
	pSettings->NET_TOUT=iIndex;
	/*
	switch (iIndex)
	{
	case 0:
		pSettings->NET_TOUT=250;
		break;
	case 1:
		pSettings->NET_TOUT=200;
		break;
	case 2:
		pSettings->NET_TOUT=150;
		break;
	case 3:
		pSettings->NET_TOUT=75;
		break;
	case 4:
		pSettings->NET_TOUT=25;
		break;
	default:
		pSettings->NET_TOUT=150;
		break;
	}
	*/

	pSettings->Volume=100*SendDlgItemMessage(hDlg,IDC_VOLSLIDER,TBM_GETPOS,0,0);
	pSettings->Brightness=100*SendDlgItemMessage(hDlg,IDC_BRIGHTSLIDER,TBM_GETPOS,0,0);
	pSettings->Contrast=100*SendDlgItemMessage(hDlg,IDC_CONTRASTSLIDER,TBM_GETPOS,0,0);

	pSettings->DBGMODE=(IsDlgButtonChecked(hDlg,IDC_DBGMODE)==BST_CHECKED);
	pSettings->USEDPPROTO=(IsDlgButtonChecked(hDlg,IDC_DPPROTO)==BST_CHECKED);
	pSettings->USEDINPUT=(IsDlgButtonChecked(hDlg,IDC_DINPUT)==BST_CHECKED);
	pSettings->USEFLIP=(IsDlgButtonChecked(hDlg,IDC_RADIO1)==BST_CHECKED);
	pSettings->USETBUFFER=(IsDlgButtonChecked(hDlg,IDC_TBUFFER)==BST_CHECKED);
	pSettings->DUPSBUFF=(IsDlgButtonChecked(hDlg,IDC_DUPSBUFF)==BST_CHECKED);
	pSettings->SPRVIDEO=(IsDlgButtonChecked(hDlg,IDC_SPRVIDEO)==BST_CHECKED);
	pSettings->ASYNCENUM=(IsDlgButtonChecked(hDlg,IDC_ASYNCENUM)==BST_CHECKED);
	pSettings->ENUMEVERY=(IsDlgButtonChecked(hDlg,IDC_ENUMEVERY)==BST_CHECKED);
	pSettings->ENABLESON=(IsDlgButtonChecked(hDlg,IDC_ENABLESON)==BST_CHECKED);
	pSettings->HISOUND=(IsDlgButtonChecked(hDlg,IDC_HISOUND)==BST_CHECKED);
	pSettings->TELCLNT=(IsDlgButtonChecked(hDlg,IDC_TELCLNT)==BST_CHECKED);
	pSettings->TELSVR=(IsDlgButtonChecked(hDlg,IDC_TELSVR)==BST_CHECKED);
	pSettings->SHOWPING=(IsDlgButtonChecked(hDlg,IDC_SHOWPING)==BST_CHECKED);
	pSettings->LOADSKIN=(IsDlgButtonChecked(hDlg,IDC_LOADSKIN)==BST_CHECKED);
	pSettings->ALLOWSPECTATORS=(IsDlgButtonChecked(hDlg,IDC_ALLOWSPECTATORS)==BST_CHECKED);
	pSettings->VSYNC=(IsDlgButtonChecked(hDlg,IDC_VSYNC)==BST_CHECKED);
	pSettings->MIGRATEHOST=(IsDlgButtonChecked(hDlg,IDC_MIGRATEHOST)==BST_CHECKED);
	pSettings->BOMBTCOLOR=(IsDlgButtonChecked(hDlg,IDC_BOMBTCOLOR)==BST_CHECKED);
	pSettings->PLAYERTCOLOR=(IsDlgButtonChecked(hDlg,IDC_PLAYERTCOLOR)==BST_CHECKED);
	pSettings->VOICE=(IsDlgButtonChecked(hDlg,IDC_VOICE)==BST_CHECKED);

	GetWindowText(GetDlgItem(hDlg,IDC_FILE_BLOCK_SIZE),Buff,6);
	pSettings->FILE_BLOCK_SIZE=atodw(Buff);
	if(pSettings->FILE_BLOCK_SIZE>32768) 
		pSettings->FILE_BLOCK_SIZE=32768;
	if(pSettings->FILE_BLOCK_SIZE<64) 
		pSettings->FILE_BLOCK_SIZE=64;
	GetWindowText(GetDlgItem(hDlg,IDC_TEVERY),Buff,6);
	pSettings->TEVERY=atol(Buff);
	if((pSettings->TEVERY<500)||(pSettings->TEVERY>=100000)) 
		pSettings->TEVERY=500;
	GetWindowText(GetDlgItem(hDlg,IDC_MAPDELAY),Buff,6);
	pSettings->MAPDELAY=atol(Buff);
	if(pSettings->MAPDELAY>99)
		pSettings->MAPDELAY=99;
return;
}

BOOL CALLBACK DlgProcOptions(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	struct SETTINGS SettingsL;
	switch (msg)
	{
	case WM_INITDIALOG:
		SettingsL=Settings;
		TmpGO=Settings.GO;
		TmpC=Settings.C;
		TmpTM=Settings.TM;
#ifdef GOA
		SettingsL.USEDPPROTO=TRUE;
#endif
		SetOptionsDialog(hDlg,&SettingsL);
		return(FALSE);
		break;

	case WM_COMMAND:
		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_WEBSITE))
		{
			ShellExecute(0,NULL,"http://perso.club-internet.fr/fabienpg",0,0,SW_SHOWNORMAL);
			SetFocus(GetDlgItem(hDlg,IDOK));
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_CFGMGR))
		{
			GetOptionsDialog(hDlg,&Settings);
			Settings.C=TmpC;
			SaveConfigFile(szCurConfig,&Settings);
			DialogBox(0,(LPCSTR)IDD_CONFIGS,hDlg,(DLGPROC)DlgProcConfigs);
			SendMessage(hDlg,WM_INITDIALOG,0,0);
			SetFocus(GetDlgItem(hDlg,IDOK));
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_SKINS))
		{
			DialogBox(0,(LPCSTR)IDD_SKINS,hDlg,(DLGPROC)DlgProcSkins);
			SetFocus(GetDlgItem(hDlg,IDOK));
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_MAPSELECT))
		{
			DialogBox(0,(LPCSTR)IDD_MAPS,hDlg,(DLGPROC)DlgProcMaps);
			SetFocus(GetDlgItem(hDlg,IDOK));
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_GAMEOPT))
		{
			TmpGO1=TmpGO;
			DialogBox(0,(LPCSTR)IDD_GAMEOPT,hDlg,(DLGPROC)DlgProcGameOptions);
			SetFocus(GetDlgItem(hDlg,IDOK));
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_TXTMSG))
		{
			TmpTM1=TmpTM;
			DialogBox(0,(LPCSTR)IDD_TXTMSG,hDlg,(DLGPROC)DlgProcTxtMsg);
			SetFocus(GetDlgItem(hDlg,IDOK));
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_CONTROLS))
		{
			DialogBox(0,(LPCSTR)IDD_CONTROLS,hDlg,(DLGPROC)DlgProcControls);
			SetFocus(GetDlgItem(hDlg,IDOK));
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_RESET))
		{
			SetDefaultOptions(&SettingsL);
			SettingsL.GO=TmpGO;
			SettingsL.C=TmpC;
			SettingsL.TM=TmpTM;
			SetOptionsDialog(hDlg,&SettingsL);
			SetFocus(GetDlgItem(hDlg,IDOK));
			break;
		}

		switch(wParam)
		{
			case IDOK:
				GetOptionsDialog(hDlg,&Settings);
				Settings.GO=TmpGO;
				Settings.C=TmpC;
				Settings.TM=TmpTM;
				SaveConfigFile(szCurConfig,&Settings);
				EndDialog(hDlg,0);
			break;

			case IDCANCEL:
				EndDialog(hDlg,-1);
			break;

			default:
				SettingsL.DBGMODE=(IsDlgButtonChecked(hDlg,IDC_DBGMODE)==BST_CHECKED);
				SettingsL.USEDPPROTO=(IsDlgButtonChecked(hDlg,IDC_DPPROTO)==BST_CHECKED);
				SettingsL.USEDINPUT=(IsDlgButtonChecked(hDlg,IDC_DINPUT)==BST_CHECKED);
				SettingsL.USEFLIP=(IsDlgButtonChecked(hDlg,IDC_RADIO1)==BST_CHECKED);
				SettingsL.USETBUFFER=(IsDlgButtonChecked(hDlg,IDC_TBUFFER)==BST_CHECKED);
				SettingsL.DUPSBUFF=(IsDlgButtonChecked(hDlg,IDC_DUPSBUFF)==BST_CHECKED);
				SettingsL.SPRVIDEO=(IsDlgButtonChecked(hDlg,IDC_SPRVIDEO)==BST_CHECKED);
				SettingsL.ASYNCENUM=(IsDlgButtonChecked(hDlg,IDC_ASYNCENUM)==BST_CHECKED);
				SettingsL.ENUMEVERY=(IsDlgButtonChecked(hDlg,IDC_ENUMEVERY)==BST_CHECKED);
				SettingsL.ENABLESON=(IsDlgButtonChecked(hDlg,IDC_ENABLESON)==BST_CHECKED);
				SettingsL.VSYNC=(IsDlgButtonChecked(hDlg,IDC_VSYNC)==BST_CHECKED);

				EnableWindow(GetDlgItem(hDlg,IDC_RADIO1),!SettingsL.DBGMODE);
				EnableWindow(GetDlgItem(hDlg,IDC_RADIO2),!SettingsL.DBGMODE);
				EnableWindow(GetDlgItem(hDlg,IDC_DEPTH),!SettingsL.DBGMODE);
				EnableWindow(GetDlgItem(hDlg,IDC_TBUFFER),SettingsL.USEFLIP&&!SettingsL.DBGMODE);
				EnableWindow(GetDlgItem(hDlg,IDC_SPRVIDEO),!SettingsL.DBGMODE);
				EnableWindow(GetDlgItem(hDlg,IDC_TEVERY),SettingsL.ENUMEVERY);
				EnableWindow(GetDlgItem(hDlg,IDC_DUPSBUFF),SettingsL.ENABLESON);
				EnableWindow(GetDlgItem(hDlg,IDC_VOLSLIDER),SettingsL.ENABLESON);
				EnableWindow(GetDlgItem(hDlg,IDC_HISOUND),SettingsL.ENABLESON);
				EnableWindow(GetDlgItem(hDlg,IDC_VOICE),SettingsL.ENABLESON);
				EnableWindow(GetDlgItem(hDlg,IDC_BRIGHTSLIDER),!SettingsL.DBGMODE);
				EnableWindow(GetDlgItem(hDlg,IDC_CONTRASTSLIDER),!SettingsL.DBGMODE);
			break;
		}
		break;
	}

	return (FALSE);
}

BOOL CALLBACK DlgProcPressKey(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BYTE DIkeyboard[256];
	BYTE keyboard[256];
	BYTE vkt=0;
	BYTE DIvkt=0;
	BYTE tcnt=0;
	DWORD vret;
	BOOL DIok;

	switch (msg)
	{
	case WM_INITDIALOG:
		PurgeKeyboard();
		SetTimer(hDlg,1,100,0);
		return TRUE;
		break;

	case WM_TIMER:
		GetKeyboardState(keyboard);
		if(lpDIDevice!=0)
		{
			lpDIDevice->Acquire();
			lpDIDevice->GetDeviceState(256,DIkeyboard);
		}
		tcnt=0;
		for(i=0;(i<256)&&(vkt==0);i++)
		{
			if((keyboard[tcnt]&0x80)&&(tcnt!=0xc))
				vkt=tcnt;
			tcnt++;
		}

		DIok=TRUE;
		if(lpDIDevice!=0)
		{
			tcnt=0;
			for(i=0;(i<256)&&(DIvkt==0);i++)
			{
				if(DIkeyboard[tcnt]&0x80)
					DIvkt=tcnt;
				tcnt++;
			}
		}
		else
		{
			DIvkt=0xff;
			DIok=0;
		}

		if(DIvkt==0)
			DIvkt=0xff;
		if((vkt!=0)&&(DIvkt!=0))
		{
			KillTimer(hDlg,1);
			vret=vkt;
			vret=(vret<<8)|(DWORD)DIvkt;
			if(DIok)
				EndDialog(hDlg,vret|0xff0000);
			else
				EndDialog(hDlg,vret);
		}
		break;

	}
//	if(GetAsyncKeyState(VK_ESCAPE)) EndDialog(hDlg,0);
	return (FALSE);
}

BOOL CALLBACK DlgProcControls(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static nCtrl=0;
	DWORD rc;
	switch (msg)
	{
	case WM_INITDIALOG:
		TmpC1=TmpC;
#ifndef DX3
		CoCreateInstance(CLSID_DirectInput,
			NULL, CLSCTX_INPROC_SERVER, IID_IDirectInput2, (void**)&lpDI);
#else
		CoCreateInstance(CLSID_DirectInput,
			NULL, CLSCTX_INPROC_SERVER, IID_IDirectInput, (void**)&lpDI);
#endif
		if(lpDI==0) return TRUE;

		if(lpDI->Initialize(GetModuleHandle(0),DIRECTINPUT_VERSION)!=DI_OK)
			ErrorMsg(LSZ_DI_CANTINIT,LSZ_DI_INIT);
		lpDI->CreateDevice(GUID_SysKeyboard,&lpDIDevice,0);
		if(lpDIDevice==0) 
		{
			lpDI->Release();
			lpDI=NULL;
			return TRUE;
		}
		lpDIDevice->SetDataFormat(&c_dfDIKeyboard);
		lpDIDevice->SetCooperativeLevel(hDlg,DISCL_BACKGROUND|DISCL_NONEXCLUSIVE);
		lpDIDevice->Acquire();

		nCtrl=0;
		CheckRadioButton(hDlg,IDC_CTRL1,IDC_CTRL4,IDC_CTRL1);

		SetWindowText(GetDlgItem(hDlg,IDC_UPLBL),KeyName(TmpC.Key_UP[0]));
		SetWindowText(GetDlgItem(hDlg,IDC_DOWNLBL),KeyName(TmpC.Key_DOWN[0]));
		SetWindowText(GetDlgItem(hDlg,IDC_LEFTLBL),KeyName(TmpC.Key_LEFT[0]));
		SetWindowText(GetDlgItem(hDlg,IDC_RIGHTLBL),KeyName(TmpC.Key_RIGHT[0]));
		SetWindowText(GetDlgItem(hDlg,IDC_FIRE1LBL),KeyName(TmpC.Key_FIRE1[0]));
		SetWindowText(GetDlgItem(hDlg,IDC_FIRE2LBL),KeyName(TmpC.Key_FIRE2[0]));
		SetWindowText(GetDlgItem(hDlg,IDC_FIRE3LBL),KeyName(TmpC.Key_FIRE3[0]));
		SetFocus(hDlg);
		return(FALSE);
		break;

	case WM_COMMAND:
		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_RESETCTRL))
		{
			SetDefaultControls(&TmpC);
			SetWindowText(GetDlgItem(hDlg,IDC_UPLBL),KeyName(TmpC.Key_UP[nCtrl]));
			SetWindowText(GetDlgItem(hDlg,IDC_DOWNLBL),KeyName(TmpC.Key_DOWN[nCtrl]));
			SetWindowText(GetDlgItem(hDlg,IDC_LEFTLBL),KeyName(TmpC.Key_LEFT[nCtrl]));
			SetWindowText(GetDlgItem(hDlg,IDC_RIGHTLBL),KeyName(TmpC.Key_RIGHT[nCtrl]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE1LBL),KeyName(TmpC.Key_FIRE1[nCtrl]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE2LBL),KeyName(TmpC.Key_FIRE2[nCtrl]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE3LBL),KeyName(TmpC.Key_FIRE3[nCtrl]));
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDC_CTRL1))
		{
			nCtrl=0;
			CheckRadioButton(hDlg,IDC_CTRL1,IDC_CTRL4,IDC_CTRL1);
			SetWindowText(GetDlgItem(hDlg,IDC_UPLBL),KeyName(TmpC.Key_UP[0]));
			SetWindowText(GetDlgItem(hDlg,IDC_DOWNLBL),KeyName(TmpC.Key_DOWN[0]));
			SetWindowText(GetDlgItem(hDlg,IDC_LEFTLBL),KeyName(TmpC.Key_LEFT[0]));
			SetWindowText(GetDlgItem(hDlg,IDC_RIGHTLBL),KeyName(TmpC.Key_RIGHT[0]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE1LBL),KeyName(TmpC.Key_FIRE1[0]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE2LBL),KeyName(TmpC.Key_FIRE2[0]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE3LBL),KeyName(TmpC.Key_FIRE3[0]));
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDC_CTRL2))
		{
			nCtrl=1;
			CheckRadioButton(hDlg,IDC_CTRL1,IDC_CTRL4,IDC_CTRL2);
			SetWindowText(GetDlgItem(hDlg,IDC_UPLBL),KeyName(TmpC.Key_UP[1]));
			SetWindowText(GetDlgItem(hDlg,IDC_DOWNLBL),KeyName(TmpC.Key_DOWN[1]));
			SetWindowText(GetDlgItem(hDlg,IDC_LEFTLBL),KeyName(TmpC.Key_LEFT[1]));
			SetWindowText(GetDlgItem(hDlg,IDC_RIGHTLBL),KeyName(TmpC.Key_RIGHT[1]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE1LBL),KeyName(TmpC.Key_FIRE1[1]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE2LBL),KeyName(TmpC.Key_FIRE2[1]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE3LBL),KeyName(TmpC.Key_FIRE3[1]));
			break;
		}
		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDC_CTRL3))
		{
			nCtrl=2;
			CheckRadioButton(hDlg,IDC_CTRL1,IDC_CTRL4,IDC_CTRL3);
			SetWindowText(GetDlgItem(hDlg,IDC_UPLBL),KeyName(TmpC.Key_UP[2]));
			SetWindowText(GetDlgItem(hDlg,IDC_DOWNLBL),KeyName(TmpC.Key_DOWN[2]));
			SetWindowText(GetDlgItem(hDlg,IDC_LEFTLBL),KeyName(TmpC.Key_LEFT[2]));
			SetWindowText(GetDlgItem(hDlg,IDC_RIGHTLBL),KeyName(TmpC.Key_RIGHT[2]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE1LBL),KeyName(TmpC.Key_FIRE1[2]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE2LBL),KeyName(TmpC.Key_FIRE2[2]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE3LBL),KeyName(TmpC.Key_FIRE3[2]));
			break;
		}
		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDC_CTRL4))
		{
			nCtrl=3;
			CheckRadioButton(hDlg,IDC_CTRL1,IDC_CTRL4,IDC_CTRL4);
			SetWindowText(GetDlgItem(hDlg,IDC_UPLBL),KeyName(TmpC.Key_UP[3]));
			SetWindowText(GetDlgItem(hDlg,IDC_DOWNLBL),KeyName(TmpC.Key_DOWN[3]));
			SetWindowText(GetDlgItem(hDlg,IDC_LEFTLBL),KeyName(TmpC.Key_LEFT[3]));
			SetWindowText(GetDlgItem(hDlg,IDC_RIGHTLBL),KeyName(TmpC.Key_RIGHT[3]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE1LBL),KeyName(TmpC.Key_FIRE1[3]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE2LBL),KeyName(TmpC.Key_FIRE2[3]));
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE3LBL),KeyName(TmpC.Key_FIRE3[3]));
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_UP))
		{
			rc=DialogBox(0,(LPCSTR)IDD_PRESSKEY,hDlg,(DLGPROC)DlgProcPressKey);
			TmpC.Key_UP[nCtrl]=(BYTE)(rc>>8);
			if((rc&0xff0000)!=0)
				TmpC.DIKey_UP[nCtrl]=(BYTE)(rc&0xff);
			SetWindowText(GetDlgItem(hDlg,IDC_UPLBL),KeyName(TmpC.Key_UP[nCtrl]));
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_DOWN))
		{
			rc=DialogBox(0,(LPCSTR)IDD_PRESSKEY,hDlg,(DLGPROC)DlgProcPressKey);
			TmpC.Key_DOWN[nCtrl]=(BYTE)(rc>>8);
			if((rc&0xff0000)!=0)
				TmpC.DIKey_DOWN[nCtrl]=(BYTE)(rc&0xff);
			SetWindowText(GetDlgItem(hDlg,IDC_DOWNLBL),KeyName(TmpC.Key_DOWN[nCtrl]));
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_LEFT))
		{
			rc=DialogBox(0,(LPCSTR)IDD_PRESSKEY,hDlg,(DLGPROC)DlgProcPressKey);
			TmpC.Key_LEFT[nCtrl]=(BYTE)(rc>>8);
			if((rc&0xff0000)!=0)
				TmpC.DIKey_LEFT[nCtrl]=(BYTE)(rc&0xff);
			SetWindowText(GetDlgItem(hDlg,IDC_LEFTLBL),KeyName(TmpC.Key_LEFT[nCtrl]));
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_RIGHT))
		{
			rc=DialogBox(0,(LPCSTR)IDD_PRESSKEY,hDlg,(DLGPROC)DlgProcPressKey);
			TmpC.Key_RIGHT[nCtrl]=(BYTE)(rc>>8);
			if((rc&0xff0000)!=0)
				TmpC.DIKey_RIGHT[nCtrl]=(BYTE)(rc&0xff);
			SetWindowText(GetDlgItem(hDlg,IDC_RIGHTLBL),KeyName(TmpC.Key_RIGHT[nCtrl]));
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_FIRE1))
		{
			rc=DialogBox(0,(LPCSTR)IDD_PRESSKEY,hDlg,(DLGPROC)DlgProcPressKey);
			TmpC.Key_FIRE1[nCtrl]=(BYTE)(rc>>8);
			if((rc&0xff0000)!=0)
				TmpC.DIKey_FIRE1[nCtrl]=(BYTE)(rc&0xff);
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE1LBL),KeyName(TmpC.Key_FIRE1[nCtrl]));
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_FIRE2))
		{
			rc=DialogBox(0,(LPCSTR)IDD_PRESSKEY,hDlg,(DLGPROC)DlgProcPressKey);
			TmpC.Key_FIRE2[nCtrl]=(BYTE)(rc>>8);
			if((rc&0xff0000)!=0)
				TmpC.DIKey_FIRE2[nCtrl]=(BYTE)(rc&0xff);
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE2LBL),KeyName(TmpC.Key_FIRE2[nCtrl]));
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_FIRE3))
		{
			rc=DialogBox(0,(LPCSTR)IDD_PRESSKEY,hDlg,(DLGPROC)DlgProcPressKey);
			TmpC.Key_FIRE3[nCtrl]=(BYTE)(rc>>8);
			if((rc&0xff0000)!=0)
				TmpC.DIKey_FIRE3[nCtrl]=(BYTE)(rc&0xff);
			SetWindowText(GetDlgItem(hDlg,IDC_FIRE3LBL),KeyName(TmpC.Key_FIRE3[nCtrl]));
		}

		switch(wParam)
		{
			case IDOK:
				if(lpDIDevice!=0)
				{
					lpDIDevice->Unacquire();
					lpDIDevice->Release();
					lpDIDevice=0;
				}
				if(lpDI!=0)
				{
					lpDI->Release();
					lpDI=0;
				}
				EndDialog(hDlg,TRUE);
			break;

			case IDCANCEL:
				if(lpDIDevice!=0)
				{
					lpDIDevice->Unacquire();
					lpDIDevice->Release();
					lpDIDevice=0;
				}
				if(lpDI!=0)
				{
					lpDI->Release();
					lpDI=0;
				}
				TmpC=TmpC1;
				EndDialog(hDlg,0);
			break;

		}
		break;

	}
	return (FALSE);
}

void ConsoleExecute(const char* szCommand)
{
	if(strcmp(szCommand,"HELP\r\n")==0)
	{
		OutFile(LSZ_DED_HELP);
		OutFile(LSZ_DED_HELPCMDLST);
	}
	else
	if(strcmp(szCommand,"MINIMIZE\r\n")==0)
		ShowWindow(hWndCon,SW_MINIMIZE);
	else
	if(strcmp(szCommand,"PING\r\n")==0)
	{
		if(NumPlayers!=0)
		{
			SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_PING,0,0,0,0);
			bShowPing=TRUE;
		}
		else
			OutFile(LSZ_DP_PING_NOPLAYERS);
	}
	else
	if(strcmp(szCommand,"HIDE\r\n")==0)
		PostMessage(hWndCon,WM_CLOSE,0,0);
	else
	if((strcmp(szCommand,"QUIT\r\n")==0)||(strcmp(szCommand,"CLOSE\r\n")==0))
		bExit=TRUE;
	else
	if(strcmp(szCommand,"RESET\r\n")==0)
		bReset=TRUE;
	else
	if(strcmp(szCommand,"STATS\r\n")==0)
	{
		OutFile(LSZ_DED_SESSIONSTATS);
		OutFileEx2(LSZ_DED_SESSIONNAME,szSessionName);
		OutFileEx2(LSZ_DED_NUMPLAYERS,NumPlayers);
		for(i=0;i<NumPlayers;i++)
		{
			OutFileEx2("%s",Bombers[i].Nom);
			OutFileEx2("Frags: %ld",Bombers[i].Frag);
			OutFileEx2("AK: %ld",Bombers[i].AK);
			OutFileEx2("Points: %ld",BPOINTS(i));
		}
		OutFileLF();
	}
	else
		OutFile(LSZ_DED_UNKNOWNCMD);
	return;
}

BOOL CALLBACK DlgProcStart(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
		HDC hDC;
		SIZE bmpsze;
		hDC=GetDC(GetDlgItem(hDlg,IDC_LOGO));
		GetBitmapDimensionEx(hBmpLogo,&bmpsze);
		BitBlt(hDC,0,0,bmpsze.cx,bmpsze.cy,hDCLogo,0,0,SRCCOPY);
		ReleaseDC(GetDlgItem(hDlg,IDC_LOGO),hDC);
		break;

	case WM_INITDIALOG:
		hDCLogo=CreateCompatibleDC(NULL);
		hBmpLogo=(HBITMAP)LoadImage(hInst,(LPCSTR)IDBMP_LOGO,IMAGE_BITMAP,0,0,0);
		hBmpOldLogo=(HBITMAP)SelectObject(hDCLogo,hBmpLogo);

		CheckRadioButton(hDlg,IDC_RADIO1,IDC_RADIO2,IDC_RADIO1);
		sprintf(Buff,"BOMBER                        v%ld.%ld.%ld.%ld",dwVersion>>24,(dwVersion>>16)&255,(dwVersion>>8)&255,dwVersion&255);
		SetWindowText(hDlg,Buff);
		SetTimer(hDlg,1,DEMO_TOUT,NULL);
		SetFocus(GetDlgItem(hDlg,IDOK));
		return(FALSE);
		break;

	case WM_TIMER:
		EndDialog(hDlg,2);
		break;

	case WM_COMMAND:
		KillTimer(hDlg,1);
		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_SCORES))
		{
			EnableWindow(hDlg,FALSE);
			MsgBoxScores();
			EnableWindow(hDlg,TRUE);
			SetFocus(GetDlgItem(hDlg,IDOK));
			SetTimer(hDlg,1,DEMO_TOUT,NULL);
			break;
		}

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_OPTIONS))
		{
			struct SETTINGS SAVSettings;
			SAVSettings=Settings;
			if(0!=DialogBox(0,(LPCSTR)IDD_OPTIONS,hDlg,(DLGPROC)DlgProcOptions))
				Settings=SAVSettings;
			SetFocus(GetDlgItem(hDlg,IDOK));
			SetTimer(hDlg,1,DEMO_TOUT,NULL);
			break;
		}

		switch(wParam)
		{
			case IDOK:
				SelectObject(hDCLogo,hBmpOldLogo);
				DeleteObject(hBmpLogo);
				DeleteDC(hDCLogo);

				if(IsDlgButtonChecked(hDlg,IDC_RADIO1)==BST_CHECKED)
					EndDialog(hDlg,0);
				else
				if(IsDlgButtonChecked(hDlg,IDC_RADIO2)==BST_CHECKED)
					EndDialog(hDlg,1);
				else
					EndDialog(hDlg,-1);
			break;

			case IDCANCEL:
				SelectObject(hDCLogo,hBmpOldLogo);
				DeleteObject(hBmpLogo);
				DeleteDC(hDCLogo);
				EndDialog(hDlg,-1);
			break;

		}
		break;

	}
	return (FALSE);
}

BOOL CALLBACK DlgProcConsole(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		SetFocus(hDlg);
		return(TRUE);
		break;

	case WM_COMMAND:
		if(LOWORD(wParam)==IDC_CONIN)
			if(HIWORD(wParam)==EN_CHANGE)
			{
				GetWindowText((HWND)lParam,Buff,BMAX_PATH);
				if(strstr(Buff,"\r\n"))
				{
					SetWindowText((HWND)lParam,"");
					ConsoleExecute(strtoupper(Buff));
				}
			}
		break;

	case WM_CLOSE:
		DisableConsole();
		break;

	}
	return (FALSE);
}

BOOL CALLBACK DlgProcWaitSession(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DPSESSIONDESC2	dpdesc;
	DWORD*			sDesc;

	switch (msg)
	{
	case WM_INITDIALOG:
		GetAsyncKeyState(VK_ESCAPE);
		SetTimer(hDlg,1,500,0);
		SetFocus(hDlg);
		return(TRUE);
		break;

	case WM_TIMER:
		dpdesc.dwSize=sizeof(DPSESSIONDESC2);
		lpDP->GetSessionDesc(0,&dpdesc.dwSize);
		sDesc=new DWORD[dpdesc.dwSize];
		sDesc[0]=dpdesc.dwSize;
		lpDP->GetSessionDesc(sDesc,&dpdesc.dwSize);
		dpdesc=*(DPSESSIONDESC2*)sDesc;
		SAFE_DELETE_ARRAY(sDesc)
		if(dpdesc.dwUser3&BS_PLAYING)
		{
			KillTimer(hDlg,1);
			EndDialog(hDlg,TRUE);
		}
		if(GetAsyncKeyState(VK_ESCAPE))
		{
			KillTimer(hDlg,1);
			EndDialog(hDlg,0);
		}
		break;
	}
	return (FALSE);
}

BOOL CALLBACK DlgProcWaitSPlaying(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DPSESSIONDESC2	dpdesc;
	DWORD*			sDesc;

	switch (msg)
	{
	case WM_INITDIALOG:
		/*
		if(bShortcutConnect)
		{
			EndDialog(hDlg,TRUE);
			return TRUE;
		}
		*/
		GetAsyncKeyState(VK_ESCAPE);
		SetTimer(hDlg,1,500,0);
		SetFocus(hDlg);
		return TRUE;
		break;

	case WM_TIMER:
		KillTimer(hDlg,1);
		dpdesc.dwSize=sizeof(DPSESSIONDESC2);
		lpDP->GetSessionDesc(0,&dpdesc.dwSize);
		sDesc=new DWORD[dpdesc.dwSize];
		sDesc[0]=dpdesc.dwSize;
		lpDP->GetSessionDesc(sDesc,&dpdesc.dwSize);
		dpdesc=*(DPSESSIONDESC2*)sDesc;
		SAFE_DELETE_ARRAY(sDesc);
		if(dpdesc.dwUser1!=dwPlayingSession)
		{
			KillTimer(hDlg,1);
			EndDialog(hDlg,TRUE);
			return FALSE;
		}
		if(GetAsyncKeyState(VK_ESCAPE))
		{
			KillTimer(hDlg,1);
			EndDialog(hDlg,0);
			return FALSE;
		}
		SetTimer(hDlg,1,500,0);
		break;
	}
	return (FALSE);
}

BOOL CALLBACK DlgProcNumPlayers(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_INITDIALOG:
		switch(dwLastNumPlayers)
		{
		case 2:
			CheckRadioButton(hDlg,IDC_TWO,IDC_FOUR,IDC_TWO);
			break;
		case 3:
			CheckRadioButton(hDlg,IDC_TWO,IDC_FOUR,IDC_THREE);
			break;
		case 4:
			CheckRadioButton(hDlg,IDC_TWO,IDC_FOUR,IDC_FOUR);
			break;
		default:
			CheckRadioButton(hDlg,IDC_TWO,IDC_FOUR,IDC_TWO);
			break;
		}

		SetFocus(hDlg);
		return(FALSE);
		break;

	case WM_COMMAND:
		switch(wParam)
		{
			case IDOK:
				if(IsDlgButtonChecked(hDlg,IDC_TWO)==BST_CHECKED)
					EndDialog(hDlg,dwLastNumPlayers=2);
				else
				if(IsDlgButtonChecked(hDlg,IDC_THREE)==BST_CHECKED)
					EndDialog(hDlg,dwLastNumPlayers=3);
				else
				if(IsDlgButtonChecked(hDlg,IDC_FOUR)==BST_CHECKED)
					EndDialog(hDlg,dwLastNumPlayers=4);
				else
					EndDialog(hDlg,0);
			break;

			case IDCANCEL:
					EndDialog(hDlg,0);
			break;

		}
		break;

	}
	return (FALSE);
}

BOOL CALLBACK DlgProcNumLocalPlayers(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_INITDIALOG:
		switch(dwLastNetNumPlayers)
		{
		case 1:
			CheckRadioButton(hDlg,IDC_ONELOCAL,IDC_THREELOCAL,IDC_ONELOCAL);
			break;
		case 2:
			CheckRadioButton(hDlg,IDC_ONELOCAL,IDC_THREELOCAL,IDC_TWOLOCAL);
			break;
		case 3:
			CheckRadioButton(hDlg,IDC_ONELOCAL,IDC_THREELOCAL,IDC_THREELOCAL);
			break;
		default:
			CheckRadioButton(hDlg,IDC_ONELOCAL,IDC_THREELOCAL,IDC_ONELOCAL);
			break;
		}
		SetFocus(hDlg);
		return(FALSE);
		break;

	case WM_COMMAND:
		switch(wParam)
		{
			case IDOK:
				if(IsDlgButtonChecked(hDlg,IDC_ONELOCAL)==BST_CHECKED)
					EndDialog(hDlg,dwLastNetNumPlayers=1);
				else
				if(IsDlgButtonChecked(hDlg,IDC_TWOLOCAL)==BST_CHECKED)
					EndDialog(hDlg,dwLastNetNumPlayers=2);
				else
				if(IsDlgButtonChecked(hDlg,IDC_THREELOCAL)==BST_CHECKED)
					EndDialog(hDlg,dwLastNetNumPlayers=3);
				else
					EndDialog(hDlg,0);
			break;

			case IDCANCEL:
					EndDialog(hDlg,0);
			break;

		}
		break;

	}
	return (FALSE);
}

BOOL FAR PASCAL DedicatedEnumConnectionsCallback(
	 LPCGUID lpguidSP, LPVOID lpConnection, DWORD dwConnectionSize,
	 LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
	if(IsEqualGUID(*lpguidSP,*(GUID*)lpContext))
	{
		if(dwConnectionSize)
		{
			*(DWORD*)Buff=dwConnectionSize;
			memcpy(&Buff[4], lpConnection, dwConnectionSize);
		}
	}
   return TRUE;
}

BOOL FAR PASCAL DirectPlayEnumConnectionsCallback(
	 LPCGUID lpguidSP, LPVOID lpConnection, DWORD dwConnectionSize,
	 LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
   HWND     hWnd = (HWND) lpContext;
   LRESULT	iIndex;
   BYTE*   lpConnectionBuffer;
#ifdef GOA
   if(!IsEqualGUID(*lpguidSP,DPSPGUID_TCPIP))
	   return TRUE;
#endif
   iIndex = SendDlgItemMessage(hWnd, IDC_SPCOMBO, LB_ADDSTRING, 0, 
	   (LPARAM) lpName->lpszShortNameA);
   if (iIndex==CB_ERR)
	   return TRUE;
   /*
   if(strstr(lpName->lpszShortNameA,"TCP/IP"))
   {
	   FILE* F=fopen("dedicated.tcp","wb");
	   fwrite(&dwConnectionSize,4,1,F);
	   fwrite(lpConnection,dwConnectionSize,1,F);
	   fclose(F);
   }

   if(strstr(lpName->lpszShortNameA,"IPX"))
   {
	   FILE* F=fopen("dedicated.ipx","wb");
	   fwrite(&dwConnectionSize,4,1,F);
	   fwrite(lpConnection,dwConnectionSize,1,F);
	   fclose(F);
   }
   */

   lpConnectionBuffer=new BYTE[dwConnectionSize];
   if(lpConnectionBuffer==NULL)
	   return TRUE;

   memcpy(lpConnectionBuffer, lpConnection, dwConnectionSize);

   SendDlgItemMessage(hWnd, IDC_SPCOMBO, LB_SETITEMDATA, (WPARAM) iIndex, 
	   (LPARAM) lpConnectionBuffer);
   return TRUE;
}

BOOL CALLBACK DlgProcProvider(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	void*	lpConnection;
	int iIndex,iCount;

	switch (msg)
	{
	case WM_INITDIALOG:
		CheckRadioButton(hDlg,IDC_RDEDICATED,IDC_RNORMAL,IDC_RDEDICATED+Settings.SERVER_TYPE);
		EnableWindow(GetDlgItem(hDlg,IDC_RDEDICATED),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_RMULTICAST),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_RNORMAL),FALSE);
		if(IsHost)
		{
			iIndex=SendDlgItemMessage(hDlg,IDC_SPCOMBO,LB_ADDSTRING,0,(LPARAM)LSZ_LOCALGAME);
			SendDlgItemMessage(hDlg,IDC_SPCOMBO,LB_SETITEMDATA,iIndex,0);
		}
		if(lpDP==NULL)
		{
#ifdef WSDP
			wsDP=new WSDPlay();
#else
#ifndef DX3
		CoCreateInstance( CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, 
						 IID_IDirectPlay4A, (LPVOID*)&lpDP);
#else
		CoCreateInstance( CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, 
						 IID_IDirectPlay3A, (LPVOID*)&lpDP);
#endif
#endif
		}

		if(lpDP==NULL)
		{
			WarningMsg(LSZ_DP_CANTCREATEDP,LSZ_DP_INIT);
			EndDialog(hDlg,-1);
			return FALSE;
		}
/*
		hr=lpDP->Initialize(0);
		if(hr!=DP_OK)
		{
			WarningMsg(LSZ_DP_CANTINIT,LSZ_DP_INIT);
			EndDialog(hDlg,-1);
			return FALSE;
		}
*/
		lpDP->EnumConnections(&BOMBER_GUID,DirectPlayEnumConnectionsCallback, hDlg, DPCONNECTION_DIRECTPLAY);
		SendDlgItemMessage(hDlg,IDC_SPCOMBO,LB_SETCURSEL,0,0);
#ifdef GOA
		SendMessage(hDlg,WM_COMMAND,(BN_CLICKED<<16)|IDOK,(LPARAM)GetDlgItem(hDlg,IDOK));
#else
		SetFocus(hDlg);
#endif
		return FALSE;
		break;

	case WM_COMMAND:
		if(IsHost)
			if(HIWORD(wParam)==LBN_SELCHANGE)
			{
				EnableWindow(GetDlgItem(hDlg,IDC_RDEDICATED),
								0!=SendDlgItemMessage(hDlg,IDC_SPCOMBO,LB_GETCURSEL,0,0));
				EnableWindow(GetDlgItem(hDlg,IDC_RMULTICAST),
								0!=SendDlgItemMessage(hDlg,IDC_SPCOMBO,LB_GETCURSEL,0,0));
				EnableWindow(GetDlgItem(hDlg,IDC_RNORMAL),
								0!=SendDlgItemMessage(hDlg,IDC_SPCOMBO,LB_GETCURSEL,0,0));
			}

		if(HIWORD(wParam)==LBN_DBLCLK)
			PostMessage(hDlg,WM_COMMAND,(BN_CLICKED<<16)|IDOK,(LPARAM)GetDlgItem(hDlg,IDOK));

		switch(wParam)
		{
			case IDOK:
				if(IsHost)
				{
					if(IsDlgButtonChecked(hDlg,IDC_RDEDICATED))
						Settings.SERVER_TYPE=ST_DEDICATED;
					if(IsDlgButtonChecked(hDlg,IDC_RMULTICAST))
						Settings.SERVER_TYPE=ST_MULTICAST;
					if(IsDlgButtonChecked(hDlg,IDC_RNORMAL))
						Settings.SERVER_TYPE=ST_NORMAL;
				}

				iIndex=SendDlgItemMessage(hDlg,IDC_SPCOMBO,LB_GETCURSEL,0,0);
				if(iIndex==LB_ERR)
				{
					EndDialog(hDlg,-1);
					break;
				}

				lpConnection=(void*)SendDlgItemMessage(hDlg,IDC_SPCOMBO,LB_GETITEMDATA,iIndex,0);
				if(lpConnection!=0)
				{
					if(DP_OK==lpDP->InitializeConnection(lpConnection, 0))
					{
						iCount=SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_GETCOUNT,0, 0);
						if(iCount!=LB_ERR)
							while(iCount>=0)
							{
								void* lpC;
								lpC=(void*)SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_GETITEMDATA,iCount, 0);
								if((lpC!=NULL)&&(lpC!=(void*)LB_ERR))
									SAFE_DELETE_ARRAY(lpC)
								SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_DELETESTRING,iCount--, 0);
							}
						EndDialog(hDlg,1);
					}
					else
					{
						iCount=SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_GETCOUNT,0, 0);
						if(iCount!=LB_ERR)
							while(iCount>=0)
							{
								void* lpC;
								lpC=(void*)SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_GETITEMDATA,iCount, 0);
								if((lpC!=NULL)&&(lpC!=(void*)LB_ERR))
									SAFE_DELETE_ARRAY(lpC)
								SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_DELETESTRING,iCount--, 0);
							}
						WarningMsg(LSZ_DP_CANTINITCONN,LSZ_DP_INIT);
						EndDialog(hDlg,-1);
					}
				}
				else
				{
					SAFE_RELEASE(lpDP);
					iCount=SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_GETCOUNT,0, 0);
					if(iCount!=LB_ERR)
						while(iCount>=0)
						{
							void* lpC;
							lpC=(void*)SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_GETITEMDATA,iCount, 0);
							if((lpC!=NULL)&&(lpC!=(void*)LB_ERR))
								SAFE_DELETE_ARRAY(lpC)
							SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_DELETESTRING,iCount--, 0);
						}
					EndDialog(hDlg,0);
				}
			break;

			case IDCANCEL:
				SAFE_RELEASE(lpDP);
				iCount=SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_GETCOUNT,0, 0);
				if(iCount!=LB_ERR)
					while(iCount>=0)
					{
						void* lpC;
						lpC=(void*)SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_GETITEMDATA,iCount, 0);
						if((lpC!=NULL)&&(lpC!=(void*)LB_ERR))
							SAFE_DELETE_ARRAY(lpC)
						SendDlgItemMessage(hDlg,IDC_SPCOMBO, LB_DELETESTRING,iCount--, 0);
					}
				EndDialog(hDlg,-1);
			break;

		}
		break;

	}
	return (FALSE);
}

BOOL FAR PASCAL EnumSessionsCallback(
	LPCDPSESSIONDESC2 lpSessionDesc, LPDWORD lpdwTimeOut,
	DWORD dwFlags, LPVOID lpContext)
{
	HWND   hWnd = (HWND)lpContext;
	LPGUID lpGuid;
	LONG   iIndex;
	DWORD	dwVer;

	SetCursor(LoadCursor(0,IDC_APPSTARTING));
	if(hWnd==0) return FALSE;

	if (dwFlags & DPESC_TIMEDOUT)
		return FALSE;

	//13 pour " (.........)\0"
	char* szEnumSName=new char[13+strlen(lpSessionDesc->lpszSessionNameA)];

	if(lpSessionDesc->dwUser3&BS_DEDICATED) //Dédié
		sprintf(szEnumSName,LSZ_DP_sDED,lpSessionDesc->lpszSessionNameA);
	else
	if(lpSessionDesc->dwUser3&BS_MULTICAST) //Multicast
		sprintf(szEnumSName,LSZ_DP_sMULTICAST,lpSessionDesc->lpszSessionNameA);
	else
		strcpy(szEnumSName,lpSessionDesc->lpszSessionNameA);

	dwVer=lpSessionDesc->dwUser2;
	if(lpSessionDesc->dwMaxPlayers!=0)
	{
		if(lpSessionDesc->dwUser4!=1)
		{
			if(lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4!=1)
			{
				sprintf(Buff,LSZ_SESSLIST0,
					szEnumSName,lpSessionDesc->dwUser4,
					lpSessionDesc->dwMaxPlayers,lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4,
					dwVer>>24,(dwVer>>16)&255,(dwVer>>8)&255,dwVer&255);
			}
			else
			{
				sprintf(Buff,LSZ_SESSLIST1,
					szEnumSName,lpSessionDesc->dwUser4,
					lpSessionDesc->dwMaxPlayers,lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4,
					dwVer>>24,(dwVer>>16)&255,(dwVer>>8)&255,dwVer&255);
			}
		}
		else
		{
			if(lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4!=1)
			{
				sprintf(Buff,LSZ_SESSLIST2,
					szEnumSName,lpSessionDesc->dwUser4,
					lpSessionDesc->dwMaxPlayers,lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4,
					dwVer>>24,(dwVer>>16)&255,(dwVer>>8)&255,dwVer&255);
			}
			else
			{
				sprintf(Buff,LSZ_SESSLIST3,
					szEnumSName,lpSessionDesc->dwUser4,
					lpSessionDesc->dwMaxPlayers,lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4,
					dwVer>>24,(dwVer>>16)&255,(dwVer>>8)&255,dwVer&255);
			}
		}
	}
	else
	{
		if(lpSessionDesc->dwCurrentPlayers!=1)
		{
			if(lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4!=1)
			{
				sprintf(Buff,LSZ_SESSLIST0nm,
					szEnumSName,lpSessionDesc->dwUser4,
					lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4,
					dwVer>>24,(dwVer>>16)&255,(dwVer>>8)&255,dwVer&255);
			}
			else
			{
				sprintf(Buff,LSZ_SESSLIST1nm,
					szEnumSName,lpSessionDesc->dwUser4,
					lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4,
					dwVer>>24,(dwVer>>16)&255,(dwVer>>8)&255,dwVer&255);
			}
		}
		else
		{
			if(lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4!=1)
			{
				sprintf(Buff,LSZ_SESSLIST2nm,
					szEnumSName,lpSessionDesc->dwUser4,
					lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4,
					dwVer>>24,(dwVer>>16)&255,(dwVer>>8)&255,dwVer&255);
			}
			else
			{
				sprintf(Buff,LSZ_SESSLIST3nm,
					szEnumSName,lpSessionDesc->dwUser4,
					lpSessionDesc->dwCurrentPlayers-lpSessionDesc->dwUser4,
					dwVer>>24,(dwVer>>16)&255,(dwVer>>8)&255,dwVer&255);
			}
		}
	}

	SAFE_DELETE_ARRAY(szEnumSName);
	iIndex = SendDlgItemMessage(hWnd, IDC_SESSIONLIST, LB_ADDSTRING, 
		(WPARAM) 0, (LPARAM) Buff);

	if (iIndex == LB_ERR)
		return TRUE;

	lpGuid = new GUID;

	*lpGuid = lpSessionDesc->guidInstance;
	SendDlgItemMessage(hWnd, IDC_SESSIONLIST, LB_SETITEMDATA, 
		(WPARAM) iIndex, (LPARAM) lpGuid);

	return TRUE;
}

BOOL CALLBACK DlgProcSessions(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int iIndex;
	DPSESSIONDESC2 sessionDesc;
	int hr,iCount,iSel;
	LPGUID lpguidSessionInstance;
	GUID SelGUID;

	switch (msg)
	{
	case WM_TIMER:
		PostMessage(hDlg,WM_COMMAND,(BN_CLICKED<<16)|IDB_REFRESH,0);
		break;

	case WM_INITDIALOG:
		if(Settings.ENUMEVERY&&(Settings.TEVERY>0))
			SetTimer(hDlg,1,Settings.TEVERY,0);

		SetCursor(LoadCursor(0,IDC_APPSTARTING));
		PostMessage(hDlg,WM_COMMAND,(BN_CLICKED<<16)|IDB_REFRESH,0);
		EnableWindow(GetDlgItem(hDlg,IDOK),FALSE);
		SetFocus(hDlg);
		return(FALSE);
		break;

	case WM_COMMAND:
		if(HIWORD(wParam)==LBN_DBLCLK)
			PostMessage(hDlg,WM_COMMAND,(BN_CLICKED<<16)|IDOK,(LPARAM)GetDlgItem(hDlg,IDOK));

		if((HIWORD(wParam)==BN_CLICKED)&&(LOWORD(wParam)==IDB_REFRESH))
		{
			if(Settings.ENUMEVERY&&(Settings.TEVERY>0))
				KillTimer(hDlg,1);
			SetCursor(LoadCursor(0,IDC_APPSTARTING));
			SetWindowText(hDlg,LSZ_SELSESSUPDATE);
			iCount=SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_GETCOUNT,0, 0);
			iSel=SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_GETCURSEL,0, 0);
			if(iSel!=LB_ERR)
				if(lpguidSessionInstance=(LPGUID)SendDlgItemMessage(hDlg,IDC_SESSIONLIST,LB_GETITEMDATA,iSel,0))
					SelGUID=*lpguidSessionInstance;
				else
					SelGUID=GUID_NULL;

			if(iCount!=LB_ERR)
				while(iCount>=0)
				{
					LPGUID lpG;
					lpG=(LPGUID)SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_GETITEMDATA,iCount, 0);
					if(lpG!=(void*)LB_ERR)
						SAFE_DELETE(lpG)
					SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_DELETESTRING,iCount--, 0);
				}

			memset(&sessionDesc,0,sizeof(DPSESSIONDESC2));
			sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
			sessionDesc.guidApplication = BOMBER_GUID;

			if(Settings.ASYNCENUM)
				hr=lpDP->EnumSessions(&sessionDesc, 0, EnumSessionsCallback,
					hDlg, DPENUMSESSIONS_ALL|DPENUMSESSIONS_ASYNC);
			else
				hr=lpDP->EnumSessions(&sessionDesc, 0, EnumSessionsCallback,
					hDlg, DPENUMSESSIONS_ALL);

			if((hr!=DP_OK)&&(hr!=DPERR_CONNECTING))
				WarningMsg(LSZ_DP_CANTENUMSESSIONS,LSZ_DP_INIT);

			iCount=SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_GETCOUNT,0, 0);
			for(iSel=0;iSel<iCount;iSel++)
				if(lpguidSessionInstance=(LPGUID)SendDlgItemMessage(hDlg,IDC_SESSIONLIST,LB_GETITEMDATA,iSel,0))
					if(IsEqualGUID(SelGUID,*lpguidSessionInstance))
						SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_SETCURSEL,iSel, 0);

			EnableWindow(GetDlgItem(hDlg,IDOK),(iCount>0));

			SetCursor(LoadCursor(0,IDC_ARROW));
			SetWindowText(hDlg,LSZ_SELSESS);
			if(Settings.ENUMEVERY&&(Settings.TEVERY>0))
				SetTimer(hDlg,1,Settings.TEVERY,0);
		}
		else
		switch(wParam)
		{
			case IDOK:
				KillTimer(hDlg,1);
				if(Settings.ASYNCENUM)
					lpDP->EnumSessions(&sessionDesc, 0, EnumSessionsCallback,
						0, DPENUMSESSIONS_ALL|DPENUMSESSIONS_STOPASYNC);

				iIndex=SendDlgItemMessage(hDlg,IDC_SESSIONLIST,LB_GETCURSEL,0,0);
				if(iIndex==LB_ERR)
					break;
				lpguidSessionInstance=(LPGUID)SendDlgItemMessage(hDlg,IDC_SESSIONLIST,LB_GETITEMDATA,iIndex,0);
				if (int(lpguidSessionInstance)==LB_ERR)
					break;

				bSpectator=(IsDlgButtonChecked(hDlg,IDC_SPECTATOR)==BST_CHECKED);
				memset(&sessionDesc,0,sizeof(DPSESSIONDESC2));
				sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
				sessionDesc.guidInstance = *lpguidSessionInstance;

				iCount=SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_GETCOUNT,0, 0);
				if(iCount!=LB_ERR)
					while(iCount>=0)
					{
						LPGUID lpG=0;
						lpG=(LPGUID)SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_GETITEMDATA,iCount, 0);
						if(lpG!=(void*)LB_ERR)
							SAFE_DELETE(lpG)
						SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_DELETESTRING,iCount--, 0);
					}

				hr = lpDP->Open(&sessionDesc, DPOPEN_JOIN);
				if (hr!=DP_OK)
				{EndDialog(hDlg,0);return(FALSE);}
				EndDialog(hDlg,1);

			break;

			case IDCANCEL:
				KillTimer(hDlg,1);
				if(Settings.ASYNCENUM)
					lpDP->EnumSessions(&sessionDesc, 0, EnumSessionsCallback,
						0, DPENUMSESSIONS_ALL|DPENUMSESSIONS_STOPASYNC);

				iCount=SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_GETCOUNT,0, 0);
				if(iCount!=LB_ERR)
					while(iCount>=0)
					{
						LPGUID lpG=0;
						lpG=(LPGUID)SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_GETITEMDATA,iCount, 0);
						if(lpG!=(void*)LB_ERR)
							SAFE_DELETE(lpG)
						SendDlgItemMessage(hDlg,IDC_SESSIONLIST, LB_DELETESTRING,iCount--, 0);
					}

				EndDialog(hDlg,-1);
			break;
		}
		break;
	}
	return (FALSE);
}

BOOL CALLBACK DlgProcSkins(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int iCount,iIndex;

	switch (msg)
	{
	case WM_INITDIALOG:
		iIndex=0;
		iCount=SendDlgItemMessage(hDlg,IDC_SKINLIST, LB_GETCOUNT,0, 0);
		if(iCount!=LB_ERR)
			while(iCount>=0)
				SendDlgItemMessage(hDlg,IDC_SKINLIST, LB_DELETESTRING,iCount--, 0);
		SendDlgItemMessage(hDlg,IDC_SKINLIST, LB_ADDSTRING,0, (int)LSZ_DEFAULT);
		for(iCount=0;iCount<NbSkins;iCount++)
		{
				SendDlgItemMessage(hDlg,IDC_SKINLIST, LB_ADDSTRING,0, (int)SkinDir[iCount]);
				if(strcmp(SkinDir[iCount],Settings.szSelSkin)==0)
					iIndex=iCount+1;
		}
		SendDlgItemMessage(hDlg,IDC_SKINLIST, LB_SETCURSEL,iIndex,0);
		SetFocus(GetDlgItem(hDlg,IDOK));
		return(FALSE);
		break;

	case WM_COMMAND:
		if(HIWORD(wParam)==LBN_DBLCLK)
			PostMessage(hDlg,WM_COMMAND,(BN_CLICKED<<16)|IDOK,(LPARAM)GetDlgItem(hDlg,IDOK));

		if(wParam==IDOK)
		{
			char szSel[BMAX_PATH];
			iIndex=SendDlgItemMessage(hDlg,IDC_SKINLIST, LB_GETCURSEL,0, 0);
			if(iIndex>0)
			{
				SendDlgItemMessage(hDlg,IDC_SKINLIST, LB_GETTEXT,iIndex,(int)szSel);
				strcpy(Settings.szSelSkin,szSel);
			}
			else
				memset(Settings.szSelSkin,0,BMAX_PATH);
			InitFileSystem();
			EndDialog(hDlg,0);
		}
		else
			if(wParam==IDCANCEL)
				EndDialog(hDlg,0);
		break;

	}
	return (FALSE);
}

void DrawMapPreview(HDC hDC,const char* szMapFile)
{
	const int XBLOC=4;
	const int YBLOC=4;
	char dn[BMAX_PATH];
	HBRUSH hb;
	RECT r;

	SetDefaultStartPos(&Settings.GO);
	if(szMapFile[0]!='\0')
		sprintf(dn,"Maps\\%s\\BMap.txt",szMapFile);
	else
		strcpy(dn,"\0");

	_chdir(szWorkingDirectory);
	FILE*F=0;
	if(dn[0]!='\0')
		F=fopen(dn,"rt");
	if(F!=0)
	{
		char c=0;
		BOOL br=0;
		for(j=0;j<15;j++)
			for(i=0;i<20;i++)
			{
				if(c==EOF)
				{
					if(!br)
					{
						br=TRUE;
						SetDefaultBlocs();
					}
				}
				else
				{
					do
					{
						c=fgetc(F);
						if(c==';')
							fgets(Buff,BMAX_PATH,F);
					}
					while((c==10)||(c==13)||(c==';'));
					Blocs[i][j].Type=((c=='x')||(c=='X'))?MUR:SOL;
					if((c>='1')&&(c<='0'+MAX_PLAYERS))
					{
						Settings.GO.StartPos[2*(c-'1')]=i;
						Settings.GO.StartPos[2*(c-'1')+1]=j;
					}
				}
			}

		fclose(F);
	}
	else
		SetDefaultBlocs();

	for(j=0;j<15;j++)
		for(i=0;i<20;i++)
		{
			r.top=YBLOC*j;
			r.left=XBLOC*i;
			r.bottom=r.top+YBLOC+1;
			r.right=r.left+XBLOC+1;
			if(Blocs[i][j].Type==MUR)
				FillRect(hDC,&r,(HBRUSH)GetStockObject(BLACK_BRUSH));
			else
				FillRect(hDC,&r,(HBRUSH)GetStockObject(GRAY_BRUSH));
		}

	for(i=0;i<MAX_PLAYERS;i++)
	{
		r.left=XBLOC*Settings.GO.StartPos[2*i];
		r.top=YBLOC*Settings.GO.StartPos[2*i+1];
		r.bottom=r.top+YBLOC+1;
		r.right=r.left+XBLOC+1;
		FillRect(hDC,&r,hb=CreateSolidBrush(BOMBER_COLOR[i]));
		DeleteObject(hb);
	}
	return;
}

BOOL CALLBACK DlgProcMaps(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int iCount,iIndex;
	HDC hDC;

	switch (msg)
	{
	case WM_INITDIALOG:
		iIndex=0;
		iCount=SendDlgItemMessage(hDlg,IDC_MAPLIST, LB_GETCOUNT,0, 0);
		if(iCount!=LB_ERR)
			while(iCount>=0)
				SendDlgItemMessage(hDlg,IDC_MAPLIST, LB_DELETESTRING,iCount--, 0);
		SendDlgItemMessage(hDlg,IDC_MAPLIST, LB_ADDSTRING,0, (int)LSZ_DEFAULT);
		for(iCount=0;iCount<NbMaps;iCount++)
		{
				SendDlgItemMessage(hDlg,IDC_MAPLIST, LB_ADDSTRING,0, (int)MapDir[iCount]);
				if(strcmp(MapDir[iCount],Settings.szSelMap)==0)
					iIndex=iCount+1;
		}
		SendDlgItemMessage(hDlg,IDC_MAPLIST, LB_SETCURSEL,iIndex,0);
		SetFocus(GetDlgItem(hDlg,IDC_MAPLIST));
		return(FALSE);
		break;

	case WM_PAINT:
		char szSel[BMAX_PATH];
		iIndex=SendDlgItemMessage(hDlg,IDC_MAPLIST, LB_GETCURSEL,0, 0);
		SendDlgItemMessage(hDlg,IDC_MAPLIST, LB_GETTEXT,iIndex,(int)szSel);
		if(iIndex==0)
			strcpy(szSel,"\0");
		DrawMapPreview(hDC=GetDC(GetDlgItem(hDlg,IDC_MAPPREVIEW)),szSel);
		ReleaseDC(GetDlgItem(hDlg,IDC_MAPPREVIEW),hDC);
		break;

	case WM_COMMAND:
		if(HIWORD(wParam)==LBN_DBLCLK)
			PostMessage(hDlg,WM_COMMAND,(BN_CLICKED<<16)|IDOK,(LPARAM)GetDlgItem(hDlg,IDOK));

		if(HIWORD(wParam)==LBN_SELCHANGE)
		{
			PostMessage(hDlg,WM_PAINT,0,0);
		}
		if(wParam==IDOK)
		{
			char szSel[BMAX_PATH];
			iIndex=SendDlgItemMessage(hDlg,IDC_MAPLIST, LB_GETCURSEL,0, 0);
			SendDlgItemMessage(hDlg,IDC_MAPLIST, LB_GETTEXT,iIndex,(int)szSel);
			if(iIndex>0)
				strcpy(Settings.szSelMap,szSel);
			else
				Settings.szSelMap[0]='\0';
			EndDialog(hDlg,0);
		}
		else
			if(wParam==IDCANCEL)
				EndDialog(hDlg,0);
		break;

	}
	return (FALSE);
}

BOOL CALLBACK DlgProcGameOptions(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char szText[BMAX_PATH];

	switch (msg)
	{
	case WM_INITDIALOG:
		while(CB_ERR!=SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_DELETESTRING,0,0));

		SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_ADDSTRING,0,(LPARAM)LSZ_NOVICE);
		SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_ADDSTRING,0,(LPARAM)LSZ_MEDIUM);
		SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_ADDSTRING,0,(LPARAM)LSZ_ADVANCED);
		SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_ADDSTRING,0,(LPARAM)LSZ_EXPERT);

		SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_SETITEMDATA,0,0);
		SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_SETITEMDATA,1,1);
		SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_SETITEMDATA,2,2);
		SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_SETITEMDATA,3,3);

		SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_SETCURSEL,TmpGO1.BOTLEVEL,0);

		SetWindowText(GetDlgItem(hDlg,IDC_BONUS0),_ltoa((int)TmpGO1.BonusRepart[0],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS3),_ltoa((int)TmpGO1.BonusRepart[3],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS4),_ltoa((int)TmpGO1.BonusRepart[4],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS5),_ltoa((int)TmpGO1.BonusRepart[5],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS6),_ltoa((int)TmpGO1.BonusRepart[6],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS7),_ltoa((int)TmpGO1.BonusRepart[7],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS8),_ltoa((int)TmpGO1.BonusRepart[8],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS9),_ltoa((int)TmpGO1.BonusRepart[9],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS10),_ltoa((int)TmpGO1.BonusRepart[10],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS11),_ltoa((int)TmpGO1.BonusRepart[11],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS12),_ltoa((int)TmpGO1.BonusRepart[12],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS14),_ltoa((int)TmpGO1.BonusRepart[14],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS15),_ltoa((int)TmpGO1.BonusRepart[15],szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BONUS16),_ltoa((int)TmpGO1.BonusRepart[16],szText,10));

		SetWindowText(GetDlgItem(hDlg,IDC_SIMULBOMB),_ltoa(TmpGO1.SIMULBOMB,szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_NBBOMB),_ltoa(TmpGO1.NBBOMB,szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_NBMINE),_ltoa(TmpGO1.NBMINE,szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_NBBLAST),_ltoa(TmpGO1.NBBLAST,szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_BLOC_COND),_ltoa(TmpGO1.BLOC_COND,szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_MORTPNT),_ltoa(TmpGO1.MORTPNT,szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_AKPNT),_ltoa(TmpGO1.AKPNT,szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_REV_TOUT),_ltoa(TmpGO1.REV_TOUT/1000,szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_MCH_TOUT),_ltoa(TmpGO1.MCH_TOUT/1000,szText,10));
		SetWindowText(GetDlgItem(hDlg,IDC_SDDEATH_TOUT),_ltoa(TmpGO1.SDDEATH_TOUT/1000,szText,10));

		CheckDlgButton(hDlg,IDC_RETNUCL,TmpGO1.RETNUCL? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_DISPRETNUCL,TmpGO1.DISPRETNUCL? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_CANREV,TmpGO1.CANREV? BST_CHECKED:BST_UNCHECKED);
		SetFocus(GetDlgItem(hDlg,IDOK));
		return(FALSE);
		break;

	case WM_COMMAND:
		if(wParam==IDOK)
		{
			TmpGO.RETNUCL=(IsDlgButtonChecked(hDlg,IDC_RETNUCL)==BST_CHECKED);
			TmpGO.DISPRETNUCL=(IsDlgButtonChecked(hDlg,IDC_DISPRETNUCL)==BST_CHECKED);
			TmpGO.CANREV=(IsDlgButtonChecked(hDlg,IDC_CANREV)==BST_CHECKED);

			GetWindowText(GetDlgItem(hDlg,IDC_BONUS0),szText,BMAX_PATH);
			TmpGO.BonusRepart[0]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS3),szText,BMAX_PATH);
			TmpGO.BonusRepart[3]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS4),szText,BMAX_PATH);
			TmpGO.BonusRepart[4]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS5),szText,BMAX_PATH);
			TmpGO.BonusRepart[5]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS6),szText,BMAX_PATH);
			TmpGO.BonusRepart[6]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS7),szText,BMAX_PATH);
			TmpGO.BonusRepart[7]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS8),szText,BMAX_PATH);
			TmpGO.BonusRepart[8]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS9),szText,BMAX_PATH);
			TmpGO.BonusRepart[9]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS10),szText,BMAX_PATH);
			TmpGO.BonusRepart[10]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS11),szText,BMAX_PATH);
			TmpGO.BonusRepart[11]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS12),szText,BMAX_PATH);
			TmpGO.BonusRepart[12]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS14),szText,BMAX_PATH);
			TmpGO.BonusRepart[14]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS15),szText,BMAX_PATH);
			TmpGO.BonusRepart[15]=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BONUS16),szText,BMAX_PATH);
			TmpGO.BonusRepart[16]=atob(szText);

			TmpGO.BOTLEVEL=SendDlgItemMessage(hDlg,IDC_BOTLEVEL,CB_GETCURSEL,0,0);

			GetWindowText(GetDlgItem(hDlg,IDC_SDDEATH_TOUT),szText,BMAX_PATH);
			TmpGO.SDDEATH_TOUT=1000*atodw(szText);
			if(TmpGO.SDDEATH_TOUT/1000>1000)
				TmpGO.SDDEATH_TOUT=1000*1000;
			GetWindowText(GetDlgItem(hDlg,IDC_REV_TOUT),szText,BMAX_PATH);
			TmpGO.REV_TOUT=1000*atodw(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_MCH_TOUT),szText,BMAX_PATH);
			TmpGO.MCH_TOUT=1000*atodw(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_MORTPNT),szText,BMAX_PATH);
			TmpGO.MORTPNT=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_AKPNT),szText,BMAX_PATH);
			TmpGO.AKPNT=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_NBMINE),szText,BMAX_PATH);
			TmpGO.NBMINE=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_NBBOMB),szText,BMAX_PATH);
			TmpGO.NBBOMB=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_NBBLAST),szText,BMAX_PATH);
			TmpGO.NBBLAST=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_SIMULBOMB),szText,BMAX_PATH);
			TmpGO.SIMULBOMB=atob(szText);
			GetWindowText(GetDlgItem(hDlg,IDC_BLOC_COND),szText,BMAX_PATH);
			TmpGO.BLOC_COND=atob(szText);
			if(TmpGO.MORTPNT>32)
				TmpGO.MORTPNT=32;
			if(TmpGO.AKPNT>32)
				TmpGO.AKPNT=32;
			if(TmpGO.NBMINE>8)
				TmpGO.NBMINE=8;
			if(TmpGO.NBBLAST>20)
				TmpGO.NBBLAST=20;
			if(TmpGO.SIMULBOMB>50)
				TmpGO.SIMULBOMB=50;
			if(TmpGO.NBBOMB>TmpGO.SIMULBOMB)
				TmpGO.NBBOMB=TmpGO.SIMULBOMB;
			if(TmpGO.BLOC_COND>100)
				TmpGO.BLOC_COND=100;
			EndDialog(hDlg,TRUE);
		}
		else
			if(wParam==IDCANCEL)
				EndDialog(hDlg,0);
			else
				if(wParam==IDB_RESETGO)
				{
					SetDefaultGameOptions(&TmpGO1);
					SendMessage(hDlg,WM_INITDIALOG,0,0);
					break;
				}
		break;

	}
	return (FALSE);
}

BOOL CALLBACK DlgProcConfigs(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int iCount,iIndex;
	char szDel[BMAX_PATH];

	switch (msg)
	{
	case WM_INITDIALOG:
		_chdir(szWorkingDirectory);
		EnumConfigs(szConfigDir);
		NbConfigs=GetConfigCount();
		for(i=0;i<NbConfigs;i++)
		{
			GetConfigName(i,ConfigName[i]);
			ConfigName[i][strlen(ConfigName[i])-4]=0;
		}
		iIndex=0;
		iCount=SendDlgItemMessage(hDlg,IDC_CFGLIST, LB_GETCOUNT,0, 0);
		if(iCount!=LB_ERR)
			while(iCount>=0)
				SendDlgItemMessage(hDlg,IDC_CFGLIST, LB_DELETESTRING,iCount--, 0);
		SendDlgItemMessage(hDlg,IDC_CFGLIST, LB_ADDSTRING,0, (int)LSZ_DEFAULT);
		for(iCount=0;iCount<NbConfigs;iCount++)
		{
				SendDlgItemMessage(hDlg,IDC_CFGLIST, LB_ADDSTRING,0, (int)ConfigName[iCount]);
				if(strcmp(ConfigName[iCount],szCurConfig)==0)
					iIndex=iCount+1;
		}
		SendDlgItemMessage(hDlg,IDC_CFGLIST, LB_SETCURSEL,iIndex,0);
		SetFocus(GetDlgItem(hDlg,IDOK));
		return(FALSE);
		break;

	case WM_COMMAND:
		if(HIWORD(wParam)==LBN_DBLCLK)
			PostMessage(hDlg,WM_COMMAND,(BN_CLICKED<<16)|IDOK,(LPARAM)GetDlgItem(hDlg,IDOK));
		if(HIWORD(wParam)!=BN_CLICKED)
			break;

		switch(LOWORD(wParam))
		{
		case IDB_NEWCFG:
			DialogBox(0,(LPCSTR)IDD_CFGNAME,hDlg,(DLGPROC)DlgProcCfgName);
			SendMessage(hDlg,WM_INITDIALOG,0,0);
			break;

		case IDB_DELCFG:
			iIndex=SendDlgItemMessage(hDlg,IDC_CFGLIST, LB_GETCURSEL,0, 0);
			if(iIndex>0)
			{
				SendDlgItemMessage(hDlg,IDC_CFGLIST, LB_GETTEXT,iIndex,(int)szDel);
				SendDlgItemMessage(hDlg,IDC_CFGLIST, LB_DELETESTRING,iIndex, 0);
				strcat(szDel,".cfg");
				_chdir(szConfigDir);
				remove(szDel);
				memset(szCurConfig,0,BMAX_PATH);
				SendMessage(hDlg,WM_INITDIALOG,0,0);
			}
			SetFocus(GetDlgItem(hDlg,IDOK));
			break;

		case IDOK:
			char szSel[BMAX_PATH];
			iIndex=SendDlgItemMessage(hDlg,IDC_CFGLIST, LB_GETCURSEL,0, 0);
			if(iIndex>0)
			{
				SendDlgItemMessage(hDlg,IDC_CFGLIST, LB_GETTEXT,iIndex,(int)szSel);
				strcpy(szCurConfig,szSel);
				LoadConfigFile(szCurConfig,&Settings);
			}
			else
			{
				memset(szCurConfig,0,BMAX_PATH);
				SetDefaultOptions(&Settings);
			}
			EndDialog(hDlg,TRUE);
			break;

		case IDCANCEL:
			EndDialog(hDlg,0);
			break;
		}
		break;

	}
	return (FALSE);
}
/*
BOOL CALLBACK DlgProcName(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndCtrl;

	switch (msg)
	{
	case WM_INITDIALOG:
		hWndCtrl=GetDlgItem(hDlg,IDC_NICKNAME);
		SetWindowText(hWndCtrl,Settings.NickName);
		SetFocus(hWndCtrl);
		return(FALSE);
		break;

	case WM_COMMAND:
		switch(wParam)
		{
			case IDOK:
			hWndCtrl=GetDlgItem(hDlg,IDC_NICKNAME);
			if(GetWindowText(hWndCtrl,Settings.NickName,32)!=0)
				EndDialog(hDlg,0);
			else
				EndDialog(hDlg,TRUE);
			break;

		}
		break;

	}
	return (FALSE);
}
*/

BOOL CALLBACK DlgProcCfgName(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndCtrl;

	switch (msg)
	{
	case WM_INITDIALOG:
		hWndCtrl=GetDlgItem(hDlg,IDC_CFGNAME);
		SetWindowText(hWndCtrl,LSZ_NEWCONFIG);
		SetFocus(hWndCtrl);
		return(FALSE);
		break;

	case WM_COMMAND:
		switch(wParam)
		{
			case IDOK:
			hWndCtrl=GetDlgItem(hDlg,IDC_CFGNAME);
			if(GetWindowText(hWndCtrl,szCurConfig,BMAX_PATH)!=0)
				EndDialog(hDlg,CreateNewConfigFile(szCurConfig));
			else
				EndDialog(hDlg,0);
			break;

		}
		break;

	}
	return (FALSE);
}

BOOL CALLBACK DlgProcNameSession(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndCtrl;

	switch (msg)
	{
	case WM_INITDIALOG:
		hWndCtrl=GetDlgItem(hDlg,IDC_SESSIONNAME);
		SetWindowText(hWndCtrl,szSessionName);
		SetFocus(hWndCtrl);
		return(FALSE);
		break;

	case WM_COMMAND:
		switch(wParam)
		{
			case IDOK:
			hWndCtrl=GetDlgItem(hDlg,IDC_SESSIONNAME);
			if(GetWindowText(hWndCtrl,szSessionName,BMAX_NAME)!=0)
				EndDialog(hDlg,0);
			break;

		}
		break;

	}
	return (FALSE);
}

void AfficheTeams(HWND hDlg,BYTE t1,BYTE t2,BYTE t3,BYTE t4)
{
	if(t1>=NUM_TEAMS+1)
		t1=VOID_TEAM;
	if(t2>=NUM_TEAMS+1)
		t2=VOID_TEAM;
	if(t3>=NUM_TEAMS+1)
		t3=VOID_TEAM;
	if(t4>=NUM_TEAMS+1)
		t4=VOID_TEAM;
	SetWindowText(GetDlgItem(hDlg,IDC_TEAM1),TeamName[t1]);
	SetWindowText(GetDlgItem(hDlg,IDC_TEAM2),TeamName[t2]);
	SetWindowText(GetDlgItem(hDlg,IDC_TEAM3),TeamName[t3]);
	SetWindowText(GetDlgItem(hDlg,IDC_TEAM4),TeamName[t4]);
	return;
}


BOOL CALLBACK DlgProcNoms(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndCtrl;
	static BYTE sTeam1,sTeam2,sTeam3,sTeam4;

	switch (msg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg,IDC_SAVESCORES,Settings.SAVESCORES?BST_CHECKED:BST_UNCHECKED);
		hWndCtrl=GetDlgItem(hDlg,IDC_NOM1);
		SetWindowText(hWndCtrl,Bombers[0].Nom);
		hWndCtrl=GetDlgItem(hDlg,IDC_NOM2);
		SetWindowText(hWndCtrl,Bombers[1].Nom);
		hWndCtrl=GetDlgItem(hDlg,IDC_NOM3);
		SetWindowText(hWndCtrl,Bombers[2].Nom);
		hWndCtrl=GetDlgItem(hDlg,IDC_NOM4);
		SetWindowText(hWndCtrl,Bombers[3].Nom);

		EnableWindow(GetDlgItem(hDlg,IDC_NOM2),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_NOM3),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_NOM4),TRUE);

		EnableWindow(GetDlgItem(hDlg,IDC_IA2),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_IA3),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_IA4),TRUE);

		EnableWindow(GetDlgItem(hDlg,IDC_TEAM2),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_TEAM3),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_TEAM4),TRUE);
		if(NumPlayers<2)
		{
			EnableWindow(GetDlgItem(hDlg,IDC_NOM2),0);
			EnableWindow(GetDlgItem(hDlg,IDC_IA2),0);
			EnableWindow(GetDlgItem(hDlg,IDC_TEAM2),0);
		}
		if(NumPlayers<3)
		{
			EnableWindow(GetDlgItem(hDlg,IDC_NOM3),0);
			EnableWindow(GetDlgItem(hDlg,IDC_IA3),0);
			EnableWindow(GetDlgItem(hDlg,IDC_TEAM3),0);
		}
		if(NumPlayers<4)
		{
			EnableWindow(GetDlgItem(hDlg,IDC_NOM4),0);
			EnableWindow(GetDlgItem(hDlg,IDC_IA4),0);
			EnableWindow(GetDlgItem(hDlg,IDC_TEAM4),0);
		}

		CheckDlgButton(hDlg,IDC_IA1,Settings.IsIA[0]? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_IA2,Settings.IsIA[1]? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_IA3,Settings.IsIA[2]? BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hDlg,IDC_IA4,Settings.IsIA[3]? BST_CHECKED:BST_UNCHECKED);
		sTeam1=Settings.GO.TEAM1;
		sTeam2=Settings.GO.TEAM2;
		sTeam3=Settings.GO.TEAM3;
		sTeam4=Settings.GO.TEAM4;
		if(sTeam1>=NUM_TEAMS+1)
			sTeam1=VOID_TEAM;
		if(sTeam2>=NUM_TEAMS+1)
			sTeam2=VOID_TEAM;
		if(sTeam3>=NUM_TEAMS+1)
			sTeam3=VOID_TEAM;
		if(sTeam4>=NUM_TEAMS+1)
			sTeam4=VOID_TEAM;

		AfficheTeams(hDlg,sTeam1,sTeam2,sTeam3,sTeam4);
		PostMessage(GetDlgItem(hDlg,IDC_NOM1),EM_SETLIMITTEXT,BMAX_NAME,0);
		PostMessage(GetDlgItem(hDlg,IDC_NOM2),EM_SETLIMITTEXT,BMAX_NAME,0);
		PostMessage(GetDlgItem(hDlg,IDC_NOM3),EM_SETLIMITTEXT,BMAX_NAME,0);
		PostMessage(GetDlgItem(hDlg,IDC_NOM4),EM_SETLIMITTEXT,BMAX_NAME,0);
		SetFocus(hDlg);
		return(FALSE);
		break;


	case WM_COMMAND:
		if(HIWORD(wParam)==STN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_TEAM1:
				sTeam1++;
				sTeam1%=NUM_TEAMS+1;
				AfficheTeams(hDlg,sTeam1,sTeam2,sTeam3,sTeam4);
				break;

			case IDC_TEAM2:
				if(NumPlayers>=2)
				{
					sTeam2++;
					sTeam2%=NUM_TEAMS+1;
					AfficheTeams(hDlg,sTeam1,sTeam2,sTeam3,sTeam4);
				}
				break;

			case IDC_TEAM3:
				if(NumPlayers>=3)
				{
					sTeam3++;
					sTeam3%=NUM_TEAMS+1;
					AfficheTeams(hDlg,sTeam1,sTeam2,sTeam3,sTeam4);
				}
				break;

			case IDC_TEAM4:
				if(NumPlayers>=4)
				{
					sTeam4++;
					sTeam4%=NUM_TEAMS+1;
					AfficheTeams(hDlg,sTeam1,sTeam2,sTeam3,sTeam4);
				}
				break;

			case IDOK:
				if(IsDlgButtonChecked(hDlg,IDC_SAVESCORES)==BST_CHECKED)
					Settings.SAVESCORES=TRUE;
				else
					Settings.SAVESCORES=FALSE;

				Bombers[0].Team=Settings.GO.TEAM1=sTeam1;
				if(NumPlayers>=2)
					Bombers[1].Team=Settings.GO.TEAM2=sTeam2;
				else
					Bombers[1].Team=sTeam2=VOID_TEAM;

				if(NumPlayers>=3)
					Bombers[2].Team=Settings.GO.TEAM3=sTeam3;
				else
					Bombers[2].Team=sTeam3=VOID_TEAM;

				if(NumPlayers>=4)
					Bombers[3].Team=Settings.GO.TEAM4=sTeam4;
				else
					Bombers[3].Team=sTeam4=VOID_TEAM;

				if((sTeam1==VOID_TEAM)&&(sTeam2==VOID_TEAM)&&(sTeam3==VOID_TEAM)&&(sTeam4==VOID_TEAM))
					Settings.GO.TEAMPLAY=FALSE;
				else
					Settings.GO.TEAMPLAY=TRUE;

				if(GetWindowText(GetDlgItem(hDlg,IDC_NOM1),Bombers[0].Nom,BMAX_NAME)!=0)
					if((GetWindowText(GetDlgItem(hDlg,IDC_NOM2),Bombers[1].Nom,BMAX_NAME)!=0)||(NumPlayers<2))
						if((GetWindowText(GetDlgItem(hDlg,IDC_NOM3),Bombers[2].Nom,BMAX_NAME)!=0)||(NumPlayers<3))
							if((GetWindowText(GetDlgItem(hDlg,IDC_NOM4),Bombers[3].Nom,BMAX_NAME)!=0)||(NumPlayers<4))
							{
								strcpy(Settings.Nom[0],Bombers[0].Nom);
								strcpy(Settings.Nom[1],Bombers[1].Nom);
								strcpy(Settings.Nom[2],Bombers[2].Nom);
								strcpy(Settings.Nom[3],Bombers[3].Nom);
								Settings.IsIA[0]=(IsDlgButtonChecked(hDlg,IDC_IA1)==BST_CHECKED);
								Settings.IsIA[1]=(IsDlgButtonChecked(hDlg,IDC_IA2)==BST_CHECKED);
								Settings.IsIA[2]=(IsDlgButtonChecked(hDlg,IDC_IA3)==BST_CHECKED);
								Settings.IsIA[3]=(IsDlgButtonChecked(hDlg,IDC_IA4)==BST_CHECKED);
								EndDialog(hDlg,TRUE);
							}
				break;

			case IDCANCEL:
				EndDialog(hDlg,0);
				break;
			}
		}
		break;
	}
	return (FALSE);
}

void ProcheMine(int Num)
{
int a,b;
	for(a=0;a<NumPlayers;a++)
		for(b=0;b<16;b++)
			if(Bombs[a][b].Etat==B_PLAYING)
				if(Bombs[a][b].NSpr==BB_MINESPR+1)
					if (DirectDist((int)Bombers[Num].X/32,(int)Bombers[Num].Y/32,Bombs[a][b].X,Bombs[a][b].Y)<=Bombs[a][b].Sens)
					{
						SendGameMessage(Num,-1,MSG_EXPBOMB,a,Bombs[a][b].X,Bombs[a][b].Y,0);
						ExploseBomb(a,b);
					}
	return;
}

int DirectDist(int x1,int y1,int x2,int y2)
{
	int a,b;
	if ((x1!=x2)&&(y1!=y2)) return MAX_DIST;
	if (x1==x2)
	{
	if (y1>y2) {a=y2;y2=y1;y1=a;y1++;}
	a=x1;
		for(b=y1;b<y2;b++)
			if(EstBloque(a,b)) return MAX_DIST;
			return (int)fabs(y2-y1);
	}
	else
	{
	if (x1>x2) {a=x2;x2=x1;x1=a;x1++;}
	b=y1;
		for(a=x1;a<x2;a++)
			if(EstBloque(a,b)) return MAX_DIST;
			return (int)fabs(x2-x1);

	}
	return MAX_DIST;
}

void ShowBack(void)
{
	if((lpDDSPrimary==NULL)||(lpDDSBack==NULL))
		return;
	int hr=0;
	if(Settings.DBGMODE)
		hr=lpDDSPrimary->Blt(&Window,lpDDSBack,0,BLIT_WAIT,NULL);
	else
		if	(Settings.USEFLIP)
//			hr=hr;
			hr=lpDDSPrimary->Flip(0,FLIP_WAIT);
		else
			hr=lpDDSPrimary->BltFast(0,0,lpDDSBack,0,BLITFAST_WAIT);

	if(hr==DDERR_SURFACELOST)
	{
		bCheckRestore=TRUE;
		//SetPause();
		bUpdateAllScreen=GetUpdateCount();
	}
return;
}

void ShowText(HDC hdc,const char* tout,int xpos,int ypos,int SelFont,int color)
{
	SetBkMode(hdc,TRANSPARENT);
	SetBkColor(hdc, RGB(0,0,0));
	AttachFont(hdc,SelFont);

	SetTextAlign(hdc,TA_CENTER);
	SetTextColor(hdc, RGB(200,200,200));
	TextOut(hdc,xpos,ypos,tout,strlen(tout));
	TextOut(hdc,xpos-1,ypos,tout,strlen(tout));
	TextOut(hdc,xpos,ypos-1,tout,strlen(tout));
	SetTextColor(hdc, color);
	TextOut(hdc,xpos-1,ypos-1,tout,strlen(tout));

	return;
}

void ShowNow(const char* text)
{
	SetWindowText(hWndWin,text);
	OutFile(text);
	if(lpDDSBack==NULL)
		return;
	HDC hdc;
	updBuff.left=updBuff.top=0;
	updBuff.right=20;
	updBuff.bottom=15;
	UpdateBuffer(updBuff);
	updBuff.left=0;
	updBuff.top=0;
	updBuff.right=640;
	updBuff.bottom=480;
	UpdateScreen(updBuff);
	if(DD_OK==lpDDSBack->GetDC(&hdc))
	{
		ShowText(hdc,text,320,10,DEFAULT_FONT,RGB(0,0,240));
		lpDDSBack->ReleaseDC(hdc);
	}
	ShowBack();
	return;
}

void ResetMulti(void)
{
	int player;
	DPSESSIONDESC2	dpsdesc;
	DWORD*			sDesc;

	bReseted=TRUE;
//	if((!bConnected)||(NumPlayers<=1)) return;
	if(!bConnected) return;
	ShowNow(LSZ_WAITMPRESET);
	for(int ct=0;ct<MAX_PLAYERS;ct++)
		bResetReady[ct]=TRUE;

	if (IsHost) 
	{
		if(lpDP!=0)
		{
			lpDP->GetSessionDesc(0,&dpsdesc.dwSize);
			sDesc=new DWORD[(dpsdesc.dwSize+3)/4];
			sDesc[0]=dpsdesc.dwSize;
			lpDP->GetSessionDesc(sDesc,&dpsdesc.dwSize);
			dpsdesc=*(DPSESSIONDESC2*)sDesc;
			SAFE_DELETE_ARRAY(sDesc)
			dpsdesc.dwUser1++;
			dpsdesc.dwUser3&=~BS_PLAYING;
			dpsdesc.lpszSessionNameA=szSessionName;
			dpsdesc.dwSize=sizeof(DPSESSIONDESC2);
			lpDP->SetSessionDesc(&dpsdesc,0);
		}

		SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_RESET,0,0,0,0);
		ShowNow(LSZ_WAITRR);
		for(player=0;player<NumPlayers;player++)
		{
			BOOL b=FALSE;
			While((BCtrl[player].Control==CTRL_NET)&&BCtrl[player].Connected&&!b)
			{
				if(ReceiveGameMessage(-1,player)==MSG_RESETREPLY)
					b=TRUE;
				FlushSysMessageQueue();
				Sleep(SLEEP_TIME);
			}
		}

		ShowNow(LSZ_RRRECEIVED);
		SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_GAMEOPTIONS,(int)&Settings.GO,0,0,0);
		if(Settings.TELSVR)
			if(strlen(Settings.szSelSkin)!=0)
			{
				for(player=0;player<NumPlayers;player++)
					if((BCtrl[player].Control==CTRL_NET)&&BCtrl[player].Connected)
					{
						sprintf(Buff,"Skins\\%s",Settings.szSelSkin);
						SendDir(player,Buff);
						FlushSysMessageQueue();
					}
			}

		ShowNow(LSZ_WAITMPRESET);
		SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_LOADSKIN,(int)Settings.szSelSkin,0,0,0);
		ShowNow(LSZ_SENDBLOCS);
		SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_ALLBLOCS,0,0,0,0);

		ShowNow(LSZ_SYNC1);
		for(player=0;player<NumPlayers;player++)
		{
			BOOL b=FALSE;
			While((BCtrl[player].Control==CTRL_NET)&&BCtrl[player].Connected&&!b)
			{
				if(ReceiveGameMessage(-1,player)==MSG_RESETREPLY)
					b=TRUE;
				FlushSysMessageQueue();
				Sleep(SLEEP_TIME);
			}
		}

		SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_SYNC,0,0,0,0);

		ShowNow(LSZ_SYNC2);
		for(player=0;player<NumPlayers;player++)
		{
			BOOL b=FALSE;
			While((BCtrl[player].Control==CTRL_NET)&&BCtrl[player].Connected&&!b)
			{
				if(ReceiveGameMessage(-1,player)==MSG_PLAY)
					b=TRUE;
				FlushSysMessageQueue();
				Sleep(SLEEP_TIME);
			}
		}
		SetWindowText(hWndWin,szTitle);

		if(lpDP!=0)
		{
			dpsdesc.dwUser3|=BS_PLAYING;
			lpDP->SetSessionDesc(&dpsdesc,0);
		}

	}
	else if(!bSpectator)
	{
		char M;
		SendGameMessage(FROM_ALLLOCALPLAYERS,-1,MSG_RESETREPLY,0,0,0,0);
		SetWindowText(hWndWin,LSZ_WAITBLOCS);

		do
		{
			M=ReceiveGameMessage(-1,-1);  //Attend les blocs
			if(M!=MSG_FILEBLOCK)
				Sleep(SLEEP_TIME);
			switch(M)
			{
			case MSG_FILEWAIT:
				ShowNow(LSZ_ONERECEIVE);
				break;
			case MSG_STARTFILE:
				ShowNow(LSZ_FILERECEIVE);
				break;
			case MSG_FILESENT:
			case MSG_ENDFILE:
				ShowNow(LSZ_WAITMPRESET);
				break;
			}
		}
		While((M!=MSG_BLOC)&&(M!=MSG_ALLBLOCS)&&(M!=MSG_HOSTQUIT));
		if(M==MSG_HOSTQUIT)
		{
			SetWindowText(hWndWin,szTitle);
			return;
		}

		SendGameMessage(FROM_ALLLOCALPLAYERS,-1,MSG_RESETREPLY,0,0,0,0);
		ShowNow(LSZ_SYNC);
		do
		{
			M=ReceiveGameMessage(-1,-1);  //Recoit les blocs,attend le reset
			FlushSysMessageQueue();
			Sleep(SLEEP_TIME);
		}
		While((M!=MSG_SYNC)&&(M!=MSG_HOSTQUIT));
		if(M==MSG_HOSTQUIT)
		{
			SetWindowText(hWndWin,szTitle);
			return;
		}

		SendGameMessage(FROM_ALLLOCALPLAYERS,-1,MSG_PLAY,0,0,0,0);
		SetWindowText(hWndWin,szTitle);
	}
	else
	{
		char M;
		do
		{
			M=ReceiveGameMessage(-1,-1);  //Attend les blocs
			Sleep(SLEEP_TIME);
		}
		While((M!=MSG_BLOC)&&(M!=MSG_ALLBLOCS)&&(M!=MSG_HOSTQUIT));
		if(M==MSG_HOSTQUIT)
		{
			SetWindowText(hWndWin,szTitle);
			return;
		}

		ShowNow(LSZ_SYNC);
		do
		{
			M=ReceiveGameMessage(-1,-1);  //Recoit les blocs,attend le reset
			Sleep(SLEEP_TIME);
		}
		While((M!=MSG_SYNC)&&(M!=MSG_HOSTQUIT));
		if(M==MSG_HOSTQUIT)
		{
			SetWindowText(hWndWin,szTitle);
			return;
		}

		SetWindowText(hWndWin,szTitle);
	}
	return;
}

void SendDir(int player,const char* szDir)
{
	char cFileName[BMAX_PATH];
	FILE* F;
	DWORD sz=0;
	BOOL rt;
	int pl;
	HANDLE hFind;
	WIN32_FIND_DATA wfd;
	char szSearchDir[BMAX_PATH];

	_chdir(szWorkingDirectory);
	SendGameMessage(FROM_FIRSTLOCALPLAYER,player,MSG_CREATEDIR,(int)szDir,0,0,0);
	strcpy(szSearchDir,szDir);
	if(szSearchDir[strlen(szSearchDir)-1]!='\\')
		strcat(szSearchDir,"\\");
	strcat(szSearchDir,"*.*");
	hFind=FindFirstFile(szSearchDir,&wfd);
	if(hFind==INVALID_HANDLE_VALUE)
		return;

	do
	{
	if((wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
	{
		strcpy(cFileName,szDir);
		if(cFileName[strlen(cFileName)-1]!='\\')
			strcat(cFileName,"\\");
		strcat(cFileName,wfd.cFileName);
		F=fopen(cFileName,"rb");
		if(F!=0)
		{
			sz=FileCRC(F);
			fclose(F);
		}
		SendGameMessage(FROM_FIRSTLOCALPLAYER,player,MSG_FILENAME,(int)sz,(int)cFileName,0,0);
		do
		{
		pl=ReceiveGameMessage(-1,player);
		FlushSysMessageQueue();
		}
		While((pl!=MSG_FILEOK)&&(pl!=MSG_FILEREQUEST)&&BCtrl[player].Connected);
		if(pl==MSG_FILEREQUEST)
			SendFile(player,cFileName);
	}
	rt=FindNextFile(hFind,&wfd);
	}while(rt);

	FindClose(hFind);
	return;
}

void SendFile(int player,const char* szFileName)
{
	FILE* F=0;
	BYTE* src=NULL;
	BYTE* dst=NULL;
	BYTE* ptr=NULL;
	BYTE* wrkmem=NULL;
	unsigned int srcsze;
	unsigned int dstsze;
	int pl;
	DWORD cntsze;
	char text[BMAX_PATH];

	for(pl=0;pl<NumPlayers;pl++)
		if((player!=pl)&&(BCtrl[pl].Control==CTRL_NET)&&BCtrl[pl].Connected)
			SendGameMessage(FROM_FIRSTLOCALPLAYER,pl,MSG_FILEWAIT,0,0,0,0);

	F=fopen(szFileName,"rb");
	if(F==0) 
	{
		SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_FILESENT,0,0,0,0);
		return;
	}
	fseek(F,0,SEEK_END);
	srcsze=ftell(F);
	fseek(F,0,SEEK_SET);
#ifdef GCOMP_LZO
	src=new BYTE[srcsze];
	dstsze=srcsze + (srcsze / 64) + 16 + 3;
	dst=new BYTE[dstsze];
	wrkmem=new BYTE[LZO1X_999_MEM_COMPRESS];
#elif defined(GCOMP_UCL)
	src=new BYTE[srcsze];
	dstsze=srcsze + (srcsze / 8) + 256 + 3;
	dst=new BYTE[dstsze];
	wrkmem=0;
#elif defined(GCOMP_BZ)
	src=new BYTE[srcsze];
	dstsze=srcsze + (srcsze / 8) + 256 + 3;
	dst=new BYTE[dstsze];
	wrkmem=0;
#else
#error Pas de compresseur !!
#endif
	fread(src,srcsze,1,F);
	fclose(F);

	sprintf(text,LSZ_COMPRESSs,szFileName);
	ShowNow(text);

#ifdef GCOMP_LZO
	lzo1x_999_compress(src,srcsze,dst,&dstsze,wrkmem);
#elif defined(GCOMP_UCL)
	ucl_nrv2d_99_compress(src,srcsze,dst,&dstsze,0,10,NULL,NULL);
#elif defined(GCOMP_BZ)
	bzBuffToBuffCompress((char*)dst,&dstsze,(char*)src,srcsze,9,0,250);
#else
#error Pas de compresseur !!
#endif

	sprintf(text,LSZ_SENDsTOs,szFileName,Bombers[player].Nom);
	ShowNow(text);

	SendGameMessage(FROM_FIRSTLOCALPLAYER,player,MSG_STARTFILE,Settings.FILE_BLOCK_SIZE,(int)szFileName,0,0);
	ptr=dst;
	for(cntsze=0;cntsze<dstsze/Settings.FILE_BLOCK_SIZE;cntsze++)
	{
		Sleep(SLEEP_TIME);
		SendGameMessage(FROM_FIRSTLOCALPLAYER,player,MSG_FILEBLOCK,(int)ptr,0,0,0);
		FlushSysMessageQueue();
		if(!BCtrl[player].Connected)
			break;
		ptr+=Settings.FILE_BLOCK_SIZE;
	}

	if(BCtrl[player].Connected)
		SendGameMessage(FROM_FIRSTLOCALPLAYER,player,MSG_ENDFILE,(int)ptr,(int)(dstsze%Settings.FILE_BLOCK_SIZE),(int)dstsze,(int)srcsze);

	SAFE_DELETE_ARRAY(wrkmem)
	SAFE_DELETE_ARRAY(dst)
	SAFE_DELETE_ARRAY(src)

	SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_FILESENT,0,0,0,0);
	ShowNow(LSZ_WAITMPRESET);
	return;
}

void AddMessage(const char* Msg,DWORD Delay,DWORD Color)
{
	OutFile(Msg);
	int V;
	V=-1;
	for(int tc=0;tc<TOT_TXTMSG;tc++)
		if (!txtMsg[tc].Valide)
		{
			V=tc;
			break;
		}

	if (V==-1) return;
	strcpy(txtMsg[V].Text,Msg);
	txtMsg[V].Delay=Delay;
	txtMsg[V].Shown=0;
	txtMsg[V].Valide=TRUE;
	txtMsg[V].FirstTime=TickCount();
	txtMsg[V].Color=Color;

	return;
}

void AffMessage(void)
{
	if(lpDDSBack==NULL)
		return;
	HDC	hdc;
	static int LastNumMToAff=0;
	int MToAff[MAX_AFFMSG];
	int NumMToAff=0;
	int tc;
	for(tc=0;tc<TOT_TXTMSG;tc++)
		if(txtMsg[tc].Valide)
			if(txtMsg[tc].Shown&&(TickCount()-txtMsg[tc].FirstTime>txtMsg[tc].Delay))
			{
				bUpdateRow[0]=GetUpdateCount();
				txtMsg[tc].Valide=FALSE;
			}
			else
				if(NumMToAff<MAX_AFFMSG)
					MToAff[NumMToAff++]=tc;

	for(tc=0;tc<NumMToAff;tc++)
	{
		int idx=MToAff[tc];
		if(!txtMsg[idx].Shown)
		{
			txtMsg[idx].Shown=TRUE;
			txtMsg[idx].FirstTime=TickCount();
		}
		if(DD_OK==lpDDSBack->GetDC(&hdc))
		{
			ShowText(hdc,txtMsg[idx].Text,320,10+14*tc,DEFAULT_FONT,txtMsg[idx].Color);
			lpDDSBack->ReleaseDC(hdc);
		}
	}

	if((LastNumMToAff>0)&&(NumMToAff==0))
		bUpdateRow[0]=GetUpdateCount();

	if((LastNumMToAff>1)&&(NumMToAff<=1))
		bUpdateRow[1]=GetUpdateCount();

	LastNumMToAff=NumMToAff;
	/*
	static int Vaffm=-1;
	if(Vaffm==-1)
	{
		for(int tc=0;tc<TOT_TXTMSG;tc++)
			if (txtMsg[tc].Valide)
			{
				Vaffm=tc;
				break;
			}

		if (Vaffm==-1)
			return;
	}

	if(!txtMsg[Vaffm].Shown)
	{
		txtMsg[Vaffm].Shown=TRUE;
		txtMsg[Vaffm].FirstTime=TickCount();
		if(DD_OK==lpDDSBack->GetDC(&hdc))
		{
			ShowText(hdc,txtMsg[Vaffm].Text,320,10,DEFAULT_FONT,txtMsg[Vaffm].Color);
			lpDDSBack->ReleaseDC(hdc);
		}
	}
	else if(TickCount()-txtMsg[Vaffm].FirstTime>txtMsg[Vaffm].Delay)
	{
		bUpdateRow[0]=GetUpdateCount();
		txtMsg[Vaffm].Valide=0;
		Vaffm=-1;
	}
	else
		if(DD_OK==lpDDSBack->GetDC(&hdc))
		{
			ShowText(hdc,txtMsg[Vaffm].Text,320,10,DEFAULT_FONT,txtMsg[Vaffm].Color);
			lpDDSBack->ReleaseDC(hdc);
		}

	*/

	return;
}

void ResetGame(void)
{
	int n,o;
	olddelta=0;
	lGTC=0;
	bReset=0;
	bMustReset=0;
	bSudden=FALSE;
	bReturnPress=FALSE;
	GameStartTime=TickCount();
	bScoreRedraw=TRUE;

	PurgeKeyboard();

	for(n=0;n<MAX_PLAYERS;n++)
	   for(o=0;o<TOT_BLAST;o++)
		   Blasts[n][o].Etat=B_DISABLED;
	UpdateBlastCount();

	for(n=0;n<MAX_PLAYERS;n++)
	   for(o=0;o<TOT_BOMB;o++)
		   Bombs[n][o].Etat=B_DISABLED;
	UpdateBombCount();

	for(n=0;n<MAX_PLAYERS;n++)
	   ResetBomber(n);

	ResetIA();
	return;
}

void PurgeKeyboard(void)
{
//	if(Settings.USEDINPUT) return;
	int key;
	BYTE Keyb[256];
	for(key=0;key<256;key++)
		GetAsyncKeyState(key);
	GetKeyboardState(Keyb);
	return;
}

int ChooseBonus(void)
{
	int tct;
	int total;
	int random;
	total=0;

	for(tct=0;tct<NUM_BONUS+1;tct++)
		total+=Settings.GO.BonusRepart[tct];

	random=randInt(1,total);
	for(tct=0;tct<NUM_BONUS+1;tct++)
	{
		if (random<=Settings.GO.BonusRepart[tct]) 
			return tct;
		random-=Settings.GO.BonusRepart[tct];
	}

	return 0;
}

void InitDSound(void)
{
	int cnt=0;
	int Num;
	if(!bConnected)
		Num=NumPlayers;
	else
		Num=MAX_PLAYERS;

	if(!LoadFile("Bomber.wav")) {UnloadFile("Bomber.wav");ErrorMsg(LSZ_NOWAV,LSZ_ERROR);}
	InitSon("Bomber.wav",Settings.Volume-10000,Settings.HISOUND);
	UnloadFile("Bomber.wav");

	Temporize();
	if(Settings.GO.SDDEATH_TOUT!=0)
		if(!LoadFile("Beep.wav")) 
			{UnloadFile("Beep.wav");ErrorMsg(LSZ_NOWAV,LSZ_ERROR);}
	if(!LoadFile("Explosion.wav")) 
		{UnloadFile("Explosion.wav");ErrorMsg(LSZ_NOWAV,LSZ_ERROR);}
	if(!LoadFile("Bonus.wav")) 
		{UnloadFile("Bonus.wav");ErrorMsg(LSZ_NOWAV,LSZ_ERROR);}
	if(!LoadFile("Vache.wav")) 
		{UnloadFile("Vache.wav");ErrorMsg(LSZ_NOWAV,LSZ_ERROR);}
	if(!LoadFile("Mort.wav")) 
		{UnloadFile("Mort.wav");ErrorMsg(LSZ_NOWAV,LSZ_ERROR);}
	if(!LoadFile("Suicide.wav")) 
		{UnloadFile("Suicide.wav");ErrorMsg(LSZ_NOWAV,LSZ_ERROR);}
	if(!LoadFile("Teleport.wav")) 
		{UnloadFile("Teleport.wav");ErrorMsg(LSZ_NOWAV,LSZ_ERROR);}
	Temporize();

	if(Settings.DUPSBUFF)
	{
		CreateWaveBuffer(Bombers[0].SndExplose=cnt++,"Explosion.wav");
		CreateWaveBuffer(Bombers[0].SndBonus=cnt++,"Bonus.wav");
		CreateWaveBuffer(Bombers[0].SndVache=cnt++,"Vache.wav");
		CreateWaveBuffer(Bombers[0].SndMort=cnt++,"Mort.wav");
		CreateWaveBuffer(Bombers[0].SndAK=cnt++,"Suicide.wav");
		CreateWaveBuffer(Bombers[0].SndTeleport=cnt++,"Teleport.wav");
		for(i=1;i<Num;i++)
		{
			Bombers[i].SndExplose=DupliqueSon(Bombers[0].SndExplose,cnt++);
			Bombers[i].SndBonus=DupliqueSon(Bombers[0].SndBonus,cnt++);
			Bombers[i].SndVache=DupliqueSon(Bombers[0].SndVache,cnt++);
			Bombers[i].SndMort=DupliqueSon(Bombers[0].SndMort,cnt++);
			Bombers[i].SndAK=DupliqueSon(Bombers[0].SndAK,cnt++);
			Bombers[i].SndTeleport=DupliqueSon(Bombers[0].SndTeleport,cnt++);
		}
	}
	else
	{
		for(i=0;i<Num;i++)
		{
			Temporize();
			CreateWaveBuffer(Bombers[i].SndExplose=cnt++,"Explosion.wav");
			CreateWaveBuffer(Bombers[i].SndBonus=cnt++,"Bonus.wav");
			CreateWaveBuffer(Bombers[i].SndVache=cnt++,"Vache.wav");
			CreateWaveBuffer(Bombers[i].SndMort=cnt++,"Mort.wav");
			CreateWaveBuffer(Bombers[i].SndAK=cnt++,"Suicide.wav");
			CreateWaveBuffer(Bombers[i].SndTeleport=cnt++,"Teleport.wav");
		}
	}
	
	if(Settings.GO.SDDEATH_TOUT!=0)
	{
		CreateWaveBuffer(Sons.SndBeep=cnt++,"Beep.wav");
		UnloadFile("Beep.wav");
	}

	Temporize();
	UnloadFile("Explosion.wav");
	UnloadFile("Bonus.wav");
	UnloadFile("Vache.wav");
	UnloadFile("Mort.wav");
	UnloadFile("Suicide.wav");
	UnloadFile("Teleport.wav");
  return;
}

void LoadSkin(const char* szSkin)
{
	EnumAll();
	if(!DoesSkinExist(szSkin))
		return;
	if(strcmp(Settings.szSelSkin,szSkin)==0)
		return;
	strcpy(Settings.szSelSkin,szSkin);
	FinSon();
	CloseFileLoader();
	InitFileSystem();
	LoadSprites();
	InitDSound();
	return;
}

void LoadSprites(void)
{
#ifndef DX3
		DDSURFACEDESC2 ddsd;
#else
		DDSURFACEDESC ddsd;
#endif

	if(lpDDSprites==0) 
		return;
	if(!LoadFile(szSprFile))
		{ErrorMsg("Fichier des sprites introuvable","Erreur");}
	if(DDReLoadBitmap(lpDDSprites,szSprFile)!=DD_OK) 
		ErrorMsg(LSZ_DD_CANTLOADSPR,LSZ_DD_INIT);
	UnloadFile(szSprFile);

	ddsd.dwSize=sizeof(ddsd);
	ddsd.ddpfPixelFormat.dwSize=sizeof(ddsd.ddpfPixelFormat);
	ddsd.dwFlags=DDSD_PIXELFORMAT;
	lpDDSPrimary->GetSurfaceDesc(&ddsd);

	if((Settings.Depth==32)||(Settings.DBGMODE&&(ddsd.ddpfPixelFormat.dwRGBBitCount==32)))
	{
		DWORD* ptr;
		ddsd.dwSize=sizeof(ddsd);
		lpDDSprites->Lock(0,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,0);
		ptr=(DWORD*)ddsd.lpSurface;
		for(i=0;i<SPR_SIZEX*SPR_SIZEY;i++)
			ptr[i]&=0xffffff;
		lpDDSprites->Unlock(0);
	}

	DDSetColorKey(lpDDSprites,RGB(255,0,255));
	return;
}

DWORD TickCount(void)
{

	LARGE_INTEGER lgi1,lgi2;
	QueryPerformanceFrequency(&lgi1);
	QueryPerformanceCounter(&lgi2);
	return int(1000*float(lgi2.LowPart)/float(lgi1.LowPart))-StartTick;

//	return GetTickCount()-StartTick;
}

DWORD fTickCount(void)
{
	LARGE_INTEGER lgi;
	QueryPerformanceCounter(&lgi);
	return lgi.u.LowPart;
}

BOOL FAR PASCAL EnumPlayersCallback(DPID dpId,DWORD dwPlayerType,
									 LPCDPNAME lpName,DWORD dwFlags,LPVOID lpContext)
{
	if(dwPlayerType!=DPPLAYERTYPE_PLAYER) 
		return TRUE;
	if((dwFlags&DPENUMPLAYERS_SPECTATOR)!=0)
		return TRUE;
	DPlayer[*(int*)lpContext].dpid=dpId;
	DPlayer[*(int*)lpContext].dpName=*lpName;
	if(lpName->lpszShortNameA)
	{
		DPlayer[*(int*)lpContext].dpName.lpszShortNameA=new char[1+strlen(lpName->lpszShortNameA)];
		strcpy(DPlayer[*(int*)lpContext].dpName.lpszShortNameA,lpName->lpszShortNameA);
	}
	(*(int*)lpContext)++;
	return TRUE;
}

DWORD GetRealNumPlayers(void)
{
	DWORD c=0;
	if(lpDP==0) return 0;
	if(lpDP->EnumPlayers(0,EnumCountRealPlayersCallback,&c,DPENUMPLAYERS_ALL)!=DP_OK) 
		return 0;
	return c;
}

BOOL FAR PASCAL EnumCountRealPlayersCallback(DPID dpId,DWORD dwPlayerType,
									 LPCDPNAME lpName,DWORD dwFlags,LPVOID lpContext)
{
	if(dwPlayerType!=DPPLAYERTYPE_PLAYER) 
		return TRUE;
	if((dwFlags&DPENUMPLAYERS_SPECTATOR)!=0)
		return TRUE;
	(*(DWORD*)lpContext)++;
	return TRUE;
}

int GetAsyncKState(BYTE touche) 
{
	if(bChat)
		return 0;
	if(Settings.USEDINPUT)
		return (Keyboard[touche]&0x80);
	else
		switch(touche)
		{
			case VK_PAUSE:
			return GetAsyncKeyState(touche);
			default:
			return GetAsyncKeyState(touche)&0x80000000;
		}
}

void AddPlayerToScores(int Num)
{
	for(int scnt=0;scnt<MAX_SCORES;scnt++)
		if(!Score[scnt].bSet||(strcmp(Score[scnt].Nom,Bombers[Num].Nom)==0))
		{
			Score[scnt].bSet=TRUE;
			Score[scnt].Frag=Bombers[Num].Frag;
			Score[scnt].AK=Bombers[Num].AK;
			Score[scnt].Parties=Bombers[Num].Parties;
			strcpy(Score[scnt].Nom,Bombers[Num].Nom);
			return;
		}

	int LeastScore=BPOINTS(Num);
	int Least=-1;
	for(scnt=0;scnt<MAX_SCORES;scnt++)
		if(SCPOINTS(scnt)<LeastScore) Least=scnt;

	if(Least>=0)
	{
		Score[Least].bSet=TRUE;
		Score[Least].Frag=Bombers[Num].Frag;
		Score[Least].AK=Bombers[Num].AK;
		Score[Least].Parties=Bombers[Num].Parties;
		strcpy(Score[Least].Nom,Bombers[Num].Nom);
	}
	return;
}

void AddTeamToScores(int Num)
{
	for(int scnt=0;scnt<MAX_SCORES;scnt++)
		if(!Score[scnt].bSet||(strcmp(Score[scnt].Nom,TeamName[Num])==0))
		{
			Score[scnt].bSet=TRUE;
			Score[scnt].Frag=TFRAGS(Num);
			Score[scnt].AK=TAK(Num);
			Score[scnt].Parties=TPARTIES(Num);
			strcpy(Score[scnt].Nom,TeamName[Num]);
			return;
		}

	int LeastScore=TPOINTS(Num);
	int Least=-1;
	for(scnt=0;scnt<MAX_SCORES;scnt++)
		if(SCPOINTS(scnt)<LeastScore) Least=scnt;

	if(Least>=0)
	{
		Score[Least].bSet=TRUE;
		Score[scnt].Frag=TFRAGS(Num);
		Score[scnt].AK=TAK(Num);
		Score[scnt].Parties=TPARTIES(Num);
		strcpy(Score[scnt].Nom,TeamName[Num]);
	}
	return;
}

void ShowScores(void)
{
	DWORD RunTime;
	char Title[BMAX_PATH];
	int NumScores=0;
	int jo,pcnt,pcnt1;
	struct RegScore rscr;
	struct EncRegScore erscr;
	FILE* F=0;

	for(pcnt=0;pcnt<MAX_SCORES;pcnt++)
		if(Score[pcnt].bSet) 
			NumScores++;
		else
		{
			for(pcnt1=pcnt+1;pcnt1<MAX_SCORES;pcnt1++)
				Score[pcnt1-1]=Score[pcnt1];
			Score[MAX_SCORES-1].bSet=0;
		}

	if(NumScores<=1)
		return;

	RunTime=TickCount()-StartTime;
	if(RunTime>3600*1000)
		sprintf(Title,LSZ_SCAFTERdHdMdS,RunTime/(3600*1000),(RunTime/(60*1000))%60,(RunTime/1000)%60);
	else if(RunTime>60*1000)
		sprintf(Title,LSZ_SCAFTERdMdS,(RunTime/(60*1000))%60,(RunTime/1000)%60);
	else
		sprintf(Title,LSZ_SCAFTERdS,(RunTime/1000)%60);

	BOOL Xchg;
	struct BomberScore ScTemp;
	do
	{
		Xchg=0;
		for(i=0;i<NumScores-1;i++)
			if(SCPOINTS(i+1)>SCPOINTS(i))
			{
				Xchg=TRUE;
				ScTemp=Score[i+1];
				Score[i+1]=Score[i];
				Score[i]=ScTemp;
			}
			else
				if((SCPOINTS(i+1)==SCPOINTS(i))&&(Score[i+1].Frag>Score[i].Frag))
				{
					Xchg=TRUE;
					ScTemp=Score[i+1];
					Score[i+1]=Score[i];
					Score[i]=ScTemp;
				}

	}
	while(Xchg);

	memset(Buff,0,BMAX_PATH);
	for(i=0;i<NumScores;i++)
	{
		sprintf(Buff1,"%s\n",Score[i].Nom);
		strcat(Buff,Buff1);
		sprintf(Buff1,"%ld Points\n",SCPOINTS(i));
		strcat(Buff,Buff1);
		sprintf(Buff1,LSZ_dVICTIMES,Score[i].Frag);
		strcat(Buff,Buff1);
		sprintf(Buff1,LSZ_ANDdAK,Score[i].AK);
		strcat(Buff,Buff1);
	}

	MessageBox(0,Buff,Title,MB_OK);

	if(Settings.SAVESCORES)
	{
		_chdir(szWorkingDirectory);
		BOOL bOK=TRUE;
		F=fopen(szScoreFile,"r+b");
		if(F!=0)
		{
			DWORD s,s1;
			BYTE dwS[4];
			fread(dwS,4,1,F);
			if(memcmp(dwS,dwSign,4)==0)
			{
				bOK=TRUE;
				fseek(F,0,SEEK_END);
				s=ftell(F)-4;
				s/=sizeof(struct EncRegScore);
				fseek(F,4,SEEK_SET);
			}
			else
			{
				s=0;
				bOK=0;
			}

			for(s1=0;s1<s;s1++)
			{
				memset(&erscr,0,sizeof(struct EncRegScore));
				fread(&erscr,sizeof(struct EncRegScore),1,F);
				if(!DecodeScore(&erscr,&rscr))
				{
					bOK=0;
					break;
				}
				for (jo=0;jo<NumScores;jo++)
					if(Score[jo].bSet)
						if(strcmp(rscr.Nom,Score[jo].Nom)==0)
						{
							Score[jo].bSet=0;
							rscr.Frag+=Score[jo].Frag;
							rscr.AK+=Score[jo].AK;
							rscr.Parties+=Score[jo].Parties;
							EncodeScore(&rscr,&erscr);
							int ft=ftell(F);
							fseek(F,ft-sizeof(struct EncRegScore),SEEK_SET);
							fwrite(&erscr,sizeof(struct EncRegScore),1,F);
							fseek(F,ft,SEEK_SET);
						}
			}
			fclose(F);
		}

		if(!bOK)
			if(IDYES==MessageBox(0,LSZ_ERRSCFILE,LSZ_SCORES,MB_YESNO|MB_DEFBUTTON1))
				remove(szScoreFile);

		int t=GetFileLength(szScoreFile);
		F=fopen(szScoreFile,"a+b");
		fclose(F);
		F=fopen(szScoreFile,"r+b");
		if(F!=0)
		{
			fseek(F,0,SEEK_SET);
			fwrite(dwSign,4,1,F);
			fseek(F,t>4?t:4,SEEK_SET);
			for (jo=0;jo<NumScores;jo++)
				if(Score[jo].bSet)
				{
					memset(&rscr,0,sizeof(struct RegScore));
					Score[jo].bSet=0;
					strcpy(rscr.Nom,Score[jo].Nom);
					rscr.Frag=Score[jo].Frag;
					rscr.AK=Score[jo].AK;
					rscr.Parties=Score[jo].Parties;
					EncodeScore(&rscr,&erscr);
					fwrite(&erscr,sizeof(struct EncRegScore),1,F);
				}

			fclose(F);
		}
	}
	return;
}

int GetFileLength(const char* file)
{
	FILE* F=fopen(file,"rb");
	int t;
	if(F!=0)
	{
		fseek(F,0,SEEK_END);
		t=ftell(F);
		fclose(F);
		return t;
	}
	else
		return 0;
}

void MsgBoxScores(void)
{
const int AFF_SCORE=128;
struct EncRegScore eiScore;
struct RegScore	iScore[AFF_SCORE];
int		iidx=0;
FILE*	F=0;
BYTE	dwS[4];

	_chdir(szWorkingDirectory);
	F=fopen(szScoreFile,"rb");
	if(F==0) goto Fail;
	fread(dwS,4,1,F);
	if(memcmp(dwS,dwSign,4)!=0)
		goto Fail1;
	iidx=0;
	while((fread(&eiScore,sizeof(struct EncRegScore),1,F)==1)&&(iidx<AFF_SCORE))
	{
		if(DecodeScore(&eiScore,&iScore[iidx]))
			iidx++;
		else
		{
			fclose(F);
			goto Fail1;
		}
	}
	fclose(F);

	BOOL Xchg;
	struct RegScore Temp;
	do
	{
		Xchg=0;
		for(i=0;i<iidx-1;i++)
		{
			int cip=CALCPOINTS(iScore[i+1].Frag,iScore[i+1].AK);
			int ci=CALCPOINTS(iScore[i].Frag,iScore[i].AK);
			if(cip>ci)
			{
				Xchg=TRUE;
				Temp=iScore[i+1];
				iScore[i+1]=iScore[i];
				iScore[i]=Temp;
			}
			else
				if((cip==ci)&&((iScore[i+1].Frag)>(iScore[i].Frag)))
				{
					Xchg=TRUE;
					Temp=iScore[i+1];
					iScore[i+1]=iScore[i];
					iScore[i]=Temp;
				}
		}
	}
	while(Xchg);

	char String[2048];
	sprintf(String,LSZ_SCENTITLE);
	for(i=0;i<iidx;i++)
	{
		//"N:NOM\t\tPOINTS\tPARTIES VICTIMES SUICIDES\n"
		if(strlen(iScore[i].Nom)<8)
			sprintf(Buff,"%ld: %s\t\t%6ld\t%7ld\t%8ld\t%8ld\n",
				i+1,iScore[i].Nom,Settings.GO.MORTPNT*int(iScore[i].Frag)-Settings.GO.AKPNT*int(iScore[i].AK),iScore[i].Parties,iScore[i].Frag,iScore[i].AK);
		else
			sprintf(Buff,"%ld: %s\t%6ld\t%7ld\t%8ld\t%8ld\n",
				i+1,iScore[i].Nom,Settings.GO.MORTPNT*int(iScore[i].Frag)-Settings.GO.AKPNT*int(iScore[i].AK),iScore[i].Parties,iScore[i].Frag,iScore[i].AK);
		if(strlen(String)+strlen(Buff)>2000)
		{
			strcat(String,"...");
			MessageBox(0,String,LSZ_BESTPSCORES,MB_OK);
//			strcpy(String,"...\n");
			sprintf(String,LSZ_SCENTITLE);
		}
		strcat(String,Buff);
	}

	MessageBox(0,String,LSZ_BESTPSCORES,MB_OK);

	do
	{
		Xchg=0;
		for(i=0;i<iidx-1;i++)
		{
			float cp=CALCPOINTS(iScore[i+1].Frag,iScore[i+1].AK)/(float)iScore[i+1].Parties;
			float c=CALCPOINTS(iScore[i].Frag,iScore[i].AK)/(float)iScore[i].Parties;
			if(cp>c)
			{
				//OutFileEx2("%f contre %f",CALCPOINTS(iScore[i+1].Frag,iScore[i+1].AK)/(float)iScore[i+1].Parties,CALCPOINTS(iScore[i].Frag,iScore[i].AK)/(float)iScore[i].Parties);
				Xchg=TRUE;
				Temp=iScore[i+1];
				iScore[i+1]=iScore[i];
				iScore[i]=Temp;
			}
			else
				if((cp==c)&&(iScore[i+1].Frag>iScore[i].Frag))
				{
					Xchg=TRUE;
					Temp=iScore[i+1];
					iScore[i+1]=iScore[i];
					iScore[i]=Temp;
				}
				else
					if((cp==c)&&(iScore[i+1].Frag==iScore[i].Frag)&&(iScore[i+1].Parties<iScore[i].Parties))
					{
						Xchg=TRUE;
						Temp=iScore[i+1];
						iScore[i+1]=iScore[i];
						iScore[i]=Temp;
					}
		}
	}
	while(Xchg);

	sprintf(String,LSZ_SCENTITLE);
	for(i=0;i<iidx;i++)
	{
		//"N:NOM\t\tPOINTS\tPARTIES VICTIMES SUICIDES\n"
		if(strlen(iScore[i].Nom)<8)
			sprintf(Buff,"%ld: %s\t\t%6ld\t%7ld\t%8ld\t%8ld\n",
				i+1,iScore[i].Nom,Settings.GO.MORTPNT*int(iScore[i].Frag)-Settings.GO.AKPNT*int(iScore[i].AK),iScore[i].Parties,iScore[i].Frag,iScore[i].AK);
		else
			sprintf(Buff,"%ld: %s\t%6ld\t%7ld\t%8ld\t%8ld\n",
				i+1,iScore[i].Nom,Settings.GO.MORTPNT*int(iScore[i].Frag)-Settings.GO.AKPNT*int(iScore[i].AK),iScore[i].Parties,iScore[i].Frag,iScore[i].AK);
		if(strlen(String)+strlen(Buff)>2000)
		{
			strcat(String,"...");
			MessageBox(0,String,LSZ_BESTRSCORES,MB_OK);
//			strcpy(String,"...\n");
			sprintf(String,LSZ_SCENTITLE);
		}
		strcat(String,Buff);
	}

	MessageBox(0,String,LSZ_BESTRSCORES,MB_OK);
	if(IDYES==MessageBox(0,LSZ_DELSCF,LSZ_SCORES,MB_YESNO|MB_DEFBUTTON2))
		remove(szScoreFile);
	return;

Fail:
	MessageBox(0,LSZ_NOSAVEDSCORES,LSZ_SCORES,MB_OK);
	return;
Fail1:
	if(IDYES==MessageBox(0,LSZ_ERRSCFILE,LSZ_SCORES,MB_YESNO|MB_DEFBUTTON1))
		remove(szScoreFile);
	return;
}
void SendMail(void)
{
//	MAIL !!!!!!!!!!!!
	/*
	LPMAPISENDMAIL lpfnMAPISendMail = NULL;
	HINSTANCE hMAPIDLL=0;
	MapiMessage	MAPIMsg;
	MapiRecipDesc Orig,Sto[2];

	memset(&MAPIMsg,0,sizeof(MapiMessage));
	memset(&Orig,0,sizeof(MapiRecipDesc));
	memset(&Sto,0,sizeof(MapiRecipDesc));

	MAPIMsg.flFlags=MAPI_RECEIPT_REQUESTED;
	MAPIMsg.lpOriginator=&Orig;
	MAPIMsg.nRecipCount=2;
	MAPIMsg.lpRecips=Sto;

	Orig.ulRecipClass=MAPI_ORIG;
	Orig.lpszName="Sender";

	Sto[0].ulRecipClass=MAPI_TO;
	Sto[0].lpszAddress="SMTP:petitgrf@esiee.fr";
	Sto[0].lpszName="petitgrf@esiee.fr";

	Sto[1].ulRecipClass=MAPI_TO;
	Sto[1].lpszAddress="SMTP:fabienpg@club-internet.fr";
	Sto[1].lpszName="fabienpg@club-internet.fr";

	MAPIMsg.lpszSubject="Scores Bomber";
	MAPIMsg.lpszNoteText=new char[BMAX_PATH];
	memset(MAPIMsg.lpszNoteText,0,BMAX_PATH);

	for(i=0;i<MAX_SCORES;i++)
	{
		if(Score[i].bSet)
		{
			sprintf(Buff,"%s\n%ld\n%ld\n",Score[i].Nom,Score[i].Frag,Score[i].AK);
			strcat(MAPIMsg.lpszNoteText,Buff);
		}
	}

	hMAPIDLL=LoadLibrary("MAPI32.DLL");
	if(hMAPIDLL==0) 
		ErrorMsg("MAPI n'est pas correctement installé","SendMail");

	lpfnMAPISendMail=(LPMAPISENDMAIL)GetProcAddress(hMAPIDLL,"MAPISendMail");

	if(lpfnMAPISendMail!=0)
		i=lpfnMAPISendMail(0,0,&MAPIMsg,MAPI_LOGON_UI|MAPI_NEW_SESSION,0);

	FreeLibrary(hMAPIDLL);
	SAFE_DELETE_ARRAY(MAPIMsg.lpszNoteText)
	if(i!=SUCCESS_SUCCESS)
		OutFile("Erreur lors de l'envoi des Scores");
*/
	return;
}

void SetDefaultGameOptions(struct GameOptions* GO)
{
BYTE BonusRepart[NUM_BONUS+1]=
	{
			150,	//aucun bonus
			0,	//inutilisé
			0,	//inutilisé
			50,	//blast
			45,	//+1 bomb
			14,	//mega bomb
			20,	//crt
			10,	//std
			20,	//lng
			20,	//rig
			25,	//ret
			8,	//nucl
			15,	//allexpl
			0,	//inutilisé
			10,	//teleport
			20,	//mine
			15	//reverse
	};

	memset(GO,0,sizeof(struct GameOptions));
	memcpy(GO->BonusRepart,BonusRepart,NUM_BONUS+1);
	SetDefaultStartPos(GO);
	GO->BLOC_COND=50;
	GO->SIMULBOMB=10;
	GO->NBBOMB=1;
	GO->NBBLAST=1;
	GO->NBMINE=0;
	GO->RETNUCL=TRUE;
	GO->DISPRETNUCL=FALSE;
	GO->CANREV=FALSE;
	GO->MORTPNT=3;
	GO->AKPNT=1;
	GO->REV_TOUT=15000;
	GO->MCH_TOUT=25000;
	GO->SDDEATH_TOUT=0;
	GO->BOTLEVEL=3;
	return;
}

void SetDefaultTextMessages(struct TextMessages* pTM)
{
	strcpy(pTM->F1TxtMsg,LSZ_NOF1MSG);
	strcpy(pTM->F2TxtMsg,LSZ_NOF2MSG);
	strcpy(pTM->F3TxtMsg,LSZ_NOF3MSG);
	strcpy(pTM->F4TxtMsg,LSZ_NOF4MSG);
	strcpy(pTM->F5TxtMsg,LSZ_NOF5MSG);
	strcpy(pTM->F6TxtMsg,LSZ_NOF6MSG);
	strcpy(pTM->F7TxtMsg,LSZ_NOF7MSG);
	strcpy(pTM->F8TxtMsg,LSZ_NOF8MSG);
	return;
}

void SetDefaultStartPos(struct GameOptions* GO)
{
BYTE	StartPos[]={
		0,0,
		19,14,
		19,0,
		0,14
};
	memcpy(GO->StartPos,StartPos,2*MAX_PLAYERS);
	return;
}

#ifndef DX3
void SetDefaultOptions(struct SETTINGS* pSettings)
{
	memset(pSettings,0,sizeof(struct SETTINGS));
	pSettings->DBGMODE=FALSE;
	pSettings->USEDPPROTO=TRUE;
	pSettings->USEDINPUT=TRUE;
	pSettings->USEFLIP=TRUE;
	pSettings->USETBUFFER=FALSE;
	pSettings->DEBUG=FALSE;
	pSettings->DUPSBUFF=TRUE;
	pSettings->SPRVIDEO=TRUE;
	pSettings->ASYNCENUM=TRUE;
	pSettings->ENUMEVERY=TRUE;
	pSettings->Depth=16;
	pSettings->TEVERY=1000;
	pSettings->NET_TOUT=2;
	pSettings->ENABLESON=TRUE;
	pSettings->HISOUND=TRUE;
	pSettings->VSYNC=FALSE;
	pSettings->TELSVR=TRUE;
	pSettings->TELCLNT=TRUE;
	pSettings->LOADSKIN=TRUE;
	pSettings->SHOWPING=FALSE;
	pSettings->MAPDELAY=0;
	pSettings->FILE_BLOCK_SIZE=256;
	pSettings->ALLOWSPECTATORS=TRUE;
	pSettings->MIGRATEHOST=TRUE;
	pSettings->Brightness=750;
	pSettings->Contrast=10000;
	pSettings->Volume=9300;
	pSettings->IsIA[0]=FALSE;
	pSettings->IsIA[1]=FALSE;
	pSettings->IsIA[2]=FALSE;
	pSettings->IsIA[3]=FALSE;
	pSettings->SAVESCORES=TRUE;
	pSettings->BOMBTCOLOR=TRUE;
	pSettings->PLAYERTCOLOR=FALSE;
	pSettings->VOICE=FALSE;
	pSettings->SERVER_TYPE=ST_NORMAL;
	strcpy(pSettings->szConfigName,LSZ_DEFAULT);
	pSettings->szSelMap[0]='\0';
	pSettings->szSelSkin[0]='\0';

	strcpy(pSettings->Nom[0],LSZ_NAME1);
	strcpy(pSettings->Nom[1],LSZ_NAME2);
	strcpy(pSettings->Nom[2],LSZ_NAME3);
	strcpy(pSettings->Nom[3],LSZ_NAME4);
	//strcpy(pSettings->NickName,"Surnom");
	SetDefaultGameOptions(&pSettings->GO);
	SetDefaultControls(&pSettings->C);
	SetDefaultTextMessages(&pSettings->TM);
	return;
}
#else
void SetDefaultOptions(struct SETTINGS* pSettings)
{
	memset(pSettings,0,sizeof(struct SETTINGS));
	pSettings->DBGMODE=FALSE;
	pSettings->USEDPPROTO=FALSE;
	pSettings->USEDINPUT=FALSE;
	pSettings->USEFLIP=FALSE;
	pSettings->USETBUFFER=FALSE;
	pSettings->DEBUG=FALSE;
	pSettings->DUPSBUFF=TRUE;
	pSettings->SPRVIDEO=TRUE;
	pSettings->ASYNCENUM=FALSE;
	pSettings->ENUMEVERY=TRUE;
	pSettings->Depth=16;
	pSettings->TEVERY=1000;
	pSettings->NET_TOUT=2;
	pSettings->ENABLESON=FALSE;
	pSettings->HISOUND=FALSE;
	pSettings->VSYNC=FALSE;
	pSettings->TELSVR=TRUE;
	pSettings->TELCLNT=TRUE;
	pSettings->LOADSKIN=TRUE;
	pSettings->SHOWPING=FALSE;
	pSettings->MAPDELAY=0;
	pSettings->FILE_BLOCK_SIZE=256;
	pSettings->ALLOWSPECTATORS=TRUE;
	pSettings->MIGRATEHOST=FALSE;
	pSettings->Brightness=750;
	pSettings->Contrast=10000;
	pSettings->Volume=9300;
	pSettings->IsIA[0]=FALSE;
	pSettings->IsIA[1]=FALSE;
	pSettings->IsIA[2]=FALSE;
	pSettings->IsIA[3]=FALSE;
	pSettings->SAVESCORES=TRUE;
	pSettings->BOMBTCOLOR=TRUE;
	pSettings->PLAYERTCOLOR=FALSE;
	pSettings->VOICE=FALSE;
	pSettings->DEDICATED=ST_NORMAL;
	strcpy(pSettings->szConfigName,LSZ_DEFAULT);
	pSettings->szSelMap[0]='\0';
	pSettings->szSelSkin[0]='\0';

	strcpy(pSettings->Nom[0],LSZ_NAME1);
	strcpy(pSettings->Nom[1],LSZ_NAME2);
	strcpy(pSettings->Nom[2],LSZ_NAME3);
	strcpy(pSettings->Nom[3],LSZ_NAME4);
	//strcpy(pSettings->NickName,"Surnom");
	SetDefaultGameOptions(&pSettings->GO);
	SetDefaultControls(&pSettings->C);
	SetDefaultTextMessages(&pSettings->TM);
	return;
}
#endif

void SetDefaultControls(struct Controls* pC)
{
#ifndef GOA
BYTE	Def_Key_UP[4]={'E',VK_NUMPAD8,'I',VK_UP};
BYTE	Def_Key_DOWN[4]={'D',VK_NUMPAD5,'K',VK_DOWN};
BYTE	Def_Key_LEFT[4]={'S',VK_NUMPAD4,'J',VK_LEFT};
BYTE	Def_Key_RIGHT[4]={'F',VK_NUMPAD6,'L',VK_RIGHT};
BYTE	Def_Key_FIRE1[4]={VK_CONTROL,VK_NUMPAD0,VK_SPACE,VK_END};
BYTE	Def_Key_FIRE2[4]={VK_SHIFT,VK_NUMPAD7,'B',VK_DELETE};
BYTE	Def_Key_FIRE3[4]={VK_MENU,VK_ADD,'N',VK_HOME};

BYTE	Def_DIKey_UP[4]={DIK_E,DIK_NUMPAD8,DIK_I,DIK_UP};
BYTE	Def_DIKey_DOWN[4]={DIK_D,DIK_NUMPAD5,DIK_K,DIK_DOWN};
BYTE	Def_DIKey_LEFT[4]={DIK_S,DIK_NUMPAD4,DIK_J,DIK_LEFT};
BYTE	Def_DIKey_RIGHT[4]={DIK_F,DIK_NUMPAD6,DIK_L,DIK_RIGHT};
BYTE	Def_DIKey_FIRE1[4]={DIK_LCONTROL,DIK_NUMPAD0,DIK_SPACE,DIK_END};
BYTE	Def_DIKey_FIRE2[4]={DIK_LSHIFT,DIK_NUMPAD7,DIK_B,DIK_DELETE};
BYTE	Def_DIKey_FIRE3[4]={DIK_LMENU,DIK_ADD,DIK_N,DIK_HOME};
#else
BYTE	Def_Key_UP[4]={VK_UP,VK_NUMPAD8,'I','E'};
BYTE	Def_Key_DOWN[4]={VK_DOWN,VK_NUMPAD5,'K','D'};
BYTE	Def_Key_LEFT[4]={VK_LEFT,VK_NUMPAD4,'J','S'};
BYTE	Def_Key_RIGHT[4]={VK_RIGHT,VK_NUMPAD6,'L','F'};
BYTE	Def_Key_FIRE1[4]={VK_CONTROL,VK_NUMPAD0,VK_SPACE,VK_END};
BYTE	Def_Key_FIRE2[4]={VK_SHIFT,VK_NUMPAD7,'B',VK_DELETE};
BYTE	Def_Key_FIRE3[4]={VK_MENU,VK_ADD,'N',VK_HOME};

BYTE	Def_DIKey_UP[4]={DIK_UP,DIK_NUMPAD8,DIK_I,DIK_E};
BYTE	Def_DIKey_DOWN[4]={DIK_DOWN,DIK_NUMPAD5,DIK_K,DIK_D};
BYTE	Def_DIKey_LEFT[4]={DIK_LEFT,DIK_NUMPAD4,DIK_J,DIK_S};
BYTE	Def_DIKey_RIGHT[4]={DIK_RIGHT,DIK_NUMPAD6,DIK_L,DIK_F};
BYTE	Def_DIKey_FIRE1[4]={DIK_LCONTROL,DIK_NUMPAD0,DIK_SPACE,DIK_END};
BYTE	Def_DIKey_FIRE2[4]={DIK_LSHIFT,DIK_NUMPAD7,DIK_B,DIK_DELETE};
BYTE	Def_DIKey_FIRE3[4]={DIK_LMENU,DIK_ADD,DIK_N,DIK_HOME};
#endif

	memcpy(pC->Key_UP,Def_Key_UP,4);
	memcpy(pC->Key_DOWN,Def_Key_DOWN,4);
	memcpy(pC->Key_LEFT,Def_Key_LEFT,4);
	memcpy(pC->Key_RIGHT,Def_Key_RIGHT,4);
	memcpy(pC->Key_FIRE1,Def_Key_FIRE1,4);
	memcpy(pC->Key_FIRE2,Def_Key_FIRE2,4);
	memcpy(pC->Key_FIRE3,Def_Key_FIRE3,4);

	memcpy(pC->DIKey_UP,Def_DIKey_UP,4);
	memcpy(pC->DIKey_DOWN,Def_DIKey_DOWN,4);
	memcpy(pC->DIKey_LEFT,Def_DIKey_LEFT,4);
	memcpy(pC->DIKey_RIGHT,Def_DIKey_RIGHT,4);
	memcpy(pC->DIKey_FIRE1,Def_DIKey_FIRE1,4);
	memcpy(pC->DIKey_FIRE2,Def_DIKey_FIRE2,4);
	memcpy(pC->DIKey_FIRE3,Def_DIKey_FIRE3,4);
	return;
}

BOOL CheckOSAndDX(void)
{
	DWORD OSv,DXv;
	GetDXVersion(&DXv,&OSv);
	sprintf(Buff,"DirectX version %ld.%ld",(DXv>>8),DXv&0xff);
	if(DXv!=0)
		OutFile(Buff);
	else
		OutFile(LSZ_UNKNOWN_DXVER);

	if(OSv==VER_PLATFORM_WIN32_WINDOWS)
		OutFile("Windows 9x");
	else
		if(OSv==VER_PLATFORM_WIN32_NT)
			OutFile("Windows NT");
		else
			OutFile(LSZ_UNKNOWN_WIN);

#ifndef DX3
	if(DXv<0x600)
		if(MessageBox(0,LSZ_DX6ORH,LSZ_ATTENTION,MB_YESNO)==IDYES)
			return TRUE;
		else
			return FALSE;
#else
	if(DXv<0x300)
		if(MessageBox(0,LSZ_DX3ORH,LSZ_ATTENTION,MB_YESNO)==IDYES)
			return TRUE;
		else
			return FALSE;
#endif

	return TRUE;
}

char* KeyName(BYTE vcode)
{
	BYTE scancode=(BYTE)(255&MapVirtualKey(vcode,0));
	if((scancode==0)||(strlen(tszName[scancode])==0))
	{
		sprintf(szGenericKey,LSZ_KEYd,vcode);
		return szGenericKey;
	}
	else
		return tszName[scancode];
}

BYTE atob(const char* string)
{
	int v=atol(string);
	if(v<0)
		return 0;
	if(v>255)
		return 255;
	return (BYTE)v;
}

DWORD atodw(const char* string)
{
	int v=atol(string);
	if(v<0)
		return 0;
	return (DWORD)v;
}

void FlipToGDI(void)
{
	if(lpDD!=0)
		lpDD->FlipToGDISurface();
	return;
}

int GetUpdateCount(void)
{
	if(Settings.USEFLIP)
		return (Settings.USETBUFFER?3:2);
	else
		return 1;
}

BOOL MemeTeam(int n1,int n2)
{
	if((Bombers[n1].Team!=VOID_TEAM)||(Bombers[n2].Team!=VOID_TEAM))
		return (Bombers[n1].Team==Bombers[n2].Team);
	else
		return FALSE;
}

void UpdateBombCount(void)
{
	for(int ipl=0;ipl<NumPlayers;ipl++)
		for(int iplb=0;iplb<TOT_BOMB;iplb++)
			if(Bombs[ipl][iplb].Etat==B_PLAYING)
				NumBombs[ipl]=iplb+1;
	return;
}

void UpdateBlastCount(void)
{
	for(int ipl=0;ipl<NumPlayers;ipl++)
		for(int iplb=0;iplb<TOT_BLAST;iplb++)
			if(Blasts[ipl][iplb].Etat==B_PLAYING)
				NumBlasts[ipl]=iplb+1;
	return;
}

DWORD FileCRC(FILE* Fich)
{
	if(Fich==NULL)
		return 0;
	fseek(Fich,0,SEEK_END);
	DWORD dwSize=ftell(Fich);
	BYTE* lpCRCMemory=new BYTE[dwSize];
	rewind(Fich);
	fread(lpCRCMemory,dwSize,1,Fich);
	DWORD CRCValue=MemoryCRC(lpCRCMemory,dwSize);
	SAFE_DELETE_ARRAY(lpCRCMemory);
	return CRCValue;
}

DWORD MemoryCRC(BYTE* lpCRC,DWORD dwSize)
{
	DWORD CRCValue=dwSize;
	for(DWORD ip=0;ip<dwSize;ip++)
		CRCValue^=((DWORD)lpCRC[i])<<(i%4);
	return CRCValue!=0 ? CRCValue:1;
}
