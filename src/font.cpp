#include "General.h"
#include "Font.h"

HFONT font[MAX_FONTS];
int NumFonts;

int NewFont(HDC hdc,const char* szFontFace,int fHeight)
{
	if((NumFonts>=MAX_FONTS)||(hdc==NULL))
		return -1;
	int he=-MulDiv(fHeight, GetDeviceCaps(hdc, LOGPIXELSY), 72);

	font[NumFonts]=CreateFont(
		he,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS 
		,CLIP_DEFAULT_PRECIS 
		,DEFAULT_QUALITY 
		,DEFAULT_PITCH|FF_DONTCARE 
		,szFontFace
		);
	if(font[NumFonts])
		return -1;
	NumFonts++;
	return NumFonts-1;
}

void AttachFont(HDC hdc,int SelFont)
{
	if((SelFont<0)||(SelFont>=NumFonts))
		return;
	if(hdc==NULL)
		return;
	HGDIOBJ hobj=SelectObject(hdc,font[SelFont]);
	if((hobj!=NULL)&&(hobj!=font[SelFont]))
		DeleteObject(hobj);
	return;
}

void DeleteFont(int SelFont)
{
	if((SelFont<0)||(SelFont>=NumFonts))
		return;
	for(int sf=SelFont;sf<NumFonts-1;sf++)
		font[sf]=font[sf+1];
	NumFonts--;
	return;
}
