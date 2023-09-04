#ifndef _CRYPTE_H_
#define _CRYPTE_H_
BOOL DecodeScore(const struct EncRegScore*,struct RegScore*);
void EncodeScore(const struct RegScore*,struct EncRegScore*);
BYTE CheckSumSettings(const struct SETTINGS*);
short EncodeChar(short);
short DecodeChar(short);
void ResetKey(const char*);
void assemble(void);
void code(void);
#endif
