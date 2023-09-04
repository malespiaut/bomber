#ifndef _FONT_H_
#define _FONT_H_
const int MAX_FONTS=64;
int NewFont(HDC,const char*,int);
void AttachFont(HDC,int);
void DeleteFont(int);
#endif
