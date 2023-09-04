#ifndef _PAK_H_
#define _PAK_H_
BOOL PAKGetOneFile(const char*,const char*,const char*);
BOOL PAKGetFile(const char*,const char*,const char*);
void PAKUnGetFile(const char*);
BOOL PAKLoadDLL(void);
void PAKFreeDLL(void);
#endif
