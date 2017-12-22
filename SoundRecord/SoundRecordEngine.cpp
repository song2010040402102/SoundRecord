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
			TRACE("¼�����!\n");
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
		//�ж������豸�Ƿ����
		UINT nReturn = ::waveInGetNumDevs();  
		TRACE("�����豸��Ŀ��%d\n", nReturn);  
		if(!nReturn)
		{
			return -1;
		}
		for (int i=0; i<nReturn; i++)  
		{  
			WAVEINCAPS wic;  
			::waveInGetDevCaps(i, &wic, sizeof(WAVEINCAPS));  
			TRACE("#%d\t�豸����%s\n", i, wic.szPname);  
		}  	

		//�������豸
		nReturn = ::waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_WavFormat, (DWORD_PTR)waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
		if(nReturn != MMSYSERR_NOERROR)
		{
			TRACE("�������豸ʧ��, errorcode: %d\n", nReturn);
			return -1;
		}		
		WAVEINCAPS wic;  
		::waveInGetDevCaps((UINT_PTR)m_hWaveIn, &wic, sizeof(WAVEINCAPS));  
		TRACE("�򿪵������豸��%s\n", wic.szPname);  
		m_bInDevOpen = true;
	}
	return 0;
}

int CSoundRecordEngine::CloseInDevice()
{
	if(m_bInDevOpen)
	{
		//�ر�¼���豸
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
	//�ж�¼��״̬
	if(m_bRecordState)
	{
		TRACE("¼�����ڽ�����, ���ȹرյ�ǰ¼��!\n");
		return -1;
	}

	//�ж�¼��ʱ��
	if(time>MAX_LENGTH_TIME)
	{
		TRACE("¼��ʱ�䲻�ܳ���5h\n");
		return -1;
	}

	//�������豸
	if(OpenInDevice()!=0)
		return -1;

	//����¼��������
	m_nBufCount = 0;
	m_maxBufCount = time*m_WavFormat.nAvgBytesPerSec;
	if(m_pBufRecord)
		delete m_pBufRecord, m_pBufRecord = NULL;
	m_pBufRecord = new unsigned char[m_maxBufCount];

	//׼��¼����
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

	//��ʼ¼��
	TRACE("Start to Record...\n");  	
	waveInStart(m_hWaveIn);  	
	m_bRecordState = true;
	return 0;
}

int CSoundRecordEngine::StopRecord()
{	
	if(!m_bRecordState)
		return -1;	

	//ֹͣ¼��
	m_bRecordState = false;
	Sleep(100);
	::waveInReset(m_hWaveIn);			 

	//�ͷ�¼��������
	for (int i=0; i<FRAGMENT_NUM; i++)  
	{  
		::waveInUnprepareHeader(m_hWaveIn, &m_inWaveHdr[i], sizeof(WAVEHDR));  
		if(m_inWaveHdr[i].lpData)
		{
			delete m_inWaveHdr[i].lpData;  
			m_inWaveHdr[i].lpData = NULL;
		}		
	} 

	//�ر������豸
	CloseInDevice();	
	return 0;	
}

int CSoundRecordEngine::SaveRecord(char* strFileName, SOUND_FORMAT format)
{
	if(m_bRecordState)
	{
		TRACE("��ֹͣ¼���ٽ��б���!\n");
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
	if(!m_pBufRecord) //��Ƶ�������ͷ�
		return -1;

	//���ļ�
	HANDLE hFile = ::CreateFile(strFileName, GENERIC_WRITE , 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return -1;

	//���������������
	::SetFilePointer(hFile, sizeof(WAVEFILEHDR), NULL, FILE_BEGIN); //�����ļ�ͷ
	DWORD dwWriten = 0, dwSumWriten = 0;
	do
	{
		::WriteFile(hFile, m_pBufRecord + dwSumWriten, FRAGMENT_SIZE, &dwWriten, NULL);	
		dwSumWriten += dwWriten;
	}while(dwWriten>0 && dwSumWriten<m_nBufCount);	

	//�趨�ļ�ͷ����
	WAVEFILEHDR wfd = 
	{
		'R', 'I', 'F', 'F', 0, 
		'W', 'A', 'V', 'E', 
		'f', 'm', 't', ' ', 16, 
		WAVE_FORMAT_PCM, 1, 8000, 8000*2, 2, 16, 
		'd', 'a', 't', 'a', 0
	};
	//��Ҫ���������һ��
	wfd.wFormatTag = m_WavFormat.wFormatTag;
	wfd.wChannels = m_WavFormat.nChannels;
	wfd.dwSamplesPerSec = m_WavFormat.nSamplesPerSec;
	wfd.dwAvgBytesPerSec = m_WavFormat.nAvgBytesPerSec;
	wfd.wBlockAlign = m_WavFormat.nBlockAlign;
	wfd.wBitsPerSample = m_WavFormat.wBitsPerSample;

	//���ݴ�С���ļ���С
	wfd.dwDataSize = dwSumWriten;
	wfd.dwFileDataSize = wfd.dwDataSize + sizeof(WAVEFILEHDR)- 8;	

	//д���ļ�ͷ
	::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	::WriteFile( hFile, &wfd, sizeof(wfd), &dwWriten, NULL);

	//�ر��ļ�
	::CloseHandle(hFile);

	//�ͷ�����������
	if(m_pBufRecord)
		delete m_pBufRecord, m_pBufRecord = NULL;

	return 0;
}

int CSoundRecordEngine::SaveRecordForAmr(char* strFileName)
{	
	if(!m_pBufRecord) //��Ƶ�������ͷ�
		return -1;

	//��Ŀ���ļ�
	FILE* pDestFile = fopen(strFileName, "wb");
	if(!pDestFile)
	{
		return -1;
	}

	//����ǰ�����������
	short speech[160] = {0};
#ifndef ETSI
	unsigned char serial_data[32] = {0};
#else
	short serial_data[250] = {0};
#endif	

	//��������	
	void* enstate = Encoder_Interface_init(0);
	if(!enstate)
		return 1;

	//д��Amr�ļ���ʶ
	fwrite("#!AMR\n", sizeof(char), strlen("#!AMR\n"), pDestFile);

	//����Ƶ���ݽ��б���
	DWORD dwSumWriteCount = 0;
	while(dwSumWriteCount < m_nBufCount)
	{
		//ȡ��Ƶ����
		int restBytes = m_nBufCount - dwSumWriteCount;
		restBytes = (restBytes > sizeof(speech)? sizeof(speech) : restBytes);
		memcpy(speech, m_pBufRecord + dwSumWriteCount, restBytes);
		dwSumWriteCount += restBytes;

		//����
		int byte_counter = Encoder_Interface_Encode(enstate, MR475, speech, serial_data, 0);

		//����������д���ļ�
		fwrite(serial_data, sizeof (UWord8), byte_counter, pDestFile);
		fflush(pDestFile);		
	}

	//��������
	Encoder_Interface_exit(enstate);

	//�ر��ļ�
	fclose(pDestFile);

	//�ͷ�����������
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
	//����ǰ�����������
	short speech[160] = {0};
#ifndef ETSI
	unsigned char serial_data[32] = {0};
#else
	short serial_data[250] = {0};
#endif

	//����ģʽ
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

	//��������	
	void* enstate = Encoder_Interface_init(0); 
	if(!enstate)
		return 1;

	//д��Amr�ļ���ʶ
#ifndef ETSI
#ifndef IF2
	fwrite("#!AMR\n", sizeof(char), strlen("#!AMR\n"), pDestFile);
#endif
#endif

	//����Ƶ���ݽ��б���
	while (fread( speech, sizeof (Word16), 160, pSrcFile ) > 0)
	{
		//����
		int byte_counter = Encoder_Interface_Encode(enstate, req_mode, speech, serial_data, 0);

		//����������д���ļ�
		fwrite(serial_data, sizeof (UWord8), byte_counter, pDestFile );
		fflush(pDestFile);
	}

	//��������
	Encoder_Interface_exit(enstate);

	//�ر��ļ�
	fclose(pSrcFile);
	fclose(pDestFile);
	return 0;
}