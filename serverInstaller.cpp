#include "clsid.h"
#include <AclAPI.h>
#include <Windows.h>
#include <combaseapi.h>
#include <rpc.h>
#include <sddl.h>
#include <wil/resource.h>
#include <wil/result_macros.h>
#include <string_view>
#include <iostream>

enum class Architecture
{
    X64,
    X86
};

// Registry Paths
constexpr wchar_t const* kBaseClassesRootClsidPath{ L"CLSID\\" };
constexpr wchar_t const* kBaseClassesRootAppIdPath{ L"AppID\\" };
constexpr wchar_t const* kComStringSecurityDescriptor
{
  L"O:PSG:BUD:"
    L"(A;;11;;;WD)"
    L"(A;;11;;;AC)"
    L"(A;;11;;;S-1-15-2-1723189366-2159580849-2248400763-1481059666-1951766778-2756563051-3565589001)"
    L"S:(ML;;NX;;;LW)"
};

inline HRESULT RegisterExtensionWithCOM(
    std::wstring_view clsId,
    std::wstring_view serverExecutablePath,
    Architecture serverArchitecture)
{
  std::wstring formattedClsId;
  formattedClsId.append(L"{").append(clsId).append(L"}");

  auto keyWow64{ serverArchitecture == Architecture::X64 ? KEY_WOW64_64KEY : KEY_WOW64_32KEY };

  wil::unique_hkey classesRootClsidKey;
  auto clsidRegistryPath{ kBaseClassesRootClsidPath + formattedClsId };
  RETURN_IF_WIN32_ERROR(RegCreateKeyExW(
        HKEY_CLASSES_ROOT,
        clsidRegistryPath.data(),
        0,
        nullptr,
        0,
        keyWow64 | KEY_CREATE_SUB_KEY | KEY_SET_VALUE,
        nullptr,
        classesRootClsidKey.addressof(),
        nullptr));

  RETURN_IF_WIN32_ERROR(
      RegSetValueExW(classesRootClsidKey.get(), L"", 0, REG_SZ, (BYTE*)L"MyServerName", 10 * sizeof(wchar_t)));

  RETURN_IF_WIN32_ERROR(RegSetValueExW(
        classesRootClsidKey.get(),
        L"AppId",
        0,
        REG_SZ,
        (BYTE*)formattedClsId.data(),
        static_cast<DWORD>(formattedClsId.size() * sizeof(wchar_t))));

  wil::unique_hkey localServerKey;
  auto localServerSubkeyPath{ clsidRegistryPath + L"\\LocalServer32" };
  RETURN_IF_WIN32_ERROR(RegCreateKeyExW(
        HKEY_CLASSES_ROOT,
        localServerSubkeyPath.data(),
        0,
        nullptr,
        0,
        keyWow64 | KEY_CREATE_SUB_KEY | KEY_SET_VALUE,
        nullptr,
        localServerKey.addressof(),
        nullptr));

  RETURN_IF_WIN32_ERROR(RegSetValueExW(
        localServerKey.get(),
        L"",
        0,
        REG_SZ,
        (BYTE*)serverExecutablePath.data(),
        static_cast<DWORD>(serverExecutablePath.size() * sizeof(wchar_t))));

  wil::unique_hkey classesRootAppIdKey;
  auto appIdRegistryPath{ kBaseClassesRootAppIdPath + formattedClsId };
  RETURN_IF_WIN32_ERROR(RegCreateKeyExW(
        HKEY_CLASSES_ROOT,
        appIdRegistryPath.data(),
        0,
        nullptr,
        0,
        keyWow64 | KEY_CREATE_SUB_KEY | KEY_SET_VALUE,
        nullptr,
        classesRootAppIdKey.addressof(),
        nullptr));

  RETURN_IF_WIN32_ERROR(
      RegSetValueExW(classesRootAppIdKey.get(), L"", 0, REG_SZ, (BYTE*)L"ExtServer", 10 * sizeof(wchar_t)));

  DWORD appIdFlags = 0;
  RETURN_IF_WIN32_ERROR(
      RegSetValueExW(classesRootAppIdKey.get(), L"AppIdFlags", 0, REG_DWORD, (BYTE*)&appIdFlags, sizeof(appIdFlags)));

  const wchar_t interactiveUserStr[] = L"Interactive User";
  RETURN_IF_WIN32_ERROR(RegSetValueExW(
        classesRootAppIdKey.get(),
        L"RunAs",
        0,
        REG_SZ,
        (BYTE*)interactiveUserStr,
        ARRAYSIZE(interactiveUserStr) * sizeof(wchar_t)));

  wil::unique_hlocal_security_descriptor securityDescriptor{};
  DWORD securityDescriptorSize;
  RETURN_LAST_ERROR_IF(!ConvertStringSecurityDescriptorToSecurityDescriptorW(
        kComStringSecurityDescriptor,
        SDDL_REVISION_1,
        &securityDescriptor,
        &securityDescriptorSize));

  RETURN_IF_WIN32_ERROR(RegSetValueExW(
        classesRootAppIdKey.get(),
        L"LaunchPermission",
        0,
        REG_BINARY,
        reinterpret_cast<byte*>(securityDescriptor.get()),
        securityDescriptorSize));

  RETURN_IF_WIN32_ERROR(RegSetValueExW(
        classesRootAppIdKey.get(),
        L"AccessPermission",
        0,
        REG_BINARY,
        reinterpret_cast<byte*>(securityDescriptor.get()),
        securityDescriptorSize));

  return S_OK;
}

int wmain (int argc, wchar_t *argv[])
{
  if (argc != 2)
  {
    std::wcout << L"Usage: " << argv[0] << L" <full path to the server exe>";
    return -1;
  }

  for (auto const arch : {Architecture::X64, Architecture::X86})
  {
    if FAILED(RegisterExtensionWithCOM(INTERFACE_CLSID_W, argv[1], arch))
    {
      std::wcout << L"Failed to register the COM server with CLSID " << INTERFACE_CLSID_W << std::endl;
      return -2;
    }
  }
  return 0;
}
