#ifndef _SON_H_
#define _SON_H_
#include <dsound.h>
void PlaySon(double,int,BOOL);
void StopSon(int);
void InitSon(char*,int,BOOL);
void CreateWaveBuffer(int,char*);
void CreateSBuffer(int,DWORD,WAVEFORMATEX*);
int DupliqueSon(int,int);
void FinSon(void);
BOOL LoadWave(char*,WAVEFORMATEX*,LPVOID,LPDWORD);
WAVEFORMATEX* GetPrimaryFormat(void);
#endif
