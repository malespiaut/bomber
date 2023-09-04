//FONCTION ACTUELLE
/*
void __inline
iFFT4G(complex *x,int astep,int tstep)
{
*/
	__asm
	{
	mov ebx,astep
	mov eax,x    	/* eax -> a */
	lea ecx,[eax+2*ebx]
	lea edx,[ebx+ecx]
	add ebx,eax

// Methode initiale
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

	fxch st(3)	//	Bre+Dre			Are-Cre	Cre+Are	Bim-Dim	Bre-Dre
	fld	st		//	Bre+Dre			Bre+Dre	Are-Cre	Cre+Are	Bim-Dim		Bre-Dre
	fadd st,st(3) //Br+Dr+Cr+ArX	Bre+Dre	Are-Cre	Cre+Are	Bim-Dim		Bre-Dre
	fxch st(3) 	//	Cre+Are			Bre+Dre	Are-Cre	X		Bim-Dim		Bre-Dre
	fsubr		//	Cr+Ar-(Br+Dr)Y	Are-Cre	X		Bim-Dim	Bre-Dre
	fxch st(1)	//	Are-Cre			Y		X		Bim-Dim	Bre-Dre
	fld	st		//	Are-Cre			Are-Cre	Y		X		Bim-Dim		Bre-Dre
	fadd st,st(4) //Ar-Cr+Bi-DiZ	Are-Cre	Y		X		Bim-Dim		Bre-Dre
	fxch st(4)	//	Bim-Dim			Are-Cre	Y		X		Z			Bre-Dre
	fsub		//	-B+D+Ar-Cr=T	Y		X		Z		Bre-Dre
	fxch st(2)	//	X				Y		T		Z		Bre-Dre
	fstp Are	//	Y				T		Z		Bre-Dre
	fstp Bre	//	T				Z		Bre-Dre
	fstp Dre	//	Z				Bre-Dre
	fstp Cre	//	Bre-Dre
                      
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
	fadd st,st(3) //Br-Dr+Ai-CiX	Bim+Dim	Aim+Cim	Aim-Cim	Bre-Dre
	fxch st(2)	//	Aim+Cim			Bim+Dim	X		Aim-Cim	Bre-Dre
	fld	st		//	Aim+Cim			Aim+Cim	Bim+Dim	X		Aim-Cim		Bre-Dre
	fadd st,st(2) //Ai+Ci+Bi+DiY	Aim+Cim	Bim+Dim	X		Aim-Cim		Bre-Dre
	fxch st(2)	//	Bim+Dim			Aim+Cim	Y		X		Aim-Cim		Bre-Dre
	fsub	      //Z=-B-D+A+C		Y		X		Aim-Cim	Bre-Dre
	fxch st(3)	//	Aim-Cim			Y		X		Z		Bre-Dre
	fsub st,st(4) //Ai-Ci-(Br-Dr)T	Y		X		Z		Bre-Dre
	fxch st(2)	//	X				Y		T		Z		Bre-Dre
	fstp Dim	//	Y				T		Z		Bre-Dre
	fstp Aim	//	T				Z		Bre-Dre
	mov eax,tstep
	fstp Cim	//	Z				Bre-Dre
	cmp eax,0
	fstp Bim	//	Bre-Dre
	fstp st
	jz done	      		/* Twiddles are all unity, skip this stuff */

	
	//				0		1		2		3		4		5

/*2tstep*/
add eax,eax
fld Bre	//Bre
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
	add eax,eax
fsubp st(4),st //sin Bimcos Bre Brecos-Bimsin
fmulp st(2),st		//Bimcos Bresin Brecos-Bimsin
	add eax,tstep
fadd			//Bim Bre
fxch
fstp Cre
fstp Cim

/*3tstep*/
fld Dre	//Bre
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
                           
done:
	}
	//return;
/*
}
*/