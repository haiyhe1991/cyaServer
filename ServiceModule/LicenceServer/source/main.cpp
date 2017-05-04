
#include "ConfigFileMan.h"
#include "LicenceCommon.h"
int main(int argc, char* argv[])
{
	InitConfigFileMan("riselcserver.properties");

	char pp[25] = {};
	GenerateEncryptKey(pp,25);
	printf("%s\n",pp);
	while (1);
	return 0;
}