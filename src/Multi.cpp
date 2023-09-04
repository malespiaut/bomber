#include "General.h"
#include "Bomber.h"
#include "Multi.h"
#include "Debug.h"
#include "Son.h"
#include "VoiceStack.h"
#include "WSDPlay.h"

#ifdef GCOMP_LZO
#include <lzo1x.h>
#elif defined(GCOMP_UCL)
#include <ucl/ucl.h>
#elif defined(GCOMP_BZ)
#include <bzlib.h>
#endif

#define	TRAITERCV(op) {if((idTo!=MulticastID)&&(!bDedicated||(idTo!=SpectatorID))&&(To!=FindLocalPlayer())) {M=-1;op}}

#ifdef WSDP
extern class WSDPlay* wsDP;
#define lpDP wsDP
#else
#ifndef DX3
extern	LPDIRECTPLAY4A	lpDP;
#else
extern	LPDIRECTPLAY3A	lpDP;
#endif
#endif

extern	HWND	hWndWin;
extern	struct	Blast	Blasts[MAX_PLAYERS][TOT_BLAST];
extern	int		NumBlasts[MAX_PLAYERS];
extern	struct	Bomber	Bombers[MAX_PLAYERS];
extern	struct	Bloc	Blocs[20][15];
extern	struct	Bomb	Bombs[MAX_PLAYERS][TOT_BOMB];
extern	int		NumBombs[MAX_PLAYERS];

extern	struct	ControlBomber	BCtrl[MAX_PLAYERS];
extern	struct	SndGlobals		Sons;
extern	struct	SETTINGS		Settings;
extern	BOOL	IsHost;
extern	int NumPlayers;
extern	char*	szTitle;
extern	char	szWorkingDirectory[BMAX_PATH];
extern	DPID	SpectatorID;
extern	DPID	MulticastID;
extern	BOOL	bConnected,bReset,bSpectator,bMustReset;
extern	BOOL	bResetReady[MAX_PLAYERS];
extern	BOOL	bScoreRedraw;
extern	BOOL	bShowPing;
extern	BOOL	bDedicated;
extern	int		bUpdateAllScreen;
extern	int		bUpdateCase[20][15];
extern	BOOL	bSudden;
extern	DWORD	xSudden;
extern	char	szSessionName[BMAX_NAME];
extern	DWORD	LastNPCheck;
extern	DWORD	NetSendTime;
extern	DWORD	cl_id;

DWORD cl_ids[MAX_PLAYERS];

DPID MulticastGrp[MAX_PLAYERS];
DPID OtherGrp;
BOOL OneOtherPlayer;
DWORD NbSends,Overruns;

struct
{
	BOOL bActive;
	DWORD TckBias;
} Synchro;

DWORD	dwLatency[MAX_PLAYERS],dwPing;
DWORD	LatencyLastUpdate;
DWORD	FILE_BLOCK_SIZE;

char szRcvFile[BMAX_PATH];
char* Buffer;
char DPBuffer[DP_BUFSIZE];
BOOL Gar,gTimeOut,gPrio,gAsync,nTimeOut,nPrio,nAsync;
BOOL bGarNeeded;
DWORD dwTimeOut,dwPriority;
int t1,t2;
BYTE b1;

int DPIDtoIDX(DPID dpId)
{
	int dpidx=0;
	for(dpidx=0;dpidx<NumPlayers;dpidx++)
	{
		if((BCtrl[dpidx].dpid==dpId)&&BCtrl[dpidx].Connected) 
			return dpidx;
	}
	return -1;
}

int allDPIDtoIDX(DPID dpId)
{
	int dpidx=0;
	for(dpidx=0;dpidx<NumPlayers;dpidx++)
	{
		if(BCtrl[dpidx].dpid==dpId)
			return dpidx;
	}
	return -1;
}

void CloseMulti(void)
{
	if(!bConnected) return;
	if(lpDP==NULL) return;

	if(IsHost)
		SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_HOSTQUIT,0,0,0,0);

	if(OtherGrp)
	{
		lpDP->DestroyGroup(OtherGrp);
		OtherGrp=0;
	}
/*
	if(bDedicated||bSpectator)
		lpDP->DestroyPlayer(SpectatorID);

	for(int i=0;i<NumPlayers;i++)
		if(IsLocalPlayer(i))
		{
			lpDP->DestroyPlayer(BCtrl[i].dpid);
			Temporize();
		}
*/
	return;
}

void InitMulti(void)
{
	DPCAPS	gar,nongar;
	if(lpDP==0) return;
	if(!bConnected) return;

	NbSends=Overruns=0;
	for(int gidx=0;gidx<MAX_PLAYERS;gidx++)
		MulticastGrp[gidx]=0;
	OtherGrp=0;
	nongar.dwSize=gar.dwSize=sizeof(DPCAPS);
	lpDP->GetCaps(&nongar,0);
	lpDP->GetCaps(&gar,DPGETCAPS_GUARANTEED);

	Gar=gTimeOut=gPrio=gAsync=0;
	nTimeOut=nPrio=nAsync=0;

	if(gar.dwFlags&(DPCAPS_GUARANTEEDSUPPORTED|DPCAPS_GUARANTEEDOPTIMIZED))
		Gar=TRUE;
	if(gar.dwFlags&DPCAPS_SENDTIMEOUTSUPPORTED)
		gTimeOut=TRUE;
	if(gar.dwFlags&DPCAPS_SENDPRIORITYSUPPORTED)
		gPrio=TRUE;
	if(gar.dwFlags&DPCAPS_ASYNCSUPPORTED)
		gAsync=TRUE;
	dwPing=gar.dwLatency;
	for(int ldx=0;ldx<MAX_PLAYERS;ldx++)
		dwLatency[ldx]=gar.dwLatency;

	if(nongar.dwFlags&(DPCAPS_GUARANTEEDSUPPORTED|DPCAPS_GUARANTEEDOPTIMIZED)) 
		Gar=TRUE;
	if(nongar.dwFlags&DPCAPS_SENDTIMEOUTSUPPORTED)
		nTimeOut=TRUE;
	if(nongar.dwFlags&DPCAPS_SENDPRIORITYSUPPORTED)
		nPrio=TRUE;
	if(nongar.dwFlags&DPCAPS_ASYNCSUPPORTED)
		nAsync=TRUE;

	dwTimeOut=nongar.dwTimeout;
	return;
}
void UpdateSessionDesc(void)
{
	if(lpDP==NULL)
		return;
	DPSESSIONDESC2 sessionDesc;
	sessionDesc.dwSize=sizeof(DPSESSIONDESC2);
	lpDP->GetSessionDesc(0,&sessionDesc.dwSize); 
	DWORD* sDesc=new DWORD[(sessionDesc.dwSize+3)/4];
	sDesc[0]=sessionDesc.dwSize;
	lpDP->GetSessionDesc(sDesc,&sessionDesc.dwSize);
	sessionDesc=*(DPSESSIONDESC2*)sDesc;
	delete[] sDesc;
	sessionDesc.dwUser4=GetRealNumPlayers();
	sessionDesc.dwSize=sizeof(DPSESSIONDESC2);
	sessionDesc.lpszSessionNameA=szSessionName;
	lpDP->SetSessionDesc(&sessionDesc,0);
	return;
}

void GereSysMsg()
{
	DPSESSIONDESC2 sessionDesc;
	int ct,ct1;
	char tcb[BMAX_PATH];
	int idx;
	DPMSG_GENERIC *dpMsg;
	dpMsg=(DPMSG_GENERIC*)Buffer;

	switch(dpMsg->dwType)
	{
	case DPSYS_CHAT:
		DPMSG_CHAT* dpChat;
		dpChat=(DPMSG_CHAT*)(dpMsg);
		if(allDPIDtoIDX(dpChat->idFromPlayer)==-1) break;
		if(DPIDtoIDX(dpChat->idFromPlayer)==-1) break;
		if(IsLocalPlayer(allDPIDtoIDX(dpChat->idFromPlayer))) break;
		sprintf(tcb,"%s: %s",Bombers[DPIDtoIDX(dpChat->idFromPlayer)].Nom,dpChat->lpChat->lpszMessageA);
		AddMessage(tcb,2500,GetBomberColor(DPIDtoIDX(dpChat->idFromPlayer)));
		break;

	case DPSYS_SETSESSIONDESC:
		if(!IsHost)
			OutFile(LSZ_DP_SDESCCHANGED);
		break;

	case DPSYS_HOST:
		if(bSpectator&&!bDedicated)
		{
			if(GetRealNumPlayers()==0)
				ErrorMsg(LSZ_DP_ALLPLOUT,LSZ_DP_MPMODE);
			ShowNow(LSZ_DP_HOSTQUIT);

			int hr=lpDP->GetSessionDesc(0,&sessionDesc.dwSize); 
			DWORD* sDesc=new DWORD[sessionDesc.dwSize];
			sDesc[0]=sessionDesc.dwSize;
			hr=lpDP->GetSessionDesc(sDesc,&sessionDesc.dwSize);
			sessionDesc=*(DPSESSIONDESC2*)sDesc;
			SAFE_DELETE_ARRAY(sDesc);

			if(hr!=DP_OK)
				ErrorMsg(LSZ_DP_CANTRETINFO,LSZ_DP_MPMODE);

			if((sessionDesc.dwUser3&BS_PLAYING)==0)
			{
				hr=DialogBox(0,(LPCSTR)IDD_WAITSESSION,NULL,(DLGPROC)DlgProcWaitSession);
				if(hr!=TRUE)
					ErrorMsg(LSZ_DP_FREEZEDSESSION,LSZ_DP_MPMODE);
			}

			/*
			lpDP->GetPlayerName(SpectatorID,0,&dpName.dwSize);
			BYTE* dpN=new BYTE[dpName.dwSize];
			ct=lpDP->GetPlayerName(SpectatorID,dpN,&dpName.dwSize);
			if(ct!=DP_OK)
				ErrorMsg("Impossible d'obtenir les informations sur votre joueur","Mode Multijoueurs");
			dpName=*((DPNAME*)dpN);
			delete[] dpN;
			*/
			ct=lpDP->DestroyPlayer(SpectatorID);
			if(ct!=DP_OK)
				ErrorMsg(LSZ_DP_CANTDELPLAYER,LSZ_DP_MPMODE);
			ct=lpDP->CreatePlayer(&SpectatorID, 0,0, 0,0,DPPLAYER_SPECTATOR);
			if(ct!=DP_OK)
				ErrorMsg(LSZ_DP_CANTRECREATEPL,LSZ_DP_INIT);
			ShowNow(LSZ_DP_SUCCESSRECON);
		}
		else
		{
			IsHost=TRUE;
			UpdateSessionDesc();
			AddMessage(LSZ_DP_NEWHOST,2500,RGB(0,0,240));
			OutFile(LSZ_DP_NEWHOST);
		}
		break;

	case DPSYS_SETPLAYERORGROUPDATA:
		DPMSG_SETPLAYERORGROUPDATA* dpSetData;
		dpSetData=(DPMSG_SETPLAYERORGROUPDATA*)(dpMsg);
		if(dpSetData->dwPlayerType!=DPPLAYERTYPE_PLAYER) break;
		if(allDPIDtoIDX(dpSetData->dpId)==-1) break;
		if(DPIDtoIDX(dpSetData->dpId)==-1) break;
		if(IsLocalPlayer(allDPIDtoIDX(dpSetData->dpId))) break;
		int v;
		struct RemotePlayerData* RD;
		v=DPIDtoIDX(dpSetData->dpId);
		if(v==-1) {OutFile(LSZ_DP_CANTFINDPL);break;}
		if(IsLocalPlayer(v)) break;
		RD=(struct RemotePlayerData*)dpSetData->lpData;
		Bombers[v].Frag=RD->Frag;
		Bombers[v].AK=RD->AK;
		Bombers[v].Team=RD->Team;
		BCtrl[v].cl_id=RD->cl_id;
		bScoreRedraw=TRUE;
		break;

	case DPSYS_CREATEPLAYERORGROUP:
		DPMSG_CREATEPLAYERORGROUP* dpCreate;
		dpCreate=(DPMSG_CREATEPLAYERORGROUP*)(dpMsg);

		if(dpCreate->dwPlayerType!=DPPLAYERTYPE_PLAYER) break;
		if((dpCreate->dwFlags&DPPLAYER_LOCAL)!=0) break;
		Synchro.bActive=0;
		Synchro.TckBias=0;
		if(OtherGrp!=0)
		{
			lpDP->DestroyGroup(OtherGrp);
			OtherGrp=0;
		}
		if((dpCreate->dwFlags&DPPLAYER_SPECTATOR)==0)
		{
			if(IsHost)
			{
				UpdateSessionDesc();
				LastNPCheck=TickCount()+NPCHECK_TOUT;
			}

			int plnum=((struct RemotePlayerData*)(dpCreate->lpData))->Me;
			if(plnum!=NumPlayers)
				OutFileEx2("Numéro de joueur %ld au lieu de %ld !",NumPlayers,plnum);

			BCtrl[NumPlayers].Control=CTRL_NET;
			BCtrl[NumPlayers].Connected=B_PLAYING;
			BCtrl[NumPlayers].dpid=dpCreate->dpId;
			for(int ipla0=0;ipla0<TOT_BOMB;ipla0++)
				Bombs[NumPlayers][ipla0].Etat=B_DISABLED;
			for(int ipla1=0;ipla1<TOT_BLAST;ipla1++)
				Blasts[NumPlayers][ipla1].Etat=B_DISABLED;

			Bombers[NumPlayers].Etat=B_PLAYING;
			Bombers[NumPlayers].StartTime=TickCount();
			Bombers[NumPlayers].Team=((struct RemotePlayerData*)(dpCreate->lpData))->Team;
			BCtrl[NumPlayers].cl_id=((struct RemotePlayerData*)(dpCreate->lpData))->cl_id;

			if(Bombers[NumPlayers].Team!=VOID_TEAM)
				Settings.GO.TEAMPLAY=TRUE;

			strcpy(Bombers[NumPlayers].Nom,dpCreate->dpnName.lpszShortNameA);
			sprintf(tcb,LSZ_DP_NEWPLAYERd,NumPlayers);
			OutFile(tcb);

			if(IsHost)
			{
				bResetReady[NumPlayers]=0;
				bMustReset=TRUE;
			}

			sprintf(tcb,LSZ_DP_sENTERED,Bombers[NumPlayers].Nom);
			AddMessage(tcb,2500,GetBomberColor(NumPlayers));
			NumPlayers++;
			UpdateBombCount();
			UpdateBlastCount();
		}
		else
			if(!IsHost||Settings.ALLOWSPECTATORS)
				AddMessage(LSZ_DP_SPECTENTER,2500,RGB(0,0,240));
			else
				lpDP->DestroyPlayer(dpCreate->dpId);

		AllocScoreSurface();
		bScoreRedraw=TRUE;
		UpdateOtherGroup();
		if(IsHost&&(MulticastID!=0))
			SendGameMessage(FROM_MULTICAST,-1,MSG_MULTICASTID,(int)MulticastID,0,0,0);
		break;

	case DPSYS_DESTROYPLAYERORGROUP:
		DPMSG_DESTROYPLAYERORGROUP* dpDestroy;
		dpDestroy=(DPMSG_DESTROYPLAYERORGROUP*)(dpMsg);

		if(dpDestroy->dwPlayerType==DPPLAYERTYPE_GROUP)
		{
			for(int gidx=0;gidx<MAX_PLAYERS;gidx++)
				if(dpDestroy->dpId==MulticastGrp[gidx])
					MulticastGrp[gidx]=0;
		}
		if(dpDestroy->dwPlayerType!=DPPLAYERTYPE_PLAYER)
			break;
		if((dpDestroy->dwFlags&DPPLAYER_LOCAL)!=0) break;
		idx=DPIDtoIDX(dpDestroy->dpId);
		if((dpDestroy->dwFlags&DPPLAYER_SPECTATOR)==0)
		{
			if(idx==-1) break;
			AddPlayerToScores(idx);
			if(Settings.GO.TEAMPLAY)
				if(Bombers[idx].Team!=VOID_TEAM)
					if(GetTeamPop(Bombers[idx].Team)>0)
						AddTeamToScores(Bombers[idx].Team);

			sprintf(tcb,LSZ_DP_sGOESWdANDd,Bombers[idx].Nom,Bombers[idx].Frag,Bombers[idx].AK);
			AddMessage(tcb,2500,GetBomberColor(idx));
			Synchro.bActive=0;
			LatencyLastUpdate=0;

			NumPlayers--;
			for(ct=idx+1;ct<MAX_PLAYERS;ct++)
			{
				for(ct1=0;ct1<TOT_BOMB;ct1++)
					Bombs[ct-1][ct1]=Bombs[ct][ct1];

				for(ct1=0;ct1<TOT_BLAST;ct1++)
					Blasts[ct-1][ct1]=Blasts[ct][ct1];

				NumBombs[ct-1]=NumBombs[ct];
				NumBlasts[ct-1]=NumBlasts[ct];
				Bombers[ct-1]=Bombers[ct];
				Bombers[ct-1].Color=ct-1;
				BCtrl[ct-1]=BCtrl[ct];
				bResetReady[ct-1]=bResetReady[ct];
				MulticastGrp[ct-1]=MulticastGrp[ct];
			}
			MulticastGrp[MAX_PLAYERS-1]=0;
			bResetReady[MAX_PLAYERS-1]=TRUE;
			BCtrl[MAX_PLAYERS-1].Connected=B_DISABLED;
			Bombers[MAX_PLAYERS-1].Etat=B_DISABLED;

			Settings.GO.TEAMPLAY=FALSE;
			for(int pltidx=0;pltidx<NumPlayers;pltidx++)
				if(Bombers[pltidx].Team!=VOID_TEAM)
					Settings.GO.TEAMPLAY=TRUE;

			if(!bSpectator)
				for(ct=0;ct<NumPlayers;ct++)
					if(IsLocalPlayer(ct))
						UpdateRemoteData(ct);

			if(IsHost)
			{
				lpDP->GetSessionDesc(0,&sessionDesc.dwSize); 
				DWORD* sDesc=new DWORD[(sessionDesc.dwSize+3)/4];
				sDesc[0]=sessionDesc.dwSize;
				lpDP->GetSessionDesc(sDesc,&sessionDesc.dwSize);
				sessionDesc=*(DPSESSIONDESC2*)sDesc;
				delete[] sDesc;
				sessionDesc.dwUser4--;
				DWORD np=GetRealNumPlayers();
				/*
				if(sessionDesc.dwUser4!=np)
				{
					OutFile("Pas le même nombre de joueurs entre dwUser4 et GetRealNumPlayers");
					sessionDesc.dwUser4=np;
				}
				*/
				sessionDesc.dwSize=sizeof(DPSESSIONDESC2);
				sessionDesc.lpszSessionNameA=szSessionName;
				lpDP->SetSessionDesc(&sessionDesc,0);
			}
		}
		else
			AddMessage(LSZ_DP_SPECTATORQUIT,2500,RGB(0,0,240));
		AllocScoreSurface();
		break;

	case DPSYS_SESSIONLOST:
		//AddMessage(LSZ_DP_EJECTED,2000,RGB(0,0,240));
		ErrorMsg(LSZ_DP_EJECTED,LSZ_DP_MPMODE);
		break;

	case DPSYS_DELETEPLAYERFROMGROUP:
		break;

	case DPSYS_SETGROUPOWNER:
		break;

	default:
		break;
	}
	return;
}

void	SendGameMessage(int MeNum,int Num,int Msg,int p1,int p2,int p3,int p4)
{
	struct	BomberUpdate*	lpBUpd;
	struct	BlocUpdate*		lpBlUpd;
	struct	BlocUpdate		BU[20][15];

	if(!bConnected) return;
	if(lpDP==0) return;
	if(bSpectator&&!bDedicated) return;
	if(NumPlayers==0)
		return;

	Buffer=DPBuffer+128;
	bGarNeeded=TRUE;
	dwPriority=0;
	switch(Msg)
	{
	case	MSG_OTHERGROUP:
		Buffer[0]=MSG_OTHERGROUP;
		*(DPID*)&Buffer[1]=(DPID)p1;
		Send(MeNum,Num,Buffer,1+sizeof(DPID));
		OutFile("OtherGroupID sent");
		break;

	case	MSG_MULTICASTID:
		Buffer[0]=MSG_MULTICASTID;
		*(DPID*)&Buffer[1]=(DPID)p1;
		Send(MeNum,Num,Buffer,1+sizeof(DPID));
		OutFile("MulticastID sent");
		break;

	case	MSG_SDDEATH:
		Buffer[0]=MSG_SDDEATH;
		Send(MeNum,Num,Buffer,1);
		OutFile("StartSudden sent");
		break;

	case	MSG_RESETREADY:
		Buffer[0]=MSG_RESETREADY;
		Send(MeNum,Num,Buffer,1);
		OutFile("ResetReady sent");
		break;

	case	MSG_SYNC:
		Buffer[0]=MSG_SYNC;
		Send(MeNum,Num,Buffer,1);
		OutFile("Sync sent");
		break;

	case	MSG_GAMEOPTIONS:
		Buffer[0]=MSG_GAMEOPTIONS;
		memcpy(&Buffer[1],(void*)p1,sizeof(struct GameOptions));
		Send(MeNum,Num,Buffer,1+sizeof(struct GameOptions));
		OutFile("GameOptions sent");
		break;

	case	MSG_STARTFILE:
		Buffer[0]=MSG_STARTFILE;
		memcpy(&Buffer[1],&p1,4);
		strcpy(&Buffer[5],(const char*)p2);
		Send(MeNum,Num,Buffer,6+strlen((const char*)p2));
		OutFile("StartFile sent");
		break;

	case	MSG_FILEBLOCK:
		Buffer[0]=MSG_FILEBLOCK;
		memcpy(&Buffer[1],(void*)p1,Settings.FILE_BLOCK_SIZE);
		Send(MeNum,Num,Buffer,1+Settings.FILE_BLOCK_SIZE);
		OutFile("FileBlock sent");
		break;

	case	MSG_ENDFILE:
		Buffer[0]=MSG_ENDFILE;
		memcpy(&Buffer[1],&p2,4);
		memcpy(&Buffer[5],&p3,4);
		memcpy(&Buffer[9],&p4,4);
		memcpy(&Buffer[13],(void*)p1,p2);
		Send(MeNum,Num,Buffer,13+p2);
		OutFile("EndFile sent");
		break;

	case	MSG_LOADSKIN:
		Buffer[0]=MSG_LOADSKIN;
		strcpy(&Buffer[1],(const char*)p1);
		Send(MeNum,Num,Buffer,2+strlen((const char*)p1));
		OutFile("LoadSkin sent");
		break;

	case	MSG_FILESENT:
		Buffer[0]=MSG_FILESENT;
		Send(MeNum,Num,Buffer,1);
		OutFile("FileSent sent");
		break;

	case	MSG_CREATEDIR:
		Buffer[0]=MSG_CREATEDIR;
		strcpy(&Buffer[1],(const char*)p1);
		Send(MeNum,Num,Buffer,2+strlen((const char*)p1));
		OutFile("CreateDir sent");
		break;

	case	MSG_FILENAME:
		Buffer[0]=MSG_FILENAME;
		*(DWORD*)&Buffer[1]=(DWORD)p1;
		strcpy(&Buffer[5],(const char*)p2);
		Send(MeNum,Num,Buffer,6+strlen((const char*)p2));
		OutFile("FileName sent");
		break;

	case	MSG_FILEOK:
		Buffer[0]=MSG_FILEOK;
		strcpy(&Buffer[1],(const char*)p1);
		Send(MeNum,Num,Buffer,2+strlen((const char*)p1));
		OutFile("FileOK sent");
		break;

	case	MSG_FILEREQUEST:
		Buffer[0]=MSG_FILEREQUEST;
		strcpy(&Buffer[1],(const char*)p1);
		Send(MeNum,Num,Buffer,2+strlen((const char*)p1));
		OutFile("FileRequest sent");
		break;

	case	MSG_FILEWAIT:
		Buffer[0]=MSG_FILEWAIT;
		Send(MeNum,Num,Buffer,1);
		OutFile("FileWait sent");
		break;

/*
	case	MSG_ASKPING:
		Buffer[0]=MSG_ASKPING;
		Send(MeNum,Num,Buffer,1);
		OutFile("AskPing sent");
		break;

	case	MSG_SETPING:
		dwPriority=65535;
		Buffer[0]=MSG_SETPING;
		memcpy(&Buffer[1],&dwLatency[Num],4);
		Send(MeNum,Num,Buffer,5);
		//Synchro.bActive=TRUE;
		OutFile("SetPing sent");
		break;
*/
	case	MSG_PING:
		//if(!IsHost) break;
		bShowPing=0;
		dwPriority=65535;
		Buffer[0]=MSG_PING;
		p1=TickCount();
		*(DWORD*)&Buffer[1]=p1;
		Send(MeNum,Num,Buffer,5);
		Synchro.bActive=0;
		Synchro.TckBias=p1;
		LatencyLastUpdate=TickCount();
		OutFile("Ping sent");
		break;

	case	MSG_PINGREPLY:
		dwPriority=65535;
		Buffer[0]=MSG_PINGREPLY;
		*(DWORD*)&Buffer[1]=p1;
		Send(MeNum,Num,Buffer,5);
		OutFile("PingReply sent");
		break;

	case	MSG_CHAT:
		Buffer[0]=MSG_CHAT;
		strcpy(&Buffer[1],(char*)p1);
		Send(MeNum,Num,Buffer,strlen((char*)p1)+2);
		OutFile("ChatMessage sent");
		AddMessage((char*)p1,2500,RGB(240,0,0));
		break;

	case	MSG_PLAY:
		Buffer[0]=MSG_PLAY;
		Send(MeNum,Num,Buffer,1);
		OutFile("PlayOK sent");
		break;

	case	MSG_EXPBOMB:
		Buffer[0]=MSG_EXPBOMB;
		Buffer[1]=p1;
		Buffer[2]=p2;
		Buffer[3]=p3;
		Send(MeNum,Num,Buffer,4);
		OutFile("ExpBomb sent");
		break;

	case	MSG_RESET:
		Buffer[0]=MSG_RESET;
		Send(MeNum,Num,Buffer,1);
		OutFile("Reset sent");
		break;

	case	MSG_RESETREPLY:
		Buffer[0]=MSG_RESETREPLY;
		Send(MeNum,Num,Buffer,1);
		OutFile("ResetReply sent");
		break;

	case	MSG_BN_TELEPORT:
		Buffer[0]=MSG_BN_TELEPORT;
		Buffer[1]=p1;
		Buffer[2]=p2;
		Send(MeNum,Num,Buffer,3);
		OutFile("Teleport sent");
		break;

	case	MSG_BN_ALLEXPL:
		Buffer[0]=MSG_BN_ALLEXPL;
		Send(MeNum,Num,Buffer,1);
		OutFile("BN_ALLEXPL sent");
		break;

	case	MSG_BN_REVERSE:
		Buffer[0]=MSG_BN_REVERSE;
		if(Synchro.bActive)
		{
			p1-=Synchro.TckBias;
			Buffer[1]=TRUE;
		}
		else
			Buffer[1]=0;

		memcpy(&Buffer[2],&p1,4);
		Send(MeNum,Num,Buffer,6);
		OutFile("BN_REVERSE sent");
		break;

	case	MSG_BN_CRT:
		Buffer[0]=MSG_BN_CRT;
		if(Synchro.bActive)
		{
			p1-=Synchro.TckBias;
			Buffer[1]=TRUE;
		}
		else
			Buffer[1]=0;
		memcpy(&Buffer[2],&p1,4);
		Send(MeNum,Num,Buffer,6);
		OutFile("BN_CRT sent");
		break;

	case	MSG_BN_LNG:
		Buffer[0]=MSG_BN_LNG;
		if(Synchro.bActive)
		{
			p1-=Synchro.TckBias;
			Buffer[1]=TRUE;
		}
		else
			Buffer[1]=0;
		memcpy(&Buffer[2],&p1,4);
		Send(MeNum,Num,Buffer,6);
		OutFile("BN_LNG sent");
		break;

	case	MSG_SNDFORME:
		Buffer[0]=MSG_SNDFORME;
		*(int*)(&Buffer[1])=p1;
		bGarNeeded=0;
		Send(MeNum,Num,Buffer,5);
		OutFile("Son sent");
		break;

	case	MSG_BN_NUCL:
		Buffer[0]=MSG_BN_NUCL;
		Send(MeNum,Num,Buffer,1);
		OutFile("Nucl sent");
		break;
/*
	case	MSG_BLOCTYPE:
		Buffer[0]=MSG_BLOCTYPE;
		Buffer[1]=p1;
		Buffer[2]=p2;
		Buffer[3]=p3;
		Send(MeNum,Num,Buffer,4);
		OutFile("Bloctype sent");
		break;
*/
	case	MSG_ERASEBONUS:
		Buffer[0]=MSG_ERASEBONUS;
		Buffer[1]=p1;
		Buffer[2]=p2;
		Send(MeNum,Num,Buffer,3);
		OutFile("RAZ bonus sent");
		break;

	case	MSG_GETFRAG:
		Buffer[0]=MSG_GETFRAG;
		Buffer[1]=p1;
		Send(MeNum,Num,Buffer,2);
		OutFile("Frag sent");
		break;

	case	MSG_GETAK:
		Buffer[0]=MSG_GETAK;
		Send(MeNum,Num,Buffer,1);
		OutFile("AK sent");
		break;

	case	MSG_NEWBOMB:
		Buffer[0]=MSG_NEWBOMB;
		Buffer[1]=p1;
		Bomb* lpBomb;
		lpBomb=(Bomb*)&Buffer[3];
		*lpBomb=Bombs[MeNum][p1];
		if(Synchro.bActive)
		{
			lpBomb->LastGTC-=Synchro.TckBias;
			Buffer[2]=TRUE;
		}
		else
			Buffer[2]=0;

		Send(MeNum,Num,Buffer,sizeof(struct Bomb)+3);
		//OutFile("Bombe sent");
		break;

	case	MSG_UPDATE:
		Buffer[0]=MSG_UPDATE;
		lpBUpd=(struct	BomberUpdate*)&Buffer[1];
		lpBUpd->X=(WORD)Bombers[p1].X;
		lpBUpd->Y=(WORD)Bombers[p1].Y;
		lpBUpd->EtatSens=(Bombers[p1].Etat<<4)|Bombers[p1].Sens;
		if(p2>0)
			p2=1;
			else
			if(p2<0)
				p2=2;
		if(p3>0)
			p3=1;
			else
			if(p3<0)
				p3=2;
		lpBUpd->Spritedxdy=((BYTE)Bombers[p1].Sprite<<4)|((BYTE)p2<<2)|((BYTE)p3);
 		bGarNeeded=FALSE;
		Send(MeNum,Num,Buffer,sizeof(BomberUpdate)+1);
		break;

	case	MSG_VOICE:
		Buffer[0]=MSG_VOICE;
		*(DWORD*)&Buffer[1]=p1;
		memcpy(&Buffer[5],(void*)p2,p1);
 		bGarNeeded=FALSE;
		Send(MeNum,Num,Buffer,5+p1);
		break;

	case	MSG_QUIT_REPLY:
		Buffer[0]=MSG_QUIT_REPLY;
		Send(MeNum,Num,Buffer,1);
		OutFile("Quit Reply sent");
		break;

	case	MSG_QUIT:
		Buffer[0]=MSG_QUIT;
		Send(MeNum,Num,Buffer,1);
		OutFile("Quit sent");
		break;

	case	MSG_HOSTQUIT:
		Buffer[0]=MSG_HOSTQUIT;
		Send(MeNum,Num,Buffer,1);
		OutFile("HostQuit sent");
		break;

	case	MSG_ALLBLOCS:
		Buffer[0]=MSG_ALLBLOCS;
		for(t1=0;t1<20;t1++)
			for(t2=0;t2<15;t2++)
				BU[t1][t2].TypeBType=(Blocs[t1][t2].TypeBonus<<2)|Blocs[t1][t2].Type;

		memcpy(&Buffer[1],BU,20*15*sizeof(BlocUpdate));
		Send(MeNum,Num,Buffer,1+20*15*sizeof(BlocUpdate));
		break;

	case	MSG_BLOC:
		Buffer[0]=MSG_BLOC;
		Buffer[1]=p1;
		Buffer[2]=p2;
		lpBlUpd=(struct	BlocUpdate*)&Buffer[3];
		//memcpy(lpBloc,&Blocs[p1][p2],sizeof(Bloc));
		lpBlUpd->TypeBType=(Blocs[p1][p2].TypeBonus<<2)|Blocs[p1][p2].Type;
		Send(MeNum,Num,Buffer,sizeof(BlocUpdate)+3);
		wsprintf(Buffer,LSZ_DP_BLdON300,p2+p1*15+1);
		SetWindowText(hWndWin,Buffer);
		OutFile(Buffer);
		break;

	default:
//		OutFile("Send: Message Inconnu !");
		break;
	}
	return;
}

BOOL FAR PASCAL EnumOtherPlayersCallback(DPID dpId,DWORD dwPlayerType,
									 LPCDPNAME lpName,DWORD dwFlags,LPVOID lpContext)
{
	if(dwPlayerType!=DPPLAYERTYPE_PLAYER) 
		return TRUE;
	if((dwFlags&DPENUMPLAYERS_SPECTATOR)==0)
		return TRUE;
	/*
	int plnum=allDPIDtoIDX(dpId);
	DWORD newcl_id=0;

	if(plnum!=-1)
	{
		newcl_id=BCtrl[plnum].cl_id;
		for(int i=0;i<MAX_PLAYERS;i++)
			if(cl_ids[i]==0)
			{
				cl_ids[i]=newcl_id;
				break;
			}
			else
				if(cl_ids[i]==newcl_id)
					return TRUE;
	}
	*/
	if(dpId==MulticastID)
		return TRUE;
	OneOtherPlayer=TRUE;
	lpDP->AddPlayerToGroup(*(DPID*)lpContext,dpId);
	return TRUE;
}

void FlushSysMessageQueue(void)
{
	if((!bConnected)||(lpDP==NULL))
		return;
	int hr;
	DPID idTo,idFrom;
	DWORD sze;
	int To;
	char M; //inutilisé
	while(TRUE)
	{
		idFrom=DPID_SYSMSG;
		sze=DP_BUFSIZE-128;
		Buffer=DPBuffer+128;
		hr=lpDP->Receive(&idFrom,&idTo,DPRECEIVE_FROMPLAYER,Buffer,&sze);
		if(hr!=DP_OK)
			return;
		if(idFrom!=DPID_SYSMSG)
			return;
		To=allDPIDtoIDX(idTo);
		TRAITERCV(continue;)
		GereSysMsg();
	}
	return;
}

int	ReceiveGameMessage(int To,int From)
{
	DWORD CRCval,CRCsrcval;
	unsigned int srcsze;
	unsigned int dstsze;
	BYTE* src;
	BYTE* dst;

	FILE*	F;
	struct	BomberUpdate*	lpBUpd;
	char	M,o1,o2;
	RECT	rct;
	int		Int;
	int		x,y;
	DWORD	sze;
	DPID	idFrom,idTo;
	int		Num;
	struct	BlocUpdate*	lpBlUpd;
	char	szDir[BMAX_PATH];
	DWORD	dwF;
	int hr;

	if(!bConnected) return -1;
	if(lpDP==NULL) return -1;
	_getcwd(szDir,BMAX_PATH);

	if(To!=-1)
		idTo=BCtrl[To].dpid;
	else
		idTo=0;
	idFrom=0;

	sze=DP_BUFSIZE-128;
	Buffer=DPBuffer+128;
	if(From==-1)
	{
		if(To!=-1)
			dwF=DPRECEIVE_TOPLAYER;
		else
			dwF=DPRECEIVE_ALL;
		if((hr=lpDP->Receive(&idFrom,&idTo,dwF,Buffer,&sze))!=DP_OK)
			return -1;

		if (idFrom==DPID_SYSMSG)
		{
			TRAITERCV(return -1;)
			if((bDedicated&&(idTo!=SpectatorID))||((MulticastID!=0)&&IsHost&&(idTo!=MulticastID)))
				return -1;
			GereSysMsg();
			return MSG_SYSDPLAY;
		}

		Num=allDPIDtoIDX(idFrom);
		if(IsLocalPlayer(Num))
			return -1;

		if((Num==-1)&&(idFrom!=MulticastID))
		{
			OutFile(LSZ_DP_UNKNOWN_IDFROM);
//			return -1;
		}
		if(To==-1)
			To=allDPIDtoIDX(idTo);
	}
	else
	{
		if(To!=-1)
			dwF=DPRECEIVE_FROMPLAYER|DPRECEIVE_TOPLAYER;
		else
			dwF=DPRECEIVE_FROMPLAYER;
		if((Bombers[From].Etat!=B_PLAYING)||(BCtrl[From].Control!=CTRL_NET))
			return -1;
		idFrom=BCtrl[From].dpid;

		if((hr=lpDP->Receive(&idFrom,&idTo,dwF,Buffer,&sze))!=DP_OK)
			return -1;

		if (idFrom==DPID_SYSMSG)
		{
			TRAITERCV(return -1;)
			if(bDedicated&&(idTo!=SpectatorID))
				return -1;
			GereSysMsg();
			return MSG_SYSDPLAY;
		}

		if(To==-1)
			To=allDPIDtoIDX(idTo);
		Num=From;
	}

//	if(!bSpectator&&!bDedicated&&(To==-1))
//		OutFile("Joueur cible non trouvé en local");

	if((bDedicated&&(idTo!=SpectatorID))||((MulticastID!=0)&&IsHost&&(idTo!=MulticastID)))
		return -1;

	if((Num==-1)&&!IsHost&&(MulticastID!=0))
		Num=NUM_MULTICAST;

	M=Buffer[0];
	if(M==MSG_MULTICASTMSG)
	{
		TRAITERCV(return -1;)
		Num=allDPIDtoIDX(*(DPID*)&Buffer[1]);
		if(IsLocalPlayer(Num))
			return -1;
		if(*(DPID*)&Buffer[1]==MulticastID)
			Num=NUM_MULTICAST;
		if(Num==-1)
			OutFile("mc message no from");
		Buffer+=1+sizeof(DPID);
		M=Buffer[0];
	}

	if(IsHost)
		if((M==MSG_DOMULTICAST)&&(((*(DPID*)&Buffer[5])==DPID_ALLPLAYERS)||(*(DPID*)&Buffer[5]==MulticastID)))
		{
			DPID MidTo;
			DWORD dwSze;
			if(Buffer[5+sizeof(DPID)]==MSG_UPDATE)
				bGarNeeded=FALSE;
			if(Buffer[5+sizeof(DPID)]==MSG_VOICE)
				bGarNeeded=FALSE;
			MidTo=*(DPID*)&Buffer[5];
			if(MidTo==DPID_ALLPLAYERS)
				if(MulticastGrp[Num]!=0)
					MidTo=MulticastGrp[Num];
			*(DPID*)&Buffer[5]=idFrom;
			dwSze=*(int*)&Buffer[1];
			Buffer[4]=MSG_MULTICASTMSG;
			SendID(MulticastID,MidTo,&Buffer[4],1+sizeof(DPID)+dwSze);
			Buffer+=5+sizeof(DPID);
			M=Buffer[0];
		}

	switch(M)
	{
	case	MSG_DOMULTICAST:
		if(IsHost&&(MulticastID!=0))
		{
			DPID MidTo;
			DWORD dwSze;
			if(Buffer[5+sizeof(DPID)]==MSG_UPDATE)
				bGarNeeded=FALSE;
			if(Buffer[5+sizeof(DPID)]==MSG_VOICE)
				bGarNeeded=FALSE;
			MidTo=*(DPID*)&Buffer[5];
			*(DPID*)&Buffer[5]=idFrom;
			dwSze=*(int*)&Buffer[1];
			Buffer[4]=MSG_MULTICASTMSG;
			SendID(MulticastID,MidTo,&Buffer[4],1+sizeof(DPID)+dwSze);
			OutFile("Multicast recv");
		}
		else
			OutFile("Multicast recu mais pas host mc");
		break;

	case	MSG_OTHERGROUP:
		TRAITERCV(break;)
		if(!IsHost)
			break;
		MulticastGrp[Num]=*(DPID*)&Buffer[1];
		OutFile("OtherGroup ID recv");
		break;

	case	MSG_MULTICASTID:
		TRAITERCV(break;)
		if(IsHost) break;
		MulticastID=*(DPID*)&Buffer[1];
		OutFile("MulticastID recv");
		UpdateOtherGroup();
		break;

	case	MSG_SDDEATH:
		TRAITERCV(break;)
		if(!bSudden)
		{
			PlaySon(0,Sons.SndBeep,0);
			bSudden=TRUE;
			xSudden=0;
			int nb;
			for(nb=0;nb<NumPlayers;nb++)
				for(int bl=0;bl<TOT_BOMB;bl++)
					Bombs[nb][bl].Etat=B_DISABLED;
			UpdateBombCount();

			for(nb=0;nb<NumPlayers;nb++)
				for(int bl=0;bl<TOT_BLAST;bl++)
					Blasts[nb][bl].Etat=B_DISABLED;
			UpdateBlastCount();
		}
		OutFile("StartSudden recv");
		break;

	case	MSG_RESETREADY:
		TRAITERCV(break;)
		bResetReady[Num]=TRUE;
		OutFile("ResetReady recv");
		break;

	case	MSG_SYNC:
		TRAITERCV(break;)
		OutFile("Sync recv");
		break;

	case	MSG_GAMEOPTIONS:
		TRAITERCV(break;)
		Settings.GO=*((struct GameOptions*)&Buffer[1]);
		/*
		Bombers[0].Team=Settings.GO.TEAM1;
		Bombers[1].Team=Settings.GO.TEAM2;
		Bombers[2].Team=Settings.GO.TEAM3;
		Bombers[3].Team=Settings.GO.TEAM4;
		*/
		OutFile("GameOptions recv");
		break;

	case	MSG_STARTFILE:
		TRAITERCV(break;)
		OutFile("StartFile recv");
		if(!Settings.TELCLNT)
			break;
		memcpy(&FILE_BLOCK_SIZE,&Buffer[1],4);
		strcpy(szRcvFile,&Buffer[5]);
		_chdir(szWorkingDirectory);
		remove(szRcvFile);
		break;

	case	MSG_FILEBLOCK:
		TRAITERCV(break;)
		OutFile("FileBlock recv");
		if(!Settings.TELCLNT)
			break;
		_chdir(szWorkingDirectory);
		F=fopen(szRcvFile,"a+b");
		if(F!=0)
		{
			if(FILE_BLOCK_SIZE!=0)
				fwrite(&Buffer[1],FILE_BLOCK_SIZE,1,F);
			fclose(F);
		}
		break;

	case	MSG_ENDFILE:
		TRAITERCV(break;)
		OutFile("EndFile recv");
		if(!Settings.TELCLNT)
			break;
		FILE_BLOCK_SIZE=0;
		_chdir(szWorkingDirectory);
		memcpy(&sze,&Buffer[1],4);
		memcpy(&srcsze,&Buffer[5],4);
		memcpy(&dstsze,&Buffer[9],4);

		F=fopen(szRcvFile,"a+b");
		if((F!=0)&&(sze!=0))
		{
			fwrite(&Buffer[13],sze,1,F);
			fclose(F);
		}
		sze=dstsze;
		src=new BYTE[srcsze];
		dst=new BYTE[dstsze];
		F=fopen(szRcvFile,"rb");
		if(F!=0)
		{
			fread(src,srcsze,1,F);
			fclose(F);
		}

#ifdef GCOMP_LZO
		x=lzo1x_decompress_safe(src,srcsze,dst,&dstsze,0);
#elif defined(GCOMP_UCL)
		x=ucl_nrv2d_decompress_safe_8(src,srcsze,dst,&dstsze,0);
#elif defined(GCOMP_BZ)
		x=bzBuffToBuffDecompress((char*)dst,&dstsze,(char*)src,srcsze,0,0);
#else
#error Pas de compresseur !!
#endif

#ifdef GCOMP_LZO
		if((dstsze==sze)&&(x==LZO_E_OK)&&(F!=0))
#elif defined(GCOMP_UCL)
		if((dstsze==sze)&&(x==UCL_E_OK)&&(F!=0))
#elif defined(GCOMP_BZ)
		if((dstsze==sze)&&(x==BZ_OK)&&(F!=0))
#else
#error Pas de compresseur !!
#endif
		{
			F=fopen(szRcvFile,"wb");
			if(F!=0)
			{
				fwrite(dst,dstsze,1,F);
				fclose(F);
			}
		}

		SAFE_DELETE_ARRAY(dst);
		SAFE_DELETE_ARRAY(src);
		memset(szRcvFile,0,BMAX_PATH);
		OutFile("EndFile ok");
		break;

	case	MSG_LOADSKIN:
		TRAITERCV(break;)
		if(Settings.LOADSKIN)
			LoadSkin((const char*)&Buffer[1]);
		OutFile("LoadSkin recv");
		break;

	case	MSG_FILESENT:
		TRAITERCV(break;)
		OutFile("FileSent recv");
		break;

	case	MSG_CREATEDIR:
		TRAITERCV(break;)
		_chdir(szWorkingDirectory);
		_mkdir(&Buffer[1]);
		OutFile("CreateDir recv");
		break;

	case	MSG_FILENAME:
		TRAITERCV({char TB[BMAX_PATH]; \
					strcpy(TB,&Buffer[5]); \
					SendGameMessage(To,Num,MSG_FILEOK,(int)TB,0,0,0); \
					break;})

		if(!Settings.TELCLNT)
		{
			char TB[BMAX_PATH];
			strcpy(TB,&Buffer[5]);
			SendGameMessage(FROM_FIRSTLOCALPLAYER,Num,MSG_FILEOK,(int)TB,0,0,0);
			break;
		}

		OutFileEx2("Filename %s",&Buffer[5]);
		CRCsrcval=*(DWORD*)&Buffer[1];
		_chdir(szWorkingDirectory);
		F=fopen(&Buffer[5],"rb");

		if(F!=0)
		{
			CRCval=FileCRC(F);
			fclose(F);
		}
		else
			sze=0;

		if((F==0)||(CRCsrcval!=CRCval))
		{
			char TB[BMAX_PATH];
			strcpy(TB,&Buffer[5]);
			SendGameMessage(FROM_FIRSTLOCALPLAYER,Num,MSG_FILEREQUEST,(int)TB,0,0,0);
		}
		else
		{
			char TB[BMAX_PATH];
			strcpy(TB,&Buffer[5]);
			SendGameMessage(FROM_FIRSTLOCALPLAYER,Num,MSG_FILEOK,(int)TB,0,0,0);
		}
		break;
 
	case	MSG_FILEOK:
		TRAITERCV(break;)
		if(!IsHost) break;
		OutFile("FileOK recv");
		break;

	case	MSG_FILEREQUEST:
		TRAITERCV(break;)
		if(!IsHost) break;
		OutFile("FileRequest recv");
		break;

	case	MSG_FILEWAIT:
		TRAITERCV(break;)
		OutFile("FileWait recv");
		break;
/*
	case	MSG_ASKPING:
		TRAITERCV(break;)
		if (!IsHost) break;
		OutFile("AskPing recv");
		SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_PING,0,0,0,0);
		break;


	case	MSG_SETPING:
		TRAITERCV(break;)
		for(Int=0;Int<MAX_PLAYERS;Int++)
			memcpy(&dwLatency[Int],&Buffer[1],4);
		//Synchro.bActive=TRUE;
		//Synchro.TckBias=TickCount()-3*dwLatency[0];
		dwPing=dwLatency[0];
		OutFile("SetPing recv");
		if(bShowPing||Settings.SHOWPING)
		{
			sprintf(Buffer,"Ping %ldms",2*dwLatency[0]);
			AddMessage(Buffer,2500,RGB(0,0,240));
			bShowPing=0;
		}
		break;
*/

	case	MSG_PING:
		TRAITERCV(break;)
		if(bDedicated)
			break;
		Int=*(DWORD*)&Buffer[1];
		Synchro.bActive=0;
		OutFile("Ping recv");
		SendGameMessage(FROM_ALLLOCALPLAYERS,Num,MSG_PINGREPLY,Int,0,0,0);
		break;

	case	MSG_PINGREPLY:
		TRAITERCV(break;)
		Int=*(DWORD*)&Buffer[1];
		OutFile("PingReply recv");
		if(TickCount()-Int<dwTimeOut) 
		{
			dwLatency[Num]=(TickCount()-Int)/2;
			dwPing=dwLatency[Num];
			if(bShowPing||Settings.SHOWPING)
			{
				sprintf(Buffer,"%s: Ping %ldms",Bombers[Num].Nom,2*dwLatency[Num]);
				AddMessage(Buffer,2500,RGB(0,0,240));
			}
//			SendGameMessage(FROM_ALLLOCALPLAYERS,Num,MSG_SETPING,0,0,0,0);
		}
		OutFileEx2("Ping de %s : %ld ms",Bombers[Num].Nom,2*dwPing);
		break;

	case	MSG_CHAT:
		TRAITERCV(break;)
		char TB[BMAX_PATH];
		sprintf(TB,"%s: %s",Bombers[Num].Nom,&Buffer[1]);
		AddMessage(TB,2500,GetBomberColor(Num));
		OutFile("ChatMessage recv");
		break;

	case	MSG_PLAY:
		TRAITERCV(break;)
		OutFile("Play recv");
		break;

	case	MSG_EXPBOMB:
		TRAITERCV(break;)
		o1=Buffer[1];
		x=Buffer[2];
		y=Buffer[3];
		int bbidx;
		for(bbidx=0;bbidx<NumBombs[o1];bbidx++)
			if(Bombs[o1][bbidx].Etat==B_PLAYING)
				if((Bombs[o1][bbidx].X==x)&&(Bombs[o1][bbidx].Y==y))
				{
					ExploseBomb(o1,bbidx);
					OutFile("ExpBomb recv OK");
					break;
				}
		OutFile("ExpBomb recv");
		break;

	case	MSG_RESET:
		TRAITERCV(break;)
		OutFile("Reset recv");
		break;

	case	MSG_RESETREPLY:
		TRAITERCV(break;)
		OutFile("ResetReply recv");
		break;

	case	MSG_BN_TELEPORT:
		TRAITERCV(break;)
		o1=Buffer[1];
		o2=Buffer[2];
		Bombers[To].X=float(o1*32+16);
		Bombers[To].Y=float(o2*32+16);
		OutFile("Teleport recv");
		break;

	case	MSG_BN_ALLEXPL:
		TRAITERCV(break;)
		for(x=0;x<NumPlayers;x++)
			if(!IsLocalPlayer(x))
			{
				for(y=0;y<NumBombs[x];y++)
					if(Bombs[x][y].Etat==B_PLAYING)
						ExploseBomb(x,y);
			}
			else
			{
				for(y=0;y<NumBombs[x];y++)
					if(Bombs[x][y].Etat==B_PLAYING)
					{
						ExploseBomb(x,y);
						SendGameMessage(x,-1,MSG_EXPBOMB,x,Bombs[x][y].X,Bombs[x][y].Y,0);
					}
			}
		OutFile("BN_ALLEXPL recv");
		break;

	case	MSG_BN_CRT:
		o1=Buffer[1];
		memcpy(&Int,&Buffer[2],4);
		Bombers[To].BType=BB_CRT;
	if(Synchro.bActive&&o1)
		Bombers[To].TpsGTC=Int+Synchro.TckBias;
	else
		Bombers[To].TpsGTC=TickCount()-dwLatency[Num];
		OutFile("BN_CRT recv");
		break;

	case	MSG_BN_LNG:
		o1=Buffer[1];
		memcpy(&Int,&Buffer[2],4);
		Bombers[To].BType=BB_LNG;
	if(Synchro.bActive&&o1)
		Bombers[To].TpsGTC=Int+Synchro.TckBias;
	else
		Bombers[To].TpsGTC=TickCount()-dwLatency[Num];
		OutFile("BN_LNG recv");
		break;

	case	MSG_BN_REVERSE:
		o1=Buffer[1];
		memcpy(&Int,&Buffer[2],4);
		Bombers[To].Reverse=-1;
	if(Synchro.bActive&&o1)
		Bombers[To].RevGTC=Int+Synchro.TckBias;
	else
		Bombers[To].RevGTC=TickCount()-dwLatency[Num];
		OutFile("BN_REV recv");
		break;

	case	MSG_SNDFORME:
		TRAITERCV(break;)
		x=*(int*)&Buffer[1];
		PlaySon((Bombers[Num].X-320)/320.0,x,0);
		OutFile("Son recv");
		break;

	case	MSG_BN_NUCL:
		TRAITERCV(break;)
		for(x=0;x<20;x++)
			for(y=0;y<15;y++)
			{
				if(Blocs[x][y].Type==CAISSE) {
					Blocs[x][y].Type=SOL;
					Blocs[x][y].TypeBonus=0;
				}
			}
		rct.top=0;
		rct.left=0;
		rct.right=20;
		rct.bottom=15;
		UpdateBuffer(rct);
		bUpdateAllScreen=GetUpdateCount();

		for(x=0;x<NumPlayers;x++)
		{
			Bombers[x].NbBomb+=10-Bombers[x].SimulBomb;
			Bombers[x].SimulBomb=10;
			Bombers[x].Blast=20;
			Bombers[x].RetOpt=0;
		}
		OutFile("Nucl recv");
		break;
/*
	case	MSG_BLOCTYPE:
		TRAITERCV(break;)
		Blocs[Buffer[1]][Buffer[2]].Type=Buffer[3];
		rct.left=Buffer[1];
		rct.top=Buffer[2];
		rct.right=Buffer[1]+1;
		rct.bottom=Buffer[2]+1;
		UpdateBuffer(rct);
		rct.left*=32;
		rct.top*=32;
		rct.right*=32;
		rct.bottom*=32;
		UpdateScreen(rct);
		bUpdateCase[Buffer[1]][Buffer[2]]=GetUpdateCount();
		OutFile("BlocType recv");
		break;
*/

	case	MSG_ERASEBONUS:
		//TRAITERCV(break;)
		o1=Buffer[1];
		o2=Buffer[2];
		Blocs[o1][o2].TypeBonus=0;
		rct.left=o1;
		rct.top=o2;
		rct.right=o1+1;
		rct.bottom=o2+1;
		UpdateBuffer(rct);
		bUpdateCase[o1][o2]=GetUpdateCount();
		OutFile("RAZ bonus recv");
		break;

	case	MSG_NEWBOMB:
		TRAITERCV(break;)
		o1=Buffer[1];
		o2=Buffer[2];
		if(Bombs[Num][o1].Etat==B_PLAYING)
		{
			o1=0;
			While((Bombs[Num][o1].Etat==B_PLAYING)&&(o1<TOT_BOMB))
				o1++;
		}
		memcpy(&Bombs[Num][o1],&Buffer[3],sizeof(Bomb));
		Blocs[Bombs[Num][o1].X][Bombs[Num][o1].Y].Bloque=1;
		if(Synchro.bActive&&o2)
			Bombs[Num][o1].LastGTC+=Synchro.TckBias;
		else
			Bombs[Num][o1].LastGTC=TickCount()-dwLatency[Num];
		UpdateBombCount();

		//OutFile("Bombe recv");
		break;

	case	MSG_VOICE:
		TRAITERCV(break;)
		PushVoiceStack(Num,Buffer+5,*(DWORD*)(&Buffer[1]));
		break;

	case	MSG_UPDATE:
		TRAITERCV(break;)
		lpBUpd=(struct	BomberUpdate*)&Buffer[1];
		Bombers[Num].X=lpBUpd->X;
		Bombers[Num].Y=lpBUpd->Y;
		Bombers[Num].Etat=lpBUpd->EtatSens>>4;
		Bombers[Num].Sens=lpBUpd->EtatSens&0xf;
		Bombers[Num].Sprite=lpBUpd->Spritedxdy>>4;
		b1=(lpBUpd->Spritedxdy>>2)&3;
		if(b1==1)
			Bombers[Num].dx=V;
		else
			if(b1==2)
				Bombers[Num].dx=-V;
			else
				Bombers[Num].dx=0;
		b1=lpBUpd->Spritedxdy&3;
		if(b1==1)
			Bombers[Num].dy=V;
		else
			if(b1==2)
				Bombers[Num].dy=-V;
			else
				Bombers[Num].dy=0;
		break;

	case	MSG_QUIT_REPLY:
		TRAITERCV(break;)
		OutFile("Quit reply recv");
		break;

	case	MSG_QUIT:
		TRAITERCV(break;)
		if(bDedicated)
			break;
		OutFile("Quit recv ");
		SendGameMessage(FROM_ALLLOCALPLAYERS,Num,MSG_QUIT_REPLY,0,0,0,0);
		break;

	case	MSG_HOSTQUIT:
		TRAITERCV(break;)
		OutFile("HostQuit recv ");
		break;

	case	MSG_ALLBLOCS:
		TRAITERCV(break;)
		lpBlUpd=(struct BlocUpdate*)&Buffer[1];
		for(t1=0;t1<20;t1++)
			for(t2=0;t2<15;t2++)
			{
				Blocs[t1][t2].Type=lpBlUpd->TypeBType&3;
				Blocs[t1][t2].TypeBonus=lpBlUpd->TypeBType>>2;
				Blocs[t1][t2].Bloque=0;
				Blocs[t1][t2].ExplLevel=0;
				Blocs[t1][t2].TypeSol=GetTypeSol(t1,t2);
				lpBlUpd++;
			}

		rct.left=0;
		rct.right=20;
		rct.top=0;
		rct.bottom=15;
		UpdateBuffer(rct);
		bUpdateAllScreen=GetUpdateCount();
		SetWindowText(hWndWin,LSZ_DP_RECVALLBLOCS);
		OutFile(LSZ_DP_RECVALLBLOCS);
		break;

	case	MSG_BLOC:
		TRAITERCV(break;)
		o1=Buffer[1];
		o2=Buffer[2];
		lpBlUpd=(struct BlocUpdate*)&Buffer[3];
		Blocs[o1][o2].Type=lpBlUpd->TypeBType&3;
		Blocs[o1][o2].TypeBonus=lpBlUpd->TypeBType>>2;
		Blocs[o1][o2].Bloque=0;
		Blocs[o1][o2].ExplLevel=0;
		Blocs[o1][o2].TypeSol=GetTypeSol(o1,o2);

		wsprintf(Buffer,LSZ_DP_RECVBLOCdON300,o2+o1*15+1);
		SetWindowText(hWndWin,Buffer);
		//OutFile(Buffer);

			rct.left=o1;
			rct.right=o1+1;
			rct.top=o2;
			rct.bottom=o2+1;
			UpdateBuffer(rct);

		//OutFile("Bloc recv");
		break;

	case MSG_GETFRAG:
		TRAITERCV(break;)
		o1=Buffer[1];
		if(!bSpectator)
			if(IsLocalPlayer(o1))
			{
				Bombers[o1].Frag++;
				UpdateRemoteData(o1);
			}
		sprintf(Buffer,LSZ_sKILLEDs,Bombers[o1].Nom,Bombers[Num].Nom);
		AddMessage(Buffer,2500,GetBomberColor(o1));
		bScoreRedraw=TRUE;
		OutFile("getFrag recv");
		break;

	case MSG_GETAK:
		TRAITERCV(break;)
		//Bombers[Num].AK++;
		sprintf(Buffer,LSZ_sSUICIDE,Bombers[Num].Nom);
		AddMessage(Buffer,2500,GetBomberColor(Bombers[Num].Color));
		bScoreRedraw=TRUE;
		OutFile("getAK recv");
		break;

	default:
		//OutFile("Receive: Message Inconnu !");
		break;

	}
	_chdir(szDir);

	return M;
}

void UpdateOtherGroup(void)
{
	if(!bConnected||(lpDP==NULL))
		return;
	if(OtherGrp)
		lpDP->DestroyGroup(OtherGrp);
	OtherGrp=0;
	lpDP->CreateGroup(&OtherGrp,NULL,NULL,0,0);
	OneOtherPlayer=FALSE;
	for(int iuog=0;iuog<MAX_PLAYERS;iuog++)
		cl_ids[iuog]=0;
	OutFile("Début OtherGroup");

	//Enum juste pour les spectateurs
	if(lpDP->EnumPlayers(0,EnumOtherPlayersCallback,&OtherGrp,DPENUMPLAYERS_REMOTE)!=DP_OK)
	{
		OutFile("Fin OtherGroup");
		lpDP->DestroyGroup(OtherGrp);
		return;
	}

	for(int clidx=0;clidx<NumPlayers;clidx++)
	{
		BOOL noDup=TRUE;
		if(BCtrl[clidx].cl_id!=cl_id)
		{
			for(int dclidx=0;dclidx<clidx;dclidx++)
				if(BCtrl[clidx].cl_id==BCtrl[dclidx].cl_id)
					noDup=FALSE;
		}
		else
			noDup=FALSE;

		if(noDup)
		{
			OneOtherPlayer=TRUE;
			lpDP->AddPlayerToGroup(OtherGrp,BCtrl[clidx].dpid);
			OutFileEx2("		Joueur %ld (cl_id %ld)",clidx,BCtrl[clidx].cl_id);
		}
	}

	OutFile("Fin OtherGroup");

	if(OneOtherPlayer)
		SendGameMessage(FROM_ALLLOCALPLAYERS,-1,MSG_OTHERGROUP,(int)OtherGrp,0,0,0);
	else
	{
		lpDP->DestroyGroup(OtherGrp);
		OtherGrp=0;
		SendGameMessage(FROM_ALLLOCALPLAYERS,-1,MSG_OTHERGROUP,0,0,0,0);
	}
	return;
}

void GereSynchro(void)
{
	if(!bConnected) return;
//	if(!IsHost) return;
	if(TickCount()-LatencyLastUpdate<PING_EVERY) return;
	SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_PING,0,0,0,0);
	return;
}

int FindLocalPlayer()
{
	for(int a=0;a<NumPlayers;a++)
		if(IsLocalPlayer(a))
			return a;
	return -1;
}

void SendID(DPID idFrom,DPID idTo,LPVOID lpData,DWORD dwSize)
{
	int hr;
	hr=DPERR_BUSY;

	char*  SendBuffer=NULL;
	if(!IsHost&&(MulticastID!=0)&&(idTo!=MulticastID))
	{
		SendBuffer=(char*)lpData;
		SendBuffer-=5+sizeof(DPID);
		SendBuffer[0]=MSG_DOMULTICAST;
		*(int*)&SendBuffer[1]=dwSize;
		*(DPID*)&SendBuffer[5]=idTo;
		//memcpy(&SendBuffer[5+sizeof(DPID)],lpData,dwSize);
		idTo=MulticastID;
		dwSize+=5+sizeof(DPID);
		lpData=SendBuffer;
	}
	else
		if((idTo==DPID_ALLPLAYERS)&&(MulticastID==0)&&(OtherGrp!=0))
			idTo=OtherGrp;

	if(bGarNeeded)
	{
#ifndef DX3
		if(Gar)
			hr=lpDP->SendEx(idFrom,idTo,  gAsync? DPSEND_ASYNC|DPSEND_GUARANTEED|DPSEND_NOSENDCOMPLETEMSG:DPSEND_GUARANTEED,  lpData,  dwSize,   gPrio?dwPriority:0,     0,   0,   0);
		else
			hr=lpDP->SendEx(idFrom,idTo,  nAsync? DPSEND_ASYNC|DPSEND_NOSENDCOMPLETEMSG:0 ,  lpData,  dwSize,   nPrio?dwPriority:0,     0,   0,   0);
#else
		if(Gar)
			hr=lpDP->Send(idFrom,idTo,  DPSEND_GUARANTEED,  lpData,  dwSize);
		else
			hr=lpDP->Send(idFrom,idTo,  0,  lpData,  dwSize);
#endif
	}
	else
	{
		NbSends++;
#ifndef DX3
		DWORD messcount,octcount;
		messcount=octcount=0;
		hr=lpDP->GetMessageQueue(0,0,DPMESSAGEQUEUE_SEND,&messcount,&octcount);
		if(messcount<MAX_SENDMSG)
			hr=lpDP->SendEx(idFrom,idTo,  nAsync? DPSEND_ASYNC|DPSEND_NOSENDCOMPLETEMSG:0 ,  lpData,  dwSize, nPrio?dwPriority:0,  nTimeOut?dwTimeOut:0,   0,   0);
		else
			OutFile("No send (saturation)");
		if(messcount>AVERAGE_SENDMSG)
			Overruns++;
#else
		hr=lpDP->Send(idFrom,idTo,  0, lpData,  dwSize);
#endif
	}

	if((hr!=DP_OK)&&(hr!=DPERR_PENDING))
	{
		OutFile("no send !");
		OutFileInt(dwSize);
		OutFileInt(*(char*)lpData);
	}

	//SAFE_DELETE_ARRAY(SendBuffer);
	if(NbSends>=32)
	{
		CheckNST(NbSends,Overruns);
		NbSends=Overruns=0;
	}
	return;
}

void CheckNST(DWORD snd,DWORD over)
{
	return;
	/*
	float overprop=float(2*float(over)/snd-1.0);
	OutFileEx("Sends ",snd);
	OutFileEx("Over ",over);
	int add_value=int(overprop*NST_MAX_BIAS);
	if(-add_value<=int(NetSendTime))
	{
		NetSendTime+=add_value;
		OutFileEx2("NetSendTime=%ld apres un inc de %ld (%ld)",NetSendTime,add_value,int(overprop*100));
	}
	else
		if(NetSendTime!=0)
		{
			NetSendTime=0;
			OutFile("NetSendTime=0");
		}
	return;
	*/
}

void Send(int SendNum,int Num,LPVOID lpData,DWORD dwSize)
{
	if(lpDP==0) return;
	if(!bConnected) return;
	DPID idTo,idFrom;
	if(Num==-1) 
		idTo=DPID_ALLPLAYERS;
	else
		idTo=BCtrl[Num].dpid;

	if(Num==NUM_MULTICAST)
		idTo=MulticastID;

	if(bDedicated)
	{
		idFrom=SpectatorID;
		SendID(idFrom,idTo,lpData,dwSize);
		return;
	}

	if(SendNum==FROM_FIRSTLOCALPLAYER)
	{
		int p=FindLocalPlayer();
		if(p==-1) return;
		idFrom=BCtrl[p].dpid;
		SendID(idFrom,idTo,lpData,dwSize);
	}
	else
		if(SendNum==FROM_ALLLOCALPLAYERS)
		{
			for(int cp=0;cp<NumPlayers;cp++)
				if(IsLocalPlayer(cp))
					SendID(BCtrl[cp].dpid,idTo,lpData,dwSize);
		}
		else
			if((MulticastID!=0)&&IsHost&&(SendNum==FROM_MULTICAST))
			{
				idFrom=MulticastID;
				SendID(idFrom,idTo,lpData,dwSize);
			}
			else
			{
				idFrom=BCtrl[SendNum].dpid;
				SendID(idFrom,idTo,lpData,dwSize);
			}

//	idf  idt flags   lpdata size priority  tout context messID
	return;
}

void UpdateRemoteData(int Num)
{
	if(!bConnected) return;
	if(lpDP==0) return;
	struct RemotePlayerData Remote;
	Remote.Me=Num;
	Remote.Frag=Bombers[Num].Frag;
	Remote.AK=Bombers[Num].AK;
	Remote.Team=Bombers[Num].Team;
	Remote.cl_id=cl_id;
	lpDP->SetPlayerData(BCtrl[Num].dpid,&Remote,sizeof(struct RemotePlayerData),DPSET_GUARANTEED|DPSET_REMOTE);
#ifndef DX3
	SendLobbyScores(Num);
#endif
	return;
}
/*
DWORD GetReceiveCount()
{
	if((!bConnected)||(lpDP==0)) return 0;
	DWORD mc=0;
	DWORD mct=0;
	for(int p=0;p<NumPlayers;p++)
		if(IsLocalPlayer(p))
		{
			lpDP->GetMessageCount(BCtrl[p].dpid,&mc);
			mct+=mc;
		}
	return mct;
}
*/

int FlushReceiveQueue()
{
	if((!bConnected)||(lpDP==NULL))
		return -1;
	char RM=-1;
	if(!bSpectator&&!bDedicated)
	{
		for(int ct=0;ct<NumPlayers;ct++)
			if(IsLocalPlayer(ct))
			{
				int hr;
				DWORD mc=0;
				hr=lpDP->GetMessageCount(BCtrl[ct].dpid,&mc);
				while((mc>0)&&(hr==DP_OK))
				{
					RM=ReceiveGameMessage(ct,-1);
//					OutFileEx("Flushing ",mc);
//					OutFileEx("		avec ",RM);
					if(IsCriticalMessage(RM))
						return RM;
					hr=lpDP->GetMessageCount(BCtrl[ct].dpid,&mc);
					if (RM==-1)
						mc=0;
				}
			}
	}
	else
	{
		do
		{
			RM=ReceiveGameMessage(-1,-1);
//			if((RM!=-1)&&(RM!=MSG_SYSDPLAY))
//				OutFileEx("Flushing ",RM);
		}
		While((RM!=MSG_SYSDPLAY)&&(RM!=-1)&&!IsCriticalMessage(RM));
	}
	return RM;
}

BOOL IsLocalPlayer(int Num)
{
	if((!bConnected)||(lpDP==NULL))
		return TRUE;
	DWORD dwFlags=0;
	if(Num<0)
		return FALSE;
	lpDP->GetPlayerFlags(BCtrl[Num].dpid,&dwFlags);
	return ((dwFlags&DPPLAYER_LOCAL)!=0);
}

void ChatMessage(int From,int To,char* text,BOOL bEcho)
{
	if(!bConnected||(lpDP==0)) return;
	if(bEcho)
	{
		char mess[BMAX_PATH];
		if(From>=0)
		{
			sprintf(mess,"%s: %s",Bombers[From].Nom,text);
			AddMessage(mess,2500,GetBomberColor(From));
		}
		else
		{
			sprintf(mess,"Chat: %s",text);
			AddMessage(mess,2500,0);
		}
	}

#ifndef DX3
	DPCHAT dpc;
	dpc.dwSize=sizeof(DPCHAT);
	dpc.dwFlags=0;
	dpc.lpszMessageA=text;
	DPID idTo,idFrom;
	if(To==-1) 
		idTo=DPID_ALLPLAYERS;
	else
		idTo=BCtrl[To].dpid;

	if(From==FROM_FIRSTLOCALPLAYER)
	{
		int p=FindLocalPlayer();
		if(p==-1) return;
		idFrom=BCtrl[p].dpid;
		lpDP->SendChatMessage(idFrom,idTo,0,&dpc);
	}
	else
		if(From==FROM_ALLLOCALPLAYERS)
		{
			for(int cp=0;cp<NumPlayers;cp++)
				if(IsLocalPlayer(cp))
					lpDP->SendChatMessage(BCtrl[cp].dpid,idTo,0,&dpc);
		}
		else
		{
			idFrom=BCtrl[From].dpid;
			lpDP->SendChatMessage(idFrom,idTo,0,&dpc);
		}
#else
		SendGameMessage(From,To,MSG_CHAT,(int)text,0,0,0);
#endif
	return;
}

BOOL IsCriticalMessage(int mess)
{
	return (mess==MSG_RESET);
}
