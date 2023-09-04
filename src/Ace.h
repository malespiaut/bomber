#ifndef _ACE_H_
#define _ACE_H_

#ifdef GCOMP_ACE
BOOL INLINE ACEGetOneFile(const char*,const char*,const char*);
BOOL INLINE ACEGetFile(const char*,const char*,const char*);
void INLINE ACEUnGetFile(const char*);
BOOL INLINE ACELoadDLL(void);
void INLINE ACEFreeDLL(void);
#else
#define ACEGetOneFile
#define ACEGetFile
#define ACEUnGetFile
#define ACELoadDLL
#define ACEFreeDLL
#endif
#endif
