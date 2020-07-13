#include <iostream>
#include "stdlib.h"
#include <tchar.h>
#include <Windows.h>

bool Inject(DWORD dwId, char * szPath)//参数1：目标进程PID  参数2：DLL路径
{
	//一、在目标进程中申请一个空间


	/*
	【1.1 获取目标进程句柄】
	参数1：想要拥有的进程权限（本例为所有能获得的权限）
	参数2：表示所得到的进程句柄是否可以被继承
	参数3：被打开进程的PID
	返回值:指定进程的句柄
	*/
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwId);


	/*
	【1.2 在目标进程的内存里开辟空间】
	参数1：目标进程句柄
	参数2：保留页面的内存地址，一般用NULL自动分配
	参数3：欲分配的内存大小，字节单位
	参数4：MEM_COMMIT：为特定的页面区域分配内存中或磁盘的页面文件中的物理存储
	参数5：PAGE_READWRITE 区域可被应用程序读写
	返回值：执行成功就返回分配内存的首地址，不成功就是NULL
	*/
	LPVOID pRemoteAddress = VirtualAllocEx(
		hProcess,
		NULL,
		1,
		MEM_COMMIT,
		PAGE_READWRITE
	);

	//二、 把dll的路径写入到目标进程的内存空间中

	DWORD dwWriteSize = 0;
	/*
	【写一段数据到刚才给指定进程所开辟的内存空间里】
	参数1：OpenProcess返回的进程句柄
	参数2：准备写入的内存首地址
	参数3：指向要写的数据的指针（准备写入的东西）
	参数4：要写入的字节数（东西的长度+0/）
	参数5： 返回值。返回实际写入的字节
	*/
	WriteProcessMemory(hProcess,
		pRemoteAddress,
		szPath,
		strlen(szPath) * 1 + 1,
		&dwWriteSize);


	//三、 创建一个远程线程，让目标进程调用LoadLibrary

	/*
	参数1：该远程线程所属进程的进程句柄
	参数2：一个指向 SECURITY_ATTRIBUTES 结构的指针, 该结构指定了线程的安全属性
	参数3：线程栈初始大小,以字节为单位,如果该值设为0,那么使用系统默认大小
	参数4：在远程进程的地址空间中,该线程的线程函数的起始地址（也就是这个线程具体要干的活儿）
	参数5：传给线程函数的参数（刚才在内存里开辟的空间里面写入的东西）
	参数6：控制线程创建的标志。0（NULL）表示该线程在创建后立即运行
	参数7：指向接收线程标识符的变量的指针。如果此参数为NULL，则不返回线程标识符
	返回值：如果函数成功，则返回值是新线程的句柄。如果函数失败，则返回值为NULL
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

	WaitForSingleObject(hThread, -1); //当句柄所指的线程有信号的时候，才会返回

	/*
	四、 【释放申请的虚拟内存空间】
	参数1：目标进程的句柄。该句柄必须拥有 PROCESS_VM_OPERATION 权限
	参数2：指向要释放的虚拟内存空间首地址的指针
	参数3：虚拟内存空间的字节数
	参数4：MEM_DECOMMIT仅标示内存空间不可用，内存页还将存在。
		   MEM_RELEASE这种方式很彻底，完全回收。
	*/
	VirtualFreeEx(hProcess, pRemoteAddress, 1, MEM_DECOMMIT);
	return 0;
}


int _tmain(int argc, _TCHAR * argv[])
{
	//这里是在目标进程的路径 所以我们暂时使用绝对路径
	char wStr[] = "D:\\source\\repos\\SharkDll\\Debug\\SharkDll.dll";

	DWORD dwId = 0;

	//参数1：(NULL
	//参数2：目标窗口的标题
	//返回值：目标窗口的句柄
	//HWND hCalc = FindWindow("TfrmMain", NULL);
	HWND hCalc = FindWindow(NULL,"微信");
	//HWND hCalc = FindWindow("IEFrame", NULL);
	printf("目标窗口的句柄为:%d\n", (int)hCalc);

	DWORD dwPid = 0;

	//参数1：目标进程的窗口句柄
	//参数2：把目标进程的PID存放进去
	DWORD dwRub = GetWindowThreadProcessId(hCalc, &dwPid);
	printf("目标窗口的进程PID为:%d\n", dwPid);
	//return 0;
	//参数1：目标进程的PID
	//参数2：想要注入DLL的路径
	Inject(dwPid, wStr);

	return 0;
}