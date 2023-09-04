#include <windows.h>
#include <stdio.h>
#include "CSon.h"


int WINAPI WinMain( HINSTANCE hInstance, 
 // handle to current instance 
 
HINSTANCE hPrevInstance, 
 // handle to previous instance 
 
LPSTR lpCmdLine, 
 // pointer to command line 
 
int nCmdShow 
 // show state of window 
 
)
 
{
long SizeFT,SizeWAV;
char *Buff_FT,*Buff_WAV;
FILE *Fin,*Fout;

FT_Start(&SizeFT,&SizeWAV);
Buff_FT=new char[SizeFT];
Buff_WAV=new char[SizeWAV];

Fin=fopen("TestIN.ond","rb");
Fout=fopen("TestOUT.ond","wb");
if ((Fin!=0) && (Fout!=0))
{

while(fread(Buff_WAV,SizeWAV,1,Fin)!=0)
{
FT_Comp(Buff_WAV,Buff_FT);
FT_Decomp(Buff_FT,Buff_WAV);

fwrite(Buff_WAV,SizeWAV,1,Fout);

}
}
else
{
MessageBox(0,"Erreur de fichier",0,0);
}
if (Fin!=0) fclose(Fin);
if (Fout!=0) fclose(Fout);

delete[](Buff_FT);
delete[](Buff_WAV);
FT_Free();
return 0;
}