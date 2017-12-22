#pragma once

#include <windows.h>
#include <mmsystem.h>   

#pragma comment(lib, "winmm.lib")   

#define MAX_LENGTH_TIME 5*60*60 //�¼��ʱ��Ϊ5h

#define FRAGMENT_NUM 4 //����4���߳̽���¼��
#define FRAGMENT_SIZE 1024 //ÿ��¼��Ƭ�εĴ�С

#define WM_RECORD_COMPLETE WM_USER + 0X1001 //¼�������Ϣ�ص�

//amr�ı���ģʽ
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

//������ʽ
enum SOUND_FORMAT
{
	SF_PCM = 0,
	SF_WAV,
	SF_AMR,
	SF_MP3,
};

//PCM�ļ�ͷ
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

//ע����಻����Ϊ�ֲ�����
class CSoundRecordEngine
{
public:
	CSoundRecordEngine(void);
	~CSoundRecordEngine(void);

private:
	//���롢����豸���
	HWAVEIN m_hWaveIn; 
	HWAVEOUT m_hWaveOut;
	
	//¼����ʽ
	WAVEFORMATEX m_WavFormat;  	

	//¼���ص�����
	static void CALLBACK waveInProc(
		HWAVEIN hwi,       
		UINT uMsg,         
		DWORD dwInstance,  
		DWORD dwParam1,    
		DWORD dwParam2     
		);

	//����¼���ص�����
	static void CALLBACK waveOutProc(
		HWAVEOUT hwo,      
		UINT uMsg,         
		DWORD dwInstance,  
		DWORD dwParam1,    
		DWORD dwParam2     
		);

	int OpenInDevice(); //��¼���豸
	int CloseInDevice(); //�ر�¼���豸
	int OpenOutDevice(); //�򿪲����豸
	int CloseOutDevice(); //�رղ����豸

	unsigned char* m_pBufRecord; //�洢¼������
	int m_nBufCount; //��¼���ֽ���
	int m_maxBufCount; //���¼���ֽ���

	WAVEHDR m_inWaveHdr[FRAGMENT_NUM]; //¼������������
	WAVEHDR m_outWaveHdr; //¼�����������

	bool m_bRecordState; //¼��״̬��true�����ڽ����У�false��¼��ֹͣ
	bool m_bInDevOpen; //¼���豸�Ƿ��
	bool m_bOutDevOpen; //����豸�Ƿ��

	HWND m_hListenWnd; //��������

public:
	//�趨¼����������
	void SetRecordListenWnd(HWND hListenWnd);

	//����¼��
	int StartRecord(int time = MAX_LENGTH_TIME);

	//ֹͣ¼��
	int StopRecord();

	//����¼��, Ĭ��ΪAMR��ʽ��Ŀǰ��֧��PCM��ʽ��MAR��ʽ
	int SaveRecord(char* strFileName, SOUND_FORMAT format = SF_AMR);
	int SaveRecordForPcm(char* strFileName);
	int SaveRecordForAmr(char* strFileName);

	//����¼��	
	int PlayRecord(char* strFileName); //����¼���ļ�ͷ��ʶ��¼����ʽ
	int PlayRecordForPcm(char* strFileName); //����pcm¼���ļ�
	int PlayRecordForAmr(char* strFileName); //����amr¼���ļ�

	//pcm��wav��mp3����Ƶתamr��Ƶ��Ŀǰת�����amrֻ֧�ֵ�����������pcm��ʽ���Ƽ�������������Ϊ8000
	int EncodeAmrFile(char* strSrcFile, char* strDestFile, AmrMode mode = AMR475);
};
