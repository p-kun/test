#include <windows.h>
#include <stdio.h>
#include <setupAPI.h>
#include <cfgmgr32.h>
#include <TCHAR.h>

#pragma comment(lib, "Cfgmgr32.lib")
#pragma comment(lib, "SetupAPI.lib")

int RejectUsbMemory()
{
  HDEVINFO         hDevInfo;
  SP_DEVINFO_DATA  device;
  DWORD            i;

  hDevInfo = SetupDiGetClassDevs( NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES );

  if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        /* ハンドルの取得に失敗 */

        return 1;
    }

  device.cbSize = sizeof(device);

  for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &device); i++)
    {
      /* デバイスのステータスを取得 */

      ULONG     status  = 0;
      ULONG     problem = 0;
      CONFIGRET cr;

      cr = CM_Get_DevNode_Status(&status, &problem, device.DevInst, 0);

      if (cr != CR_SUCCESS)
        {
          continue;
        }

      if (!(status & DN_DISABLEABLE) || !(status & DN_REMOVABLE))
        {
          continue;
        }

      BOOL   ret = FALSE;
      BYTE  *value = NULL;
      DWORD  type;
      DWORD  size = 0;

      while(1)
        {
          ret = ::SetupDiGetDeviceRegistryProperty(hDevInfo, &device, SPDRP_SERVICE, &type, value, size, &size);

          if (ret || (GetLastError() != ERROR_INSUFFICIENT_BUFFER))
            {
                break;
            }

          if (value)
            {
              delete value;
            }

          value = new BYTE[size];
        }

      if (*value == *("USBSTOR"))
        {
          if (device.DevInst > 0)
            {
              if (CM_Request_Device_Eject(device.DevInst, NULL, NULL, 0, 0) == CR_SUCCESS)
                {
                  /* 成功 */;
                }
              else
                {
                  /* 失敗 */;
                }
            }
        }
    }

  return 0;
}


int _tmain( int argc, TCHAR **argv )
{
  HDEVINFO         hDevInfo;
  SP_DEVINFO_DATA  device;

  hDevInfo = SetupDiGetClassDevs( NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);

  if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        /* ハンドルの取得に失敗 */

        return 1;
    }

  SP_DEVINSTALL_PARAMS  DeviceInstallParams;

  device.cbSize = sizeof(device);

  memset(&DeviceInstallParams, 0, device.cbSize);

  DeviceInstallParams.cbSize  = sizeof(DeviceInstallParams);
  DeviceInstallParams.Flags   = DI_ENUMSINGLEINF;
  DeviceInstallParams.FlagsEx = DI_FLAGSEX_DRIVERLIST_FROM_URL;
/*
  DeviceInstallParams.FlagsEx;
  DeviceInstallParams.hwndParent;
  DeviceInstallParams.InstallMsgHandler;
  DeviceInstallParams.InstallMsgHandlerContext;
  DeviceInstallParams.FileQueue;
  DeviceInstallParams.ClassInstallReserved;
  DeviceInstallParams.Reserved;
  DeviceInstallParams.DriverPath[MAX_PATH];
*/





  for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &device); i++)
    {
      ULONG     status  = 0;
      ULONG     problem = 0;
      CONFIGRET cr;

      if (SetupDiGetDeviceInstallParams(hDevInfo, &device, &DeviceInstallParams))
        {
          _tprintf(L"OK %s\n", DeviceInstallParams.DriverPath);
        }



      cr = CM_Get_DevNode_Status(&status, &problem, device.DevInst, 0);

      if (cr != CR_SUCCESS)
        {
          continue;
        }

      if (!(status & DN_REMOVABLE))
        {
          continue;
        }

      if (!(status & DN_DISABLEABLE))
        {
          continue;
        }

//    _tprintf(L"%d : %x %x %x\n", i, device.DevInst, status, problem);

      BOOL   ret;
      TCHAR  value[256];
      DWORD  type;
//    DWORD  size = 0;

//    ret = ::SetupDiGetDeviceRegistryProperty(hDevInfo, &device, SPDRP_SERVICE, &type, NULL, 0, &size);

//    printf("1 <%02X> %d %d %d\n", ret, type, size, GetLastError());
//    printf("%02X\n", value[0]);
//    printf("%02X\n", value[1]);

      ret = ::SetupDiGetDeviceRegistryProperty(hDevInfo, &device, SPDRP_FRIENDLYNAME, &type, (PBYTE)value, 256, NULL);
      if (ret)
        {
          _tprintf(L"%d: %d %d\n", SPDRP_FRIENDLYNAME, type, GetLastError());
          _tprintf(L"%d: %s\n", SPDRP_FRIENDLYNAME, value);
        }

      ret = ::SetupDiGetDeviceProperty(hDevInfo, &device);
    }

  return 0;
}
