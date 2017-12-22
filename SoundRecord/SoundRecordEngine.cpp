#include "StdAfx.h"
#include ".\soundrecordengine.h"

extern "C"
{
	#include "EncodeAmr\typedef.h"
	#include "EncodeAmr\interf_enc.h"	
}

CSoundRecordEngine::CSoundRecordEngine(void)
{
	m_hWaveIn = NULL;
	m_hWaveOut = NULL;

	m_WavFormat.wFormatTag = WAVE_FORMAT_PCM;  
	m_WavFormat.nChannels = 1;  
	m_WavFormat.nSamplesPerSec = 8000;  
	m_WavFormat.nAvgBytesPerSec = 8000*2;  
	m_WavFormat.nBlockAlign = 2;  
	m_WavFormat.wBitsPerSample = 16;  
	m_WavFormat.cbSize = 0;  

	m_pBufRecord = NULL;
	m_nBufCount = 0;
	m_maxBufCount = 0;

	m_bRecordState = false;
	m_bInDevOpen = false;
	m_bOutDevOpen = false;

	m_hListenWnd = NULL;
}

CSoundRecordEngine::~CSoundRecordEngine(void)
{
}

void CALLBACK CSoundRecordEngine::waveInProc(
								HWAVEIN hwi,       
								UINT uMsg,         
								DWORD dwInstance,  
								DWORD dwParam1,    
								DWORD dwParam2     
								)
{
	CSoundRecordEngine *pSRE = (CSoundRecordEngine*)dwInstance;
	if(!pSRE)
		return;

	static int bStop = false;

	LPWAVEHDR pwh = (LPWAVEHDR)dwParam1;  
	if (WIM_DATA == uMsg)		
	{  
		if(pSRE->m_nBufCount < pSRE->m_maxBufCount && pSRE->m_bRecordState)
		{
			int temp = pSRE->m_maxBufCount - pSRE->m_nBufCount;  
			temp = (temp>pwh->dwBytesRecorded) ? pwh->dwBytesRecorded : temp;  
			if(pwh->lpData)
			{
				memcpy(pSRE->m_pBufRecord + pSRE->m_nBufCount, pwh->lpData, temp);  
				pSRE->m_nBufCount += temp;  

				waveInAddBuffer(hwi, pwh, sizeof(WAVEHDR));  	
			}	
			bStop = false;
		}		
		else
		{
			TRACE("录音完毕!\n");
			if(pSRE->m_hListenWnd && !bStop)
			{				
				bStop = true;
				::PostMessage(pSRE->m_hListenWnd, WM_RECORD_COMPLETE, 0, 0);
			}				
		}			
	} 
	return;
}


void CALLBACK CSoundRecordEngine::waveOutProc(
								 HWAVEOUT hwo,      
								 UINT uMsg,         
								 DWORD dwInstance,  
								 DWORD dwParam1,    
								 DWORD dwParam2     
								 )
{
	return;
}

int CSoundRecordEngine:: OpenInDevice()
{
	if(!m_bInDevOpen)
	{
		//判断输入设备是否存在
		UINT nReturn = ::waveInGetNumDevs();  
		TRACE("输入设备数目：%d\n", nReturn);  
		if(!nReturn)
		{
			return -1;
		}
		for (int i=0; i<nReturn; i++)  
		{  
			WAVEINCAPS wic;  
			::waveInGetDevCaps(i, &wic, sizeof(WAVEINCAPS));  
			TRACE("#%d\t设备名：%s\n", i, wic.szPname);  
		}  	

		//打开输入设备
		nReturn = ::waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_WavFormat, (DWORD_PTR)waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
		if(nReturn != MMSYSERR_NOERROR)
		{
			TRACE("打开输入设备失败, errorcode: %d\n", nReturn);
			return -1;
		}		
		WAVEINCAPS wic;  
		::waveInGetDevCaps((UINT_PTR)m_hWaveIn, &wic, sizeof(WAVEINCAPS));  
		TRACE("打开的输入设备：%s\n", wic.szPname);  
		m_bInDevOpen = true;
	}
	return 0;
}

int CSoundRecordEngine::CloseInDevice()
{
	if(m_bInDevOpen)
	{
		//关闭录音设备
		UINT nReturn = ::waveInClose(m_hWaveIn);
		if(nReturn == MMSYSERR_NOERROR)		
			m_bInDevOpen = false;
	}	
	return 0;
}

int CSoundRecordEngine::OpenOutDevice()
{
	return 0;
}

int CSoundRecordEngine::CloseOutDevice()
{
	return 0;
}

void CSoundRecordEngine::SetRecordListenWnd(HWND hListenWnd)
{
	m_hListenWnd = hListenWnd;
}

int CSoundRecordEngine::StartRecord(int time)
{
	//判断录音状态
	if(m_bRecordState)
	{
		TRACE("录音正在进行中, 请先关闭当前录音!\n");
		return -1;
	}

	//判断录音时间
	if(time>MAX_LENGTH_TIME)
	{
		TRACE("录音时间不能超过5h\n");
		return -1;
	}

	//打开输入设备
	if(OpenInDevice()!=0)
		return -1;

	//创建录音缓冲区
	m_nBufCount = 0;
	m_maxBufCount = time*m_WavFormat.nAvgBytesPerSec;
	if(m_pBufRecord)
		delete m_pBufRecord, m_pBufRecord = NULL;
	m_pBufRecord = new unsigned char[m_maxBufCount];

	//准备录音流
	for (int i=0; i<FRAGMENT_NUM; i++)  
	{  
		m_inWaveHdr[i].lpData = new char[FRAGMENT_SIZE];  
		m_inWaveHdr[i].dwBufferLength = FRAGMENT_SIZE;  
		m_inWaveHdr[i].dwBytesRecorded = 0;  
		m_inWaveHdr[i].dwUser = NULL;  
		m_inWaveHdr[i].dwFlags = 0;  
		m_inWaveHdr[i].dwLoops = 1;  
		m_inWaveHdr[i].lpNext = NULL;  
		m_inWaveHdr[i].reserved = 0;  

		::waveInPrepareHeader(m_hWaveIn, &m_inWaveHdr[i], sizeof(WAVEHDR));  
		::waveInAddBuffer(m_hWaveIn, &m_inWaveHdr[i], sizeof(WAVEHDR));  
	}  

	//开始录音
	TRACE("Start to Record...\n");  	
	waveInStart(m_hWaveIn);  	
	m_bRecordState = true;
	return 0;
}

int CSoundRecordEngine::StopRecord()
{	
	if(!m_bRecordState)
		return -1;	

	//停止录音
	m_bRecordState = false;
	Sleep(100);
	::waveInReset(m_hWaveIn);			 

	//释放录音输入流
	for (int i=0; i<FRAGMENT_NUM; i++)  
	{  
		::waveInUnprepareHeader(m_hWaveIn, &m_inWaveHdr[i], sizeof(WAVEHDR));  
		if(m_inWaveHdr[i].lpData)
		{
			delete m_inWaveHdr[i].lpData;  
			m_inWaveHdr[i].lpData = NULL;
		}		
	} 

	//关闭输入设备
	CloseInDevice();	
	return 0;	
}

int CSoundRecordEngine::SaveRecord(char* strFileName, SOUND_FORMAT format)
{
	if(m_bRecordState)
	{
		TRACE("先停止录音再进行保存!\n");
		return -1;
	}

	switch(format)
	{
	case SF_PCM:
		{
			SaveRecordForPcm(strFileName);
		}
		break;
	case SF_AMR:
		{
			SaveRecordForAmr(strFileName);
		}
		break;
	default:
		break;
	}
	return 0;
}

int CSoundRecordEngine::SaveRecordForPcm(char* strFileName)
{
	if(!m_pBufRecord) //音频数据已释放
		return -1;

	//打开文件
	HANDLE hFile = ::CreateFile(strFileName, GENERIC_WRITE , 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return -1;

	//先填冲语音数据区
	::SetFilePointer(hFile, sizeof(WAVEFILEHDR), NULL, FILE_BEGIN); //跳过文件头
	DWORD dwWriten = 0, dwSumWriten = 0;
	do
	{
		::WriteFile(hFile, m_pBufRecord + dwSumWriten, FRAGMENT_SIZE, &dwWriten, NULL);	
		dwSumWriten += dwWriten;
	}while(dwWriten>0 && dwSumWriten<m_nBufCount);	

	//设定文件头参数
	WAVEFILEHDR wfd = 
	{
		'R', 'I', 'F', 'F', 0, 
		'W', 'A', 'V', 'E', 
		'f', 'm', 't', ' ', 16, 
		WAVE_FORMAT_PCM, 1, 8000, 8000*2, 2, 16, 
		'd', 'a', 't', 'a', 0
	};
	//需要与采样参数一致
	wfd.wFormatTag = m_WavFormat.wFormatTag;
	wfd.wChannels = m_WavFormat.nChannels;
	wfd.dwSamplesPerSec = m_WavFormat.nSamplesPerSec;
	wfd.dwAvgBytesPerSec = m_WavFormat.nAvgBytesPerSec;
	wfd.wBlockAlign = m_WavFormat.nBlockAlign;
	wfd.wBitsPerSample = m_WavFormat.wBitsPerSample;

	//数据大小和文件大小
	wfd.dwDataSize = dwSumWriten;
	wfd.dwFileDataSize = wfd.dwDataSize + sizeof(WAVEFILEHDR)- 8;	

	//写入文件头
	::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	::WriteFile( hFile, &wfd, sizeof(wfd), &dwWriten, NULL);

	//关闭文件
	::CloseHandle(hFile);

	//释放语音缓冲区
	if(m_pBufRecord)
		delete m_pBufRecord, m_pBufRecord = NULL;

	return 0;
}

int CSoundRecordEngine::SaveRecordForAmr(char* strFileName)
{	
	if(!m_pBufRecord) //音频数据已释放
		return -1;

	//打开目的文件
	FILE* pDestFile = fopen(strFileName, "wb");
	if(!pDestFile)
	{
		return -1;
	}

	//编码前与编码后的数据
	short speech[160] = {0};
#ifndef ETSI
	unsigned char serial_data[32] = {0};
#else
	short serial_data[250] = {0};
#endif	

	//启动编码	
	void* enstate = Encoder_Interface_init(0);
	if(!enstate)
		return 1;

	//写入Amr文件标识
	fwrite("#!AMR\n", sizeof(char), strlen("#!AMR\n"), pDestFile);

	//对音频内容进行编码
	DWORD dwSumWriteCount = 0;
	while(dwSumWriteCount < m_nBufCount)
	{
		//取音频数据
		int restBytes = m_nBufCount - dwSumWriteCount;
		restBytes = (restBytes > sizeof(speech)? sizeof(speech) : restBytes);
		memcpy(speech, m_pBufRecord + dwSumWriteCount, restBytes);
		dwSumWriteCount += restBytes;

		//编码
		int byte_counter = Encoder_Interface_Encode(enstate, MR475, speech, serial_data, 0);

		//编码后的数据写入文件
		fwrite(serial_data, sizeof (UWord8), byte_counter, pDestFile);
		fflush(pDestFile);		
	}

	//结束编码
	Encoder_Interface_exit(enstate);

	//关闭文件
	fclose(pDestFile);

	//释放语音缓冲区
	if(m_pBufRecord)
		delete m_pBufRecord, m_pBufRecord = NULL;
	return 0;
}

int CSoundRecordEngine::PlayRecord(char* strFileName)
{
	return 0;	
}

int CSoundRecordEngine::PlayRecordForPcm(char* strFileName)
{
	return 0;
}

int CSoundRecordEngine::PlayRecordForAmr(char* strFileName)
{
	return 0;	
}

int CSoundRecordEngine::EncodeAmrFile(char* strSrcFile, char* strDestFile, AmrMode mode)
{
	//编码前与编码后的数据
	short speech[160] = {0};
#ifndef ETSI
	unsigned char serial_data[32] = {0};
#else
	short serial_data[250] = {0};
#endif

	//编码模式
	enum Mode req_mode = MR475;	

	FILE* pSrcFile = fopen(strSrcFile, "rb");
	if(!pSrcFile)
	{
		return 1;
	}	
	FILE* pDestFile = fopen(strDestFile, "wb");
	if(!pDestFile)
	{
		fclose(pSrcFile);
		return 1;
	}

	//启动编码	
	void* enstate = Encoder_Interface_init(0); 
	if(!enstate)
		return 1;

	//写入Amr文件标识
#ifndef ETSI
#ifndef IF2
	fwrite("#!AMR\n", sizeof(char), strlen("#!AMR\n"), pDestFile);
#endif
#endif

	//对音频内容进行编码
	while (fread( speech, sizeof (Word16), 160, pSrcFile ) > 0)
	{
		//编码
		int byte_counter = Encoder_Interface_Encode(enstate, req_mode, speech, serial_data, 0);

		//编码后的数据写入文件
		fwrite(serial_data, sizeof (UWord8), byte_counter, pDestFile );
		fflush(pDestFile);
	}

	//结束编码
	Encoder_Interface_exit(enstate);

	//关闭文件
	fclose(pSrcFile);
	fclose(pDestFile);
	return 0;
}