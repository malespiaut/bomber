#include "General.h"
#include "Bomber.h"
#include "Multi.h"
#include "Debug.h"
#include "Son.h"
#include "VoiceStack.h"
#include "iVoiceStack.h"
#include <dsound.h>

extern LPDIRECTSOUND lpDS;

LPDIRECTSOUNDBUFFER lpDSBVS[MAX_PLAYERS];
struct VoiceStackElement VStack[MAX_PLAYERS][MAX_VS_ELEMENTS];
BOOL bLastWasFirst[MAX_PLAYERS];

extern DWORD FtDSize,FtCSize;
char *lpVSDecompBlock,*lpVSTempBlock,*lpVSUpsBlock;
DWORD VS_SrcFreq;
DWORD VS_SrcChan;

void CreateVoiceStack(int Num)
{
	if(lpDS==NULL)
		return;
	if(Num>=MAX_PLAYERS)
		return;
	if(lpDSBVS[Num])
		return;
//	if(IsLocalPlayer(Num))
//		return;
	DSBUFFERDESC dsbd;
	memset(&dsbd,0,sizeof(DSBUFFERDESC));
	dsbd.dwSize=sizeof(DSBUFFERDESC);
	dsbd.dwReserved=0;
	dsbd.dwFlags=DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_GLOBALFOCUS;
	dsbd.lpwfxFormat=GetPrimaryFormat();
/*
	dsbd.lpwfxFormat->nAvgBytesPerSec=11025*2;
	dsbd.lpwfxFormat->nBlockAlign=2;
	dsbd.lpwfxFormat->nChannels=1;
	dsbd.lpwfxFormat->nSamplesPerSec=11025;
*/
	VS_SrcChan=dsbd.lpwfxFormat->nChannels;
	VS_SrcFreq=dsbd.lpwfxFormat->nSamplesPerSec;
	DWORD DSize=VS_SrcChan*FtDSize*(VS_SrcFreq/11025);

	dsbd.dwBufferBytes=2*DSize;

	int hr=lpDS->CreateSoundBuffer(&dsbd,&lpDSBVS[Num],NULL);
	if(hr!=DS_OK)
		return;
	lpDSBVS[Num]->Play(0,0,DSBPLAY_LOOPING);
	SAFE_DELETE(dsbd.lpwfxFormat);
	for(int idx=0;idx<MAX_VS_ELEMENTS;idx++)
	{
		VStack[Num][idx].bActive=FALSE;
		VStack[Num][idx].lpBlock=new char[FtCSize];
	}

	bLastWasFirst[Num]=FALSE;
	if(lpVSDecompBlock==NULL)
		lpVSDecompBlock=new char[FtDSize];
	if(lpVSUpsBlock==NULL)
		lpVSUpsBlock=new char[((FtDSize*VS_SrcFreq)/11025)];
	if(lpVSTempBlock==NULL)
		lpVSTempBlock=new char[((FtDSize*VS_SrcFreq)/11025)];
	return;
}

void DestroyVoiceStack(int Num)
{
	if(Num>=MAX_PLAYERS)
		return;
//	if(IsLocalPlayer(Num))
//		return;
	for(int idx=0;idx<MAX_VS_ELEMENTS;idx++)
	{
		SAFE_DELETE_ARRAY(VStack[Num][idx].lpBlock);
	}
	if(lpDSBVS[Num])
	{
		lpDSBVS[Num]->Stop();
		SAFE_RELEASE(lpDSBVS[Num]);
	}
	return;
}

void DestroyAllVoiceStacks(void)
{
	for(int idx=0;idx<MAX_PLAYERS;idx++)
		DestroyVoiceStack(idx);
	SAFE_DELETE_ARRAY(lpVSDecompBlock);
	SAFE_DELETE_ARRAY(lpVSUpsBlock);
	SAFE_DELETE_ARRAY(lpVSTempBlock);
	return;
}

void PushVoiceStack(int Num,void* lpBlock,DWORD dwSize)
{
	if(Num>=MAX_PLAYERS)
		return;
//	if(IsLocalPlayer(Num))
//		return;
	if(lpDSBVS[Num]==NULL)
		CreateVoiceStack(Num);
	if(lpDSBVS[Num]==NULL)
		return;
	DWORD dwTime=0xffffffff;
	int ifnd=-1;

	for(int idx=0;idx<MAX_VS_ELEMENTS;idx++)
		if(!VStack[Num][idx].bActive)
		{
			ifnd=idx;
			break;
		}
		else
			if(VStack[Num][idx].dwTime<dwTime)
			{
				dwTime=VStack[Num][idx].dwTime;
				ifnd=idx;
			}

	/*
	if(VStack[Num][ifnd].bActive)
	{
		SAFE_DELETE_ARRAY(VStack[Num][ifnd].lpBlock);
	}
	else
	*/
	VStack[Num][ifnd].bActive=TRUE;
	VStack[Num][ifnd].dwTime=TickCount();
	VStack[Num][ifnd].dwSize=dwSize;
	if(dwSize<=FtCSize)
		memcpy(VStack[Num][ifnd].lpBlock,lpBlock,dwSize);
	else
		VStack[Num][ifnd].bActive=FALSE;

	return;
}

int PopVoiceStack(int Num)
{
	/*
	VStack[Num][0].lpBlock=new BYTE[FtDSize];
	for(DWORD ib=0;ib<FtDSize/2;ib++)
		*(ib+(short*)VStack[Num][0].lpBlock)=short(10000*sin(ib*.02));
	VStack[Num][0].dwSize=FtDSize;
	return 0;
	*/

	if(Num>=MAX_PLAYERS)
		return -1;
//	if(IsLocalPlayer(Num))
//		return -1;
	DWORD dwTime=0xffffffff;
	int FoundIDX=-1;
	for(int idx=0;idx<MAX_VS_ELEMENTS;idx++)
		if(VStack[Num][idx].bActive&&(VStack[Num][idx].dwTime<dwTime))
		{
			FoundIDX=int(idx);
			dwTime=VStack[Num][idx].dwTime;
		}

	return FoundIDX;
}

void GereVoiceStacks(void)
{
	for(int idx=0;idx<MAX_PLAYERS;idx++)
	{
//		if(IsLocalPlayer(idx))
//			continue;
		if(lpDSBVS[idx])
		{
			DWORD CurPos=0;
			int hr=lpDSBVS[idx]->GetCurrentPosition(&CurPos,0);

			void *lpBlock1=NULL,*lpBlock2=NULL;
			DWORD dwSBlock1,dwSBlock2;
			DWORD DSize=VS_SrcChan*FtDSize*(VS_SrcFreq/11025);

			if(CurPos>DSize+32)
			{
				if(!bLastWasFirst[idx])
				{
					int hr=lpDSBVS[idx]->Lock(0,DSize,&lpBlock1,&dwSBlock1,&lpBlock2,&dwSBlock2,0);
					if(hr!=DS_OK)
						continue;
					bLastWasFirst[idx]=TRUE;
				}
			}
			else
			if(CurPos<DSize-32)
			{
				if(bLastWasFirst[idx])
				{
					int hr=lpDSBVS[idx]->Lock(DSize,DSize,&lpBlock1,&dwSBlock1,&lpBlock2,&dwSBlock2,0);
					if(hr!=DS_OK)
						continue;
					bLastWasFirst[idx]=FALSE;
				}
			}

			if(lpBlock1)
			{
				int elmt=PopVoiceStack(idx);
				if(elmt>=0)
				{
					FT_Decomp((char*)VStack[idx][elmt].lpBlock,lpVSDecompBlock);
					if(VS_SrcFreq!=11025)
						FT_ReSample((short*)lpVSDecompBlock,(short*)lpVSUpsBlock,(short*)lpVSTempBlock,FtDSize/2,11025,VS_SrcFreq);
					else
						memcpy(lpVSUpsBlock,lpVSDecompBlock,FtDSize);
					if(VS_SrcChan==2)
						FT_MonoToStereo((short*)lpVSUpsBlock,(short*)lpBlock1,(VS_SrcFreq/11025)*FtDSize/2);
					else
						memcpy(lpBlock1,lpVSUpsBlock,FtDSize*(VS_SrcFreq/11025));
					VStack[idx][elmt].bActive=FALSE;
				}
				else
					memset(lpBlock1,0,DSize);
				lpDSBVS[idx]->Unlock(lpBlock1,dwSBlock1,lpBlock2,dwSBlock2);
			}

		}
	}
	return;
}
