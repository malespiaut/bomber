#include "General.h"
#include "Debug.h"

#undef PAK_RAR
#undef PAK_ACE
#undef PAK_UCL
#undef PAK_LZO
#undef PAK_BZ

//#define PAK_RAR
//#define PAK_ACE

#ifdef GCOMP_BZ
#define PAK_BZ
#endif
#ifdef GCOMP_UCL
#define PAK_UCL
#endif
#ifdef GCOMP_LZO
#define PAK_LZO
#endif

#ifdef PAK_BZ
#include "pBZ.h"
#endif

#ifdef PAK_LZO
#include "pLZO.h"
#endif

#ifdef PAK_UCL
#include "pUCL.h"
#endif

#ifdef PAK_RAR
#include "RAR.h"
#endif

#ifdef PAK_ACE
#include "ACE.h"
#endif

BOOL PAKLoadDLL()
{
#ifdef PAK_BZ
	if(!BZLoadDLL()) return 0;
#endif
#ifdef PAK_LZO
	if(!LZOLoadDLL()) return 0;
#endif
#ifdef PAK_UCL
	if(!UCLLoadDLL()) return 0;
#endif
#ifdef PAK_RAR
	if(!RARLoadDLL()) return 0;
#endif
#ifdef PAK_ACE
	if(!ACELoadDLL()) return 0;
#endif
	return TRUE;
}

void PAKFreeDLL()
{
#ifdef PAK_BZ
	BZFreeDLL();
#endif
#ifdef PAK_LZO
	LZOFreeDLL();
#endif
#ifdef PAK_UCL
	UCLFreeDLL();
#endif
#ifdef PAK_RAR
	RARFreeDLL();
#endif
#ifdef PAK_ACE
	ACEFreeDLL();
#endif
	return;
}

BOOL PAKGetFile(const char* ArcName,const char* FileName,const char *Password)
{
#ifdef PAK_BZ
	if(BZGetFile(ArcName,FileName,Password))
		return TRUE;
#endif
#ifdef PAK_LZO
	if(LZOGetFile(ArcName,FileName,Password))
		return TRUE;
#endif
#ifdef PAK_UCL
	if(UCLGetFile(ArcName,FileName,Password))
		return TRUE;
#endif
#ifdef PAK_RAR
	if(RARGetFile(ArcName,FileName,Password))
		return TRUE;
#endif
#ifdef PAK_ACE
	if(ACEGetFile(ArcName,FileName,Password))
		return TRUE;
#endif
	return 0;
}

BOOL PAKGetOneFile(const char* ArcName,const char* FileName,const char *Password)
{
#ifdef PAK_BZ
	if(BZGetOneFile(ArcName,FileName,Password))
		return TRUE;
#endif
#ifdef PAK_LZO
	if(LZOGetOneFile(ArcName,FileName,Password))
		return TRUE;
#endif
#ifdef PAK_UCL
	if(UCLGetOneFile(ArcName,FileName,Password))
		return TRUE;
#endif
#ifdef PAK_RAR
	if(RARGetOneFile(ArcName,FileName,Password))
		return TRUE;
#endif
#ifdef PAK_ACE
	if(ACEGetOneFile(ArcName,FileName,Password))
		return TRUE;
#endif
	return 0;
}

void PAKUnGetFile(const char* FileName)
{
#ifdef PAK_BZ
	BZUnGetFile(FileName);
#endif
#ifdef PAK_LZO
	LZOUnGetFile(FileName);
#endif
#ifdef PAK_UCL
	UCLUnGetFile(FileName);
#endif
#ifdef PAK_RAR
	RARUnGetFile(FileName);
#endif
#ifdef PAK_ACE
	ACEUnGetFile(FileName);
#endif
	return;
}
