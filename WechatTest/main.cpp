#include <iostream>
#include "stdlib.h"
#include <tchar.h>
#include <Windows.h>

bool Inject(DWORD dwId, char * szPath)//����1��Ŀ�����PID  ����2��DLL·��
{
	//һ����Ŀ�����������һ���ռ�


	/*
	��1.1 ��ȡĿ����̾����
	����1����Ҫӵ�еĽ���Ȩ�ޣ�����Ϊ�����ܻ�õ�Ȩ�ޣ�
	����2����ʾ���õ��Ľ��̾���Ƿ���Ա��̳�
	����3�����򿪽��̵�PID
	����ֵ:ָ�����̵ľ��
	*/
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwId);


	/*
	��1.2 ��Ŀ����̵��ڴ��￪�ٿռ䡿
	����1��Ŀ����̾��
	����2������ҳ����ڴ��ַ��һ����NULL�Զ�����
	����3����������ڴ��С���ֽڵ�λ
	����4��MEM_COMMIT��Ϊ�ض���ҳ����������ڴ��л���̵�ҳ���ļ��е�����洢
	����5��PAGE_READWRITE ����ɱ�Ӧ�ó����д
	����ֵ��ִ�гɹ��ͷ��ط����ڴ���׵�ַ�����ɹ�����NULL
	*/
	LPVOID pRemoteAddress = VirtualAllocEx(
		hProcess,
		NULL,
		1,
		MEM_COMMIT,
		PAGE_READWRITE
	);

	//���� ��dll��·��д�뵽Ŀ����̵��ڴ�ռ���

	DWORD dwWriteSize = 0;
	/*
	��дһ�����ݵ��ղŸ�ָ�����������ٵ��ڴ�ռ��
	����1��OpenProcess���صĽ��̾��
	����2��׼��д����ڴ��׵�ַ
	����3��ָ��Ҫд�����ݵ�ָ�루׼��д��Ķ�����
	����4��Ҫд����ֽ����������ĳ���+0/��
	����5�� ����ֵ������ʵ��д����ֽ�
	*/
	WriteProcessMemory(hProcess,
		pRemoteAddress,
		szPath,
		strlen(szPath) * 1 + 1,
		&dwWriteSize);


	//���� ����һ��Զ���̣߳���Ŀ����̵���LoadLibrary

	/*
	����1����Զ���߳��������̵Ľ��̾��
	����2��һ��ָ�� SECURITY_ATTRIBUTES �ṹ��ָ��, �ýṹָ�����̵߳İ�ȫ����
	����3���߳�ջ��ʼ��С,���ֽ�Ϊ��λ,�����ֵ��Ϊ0,��ôʹ��ϵͳĬ�ϴ�С
	����4����Զ�̽��̵ĵ�ַ�ռ���,���̵߳��̺߳�������ʼ��ַ��Ҳ��������߳̾���Ҫ�ɵĻ����
	����5�������̺߳����Ĳ������ղ����ڴ��￪�ٵĿռ�����д��Ķ�����
	����6�������̴߳����ı�־��0��NULL����ʾ���߳��ڴ�������������
	����7��ָ������̱߳�ʶ���ı�����ָ�롣����˲���ΪNULL���򲻷����̱߳�ʶ��
	����ֵ����������ɹ����򷵻�ֵ�����̵߳ľ�����������ʧ�ܣ��򷵻�ֵΪNULL
	*/
	HANDLE hThread = CreateRemoteThread(
		hProcess,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)LoadLibrary,
		pRemoteAddress,
		NULL,
		NULL
	);
	if (hThread == NULL) {
		printf("CreateRemoteThread fail:%d\n", (int)hThread);
	}
	else
	{
		printf("hThread:%d\n", (int)hThread);
	}

	WaitForSingleObject(hThread, -1); //�������ָ���߳����źŵ�ʱ�򣬲Ż᷵��

	/*
	�ġ� ���ͷ�����������ڴ�ռ䡿
	����1��Ŀ����̵ľ�����þ������ӵ�� PROCESS_VM_OPERATION Ȩ��
	����2��ָ��Ҫ�ͷŵ������ڴ�ռ��׵�ַ��ָ��
	����3�������ڴ�ռ���ֽ���
	����4��MEM_DECOMMIT����ʾ�ڴ�ռ䲻���ã��ڴ�ҳ�������ڡ�
		   MEM_RELEASE���ַ�ʽ�ܳ��ף���ȫ���ա�
	*/
	VirtualFreeEx(hProcess, pRemoteAddress, 1, MEM_DECOMMIT);
	return 0;
}


int _tmain(int argc, _TCHAR * argv[])
{
	//��������Ŀ����̵�·�� ����������ʱʹ�þ���·��
	char wStr[] = "D:\\source\\repos\\SharkDll\\Debug\\SharkDll.dll";

	DWORD dwId = 0;

	//����1��(NULL
	//����2��Ŀ�괰�ڵı���
	//����ֵ��Ŀ�괰�ڵľ��
	//HWND hCalc = FindWindow("TfrmMain", NULL);
	HWND hCalc = FindWindow(NULL,"΢��");
	//HWND hCalc = FindWindow("IEFrame", NULL);
	printf("Ŀ�괰�ڵľ��Ϊ:%d\n", (int)hCalc);

	DWORD dwPid = 0;

	//����1��Ŀ����̵Ĵ��ھ��
	//����2����Ŀ����̵�PID��Ž�ȥ
	DWORD dwRub = GetWindowThreadProcessId(hCalc, &dwPid);
	printf("Ŀ�괰�ڵĽ���PIDΪ:%d\n", dwPid);
	//return 0;
	//����1��Ŀ����̵�PID
	//����2����Ҫע��DLL��·��
	Inject(dwPid, wStr);

	return 0;
}