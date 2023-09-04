//#define WIN32_LEAN_AND_MEAN
//#pragma warning( disable : 4201 )
//#pragma warning( disable : 4514 )

//#include <windows.h>
//#include <stdio.h>
#include <math.h>
#include <memory.h>

#include "WhatFFT.h"

#ifdef UseFFT4ASM
#include "fft4asm.h"
#endif
#ifdef UseFFT4C
#include "fft4c.h"
#endif
#ifdef UseFFTW
#include "fftwcompat.h"
#endif

#include "FourierFloat.h"
#include "CSon.h"

#define DELETE_ARRAY(lp) {if(lp){delete[]lp;lp=0;}}
#define DELETE(lp) {if(lp){delete lp;lp=0;}}
#define QualiteSampling 16

int PPP0=16384;
int RECOUVR=32;
int NCH;
//int SAMPLERATE;
int BPS;
int BPA;// = 1 + (Ent.Odc And 15)
int BPP;// = 1 + ((Ent.Odc \ 16) And 15)
int METH;// = 1 + ((Ent.Odc \ 256) And 3)
int BDD;// = (Ent.Odc \ 1024) And 1
int SURC;// = (Ent.Odc \ 2048) And 3     'surcompression
int STER;// = (Ent.Odc \ 8192) And 1
int CHEV;// = (Ent.Odc \ 16384) And 1   'chevauchement
int RCHEV;// = RECOUVR * CHEV
int PPP;// = PPP0 \ (4 ^ (METH - 1))
int PPPRC;// = PPP - RCHEV
int BT;// = BPP + BPA
int MXP;// = 2 ^ BPP
int MXA;// = 2 ^ BPA
int MST;// = 2 ^ BT - 1
complex* TFFT;
complex* TFFTEXTRA;
//char* LuM=0;
char* EcrFT;
float* TEXC;
float* TEXS;
float* TCSR;
unsigned long*Enveloppe;
//char* FBuf;
int LONGENR;
int MAXIF;
//int PosInLuM;
//int LenLuM;
//int NLONG;
//int PIF;
//int PIFinit;
//int TrueLenFT;
//int PosBuf;
float SeuilAmplitudeEmit,LastRatio;
float Convolution[QualiteSampling];

static float PI;
static float DPI;

#if 0
typedef struct
{
	short int ChaineIdent;
	long Odc;
	long TailleInit;
	long NumBlocs;
} EnteteFt;


/*
Type WAVEFORMAT
wFormatTag As Integer
nChannels As Integer
nSamplesPerSec As Long
nAvgBytesPerSec As Long
nBlockAlign As Integer
wBitsPerSample As Integer
End Type    '16
*/

typedef struct
{
	short int wFormatTag;
	short int nChannels;
	long SamplesPerSec;
	long nAvgBytesPerSec;
	short int nBlockAlign;
	short int wBitsPerSample;
} xWAVEFORMAT;

/*
Type ENTETEWAV
SRIFF As String * 4
TM8 As Long
SWAVE As String * 4
SFMT As String * 4
TEnt As Long
WVF As WAVEFORMAT '16
SDATA As String * 4
LGData As Long
End Type    '44
*/

typedef struct
{
	long SRIFF;
	long TM8;
	long SWAVE;
	long SFMT;
	long TEnt;
	xWAVEFORMAT WVF;
	long SDATA;
	long LGData;
} ENTETEWAV;
#endif

//Initialisation
void FT_Start(long*SizeFT,long*SizeWAV)
{
	int N,Channel,W;
	//ULONG rien;
	INITFFT();
	PI=(float)(4.0*atan(1));
	DPI=2*PI;
	BPA = 2;//1 + (Ent.Odc&15);
	BPP = 2;//1 + ((Ent.Odc>>4) & 15);
	METH = 3;//1 + ((Ent.Odc>>8) & 3);
	BDD = 1;//(Ent.Odc>>10) & 1;
	SURC = 3;//(Ent.Odc>>11) & 3;//     'surcompression
	STER = 0;//(Ent.Odc>>13) & 1;
	CHEV = 0;//1;//(Ent.Odc>>14) & 1;//   'chevauchement
	RCHEV = RECOUVR * CHEV;
	//SAMPLERATE=EnWAV.WVF.SamplesPerSec;
	NCH=1+STER;
	BPS=8*(1+BDD);
	
	PPP = PPP0 / (1<<(2*(METH - 1)));
	PPPRC = PPP - RCHEV;
	BT = BPP + BPA;
	MXP = 1<<BPP;
	MXA = 1<<BPA;
	MST = (1<<BT) - 1;

	DELETE_ARRAY(TFFT);
	TFFT=new complex[PPP];

	DELETE_ARRAY(TFFTEXTRA);
	if (CHEV) 
		TFFTEXTRA=new complex[(1+STER)*RECOUVR];

	*SizeWAV=(1+STER)*(1+BDD)*PPPRC;
	/*
	LenLuM=(1+STER)*(1+BDD)*PPPRC;
	if (LuM) GlobalFree(LuM);
	LuM=(char*)GlobalAlloc(GPTR,LenLuM);
	*/

	/*
	wsprintf(mstr,"MXP %ld MXA %ld RECOUVR %ld",
	MXP,    MXA,    RECOUVR);
	MessageBox(mod.hMainWindow,mstr,"FT",0);
	*/
	
	DELETE_ARRAY(TEXC);
	TEXC=new float[MXP];
	DELETE_ARRAY(TEXS);
	TEXS=new float[MXP];
	DELETE_ARRAY(TCSR);
	TCSR=new float[RECOUVR];
	
	for (N=0;N<MXP;N++)
	{
		TEXC[N]=(float)cos(DPI*N/(float)MXP);
		TEXS[N]=(float)-sin(DPI*N/(float)MXP);
	}
	
	for (N=0;N<RECOUVR;N++)
	{
		TCSR[N]=(float)(0.5+0.5*cos(PI*(N+1)/(float)(RECOUVR+1)));
	}
	
	if (CHEV)
	{
		if (BDD==0)
		{
			for (N=0;N<(1+STER)*RECOUVR;N++)
			{
				TFFTEXTRA[N].re=128;
				TFFTEXTRA[N].im=0;
			}
		}
		else
		{
			for (N=0;N<(1+STER)*RECOUVR;N++)
			{
				TFFTEXTRA[N].re=0;
				TFFTEXTRA[N].im=0;
			}
		}
	}
	
	LONGENR = 8 + ((PPP / 16) * BT * (4 - SURC)) / 4;
	*SizeFT=LONGENR;
	MAXIF = (PPP * (4 - SURC)) / 8;
	
	DELETE_ARRAY(Enveloppe);
	Enveloppe=new unsigned long[MAXIF*(1+STER)];
	
	for (Channel=0;Channel<(STER+1);Channel++)
	{
		for (W=0;W<MAXIF;W++)
		{
			Enveloppe[W+MAXIF*Channel]=65535/(W+1);
		}
	}

SeuilAmplitudeEmit=(float)(1024.0/sqrt(PPP));
	
	return;
}

void FT_Restart()
{
	int W;
	{
		for (W=0;W<MAXIF;W++)
		{
			Enveloppe[W]=65535/(W+1);
		}
	}
	if (CHEV)
		memset(TFFTEXTRA,0,(1+STER)*RECOUVR*sizeof(complex));
	
}

//desallocations FT
void FT_Free()
{
	DELETE_ARRAY(TFFT);
	DELETE_ARRAY(TFFTEXTRA);
	/*
	if (LuM) GlobalFree(LuM);
	LuM=0;
	*/
	DELETE_ARRAY(TEXC);
	DELETE_ARRAY(TEXS);
	DELETE_ARRAY(TCSR);
	DELETE_ARRAY(Enveloppe);
}


//decompression
long FT_Decomp(char * BufferFTSrc,char * BufferWAVDst)
{
	int N,WPOS;
	//ULONG rien;
	float M1;
	
	FT_Restart();
	
	EcrFT=BufferFTSrc;
	TFFT[0].re=*(float*)EcrFT;
	TFFT[0].im=0;
	M1=*(float*)(EcrFT+4);
	WPOS=Expand(TFFT,Enveloppe,TEXC,TEXS,&M1,MXA,BPA,MAXIF,PPP,EcrFT+8,BT,MST);
	
	SFFT(TFFT, 8 - METH);
	SFFTSWAP(TFFT, 8 - METH, PPP);
	
	for(N=0;N<RCHEV;N++)
		TFFT[N].re=(float)(TFFT[N].re*(1.0-TCSR[N])+TFFTEXTRA[N].re*TCSR[N]);
	
	
	if (CHEV)
		memcpy(TFFTEXTRA,&TFFT[PPPRC],RCHEV*sizeof(complex));
	
	for(N=0;N<PPPRC;N++)
	{
		if (TFFT[N].re>32767)
			*(short int*)(BufferWAVDst+2*((1+STER)*N))=32767;
		else if (TFFT[N].re<-32768)
			*(short int*)(BufferWAVDst+2*((1+STER)*N))=-32768;
		else
			*(short int*)(BufferWAVDst+2*((1+STER)*N))=(short int)(TFFT[N].re);
	}
	return 8+((WPOS+7)/8);//lecture vraie dans le buffer ft (octets)
}

long FT_Comp(char * BufferWAVSrc,char * BufferFTDst)
{
	int N,WPOS;
	float M1;
	
	FT_Restart();
	
	memset(BufferFTDst,0,LONGENR);
	
	for(N=0;N<PPP;N++)
		TFFT[N].im=0;
	
	
	for(N=0;N<RCHEV;N++)
		TFFT[N]=TFFTEXTRA[N];
	
	for(N=0;N<PPPRC;N++)
	{
		TFFT[N+RCHEV].re=(( short int *)BufferWAVSrc)[N];
	}
	for(N=0;N<RCHEV;N++)
		TFFTEXTRA[N]=TFFT[N+PPPRC];
	
	SFFT(TFFT, 8 - METH);
	SFFTSWAP(TFFT, 8 - METH, PPP);
	
	WPOS = Discretise(TFFT, Enveloppe, &M1, MXP, BPP, MXA, BPA, MAXIF, BufferFTDst+8, BT);
	//*((float*)BufferFTDst)=TFFT[0].re;
	if (M1>SeuilAmplitudeEmit)
	{
		*((float*)BufferFTDst)=0;//TFFT[0].re/PPP;
		*((float*)(BufferFTDst+4))=(float)2.0*M1/PPP;//TFFT[0].re;
		
		return 8+((WPOS+7)/8);//ecriture vraie dans le buffer ft (octets)
	}
	else
		return 0;
}

void FT_StereoToMono(short * BuffSrcSter, short * BuffDstMono, int NumSamples)
{
	int i;
	for(i=0;i<NumSamples;i++)
	{
		BuffDstMono[i]=(short)(((long)BuffSrcSter[(i<<1)]+(long)BuffSrcSter[(i<<1)+1])>>1);
	}
}

void FT_MonoToStereo(short * BuffSrcMono, short * BuffDstSter, int NumSamples)
{
	int i;
	for(i=0;i<NumSamples;i++)
	{
		BuffDstSter[(i<<1)+1]=BuffDstSter[i<<1]=BuffSrcMono[i];
	}
}

long FT_ReSample(short * BuffSrcMono, short * BuffDstMono, short * BuffTmp, int NumSamples, int FreqSrc, int FreqDst)
//BuffTmp doit etre de la taille de SUP(size Src, size Dst)
//return le nombre exact de samples
//qui vaut tres exactement (calculs int): (NumSamples*FreqDst)/FreqSrc
{
	int i;
	
	if (FreqSrc==FreqDst)
	{
		memcpy(BuffDstMono,BuffSrcMono,NumSamples<<1);
		/*
		for(i=0;i<NumSamples;i++)
		{
			BuffDstMono[i]=BuffSrcMono[i];
		}
		*/
		return NumSamples;
	}
	
	else if (FreqSrc>FreqDst)
	{
		FT_CutAtRatio(BuffSrcMono,BuffTmp,NumSamples,(float)FreqDst/(float)FreqSrc);
		
		long NumSamplesDst=(NumSamples*FreqDst)/FreqSrc;
		for(i=0;i<NumSamplesDst;i++)//undersample BuffTmp
		{
			BuffDstMono[i]=BuffTmp[(i*FreqSrc)/FreqDst];
		}
		
		return NumSamplesDst;
	}
	
	else //FreqSrc<FreqDst
	{
		long NumSamplesDst=(NumSamples*FreqDst)/FreqSrc;
		for(i=0;i<NumSamplesDst;i++)//oversample BuffSrc
		{
			BuffTmp[i]=BuffSrcMono[(i*FreqSrc)/FreqDst];
		}
		BuffTmp[NumSamplesDst-1]=BuffTmp[0]=0;
		FT_CutAtRatio(BuffTmp,BuffDstMono,NumSamplesDst,(float)FreqSrc/(float)FreqDst);
		return NumSamplesDst;
	}
	
}

void FT_CutAtRatio(short * BuffIn, short * BuffOut, int NumSamples, float ratio)
{
#if (QualiteSampling==0)
	int i;
	for(i=0;i<NumSamples;i++)
	{
		BuffOut[i]=BuffIn[i];
	}
	
#else
	int i,j;
	float sum;
	if (LastRatio!=ratio)
	{
		float alpha,Normalize=0;
		Convolution[0]=1;
		PI=(float)(4.0*atan(1));
		DPI=2*PI;
		for(i=1;i<QualiteSampling;i++)
		{
			alpha=i*PI*ratio;
			Convolution[i]=(float)sin(alpha)/alpha;
			Normalize+=Convolution[i]*Convolution[i];
		}

		Normalize=1/((float)sqrt(8*Normalize+4));
		for(i=0;i<QualiteSampling;i++)
		{
			Convolution[i]*=Normalize;
		}

		LastRatio=ratio;

		
	}
	
	if (NumSamples<2*QualiteSampling)
	{
		for(i=0;i<NumSamples;i++)
		{
			sum=0.0;
			/*
			for(j=1-QualiteSampling;j<QualiteSampling;j++)
			{
			sum+=Convolution[FT_ReflectBound(j,0,QualiteSampling-1)]*BuffIn[FT_SatureBound(i+j,0,NumSamples-1)];
			}
			*/
			for(j=1-QualiteSampling;j<0;j++)
			{
				sum+=Convolution[-j]*BuffIn[FT_SatureBound(i+j,0,NumSamples-1)];
			}
			for(j=0;j<QualiteSampling;j++)
			{
				sum+=Convolution[j]*BuffIn[FT_SatureBound(i+j,0,NumSamples-1)];
			}
			if (sum>32767)
				BuffOut[i]=32767;
			else if (sum<-32768)
				BuffOut[i]=-32768;
			else
				BuffOut[i]=(short)sum;
			
		}
	}
	else
	{
		for(i=0;i<QualiteSampling;i++)
		{
			sum=0.0;
			for(j=1-QualiteSampling;j<0;j++)
			{
				sum+=Convolution[-j]*BuffIn[FT_SatureBound(i+j,0,NumSamples-1)];
			}
			for(j=0;j<QualiteSampling;j++)
			{
				sum+=Convolution[j]*BuffIn[i+j];
			}
			if (sum>32767)
				BuffOut[i]=32767;
			else if (sum<-32768)
				BuffOut[i]=-32768;
			else
				BuffOut[i]=(short)sum;
		}
		
		for(i=QualiteSampling;i<(NumSamples-QualiteSampling);i++)
		{
			sum=0.0;
			for(j=1-QualiteSampling;j<0;j++)
			{
				sum+=Convolution[-j]*BuffIn[i+j];
			}
			for(j=0;j<QualiteSampling;j++)
			{
				sum+=Convolution[j]*BuffIn[i+j];
			}
			if (sum>32767)
				BuffOut[i]=32767;
			else if (sum<-32768)
				BuffOut[i]=-32768;
			else
				BuffOut[i]=(short)sum;
		}
		
		for(i=(NumSamples-QualiteSampling);i<NumSamples;i++)
		{
			sum=0.0;
			for(j=1-QualiteSampling;j<0;j++)
			{
				sum+=Convolution[-j]*BuffIn[i+j];
			}
			for(j=0;j<QualiteSampling;j++)
			{
				sum+=Convolution[j]*BuffIn[FT_SatureBound(i+j,0,NumSamples-1)];
			}
			if (sum>32767)
				BuffOut[i]=32767;
			else if (sum<-32768)
				BuffOut[i]=-32768;
			else
				BuffOut[i]=(short)sum;
		}
	}
#endif
}

int FT_ReflectBound(int i, int Min, int Max)
{
	if (i<Min) return FT_ReflectBound(2*Min-i,Min,Max);
	if (i>Max) return FT_ReflectBound(2*Max-i,Min,Max);
	return i;
}

int FT_SatureBound(int i, int Min, int Max)
{
	if (i<Min) return Min;
	if (i>Max) return Max;
	return i;
}
