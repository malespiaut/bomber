#include "General.h"
#include "Debug.h"
#include "iSon.h"
#include "Son.h"
#include "Bomber.h"
//#include <mmreg.h>

extern	HWND	hWndWin;
extern	BOOL	bDedicated;
extern struct SETTINGS Settings;

LPDIRECTSOUND		lpDS;
LPDIRECTSOUNDBUFFER	lpDSPrimary;
LPDIRECTSOUNDBUFFER	lpDSBuffer[N_BUFF];

DWORD			SizeBuffer[N_BUFF];
double			PanBuffer[N_BUFF];
DWORD			MoyPan[N_BUFF];
DSBUFFERDESC	dsdesc;
int				Volume;
BOOL			bQuality;

void InitSon(char* FileName,int lVolume,BOOL bHighQuality)
{
	WAVEFORMATEX wfxFormat;
	int	hr;
	if(!Settings.ENABLESON||bDedicated)
		return;
	if (CoCreateInstance(CLSID_DirectSound,
		NULL, CLSCTX_INPROC_SERVER, IID_IDirectSound, (void**)&lpDS)!=S_OK)
			ErrorMsg(LSZ_DS_FAILED,LSZ_DS_INIT);

	if (lpDS->Initialize(NULL)!=DS_OK)
		ErrorMsg(LSZ_DS_CANTINIT,LSZ_DS_INIT);
	lpDS->Compact();

	hr=lpDS->SetCooperativeLevel(hWndWin,DSSCL_PRIORITY);
	if(hr!=DS_OK)
		hr=lpDS->SetCooperativeLevel(hWndWin,DSSCL_EXCLUSIVE);
	if(hr!=DS_OK)
		ErrorMsg(LSZ_DS_CANTSETCOOP,LSZ_DS_INIT);

	memset(&dsdesc,0,sizeof(DSBUFFERDESC));
	dsdesc.dwSize=sizeof(DSBUFFERDESC);
	dsdesc.dwFlags=DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_PRIMARYBUFFER;

	hr=lpDS->CreateSoundBuffer(&dsdesc,&lpDSPrimary,0);
	if ((hr!=DS_OK)||(lpDSPrimary==0))
	{
		memset(&dsdesc,0,sizeof(DSBUFFERDESC));
		dsdesc.dwSize=sizeof(DSBUFFERDESC);
		dsdesc.dwFlags=DSBCAPS_PRIMARYBUFFER;

		hr=lpDS->CreateSoundBuffer(&dsdesc,&lpDSPrimary,0);
	}
	if ((hr!=DS_OK)||(lpDSPrimary==0))
		ErrorMsg(LSZ_DS_CANTCREATEBP,LSZ_DS_INIT);

	if(!LoadWave(FileName,&wfxFormat,0,0))
		bHighQuality=0;

	bQuality=bHighQuality;
	if(!bHighQuality)
	{
		wfxFormat.wFormatTag=WAVE_FORMAT_PCM;
		wfxFormat.wBitsPerSample=16;
		wfxFormat.nChannels=1;
		wfxFormat.nSamplesPerSec=22050;
		wfxFormat.nBlockAlign=2;
		wfxFormat.nAvgBytesPerSec=44100;
	}

	lpDSPrimary->SetFormat(&wfxFormat);
	lpDSPrimary->Play(0,0,DSBPLAY_LOOPING);

	Volume=lVolume;
	if(Volume<-10000) Volume=-10000;
	if(Volume>0) Volume=0;
	return;
}

WAVEFORMATEX* GetPrimaryFormat(void)
{
	if(lpDSPrimary)
	{
		WAVEFORMATEX* pwfxFormat;
		pwfxFormat=new WAVEFORMATEX;
		pwfxFormat->cbSize=sizeof(WAVEFORMATEX);
		lpDSPrimary->GetFormat(pwfxFormat,pwfxFormat->cbSize,NULL);
		return pwfxFormat;
	}
	else
		return NULL;
}

void CreateWaveBuffer(int Num,char* FileName)
{
    HMMIO       hmmio;              // file handle for open file 
    MMCKINFO    mmckinfoParent;     // parent chunk information 
    MMCKINFO    mmckinfoSubchunk;   // subchunk information structure 
    DWORD       dwFmtSize;          // size of "FMT" chunk 
    DWORD       dwDataSize;         // size of "DATA" chunk 
    char	    *pFormat;           // address of "FMT" chunk 
	WAVEFORMATEX	wfxFormat;
	LPVOID		lpBuff;
 
	if(!Settings.ENABLESON||bDedicated)
		return;
    if(!(hmmio = mmioOpen(FileName, NULL, 
        MMIO_READ | MMIO_ALLOCBUF))) 
	        return;
 
    mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
    if (mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent, NULL, 
        MMIO_FINDRIFF)) 
    { 
        mmioClose(hmmio, 0); 
        return; 
    } 
    mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' '); 
    if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, 
        MMIO_FINDCHUNK)) 
    { 
        mmioClose(hmmio, 0); 
        return; 
    } 
 
    dwFmtSize = mmckinfoSubchunk.cksize; 
	if(dwFmtSize<2+sizeof(WAVEFORMAT))
	{
        mmioClose(hmmio, 0); 
        return; 
	}

	pFormat=new char[dwFmtSize];
    if ((DWORD)mmioRead(hmmio, pFormat, dwFmtSize) != dwFmtSize)
	{ 
        mmioClose(hmmio, 0); 
		delete[]pFormat;
        return; 
    } 
	memcpy(&wfxFormat,pFormat,2+sizeof(WAVEFORMAT));
	wfxFormat.cbSize=0;
	if(wfxFormat.wFormatTag!=WAVE_FORMAT_PCM)
	{
		mmioClose(hmmio, 0); 
		return; 
	}
	delete[]pFormat;
 
    mmioAscend(hmmio, &mmckinfoSubchunk, 0); 
 
    mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a'); 
    if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, 
        MMIO_FINDCHUNK)) 
    { 
        mmioClose(hmmio, 0); 
        return;
    } 
 
    dwDataSize = mmckinfoSubchunk.cksize; 
    if (dwDataSize == 0L)
	{ 
        mmioClose(hmmio, 0); 
        return;
    } 
	CreateSBuffer(Num,dwDataSize,&wfxFormat);
 
	lpDSBuffer[Num]->Lock(0,dwDataSize,&lpBuff,&dwDataSize,0,0,DSBLOCK_ENTIREBUFFER);
	if (lpBuff==0)
		ErrorMsg(LSZ_DS_CANTLOCK,LSZ_DS_INIT);

    if((DWORD)mmioRead(hmmio, (HPSTR) lpBuff, dwDataSize) != dwDataSize)
	{ 
		lpDSBuffer[Num]->Unlock(lpBuff,dwDataSize,0,0);
        mmioClose(hmmio, 0); 
        return; 
    } 
 
	lpDSBuffer[Num]->Unlock(lpBuff,dwDataSize,0,0);
    mmioClose(hmmio, 0); 

	if (lpDSBuffer[Num]==0)
		ErrorMsg(LSZ_DS_CANTCREATEBS,LSZ_DS_INIT);
	return;
}

void CreateSBuffer(int Num,DWORD Size,WAVEFORMATEX* lpwfxFormat)
{
	if (Num>=N_BUFF) 
		ErrorMsg(LSZ_DS_NOTENOUGHBUFF,LSZ_DS_INIT);

	SAFE_RELEASE(lpDSBuffer[Num])

	dsdesc.dwSize=sizeof(DSBUFFERDESC);
	if(bQuality)
		dsdesc.dwFlags=DSBCAPS_STATIC|DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME;
	else
		dsdesc.dwFlags=DSBCAPS_STATIC|DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLVOLUME;
	dsdesc.dwBufferBytes=Size;
	SizeBuffer[Num]=Size;
	dsdesc.lpwfxFormat=lpwfxFormat;
	lpDS->CreateSoundBuffer(&dsdesc,&lpDSBuffer[Num],0);
	if(lpDSBuffer[Num]!=0)
		lpDSBuffer[Num]->SetVolume(Volume);

	return;
}

void FinSon(void)
{
	for (int a=0;a<N_BUFF;a++)
		SAFE_RELEASE(lpDSBuffer[a])

	SAFE_RELEASE(lpDSPrimary)
	SAFE_RELEASE(lpDS)
	return;
}

int DupliqueSon(int NumSrc,int NumDest)
{
	if(!Settings.ENABLESON||bDedicated)
		return -1;
	if(lpDSBuffer[NumSrc]==0)
		return -1;
	if(NumDest>=N_BUFF)
		return -1;
	if(lpDS==0)
		return -1;
	lpDS->DuplicateSoundBuffer(lpDSBuffer[NumSrc],&lpDSBuffer[NumDest]);
	SizeBuffer[NumDest]=SizeBuffer[NumSrc];
	return NumDest;
}

void PlaySon(double Pan,int Num,BOOL Looping)
{
	DWORD	dwStatus;
	if(!Settings.ENABLESON||bDedicated)
		return;
	if(Num>=N_BUFF)
		return;
	if (lpDSBuffer[Num]==0) return;
	lpDSBuffer[Num]->GetStatus(&dwStatus);
	if ((dwStatus&DSBSTATUS_BUFFERLOST)!=0)
	{
		FinSon();
		InitDSound();
		return;
	}
	if ((dwStatus&DSBSTATUS_PLAYING)!=0)
	{
		PanBuffer[Num]+=Pan;
		Pan=PanBuffer[Num]/double(++MoyPan[Num]);
	}
	else
	{
		PanBuffer[Num]=Pan;
		MoyPan[Num]=1;
	}

	Pan=Pan*Pan*Pan;

	int hr=lpDSBuffer[Num]->SetPan(int(DSBPAN_RIGHT*0.5*Pan));

	lpDSBuffer[Num]->Play(0,0,Looping?DSBPLAY_LOOPING:0);
	return;
}

void StopSon(int Num)
{
	if(!Settings.ENABLESON||bDedicated)
		return;
	if(Num>=N_BUFF)
		return;
	if (lpDSBuffer[Num]==0) return;
	lpDSBuffer[Num]->Stop();
	return;
}

BOOL LoadWave(char* szFileName,WAVEFORMATEX* lpwfxFormat,LPVOID lpBuff,LPDWORD lpdwSize)
{ 
    HMMIO       hmmio;              // file handle for open file 
    MMCKINFO    mmckinfoParent;     // parent chunk information 
    MMCKINFO    mmckinfoSubchunk;   // subchunk information structure 
    DWORD       dwFmtSize;          // size of "FMT" chunk 
    DWORD       dwDataSize;         // size of "DATA" chunk 
    char	    *pFormat;           // address of "FMT" chunk 
 
    if(!(hmmio = mmioOpen(szFileName, NULL, 
        MMIO_READ | MMIO_ALLOCBUF))) 
	        return FALSE;
 
    mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
    if (mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent, NULL, 
        MMIO_FINDRIFF)) 
    { 
        mmioClose(hmmio, 0); 
        return FALSE; 
    } 
    mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' '); 
    if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, 
        MMIO_FINDCHUNK)) 
    { 
        mmioClose(hmmio, 0); 
        return FALSE; 
    } 
 
    dwFmtSize = mmckinfoSubchunk.cksize; 
	if(dwFmtSize<2+sizeof(WAVEFORMAT))
	{
        mmioClose(hmmio, 0); 
        return FALSE; 
	}

	pFormat=new char[dwFmtSize];
    if ((DWORD)mmioRead(hmmio, pFormat, dwFmtSize) != dwFmtSize)
	{ 
        mmioClose(hmmio, 0); 
		delete[]pFormat;
        return FALSE; 
    } 
	if(lpwfxFormat!=0)
	{
		memcpy(lpwfxFormat,pFormat,2+sizeof(WAVEFORMAT));
		lpwfxFormat->cbSize=0;
		if(lpwfxFormat->wFormatTag!=WAVE_FORMAT_PCM)
		{
			mmioClose(hmmio, 0); 
			return FALSE; 
		}
	}
	delete[]pFormat;
	if(lpdwSize==0) 
	{
		mmioClose(hmmio, 0); 
		return TRUE;
	}
 
    mmioAscend(hmmio, &mmckinfoSubchunk, 0); 
 
    mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a'); 
    if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, 
        MMIO_FINDCHUNK)) 
    { 
        mmioClose(hmmio, 0); 
        return FALSE; 
    } 
 
    dwDataSize = mmckinfoSubchunk.cksize; 
    if (dwDataSize == 0L)
	{ 
        mmioClose(hmmio, 0); 
        return TRUE; 
    } 
 
	if((*lpdwSize==0)||(lpBuff==0))
	{
		*lpdwSize=dwDataSize;
        mmioClose(hmmio, 0); 
		return TRUE;
	}
	if(*lpdwSize>dwDataSize)
		*lpdwSize=dwDataSize;
    if((DWORD)mmioRead(hmmio, (HPSTR) lpBuff, *lpdwSize) != *lpdwSize)
	{ 
        mmioClose(hmmio, 0); 
        return FALSE; 
    } 
 
    mmioClose(hmmio, 0); 
	return TRUE;
} 
