//	指定した範囲のIPアドレスにPINGを実行し、レスポンスがあればマックアドレスとホスト名を表示
//	2014/07/20

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <locale.h>
#include <tchar.h>

#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")	//	gethostbyaddr関数に必要

#define BUF_SIZE 32

void _tmain(void){
	_tsetlocale(LC_ALL,_TEXT(""));	//	UNICODE文字を標準出力に正しく表示できるように設定
	HANDLE hIcmp;
	IPAddr ipaddr;

	hIcmp=IcmpCreateFile();
	char cbRequest[BUF_SIZE];	//	送信するデータ
	memset(cbRequest,'a',BUF_SIZE);	//	送信するデータを設定
	DWORD cbReply=sizeof(ICMP_ECHO_REPLY)+BUF_SIZE;
	char* pReply=new char[cbReply];	//	受信するヘッダー + 受信するデータ
	PICMP_ECHO_REPLY p=(PICMP_ECHO_REPLY)pReply;

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);	//	WINSOCKを初期化

	unsigned char* ip=(unsigned char*)&ipaddr;
	_tprintf(_TEXT("IPアドレス      time  TTL マックアドレス    ホスト名\n"));

	for(unsigned n=0;n<=254;n++){	//	IPアドレスの検索範囲を設定している
		ip[0]=192;	//	IPアドレス	ビックエンディアンで設定
		ip[1]=168;
		ip[2]=0;
		ip[3]=n;
		_tprintf(_TEXT("%3i.%3i.%3i.%3i\r"),ip[0],ip[1],ip[2],ip[3]);
		// pingを実行
		//                           IPアドレス 送信データ データーサイズ                     タイムアウトms
		DWORD ret=IcmpSendEcho( hIcmp ,ipaddr , cbRequest ,BUF_SIZE , NULL , pReply , cbReply , 2000);
		if(ret){	//	pingが成功した
			unsigned char mac[6];
			struct hostent* hostp;
			ULONG len=sizeof(mac)/sizeof(unsigned char);
			if(SendARP(ipaddr,NULL,mac,&len)==NO_ERROR){	//	IPアドレスからmacアドレスを取得
				hostp=gethostbyaddr((const char*)ip,4,AF_INET);	//	IPアドレスからホスト名を取得
				TCHAR buf[64];
				if(hostp)
#ifdef UNICODE
					//	UNICODEに変換
					MultiByteToWideChar(932,0,hostp->h_name,-1,buf,sizeof(buf)/sizeof(TCHAR));
#else
					_tcscpy_s(buf,sizeof(buf)/sizeof(TCHAR),hostp->h_name);
					
#endif
				else
					buf[0]=_T('\0');
				_tprintf(_TEXT("%3i.%3i.%3i.%3i "),ip[0],ip[1],ip[2],ip[3]);
			_tprintf(_TEXT("%4d %4d %02x:%02x:%02x:%02x:%02x:%02x %s\n"),p->RoundTripTime,p->Options.Ttl,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],buf);
			}

		}else{	//	pingに失敗した場合
		}
	}
	delete [] pReply;
	IcmpCloseHandle(hIcmp);
	WSACleanup();
}



void DispResInfo()
{
	NETRESOURCE SrcNr;
	LPNETRESOURCE pDestNr;
	DWORD DestNrSize = 1000;
	char *pSystem;

	DWORD lResult;

	SrcNr.lpProvider = "Microsoft Windows Network";
	SrcNr.dwType = RESOURCETYPE_DISK;
	SrcNr.lpRemoteName = "\\\\foo\\bar\\abc";

	pDestNr = (LPNETRESOURCE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, DestNrSize);

	lResult = WNetGetResourceInformation(&SrcNr, pDestNr, &DestNrSize, &pSystem);
	
	if(lResult == NO_ERROR) {
		printf("ネットワーク名: %s\n", pDestNr->lpRemoteName);
		printf("ネットワークプロバイダ: %s\n", pDestNr->lpProvider);
		printf("リソースのタイプ: ");
		switch(pDestNr->dwType) {
			case RESOURCETYPE_ANY: printf("全てのリソース\n"); break;
			case RESOURCETYPE_DISK: printf("ディスクリソース\n"); break;
			case RESOURCETYPE_PRINT: printf("印刷リソース\n"); break;
			default: printf("%08x\n", pDestNr->dwType);
		}
		printf("表示方法: ");
		switch(pDestNr->dwDisplayType) {
			case RESOURCEDISPLAYTYPE_ROOT: printf("ルートネットワーク\n"); break;
			case RESOURCEDISPLAYTYPE_NETWORK: printf("ネットワーク\n"); break;
			case RESOURCEDISPLAYTYPE_DOMAIN: printf("オブジェクトをドメインとして表示\n"); break;
			case RESOURCEDISPLAYTYPE_SERVER: printf("オブジェクトをサーバとして表示\n"); break;
			case RESOURCEDISPLAYTYPE_SHARE: printf("オブジェクトを共有ポイントとして表示\n"); break;
			case RESOURCEDISPLAYTYPE_DIRECTORY: printf("ディレクトリ\n"); break;
			case RESOURCEDISPLAYTYPE_GENERIC: printf("オブジェクトの表示方法は任意\n"); break;
			default: printf("%08x\n", pDestNr->dwDisplayType);
		}
		printf("リソースの使用方法(%08x):\n", pDestNr->dwUsage);
		if(pDestNr->dwUsage & RESOURCEUSAGE_CONNECTABLE) printf("\t接続可能リソース\n");
		if(pDestNr->dwUsage & RESOURCEUSAGE_CONTAINER) printf("\tコンテナリソース\n");
		if(pDestNr->dwUsage & RESOURCEUSAGE_NOLOCALDEVICE) printf("\tローカルデバイスに割り当てること無く使用可能\n");
		printf("共有名を除いたパス名: %s\n", pSystem);

	} else {
		printf("エラー発生(%d).\n");
	}

	GlobalFree(pDestNr);
}


void ConvertUNC()
{
	LPREMOTE_NAME_INFO pInfo;
	DWORD InfoSize = 1000;
	
	pInfo = GlobalAlloc(GMEM_FIXED, InfoSize);

	WNetGetUniversalName("s:\\hogehoge.txt", REMOTE_NAME_INFO_LEVEL, pInfo, &InfoSize);

	printf("UNC名: %s\n", pInfo->lpUniversalName);
	printf("ネットワークリソース名: %s\n", pInfo->lpConnectionName);
	printf("ドライブ名を除いたパス: %s\n", pInfo->lpRemainingPath);

	GlobalFree(pInfo);
}
