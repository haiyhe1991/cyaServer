#include "CfgCom.h"
#include <fstream>

BOOL OpenCfgFile(const char* filename, Json::Value& rootValue)
{
	std::ifstream ifs;
#if defined(WIN32)
	WCHAR wszFilePath[MAX_PATH] = { 0 };
	ifs.open(ctowc(filename, wszFilePath, sizeof(wszFilePath)));
#else
	ifs.open(filename);
#endif
	if (!ifs.is_open())
		return false;

	Json::Reader reader;
	if (!reader.parse(ifs, rootValue))
	{
		ifs.close();
		return false;
	}
	ifs.close();
	return true;
}