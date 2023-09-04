//FONCTION ACTUELLE PLUS RAPIDE (- de XCHG)
/*
void __inline
iFFT4G(complex *x,int astep,int tstep)
{
*/
#undef	Are
#undef	Bre
#undef	Cre
#undef	Dre
#undef	Aim
#undef	Bim
#undef	Cim
#undef	Dim

#define	Are	float ptr [eax]
#define	Aim	float ptr [eax+4]
#define	Bre	float ptr [ebx]
#define	Bim	float ptr [ebx+4]
#define	Cre	float ptr [ecx]
#define	Cim	float ptr [ecx+4]
#define	Dre	float ptr [edx]
#define	Dim	float ptr [edx+4]

	__asm
	{
	mov ebx,astep
	mov eax,x    	/* eax -> a */
#ifdef AddToX
	add eax,AddToX
#endif
	lea ecx,[eax+2*ebx]
	lea edx,[ebx+ecx]
	add ebx,eax

//					0				1		2		3		4			5
	fld Bre		//	Bre
	fld st		//	Bre		Bre
	fld Dre		//	Dre		Bre		Bre
	fsub st(2),st //Dre		Bre		Bre-Dre
	fadd		//	Dre+Bre Bre-Dre

	fld Are		//	Are			Dre+Bre	Bre-Dre
	fld st		//	Are Are		Dre+Bre	Bre-Dre
	fld Cre		//	Cre Are		Are		Dre+Bre Bre-Dre
	fadd st(2),st//	Cre Are		Are+Cre	Dre+Bre Bre-Dre
	fsub		//	Cre-Are		Are+Cre	Dre+Bre Bre-Dre

	fld	Bim		//	Bim				Are-Cre	Cre+Are	Bre+Dre	Bre-Dre
	fsub Dim	//	Bim-Dim			Are-Cre	Cre+Are	Bre+Dre	Bre-Dre

	fld st(3)   //	Bre+Dre			Bim-Dim	Are-Cre	Cre+Are	Bre+Dre	Bre-Dre
	fadd st,st(3)// X=A+B+C+D		Bim-Dim	Are-Cre	Cre+Are	Bre+Dre	Bre-Dre
	fxch st(3)   // Cre+Are			Bim-Dim	Are-Cre X		Bre+Dre Bre-Dre
	fsubrp st(4),st//Bim-Dim		Are-Cre X		Y=A-B+C-D Bre-Dre
	fld st(1)    // Are-Cre			Bim-Dim			Are-Cre X		Y		Bre-Dre
	fadd st,st(1)// Z=A-C+B-D		Bim-Dim			Are-Cre X		Y		Bre-Dre
	fxch st(3)   // X				Bim-Dim			Are-Cre	Z		Y		Bre-Dre
	fstp Are	//	Bim-Dim			Are-Cre	Z		Y		Bre-Dre
	fsub		//  T=A-B-C+D		Z				Y		Bre-Dre
	fxch st(2)	//  Y				Z				T		Bre-Dre
	fstp Bre	//  Z				T		Bre-Dre
	fstp Cre	//  T		Bre-Dre
	fstp Dre	//  Bre-Dre

    	/* Now do the imaginaries */
//					0				1		2		3		4			5
	fld Aim	//Aim
	fld st	//Aim Aim
	fld Cim	//Cim Aim Aim
	fsub st(2),st	//Cim Aim Aim-Cim
	fadd		//	Aim+Cim			Aim-Cim	Bre-Dre

	fld	Bim		//	Bim				Aim+Cim	Aim-Cim	Bre-Dre
	fadd Dim	//	Bim+Dim			Aim+Cim	Aim-Cim	Bre-Dre

	fld	st(3)	//	Bre-Dre			Bim+Dim	Aim+Cim	Aim-Cim	Bre-Dre
	fadd st,st(3) //X=B-D+A-C		Bim+Dim	Aim+Cim	Aim-Cim	Bre-Dre
	fld	st(2)	  //Aim+Cim			X		Bim+Dim	Aim+Cim	Aim-Cim	Bre-Dre
	fadd st,st(2) //Y=A+C+B+D		X		Bim+Dim	Aim+Cim	Aim-Cim	Bre-Dre
	fxch st(2) //	Bim+Dim			X		Y		Aim+Cim	Aim-Cim	Bre-Dre
	fsubp st(3),st//X				Y		Z=A+C-B-DAim-CimBre-Dre
	fstp Dim	//	Y				Z		Aim-Cim	Bre-Dre
	fstp Aim	//	Z				Aim-Cim	Bre-Dre
	fxch		//  Aim-Cim			Z		Bre-Dre
	fsubrp st(2),st//Z				T=A-C-B+D
#if defined(tstep)&&tstep==0
	fstp Bim//		T
	fstp Cim
#else
	mov eax,tstep//eax sert pour fstp Aim
	fstp Bim//		T
	cmp eax,0
	fstp Cim
	jz fft4lbldone	      		/* Twiddles are all unity, skip this stuff */

	
	//				0		1		2		3		4		5

/*2tstep*/
fld Bre	//Bre
add eax,eax
fld st	//Bre	Bre
fld Bim	//Bim	Bre		Bre
fld st	//Bim	Bim		Bre		Bre
fld COS(eax)	//cos	Bim		Bim		Bre		Bre
fmul st(4),st	//cos	Bim		Bim		Bre		Brecos
fld SIN(eax+MAXPOINTS_I)	//sin	cos	Bim	Bim	Bre	Brecos
fmul st(3),st	//sin	cos	Bim	Bimsin	Bre	Brecos
fxch st(2)		//Bim	cos	sin	Bimsin	Bre	Brecos
fmul			//Bimcos sin	Bimsin	Bre	Brecos
fxch st(2)		//Bimsin sin	Bimcos	Bre	Brecos
fsubp st(4),st //sin Bimcos Bre Brecos-Bimsin
fmulp st(2),st		//Bimcos Bresin Brecos-Bimsin
mov eax,tstep
fadd			//Bim Bre
fxch
fstp Bre
fstp Bim

/*1tstep*/
fld Cre	//Bre
	mov ebx,eax
fld st	//Bre	Bre
fld Cim	//Bim	Bre		Bre
fld st	//Bim	Bim		Bre		Bre
fld COS(eax)	//cos	Bim		Bim		Bre		Bre
fmul st(4),st	//cos	Bim		Bim		Bre		Brecos
fld SIN(eax+MAXPOINTS_I)	//sin	cos	Bim	Bim	Bre	Brecos
fmul st(3),st	//sin	cos	Bim	Bimsin	Bre	Brecos
fxch st(2)		//Bim	cos	sin	Bimsin	Bre	Brecos
fmul			//Bimcos sin	Bimsin	Bre	Brecos
fxch st(2)		//Bimsin sin	Bimcos	Bre	Brecos
fsubp st(4),st //sin Bimcos Bre Brecos-Bimsin
fmulp st(2),st		//Bimcos Bresin Brecos-Bimsin
	add eax,eax
fadd			//Bim Bre
fxch
fstp Cre
fstp Cim

/*3tstep*/
fld Dre	//Bre
add eax,ebx
fld st	//Bre	Bre
fld Dim	//Bim	Bre		Bre
fld st	//Bim	Bim		Bre		Bre
fld COS(eax)	//cos	Bim		Bim		Bre		Bre
fmul st(4),st	//cos	Bim		Bim		Bre		Brecos
fld SIN(eax+MAXPOINTS_I)	//sin	cos	Bim	Bim	Bre	Brecos
fmul st(3),st	//sin	cos	Bim	Bimsin	Bre	Brecos
fxch st(2)		//Bim	cos	sin	Bimsin	Bre	Brecos
fmul			//Bimcos sin	Bimsin	Bre	Brecos
fxch st(2)		//Bimsin sin	Bimcos	Bre	Brecos
fsubp st(4),st //sin Bimcos Bre Brecos-Bimsin
fmulp st(2),st		//Bimcos Bresin Brecos-Bimsin
fadd			//Bim Bre
fxch
fstp Dre
fstp Dim

/* modele
fld Dre		//	Dre
fld Dim		//	Dim		Dre
fld COS(eax)//cos		Dim		Dre
fld SIN(eax+MAXPOINTS)//sin		cos		Dim		Dre
fld st(3) //Dre sin cos Dim Dre
fmul st,st(2) //Drecos sin cos Dim Dre
fxch st(2) //cos sin Drecos Dim Dre
fld st(3) //Dim cos sin Drecos Dim Dre
fmul st,st(2) //Dimsin cos sin Drecos Dim Dre
fxch st(4) //Dim cos sin Drecos Dimsin Dre
fmul //Dimcos sin Drecos Dimsin Dre
fxch st(4) //Dre sin Drecos Dimsis Dimcos
fmul //Dresin Drecos Dimsin Dimcos
fxch st(2) //Dimsin Drecos Dresin Dimcos
fsub //Dre' Dresin Dimcos
fstp Dre //Dresin Dimcos
fadd //Dim'
fstp Dim
*/
                           
fft4lbldone:
#endif
	}
