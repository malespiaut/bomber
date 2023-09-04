#ifndef _GENERAL_H_
#define _GENERAL_H_
#undef STRICT
#define STRICT
#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#undef IDIRECTPLAY2_OR_GREATER
#define IDIRECTPLAY2_OR_GREATER

#undef GCOMP_BZ
#undef GCOMP_LZO
#undef GCOMP_UCL
#undef GCOMP_RAR
#undef GCOMP_ACE

//#define GCOMP_ACE
//#define GCOMP_RAR
//#define GCOMP_LZO
//#define GCOMP_BZ
#define GCOMP_UCL

#undef DX7
#undef DX5
#undef DX3
#define DX7

#undef WSDP
//#define WSDP

#undef min
#undef max
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

const int BMAX_NAME=32;
const int BMAX_PATH=128;

const int MAX_VIDMODES=8;
const int MAX_PLAYERS=4;
const int MAX_SESSION_PLAYERS=0;
const int MAX_SCORES=8;

const int MAX_MAPS=64;
const int MAX_CONFIGS=64;
const int MAX_SKINS=64;
const int MAX_LOADEDFILES=16;

const int TOT_TXTMSG=16;
const int TOT_BOMB=96;
const int TOT_BLAST=256;
const int NUM_BONUS=17;

const int ST_DEDICATED=0;
const int ST_MULTICAST=1;
const int ST_NORMAL=2;

const int BS_PLAYING=1<<1;
const int BS_MULTICAST=1<<2;
const int BS_DEDICATED=1<<3;

 #undef  NOGDICAPMASKS   //  - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
 #undef  NOVIRTUALKEYCODES //- VK_*
 #undef  NOWINMESSAGES     //- WM_*, EM_*, LB_*, CB_*
 #undef  NOWINSTYLES       //- WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
 #undef  NOSYSMETRICS      //- SM_*
 #undef  NOMENUS           //- MF_*
 #undef  NOICONS           //- IDI_*
 #undef  NOKEYSTATES       //- MK_*
 #undef  NOSYSCOMMANDS     //- SC_*
 #undef  NORASTEROPS       //- Binary and Tertiary raster ops
 #undef  NOSHOWWINDOW      //- SW_*
 #undef  OEMRESOURCE       //- OEM Resource values
 #undef  NOATOM            //- Atom Manager routines
 #undef  NOCLIPBOARD       //- Clipboard routines
 #undef  NOCOLOR           //- Screen colors
 #undef  NOCTLMGR          //- Control and Dialog routines
 #undef  NODRAWTEXT        //- DrawText() and DT_*
 #undef  NOGDI             //- All GDI defines and routines
 #undef  NOKERNEL          //- All KERNEL defines and routines
 #undef  NOUSER            //- All USER defines and routines
 #undef  NONLS             //- All NLS defines and routines
 #undef  NOMB              //- MB_* and MessageBox()
 #undef  NOMEMMGR          //- GMEM_*, LMEM_*, GHND, LHND, associated routines
 #undef  NOMETAFILE        //- typedef METAFILEPICT
 #undef  NOMINMAX          //- Macros min(a,b) and max(a,b)
 #undef  NOMSG             //- typedef MSG and associated routines
 #undef  NOOPENFILE        //- OpenFile(), OemToAnsi, AnsiToOem, and OF_*
 #undef  NOSCROLL          //- SB_* and scrolling routines
 #undef  NOSERVICE         //- All Service Controller routines, SERVICE_ equates, etc.
 #undef  NOSOUND           //- Sound driver routines
 #undef  NOTEXTMETRIC      //- typedef TEXTMETRIC and associated routines
 #undef  NOWH              //- SetWindowsHook and WH_*
 #undef  NOWINOFFSETS      //- GWL_*, GCL_*, associated routines
 #undef  NOCOMM            //- COMM driver routines
 #undef  NOKANJI           //- Kanji support stuff.
 #undef  NOHELP            //- Help engine interface.
 #undef  NOPROFILER        //- Profiler interface.
 #undef  NODEFERWINDOWPOS  //- DeferWindowPos routines
 #undef  NOMCX             //- Modem Configuration Extensions

 #define  NOGDICAPMASKS   //  - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
// #define  NOVIRTUALKEYCODES //- VK_*
// #define  NOWINMESSAGES     //- WM_*, EM_*, LB_*, CB_*
// #define  NOWINSTYLES       //- WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
// #define  NOSYSMETRICS      //- SM_*

 #define  NOMENUS           //- MF_*
 #define  NOICONS           //- IDI_*
 #define  NOKEYSTATES       //- MK_*
 #define  NOSYSCOMMANDS     //- SC_*
 //#define  NORASTEROPS       //- Binary and Tertiary raster ops
// #define  NOSHOWWINDOW      //- SW_*
 #define  OEMRESOURCE       //- OEM Resource values
 #define  NOATOM            //- Atom Manager routines
 #define  NOCLIPBOARD       //- Clipboard routines
// #define  NOCOLOR           //- Screen colors
// #define  NOCTLMGR          //- Control and Dialog routines
// #define  NODRAWTEXT        //- DrawText() and DT_*
// #define  NOGDI             //- All GDI defines and routines
// #define  NOKERNEL          //- All KERNEL defines and routines
// #define  NOUSER            //- All USER defines and routines
 #define  NONLS             //- All NLS defines and routines
// #define  NOMB              //- MB_* and MessageBox()
 #define  NOMEMMGR          //- GMEM_*, LMEM_*, GHND, LHND, associated routines
 #define  NOMETAFILE        //- typedef METAFILEPICT
 #define  NOMINMAX          //- Macros min(a,b) and max(a,b)
// #define  NOMSG             //- typedef MSG and associated routines
 #define  NOOPENFILE        //- OpenFile(), OemToAnsi, AnsiToOem, and OF_*
 #define  NOSCROLL          //- SB_* and scrolling routines
 #define  NOSERVICE         //- All Service Controller routines, SERVICE_ equates, etc.
 #define  NOSOUND           //- Sound driver routines
// #define  NOTEXTMETRIC      //- typedef TEXTMETRIC and associated routines
 #define  NOWH              //- SetWindowsHook and WH_*
// #define  NOWINOFFSETS      //- GWL_*, GCL_*, associated routines
// #define  NOCOMM            //- COMM driver routines
 #define  NOKANJI           //- Kanji support stuff.
 #define  NOHELP            //- Help engine interface.
 #define  NOPROFILER        //- Profiler interface.
 #define  NODEFERWINDOWPOS  //- DeferWindowPos routines
 #define  NOMCX             //- Modem Configuration Extensions

#undef DIRECTINPUT_VERSION
#undef DIRECTSOUND_VERSION
#undef DIRECTDRAW_VERSION
#ifndef DX3
#define DIRECTINPUT_VERSION 0x0500
#define DIRECTSOUND_VERSION 0x0500
#define DIRECTDRAW_VERSION 0x0500
#else
#define DIRECTINPUT_VERSION 0x0300
#define DIRECTSOUND_VERSION 0x0300
#define DIRECTDRAW_VERSION 0x0300
#endif

#include <winsock2.h>
#include <windows.h>
#include <cstdio>
#include <cmath>
#include <cfloat>
#include <mmsystem.h>
#include <dplay.h>
#include <fcntl.h>
#include <direct.h>

#include "resource.h"
#include "lstrings.h"
#include "CSon/CSon.h"

#define While(c) while((c)&&((GetAsyncKeyState(VK_RETURN)&0x80000000)==0))

struct TEXTMESSAGE
{
	BOOL	Valide;
	BOOL	Shown;
	DWORD	FirstTime;
	char	Text[BMAX_PATH];;
	DWORD	Delay;
	DWORD	Color;
};

struct BomberScore
{
	BOOL bSet;
	char Nom[BMAX_NAME];
	DWORD Frag;
	DWORD AK;
	DWORD Parties;
};

struct RegScore
{
	char Nom[BMAX_NAME];
	DWORD Frag;
	DWORD AK;
	DWORD Parties;
};

struct EncRegScore
{
	struct RegScore RS;
	BYTE cksum;
};

struct DPlayPlayer
{
	DPID dpid;
	DPNAME dpName;
	int	Num;
};

struct	BomberUpdate
{
	WORD	X;
	WORD	Y;
	BYTE	EtatSens;
	BYTE	Spritedxdy;
};//Taille : 6

struct	Blast
{
	BYTE	X;
	BYTE	Y;
	BYTE	NSpr;
	BYTE	Etat;
	DWORD	LastGTC;
	DWORD	TOut;
	BOOL	BombBlast;
	BYTE	Type;
};//Taille : 17

struct Bomb
{
	BYTE	X;
	BYTE	Y;
	BYTE	NSpr;
	BYTE	BSpr;
	DWORD	LastGTC;
	WORD	TOut;
	BYTE	Mode;
	BYTE	Etat;
	BYTE	Blast;
	BYTE	Sens;
	BYTE	MustExpl;
};//Taille : 15

struct	BlocUpdate
{
	BYTE	TypeBType;
};//Taille : 1

struct	Bloc
{
	BYTE	Type;
	BYTE	TypeBonus;
	BYTE	ExplLevel;
	BYTE	TypeSol;
	BYTE	Bloque;
};//Taille : 5

struct	Bomber
{
	float	X;
	float	Y;

	BYTE	Etat;
	BYTE	SimulBomb;
	BYTE	NbBomb;
	BYTE	Sprite;

	BYTE	Sens;
	BYTE	Blast;
	BYTE	NbMine;
	BYTE	NbRet;

	DWORD	RevGTC;
	DWORD	TpsGTC;
	DWORD	LastGTC1;

	WORD	Frag;
	WORD	AK;
	DWORD	Parties;

	BYTE	CmdCtrl;
	BYTE	Team;

	BYTE	BType;
	BYTE	RetOpt;
	BYTE	RigOpt;
	char	Reverse;
	char	Nom[BMAX_NAME];

	BYTE	SndExplose;
	BYTE	SndBonus;
	BYTE	SndVache;
	BYTE	SndMort;
	BYTE	SndAK;
	BYTE	SndTeleport;

	short	dx;
	short	dy;
	BYTE	Color;
	DWORD	NetGTC;
	DWORD	StartTime;

	int		oldSensX;
	int		oldSensY;
};//Taille : 48

struct ControlBomber
{
	BYTE	Control;
	BYTE Connected;
	DPID	dpid;
	DWORD	cl_id;
};

struct GameOptions
{
	BYTE BonusRepart[NUM_BONUS+1];
	BYTE StartPos[2*MAX_PLAYERS];
	BYTE NBBOMB;
	BYTE NBBLAST;
	BYTE BLOC_COND;
	BYTE SIMULBOMB;
	BYTE NBMINE;
	BYTE MORTPNT;
	BYTE AKPNT;
	BOOL RETNUCL;
	BOOL DISPRETNUCL;
	BOOL CANREV;
	DWORD REV_TOUT;
	DWORD MCH_TOUT;
	DWORD SDDEATH_TOUT;
	DWORD BOTLEVEL;
	BOOL TEAMPLAY;
	BYTE TEAM1;
	BYTE TEAM2;
	BYTE TEAM3;
	BYTE TEAM4;
};

struct Controls
{
	BYTE Key_UP[4];
	BYTE Key_DOWN[4];
	BYTE Key_LEFT[4];
	BYTE Key_RIGHT[4];
	BYTE Key_FIRE1[4];
	BYTE Key_FIRE2[4];
	BYTE Key_FIRE3[4];
	BYTE DIKey_UP[4];
	BYTE DIKey_DOWN[4];
	BYTE DIKey_LEFT[4];
	BYTE DIKey_RIGHT[4];
	BYTE DIKey_FIRE1[4];
	BYTE DIKey_FIRE2[4];
	BYTE DIKey_FIRE3[4];
};

struct TextMessages
{
	char F1TxtMsg[BMAX_PATH];
	char F2TxtMsg[BMAX_PATH];
	char F3TxtMsg[BMAX_PATH];
	char F4TxtMsg[BMAX_PATH];
	char F5TxtMsg[BMAX_PATH];
	char F6TxtMsg[BMAX_PATH];
	char F7TxtMsg[BMAX_PATH];
	char F8TxtMsg[BMAX_PATH];
};

struct SETTINGS
{
	char szConfigName[BMAX_PATH];
	char Nom[MAX_PLAYERS][BMAX_NAME];
	BOOL IsIA[MAX_PLAYERS];
	char szSelSkin[BMAX_PATH];
	char szSelMap[BMAX_PATH];
	BOOL ALLOWSPECTATORS;
	BOOL DBGMODE;
	BOOL USEDPPROTO;
	BOOL MIGRATEHOST;
	BOOL USEDINPUT;
	BOOL USEFLIP;
	BOOL USETBUFFER;
	BOOL DEBUG;
	BOOL DUPSBUFF;
	BOOL SPRVIDEO;
	BOOL ASYNCENUM;
	BOOL ENUMEVERY;
	int Depth;
	DWORD TEVERY;
	DWORD NET_TOUT;
	BOOL ENABLESON;
	BOOL HISOUND;
	BOOL VSYNC;
	BOOL SHOWPING;
	BOOL TELSVR;
	BOOL TELCLNT;
	BOOL LOADSKIN;
	BOOL SAVESCORES;
	BOOL BOMBTCOLOR;
	BOOL PLAYERTCOLOR;
	BOOL VOICE;
	DWORD SERVER_TYPE;
	DWORD FILE_BLOCK_SIZE;
	DWORD MAPDELAY;
	int Volume;
	int Brightness;
	int Contrast;
	struct TextMessages TM;
	struct Controls	C;
	struct GameOptions GO;
	BYTE cksum;
};

struct RemotePlayerData
{
	int Me;
	int Frag;
	int AK;
	DWORD cl_id;
	BYTE Team;
};

struct SndGlobals
{
	int SndBeep;
};

struct PipeUpdate
{
	DWORD dwSize;
	char szSessionName[BMAX_NAME];
	DWORD NumPlayers;
	char Nom[MAX_PLAYERS][BMAX_NAME];
	DWORD Frag[MAX_PLAYERS];
	DWORD AK[MAX_PLAYERS];
	DWORD Score[MAX_PLAYERS];
};

#endif
