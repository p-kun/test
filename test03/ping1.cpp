//	�w�肵���͈͂�IP�A�h���X��PING�����s���A���X�|���X������΃}�b�N�A�h���X�ƃz�X�g����\��
//	2014/07/20

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <locale.h>
#include <tchar.h>

#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")	//	gethostbyaddr�֐��ɕK�v

#define BUF_SIZE 32

void _tmain(void){
	_tsetlocale(LC_ALL,_TEXT(""));	//	UNICODE������W���o�͂ɐ������\���ł���悤�ɐݒ�
	HANDLE hIcmp;
	IPAddr ipaddr;

	hIcmp=IcmpCreateFile();
	char cbRequest[BUF_SIZE];	//	���M����f�[�^
	memset(cbRequest,'a',BUF_SIZE);	//	���M����f�[�^��ݒ�
	DWORD cbReply=sizeof(ICMP_ECHO_REPLY)+BUF_SIZE;
	char* pReply=new char[cbReply];	//	��M����w�b�_�[ + ��M����f�[�^
	PICMP_ECHO_REPLY p=(PICMP_ECHO_REPLY)pReply;

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);	//	WINSOCK��������

	unsigned char* ip=(unsigned char*)&ipaddr;
	_tprintf(_TEXT("IP�A�h���X      time  TTL �}�b�N�A�h���X    �z�X�g��\n"));

	for(unsigned n=0;n<=254;n++){	//	IP�A�h���X�̌����͈͂�ݒ肵�Ă���
		ip[0]=192;	//	IP�A�h���X	�r�b�N�G���f�B�A���Őݒ�
		ip[1]=168;
		ip[2]=0;
		ip[3]=n;
		_tprintf(_TEXT("%3i.%3i.%3i.%3i\r"),ip[0],ip[1],ip[2],ip[3]);
		// ping�����s
		//                           IP�A�h���X ���M�f�[�^ �f�[�^�[�T�C�Y                     �^�C���A�E�gms
		DWORD ret=IcmpSendEcho( hIcmp ,ipaddr , cbRequest ,BUF_SIZE , NULL , pReply , cbReply , 2000);
		if(ret){	//	ping����������
			unsigned char mac[6];
			struct hostent* hostp;
			ULONG len=sizeof(mac)/sizeof(unsigned char);
			if(SendARP(ipaddr,NULL,mac,&len)==NO_ERROR){	//	IP�A�h���X����mac�A�h���X���擾
				hostp=gethostbyaddr((const char*)ip,4,AF_INET);	//	IP�A�h���X����z�X�g�����擾
				TCHAR buf[64];
				if(hostp)
#ifdef UNICODE
					//	UNICODE�ɕϊ�
					MultiByteToWideChar(932,0,hostp->h_name,-1,buf,sizeof(buf)/sizeof(TCHAR));
#else
					_tcscpy_s(buf,sizeof(buf)/sizeof(TCHAR),hostp->h_name);
					
#endif
				else
					buf[0]=_T('\0');
				_tprintf(_TEXT("%3i.%3i.%3i.%3i "),ip[0],ip[1],ip[2],ip[3]);
			_tprintf(_TEXT("%4d %4d %02x:%02x:%02x:%02x:%02x:%02x %s\n"),p->RoundTripTime,p->Options.Ttl,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],buf);
			}

		}else{	//	ping�Ɏ��s�����ꍇ
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
		printf("�l�b�g���[�N��: %s\n", pDestNr->lpRemoteName);
		printf("�l�b�g���[�N�v���o�C�_: %s\n", pDestNr->lpProvider);
		printf("���\�[�X�̃^�C�v: ");
		switch(pDestNr->dwType) {
			case RESOURCETYPE_ANY: printf("�S�Ẵ��\�[�X\n"); break;
			case RESOURCETYPE_DISK: printf("�f�B�X�N���\�[�X\n"); break;
			case RESOURCETYPE_PRINT: printf("������\�[�X\n"); break;
			default: printf("%08x\n", pDestNr->dwType);
		}
		printf("�\�����@: ");
		switch(pDestNr->dwDisplayType) {
			case RESOURCEDISPLAYTYPE_ROOT: printf("���[�g�l�b�g���[�N\n"); break;
			case RESOURCEDISPLAYTYPE_NETWORK: printf("�l�b�g���[�N\n"); break;
			case RESOURCEDISPLAYTYPE_DOMAIN: printf("�I�u�W�F�N�g���h���C���Ƃ��ĕ\��\n"); break;
			case RESOURCEDISPLAYTYPE_SERVER: printf("�I�u�W�F�N�g���T�[�o�Ƃ��ĕ\��\n"); break;
			case RESOURCEDISPLAYTYPE_SHARE: printf("�I�u�W�F�N�g�����L�|�C���g�Ƃ��ĕ\��\n"); break;
			case RESOURCEDISPLAYTYPE_DIRECTORY: printf("�f�B���N�g��\n"); break;
			case RESOURCEDISPLAYTYPE_GENERIC: printf("�I�u�W�F�N�g�̕\�����@�͔C��\n"); break;
			default: printf("%08x\n", pDestNr->dwDisplayType);
		}
		printf("���\�[�X�̎g�p���@(%08x):\n", pDestNr->dwUsage);
		if(pDestNr->dwUsage & RESOURCEUSAGE_CONNECTABLE) printf("\t�ڑ��\���\�[�X\n");
		if(pDestNr->dwUsage & RESOURCEUSAGE_CONTAINER) printf("\t�R���e�i���\�[�X\n");
		if(pDestNr->dwUsage & RESOURCEUSAGE_NOLOCALDEVICE) printf("\t���[�J���f�o�C�X�Ɋ��蓖�Ă邱�Ɩ����g�p�\\n");
		printf("���L�����������p�X��: %s\n", pSystem);

	} else {
		printf("�G���[����(%d).\n");
	}

	GlobalFree(pDestNr);
}


void ConvertUNC()
{
	LPREMOTE_NAME_INFO pInfo;
	DWORD InfoSize = 1000;
	
	pInfo = GlobalAlloc(GMEM_FIXED, InfoSize);

	WNetGetUniversalName("s:\\hogehoge.txt", REMOTE_NAME_INFO_LEVEL, pInfo, &InfoSize);

	printf("UNC��: %s\n", pInfo->lpUniversalName);
	printf("�l�b�g���[�N���\�[�X��: %s\n", pInfo->lpConnectionName);
	printf("�h���C�u�����������p�X: %s\n", pInfo->lpRemainingPath);

	GlobalFree(pInfo);
}
