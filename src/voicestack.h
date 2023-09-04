#ifndef _VOICESTACK_H_
#define _VOICESTACK_H_
void CreateVoiceStack(int);
void DestroyVoiceStack(int);
void DestroyAllVoiceStacks(void);
void PushVoiceStack(int,void*,DWORD);
int PopVoiceStack(int);
void GereVoiceStacks(void);
#endif
