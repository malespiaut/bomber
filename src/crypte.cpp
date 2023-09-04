#include "General.h"
#include "Crypte.h"
#include "Aleat.h"

const char szCryptKey[17]="BombSCRCryptKey!";

BYTE CheckSumSettings(const struct SETTINGS* pSettings)
{
	BYTE ck=0;
	BYTE* ptr=(BYTE*)pSettings;
	int cnt;
	for(cnt=0;cnt<sizeof(struct SETTINGS);cnt++)
		ck=ck^(*ptr++) +1;
	return ck;
}

BOOL DecodeScore(const struct EncRegScore* pERS,struct RegScore* pRS)
{
	int ct;
	ResetKey(szCryptKey);
	BYTE *ptr;
	BYTE cksum;
	ptr=(BYTE*)&pERS->RS;
	for(ct=0;ct<sizeof(struct RegScore);ct++)
	{
		(*ptr)=(char)DecodeChar(*ptr);
		ptr++;
	}
	*pRS=pERS->RS;

	ptr=(BYTE*)pRS;
	cksum=127;
	for(ct=0;ct<sizeof(struct RegScore);ct++)
	{
		cksum=(cksum^(*ptr)) +(BYTE)ct;
		ptr++;
	}
	if(cksum!=pERS->cksum) return 0;

	for(ct=0;ct<BMAX_NAME;ct++)
		if(pRS->Nom[ct]==0)
			return TRUE;
	return FALSE;
}

void EncodeScore(const struct RegScore* pRS,struct EncRegScore* pERS)
{
	int ct;
	ResetKey(szCryptKey);
	BYTE *ptr;
	pERS->RS=*pRS;
	pERS->cksum=127;

	ptr=(BYTE*)&pERS->RS;
	for(ct=0;ct<sizeof(struct RegScore);ct++)
	{
		pERS->cksum=(pERS->cksum^(*ptr)) +(BYTE)ct;
		ptr++;
	}

	ptr=(BYTE*)&pERS->RS;
	for(ct=0;ct<sizeof(struct RegScore);ct++)
	{
		(*ptr)=(char)EncodeChar(*ptr);
		ptr++;
	}
	return;
}

unsigned short ax,bx,cx,dx,si;
unsigned short tmp,x1a2,x1a0[8],res,ic,inter,cfc,cfd,compte;
unsigned char cle[16];

void ResetKey(const char* key)
{
memcpy(cle,key,16);
ax=0;
bx=0;
cx=0;
dx=0;
si=0;
tmp=0;
x1a2=0;
x1a0[0]=0;
x1a0[1]=0;
x1a0[2]=0;
x1a0[3]=0;
x1a0[4]=0;
res=0;
ic=0;
inter=0;
cfc=0;
cfd=0;
compte=0;
return;
}

void code(void)
{
dx=x1a2+ic;
ax=x1a0[ic];
cx=0x015a;
bx=0x4e35;

tmp=ax;
ax=si;
si=tmp;

tmp=ax;
ax=dx;
dx=tmp;

if (ax!=0)
{
ax=ax*bx;
}

tmp=ax;
ax=cx;
cx=tmp;

if (ax!=0)
{
ax=ax*si;
cx=ax+cx;
}

tmp=ax;
ax=si;
si=tmp;
ax=ax*bx;
dx=cx+dx;

ax=ax+1;

x1a2=dx;
x1a0[ic]=ax;

res=ax^dx;
ic=ic+1;
return;
}

void assemble(void)
{
x1a0[0]= ( cle[0]*256 )+ cle[1];
code();
inter=res;

x1a0[1]= x1a0[0] ^ ( (cle[2]*256) + cle[3] );
code();
inter=inter^res;

x1a0[2]= x1a0[1] ^ ( (cle[4]*256) + cle[5] );
code();
inter=inter^res;

x1a0[3]= x1a0[2] ^ ( (cle[6]*256) + cle[7] );
code();
inter=inter^res;

x1a0[4]= x1a0[3] ^ ( (cle[8]*256) + cle[9] );
code();
inter=inter^res;

x1a0[5]= x1a0[4] ^ ( (cle[10]*256) + cle[11] );
code();
inter=inter^res;

x1a0[6]= x1a0[5] ^ ( (cle[12]*256) + cle[13] );
code();
inter=inter^res;

x1a0[7]= x1a0[6] ^ ( (cle[14]*256) + cle[15] );
code();
inter=inter^res;

ic=0;
return;
}

short EncodeChar(short c)
{
assemble();
cfc=inter>>8;
cfd=inter&255;

for (compte=0;compte<=15;compte++)
	cle[compte]=cle[compte]^c;
c=c ^ (cfc^cfd);
return c;
}

short DecodeChar(short c)
{
assemble();
cfc=inter>>8;
cfd=inter&255;

c=c ^ (cfc^cfd);
for (compte=0;compte<=15;compte++)
	cle[compte]=cle[compte]^c;
return c;
}
