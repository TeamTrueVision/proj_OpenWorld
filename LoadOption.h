/* LoadOption.h
 * ����: ������ �ɼ��� �ҷ����ų� ���� ����
 * �ڴ�: ����
 */

#pragma once

class openOption
{
private:
	char szFileName[256];

public:
	openOption();
	~openOption();

	void OptionFile(const char *filename); //ini ������ �̸� ����

	char *getOptionByString(char *field, char *option); //char* �������� �� ����
	int getOptionByInt(char *field, char *option); //int �������� �� ����

	int setOptionByInt(char *field, char *option, int value);
	int setOptionByChar(char *field, char *option, char *value);
};

static openOption Option;