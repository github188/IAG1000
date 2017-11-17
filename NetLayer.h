#include<WinSock2.h>
#pragma comment( lib, "Ws2_32.lib")
enum
{
	MainData=10,
	SubData,
};
typedef bool (__stdcall *fMessCallBack)(long lLoginID, char *pBuf,
									   unsigned long dwBufLen, long dwUser);

typedef struct 
{
//	bool InitConnect();
//	bool CreatSocket();
	SOCKET          conn_socket;
	SOCKET			rt_socket; 
	SOCKET			pb_socket;
	SOCKET			m_ClientSocket;//连接上客户端主socket
	SOCKET			m_ClientSubSocket;//连接上客户端子socket
	HANDLE m_handThread;
	bool m_bExit;//退出

//	bool ResvM();
//	bool ResvSub();
//	bool Send(char* bufIN,long len);
	fMessCallBack m_MainCallBack;
	fMessCallBack m_SubCallBack;
	long m_userData;
	//bool SetCallBack(fMessCallBack Mcnnect ,fMessCallBack subConnect,long userData);
}NetLayer;
