#include "General.h"
#include <mmsystem.h>
#include <dsound.h>
#include "Bomber.h"
#include "Debug.h"
#include "Son.h"
#include "Multi.h"
#include "Voice.h"
#include "VoiceStack.h"

LPDIRECTSOUNDCAPTURE lpDSC;
LPDIRECTSOUNDCAPTUREBUFFER lpDSCB;
BOOL bStarted;

DWORD CurBlock,NumBlocks;
DWORD FtDSize,FtCSize;
long SendedBytes,NumSends;
DWORD LastBaudTick;
char *lpCompBlock,*lpDecompBlock,*lpTempBlock;
const DWORD RB_SIZE=65536*2;
DWORD SrcChan;
DWORD SrcFreq;

void InitVoiceCapture(void)
{
	int hr;
	if (CoCreateInstance(CLSID_DirectSoundCapture,
		NULL, CLSCTX_INPROC_SERVER, IID_IDirectSoundCapture, (void**)&lpDSC)!=S_OK)
			ErrorMsg(LSZ_DSC_FAILED,LSZ_DSC_INIT);

	if (lpDSC->Initialize(NULL)!=DS_OK)
		ErrorMsg(LSZ_DSC_CANTINIT,LSZ_DSC_INIT);

	DSCBUFFERDESC dscbd;
	memset(&dscbd,0,sizeof(DSCBUFFERDESC));
	dscbd.dwSize=sizeof(DSCBUFFERDESC);
	dscbd.dwReserved=0;
	dscbd.dwBufferBytes=RB_SIZE;
	dscbd.dwFlags=0;
	dscbd.lpwfxFormat=GetPrimaryFormat();
/*
	dscbd.lpwfxFormat->nAvgBytesPerSec=11025*2;
	dscbd.lpwfxFormat->nBlockAlign=2;
	dscbd.lpwfxFormat->nChannels=1;
	dscbd.lpwfxFormat->nSamplesPerSec=11025;
*/
	hr=lpDSC->CreateCaptureBuffer(&dscbd,&lpDSCB,NULL);
	SrcChan=dscbd.lpwfxFormat->nChannels;
	SrcFreq=dscbd.lpwfxFormat->nSamplesPerSec;

	SAFE_DELETE(dscbd.lpwfxFormat);

	if(hr!=DS_OK)
		ErrorMsg(LSZ_DSC_CANTCREATEDSCB,LSZ_DSC_INIT);

	CurBlock=0;
	NumBlocks=dscbd.dwBufferBytes/(SrcChan*FtDSize*(SrcFreq/11025));
	lpDecompBlock=new char[FtDSize];
	lpTempBlock=new char[FtDSize*(SrcFreq/11025)];
	lpCompBlock=new char[FtCSize];
	return;
}

void StartVoiceCapture(void)
{
	if(lpDSCB==NULL)
		return;
	if(bStarted)
		return;
	lpDSCB->Start(DSCBSTART_LOOPING);
	bStarted=TRUE;
	LastBaudTick=0;
	NumSends=SendedBytes=0;
	CurBlock=0;
	return;
}

void StopVoiceCapture(void)
{
	if(lpDSCB==NULL)
		return;
	if(!bStarted)
		return;
	lpDSCB->Stop();
	bStarted=FALSE;
	return;
}

void FinVoiceCapture(void)
{
	if(lpDSCB&&bStarted)
	{
		lpDSCB->Stop();
		bStarted=FALSE;
	}
	SAFE_RELEASE(lpDSCB);
	SAFE_RELEASE(lpDSC);
	SAFE_DELETE_ARRAY(lpCompBlock);
	SAFE_DELETE_ARRAY(lpTempBlock);
	SAFE_DELETE_ARRAY(lpDecompBlock);
	//FtStop();
	return;
}

void GereVoiceCapture(void)
{
	if(!bStarted)
		return;
	if(LastBaudTick==0)
		LastBaudTick=TickCount();
	DWORD CurPos,NewCurBlock;
	lpDSCB->GetCurrentPosition(NULL,&CurPos);
	if(CurPos>128)
		CurPos-=128;
	else
		CurPos=RB_SIZE-(128-CurPos);

	DWORD DSize=FtDSize*SrcChan*(SrcFreq/11025);
	NewCurBlock=int(floor(float(CurPos)/DSize));
	if(NewCurBlock!=CurBlock)
	{
		if(NewCurBlock<CurBlock)
		{
			while(CurBlock<NumBlocks)
			{
				SendCapturedBlock();
				CurBlock++;
			}
			CurBlock=0;
		}

		while(CurBlock<NewCurBlock)
		{
			SendCapturedBlock();
			CurBlock++;
		}
	}
	return;
}

void SendCapturedBlock(void)
{
	void *lpBlock1=NULL,*lpBlock2=NULL;
	DWORD dwSBlock1,dwSBlock2;

	DWORD DSize=FtDSize*SrcChan*(SrcFreq/11025);
	int hr=lpDSCB->Lock(CurBlock*DSize,DSize,&lpBlock1,&dwSBlock1,&lpBlock2,&dwSBlock2,0);
	if(hr!=DS_OK)
		return;

	if(SrcChan==2)
		FT_StereoToMono((short*)lpBlock1,(short*)lpBlock1,(SrcFreq/11025)*FtDSize/2);
	if(SrcFreq!=11025)
		FT_ReSample((short*)lpBlock1,(short*)lpDecompBlock,(short*)lpTempBlock,(SrcFreq/11025)*FtDSize/2,SrcFreq,11025);
	else
		memcpy(lpDecompBlock,lpBlock1,FtDSize);

	lpDSCB->Unlock(lpBlock1,FtDSize,lpBlock2,0);


	long CSize;
	CSize=FT_Comp(lpDecompBlock,lpCompBlock);

	if(CSize>0)
		SendGameMessage(FROM_FIRSTLOCALPLAYER,-1,MSG_VOICE,CSize,(int)lpCompBlock,0,0);
//		PushVoiceStack(2,lpCompBlock,CSize);

	SendedBytes+=CSize;
	NumSends++;
	if(TickCount()-LastBaudTick>=2000)
	{
		OutFileEx2("Débit %ld en %ld envois",SendedBytes/2,NumSends/2);
		NumSends=SendedBytes=0;
		LastBaudTick=TickCount();
	}
	return;
}
