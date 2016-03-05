#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

using namespace std;


bool injectDLL(DWORD processID);

char fileToInject[] = "ACubeDLL.dll"; //Dll name to inject
char processName[] = "ac_client.exe"; //Name of process
char dllPath[250] = "C:\\Fun\\"; //File location
typedef HINSTANCE(*fpLoadLibrary)(char*);

int main()
{
	DWORD processID = NULL; //Dword is 32bit unsigned int, meant to hold process id

	PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) }; // holds processEntry from process table
	HANDLE hProcSnap;

	while (!processID) //Enter loop of finding process, takes multiple snapshots of system processes
	{
		system("CLS");
		cout << "Searching for " << processName << "..." << endl;
		cout << "make sure your game is running" << endl;
		hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //Takes a picture of all the processes in the system, 0 indicates the current process

		if (Process32First(hProcSnap, &pe32)) //Stores the next process entry in pe32
		{
			do
			{
				if (!strcmp(pe32.szExeFile, processName)) //Check process name against desired name
				{
					processID = pe32.th32ProcessID; //Grab the correct process ID
					break;
				}
			} while (Process32Next(hProcSnap, &pe32)); //Gets next pe32 structure
			Sleep(1000);
		}
	}

	while (!injectDLL(processID))
	{
		system("CLS");
		cout << "DLL failed to inject" << endl;
		Sleep(1000);
	}

	cout << "DLL inject sucessful" << endl << endl;
	cout << "Closing injector..." << endl;

	CloseHandle(hProcSnap);
	Sleep(5000);
	return 0;
}

bool injectDLL(DWORD processID)
{
	HANDLE hProc;
	LPVOID paramAddr;

	HINSTANCE hDLL = LoadLibrary("KERNEL32");
	fpLoadLibrary loadLibraryAddr = (fpLoadLibrary)GetProcAddress(hDLL, "LoadLibraryA");

	hProc = OpenProcess(PROCESS_ALL_ACCESS, false, processID); //Open desired process object with all access

	strcat(dllPath, fileToInject); //Add user determined path to front of file to inject

	paramAddr = VirtualAllocEx(hProc, 0, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE); //add virtual memory to process, make room for dll
	bool memoryWritten = WriteProcessMemory(hProc, paramAddr, dllPath, strlen(dllPath) + 1, NULL);	//Write dll to process memory 

	CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, paramAddr, 0, 0); //Start new thread running dll as 32bit

	CloseHandle(hProc); //Close handle to the process
	return memoryWritten;	//Return wether the memory was written to the process

};