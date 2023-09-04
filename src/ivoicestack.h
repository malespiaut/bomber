#ifndef _iVS_H_
#define _iVS_H_
const DWORD MAX_VS_ELEMENTS=8;
struct VoiceStackElement
{
	DWORD dwTime;
	BOOL bActive;
	DWORD dwSize;
	char *lpBlock;
};
#endif
