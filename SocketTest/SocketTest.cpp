// SocketTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include "udp.h"
#include <stdio.h>
#define __STDC_CONSTANT_MACROS

#include <ctime>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include "winsock2.h"
#include <windows.h>
#include <string>
#define SDL_MAIN_HANDLED

#define DECODE_FMT_RGB24 1

//#ifndef _WIN32

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavdevice/avdevice.h"
#include "SDL/SDL.h"
}
//#endif // !_WIN32

using namespace std;

//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)
#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit = 0;
int thread_pause = 0;
const int BLOCK_W = 20;
const int BLOCK_H = 20;
int blockBytes;//每个数据块包含字节大小
const int WIDTH = 640;
const int HEIGHT = 480;

int sfp_refresh_thread(void *opaque)
{
	thread_exit = 0;
	while (!thread_exit) {
		if (!thread_pause) {
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);
		}
		SDL_Delay(10);
	}
	thread_exit = 0;
	thread_pause = 0;
	//Break
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);

	return 0;
}
void initSocket();
void sendFrame(uint8_t* data,int lineSize);
void releaseSocket();

int main(int argc, char* argv[]) {
	
	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame, *pFrameYUV;
	//unsigned char *out_buffer;
	AVPacket *packet;
	int ret, got_picture;
	//-------------SDL---------------
	int screen_w, screen_h;
	SDL_Window *screen;
	SDL_Renderer *sdlRenderer;
	SDL_Texture *sdlTexture;
	SDL_Rect sdlRect;
	SDL_Thread *video_tid;
	SDL_Event event;

	struct SwsContext *img_convert_ctx;

	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	//Register Device
	avdevice_register_all();
	//Windows
#ifdef _WIN32
#if USE_DSHOW
	//Use dshow
	//
	//Need to Install screen-capture-recorder
	//screen-capture-recorder
	//Website: http://sourceforge.net/projects/screencapturer/
	//
	AVInputFormat *ifmt = av_find_input_format("dshow");
	if (avformat_open_input(&pFormatCtx, "video=screen-capture-recorder", ifmt, NULL) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}
#else
	//Use gdigrab
	AVDictionary* options = NULL;
	//Set some options
	//grabbing frame rate
	av_dict_set(&options,"framerate","60",0);
	//The distance from the left edge of the screen or desktop
	//av_dict_set(&options,"offset_x","20",0);
	//The distance from the top edge of the screen or desktop
	//av_dict_set(&options,"offset_y","40",0);
	//Video frame size. The default is to capture the full screen
	string vs = to_string(WIDTH) + "x" + to_string(HEIGHT);
	cout << "video size:" << vs << endl;
	av_dict_set(&options,"video_size",vs.c_str() ,0);
	AVInputFormat *ifmt = av_find_input_format("gdigrab");
	if (avformat_open_input(&pFormatCtx, "desktop", ifmt, &options) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}

#endif
#endif
	if (avformat_find_stream_info(pFormatCtx, NULL)<0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex = -1;
	for (i = 0; i<pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL)<0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	int numByts;
	uint8_t *out_buffer;// , *block_buffer;
#ifdef DECODE_FMT_RGB24
	//cout << "123123123 " << endl;
	AVPixelFormat format = AV_PIX_FMT_RGB24;
	numByts = avpicture_get_size(format, pCodecCtx->width, pCodecCtx->height);
	blockBytes = avpicture_get_size(format, BLOCK_W, BLOCK_H);
	out_buffer = (uint8_t *)av_malloc(numByts);
	//block_buffer = (uint8_t *)av_malloc(blockBytes);
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, format, pCodecCtx->width, pCodecCtx->height);
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, format, SWS_BICUBIC, NULL, NULL, NULL);
#else
	AVPixelFormat format = AV_PIX_FMT_YUV420P;
	numByts = av_image_get_buffer_size(format, pCodecCtx->width, pCodecCtx->height, 1);
	out_buffer = (uint8_t *)av_malloc(numByts);
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, format, pCodecCtx->width, pCodecCtx->height, 1);
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, format, SWS_BICUBIC, NULL, NULL, NULL);
#endif // DECODE_FMT_RGB24

	


	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - s%\n", SDL_GetError());
		return -1;
	}
	//SDL 2.0 Support for multiple windows
	screen_w = pCodecCtx->width;
	screen_h = pCodecCtx->height;
	screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, SDL_WINDOW_OPENGL);
	if (!screen) {
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}
	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
#ifdef DECODE_FMT_RGB24
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);
#else
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);
#endif // DECODE_FMT_RGB24
	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	video_tid = SDL_CreateThread(sfp_refresh_thread, NULL, NULL);
	//--------------SDL End---------------
	//initSocket
	initSocket();
	//Event Loop
	int fps = 0;
	clock_t start, end;
	start = clock();
	/*uint8_t * bufferPtr;*/// , *srcPtr;
	for (;;) {
		//wait
		SDL_WaitEvent(&event);
		if (event.type == SFM_REFRESH_EVENT) {
			while (1)
			{
				if (av_read_frame(pFormatCtx, packet) < 0)
					thread_exit = 1;
				if (packet->stream_index == videoindex)
					break;
			}
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0) {
				printf("Decode Error.\n");
				return -1;
			}
			if (got_picture) {
				//bufferPtr = block_buffer;
				//srcPtr = pFrame->data[0];
				sendFrame(pFrame->data[0],pFrame->linesize[0]);
				sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
				//SDL-----------------
				SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
				SDL_RenderClear(sdlRenderer);
				//SDL_RenderCopy(sdlRenderer, sdlTexture, &sdlRect, &sdlRect);
				SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
				SDL_RenderPresent(sdlRenderer);
				//SDL End---------------
				fps++;
				if (fps > 60) {
					fps = 0;
					end = clock();
					//cout << "linesSize" << pFrameYUV->linesize[0] << endl;
					//cout << "size: " << sizeof(pFrame->extended_data)/(sizeof(uint8_t)) << " width: " << pCodecCtx->width << " height: " << pCodecCtx->height << endl;
					//cout << start << "---" << end << "---" << (end - start) << endl;
					cout << "FPS:" << 1000 * 60 / (end - start) << endl;
					start = clock();
				}
			}
			av_free_packet(packet);
		}else if (event.type == SDL_KEYDOWN) {
			//Pause
			cout << "press key: " << event.key.keysym.sym << endl;
			if (event.key.keysym.sym == 32)//SDLK_SPACE)
				thread_pause == !thread_pause;
		}
		else if (event.type == SDL_KEYDOWN) {
			thread_exit = 1;
		}
		else if(event.type == SFM_BREAK_EVENT)
		{
			releaseSocket();
			break;
		}
	}
	sws_freeContext(img_convert_ctx);
	SDL_Quit();
	//-----------------------
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
	system("pause");
	return 0;
}

//void someTest() {
	//getVideoInfo("1.mp4");
	//SDL Test
	//if (SDL_Init(SDL_INIT_VIDEO) != 0){
	//	cout << "SDL_Init Error: " << SDL_GetError() << endl;
	//	return 1;
	//}
	//SDL_Quit();

	//startFFmpeg();
	//receiveData();
//}

const long lngSendTimeOut = 1000;
const long lngRecvTimeOut = 1000;
WSADATA wsaData;
long    addr;
long    wrc;
SOCKADDR_IN sockObject;
SOCKET    sSend;
//u_long nonblocking = 1;
//char    transbuf[1600];//这个地方的数据大小，只是跟填充网络发送缓冲区有关
char    IPADDR[17];
uint8_t* block_buffer, *srcPtr;

void initSocket() {
	//使用socket之前必须调用，用于绑定某版本的socket
	WSAStartup(0x202, &wsaData);
	sSend = socket(AF_INET, SOCK_DGRAM, 0);
	//设置为非阻塞模式
	u_long nonblocking = 1;
	ioctlsocket(sSend, FIONBIO, &nonblocking);//在非阻塞模式下,udp发送基本不占用时间,而且跟ip地址无关系
	setsockopt(sSend, SOL_SOCKET, SO_SNDTIMEO, (char*)&lngSendTimeOut, sizeof(lngSendTimeOut));
	setsockopt(sSend, SOL_SOCKET, SO_RCVTIMEO, (char*)&lngRecvTimeOut, sizeof(lngRecvTimeOut));
	sockObject.sin_port = htons(3500);
	sockObject.sin_family = AF_INET;
	block_buffer = (uint8_t *)av_malloc(blockBytes);
}
void sendFrame(uint8_t* data, int lineSize)
{
	//DWORD    LastTime;
	//DWORD    Curtime;
	//DWORD    EscpTime;
	uint8_t * bufferPtr;
	bufferPtr = block_buffer;
	srcPtr = data;
	for (int i = 0; i < BLOCK_H; i++) {
		memcpy(bufferPtr, srcPtr, BLOCK_W * 3);
		bufferPtr += BLOCK_W * 3;
		srcPtr += lineSize;
	}
	//memset(transbuf, 'a', sizeof(transbuf));
	/*for (int i = 1; i < 35; i++)
	{*/

	//这个地方的耗时跟两个因素有关系
	//官方：
	//If no buffer space is available within the transport system to hold the data to be transmitted, 
	//sendto will block unless the socket has been placed in a nonblocking mode. 
	//也就是说,对于sendto函数，他的发送返回成功并不代表网络发送了，他只是将数据发送到传输层的缓冲区，就返回结果
	//而对于阻塞的socket,当这个ip地址即使不存在，但缓冲区未满的情况下，他也是返回成功的，而且基本不耗时，
	//但当缓冲区满了以后,sendto就会阻塞，这个时候会产生耗时
	//对于非阻塞的socket,缓冲区未满的时候，它基本不占发送时间，满了，sendto也会立即返回结果，整个过程基本不耗时
	sprintf(IPADDR, "192.168.0.%d", 1);
	addr = inet_addr((char *)IPADDR);
	memcpy(&sockObject.sin_addr, &addr, sizeof(addr));//赋值IP地址
	wrc = sendto(sSend, (char*)block_buffer, blockBytes, 0, (struct sockaddr *)&sockObject, sizeof(sockObject));

	//clock_t start, end;
	//for (int j = 0; j < 10; j++) {
		//start = clock();
		//for (int i = 0; i < 400; i++) {
			//wrc = sendto(sSend, transbuf, sizeof(transbuf), 0, (struct sockaddr *)&sockObject, sizeof(sockObject));
		//}
		//end = clock();
		//cout << end - start << "/" << CLOCKS_PER_SEC << " (s) " << endl;
		//Sleep(1);
	//}

	//closesocket(sSend);
	//WSACleanup();
	//int x;
	//cin >> x;
	//return 0;
}

void releaseSocket() {
	closesocket(sSend);
	WSACleanup();
}