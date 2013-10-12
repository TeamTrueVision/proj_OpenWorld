/* LoadOption.cpp
 * ÄÚ´õ: °í¼ºÈÆ
 */

#include <Windows.h>
#include <string>
#include <tchar.h>

#include "LoadOption.h"
#include "ApplicationLogger.h"

char szTmp[30] = {NULL,};
int nTmp = 0;

openOption::openOption()
{
	ApplicationLogger.write("ini option loader initialized.");
}

openOption::~openOption()
{
}

void openOption::OptionFile(const char *filename)
{
	strcpy_s(szFileName, filename);
	ApplicationLogger.write("ini loader>> filename: %s", filename);
}

char *openOption::getOptionByString(char *field, char *option)
{
	if(!GetPrivateProfileString(field, option, 0, szTmp, sizeof(szTmp), szFileName)) {
		ApplicationLogger.write("ini loader error: ini file - %s, field - %s, option - %s", szFileName, field, option);
		return NULL;
	}
	else return szTmp;
}

int openOption::getOptionByInt(char *field, char *option)
{
	nTmp = GetPrivateProfileInt(field, option, -1, szFileName);

	if(nTmp == -1) {
		ApplicationLogger.write("ini loader error: ini file - %s, field - %s, option - %s", szFileName, field, option);
		return -1;
	}
	else return nTmp;
}

int openOption::setOptionByInt(char *field, char *option, int value)
{
	char buf[256];

	_itoa_s(value, buf, 10);

	if(!WritePrivateProfileString(field, option, buf, szFileName)) {
		ApplicationLogger.write("ini value set error: ini file - %s, field - %s, option - %s", szFileName, field, value);
		return 0;
	} else return 1;
}

int openOption::setOptionByChar(char *field, char *option, char *value)
{
	if(!WritePrivateProfileString(field, option, value, szFileName)) {
		ApplicationLogger.write("ini value set error: ini file - %s, field - %s, option - %s", szFileName, field, value);
		return 0;
	} else return 1;
}