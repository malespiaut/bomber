#ifndef _MULTI_H_
#define _MULTI_H_
const int PING_EVERY=5000;

const int FROM_FIRSTLOCALPLAYER=-1;
const int FROM_ALLLOCALPLAYERS=-2;
const int FROM_MULTICAST=-3;

const int MSG_UPDATE=1;
const int MSG_BLOC=2;
const int MSG_ALLBLOCS=3;
const int MSG_CHAT=4;
const int MSG_GAMEOPTIONS=5;

const int MSG_EXPBOMB=10;
const int MSG_GETFRAG=11;
const int MSG_GETAK=12;
const int MSG_NEWBOMB=13;
//const int MSG_BLOCTYPE=14;
const int MSG_ERASEBONUS=14;
const int MSG_SNDFORME=15;

const int MSG_BN_NUCL=20;
const int MSG_BN_LNG=21;
const int MSG_BN_CRT=22;
const int MSG_BN_REVERSE=23;
const int MSG_BN_ALLEXPL=24;
const int MSG_BN_TELEPORT=25;

const int MSG_PLAY=30;
const int MSG_RESET=31;
const int MSG_RESETREPLY=32;
const int MSG_QUIT=33;
const int MSG_QUIT_REPLY=34;
const int MSG_RESETREADY=35;
const int MSG_SYNC=36;
const int MSG_HOSTQUIT=37;

const int MSG_PING=40;
const int MSG_PINGREPLY=41;

const int MSG_CREATEDIR=60;
const int MSG_FILENAME=61;
const int MSG_FILESENT=62;
const int MSG_FILEREQUEST=63;
const int MSG_FILEOK=64;
const int MSG_FILEWAIT=65;
const int MSG_STARTFILE=66;
const int MSG_FILEBLOCK=67;
const int MSG_ENDFILE=68;

const int MSG_LOADSKIN=70;
const int MSG_SDDEATH=75;

const int MSG_DOMULTICAST=80;
const int MSG_MULTICASTID=81;
const int MSG_MULTICASTMSG=82;

const int MSG_OTHERGROUP=90;

const int MSG_VOICE=100;

const int MSG_SYSDPLAY=127;

const int NUM_MULTICAST=-2; //Joueur Multicast

const int MAX_SENDMSG=8;
const int AVERAGE_SENDMSG=3;
const int NST_MAX_BIAS=100;
const int NST_DEC_BIAS=10;

const DWORD DP_BUFSIZE=8192;

int FindLocalPlayer(void);
BOOL IsLocalPlayer(int);
void GereSysMsg(void);
void InitMulti(void);
int DPIDtoIDX(DPID);
int allDPIDtoIDX(DPID);
void ChatMessage(int,int,char*,BOOL);
void SendGameMessage(int,int,int,int,int,int,int);
int ReceiveGameMessage(int,int); 
void Send(int,int,LPVOID,DWORD);
void SendID(DPID,DPID,LPVOID,DWORD);
void UpdateRemoteData(int);
void GereSynchro(void);
int FlushReceiveQueue(void);
void FlushSysMessageQueue(void);
//DWORD GetReceiveCount(void);
BOOL IsCriticalMessage(int);
void InitMulti(void);
void CloseMulti(void);
void UpdateSessionDesc(void);
void CheckNST(DWORD,DWORD);
void UpdateOtherGroup(void);
#endif
