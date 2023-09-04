#ifndef _iIA_H_
#define _iIA_H_
int NumCaisses(void);
BOOL CulDeSac(int XB,int YB,int dx,int dy,int BlastBomb);
void AppuieTouches(int n,int DirX,int DirY,int XB,int YB,int NrT);
void BougeAleat(int n,int XB,int YB,int NrT);
BOOL ADistance(int XBr,int YBr,int X,int Y,int DistMax);
BOOL InteretEgaux(int XB,int YB);
void DirAleat(int XB,int YB,int* pdx,int* pdy);
BOOL EstSol(int XB,int YB);
void GenereCase(int n);
BOOL PeutFaireExploser(int nb);
BOOL PeutAvancer(int n,int XB,int YB,int dx,int dy);
BOOL PeutPoserBombe(int n,int XB,int YB,int dx,int dy);
BOOL PeutToutFaireExploser(int nb);
void EraseData(int XB,int YB);
void SimulExplosion(int a,int b);
void FaitExploser(int nb,int NrT);
void DangerBombe(int n,int a,int b,int XB,int YB,BOOL bn_allexpl_present);
BOOL FindBombe(int xc,int yc,int* pXB,int* pYB);
void GenereCaisse(int x,int y, int blast);
void DelayedKeypress(int NrT);

const int ICAISSE=9;
const int IBOMBE=60;
const int IBOMBER=ICAISSE+6;
const int TICK_IA=350;
const int TICK_ALEAT=500;
const int TICK_IATOUT=15000;

const int DNG_NONE=0;
const int DNG_LOW=11;
const int DNG_MED=8;
const int DNG_HIGH=5;
const int DNG_MAX=1;

struct IACase
{
	int Interet;
	int Danger;
	int PoseBombe;
	BOOL Bloque;
	int DngOwner;
	BOOL Bombe;
	BOOL Blast;
	int BlastOwner;
	int ForceDanger;
};
#endif
