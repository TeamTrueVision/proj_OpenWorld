/* ApplicationLogger.h
 * ����: ���� ����� ������ ���� ���� ���
 * �ڴ�: ����
 */

#pragma once

extern void ErrorMessageBox(const char *msg);

class openLogger
{
private:
public:
	openLogger();
	~openLogger();

	void write(const char *fmt, ...); //���Ͽ� �α�
	void dbgc(const char *fmt, ...); //�ܼ�â�� �����α�
};

static openLogger ApplicationLogger;