#ifdef EvarieFast
		{
		for (unsigned long W=(unsigned long)Enveloppe;W<(unsigned long)(Enveloppe+MAXIF);W+=16)
			{
			
				*((unsigned long *)(W))=*((unsigned long *)(W))*65534/EvRef+1;
				*((unsigned long *)(W+4))=*((unsigned long *)(W+4))*65534/EvRef+1;
				*((unsigned long *)(W+8))=*((unsigned long *)(W+8))*65534/EvRef+1;
				*((unsigned long *)(W+12))=*((unsigned long *)(W+12))*65534/EvRef+1;
			}
		}
	/*
		{
		unsigned long TempInt;
		TempInt=(unsigned long)(Enveloppe+MAXIF);
			{
			for (unsigned long W=(unsigned long)Enveloppe;W<TempInt;W+=4)
				*(unsigned long *)W=*(unsigned long *)W*65534/EvRef+1;
			}
		}
	*/
	
	Enveloppe[0]=((Enveloppe[0]+Enveloppe[1])>>1)+1;
	Enveloppe[MAXIF-1]=((Enveloppe[MAXIF-1]+Enveloppe[MAXIF-2])>>1)+1;
	{
		unsigned long Last1=Enveloppe[0];
		unsigned long Last2=Enveloppe[1];
		
		unsigned long W=1;
		while(W<(MAXIF-4))
		{
			/*
			Enveloppe[W]=((Last1+DAncien*Last2+Enveloppe[W+1])>>DDec);
			Last1=Last2;
			Last2=Enveloppe[W+1];
			*/
			Enveloppe[W]=((Last1+DAncien*Last2+Enveloppe[W+1])>>DDec);
			Last1=Last2;
			Last2=Enveloppe[W+1];

			Enveloppe[W+1]=((Last1+DAncien*Last2+Enveloppe[W+2])>>DDec);
			Last1=Last2;
			Last2=Enveloppe[W+2];

			Enveloppe[W+2]=((Last1+DAncien*Last2+Enveloppe[W+3])>>DDec);
			Last1=Last2;
			Last2=Enveloppe[W+3];

			Enveloppe[W+3]=((Last1+DAncien*Last2+Enveloppe[W+4])>>DDec);
			Last1=Last2;
			Last2=Enveloppe[W+4];

			W+=4;
		}

			Enveloppe[W]=((Last1+DAncien*Last2+Enveloppe[W+1])>>DDec);
			Last1=Last2;
			Last2=Enveloppe[W+1];
			Enveloppe[W+1]=((Last1+DAncien*Last2+Enveloppe[W+2])>>DDec);
	}
#else
#ifdef EVarie
{
	unsigned long TempInt;
	TempInt=(unsigned long)(Enveloppe+MAXIF);
		{
		for (unsigned long W=(unsigned long)Enveloppe;W<TempInt;W+=4)
			*(unsigned long *)W=*(unsigned long *)W*65534/EvRef+1;
		}
	
	
	Enveloppe[0]=((Enveloppe[0]+Enveloppe[1])>>1)+1;
	Enveloppe[MAXIF-1]=((Enveloppe[MAXIF-1]+Enveloppe[MAXIF-2])>>1)+1;
	{
		unsigned long Last1=Enveloppe[0];
		unsigned long Last2=Enveloppe[1];
		for (unsigned long W=1;W<MAXIF-1;W++)
		{
			Enveloppe[W]=((Last1+DAncien*Last2+Enveloppe[W+1])>>DDec);
			Last1=Last2;
			Last2=Enveloppe[W+1];
		}
	}
}
#endif
#endif