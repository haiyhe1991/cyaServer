#ifndef __CYA_PC_COM_H__
#define __CYA_PC_COM_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <list>
#include "cyaThread.h"
#include "cyaLocker.h"

#if defined(__linux__)
	#include <termios.h> // PPSIX �ն˿��ƶ���
#endif

/*******************��д����*********************/

class PCComport
{
public:
	PCComport();
	~PCComport();

	enum ComAccessMode
	{
		CAM_READ = 1,
		CAM_WRITE = 2
	};

	enum ComBaudRate
	{
		CPBR_110,
		CPBR_300,
		CPBR_600,
		CPBR_1200,
		CPBR_2400,
		CPBR_4800,
		CPBR_9600,
		CPBR_14400,
		CPBR_19200,
		CPBR_38400,
		CPBR_56000,
		CPBR_57600,
		CPBR_115200,
		CPBR_128000,
		CPBR_256000,

		CPBR_0,
		CPBR_50,
		CPBR_75,
		CPBR_134,
		CPBR_150,
		CPBR_200,
		CPBR_230400
	};

	enum ComParityType
	{
		CP_NO,		//��У��
		CP_ODD,		//��У��
		CP_EVEN		//żУ��
	};

	enum ComStopBits
	{
		CSB_ONE,			//1ֹͣλ
		CSB_ONEDOTFIVE,		//1.5ֹͣλ
		CSB_TWO				//2ֹͣλ
	};

	enum ComReadStatus
	{
		CRS_MUST,
		CRS_MAX
	};

	struct ComReadHandler
	{
	public:
		ComReadHandler(){}
		virtual ~ComReadHandler(){}

		virtual void RequestRead(void*& buf, int& bufLen, ComReadStatus& readSatus) = 0;
		virtual void OnRead(void* buf, int bufLen) = 0;
	};

	BOOL OpenComport(const char* portName, //������("COM1"/"/dev/ttyS1"/"/dev/12")
		ComAccessMode mode,	//����ģʽ(����д����|д)
		ComBaudRate cbr,		//������
		ComParityType parity,	//У��
		ComStopBits stop,		//ֹͣλ
		ComReadHandler* handler,	//ʹ������д��
		BYTE byteSize = 8,		//����λ
		int flowcontrol = 0,	//����
		DWORD inBuffer = 1024,	//���뻺����
		DWORD outBuffer = 1024);	//���������
	void CloseComport();
#if defined(WIN32)
	HANDLE GetComHandle()
	{
		return m_hComport;
	}
#elif defined(__linux__)
	int GetComFd()
	{
		return m_comfd;
	}
#endif
	BOOL WriteComportData(const void* data, int dataLen);

	struct ReadOrWriteBuf
	{
		void* buf;
		int needLen;
		int bufLen;

		ReadOrWriteBuf()
		{
			Init();
		}

		void Init()
		{
			buf = NULL;
			needLen = 0;
			bufLen = 0;
		}
		BOOL IsNull()
		{
			return buf == NULL ? true : false;
		}
	};

	private:
		class ComReadSite
		{
		public:
			ComReadSite(PCComport* parent, ComReadHandler* handler);
			~ComReadSite();

			void RequestRead(void*& buf, int& bufLen, ComReadStatus& readSatus)
			{
				m_readHandler->RequestRead(buf, bufLen, readSatus);
			}
			void OnRead(void* buf, int bufLen)
			{
				m_readHandler->OnRead(buf, bufLen);
			}

			void Open();
			void Close();

		private:
			static int THWorker(void* p);
			int OnWorker();

		private:
			PCComport*	m_parent;
			OSThread	m_thReadHandle;
			BOOL		m_exit;
#if defined(WIN32)
			OVERLAPPED	m_ovRead;
#endif
			ReadOrWriteBuf	m_readBuf;
			ComReadStatus	m_readSatus;
			ComReadHandler* m_readHandler;
		};
		class ComWriteSite
		{
		public:
			ComWriteSite(PCComport* parent);
			~ComWriteSite();

			void Open();
			void Close();

			BOOL InputData(const void* data, int dataLen);

		private:
			static int THWorker(void* p);
			int OnWorker();

		private:
			PCComport*	m_parent;
			OSThread	m_thWriteHandle;
			BOOL		m_exit;
#if defined(WIN32)
			OVERLAPPED	m_ovWrite;
#endif
			CXTLocker	m_locker;
			std::list<ReadOrWriteBuf> m_bufferList;
			ReadOrWriteBuf m_wtBuf;
		};
private:
#if defined(WIN32)
	HANDLE	m_hComport;
#elif defined(__linux__)
	int		m_comfd;
#endif
	ComReadSite*	m_readSite;
	ComWriteSite*	m_writeSite;
};
#endif