#include <windows.h>

DWORD CurrentAleat;
double InvDWORD = 1.0/65536.0/65536.0;

void InitAleat(DWORD FirstVal)
{
	CurrentAleat=FirstVal;
	return;
}

double Aleat(double min,double max)
{
return min+(max-min)*InvDWORD*(double)(CurrentAleat=(1+(CurrentAleat*16807)));
}

int randInt( int low, int high )
{
	int range = 1+ high - low;
	int num = rand() % range;
	return( num + low );
}
