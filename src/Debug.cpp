#include "General.h"
#include "Bomber.h"
#include "Debug.h"
#include <ddraw.h>
#include <stdlib.h>

extern	HWND	hWndWin;
extern	HWND	hWndCon;
char*	szDebug="debug.txt";
extern	DWORD	TimerRef;
extern	struct	SETTINGS	Settings;
extern	char	szWorkingDirectory[BMAX_PATH];
BOOL	bOut;
char	szCDir[BMAX_PATH];
#ifndef DX3
extern LPDIRECTDRAW4	 		lpDD;
#else
extern LPDIRECTDRAW2	 		lpDD;
#endif
extern	BOOL	bConsole;

char* strtoupper(char* String)
{
	BOOL cvt=TRUE;
	int lng=strlen(String);
	for(int cnt=0;cnt<lng;cnt++)
	{
		if(String[cnt]=='\"') //ne converit pas le texte entre " "
			cvt=!cvt;
		if(cvt)
			String[cnt]=toupper(String[cnt]);
	}

	return String;
}
void EndOutFile(void)
{
	_getcwd(szCDir,BMAX_PATH);
	_chdir(szWorkingDirectory);
	if(!bOut)
		remove(szDebug);
	_chdir(szCDir);
	return;
}

void OutFileEx(const char *String,int Valeur)
{
	FILE	*Fichier;
	if(!Settings.DEBUG&&!bConsole)
		return;

	if(bConsole)
	{
		char b[BMAX_PATH];
		sprintf(b,"%s %ld\10\10",String,Valeur);
		DWORD txtsze=0;
		char* ConText=NULL;
		txtsze=GetWindowTextLength(GetDlgItem(hWndCon,IDC_CONOUT));
		txtsze++;
		DWORD len=strlen(b);
		ConText=new char[txtsze+len];
		memset(ConText,0,txtsze+len);
		GetWindowText(GetDlgItem(hWndCon,IDC_CONOUT),ConText,txtsze);
		strcat(ConText,b);
		SetWindowText(GetDlgItem(hWndCon,IDC_CONOUT),ConText);
		SAFE_DELETE_ARRAY(ConText);
		SendDlgItemMessage(hWndCon,IDC_CONOUT,EM_SETSEL,txtsze+len,txtsze+len);
		SendDlgItemMessage(hWndCon,IDC_CONOUT,EM_SCROLLCARET,0,0);

		/*
		StringToOem(b);
		DWORD len=strlen(b);
		HANDLE hConOut=GetStdHandle(STD_OUTPUT_HANDLE);
		WriteConsole(hConOut,b,len,&len,0);
		WriteConsole(hConOut,"\n",1,&len,0);
		*/
	}

	_getcwd(szCDir,BMAX_PATH);
	_chdir(szWorkingDirectory);
	Fichier=fopen(szDebug,"a+");
	fprintf(Fichier,"%ld : %s%ld\n",GetTickCount()-TimerRef,String,Valeur);
	fclose(Fichier);
	bOut=TRUE;
	_chdir(szCDir);
	return;
}

void _cdecl OutFileEx2(const char *String,...)
{
	FILE	*Fichier;
	va_list vl;

	if(!Settings.DEBUG&&!bConsole)
		return;
	va_start(vl,String);

	if(bConsole)
	{
		char b[BMAX_PATH];
		vsprintf(b,String,vl);
		strcat(b,"\r\n");
		DWORD txtsze=0;
		char* ConText=NULL;
		txtsze=GetWindowTextLength(GetDlgItem(hWndCon,IDC_CONOUT));
		txtsze++;
		DWORD len=strlen(b);
		ConText=new char[txtsze+len];
		memset(ConText,0,txtsze+len);
		GetWindowText(GetDlgItem(hWndCon,IDC_CONOUT),ConText,txtsze);
		strcat(ConText,b);
		SetWindowText(GetDlgItem(hWndCon,IDC_CONOUT),ConText);
		SAFE_DELETE_ARRAY(ConText);
		SendDlgItemMessage(hWndCon,IDC_CONOUT,EM_SETSEL,txtsze+len,txtsze+len);
		SendDlgItemMessage(hWndCon,IDC_CONOUT,EM_SCROLLCARET,0,0);
		/*
		StringToOem(b);
		DWORD len=strlen(b);
		HANDLE hConOut=GetStdHandle(STD_OUTPUT_HANDLE);
		WriteConsole(hConOut,b,len,&len,0);
		WriteConsole(hConOut,"\n",1,&len,0);
		*/
	}

	_getcwd(szCDir,BMAX_PATH);
	_chdir(szWorkingDirectory);
	Fichier=fopen(szDebug,"a+");
	fprintf(Fichier,"%ld : ",GetTickCount()-TimerRef);
	vfprintf(Fichier,String,vl);
	fprintf(Fichier,"\n");
	fclose(Fichier);
	bOut=TRUE;
	_chdir(szCDir);
	return;
}

void OutFileInt(int Valeur)
{
	FILE	*Fichier;
	if(!Settings.DEBUG&&!bConsole)
		return;
	if(bConsole)
	{
		char b[BMAX_PATH];
		sprintf(b,"%ld\r\n",Valeur);
		DWORD txtsze=0;
		char* ConText=NULL;
		txtsze=GetWindowTextLength(GetDlgItem(hWndCon,IDC_CONOUT));
		txtsze++;
		DWORD len=strlen(b);
		ConText=new char[txtsze+len];
		memset(ConText,0,txtsze+len);
		GetWindowText(GetDlgItem(hWndCon,IDC_CONOUT),ConText,txtsze);
		strcat(ConText,b);
		SetWindowText(GetDlgItem(hWndCon,IDC_CONOUT),ConText);
		SAFE_DELETE_ARRAY(ConText);
		SendDlgItemMessage(hWndCon,IDC_CONOUT,EM_SETSEL,txtsze+len,txtsze+len);
		SendDlgItemMessage(hWndCon,IDC_CONOUT,EM_SCROLLCARET,0,0);
/*
		StringToOem(b);
		DWORD len=strlen(b);
		HANDLE hConOut=GetStdHandle(STD_OUTPUT_HANDLE);
		WriteConsole(hConOut,b,len,&len,0);
*/
	}

	_getcwd(szCDir,BMAX_PATH);
	_chdir(szWorkingDirectory);
	Fichier=fopen(szDebug,"a+");
	fprintf(Fichier,"%ld : %ld\n",GetTickCount()-TimerRef,Valeur);
	fclose(Fichier);
	bOut=TRUE;
	_chdir(szCDir);
	return;
}

void OutFile(const char *String)
{
	FILE	*Fichier;
	if(!Settings.DEBUG&&!bConsole)
		return;
	if(bConsole)
	{
		DWORD txtsze=0;
		char* ConText=NULL;
		txtsze=GetWindowTextLength(GetDlgItem(hWndCon,IDC_CONOUT));
		txtsze++;
		DWORD len=strlen(String)+strlen("\r\n");
		ConText=new char[txtsze+len];
		memset(ConText,0,txtsze+len);
		GetWindowText(GetDlgItem(hWndCon,IDC_CONOUT),ConText,txtsze);
		strcat(ConText,String);
		strcat(ConText,"\r\n");
		SetWindowText(GetDlgItem(hWndCon,IDC_CONOUT),ConText);
		SAFE_DELETE_ARRAY(ConText);
		SendDlgItemMessage(hWndCon,IDC_CONOUT,EM_SETSEL,txtsze+len,txtsze+len);
		SendDlgItemMessage(hWndCon,IDC_CONOUT,EM_SCROLLCARET,0,0);
		/*
		StringToOem(b);
		DWORD len=strlen(b);
		HANDLE hConOut=GetStdHandle(STD_OUTPUT_HANDLE);
		WriteConsole(hConOut,b,len,&len,0);
		WriteConsole(hConOut,"\n",1,&len,0);
		*/
	}

	_getcwd(szCDir,BMAX_PATH);
	_chdir(szWorkingDirectory);
	Fichier=fopen(szDebug,"a+");
	fprintf(Fichier,"%ld : %s\n",GetTickCount()-TimerRef,String);
	fclose(Fichier);
	bOut=TRUE;
	_chdir(szCDir);
	return;
}

void OutFileLF(void)
{
	FILE	*Fichier;
	if(!Settings.DEBUG&&!bConsole)
		return;
	if(bConsole)
	{
		DWORD txtsze=0;
		char* ConText=NULL;
		txtsze=GetWindowTextLength(GetDlgItem(hWndCon,IDC_CONOUT));
		txtsze++;
		DWORD len=strlen("\r\n");
		ConText=new char[txtsze+len];
		memset(ConText,0,txtsze+len);
		GetWindowText(GetDlgItem(hWndCon,IDC_CONOUT),ConText,txtsze);
		strcat(ConText,"\r\n");
		SetWindowText(GetDlgItem(hWndCon,IDC_CONOUT),ConText);
		SAFE_DELETE_ARRAY(ConText);
		SendDlgItemMessage(hWndCon,IDC_CONOUT,EM_SETSEL,txtsze+len,txtsze+len);
		SendDlgItemMessage(hWndCon,IDC_CONOUT,EM_SCROLLCARET,0,0);
	}

	_getcwd(szCDir,BMAX_PATH);
	_chdir(szWorkingDirectory);
	Fichier=fopen(szDebug,"a+");
	fprintf(Fichier,"\n");
	fclose(Fichier);
	bOut=TRUE;
	_chdir(szCDir);
	return;
}

void ClearOutFile(void)
{
	FILE	*Fichier;
	_getcwd(szCDir,BMAX_PATH);
	_chdir(szWorkingDirectory);
	Fichier=fopen(szDebug,"w");
	fclose(Fichier);
	bOut=0;
	_chdir(szCDir);
	return;
}

void ErrorMsg(LPCSTR Prompt,LPCSTR Title)
{
	char TmpBuff[BMAX_PATH];
	FlipToGDI();
	MessageBox(hWndWin,Prompt,Title,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);

#ifdef DX3
	lpDD->RestoreDisplayMode();
#endif
	ShowWindow(hWndWin,SW_HIDE);
	sprintf(TmpBuff,"%s: %s",Title,Prompt);
	OutFile(TmpBuff);

	CleanUp(0);
	FinalShutdown();
	exit(EXIT_FAILURE);
	return;
}

void WarningMsg(LPCSTR Prompt,LPCSTR Title)
{
	char TmpBuff[BMAX_PATH];
	FlipToGDI();
	MessageBox(hWndWin,Prompt,Title,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
	sprintf(TmpBuff,"%s: %s",Title,Prompt);
	OutFile(TmpBuff);
	return;
}

char* StringToOem(char* s)
{
	CharToOem(s,s);
	return s;
	/*
	DWORD l=strlen(s);
	for(DWORD cc=0;cc<l;cc++)
		if(!isdigit(s[cc]))
			CharToOemBuff(s+cc,s+cc,1);
	return s;
	*/
}
