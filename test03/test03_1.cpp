#pragma comment(lib, "SetupAPI.lib")

//#define STRICT
#include <Windows.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>
#include <tchar.h>
#define INITGUID
#include <devpkey.h>
#undef INITGUID

// デバイスの文字列型プロパティをヒープに確保して返します。
LPTSTR HeapAllocDevicePropertyString(
    HANDLE              HeapHandle,
    HDEVINFO            DeviceInfoSet,
    PSP_DEVINFO_DATA    DeviceInfoData,
    const DEVPROPKEY*   PropertyKey,
    DEVPROPTYPE*        PropertyType,
    PDWORD              CopiedSize,
    DWORD               Flags)
{
    DEVPROPTYPE PropType;
    DWORD Size;
    if (!SetupDiGetDeviceProperty(
        DeviceInfoSet,
        DeviceInfoData,
        PropertyKey,
        &PropType,
        nullptr, 0,
        &Size,
        0) && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        if (PropertyType != nullptr) {
            *PropertyType = DEVPROP_TYPE_NULL;
        }
        if (CopiedSize != nullptr) {
            *CopiedSize = 0;
        }
        return nullptr;
    }
    if (PropertyType != nullptr) {
        *PropertyType = PropType;
    }
    if (CopiedSize != nullptr) {
        *CopiedSize = Size;
    }
    if (PropType != DEVPROP_TYPE_STRING) {
        return nullptr;
    }
    LPTSTR buffer = (LPTSTR)HeapAlloc(HeapHandle, 0, Size);
    if (!SetupDiGetDeviceProperty(
        DeviceInfoSet,
        DeviceInfoData,
        PropertyKey,
        &PropType,
        (PBYTE)buffer, Size,
        &Size,
        0))
    {
        HeapFree(HeapHandle, 0, buffer);
        return nullptr;
    }
    return buffer;
    }

#include <iostream>

#if defined(UNICODE) || defined(_UNICODE)
#define tcout std::wcout
#else
#define tcout std::cout
#endif

void _tmain()
{
    // std::wcoutの日本語文字化けを回避
    std::wcout.imbue(std::locale("Japanese", std::locale::ctype));

    // 現在システムに存在する全てのクラス・インターフェイスを列挙
    tcout << TEXT("DIGCF_ALLCLASSES | DIGCF_PRESENT") << std::endl;
    HDEVINFO DevInfoHandle = SetupDiGetClassDevs(
        nullptr, nullptr, nullptr,
        DIGCF_ALLCLASSES | DIGCF_PRESENT);
    SP_DEVINFO_DATA DevInfoData = {sizeof SP_DEVINFO_DATA};
    for (DWORD i = 0; SetupDiEnumDeviceInfo(DevInfoHandle, i, &DevInfoData); i++)
    {
        LPTSTR ClassName = HeapAllocDevicePropertyString(
            GetProcessHeap(),
            DevInfoHandle,
            &DevInfoData,
            &DEVPKEY_Device_Class,
            nullptr,
            nullptr,
            0);
        LPTSTR DeviceDesc = HeapAllocDevicePropertyString(
            GetProcessHeap(),
            DevInfoHandle,
            &DevInfoData,
            &DEVPKEY_Device_DeviceDesc,
            nullptr,
            nullptr,
            0);
        tcout
            << (ClassName ? ClassName : TEXT(""))
            << TEXT(" - ")
            << (DeviceDesc ? DeviceDesc : TEXT(""))
            << std::endl;
        HeapFree(GetProcessHeap(), 0, DeviceDesc);
        HeapFree(GetProcessHeap(), 0, ClassName);
    }
    SetupDiDestroyDeviceInfoList(DevInfoHandle);
}
