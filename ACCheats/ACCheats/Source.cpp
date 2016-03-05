#include <iostream>
#include <Windows.h>
#include <string>
#include <ctime>

DWORD findDmaAddr(int PointerLevel, HANDLE hProcHandle, DWORD offsets[], DWORD BaseAddress);
void writeToMemory(HANDLE hProcHandle);


std::string GameName = "AssaultCube";
LPCWSTR LGameWindow = L"AssaultCube";
std::string GameStatus;

bool isGameAvail;
bool UpdateOnNextRun;

//AMMO VARS
bool AmmoStatus;
BYTE AmmoValue[] = {0xA3, 0X1C, 0x0, 0x0};
DWORD AmmoBaseAddr = { 0x00509B74 }; //Used final ammo address
DWORD AmmoOffsets[] = {0x384, 0x14, 0x0}; //Offset values for ammo

//Health VARS
bool HealthStatus;
BYTE HealthValue[] = { 0x39, 0x5, 0x0, 0x0 };
DWORD HealthBaseAddr = { 0x00509B74 }; //Used final ammo address
DWORD HealthOffsets[] = { 0xF8 }; //Offset values for health, 1 level in

int main()
{
	HWND  hGameWindow = NULL;
	int timeSinceLastUpdate = clock(), GameAvailTMR = clock(), onePressTMR = clock();
	DWORD dwProcID = NULL;
	HANDLE hProcHandle = NULL;
	UpdateOnNextRun = true;
	std::string sAmmoStatus = "OFF";
	std::string sHealthStatus = "OFF";

	while (!GetAsyncKeyState(VK_INSERT))
	{
		if (clock() - GameAvailTMR > 100)
		{
			GameAvailTMR = clock();
			isGameAvail = false;

			hGameWindow = FindWindow(NULL, LGameWindow); //find assaultcube window, first parameter is window class, second is window name
			if (hGameWindow)
			{
				GetWindowThreadProcessId(hGameWindow, &dwProcID); //Store window's process id
				if (dwProcID)
				{ 
					hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID); //getting handle of the application, process must be running
					if (hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL)
					{
						GameStatus = "Failed to open process for valid handle";
					}
					else
					{
						GameStatus = "AssaultCube ready to Hack";
						isGameAvail = true;
					}
				}
				else
				{
					GameStatus = "Failed to get process ID";
				}
			}
			else
			{
				GameStatus = "AssaultCube not found";
			}

			if (UpdateOnNextRun || clock() - timeSinceLastUpdate > 5000)
			{
				system("cls");
				std::cout << "-------------------------------------------------------------" << std::endl;
				std::cout << "			AssaultCube memory hacker" << std::endl;
				std::cout << "-------------------------------------------------------------" << std::endl << std::endl;
				std::cout << "GAME STATUS " << GameStatus << std::endl << std::endl;
				std::cout << "[F1] Unlimited Ammo -> " << sAmmoStatus << " <-" << std::endl << std::endl;
				std::cout << "[F2] Unlimited Health -> " << sHealthStatus << " <-" << std::endl << std::endl;
				std::cout << "[insert] Exit" << std::endl;

				UpdateOnNextRun = false;
				timeSinceLastUpdate = clock();
			}

			if (isGameAvail)
			{
				//write to memory
				writeToMemory(hProcHandle);
			}
		}

		//Allows turning on/off of hacks without accidentally pressing lots of times
		if (clock() - onePressTMR > 400)
		{
			if (isGameAvail)
			{
				//Ammo
				if (GetAsyncKeyState(VK_F1))
				{
					onePressTMR = clock();
					AmmoStatus = !AmmoStatus;
					UpdateOnNextRun = true;

					if (AmmoStatus)
					{
						sAmmoStatus = "ON";
					}
					else
					{
						sAmmoStatus = "OFF";
					}
				}

				//Health
				if (GetAsyncKeyState(VK_F2))
				{
					onePressTMR = clock();
					HealthStatus = !HealthStatus;
					UpdateOnNextRun = true;

					if (HealthStatus)
					{
						sHealthStatus = "ON";
					}
					else
					{
						sHealthStatus = "OFF";
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

DWORD findDmaAddr(int pointerLevel, HANDLE hProcHandle, DWORD offsets[], DWORD BaseAddress)
{
	DWORD pointer = BaseAddress, pTemp, pointerAddr;

	for (int c = 0; c < pointerLevel; c++)
	{
		if (c == 0)
		{
			//Store initial process memory into pTemp
			ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, sizeof(pTemp), NULL);
		}

		//Calculate pTemp + current offset
		pointerAddr = pTemp + offsets[c];

		//Read offset memory into pTemp
		ReadProcessMemory(hProcHandle, (LPCVOID)pointerAddr, &pTemp, sizeof(pTemp), NULL);
	}

	return pointerAddr;
}

void writeToMemory(HANDLE hProcHandle)
{
	DWORD AddressToWrite;
	if (AmmoStatus)
	{
		AddressToWrite = findDmaAddr(3, hProcHandle, AmmoOffsets, AmmoBaseAddr);
		WriteProcessMemory(hProcHandle, (BYTE*)AddressToWrite, &AmmoValue, sizeof(AmmoValue), NULL);
	}

	if (HealthStatus)
	{
		AddressToWrite = findDmaAddr(1, hProcHandle, HealthOffsets, HealthBaseAddr);
		WriteProcessMemory(hProcHandle, (BYTE*)AddressToWrite, &HealthValue, sizeof(HealthValue), NULL);
	}
}