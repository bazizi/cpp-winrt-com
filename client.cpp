#include "winrt/MyNamespace.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/base.h"

#include <combaseapi.h>
#include <iostream>
#include <objbase.h>
#include <wrl.h>
#include <Windows.h>
#include <wrl/module.h>

#include <iostream>

int main (int argc, char *argv[])
{
  CoInitialize(nullptr);

  // {61DEBD51-F187-41CD-B2DE-5B7F1822F0BA}
  static const GUID guid = 
  { 0x61debd51, 0xf187, 0x41cd, { 0xb2, 0xde, 0x5b, 0x7f, 0x18, 0x22, 0xf0, 0xba } };

  try{
    auto instance = winrt::create_instance<winrt::MyNamespace::IMyInterface>(guid);
    auto const username = instance.GetUsername();
    std::wcout << username.c_str() << std::endl;
  }
  catch (winrt::hresult_error const& hr)
  {
    std::wcout << hr.message().c_str() << std::endl;
  }

  CoUninitialize();
  return 0;
}
