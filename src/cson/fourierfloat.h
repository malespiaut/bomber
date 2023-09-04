
/*????0000 0?111111*/
/*
#define SetCoproStateFirst {\
 __asm{__asm fnstcw CoproState\
 __asm	mov ax, CoproState\
 __asm	and ax, not 300h\
 __asm	or  ax, 3fh\
 __asm	and ax, not 0C00h\
 __asm	mov CoproState, ax\
 __asm	fldcw  CoproState}}
 */
#define SetCoproStateFirst {\
 __asm{__asm fnstcw CoproState\
 __asm	mov ax, CoproState\
 __asm	and ax, not 0f80h\
 __asm	or  ax, 3fh\
 __asm	mov CoproState, ax\
 __asm	fldcw  CoproState}}
//#undef SetCoproStateFirst
//#define SetCoproStateFirst
#define SetCoproState __asm	fldcw  CoproState
//#undef SetCoproState
//#define SetCoproState

#define ResetCoproState __asm fninit
#undef ResetCoproState
#define ResetCoproState

void __stdcall INITFFT(void);
void __stdcall SFFTSWAP(complex * D,long NrTMax,long TMax);
void __stdcall SFFT(complex * D,long n);
void __stdcall SFFTSWAP(complex * D,long NrTMax,long TMax);
long __stdcall Expand(complex * TFFT1,unsigned long * Enveloppe,float * TEXC,float * TEXS,float * M1,unsigned  long MXA,unsigned long BPA,unsigned long MAXIF,unsigned  long PPP, char * EcrFT,unsigned long BTST,unsigned long MST);
long __stdcall ExpandFast(complex * TFFT1,unsigned long * Enveloppe,float * TEXC,float * TEXS,float * M1,unsigned  long MXA,unsigned long BPA,unsigned long MAXIF,unsigned  long PPP, char * EcrFT,unsigned long BTST,unsigned long MST);
long __stdcall Discretise(complex * TFFT2,unsigned long * Enveloppe,float * M1,unsigned long MXP,unsigned  long BPP,unsigned  long MXA,unsigned  long BPA,unsigned long MAXIF,char * EcrFT,unsigned long BTST);
