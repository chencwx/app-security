#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#define MAX_THREADS 10
#define BUF_SIZE 255

DWORD WINAPI MyThreadFunction(LPVOID lpParam);

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct MyData {
	int val1;
	int val2;
} MYDATA, * PMYDATA;


int _tmain()//主函数，参数可以为unicode字符串
{
	PMYDATA pDataArray[MAX_THREADS];
	DWORD   dwThreadIdArray[MAX_THREADS];
	HANDLE  hThreadArray[MAX_THREADS];
	DWORD start = GetTickCount();//查看等待了多久
	// Create MAX_THREADS worker threads.

	for (int i = 0; i < MAX_THREADS; i++)
	{
		// Allocate memory for thread data.

		pDataArray[i] = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
			sizeof(MYDATA));
		
		if (pDataArray[i] == NULL)
		{
			// If the array allocation fails, the system is out of memory
			// so there is no point in trying to print an error message.
			// Just terminate execution.
			ExitProcess(2);
		}

		// Generate unique data for each thread to work with.

		pDataArray[i]->val1 = i;//赋值，传参
		pDataArray[i]->val2 = i + 100;
		MyThreadFunction(pDataArray[i]);
		// Create the thread to begin execution on its own.

		//hThreadArray[i] = CreateThread(
		//	NULL,                   // default security attributes
		//	0,                      // use default stack size  
		//	MyThreadFunction,       // thread function name，线程函数
		//	pDataArray[i],          // argument to thread function，传的参数即为前面的函数体 
		//	0,                      // use default creation flags 
		//	&dwThreadIdArray[i]);   // returns the thread identifier 保存id号


		// Check the return value for success.
		// If CreateThread fails, terminate execution. 
		// This will automatically clean up threads and memory. 

		/*if (hThreadArray[i] == NULL)
		{
			printf("CreateThread Eroor(%d)", GetLastError());
			
			ExitProcess(3);
		}*/
	} // End of main thread creation loop.
	
	// Wait until all threads have terminated.
	
	/*WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);*///等待十个对象，存在array里，十个都
	//等到才能返回，false表示只要一个等到就返回，infinite表示无限等待
	DWORD end = GetTickCount();//查看总共的时间
	printf("tick count:%d\n", end - start);//查看消耗的时间
	// Close all thread handles and free memory allocations.

	for (int i = 0; i < MAX_THREADS; i++)
	{
		CloseHandle(hThreadArray[i]);
		if (pDataArray[i] != NULL)
		{
			HeapFree(GetProcessHeap(), 0, pDataArray[i]);
			pDataArray[i] = NULL;    // Ensure address is not reused.
		}
	}

	return 0;
}


DWORD WINAPI MyThreadFunction(LPVOID lpParam) //等价printf
{

	PMYDATA pDataArray;
	// Cast the parameter to the correct data type.
	// The pointer is known to be valid because 
	// it was checked for NULL before the thread was created.

	pDataArray = (PMYDATA)lpParam;
	Sleep(1000);//等1000ms
	
	printf("Parameters = %d, %d\n", pDataArray->val1, pDataArray->val2);
	return 0;
}



