#include "General.h"
#include "Bomber.h"
#include "IA.h"
#include "iIA.h"
#include "Debug.h"

#if 0
#include <seer.h>
extern	struct	Bomber	Bombers[MAX_PLAYERS];
extern	struct	Blast	Blasts[MAX_PLAYERS][TOT_BLAST];
extern	int		NumBlasts[MAX_PLAYERS];
extern	struct	Bomb	Bombs[MAX_PLAYERS][TOT_BOMB];
extern	int		NumBombs[MAX_PLAYERS];
extern	struct	Bloc	Blocs[20][15];
extern	struct	ControlBomber	BCtrl[MAX_PLAYERS];
extern	struct	SETTINGS	Settings;
extern	int NumPlayers;
extern BOOL		UPlck[2*MAX_PLAYERS],DOWNlck[2*MAX_PLAYERS];
extern BOOL		LEFTlck[2*MAX_PLAYERS],RIGHTlck[2*MAX_PLAYERS];
extern BOOL		F1lck[2*MAX_PLAYERS],F2lck[2*MAX_PLAYERS];
extern BOOL		F3lck[2*MAX_PLAYERS];

scInstance* IAInst;
scScript IAScript;
int IANumPlayers;

void InitIA(void)
{
	IAScript=scCompile_File("Scripts\\IA.sc");
	if(IAScript==0)
		return;

//extern	struct	Bomber	Bombers[MAX_PLAYERS];
	scAddExtSym(Bombers);
//extern	struct	Blast	Blasts[MAX_PLAYERS][TOT_BLAST];
	scAddExtSym(Blasts);
//extern	int		NumBlasts[MAX_PLAYERS];
	scAddExtSym(NumBlasts);
//extern	struct	Bomb	Bombs[MAX_PLAYERS][TOT_BOMB];
	scAddExtSym(Bombs);
//extern	int		NumBombs[MAX_PLAYERS];
	scAddExtSym(NumBombs);
//extern	struct	Bloc	Blocs[20][15];
	scAddExtSym(Blocs);
//extern	struct	ControlBomber	BCtrl[MAX_PLAYERS];
	scAddExtSym(BCtrl);
//extern	struct	SETTINGS	Settings;
	scAddExtSym(Settings);
	scAddExtSym(IANumPlayers);
//extern BOOL		UPlck[2*MAX_PLAYERS];
	scAddExtSym(UPlck);
//extern BOOL		DOWNlck[2*MAX_PLAYERS];
	scAddExtSym(DOWNlck);
//extern BOOL		LEFTlck[2*MAX_PLAYERS];
	scAddExtSym(LEFTlck);
//extern BOOL		RIGHTlck[2*MAX_PLAYERS];
	scAddExtSym(RIGHTlck);
//extern BOOL		F1lck[2*MAX_PLAYERS];
	scAddExtSym(F1lck);
//extern BOOL		F2lck[2*MAX_PLAYERS];
	scAddExtSym(F2lck);
//extern BOOL		F3lck[2*MAX_PLAYERS];
	scAddExtSym(F3lck);

	scAddExtSym(TickCount);
	scAddExtSym(rand);
	scAddExtSym(sqrt);
	scAddExtSym(pow);
	scAddExtSym(memcpy);
	IANumPlayers=NumPlayers;


	IAInst=scCreate_Instance(IAScript,"");
	if(IAInst==0)
		return;
	return;
}

void UninitIA(void)
{
	if(IAInst)
	{
		scFree_Instance(IAInst);
		IAInst=NULL;
	}
	if(IAScript)
	{
		free(IAScript);
		IAScript=0;
	}
	return;
}

void ResetIA(void)
{
	if(IAInst==NULL)
		return;
	int scres=scCall_Instance(IAInst,scGet_Symbol(IAInst,"ResetIA"));
	return;
}

void ControleIA(int n)
{
	if(IAInst==NULL)
		return;
	int scres=scCall_Instance(IAInst,scGet_Symbol(IAInst,"ControleIA"),n);
	return;
}

#else

extern	struct	Bomber	Bombers[MAX_PLAYERS];
extern	struct	Blast	Blasts[MAX_PLAYERS][TOT_BLAST];
extern	int		NumBlasts[MAX_PLAYERS];
extern	struct	Bomb	Bombs[MAX_PLAYERS][TOT_BOMB];
extern	int		NumBombs[MAX_PLAYERS];
extern	struct	Bloc	Blocs[20][15];
extern	struct	ControlBomber	BCtrl[MAX_PLAYERS];
extern	struct	SETTINGS	Settings;
extern	int NumPlayers;
extern BOOL		UPlck[2*MAX_PLAYERS],DOWNlck[2*MAX_PLAYERS];
extern BOOL		LEFTlck[2*MAX_PLAYERS],RIGHTlck[2*MAX_PLAYERS];
extern BOOL		F1lck[2*MAX_PLAYERS],F2lck[2*MAX_PLAYERS];
extern BOOL		F3lck[2*MAX_PLAYERS];

DWORD LastIAMove;

BOOL	lUPlck[2*MAX_PLAYERS],lDOWNlck[2*MAX_PLAYERS];
BOOL	lLEFTlck[2*MAX_PLAYERS],lRIGHTlck[2*MAX_PLAYERS];
BOOL	lF1lck[2*MAX_PLAYERS],lF2lck[2*MAX_PLAYERS];
DWORD	timernkey[2*MAX_PLAYERS];

struct IACase Case[20][15];
struct IACase lCase[20][15];
int LastXB[MAX_PLAYERS];
int LastYB[MAX_PLAYERS];
DWORD LastTickIA[MAX_PLAYERS];
BOOL bAleat[MAX_PLAYERS];
int LastXAleat[MAX_PLAYERS];
int LastYAleat[MAX_PLAYERS];
DWORD LastTickAleat[MAX_PLAYERS];
DWORD LastRealMove[MAX_PLAYERS];
DWORD BotLatency;

BYTE InteretBonus[NUM_BONUS+1]=
	{
			0,	//aucun bonus
			0,	//inutilisé
			0,	//inutilisé
			30,	//blast
			30,	//+1 bomb
			45,	//mega bomb
			10,	//crt
			15,	//std
			25,	//lng
			20,	//rig
			45,	//ret
			30,	//nucl
			30,	//allexpl
			0,	//inutilisé
			25,	//teleport
			0,	//mine
			35	//reverse
	};

void InitIA(void)
{
	return;
}

void UninitIA(void)
{
	return;
}

void ResetIA(void)
{
	int cnt;
	for(cnt=0;cnt<MAX_PLAYERS;cnt++)
	{
		LastXB[cnt]=0;
		LastYB[cnt]=0;
		LastTickIA[cnt]=0;
		bAleat[cnt]=FALSE;
		LastXAleat[cnt]=0;
		LastYAleat[cnt]=0;
		LastTickAleat[cnt]=0;
		LastRealMove[cnt]=0;
	}
	for(cnt=0;cnt<2*MAX_PLAYERS;cnt++)
	{
		lRIGHTlck[cnt]=RIGHTlck[cnt]=0;
		lLEFTlck[cnt]=LEFTlck[cnt]=0;
		lUPlck[cnt]=UPlck[cnt]=0;
		lDOWNlck[cnt]=DOWNlck[cnt]=0;
		lF1lck[cnt]=F1lck[cnt]=0;
		lF2lck[cnt]=F2lck[cnt]=0;
		timernkey[cnt]=0;
	}
	switch(Settings.GO.BOTLEVEL)
	{
	case 0:
		BotLatency=300;
		break;
	case 1:
		BotLatency=150;
		break;
	case 2:
		BotLatency=50;
		break;
	case 3:
		BotLatency=0;
		break;
	default:
		BotLatency=50;
		break;
	}
	return;
}

void ControleIA(int n)
{
	int i,j,k,l;
	int NrT=Bombers[n].CmdCtrl;
	BOOL bDoHigh=FALSE;

	int XB=int(Bombers[n].X)/32;
	int YB=int(Bombers[n].Y)/32;

	if(LEFTlck[NrT]||RIGHTlck[NrT]||UPlck[NrT]||DOWNlck[NrT])
	{
		if(F1lck[NrT]||F2lck[NrT])
			LastRealMove[n]=TickCount();

		if(F2lck[NrT])
			LastTickIA[n]-=TICK_IA;

		F1lck[NrT]=FALSE;
		F2lck[NrT]=FALSE;
		F3lck[NrT]=FALSE;
		if(TickCount()-LastTickIA[n]<TICK_IA)
		{
			if((abs((((int)Bombers[n].X)&31)-16)>4)&&LEFTlck[NrT]) return;
			if((abs((((int)Bombers[n].X)&31)-16)>4)&&RIGHTlck[NrT]) return;
			if((abs((((int)Bombers[n].Y)&31)-16)>4)&&UPlck[NrT]) return;
			if((abs((((int)Bombers[n].Y)&31)-16)>4)&&DOWNlck[NrT]) return;
			if((LastXB[n]==XB)&&(LastYB[n]==YB))
				return;
		}
	}
	else
		if(F1lck[NrT]||F2lck[NrT])
			LastRealMove[n]=TickCount();

	F1lck[NrT]=FALSE;
	F2lck[NrT]=FALSE;
	F3lck[NrT]=FALSE;

	if((TickCount()-LastRealMove[n]>TICK_IATOUT)&&(LastRealMove[n]!=0))
	{
		if(!Blocs[XB][YB].Bloque)
		{
			if(Bombers[n].NbBomb>0)
				F1lck[NrT]=TRUE;
			else if(Bombers[n].RigOpt)
				F3lck[NrT]=TRUE;
			else
				F2lck[NrT]=TRUE;
		}
		else
			F2lck[NrT]=TRUE;
//			bDoHigh=TRUE;
//			F3lck[NrT]=TRUE;
		return;
	}

	LastXB[n]=XB;
	LastYB[n]=YB;

	LastTickIA[n]=TickCount();
	LEFTlck[NrT]=FALSE;
	RIGHTlck[NrT]=FALSE;
	UPlck[NrT]=FALSE;
	DOWNlck[NrT]=FALSE;

	GenereCase(n);
	BOOL PFE=PeutFaireExploser(n);
	if(PFE)
		FaitExploser(n,NrT);
//	if(bDoHigh)
//		Case[XB][YB].Danger=DNG_HIGH;

	int InteretMin=-4*IBOMBE+Case[XB][YB].Interet;
	k=XB;
	l=YB;
	for(i=0;i<20;i++)
		for(j=0;j<15;j++)
			if((Case[i][j].Danger==0)&&((Case[i][j].Interet>InteretMin)||((rand()%2)&&(Case[i][j].Interet==InteretMin)) ))
			{
				InteretMin=Case[i][j].Interet;
				k=i;
				l=j;
			}

	int DirX=k-XB;
	int DirY=l-YB;
	if(abs(DirX)+abs(DirY)==1)
		if(InteretEgaux(XB,YB))
			DirAleat(XB,YB,&DirX,&DirY);

	AppuieTouches(n,DirX,DirY,XB,YB,NrT);

	if(Case[XB][YB].Danger!=DNG_NONE)
		UPlck[NrT]=DOWNlck[NrT]=LEFTlck[NrT]=RIGHTlck[NrT]=FALSE;

	if(Case[XB][YB].Danger!=DNG_NONE)
	{
		if((UPlck[NrT]==0)&&(DOWNlck[NrT]==0)&&(LEFTlck[NrT]==0)&&(RIGHTlck[NrT]==0))
		{
			//if((DirX!=0)&&(DirY!=0))
			//{

				InteretMin=-4*IBOMBE+Case[XB][YB].Interet;
				k=XB;
				l=YB;
				if(InteretEgaux(XB,YB))
				{
					DirAleat(XB,YB,&k,&l);
					k+=XB;
					l+=YB;
				}
				else
				{
					if(XB<19)
						if(((Case[XB+1][YB].Interet>InteretMin)||((rand()%2)&&(Case[XB+1][YB].Interet==InteretMin)))&&PeutAvancer(n,XB,YB,1,0))
						{
							k=XB+1;
							l=YB;
							InteretMin=Case[XB+1][YB].Interet;
						}

					if(YB>0)
						if(((Case[XB][YB-1].Interet>InteretMin)||((rand()%2)&&(Case[XB][YB-1].Interet==InteretMin)))&&PeutAvancer(n,XB,YB,0,-1))
						{
							k=XB;
							l=YB-1;
							InteretMin=Case[XB][YB-1].Interet;
						}

					if(XB>0)
						if(((Case[XB-1][YB].Interet>InteretMin)||((rand()%2)&&(Case[XB+1][YB].Interet==InteretMin)))&&PeutAvancer(n,XB,YB,-1,0))
						{
							k=XB-1;
							l=YB;
							InteretMin=Case[XB-1][YB].Interet;
						}

					if(YB<14)
						if(((Case[XB][YB+1].Interet>InteretMin)||((rand()%2)&&(Case[XB][YB+1].Interet==InteretMin)))&&PeutAvancer(n,XB,YB,0,1))
						{
							k=XB;
							l=YB+1;
							InteretMin=Case[XB][YB+1].Interet;
						}
				}
				
//				int d=Case[k][l].Danger;
//				Case[k][l].Danger=0;
				AppuieTouches(n,k-XB,l-YB,XB,YB,NrT);
//				Case[k][l].Danger=d;
				
			//}
		}
	}
	
	if((UPlck[NrT]==0)&&(DOWNlck[NrT]==0)&&(LEFTlck[NrT]==0)&&(RIGHTlck[NrT]==0))
	{
		if(PFE)
			FaitExploser(n,NrT);

		if((F1lck[NrT]==0)&&(F2lck[NrT]==0))
			if(EstBloque(XB,YB)||(NumCaisses()==0)||(TickCount()-LastIAMove>2500))
				BougeAleat(n,XB,YB,NrT);
			else
				bAleat[n]=FALSE;
	}
	else
		bAleat[n]=FALSE;

	if(!((UPlck[NrT]==0)&&(DOWNlck[NrT]==0)&&(LEFTlck[NrT]==0)&&(RIGHTlck[NrT]==0)))
		LastIAMove=TickCount();
	
	int dx=LEFTlck[NrT]?-1:RIGHTlck[NrT]?1:0;
	int dy=UPlck[NrT]?-1:DOWNlck[NrT]?1:0;
	if(PeutPoserBombe(n,XB,YB,dx,dy))
		F1lck[NrT]=TRUE;

	DelayedKeypress(NrT);

	return;
}

int NumCaisses(void)
{
	int count=0;
	for(int y=0;y<15;y++)
		for(int x=0;x<20;x++)
			if(Blocs[x][y].Type==CAISSE)
				count++;

	return count;
}

BOOL PeutAvancer(int n,int XB,int YB,int dx,int dy)
{
	if(dx!=0)
		dx=(int)_copysign(1,dx);
	if(dy!=0)
		dy=(int)_copysign(1,dy);
	if(Blocs[XB+dx][YB+dy].TypeBonus==BN_ALLEXPL)
		if(!PeutToutFaireExploser(n))
			return FALSE;

	if(dx)
	{
		if(!Case[XB+dx][YB].Bloque&&!Case[XB+dx][YB].Bombe)
			if(Case[XB][YB].Danger==DNG_NONE)
				return (Case[XB+dx][YB].Danger==DNG_NONE)||(Case[XB+dx][YB].Danger>DNG_HIGH+1);
			else
				return (Case[XB+dx][YB].Danger==DNG_NONE)||(Case[XB+dx][YB].Danger>=Case[XB][YB].Danger)||(Case[XB+dx][YB].Danger>DNG_HIGH);
		return FALSE;
	}
	else
		if(dy)
		{
			if(!Case[XB][YB+dy].Bloque&&!Case[XB][YB+dy].Bombe)
				if(Case[XB][YB].Danger==DNG_NONE)
					return (Case[XB][YB+dy].Danger==DNG_NONE)||(Case[XB][YB+dy].Danger>DNG_HIGH+1);
				else
					return (Case[XB][YB+dy].Danger==DNG_NONE)||(Case[XB][YB+dy].Danger>=Case[XB][YB].Danger)||(Case[XB][YB+dy].Danger>DNG_HIGH);
			return FALSE;
		}
		else
			return FALSE;

//	return FALSE;
}

void EraseData(int XB,int YB)
{
	for(int i=0;i<20;i++)
		for(int j=0;j<15;j++)
		{
			Case[i][j].Interet=-abs(i-XB)-abs(j-YB);
			Case[i][j].Danger=DNG_NONE;
			Case[i][j].PoseBombe=0;
			Case[i][j].Bloque=0;
			Case[i][j].DngOwner=0;
			Case[i][j].Bombe=FALSE;
			Case[i][j].Blast=FALSE;
			Case[i][j].BlastOwner=0;
			Case[i][j].ForceDanger=DNG_NONE;
		}
	return;
}

void GenereCaisse(int x,int y, int blast)
{
	int px,py;
	Case[x][y].Bloque=1;
	Case[x][y].PoseBombe++;
	for(px=x+1;px<=x+blast;px++)
	{
		if(EstBloque(px,y))
			break;
		Case[px][y].Interet+=ICAISSE;
		Case[px][y].PoseBombe++;
	}
	for(px=x-1;px>=x-blast;px--)
	{
		if(EstBloque(px,y))
			break;
		Case[px][y].Interet+=ICAISSE;
		Case[px][y].PoseBombe++;
	}
	for(py=y+1;py<=y+blast;py++)
	{
		if(EstBloque(x,py))
			break;
		Case[x][py].Interet+=ICAISSE;
		Case[x][py].PoseBombe++;
	}
	for(py=y-1;py>=y-blast;py--)
	{
		if(EstBloque(x,py))
			break;
		Case[x][py].Interet+=ICAISSE;
		Case[x][py].PoseBombe++;
	}
	return;
}

void GenereCase(int n)
{
	BOOL bn_allexpl_present=0;
	int i,j,k;
	int NrT=Bombers[n].CmdCtrl;

	int XB=int(Bombers[n].X)/32;
	int YB=int(Bombers[n].Y)/32;
	EraseData(XB,YB);

	if(!Settings.GO.TEAMPLAY)
	{
		for(i=0;i<NumPlayers;i++)
		{
			if((i==n)||(Bombers[i].Etat!=B_PLAYING)||(Bombers[i].Sens==B_MORT)) continue;
			for(k=int(Bombers[i].X/32);k<=int(Bombers[i].X/32)+Bombers[n].Blast;k++)
			{
				if(EstBloque(k,(int)Bombers[i].Y/32))
					break;
				Case[k][(int)Bombers[i].Y/32].Interet+=IBOMBER;
				Case[k][(int)Bombers[i].Y/32].PoseBombe=1;
			}

			for(k=int(Bombers[i].X/32)-1;k>=int(Bombers[i].X/32)-Bombers[n].Blast;k--)
			{
				if(EstBloque(k,(int)Bombers[i].Y/32))
					break;
				Case[k][(int)Bombers[i].Y/32].Interet+=IBOMBER;
				Case[k][(int)Bombers[i].Y/32].PoseBombe=1;
			}

			for(k=int(Bombers[i].Y/32);k<=int(Bombers[i].Y/32)+Bombers[n].Blast;k++)
			{
				if(EstBloque((int)Bombers[i].X/32,k))
					break;
				Case[(int)Bombers[i].X/32][k].Interet+=IBOMBER;
				Case[(int)Bombers[i].X/32][k].PoseBombe=1;
			}

			for(k=int(Bombers[i].Y/32)-1;k>=int(Bombers[i].Y/32)-Bombers[n].Blast;k--)
			{
				if(EstBloque((int)Bombers[i].X/32,k))
					break;
				Case[(int)Bombers[i].X/32][k].Interet+=IBOMBER;
				Case[(int)Bombers[i].X/32][k].PoseBombe=1;
			}
		}
	}
	else
	{
		for(i=0;i<NumPlayers;i++)
		{
			if((i==n)||MemeTeam(n,i)||(Bombers[i].Etat!=B_PLAYING)||(Bombers[i].Sens==B_MORT)) continue;
			for(k=int(Bombers[i].X/32)-Bombers[n].Blast;k<=int(Bombers[i].X/32)+Bombers[n].Blast;k++)
			{
				if(k<0 || k>19) continue;
				if(Blocs[k][(int)Bombers[i].Y/32].Type!=SOL) break;
				Case[k][(int)Bombers[i].Y/32].Interet+=IBOMBER;
				Case[k][(int)Bombers[i].Y/32].PoseBombe=1;
			}

			for(k=int(Bombers[i].Y/32)-Bombers[n].Blast;k<=int(Bombers[i].Y/32)+Bombers[n].Blast;k++)
			{
				if(k<0 || k>14) continue;
				if(Blocs[(int)Bombers[i].X/32][k].Type!=SOL) break;
				Case[(int)Bombers[i].X/32][k].Interet+=IBOMBER;
				Case[(int)Bombers[i].X/32][k].PoseBombe=1;
			}
		}
	}

	BOOL PTFE=PeutToutFaireExploser(n);
	for(j=0;j<15;j++)
	{
		for(i=0;i<20;i++)
		{
			if(Blocs[i][j].Bloque)
				Case[i][j].Bloque=1;
			switch(Blocs[i][j].Type)
			{
			case MUR:
				Case[i][j].Bloque=1;
				//Case[i][j].Interet=0;
				break;
			case CAISSE:
				GenereCaisse(i,j,Bombers[n].Blast);
				break;
				
			case SOL:
				Case[i][j].Interet+=InteretBonus[Blocs[i][j].TypeBonus];
				if(Blocs[i][j].TypeBonus==BN_ALLEXPL)
				{
					bn_allexpl_present=TRUE;
					if(!PTFE)
					{
						Case[i][j].Interet-=10000;
						Case[i][j].Bloque=1;
					}
				}
				break;
			}
		}
	}

	for(i=0;i<NumPlayers;i++)
		for(j=0;j<NumBombs[i];j++)
			if(Bombs[i][j].Etat==B_PLAYING)
				Case[Bombs[i][j].X][Bombs[i][j].Y].Bombe=TRUE;

	for(i=0;i<NumPlayers;i++)
		for(j=0;j<NumBombs[i];j++)
			if(Bombs[i][j].Etat==B_PLAYING)
				DangerBombe(n,i,j,XB,YB,bn_allexpl_present);


	if(!Settings.GO.TEAMPLAY)
	{
		for(i=0;i<NumPlayers;i++)
			for(j=0;j<NumBlasts[i];j++)
				if(Blasts[i][j].Etat==B_PLAYING)
				{
					Case[Blasts[i][j].X][Blasts[i][j].Y].Interet=-32000;
					Case[Blasts[i][j].X][Blasts[i][j].Y].Danger=DNG_MAX;
				}
	}
	else
	{
		for(i=0;i<NumPlayers;i++)
		{
			if((i!=n)&&MemeTeam(i,n))
				continue;
			for(j=0;j<NumBlasts[i];j++)
				if(Blasts[i][j].Etat==B_PLAYING)
				{
					Case[Blasts[i][j].X][Blasts[i][j].Y].Interet=-32000;
					Case[Blasts[i][j].X][Blasts[i][j].Y].Danger=DNG_MAX;
				}
		}
	}

	return;
}

void BougeAleat(int n,int XB,int YB,int NrT)
{
	/*
	for(int cx=0;cx<20;cx++)
		for(int cy=0;cy<15;cy++)
			if(Danger[cx][cy]==2) Danger[cx][cy]=1;
	*/

	int dx,dy;

	if(!bAleat||((LastXAleat[n]==0)&&(LastYAleat[n]==0))||(TickCount()-LastTickAleat[n]>TICK_ALEAT))
	{
		LastTickAleat[n]=TickCount();
		dx=rand()%3-1;
		dy=rand()%3-1;
	}
	else
	{
		dx=LastXAleat[n];
		dy=LastYAleat[n];
	}

	if(XB==0)
		dx=abs(dx);
	if(YB==0)
		dy=abs(dy);
	if(XB==19)
		dx=-abs(dx);
	if(YB==14)
		dy=-abs(dy);

	if(!PeutAvancer(n,XB,YB,dx,0))
		dx=0;
	if(!PeutAvancer(n,XB,YB,0,dy))
		dy=0;
	LastXAleat[n]=dx;
	LastYAleat[n]=dy;
	AppuieTouches(n,dx,dy,XB,YB,NrT);
	bAleat[n]=TRUE;
	return;
}

void SimulExplosion(int a,int b)
{
	int k,l,m;
 	Case[Bombs[a][b].X][Bombs[a][b].Y].Interet=-32000;
 	Case[Bombs[a][b].X][Bombs[a][b].Y].Danger=DNG_MAX;
 	Case[Bombs[a][b].X][Bombs[a][b].Y].Blast=TRUE;
 	Case[Bombs[a][b].X][Bombs[a][b].Y].BlastOwner=a;
 	Case[Bombs[a][b].X][Bombs[a][b].Y].Bombe=FALSE;

	for(l=Bombs[a][b].X-1;l>=Bombs[a][b].X-Bombs[a][b].Blast;l--)
	{
		if((l<0)||(l>19)) break;
		if(Case[l][Bombs[a][b].Y].Bombe)
		{
			BOOL br=FALSE;
			for(m=0;m<NumPlayers;m++)
				for(k=0;k<NumBombs[m];k++)
					if((Bombs[m][k].Etat==B_PLAYING)&&(Bombs[m][k].X==l)&&(Bombs[m][k].Y==Bombs[a][b].Y)) 
					{
						SimulExplosion(m,k);
						br=TRUE;
					}
			if(br)
				break;
		}
		Case[l][Bombs[a][b].Y].Interet=-32000;
		Case[l][Bombs[a][b].Y].Danger=DNG_MAX;
		Case[l][Bombs[a][b].Y].Blast=TRUE;
		Case[l][Bombs[a][b].Y].BlastOwner=a;
		if(EstBloque(l,Bombs[a][b].Y))
			break;
	}

	for(l=Bombs[a][b].X+1;l<=Bombs[a][b].X+Bombs[a][b].Blast;l++)
	{
		if((l<0)||(l>19)) break;
		if(Case[l][Bombs[a][b].Y].Bombe)
		{
			BOOL br=FALSE;
			for(m=0;m<NumPlayers;m++)
				for(k=0;k<NumBombs[m];k++)
					if((Bombs[m][k].Etat==B_PLAYING)&&(Bombs[m][k].X==l)&&(Bombs[m][k].Y==Bombs[a][b].Y))
					{
						SimulExplosion(m,k);
						br=TRUE;
					}
			if(br)
				break;
		}
		Case[l][Bombs[a][b].Y].Interet=-32000;
		Case[l][Bombs[a][b].Y].Danger=DNG_MAX;
		Case[l][Bombs[a][b].Y].Blast=TRUE;
		Case[l][Bombs[a][b].Y].BlastOwner=a;
		if(EstBloque(l,Bombs[a][b].Y))
			break;
	}

	for(l=Bombs[a][b].Y-1;l>=Bombs[a][b].Y-Bombs[a][b].Blast;l--)
	{
		if((l<0)||(l>14)) break;
		if(Case[Bombs[a][b].X][l].Bombe)
		{
			BOOL br=FALSE;
			for(m=0;m<NumPlayers;m++)
				for(k=0;k<NumBombs[m];k++)
					if((Bombs[m][k].Etat==B_PLAYING)&&(Bombs[m][k].X==Bombs[a][b].X)&&(Bombs[m][k].Y==l))
					{
						SimulExplosion(m,k);
						br=TRUE;
					}
			if(br)
				break;
		}
		Case[Bombs[a][b].X][l].Interet=-32000;
		Case[Bombs[a][b].X][l].Danger=DNG_MAX;
		Case[Bombs[a][b].X][l].Blast=TRUE;
		Case[Bombs[a][b].X][l].BlastOwner=a;
		if(EstBloque(Bombs[a][b].X,l))
			break;
	}

	for(l=Bombs[a][b].Y+1;l<=Bombs[a][b].Y+Bombs[a][b].Blast;l++)
	{
		if((l<0)||(l>14)) break;
		if(Case[Bombs[a][b].X][l].Bombe)
		{
			BOOL br=FALSE;
			for(m=0;m<NumPlayers;m++)
				for(k=0;k<NumBombs[m];k++)
					if((Bombs[m][k].Etat==B_PLAYING)&&(Bombs[m][k].X==Bombs[a][b].X)&&(Bombs[m][k].Y==l))
					{
						SimulExplosion(m,k);
						br=TRUE;
					}
			if(br)
				break;
		}
		Case[Bombs[a][b].X][l].Interet=-32000;
		Case[Bombs[a][b].X][l].Danger=DNG_MAX;
		Case[Bombs[a][b].X][l].Blast=TRUE;
		Case[Bombs[a][b].X][l].BlastOwner=a;
		if(EstBloque(Bombs[a][b].X,l))
			break;
	}

 	Case[Bombs[a][b].X][Bombs[a][b].Y].Bombe=TRUE;
	return;
}

void DangerBombe(int n,int a,int b,int XB,int YB,BOOL bn_allexpl_present)
{
	int k,m;
	if((XB==Bombs[a][b].X)&&(YB==Bombs[a][b].Y))
	{
		if(Bombs[a][b].X>0)
			if(CulDeSac(Bombs[a][b].X,Bombs[a][b].Y,-1,0,Bombs[a][b].Blast))
				Case[Bombs[a][b].X-1][Bombs[a][b].Y].Danger=DNG_MAX;

		if(Bombs[a][b].X<19)
			if(CulDeSac(Bombs[a][b].X,Bombs[a][b].Y,1,0,Bombs[a][b].Blast))
				Case[Bombs[a][b].X+1][Bombs[a][b].Y].Danger=DNG_MAX;

		if(Bombs[a][b].Y>0)
			if(CulDeSac(Bombs[a][b].X,Bombs[a][b].Y,0,-1,Bombs[a][b].Blast))
				Case[Bombs[a][b].X][Bombs[a][b].Y-1].Danger=DNG_MAX;

		if(Bombs[a][b].Y<14)
			if(CulDeSac(Bombs[a][b].X,Bombs[a][b].Y,0,1,Bombs[a][b].Blast))
				Case[Bombs[a][b].X][Bombs[a][b].Y+1].Danger=DNG_MAX;
	}

	if(Settings.GO.TEAMPLAY&&MemeTeam(n,a)&&(n!=a))
		m=DNG_LOW;
	else
	{
		if(Bombs[a][b].MustExpl!=0)
			m=DNG_MAX;
		else
			if(Bombs[a][b].NSpr<BB_RETSPR)
			{
				m=BB_RETSPR-Bombs[a][b].NSpr;
				if((m==DNG_NONE)||(m>DNG_LOW+1))
					m=DNG_MAX;
			}
			else
			{
				if(Bombers[a].Sens!=B_MORT)
				{
					if(a!=n)
						m=DNG_HIGH;
					else
						m=DNG_HIGH+1;
				}
				else
					m=DNG_LOW;
			}
	}

	if(Case[Bombs[a][b].X][Bombs[a][b].Y].ForceDanger!=DNG_NONE)
		m=min(m,Case[Bombs[a][b].X][Bombs[a][b].Y].ForceDanger);
		
	if(bn_allexpl_present)
		m=DNG_HIGH;

	Case[Bombs[a][b].X][Bombs[a][b].Y].ForceDanger=m;
	Case[Bombs[a][b].X][Bombs[a][b].Y].Danger=m;
	Case[Bombs[a][b].X][Bombs[a][b].Y].DngOwner=a;
	Case[Bombs[a][b].X][Bombs[a][b].Y].Interet+=-IBOMBE*32;

	for(k=Bombs[a][b].X+1;k<=Bombs[a][b].X+Bombs[a][b].Blast;k++)
	{
		if((k<0)||(k>19)) continue;
		if(Case[k][Bombs[a][b].Y].Bombe)
			if((Case[k][Bombs[a][b].Y].ForceDanger==0)||(Case[k][Bombs[a][b].Y].ForceDanger>m))
			{
				int xc,yc;
				Case[k][Bombs[a][b].Y].ForceDanger=m;
				if(FindBombe(k,Bombs[a][b].Y,&xc,&yc))
					DangerBombe(n,xc,yc,XB,YB,bn_allexpl_present);
			}
		if((k!=Bombs[a][b].X)&&EstBloque(k,Bombs[a][b].Y)) break;
		Case[k][Bombs[a][b].Y].Interet+=-IBOMBE*32+IBOMBE*abs(k-Bombs[a][b].X);
		Case[k][Bombs[a][b].Y].Danger=Case[k][Bombs[a][b].Y].Danger!=DNG_NONE?min(Case[k][Bombs[a][b].Y].Danger,m):m;
		Case[k][Bombs[a][b].Y].DngOwner=a;
	}

	for(k=Bombs[a][b].X-1;k>=Bombs[a][b].X-Bombs[a][b].Blast;k--)
	{
		if((k<0)||(k>19)) continue;
		if(Case[k][Bombs[a][b].Y].Bombe)
			if((Case[k][Bombs[a][b].Y].ForceDanger==0)||(Case[k][Bombs[a][b].Y].ForceDanger>m))
			{
				int xc,yc;
				Case[k][Bombs[a][b].Y].ForceDanger=m;
				if(FindBombe(k,Bombs[a][b].Y,&xc,&yc))
					DangerBombe(n,xc,yc,XB,YB,bn_allexpl_present);
			}
		if((k!=Bombs[a][b].X)&&EstBloque(k,Bombs[a][b].Y)) break;
		Case[k][Bombs[a][b].Y].Interet+=-IBOMBE*32+IBOMBE*abs(k-Bombs[a][b].X);
		Case[k][Bombs[a][b].Y].Danger=Case[k][Bombs[a][b].Y].Danger!=DNG_NONE?min(Case[k][Bombs[a][b].Y].Danger,m):m;
		Case[k][Bombs[a][b].Y].DngOwner=a;
	}

	for(k=Bombs[a][b].Y+1;k<=Bombs[a][b].Y+Bombs[a][b].Blast;k++)
	{
		if((k<0)||(k>14)) continue;
		if(Case[Bombs[a][b].X][k].Bombe)
			if((Case[Bombs[a][b].X][k].ForceDanger==0)||(Case[Bombs[a][b].X][k].ForceDanger>m))
			{
				int xc,yc;
				Case[Bombs[a][b].X][k].ForceDanger=m;
				if(FindBombe(Bombs[a][b].X,k,&xc,&yc))
					DangerBombe(n,xc,yc,XB,YB,bn_allexpl_present);
			}
		if((k!=Bombs[a][b].Y)&&EstBloque(Bombs[a][b].X,k)) break;
		Case[Bombs[a][b].X][k].Interet+=-IBOMBE*32+IBOMBE*abs(k-Bombs[a][b].Y);
		Case[Bombs[a][b].X][k].Danger=Case[Bombs[a][b].X][k].Danger!=DNG_NONE?min(Case[Bombs[a][b].X][k].Danger,m):m;
		Case[Bombs[a][b].X][k].DngOwner=a;
	}
	
	for(k=Bombs[a][b].Y-1;k>=Bombs[a][b].Y-Bombs[a][b].Blast;k--)
	{
		if((k<0)||(k>14)) continue;
		if(Case[Bombs[a][b].X][k].Bombe)
			if((Case[Bombs[a][b].X][k].ForceDanger==0)||(Case[Bombs[a][b].X][k].ForceDanger>m))
			{
				int xc,yc;
				Case[Bombs[a][b].X][k].ForceDanger=m;
				if(FindBombe(Bombs[a][b].X,k,&xc,&yc))
					DangerBombe(n,xc,yc,XB,YB,bn_allexpl_present);
			}
		if((k!=Bombs[a][b].Y)&&EstBloque(Bombs[a][b].X,k)) break;
		Case[Bombs[a][b].X][k].Interet+=-IBOMBE*32+IBOMBE*abs(k-Bombs[a][b].Y);
		Case[Bombs[a][b].X][k].Danger=Case[Bombs[a][b].X][k].Danger!=DNG_NONE?min(Case[Bombs[a][b].X][k].Danger,m):m;
		Case[Bombs[a][b].X][k].DngOwner=a;
	}
	return;
}

BOOL FindBombe(int xc,int yc,int* pXB,int* pYB)
{
	for(int pl=0;pl<NumPlayers;pl++)
		for(int bidx=0;bidx<NumBombs[pl];bidx++)
			if(Bombs[pl][bidx].Etat==B_PLAYING)
				if((Bombs[pl][bidx].X==xc)&&(Bombs[pl][bidx].Y==yc))
				{
					*pXB=pl;
					*pYB=bidx;
					return TRUE;
				}
	return FALSE;
}

BOOL PeutFaireExploser(int nb)
{
	DWORD	T=TickCount();
	int	P=-1;
	int scr=0;
	BOOL ak=FALSE;
	for(int cnt=0;cnt<NumBombs[nb];cnt++)
		if((Bombs[nb][cnt].Etat==B_PLAYING)&&(Bombs[nb][cnt].NSpr==BB_RETSPR)&&(Bombs[nb][cnt].LastGTC<T))
		{
			T=Bombs[nb][cnt].LastGTC;
			P=cnt;
		}

	if(P<0)
		return FALSE;

	memcpy(lCase,Case,20*15*sizeof(IACase));
	SimulExplosion(nb,P);

	for(int bidx=0;bidx<NumPlayers;bidx++)
	{
		if((Bombers[bidx].Sens==B_MORT)||(Bombers[bidx].Etat!=B_PLAYING))
			continue;
		for(int x=0;x<20;x++)
			for(int y=0;y<15;y++)
				if(Case[x][y].Blast)
					if(Dist((int)Bombers[bidx].X,(int)Bombers[bidx].Y,x*32+16,y*32+16)<HIT_DIST)
					{
						if(Case[x][y].BlastOwner!=nb)
						{
							if(bidx==Case[x][y].BlastOwner)
								scr+=Settings.GO.AKPNT;
							else
								scr-=Settings.GO.MORTPNT;
						}
						else
						{
							if(bidx==nb)
							{
								scr-=Settings.GO.AKPNT;
								ak=TRUE;
							}
							else
								scr+=Settings.GO.MORTPNT;
						}
					}
	}

	memcpy(Case,lCase,20*15*sizeof(IACase));
	return (scr>0)||((scr==0)&&!ak);
}

BOOL PeutToutFaireExploser(int nb)
{
	return FALSE;
	BOOL ak=FALSE;
	int scr=0;
	int x,y;

	memcpy(lCase,Case,20*15*sizeof(IACase));
	for(x=0;x<NumPlayers;x++)
		for(y=0;y<NumBombs[x];y++)
			if(Bombs[x][y].Etat==B_PLAYING) 
				SimulExplosion(x,y);

	for(int bidx=0;bidx<NumPlayers;bidx++)
	{
		if((Bombers[bidx].Sens==B_MORT)||(Bombers[bidx].Etat!=B_PLAYING))
			continue;
		for(int x=0;x<20;x++)
			for(int y=0;y<15;y++)
				if(Case[x][y].Blast)
					if(Dist((int)Bombers[bidx].X,(int)Bombers[bidx].Y,x*32+16,y*32+16)<HIT_DIST)
					{
						if(Case[x][y].BlastOwner!=nb)
						{
							if(bidx==Case[x][y].BlastOwner)
								scr+=Settings.GO.AKPNT;
							else
								scr-=Settings.GO.MORTPNT;
						}
						else
						{
							if(bidx==nb)
							{
								scr-=Settings.GO.AKPNT;
								ak=TRUE;
							}
							else
								scr+=Settings.GO.MORTPNT;
						}
					}
	}

	memcpy(Case,lCase,20*15*sizeof(IACase));
	return (scr>0)||((scr==0)&&!ak);
}

BOOL ADistance(int XBr,int YBr,int X,int Y,int DistMax)
{
	int XB,YB;
	XB=XBr/32;
	YB=YBr/32;
	if((XB!=X)&&(YB!=Y))
		return FALSE;
	if((XB==X)&&(YB==Y))
		return TRUE;
	if(YB==Y)
	{
		if(XB>X)
		{
			for(int px=X+1;px<XB;px++)
				if(Case[px][YB].Bloque)
					return FALSE;
			if(XB-X==1+DistMax)
				if(Dist(XBr,YBr,16+(XB-1)*32,16+YB*32)<HIT_DIST)
					return TRUE;
		}
		else
		if(XB<X)
		{
			for(int px=X-1;px>XB;px--)
				if(Case[px][YB].Bloque)
					return FALSE;

			if(X-XB==1+DistMax)
				if(Dist(XBr,YBr,16+(XB+1)*32,16+YB*32)<HIT_DIST)
					return TRUE;
		}

		if(abs(XB-X)>DistMax)
			return FALSE;
		return TRUE;
	}
	else
	if(XB==X)
	{
		if(YB>Y)
		{
			for(int py=Y+1;py<YB;py++)
				if(Case[XB][py].Bloque)
					return FALSE;
			if(YB-Y==1+DistMax)
				if(Dist(XBr,YBr,16+XB*32,16+(YB-1)*32)<HIT_DIST)
					return TRUE;
		}
		else
		if(YB<Y)
		{
			for(int py=Y-1;py>YB;py--)
				if(Case[XB][py].Bloque)
					return FALSE;
			if(Y-YB==1+DistMax)
				if(Dist(XBr,YBr,16+XB*32,16+(YB+1)*32)<HIT_DIST)
					return TRUE;
		}
		if(abs(YB-Y)>DistMax)
			return FALSE;
		return TRUE;
	}

	return TRUE;
}

BOOL EstSol(int XB,int YB)
{
	if((XB<0)||(XB>19)) return FALSE;
	if((YB<0)||(YB>14)) return FALSE;
	return (!Case[XB][YB].Bombe)&&(!Case[XB][YB].Bloque)&&(Blocs[XB][YB].Type==SOL);
}

BOOL CulDeSac(int XB,int YB,int dx,int dy,int BlastBomb)
{
	BOOL cds=0;
	int e=0;
	if(dx!=0) 
		dx=int(_copysign(1,dx));
	if(dy!=0) 
		dy=int(_copysign(1,dy));
	if(BlastBomb==0) return 0;

	if(dx!=0)
	{
		int px=XB;
		do
		{
			px+=dx;
			if(EstSol(px,YB))
			{
				if(EstSol(px,YB+1)||EstSol(px,YB-1))
					e++;
			}
			else
				cds=TRUE;
		}
		while((abs(px-XB)<BlastBomb)&&!cds);
		if(!cds)
			if(EstSol(XB+(BlastBomb+1)*dx,YB))
				e++;
		if(e==0)
			cds=TRUE;
		else
			cds=FALSE;
	}
	else
		if(dy!=0)
		{
		int py=YB;
		do
		{
			py+=dy;
			if(EstSol(XB,py))
			{
				if(EstSol(XB+1,py)||EstSol(XB-1,py))
					e++;
			}
			else
				cds=TRUE;
		}
		while((abs(py-YB)<BlastBomb)&&!cds);
		if(!cds)
			if(EstSol(XB,YB+(BlastBomb+1)*dy))
				e++;
		if(e==0)
			cds=TRUE;
		else
			cds=FALSE;
		}
		else
			cds=FALSE;

	return cds;
}

void AppuieTouches(int n,int DirX,int DirY,int XB,int YB,int NrT)
{
	const int LimitEcart=4;
	if ((XB>0)&&(XB<19)&&(YB>0)&&(YB<14))
	{
		if(DirX>0)
			if((DirY<0)&&(Bombers[n].X-XB*32<LimitEcart)&&Case[XB][YB-1].Bombe)
				DirY=0;

		if(DirX>0)
			if((DirY>0)&&(Bombers[n].X-XB*32<LimitEcart)&&Case[XB][YB+1].Bombe)
				DirY=0;

		if(DirX<0)
			if((DirY<0)&&(Bombers[n].X-XB*32>-LimitEcart)&&Case[XB][YB-1].Bombe)
				DirY=0;

		if(DirX<0)
			if((DirY>0)&&(Bombers[n].X-XB*32>-LimitEcart)&&Case[XB][YB+1].Bombe)
				DirY=0;


		if(DirY>0)
			if((DirX<0)&&(Bombers[n].Y-YB*32<LimitEcart)&&Case[XB-1][YB].Bombe)
				DirX=0;

		if(DirY>0)
			if((DirX>0)&&(Bombers[n].Y-YB*32<LimitEcart)&&Case[XB+1][YB].Bombe)
				DirX=0;

		if(DirY<0)
			if((DirX<0)&&(Bombers[n].Y-YB*32>-LimitEcart)&&Case[XB-1][YB].Bombe)
				DirX=0;

		if(DirY<0)
			if((DirX>0)&&(Bombers[n].Y-YB*32>-LimitEcart)&&Case[XB+1][YB].Bombe)
				DirX=0;
	}

		if(DirX>0)
			if(PeutAvancer(n,XB,YB,1,0))
				RIGHTlck[NrT]=TRUE;

		if(DirX<0)
			if(PeutAvancer(n,XB,YB,-1,0))
				LEFTlck[NrT]=TRUE;

		if(DirY>0)
			if(PeutAvancer(n,XB,YB,0,1))
				DOWNlck[NrT]=TRUE;

		if(DirY<0)
			if(PeutAvancer(n,XB,YB,0,-1))
				UPlck[NrT]=TRUE;

		if(rand()%2)
		{
			if ((LEFTlck[NrT]==TRUE)||(RIGHTlck[NrT]==TRUE))
				UPlck[NrT]=DOWNlck[NrT]=0;
		}
		else
		{
			if ((UPlck[NrT]==TRUE)||(DOWNlck[NrT]==TRUE))
				LEFTlck[NrT]=RIGHTlck[NrT]=0;
		}
return;
}

void DelayedKeypress(int NrT)
{
	if(BotLatency==0)
		return;
	if((RIGHTlck[NrT]!=lRIGHTlck[NrT])
		||(LEFTlck[NrT]!=lLEFTlck[NrT])
		||(UPlck[NrT]!=lUPlck[NrT])
		||(DOWNlck[NrT]!=lDOWNlck[NrT])
		||(F1lck[NrT]!=lF1lck[NrT])
		||(F2lck[NrT]!=lF2lck[NrT]))
	{
		if(timernkey[NrT]==0)
		{
			timernkey[NrT]=TickCount()+BotLatency+int(pow(rand()%BotLatency,2)/BotLatency);
			RIGHTlck[NrT]=0;
			LEFTlck[NrT]=0;
			UPlck[NrT]=0;
			DOWNlck[NrT]=0;
			F1lck[NrT]=0;
			F2lck[NrT]=0;
		}
		else
			if(TickCount()<timernkey[NrT])
			{
				RIGHTlck[NrT]=0;
				LEFTlck[NrT]=0;
				UPlck[NrT]=0;
				DOWNlck[NrT]=0;
				F1lck[NrT]=0;
				F2lck[NrT]=0;
			}
			else
			{
				lRIGHTlck[NrT]=RIGHTlck[NrT]; //keypress ici
				lLEFTlck[NrT]=LEFTlck[NrT];
				lUPlck[NrT]=UPlck[NrT];
				lDOWNlck[NrT]=DOWNlck[NrT];
				lF1lck[NrT]=F1lck[NrT];
				lF2lck[NrT]=F2lck[NrT];
				timernkey[NrT]=0;
			}
	}
	else
		timernkey[NrT]=0;
	return;
}

/*
BOOL IsBonusPresent(int bonusID)
{
	for(int cx=0;cx<20;cx++)
		for(int cy=0;cy<15;cy++)
			if((Blocs[cx][cy].Type==SOL)&&(Blocs[cx][cy].TypeBonus==bonusID))
				return TRUE;
	return FALSE;
}
*/
BOOL PeutPoserBombe(int n,int XB,int YB,int dx,int dy)
{
	if((Case[XB][YB].PoseBombe==0)||(Bombers[n].NbBomb<=0))
		return FALSE;
	if((dx!=0)||(dy!=0))
	{
		if(CulDeSac(XB,YB,dx,dy,Bombers[n].Blast))
			return FALSE;
	}
	else
	{
		if(CulDeSac(XB,YB,-1,0,Bombers[n].Blast))
			if(CulDeSac(XB,YB,1,0,Bombers[n].Blast))
				if(CulDeSac(XB,YB,0,-1,Bombers[n].Blast))
					if(CulDeSac(XB,YB,0,1,Bombers[n].Blast))
						return FALSE;
	}
	return TRUE;
}

BOOL InteretEgaux(int XB,int YB)
{
	if((XB==0)||(XB==19)||(YB==0)||(YB==14))
		return FALSE;
	int icmp=Case[XB-1][YB].Interet;
	if(Case[XB-1][YB].Danger!=0)
		return FALSE;
	if((Case[XB+1][YB].Interet==icmp)&&!Case[XB+1][YB].Danger)
		if((Case[XB][YB+1].Interet==icmp)&&!Case[XB][YB+1].Danger)
			if((Case[XB][YB-1].Interet==icmp)&&!Case[XB][YB-1].Danger)
				return TRUE;

	return FALSE;
}

void DirAleat(int XB,int YB,int* pdx,int* pdy)
{
	if(rand()%2)
	{
		*pdx=(int)_copysign(1,rand()%4-1.5);
		*pdy=0;
	}
	else
	{
		*pdx=0;
		*pdy=(int)_copysign(1,rand()%4-1.5);
	}
	return;
}

void FaitExploser(int nb,int NrT)
{
	F2lck[NrT]=TRUE;
	DWORD T=TickCount();
	int	P=-1;
	int scr=0;
	for(int cnt=0;cnt<NumBombs[nb];cnt++)
		if((Bombs[nb][cnt].Etat==B_PLAYING)&&(Bombs[nb][cnt].NSpr==BB_RETSPR)&&(Bombs[nb][cnt].LastGTC<T))
		{
			T=Bombs[nb][cnt].LastGTC;
			P=cnt;
		}

	if(P<0)
		return;

	SimulExplosion(nb,P);
	return;
}
#endif
