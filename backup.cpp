//
//char recvBuf[1024];
//SOCKET sockConn;
///**
//* ��һ���µ��߳������������
//*/
//DWORD WINAPI Fun(LPVOID lpParamter)
//{
//	while (true) {
//		memset(recvBuf, 0, sizeof(recvBuf));
//		//��������
//		recv(sockConn, recvBuf, sizeof(recvBuf), 0);
//		//printf("%s\n", recvBuf);
//		cout << recvBuf << endl;
//	}
//	closesocket(sockConn);
//}

//int receiveData() {
//	WSADATA wsaData;
//	int port = 6666;//�˿ں�
//	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//	{
//		printf("��ʼ��ʧ��");
//		return 0;
//	}
//
//	//�������ڼ������׽���,������˵��׽���
//	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
//
//	SOCKADDR_IN addrSrv;
//	addrSrv.sin_family = AF_INET;
//	addrSrv.sin_port = htons(port); //1024���ϵĶ˿ں�
//									/**
//									* INADDR_ANY����ָ����ַΪ0.0.0.0�ĵ�ַ�������ַ��ʵ�ϱ�ʾ��ȷ����ַ�������е�ַ�����������ַ���� һ����˵���ڸ���ϵͳ�о������Ϊ0ֵ��
//									*/
//	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//
//	int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
//	if (retVal == SOCKET_ERROR) {
//		printf("����ʧ��:%d\n", WSAGetLastError());
//		return 0;
//	}
//
//	if (listen(sockSrv, 10) == SOCKET_ERROR) {
//		printf("����ʧ��:%d", WSAGetLastError());
//		return 0;
//	}
//
//	SOCKADDR_IN addrClient;
//	int len = sizeof(SOCKADDR);
//
//	while (1)
//	{
//		//�ȴ��ͻ�������
//		sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
//		if (sockConn == SOCKET_ERROR) {
//			printf("�ȴ�����ʧ��:%d", WSAGetLastError());
//			break;
//		}
//
//		printf("�ͻ��˵�IP��:[%s]\n", inet_ntoa(addrClient.sin_addr));
//
//		////��������
//		//char sendbuf[] = "��ã����Ƿ���ˣ�����һ�������";
//		//int iSend = send(sockConn, sendbuf, sizeof(sendbuf), 0);
//		//if (iSend == SOCKET_ERROR) {
//		//	printf("����ʧ��");
//		//	break;
//		//}
//
//		HANDLE hThread = CreateThread(NULL, 0, Fun, NULL, 0, NULL);
//		CloseHandle(hThread);
//	}
//
//	closesocket(sockSrv);
//	WSACleanup();
//	system("pause");
//	return 0;
//}

//void startFFmpeg() {
//	TCHAR szCommandLine[] = TEXT("ffmpeg -f gdigrab -framerate 30 -offset_x 10 -offset_y 20 -video_size 640x480 -i desktop -f h264 tcp://localhost:6666");//����WCHAR
//	STARTUPINFO si;
//	PROCESS_INFORMATION pi;
//	ZeroMemory(&si, sizeof(si));
//	si.cb = sizeof(si);
//	ZeroMemory(&pi, sizeof(pi));
//
//	si.dwFlags = STARTF_USESHOWWINDOW;  // ָ��wShowWindow��Ա��Ч
//	si.wShowWindow = TRUE;          // �˳�Ա��ΪTRUE�Ļ�����ʾ�½����̵������ڣ�
//									// ΪFALSE�Ļ�����ʾ
//	BOOL bRet = ::CreateProcess(
//		NULL,           // ���ڴ�ָ����ִ���ļ����ļ���
//		szCommandLine,      // �����в���
//		NULL,           // Ĭ�Ͻ��̰�ȫ��
//		NULL,           // Ĭ���̰߳�ȫ��
//		FALSE,          // ָ����ǰ�����ڵľ�������Ա��ӽ��̼̳�
//		0,//CREATE_NEW_CONSOLE, // Ϊ�½��̴���һ���µĿ���̨����
//		NULL,           // ʹ�ñ����̵Ļ�������
//		NULL,           // ʹ�ñ����̵���������Ŀ¼
//		&si,
//		&pi);
//
//	if (bRet)
//	{
//		//WaitForSingleObject(pi.hProcess, INFINITE);
//		// ��Ȼ���ǲ�ʹ�������������������̽����ǹر�
//		::CloseHandle(pi.hThread);
//		::CloseHandle(pi.hProcess);
//
//		printf(" �½��̵Ľ���ID�ţ�%d \n", pi.dwProcessId);
//		printf(" �½��̵����߳�ID�ţ�%d \n", pi.dwThreadId);
//	}
//}

//void getVideoInfo(char* filepath) {
//	//FFmpeg��ر���
//	AVFormatContext * pFormatCtx;//�洢����Ƶ��װ��ʽ��������Ϣ
//	//char *filepath = "1.mp4";
//	av_register_all();//��ʼ��libformat��ȹ���
//	pFormatCtx = avformat_alloc_context();//����formatcontext�����ڴ�
//	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {
//		printf("Couldn't open input stream.\n");
//		return;
//	}
//	cout << "�ļ����� " << pFormatCtx->filename << endl;
//	cout << "��������" << pFormatCtx->nb_streams << endl;
//	printf("ʱ�� %d ��\n", pFormatCtx->duration/1000000);
//	//printf("�ܱ����ʣ� %d kbps\n", pFormatCtx->bit_rate);//��ʱû�취��ȡ
//
//	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {//��ȡ����Ϣ
//		printf("Couldn't found stream information.\n");
//		return ;
//	}
//	printf("�ܱ����ʣ�%d kbps\n", pFormatCtx->bit_rate / 1000);
//	avformat_close_input(&pFormatCtx);//�ر�����
//}