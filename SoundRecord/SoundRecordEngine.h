#pragma once

#include <windows.h>
#include <mmsystem.h>   

#pragma comment(lib, "winmm.lib")   

#define MAX_LENGTH_TIME 5*60*60 //最长录音时间为5h

#define FRAGMENT_NUM 4 //开启4个线程进行录音
#define FRAGMENT_SIZE 1024 //每个录音片段的大小

#define WM_RECORD_COMPLETE WM_USER + 0X1001 //录音完成消息回调

//amr的编码模式
enum AmrMode 
{	AMR475 = 0,
	AMR515,
	AMR59,
	AMR67,
	AMR74,
	AMR795,
	AMR102,
	AMR122,
	AMRDTX
};

//声音格式
enum SOUND_FORMAT
{
	SF_PCM = 0,
	SF_WAV,
	SF_AMR,
	SF_MP3,
};

//PCM文件头
typedef struct _wavefilehdr
{
	char RiffId[4]; // "RIFF"
	DWORD dwFileDataSize; // file size - 8
	char WaveId[4]; // "WAVE" 
	char FMTId[4]; // "fmt "
	DWORD dwFmtSize; // 16
	WORD wFormatTag; // WAVE_FORMAT_PCM
	WORD wChannels; // 1
	DWORD dwSamplesPerSec; // 8000
	DWORD dwAvgBytesPerSec; // 8000*2
	WORD wBlockAlign; // 2
	WORD wBitsPerSample; // 16
	char DataId[4]; // "data"
	DWORD dwDataSize;

}WAVEFILEHDR, *PWAVEFILEHDR;

//注意此类不能作为局部变量
class CSoundRecordEngine
{
public:
	CSoundRecordEngine(void);
	~CSoundRecordEngine(void);

private:
	//输入、输出设备句柄
	HWAVEIN m_hWaveIn; 
	HWAVEOUT m_hWaveOut;
	
	//录音格式
	WAVEFORMATEX m_WavFormat;  	

	//录音回调函数
	static void CALLBACK waveInProc(
		HWAVEIN hwi,       
		UINT uMsg,         
		DWORD dwInstance,  
		DWORD dwParam1,    
		DWORD dwParam2     
		);

	//播放录音回调函数
	static void CALLBACK waveOutProc(
		HWAVEOUT hwo,      
		UINT uMsg,         
		DWORD dwInstance,  
		DWORD dwParam1,    
		DWORD dwParam2     
		);

	int OpenInDevice(); //打开录音设备
	int CloseInDevice(); //关闭录音设备
	int OpenOutDevice(); //打开播放设备
	int CloseOutDevice(); //关闭播放设备

	unsigned char* m_pBufRecord; //存储录音数据
	int m_nBufCount; //已录制字节数
	int m_maxBufCount; //最大录制字节数

	WAVEHDR m_inWaveHdr[FRAGMENT_NUM]; //录音输入数据流
	WAVEHDR m_outWaveHdr; //录音输出数据流

	bool m_bRecordState; //录音状态，true：正在进行中；false：录音停止
	bool m_bInDevOpen; //录音设备是否打开
	bool m_bOutDevOpen; //输出设备是否打开

	HWND m_hListenWnd; //监听窗体

public:
	//设定录音监听窗体
	void SetRecordListenWnd(HWND hListenWnd);

	//启动录音
	int StartRecord(int time = MAX_LENGTH_TIME);

	//停止录音
	int StopRecord();

	//保存录音, 默认为AMR格式，目前仅支持PCM格式和MAR格式
	int SaveRecord(char* strFileName, SOUND_FORMAT format = SF_AMR);
	int SaveRecordForPcm(char* strFileName);
	int SaveRecordForAmr(char* strFileName);

	//播放录音	
	int PlayRecord(char* strFileName); //根据录音文件头来识别录音格式
	int PlayRecordForPcm(char* strFileName); //播放pcm录音文件
	int PlayRecordForAmr(char* strFileName); //播放amr录音文件

	//pcm、wav、mp3等音频转amr音频，目前转换后的amr只支持单声道，对于pcm格式，推荐参数：采样率为8000
	int EncodeAmrFile(char* strSrcFile, char* strDestFile, AmrMode mode = AMR475);
};
