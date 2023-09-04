#include <memory.h>
#include <winsock2.h>
#include <windows.h>
#define IDIRECTPLAY2_OR_GREATER
#include <dplay.h>
#include "WSDPlay.h"

const u_short TCP_PORT_NUMBER=2760;
const u_short UDP_PORT_NUMBER=2761;

WSDPlay::WSDPlay()
{
	WSADATA wsadata;
	WSAStartup(0x202,&wsadata);
	WSAPROTOCOL_INFO lpProto[16];
	DWORD len=16*sizeof(WSAPROTOCOL_INFO);
	WSAEnumProtocols(NULL,lpProto,&len);
	len/=sizeof(WSAPROTOCOL_INFO);
	udp=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	u_long u=1;
	ioctlsocket(udp,FIONBIO,&u);

	for(DWORD i=0;i<WSDP_MAXPLAYERS;i++)
		memset(&player[i],0,sizeof(struct WSDP_Player));
	for(i=0;i<WSDP_MAXGROUPS;i++)
		memset(&group[i],0,sizeof(struct WSDP_Group));
	NumMsg=0;
	MsgOffset[0]=0;
	MsgOffset[1]=0;
	memset(&dpDesc,0,sizeof(dpDesc));
	dpDesc.dwSize=sizeof(dpDesc);
	return;
}

WSDPlay::~WSDPlay()
{
	for(DWORD i=0;i<WSDP_MAXPLAYERS;i++)
		if(player[i].dpId)
			if(player[i].lpData)
				delete[]player[i].lpData;
	closesocket(udp);
	udp=0;
	if(IsHost)
		closesocket(tcplisten);
	WSACleanup();
	return;
}

HRESULT WSDPlay::AddPlayerToGroup(DPID idGrp,DPID idPlayer)
{
	return DP_OK;
}

HRESULT WSDPlay::Close()
{
	return DP_OK;
}

HRESULT WSDPlay::CreateGroup(LPDPID lpdpID,LPDPNAME lpName,LPVOID lpData,DWORD dwDataSize,DWORD dwFlags)
{
	return DP_OK;
}

HRESULT WSDPlay::CreatePlayer(LPDPID lpdpID,LPDPNAME lpName,HANDLE hEvent,LPVOID lpData,DWORD dwDataSize,DWORD dwFlags)
{
	return DP_OK;
}

HRESULT WSDPlay::DestroyGroup(DPID dpId)
{
	return DP_OK;
}

HRESULT WSDPlay::DestroyPlayer(DPID dpId)
{
	return DP_OK;
}

HRESULT WSDPlay::EnumConnections(LPCGUID lpAppGUID,LPDPENUMCONNECTIONSCALLBACK callback,LPVOID lpContext,DWORD dwFlags)
{
	DPNAME dpName;
	dpName.dwSize=sizeof(DPNAME);
	dpName.lpszShortNameA=dpName.lpszLongNameA="WSDPplay Wrapper (Winsock 2.0 sur UDP)";
	dpName.dwFlags=0;
	DWORD dwEF=0;
	callback(&GUID_NULL,"WSDP",5,&dpName,dwEF,lpContext);
	return DP_OK;
}

HRESULT WSDPlay::EnumPlayers(LPGUID sessGUID,LPDPENUMPLAYERSCALLBACK2 callback,LPVOID lpContext,DWORD dwFlags)
{
	return DP_OK;
}

HRESULT WSDPlay::EnumSessions(LPDPSESSIONDESC2 lpdpDesc,DWORD dwTimeOut,LPDPENUMSESSIONSCALLBACK2 callback,LPVOID lpContext,DWORD dwFlags)
{
	return DP_OK;
}

HRESULT WSDPlay::GetCaps(LPDPCAPS lpdpCaps,DWORD dwFlags)
{
	if(lpdpCaps->dwSize!=sizeof(DPCAPS))
		return DPERR_INVALIDPARAMS;
	memset(lpdpCaps,0,sizeof(DPCAPS));
	lpdpCaps->dwSize=sizeof(DPCAPS);
	lpdpCaps->dwHeaderLength=20;
	lpdpCaps->dwHundredBaud=0;
	lpdpCaps->dwLatency=500;
	lpdpCaps->dwMaxPlayers=WSDP_MAXPLAYERS;
	lpdpCaps->dwMaxLocalPlayers=WSDP_MAXPLAYERS;
	return DP_OK;
}

HRESULT WSDPlay::GetMessageCount(DPID dpId,LPDWORD lpCount)
{
	*lpCount=0;
	return DP_OK;
}

HRESULT WSDPlay::GetMessageQueue(DPID idFrom,DPID idTo,DWORD dwFlags,LPDWORD lpNumMsg,LPDWORD lpNumBytes)
{
	*lpNumMsg=*lpNumBytes=0;
	return DP_OK;
}

HRESULT WSDPlay::GetPlayerData(DPID dpId,LPVOID lpData,LPDWORD lpdwSize,DWORD dwFlags)
{
	return DP_OK;
}

HRESULT WSDPlay::GetPlayerFlags(DPID dpId,LPDWORD lpdwFlags)
{
	return DP_OK;
}

HRESULT WSDPlay::GetSessionDesc(LPVOID lpdpDesc,LPDWORD lpdwSize)
{
	if(lpdwSize==NULL)
		return DPERR_INVALIDPARAMS;
	if((lpdpDesc==NULL)||(*lpdwSize<sizeof(dpDesc)))
	{
		*lpdwSize=sizeof(dpDesc);
		return DPERR_BUFFERTOOSMALL;
	}
	*(LPDPSESSIONDESC2)lpdpDesc=dpDesc;
	return DP_OK;
}

HRESULT WSDPlay::InitializeConnection(LPVOID lpConnection,DWORD dwFlags)
{
	if(strcmp((char*)lpConnection,"WSDP"))
		return DPERR_INVALIDPARAMS;
	SOCKADDR_IN sa;
	memset(&sa,0,sizeof(sa));
	sa.sin_port=htons(UDP_PORT_NUMBER);
	sa.sin_family=AF_INET;
	sa.sin_addr.s_addr=htonl(INADDR_ANY);
	bind(udp,(SOCKADDR*)&sa,sizeof(sa));
	return DP_OK;
}

HRESULT WSDPlay::Open(LPDPSESSIONDESC2 lpdpDesc,DWORD dwFlags)
{
	dpDesc=*lpdpDesc;
	if(dwFlags&DPOPEN_CREATE)
	{
		tcplisten=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		SOCKADDR_IN sa;
		memset(&sa,0,sizeof(sa));
		sa.sin_port=htons(TCP_PORT_NUMBER);
		sa.sin_family=AF_INET;
		sa.sin_addr.s_addr=htonl(INADDR_ANY);
		bind(tcplisten,(SOCKADDR*)&sa,sizeof(sa));
		u_long u=1;
		ioctlsocket(tcplisten,FIONBIO,&u);
		listen(tcplisten,3);
		IsHost=TRUE;
	}
	else
	{
		IsHost=FALSE;
	}
	return DP_OK;
}

HRESULT WSDPlay::Receive(LPDPID lpIdFrom,LPDPID lpidTo,DWORD dwFlags,LPVOID lpData,LPDWORD lpdwSize)
{
	return DPERR_NOMESSAGES;
}

HRESULT WSDPlay::Release()
{
	delete this;
	return 0;
}

HRESULT WSDPlay::SendChatMessage(DPID idFrom,DPID idTo,DWORD dwFlags,LPDPCHAT lpMessage)
{
	return DP_OK;
}

HRESULT WSDPlay::SendEx(DPID idFrom,DPID idTo,DWORD dwFlags,LPVOID lpData,DWORD dwSize,DWORD,DWORD,LPVOID,LPDWORD)
{
	return DP_OK;
}

HRESULT WSDPlay::SetPlayerData(DPID dpId,LPVOID lpData,DWORD dwSize,DWORD dwFlags)
{
	return DP_OK;
}

HRESULT WSDPlay::SetSessionDesc(LPDPSESSIONDESC2 lpdpDesc,DWORD dwFlags)
{
	dpDesc=*lpdpDesc;
	return DP_OK;
}

void WSDPlay::Update(void)
{
	if(NumMsg>=WSDP_MAXMSG-1)
		return; //Overrun
	if(MsgOffset[NumMsg+1]>=WSDP_BUFSIZE-1)
		return; //Overrun
	SOCKADDR_IN from;
	int fromlen=sizeof(from);
	int lng=recvfrom(udp,(char*)&Queue[MsgOffset[NumMsg]],WSDP_BUFSIZE-MsgOffset[NumMsg],0,(SOCKADDR*)&from,&fromlen);
	if(lng==SOCKET_ERROR)
		return;
	MsgOffset[NumMsg+1]=MsgOffset[NumMsg]+lng;
	NumMsg++;
	for(int pli=0;pli<WSDP_MAXPLAYERS;pli++)
		if(player[pli].dpId)
			if(memcmp(&from,&player[pli].addr,sizeof(from))==0)
				return;

// Ajoute un joueur
	DPMSG_CREATEPLAYERORGROUP dpMsg;
	dpMsg.dwType=DPSYS_CREATEPLAYERORGROUP;
	dpMsg.dpId=*(DPID*)&Queue[MsgOffset[NumMsg-1]];
	dpMsg.dpIdParent=0;
	return;
}
