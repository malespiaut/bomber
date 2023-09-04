#undef	Are
#undef	Bre
#undef	Cre
#undef	Dre
#undef	Aim
#undef	Bim
#undef	Cim
#undef	Dim

#define	Are	float ptr [eax]
#define	Aim	float ptr [4+eax]
#define Bre	float ptr [8+eax]
#define	Bim	float ptr [12+eax]
#define	Cre	float ptr [16+eax]
#define	Cim	float ptr [20+eax]
#define	Dre	float ptr [24+eax]
#define	Dim	float ptr [28+eax]


__asm
	{
#ifdef AddToX
	//mov	eax,x    ;	/* eax -> x[0] */
	add eax,AddToX
#else
	mov	eax,x    ;	/* eax -> x[0] */
#endif

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
	fstp Bim//		T
	fstp Cim

	}
