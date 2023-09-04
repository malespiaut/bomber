#ifndef _LOADFILE_H_
#define _LOADFILE_H_
BOOL DoesFileExist(const char*);
BOOL InitFileLoader(const char*,const char*,const char*,const char*);
BOOL LoadFile(const char*);
BOOL LoadRootFile(const char*);
void UnloadFile(const char*);
void CloseFileLoader(void);
#endif
