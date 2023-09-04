#ifndef _RAR_H
#define _RAR_H
#ifdef GCOMP_RAR
BOOL RARGetOneFile(const char*,const char*,const char*);
BOOL RARGetFile(const char*,const char*,const char*);
void RARUnGetFile(const char*);
BOOL RARLoadDLL(void);
void RARFreeDLL(void);
#else
#define RARGetOneFile
#define RARGetFile
#define RARUnGetFile
#define RARLoadDLL
#define RARFreeDLL
#endif
#endif
