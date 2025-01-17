#include "winrt/MyNamespace.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/base.h"

#include <combaseapi.h>
#include <iostream>
#include <objbase.h>
#include <winerror.h>
#include <wrl.h>
#include <Windows.h>
#include <wrl/module.h>

#include <iostream>

#include <clsid.h>
#include <wtypesbase.h>

int main (int argc, char *argv[])
{
  CoInitialize(nullptr);

  CLSID clsid = {};
  if (auto res = CLSIDFromString(L"{" INTERFACE_CLSID_W L"}", &clsid); res != NOERROR)
  {
    std::wcout << L"Failed to convert CLSID " << INTERFACE_CLSID_W;
    return res;
  }

  try{
    auto instance = winrt::create_instance<winrt::MyNamespace::IMyInterface>(clsid, CLSCTX_LOCAL_SERVER, nullptr);
    auto const username = instance.GetUsername();
    std::wcout << "username is: " << username.c_str() << std::endl;
  }
  catch (winrt::hresult_error const& hr)
  {
    std::wcout << hr.message().c_str() << std::endl;
  }

  CoUninitialize();
  return 0;
}
