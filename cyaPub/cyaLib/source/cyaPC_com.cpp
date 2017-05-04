#include "cyaSock.h"
#include "cyaPC_com.h"

PCComport::PCComport()
#if defined(WIN32)
	: m_hComport(INVALID_HANDLE_VALUE)
#elif defined(__linux__)
	: m_comfd(-1)
#endif
	, m_readSite(NULL)
	, m_writeSite(NULL)
{
}

PCComport::~PCComport()
{
	CloseComport();
}

BOOL PCComport::OpenComport(const char* portName,
							ComAccessMode mode,
							ComBaudRate cbr,
							ComParityType parity,
							ComStopBits stop,
							ComReadHandler* handler,
							BYTE byteSize /*= 8*/,
							int flowcontrol /*= 0*/,
							DWORD inBuffer /*= 1024*/,
							DWORD outBuffer /*= 1024*/)
{
	if (portName == NULL || strlen(portName) <= 0)
		return false;

#if defined(WIN32)
	DWORD accessMode = GENERIC_READ;
	DWORD shareMode = FILE_SHARE_READ;
	if (mode == (CAM_READ | CAM_WRITE))
	{
		if (handler == NULL)
			return false;
		accessMode |= GENERIC_WRITE;
		shareMode |= FILE_SHARE_WRITE;
		m_readSite = new PCComport::ComReadSite(this, handler);
		m_writeSite = new PCComport::ComWriteSite(this);
		ASSERT(m_readSite != NULL && m_writeSite != NULL);
	}
	else if (mode & CAM_WRITE)
	{
		accessMode = GENERIC_WRITE;
		shareMode = FILE_SHARE_WRITE;
		m_writeSite = new PCComport::ComWriteSite(this);
		ASSERT(m_writeSite != NULL);
	}
	else
	{
		if (handler == NULL)
			return false;
		m_readSite = new PCComport::ComReadSite(this, handler);
		ASSERT(m_readSite != NULL);
	}

	m_hComport = CreateFileA(portName,
		accessMode,
		shareMode,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);
	if (INVALID_HANDLE_VALUE == m_hComport)
	{
		CloseComport();
		return false;
	}
	SetupComm(m_hComport, inBuffer, outBuffer);	//设置输入/输出缓冲区大小

	DCB dcb;
	memset(&dcb, 0, sizeof(DCB));
	BOOL ret = GetCommState(m_hComport, &dcb);
	if (!ret)
	{
		CloseComport();
		return false;
	}
	switch (cbr)
	{
	case CPBR_110:
		dcb.BaudRate = CBR_110;
		break;
	case CPBR_300:
		dcb.BaudRate = CBR_300;
		break;
	case CPBR_600:
		dcb.BaudRate = CBR_600;
		break;
	case CPBR_1200:
		dcb.BaudRate = CBR_1200;
		break;
	case CPBR_2400:
		dcb.BaudRate = CBR_2400;
		break;
	case CPBR_4800:
		dcb.BaudRate = CBR_4800;
		break;
	case CPBR_9600:
		dcb.BaudRate = CBR_9600;
		break;
	case CPBR_14400:
		dcb.BaudRate = CBR_14400;
		break;
	case CPBR_19200:
		dcb.BaudRate = CBR_19200;
		break;
	case CPBR_38400:
		dcb.BaudRate = CBR_38400;
		break;
	case CPBR_56000:
		dcb.BaudRate = CBR_56000;
		break;
	case CPBR_57600:
		dcb.BaudRate = CBR_57600;
		break;
	case CPBR_115200:
		dcb.BaudRate = CBR_115200;
		break;
	case CPBR_128000:
		dcb.BaudRate = CBR_128000;
		break;
	case CPBR_256000:
		dcb.BaudRate = CBR_256000;
		break;
	default:
		dcb.BaudRate = (DWORD)cbr;
		break;
	}

	switch (parity)
	{
	case CP_NO:
		dcb.Parity = NOPARITY;
		break;
	case CP_ODD:
		dcb.Parity = ODDPARITY;
		break;
	case CP_EVEN:
		dcb.Parity = EVENPARITY;
		break;
	default:
		dcb.Parity = (BYTE)parity;
		break;
	}

	switch (stop)
	{
	case CSB_ONE:
		dcb.StopBits = ONESTOPBIT;
		break;
	case CSB_ONEDOTFIVE:
		dcb.StopBits = ONE5STOPBITS;
		break;
	case CSB_TWO:
		dcb.StopBits = TWOSTOPBITS;
	default:
		dcb.StopBits = (BYTE)stop;
		break;
	}
	dcb.ByteSize = byteSize;
	dcb.fInX = dcb.fOutX = flowcontrol == 2 ? 1 : 0;

	ret = SetCommState(m_hComport, &dcb);
	if (!ret)
	{
		CloseComport();
		return false;
	}
	ret = PurgeComm(m_hComport, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	if (!ret)
	{
		CloseComport();
		return false;
	}
#elif defined(__linux__)
	if (mode == (CAM_READ | CAM_WRITE))
	{
		if (handler == NULL)
			return false;
		m_readSite = new PCComport::ComReadSite(this, handler);
		m_writeSite = new PCComport::ComWriteSite(this);
		ASSERT(m_readSite != NULL && m_writeSite != NULL);
		m_comfd = open(portName, O_RDWR, 0);
	}
	else if (mode & CAM_WRITE)
	{
		m_writeSite = new PCComport::ComWriteSite(this);
		ASSERT(m_writeSite != NULL);
		m_comfd = open(portName, O_RDONLY, 0);
	}
	else
	{
		if (handler == NULL)
			return false;
		m_readSite = new PCComport::ComReadSite(this, handler);
		ASSERT(m_readSite != NULL);
		m_comfd = open(portName, O_WRONLY, 0);
	}
	if (m_comfd <= 0)
	{
		CloseComport();
		return false;
	}
	int baudRate = B0;
	switch (cbr)
	{
	case CPBR_110:
		baudRate = B110;
		break;
	case CPBR_300:
		baudRate = B300;
		break;
	case CPBR_600:
		baudRate = B600;
		break;
	case CPBR_1200:
		baudRate = B1200;
		break;
	case CPBR_2400:
		baudRate = B2400;
		break;
	case CPBR_4800:
		baudRate = B4800;
		break;
	case CPBR_9600:
		baudRate = B9600;
		break;
	case CPBR_19200:
		baudRate = B19200;
		break;
	case CPBR_38400:
		baudRate = B38400;
		break;
	case CPBR_57600:
		baudRate = B57600;
		break;
	case CPBR_115200:
		baudRate = B115200;
		break;
	case CPBR_0:
		baudRate = B0;
		break;
	case CPBR_50:
		baudRate = B50;
		break;
	case CPBR_75:
		baudRate = B75;
		break;
	case CPBR_134:
		baudRate = B134;
		break;
	case CPBR_150:
		baudRate = B150;
		break;
	case CPBR_200:
		baudRate = B200;
		break;
	case CPBR_230400:
		baudRate = B230400;
		break;
	default:
		break;
	}
	// 设置波特率
	int status;
	struct termios opt;
	if (0 != tcgetattr(m_comfd, &opt))
	{
		CloseComport();
		return false;
	}

	tcflush(m_comfd, TCIOFLUSH);
	cfsetispeed(&opt, baudRate);
	cfsetospeed(&opt, baudRate);
	opt.c_iflag &= ~ICRNL;
	status = tcsetattr(m_comfd, TCSANOW, &opt);
	if (status != 0)
	{
		CloseComport();
		return false;
	}
	tcflush(m_comfd, TCIOFLUSH);

	if (tcgetattr(m_comfd, &opt) != 0)
	{
		CloseComport();
		return false;
	}

	// 设置数据位数
	opt.c_cflag &= ~CSIZE;
	switch (byteSize)
	{
	case 5:
		opt.c_cflag |= CS5;
	case 6:
		opt.c_cflag |= CS6;
	case 7:
		opt.c_cflag |= CS7;
		break;
	case 8:
		opt.c_cflag |= CS8;
		break;
	default:
		opt.c_cflag |= CS8;
		break;
	}
	// 设置校验位
	switch (parity)
	{
	case CP_NO:
		opt.c_cflag &= ~PARENB;		// Clear parity enable
		opt.c_iflag &= ~INPCK;		// Enable parity checking
		break;
	case CP_ODD:
		opt.c_cflag |= (PARODD | PARENB);	// 设置为奇效验
		opt.c_iflag |= INPCK;				// Disnable parity checking
		break;
	case CP_EVEN:
		opt.c_cflag |= PARENB;		// Enable parity
		opt.c_cflag &= ~PARODD;		// 转换为偶效验
		opt.c_iflag |= INPCK;		// Disnable parity checking
		break;
	default:
		break;
	}

	// 设置停止位
	switch (stop)
	{
	case CSB_ONE:
		opt.c_cflag &= ~CSTOPB;
		break;
	case CSB_TWO:
		opt.c_cflag |= CSTOPB;
		break;
	default:
		opt.c_cflag &= ~CSTOPB;
		break;
	}

	if (parity != CP_NO)
		opt.c_iflag |= INPCK;
	tcflush(m_comfd, TCIFLUSH);
	opt.c_cc[VTIME] = 150; // 设置超时15 seconds
	opt.c_cc[VMIN] = 0; // Update the options and do it NOW

	// Raw Mode
	opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Input
	opt.c_oflag &= ~OPOST;							// Output

	// 设置流控
	switch (flowcontrol)
	{
	case 0:
		opt.c_cflag &= ~CRTSCTS;
		opt.c_iflag &= ~(IXON | IXOFF);
		break;
	case 1:
		opt.c_cflag |= CRTSCTS;
		opt.c_iflag &= ~(IXON | IXOFF);
		break;
	case 2:
		opt.c_iflag |= IXON | IXOFF;
		opt.c_cflag &= ~CRTSCTS;
		break;
	default:
		break;
	}

	if (tcsetattr(m_comfd, TCSANOW, &opt) != 0)
	{
		CloseComport();
		return false;
	}

	SockSetNonBlock(m_comfd, true);
#endif
	if (m_readSite)
		m_readSite->Open();
	if (m_writeSite)
		m_writeSite->Open();

	return true;
}

BOOL PCComport::WriteComportData(const void* data, int dataLen)
{
#if defined(WIN32)
	if (m_writeSite == NULL || data == NULL || dataLen <= 0)
		return false;
	return m_writeSite->InputData(data, dataLen);
#elif defined(__linux__)
	return false;
#endif
}

void PCComport::CloseComport()
{
	if (m_readSite)
	{
		m_readSite->Close();
		delete m_readSite;
		m_readSite = NULL;
	}
	if (m_writeSite)
	{
		m_writeSite->Close();
		delete m_writeSite;
		m_writeSite = NULL;
	}
#if defined(WIN32)
	if (m_hComport != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComport);
		m_hComport = INVALID_HANDLE_VALUE;
	}
#elif defined(__linux__)
	if (m_comfd > 0)
	{
		close(m_comfd);
		m_comfd = -1;
	}
#endif
}

PCComport::ComReadSite::ComReadSite(PCComport* parent, ComReadHandler* handler)
									: m_parent(parent)
									, m_thReadHandle(NULL)
									, m_exit(true)
									, m_readHandler(handler)
{
#if defined(WIN32)
	memset(&m_ovRead, 0, sizeof(OVERLAPPED));
#endif
}

PCComport::ComReadSite::~ComReadSite()
{

}

void PCComport::ComReadSite::Open()
{
	m_readBuf.Init();
	RequestRead(m_readBuf.buf, m_readBuf.needLen, m_readSatus);
	m_exit = false;
#if defined(WIN32)
	m_ovRead.hEvent = CreateEvent(NULL, false, false, NULL);
	ASSERT(m_ovRead.hEvent != NULL);
#endif
	OSCreateThread(&m_thReadHandle, NULL, ComReadSite::THWorker, this, 0);
}

void PCComport::ComReadSite::Close()
{
	m_exit = true;
#if defined(WIN32)
	if (m_ovRead.hEvent != NULL)
		SetEvent(m_ovRead.hEvent);
#endif
	if (m_thReadHandle != NULL)
	{
		OSCloseThread(m_thReadHandle);
		m_thReadHandle = NULL;
	}
#if defined(WIN32)
	if (m_ovRead.hEvent != NULL)
		CloseHandle(m_ovRead.hEvent);
#endif
}

int PCComport::ComReadSite::THWorker(void* p)
{
	ComReadSite* pThis = (ComReadSite*)p;
	return pThis->OnWorker();
}

int PCComport::ComReadSite::OnWorker()
{
	while (!m_exit)
	{
		if (m_readBuf.IsNull())
			break;
#if defined(WIN32)
		DWORD dwRead = 0;
		BOOL ret = ReadFile(m_parent->GetComHandle(),
			(char*)m_readBuf.buf + m_readBuf.bufLen,
			m_readBuf.needLen - m_readBuf.bufLen,
			&dwRead, &m_ovRead);
		if (!ret)	//没读到数据
		{
			DWORD rst = WaitForSingleObject(m_ovRead.hEvent, INFINITE);
			if (rst == WAIT_OBJECT_0)
			{
				if (m_exit)
					break;	//退出
				else
				{
					//读到数据或退出m_ovRead.InternalHigh
					dwRead = m_ovRead.InternalHigh;
				}
			}
			else
				break;
		}

		ASSERT(dwRead > 0);
		m_readBuf.bufLen += dwRead;
		if (m_readSatus == PCComport::CRS_MAX || m_readBuf.bufLen == m_readBuf.needLen)
		{
			OnRead(m_readBuf.buf, m_readBuf.bufLen);
			m_readBuf.Init();
			RequestRead(m_readBuf.buf, m_readBuf.needLen, m_readSatus);
		}
#elif defined(__linux__)
		int fd = m_parent->GetComFd();
		int r = FDPoll(fd, 0x01, 50);
		if (r < 0)
			break;
		else if (r == 0)
			continue;
		else
		{
			int n = read(fd,
				(char*)m_readBuf.buf + m_readBuf.bufLen,
				m_readBuf.needLen - m_readBuf.bufLen);
			ASSERT(n > 0);
			m_readBuf.bufLen += n;
			if (m_readSatus == PCComport::CRS_MAX || m_readBuf.bufLen == m_readBuf.needLen)
			{
				OnRead(m_readBuf.buf, m_readBuf.bufLen);
				m_readBuf.Init();
				RequestRead(m_readBuf.buf, m_readBuf.needLen, m_readSatus);
			}
		}
#endif
	}
	return 0;
}

PCComport::ComWriteSite::ComWriteSite(PCComport* parent)
										: m_parent(parent)
										, m_thWriteHandle(NULL)
										, m_exit(true)
{
#if defined(WIN32)
	memset(&m_ovWrite, 0, sizeof(OVERLAPPED));
#endif
}

PCComport::ComWriteSite::~ComWriteSite()
{

}

void PCComport::ComWriteSite::Open()
{
	m_exit = false;
#if defined(WIN32)
	m_ovWrite.hEvent = CreateEvent(NULL, false, false, NULL);
	ASSERT(m_ovWrite.hEvent != NULL);
#endif
	OSCreateThread(&m_thWriteHandle, NULL, ComWriteSite::THWorker, this, 0);
}

void PCComport::ComWriteSite::Close()
{
	m_exit = true;
#if defined(WIN32)
	if (m_ovWrite.hEvent != NULL)
		SetEvent(m_ovWrite.hEvent);
#endif
	if (m_thWriteHandle != NULL)
	{
		OSCloseThread(m_thWriteHandle);
		m_thWriteHandle = NULL;
	}
#if defined(WIN32)
	if (m_ovWrite.hEvent != NULL)
		CloseHandle(m_ovWrite.hEvent);
#endif
	if (!m_wtBuf.IsNull())
	{
		free(m_wtBuf.buf);
		m_wtBuf.Init();
	}
	CXTAutoLock lock(m_locker);
	for (std::list<ReadOrWriteBuf>::iterator it = m_bufferList.begin(); it != m_bufferList.end(); ++it)
	{
		ReadOrWriteBuf& buf = *it;
		if (!buf.IsNull())
			free(buf.buf);
	}
	m_bufferList.clear();
}

BOOL PCComport::ComWriteSite::InputData(const void* data, int dataLen)
{
	ReadOrWriteBuf buf;
	buf.buf = malloc(dataLen);
	if (buf.buf == NULL)
		return false;
	buf.needLen = dataLen;
	buf.bufLen = 0;
	memcpy(buf.buf, data, dataLen);

	CXTAutoLock lock(m_locker);
	if (m_exit)
	{
		free(buf.buf);
		return false;
	}
	m_bufferList.push_back(buf);
	return true;
}

int PCComport::ComWriteSite::THWorker(void* p)
{
	ComWriteSite* pThis = (ComWriteSite*)p;
	return pThis->OnWorker();
}

int PCComport::ComWriteSite::OnWorker()
{
	while (!m_exit)
	{
		if (m_wtBuf.IsNull())
		{
			if (m_bufferList.empty())
			{
				Sleep(1);
				continue;
			}

			CXTAutoLock lock(m_locker);
			if (m_bufferList.empty())
			{
				Sleep(1);
				continue;
			}

			m_wtBuf = m_bufferList.front();
			m_bufferList.pop_front();
		}
#if defined(WIN32)
		DWORD dwWrite = 0;
		BOOL ret = WriteFile(m_parent->GetComHandle(),
			(char*)m_wtBuf.buf + m_wtBuf.bufLen,
			m_wtBuf.needLen - m_wtBuf.bufLen,
			&dwWrite, &m_ovWrite);
		if (!ret)
		{
			DWORD rst = WaitForSingleObject(m_ovWrite.hEvent, INFINITE);
			if (rst == WAIT_OBJECT_0)
			{
				if (m_exit)
					break;	//退出
				else
				{
					//读到数据或退出m_ovRead.InternalHigh
					dwWrite = m_ovWrite.InternalHigh;
				}
			}
			else
				break;
		}
		ASSERT(dwWrite > 0);
		m_wtBuf.bufLen += dwWrite;
#elif defined(__linux__)
		int fd = m_parent->GetComFd();
		int r = FDPoll(fd, 0x04, 50);
		if (r < 0)
			break;
		else if (r == 0)
			continue;
		else
		{
			int n = write(fd,
				(char*)m_wtBuf.buf + m_wtBuf.bufLen,
				m_wtBuf.needLen - m_wtBuf.bufLen);
			ASSERT(n > 0);
			m_wtBuf.bufLen += n;
		}
#endif
		if (m_wtBuf.needLen == m_wtBuf.bufLen)
		{
			free(m_wtBuf.buf);
			m_wtBuf.Init();
		}
	}
	return 0;
}
