/* LoadOption.h
 * 설명: 설정된 옵션을 불러오거나 새로 설정
 * 코더: 고성훈
 */

#pragma once

class openOption
{
private:
	char szFileName[256];

public:
	openOption();
	~openOption();

	void OptionFile(const char *filename); //ini 파일의 이름 설정

	char *getOptionByString(char *field, char *option); //char* 형식으로 값 리턴
	int getOptionByInt(char *field, char *option); //int 형식으로 값 리턴

	int setOptionByInt(char *field, char *option, int value);
	int setOptionByChar(char *field, char *option, char *value);
};

static openOption Option;