//
//char recvBuf[1024];
//SOCKET sockConn;
///**
//* 在一个新的线程里面接收数据
//*/
//DWORD WINAPI Fun(LPVOID lpParamter)
//{
//	while (true) {
//		memset(recvBuf, 0, sizeof(recvBuf));
//		//接收数据
//		recv(sockConn, recvBuf, sizeof(recvBuf), 0);
//		//printf("%s\n", recvBuf);
//		cout << recvBuf << endl;
//	}
//	closesocket(sockConn);
//}

//int receiveData() {
//	WSADATA wsaData;
//	int port = 6666;//端口号
//	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//	{
//		printf("初始化失败");
//		return 0;
//	}
//
//	//创建用于监听的套接字,即服务端的套接字
//	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
//
//	SOCKADDR_IN addrSrv;
//	addrSrv.sin_family = AF_INET;
//	addrSrv.sin_port = htons(port); //1024以上的端口号
//									/**
//									* INADDR_ANY就是指定地址为0.0.0.0的地址，这个地址事实上表示不确定地址，或“所有地址”、“任意地址”。 一般来说，在各个系统中均定义成为0值。
//									*/
//	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//
//	int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
//	if (retVal == SOCKET_ERROR) {
//		printf("连接失败:%d\n", WSAGetLastError());
//		return 0;
//	}
//
//	if (listen(sockSrv, 10) == SOCKET_ERROR) {
//		printf("监听失败:%d", WSAGetLastError());
//		return 0;
//	}
//
//	SOCKADDR_IN addrClient;
//	int len = sizeof(SOCKADDR);
//
//	while (1)
//	{
//		//等待客户请求到来
//		sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
//		if (sockConn == SOCKET_ERROR) {
//			printf("等待请求失败:%d", WSAGetLastError());
//			break;
//		}
//
//		printf("客户端的IP是:[%s]\n", inet_ntoa(addrClient.sin_addr));
//
//		////发送数据
//		//char sendbuf[] = "你好，我是服务端，咱们一起聊天吧";
//		//int iSend = send(sockConn, sendbuf, sizeof(sendbuf), 0);
//		//if (iSend == SOCKET_ERROR) {
//		//	printf("发送失败");
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
//	TCHAR szCommandLine[] = TEXT("ffmpeg -f gdigrab -framerate 30 -offset_x 10 -offset_y 20 -video_size 640x480 -i desktop -f h264 tcp://localhost:6666");//或者WCHAR
//	STARTUPINFO si;
//	PROCESS_INFORMATION pi;
//	ZeroMemory(&si, sizeof(si));
//	si.cb = sizeof(si);
//	ZeroMemory(&pi, sizeof(pi));
//
//	si.dwFlags = STARTF_USESHOWWINDOW;  // 指定wShowWindow成员有效
//	si.wShowWindow = TRUE;          // 此成员设为TRUE的话则显示新建进程的主窗口，
//									// 为FALSE的话则不显示
//	BOOL bRet = ::CreateProcess(
//		NULL,           // 不在此指定可执行文件的文件名
//		szCommandLine,      // 命令行参数
//		NULL,           // 默认进程安全性
//		NULL,           // 默认线程安全性
//		FALSE,          // 指定当前进程内的句柄不可以被子进程继承
//		0,//CREATE_NEW_CONSOLE, // 为新进程创建一个新的控制台窗口
//		NULL,           // 使用本进程的环境变量
//		NULL,           // 使用本进程的驱动器和目录
//		&si,
//		&pi);
//
//	if (bRet)
//	{
//		//WaitForSingleObject(pi.hProcess, INFINITE);
//		// 既然我们不使用两个句柄，最好是立刻将它们关闭
//		::CloseHandle(pi.hThread);
//		::CloseHandle(pi.hProcess);
//
//		printf(" 新进程的进程ID号：%d \n", pi.dwProcessId);
//		printf(" 新进程的主线程ID号：%d \n", pi.dwThreadId);
//	}
//}

//void getVideoInfo(char* filepath) {
//	//FFmpeg相关变量
//	AVFormatContext * pFormatCtx;//存储音视频封装格式包含的信息
//	//char *filepath = "1.mp4";
//	av_register_all();//初始化libformat库等工作
//	pFormatCtx = avformat_alloc_context();//分配formatcontext所需内存
//	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {
//		printf("Couldn't open input stream.\n");
//		return;
//	}
//	cout << "文件名： " << pFormatCtx->filename << endl;
//	cout << "流个数：" << pFormatCtx->nb_streams << endl;
//	printf("时长 %d 秒\n", pFormatCtx->duration/1000000);
//	//printf("总比特率： %d kbps\n", pFormatCtx->bit_rate);//此时没办法获取
//
//	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {//读取流信息
//		printf("Couldn't found stream information.\n");
//		return ;
//	}
//	printf("总比特率：%d kbps\n", pFormatCtx->bit_rate / 1000);
//	avformat_close_input(&pFormatCtx);//关闭输入
//}