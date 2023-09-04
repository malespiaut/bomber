#define WIN32_LEAN_AND_MEAN

#pragma warning( disable : 4514 )
#pragma warning( disable : 4725 )//Bug Pentium
//#pragma warning( disable : 4201 )
//#include <stdio.h>
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

#include <math.h>

#define EVarie
//#undef EVarie
#define EVarieFast
#undef EVarieFast
#define CAncien 3
#define CDec 2
#define DAncien 6
#define DDec 3
#define ExpandASM
//#undef ExpandASM
#define ExpandASMU
//#undef ExpandASMU

#define LITBLOCBITS(Zone,WPOS) (*((unsigned int *)((Zone)+((WPOS)>>3)))>>((WPOS)&7))


float DPI,PI;//,MPI;
short CoproState;

void __stdcall INITFFT(void)
{
	SetCoproStateFirst
	__asm
	{
		fldpi
		fst float ptr PI
		fld st
		fadd
		fstp float ptr DPI
	}
//	DPI=(float)8*(float)atan(1);
//	PI=(float)4*(float)atan(1);
//	MPI=(float)2*(float)atan(1);

#ifdef UseFFT4ASM
	fftinit();
#endif
#ifdef UseFFT4C
	fftinit();
#endif
#ifdef UseFFTW
	fftwinit();
#endif

	ResetCoproState
	return;
}

void __stdcall SFFT(complex * D,long n)
{
	SetCoproState
#ifdef UseFFT4ASM
	fftn(D,n);
#endif
#ifdef UseFFT4C
	fftn(D,n);
#endif
#ifdef UseFFTW
	fftwn(D,n);
#endif

	ResetCoproState
	return;
}

void __stdcall SFFTSWAP(complex * D,long NrTMax,long TMax)
{
#ifdef UseFFT4ASM
	fftswap(D,NrTMax,TMax);
#endif
#ifdef UseFFT4C
	fftswap(D,TMax);
#endif
	return;
}

/*
unsigned int __inline LITBLOCBITS(char * Zone,unsigned int WPOS)
{
	return *((unsigned int *)(Zone+(WPOS>>3)))>>(WPOS&7);
}
*/

#ifdef ExpandASMU

long __stdcall Expand(complex * TFFT1,unsigned long * Enveloppe,float * TEXC,float * TEXS,float * M1,unsigned  long MXA,unsigned long BPA,unsigned long MAXIF,unsigned  long PPP, char * EcrFT,unsigned long BTST,unsigned long MST)
{
	unsigned long WPOS;
	unsigned long EvRef=1;
	{
	int VUNDEMI=0x3f000000;
	unsigned long MSA,PTEnveloppe,Compteur;

	SetCoproState

__asm
	{	
		mov eax,MXA
		mov edi,TFFT1
		mov esi,Enveloppe
		mov PTEnveloppe,esi
		fld float ptr VUNDEMI
		dec eax
		fld1
		mov MSA,eax
		fild DWORD ptr MXA
		mov eax,M1
		fdiv//invMXA
		fld float ptr [eax]//M1

		//mov Compteur,eax

		fld [edi]
		mov ecx,MAXIF
		fld [edi+4]
		fxch

		xor esi,esi
fwmf:
		mov eax,esi
		mov edx,EcrFT
		shr eax,3
		mov Compteur,ecx
		mov ecx,esi
		mov eax,[eax+edx]//EcrFT+WPOS>>3
		and cl,7//WPOS and 7
		mov ebx,MSA//Masque ampl
		shr eax,cl//Valeur remise en place
		fstp float ptr [edi]//TFFT.RE
		and ebx,eax//ampl
		fstp float ptr [edi+4]//TFFT.IM
		jz nul

		mov DWORD ptr [edi+8],ebx//ampl
		push eax//sauve Valeur remise en place
		mov cl,byte ptr BPA	//BPA  
		fild DWORD ptr [edi+8]   //ampl M1 invMAX VUNDEMI
		mov eax,CAncien	//CAncien	
		fadd st,st(3)   //ampl+.5 M1 invMAX VUNDEMI
		shl eax,cl  //CAncien<<BPA
		fmul st,st(2)  //(ampl+.5)*invMAX M1 invMAX VUNDEMI
		mov edx,PTEnveloppe //enveloppe
		add eax,ebx //(CAncien<<BPA)+ampl=1+K
		fild DWORD ptr [edx] //ANCIENNEenveloppe (Temp+.5)*invMAX M1 invMAX VUNDEMI
		imul eax,DWORD ptr [edx]//(1+K)*enveloppe
		add cl,CDec//BPA+CDec
		fmul st,st(2) //M1*enveloppe (Temp+.5)*invMAX M1 invMAX VUNDEMI
		fxch //(Temp+.5)*invMAX M1*enveloppe M1 invMAX VUNDEMI
		shr eax,cl//(1+K)*enveloppe>>(BPA+CDec)
		fmul st,st//((Temp+.5)*invMAX)^2 M1*enveloppe M1 invMAX VUNDEMI
		cmp eax,EvRef //enveloppe plus grande?
		mov [edx],eax//NOUVELLE enveloppe
		ja sup1
nosup1:
		add esi,BTST//increment du nombre de bits requis
		pop eax		//ampl
		mov cl,byte ptr BPA//BPA
		fmul //M1*enveloppe*((Temp+.5)*invMAX)^2 M1 invMAX VUNDEMI
		and eax,MST//masque la phase
		add edi,8
		mov ebx,TEXC//cos
		shr eax,cl//edx=phase
		mov ecx,Compteur
		add edx,4 //nveau pt enveloppe
		fld st//a a M1 invMAX VUNDEMI
		fmul float ptr [ebx+4*eax]//ca a M1
		fxch// st(1)//a ca m1
		mov ebx,TEXS //sin
	dec ecx
		fmul float ptr [ebx+4*eax] //sa ca m1 invMAX VUNDEMI
		fxch// st(1)//ca sa m1
		mov PTEnveloppe,edx//nveau pt enveloppe
	jnz fwmf
	jmp FiniD

sup1:
	mov EvRef,eax
	jmp nosup1


nul:
		mov ebx,PTEnveloppe//enveloppe
		mov eax,CAncien//coeff lowpass
		add esi,BPA//increment du nombre de bits requis
		imul eax,[ebx]//CAncien*Enveloppe
		fldz
		shr eax,CDec//CAncien*Enveloppe/(2^CDec)
		fldz
		add edi,8//TFFT suivant
		cmp eax,EvRef//est-ce une plus grande enveloppe?
		mov [ebx],eax//nvelle env
		ja sup0
nosup0:
		mov ecx,Compteur
		add ebx,4 //nouveau PTEnveloppe
		dec ecx
		mov PTEnveloppe,ebx //nouveau PTEnveloppe
		jnz fwmf
		jmp FiniD

sup0:
	mov EvRef,eax
	jmp nosup0

FiniD:
		fstp float ptr [edi]//TFFT.RE
		fstp float ptr [edi+4]//TFFT.IM

		mov WPOS,esi
		fstp st
		mov edi,MAXIF
		fstp st
		mov ecx,PPP
		fstp st
		inc edi
		xor eax,eax
		sub ecx,edi
		shl edi,3
		add ecx,ecx
		add edi,TFFT1
		rep stosd

	}
	}
	
#include "TraiteEnv.cpp"

	ResetCoproState
	return WPOS;
}

#elif defined ExpandASM

long __stdcall Expand(complex * TFFT1,unsigned long * Enveloppe,float * TEXC,float * TEXS,float * M1,unsigned  long MXA,unsigned long BPA,unsigned long MAXIF,unsigned  long PPP, char * EcrFT,unsigned long BTST,unsigned long MST)
{
	unsigned long WPOS;
	unsigned long EvRef=1;
	{
	int VUNDEMI=0x3f000000;
	unsigned long MSA,PTEnveloppe,Compteur;

	SetCoproState

__asm
	{	
		mov eax,MXA
		mov edi,TFFT1
		mov esi,Enveloppe
		mov PTEnveloppe,esi
		fld float ptr VUNDEMI
		dec eax
		fld1
		mov MSA,eax
		fild DWORD ptr MXA
		mov eax,M1
		fdiv//invMXA
		fld float ptr [eax]//M1

		//mov Compteur,eax

		fld [edi]
		mov ecx,MAXIF
		fld [edi+4]
		fxch

		xor esi,esi
fwmf:
		mov eax,esi
		mov edx,EcrFT
		shr eax,3
		mov Compteur,ecx
		mov ecx,esi
		mov eax,[eax+edx]//EcrFT+WPOS>>3
		and cl,7//WPOS and 7
		mov ebx,MSA//Masque ampl
		shr eax,cl//Valeur remise en place
		fstp float ptr [edi]//TFFT.RE
		and ebx,eax//ampl
		fstp float ptr [edi+4]//TFFT.IM
		jz nul

		mov DWORD ptr [edi+8],ebx//ampl
		push eax//sauve Valeur remise en place
		mov cl,byte ptr BPA	//BPA  
		fild DWORD ptr [edi+8]   //ampl M1 invMAX VUNDEMI
		mov eax,CAncien	//CAncien	
		fadd st,st(3)   //ampl+.5 M1 invMAX VUNDEMI
		shl eax,cl  //CAncien<<BPA
		mov edx,PTEnveloppe //enveloppe
		fmul st,st(2)  //(ampl+.5)*invMAX M1 invMAX VUNDEMI
		add eax,ebx //(CAncien<<BPA)+ampl=1+K
		add esi,BTST//increment du nombre de bits requis
		imul eax,DWORD ptr [edx]//(1+K)*enveloppe
		fmul st,st//((Temp+.5)*invMAX)^2 M1 invMAX VUNDEMI
		add cl,CDec//BPA+CDec
		fild DWORD ptr [edx] //ANCIENNEenveloppe ((Temp+.5)*invMAX)^2 M1 invMAX VUNDEMI
		shr eax,cl//(1+K)*enveloppe>>(BPA+CDec)
		fmul st,st(2) //M1*enveloppe ((Temp+.5)*invMAX)^2 M1 invMAX VUNDEMI
		cmp eax,EvRef //enveloppe plus grande?
		mov [edx],eax//NOUVELLE enveloppe
		ja sup1
nosup1:
		pop eax		//ampl
		mov cl,byte ptr BPA//BPA
		fmul //M1*enveloppe*((Temp+.5)*invMAX)^2 M1 invMAX VUNDEMI
		and eax,MST//masque la phase
		add edi,8
		mov ebx,TEXC//cos
		shr eax,cl//edx=phase
		mov ecx,Compteur
		add edx,4 //nveau pt enveloppe
		fld st//a a M1 invMAX VUNDEMI
		fmul float ptr [ebx+4*eax]//ca a M1
		fxch// st(1)//a ca m1
		mov ebx,TEXS //sin
	dec ecx
		fmul float ptr [ebx+4*eax] //sa ca m1 invMAX VUNDEMI
		fxch// st(1)//ca sa m1
		mov PTEnveloppe,edx//nveau pt enveloppe
	jnz fwmf
	jmp FiniD

sup1:
	mov EvRef,eax
	jmp nosup1


nul:
		mov ebx,PTEnveloppe//enveloppe
		mov eax,CAncien//coeff lowpass
		add esi,BPA//increment du nombre de bits requis
		imul eax,[ebx]//CAncien*Enveloppe
		fldz
		shr eax,CDec//CAncien*Enveloppe/(2^CDec)
		fldz
		add edi,8//TFFT suivant
		cmp eax,EvRef//est-ce une plus grande enveloppe?
		mov [ebx],eax//nvelle env
		ja sup0
nosup0:
		mov ecx,Compteur
		add ebx,4 //nouveau PTEnveloppe
		dec ecx
		mov PTEnveloppe,ebx //nouveau PTEnveloppe
		jnz fwmf
		jmp FiniD

sup0:
	mov EvRef,eax
	jmp nosup0

FiniD:
		fstp float ptr [edi]//TFFT.RE
		fstp float ptr [edi+4]//TFFT.IM

		mov WPOS,esi
		fstp st
		mov edi,MAXIF
		fstp st
		mov ecx,PPP
		fstp st
		inc edi
		xor eax,eax
		sub ecx,edi
		shl edi,3
		add ecx,ecx
		add edi,TFFT1
		rep stosd

	}
	}
	
#include "TraiteEnv.cpp"

	ResetCoproState
	return WPOS;
}

#else

long __stdcall Expand(complex * TFFT1,unsigned long * Enveloppe,float * TEXC,float * TEXS,float * M1,unsigned  long MXA,unsigned long BPA,unsigned long MAXIF,unsigned  long PPP, char * EcrFT,unsigned long BTST,unsigned long MST)
{
	unsigned long WPOS=0;
	unsigned long EvRef=1;
	float invMXA=(float)1.0/(float)(MXA);

	{
	for (unsigned long W=0;W<MAXIF;W++)
	{

		unsigned long VL=(LITBLOCBITS(EcrFT,WPOS)&MST);
		unsigned long CLONG;
		if ((CLONG=(VL&(MXA-1)))==0)//WARNING EXPRES
		{

			WPOS+=BPA;
			TFFT1[W+1].re=0;
			TFFT1[W+1].im=0;
			Enveloppe[W]=((Enveloppe[W]*CAncien)>>CDec);
			if (Enveloppe[W]>EvRef) EvRef=Enveloppe[W];
		}
		else
		{
			WPOS+=BTST;
			float Temp=((float)CLONG+(float)0.5)*invMXA;
			Temp*=Temp*(*M1)*(float)Enveloppe[W];
			Enveloppe[W]=((Enveloppe[W]*(CLONG+(CAncien<<BPA)))>>(CDec+BPA));
			TFFT1[W+1].re=TEXC[VL>>=BPA]*Temp;
			TFFT1[W+1].im=TEXS[VL]*Temp;
			if (Enveloppe[W]>EvRef) EvRef=Enveloppe[W];
		}
	}
	}

	{
	for (unsigned long W=(unsigned long)(TFFT1+MAXIF);W<(unsigned long)(TFFT1+PPP);W+=8)
	{
		((complex *)W)->re=0;
		((complex *)W)->im=0;
	}
	}

#include "TraiteEnv.cpp"

	return WPOS;
}

#endif

long __stdcall ExpandFast(complex * TFFT1,unsigned long * Enveloppe,float * TEXC,float * TEXS,float * M1,unsigned  long MXA,unsigned long BPA,unsigned long MAXIF,unsigned  long PPP, char * EcrFT,unsigned long BTST,unsigned long MST)
{
	unsigned long WPOS;
	unsigned long EvRef=1;
	{
	unsigned long MSA,PTEnveloppe,Compteur;

	__asm
	{	
		mov eax,MXA
		mov esi,Enveloppe
		mov PTEnveloppe,esi
		dec eax
		mov MSA,eax

		mov ecx,MAXIF
		xor esi,esi
fwmf:
		mov eax,esi
		mov edx,EcrFT
		shr eax,3
		mov Compteur,ecx
		mov ecx,esi
		mov eax,[eax+edx]//EcrFT+WPOS>>3
		and cl,7//WPOS and 7
		mov ebx,MSA//Masque ampl
		shr eax,cl//Valeur remise en place
		and ebx,eax//ampl
		jz nul

		mov cl,byte ptr BPA
		mov eax,CAncien
		shl eax,cl
		mov edx,PTEnveloppe
		add eax,ebx
		imul eax,DWORD ptr [edx]
		add cl,CDec
		shr eax,cl
		cmp eax,EvRef
		ja sup1
nosup1:
		mov [edx],eax
		mov ecx,Compteur
		add edx,4
		add esi,BTST
		dec ecx
		mov PTEnveloppe,edx
		jnz fwmf
		jmp FiniF
sup1:
		mov EvRef,eax
		jmp nosup1

nul:
		mov ebx,PTEnveloppe//enveloppe
		mov eax,CAncien//coeff lowpass
		add esi,BPA//increment du nombre de bits requis
		imul eax,[ebx]//CAncien*Enveloppe
		shr eax,CDec//CAncien*Enveloppe/(2^CDec)
		add edi,8
		cmp eax,EvRef//est-ce une plus grande enveloppe?
		mov [ebx],eax
		ja sup0
nosup0:
		mov ecx,Compteur
		add ebx,4
		dec ecx
		mov PTEnveloppe,ebx
		jnz fwmf
		jmp FiniF
sup0:
		mov EvRef,eax
		jmp nosup0

FiniF:
	mov WPOS,esi
	}
	}

/*	
	for (W=0;W<MAXIF;W++)
	{
		VL=(LITBLOCBITS(EcrFT,WPOS)&MST);
		if ((CLONG=(VL&(MXA-1)))==0)
		{
#ifdef EVarie
			//TempInt=Enveloppe[W-1];
			Enveloppe[W]=((Enveloppe[W]*CAncien)>>CDec);
			if (Enveloppe[W]>EvRef) EvRef=Enveloppe[W];
			//Enveloppe[W-1]=TempInt;
#endif
			WPOS+=BPA;
		}
		else
		{
			//TempInt=Enveloppe[W-1];
#ifdef EVarie
			Enveloppe[W]=((Enveloppe[W]*(CLONG+(CAncien<<BPA)))>>(CDec+BPA));
			if (Enveloppe[W]>EvRef) EvRef=Enveloppe[W];
			//Enveloppe[W-1]=TempInt;
#endif
			WPOS+=BTST;
		}
	}

*/

#include "TraiteEnv.cpp"

	return WPOS;
}

long __stdcall Discretise(complex * TFFT2,unsigned long * Enveloppe,float * M1,unsigned long MXP,unsigned  long BPP,unsigned  long MXA,unsigned  long BPA,unsigned long MAXIF,char * EcrFT,unsigned long BTST)
{
	unsigned long W,CLONG;//,TempInt;
	//unsigned long Last1,Last2;
	
	float MD=0.0;
	float MXPSDPI=(float)MXP/DPI;
	int VUNDEMI=0x3f000000;
	complex * TFFTW=TFFT2;
	
	SetCoproState

	/*
	for (W=0;W<MAXIF;W++)
	{
		TFFTW++;
		Ctemp.re =(float)sqrt(TFFTW->re*TFFTW->re + TFFTW->im*TFFTW->im)/(float)Enveloppe[W];
		Ctemp.im=(float)atan(TFFTW->im/TFFTW->re);
		*/
		__asm
		{
			mov eax,TFFTW
			fld1
			fdiv DPI
			mov ebx,Enveloppe
			fld1
			mov ecx,MAXIF

			fld VUNDEMI
			FIDIV MXP
			fld1
			fadd
			fld1
			fadd

ReBcl:
			add eax,8
			fld float ptr [eax]		//re
			fld float ptr [eax+4]	//im re
			fld st(1)				//re im re
			fmul st,st(2)			//re² im re
			fld st(1)				//im re² im re
			fmul st,st(2)			//im² re² im re
			fadd
			fsqrt					//m im re //2.0n 1 1/dpi
			
			fidiv int ptr [ebx]
			add ebx,4

			fstp float ptr [eax]
			fxch
			fpatan		//angle 2.0n 1 1/dpi
			mov edx,[eax]
			cmp MD,edx
			fst float ptr [eax+4]
			jae NoNewMD
			mov  MD,edx
NoNewMD:
			fmul st,st(3) //angle 2.0n 1 1/dpi
			test DWORD ptr [eax+4],0x80000000
			fadd st,st(1) //2+1/(2MXP)
			jz supz1
			fadd st,st(2) //1
supz1:
			dec ecx
			fstp float ptr [eax+4]
			jnz ReBcl

			fstp ST
			fstp ST
			fstp ST
		}
/*
		if (TFFTW->re<0) Ctemp.im+=PI;
		if (Ctemp.im<0) Ctemp.im+=DPI;
		*TFFTW=Ctemp;
		if (TFFTW->re>MD) 
		{
			MD=TFFTW->re;
		}
	}
*/
	if (MD==0.0) MD=1.0;
	*M1=MD;
	float MXAp2MD=(float)(MXA)*(float)(MXA)/MD;
	
	unsigned long WPOS=0;
	unsigned long EvRef=1;
	int Decal=23-BPP-1;
	int MSP=MXP-1;
	
	TFFTW=TFFT2;
	
	for (W=0;W<MAXIF;W++)
	{
		
		//CLONG=(long)floor(sqrt((++TFFTW)->re*MXAp2MD));
		__asm
		{
			mov eax,TFFTW
			add eax,8
			fld float ptr MXAp2MD
			fmul float ptr [eax]
			fsqrt
			mov TFFTW,eax
			fsub VUNDEMI//probleme si negatif
			fistp CLONG

				mov ecx,BPA
				mov ebx,CLONG
/*
				cmp ebx,0 //long
				jg NonNul
				jz NonPlante
				push eax
				xor eax,eax
				mov eax,dword ptr [eax]
				pop eax
				NonPlante:
*/
				or ebx,ebx //pb si ech nul
				jnz NonNul

				add WPOS,ecx
				jmp Ok
NonNul:
			mov edx,MXA
				mov eax,[eax+4]
			xor edx,ebx//CMC; SBB EBX,0;
				mov cl,BYTE ptr Decal
			setz dl
				shr eax,cl
			and edx,1
				and eax,MSP
			sub ebx,edx
				mov cl,BYTE ptr BPA
			mov edx,WPOS
				shl eax,cl
			mov CLONG,ebx //verrouillé de courte date
			mov cl,dl
			add eax,ebx
			and cl,7
			shr edx,3
			shl eax,cl
			add edx,EcrFT
			mov ebx,BTST
			or [edx],eax
			add WPOS,ebx
				
Ok:
		}
#ifdef EVarie
		Enveloppe[W]=((Enveloppe[W]*(CLONG+(CAncien<<BPA)))>>(CDec+BPA));
		if (Enveloppe[W]>EvRef) EvRef=Enveloppe[W];
#endif
	}
	
	
	
/*	
#ifdef EVarie
	TempInt=(unsigned long)(Enveloppe+MAXIF);
	for (W=(unsigned long)Enveloppe;W<TempInt;W+=4)
		*(unsigned long *)W=*(unsigned long *)W*65534/EvRef+1;
	
	Enveloppe[0]=((Enveloppe[0]+Enveloppe[1])>>1)+1;
	Enveloppe[MAXIF-1]=((Enveloppe[MAXIF-1]+Enveloppe[MAXIF-2])>>1)+1;
	Last1=Enveloppe[0];
	Last2=Enveloppe[1];
	for (W=1;W<MAXIF-1;W++)
	{
		Enveloppe[W]=((Last1+DAncien*Last2+Enveloppe[W+1])>>DDec);
		Last1=Last2;
		Last2=Enveloppe[W+1];
	}
#endif
*/
	#include "TraiteEnv.cpp"
	
	ResetCoproState
	return WPOS;
}
