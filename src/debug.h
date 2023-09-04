#ifndef _DEBUG_H_
#define _DEBUG_H_
void OutFile(const char*);
void OutFileEx(const char*,int);
void _cdecl OutFileEx2(const char*,...);
void OutFileInt(int);
void OutFileLF(void);
void ClearOutFile(void);
void ErrorMsg(LPCSTR,LPCSTR);
void WarningMsg(LPCSTR,LPCSTR);
void EndOutFile(void);
char* strtoupper(char*);
char* StringToOem(char*);

#define SAFE_RELEASE(p) {if(p) {(p)->Release();(p)=NULL;}}
#define SAFE_DELETE(p) {if(p) {delete (p);(p)=NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p) {delete[] (p);(p)=NULL;}}
#endif
