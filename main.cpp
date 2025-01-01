#include "winrt/MyNamespace.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/base.h"

#include <Windows.h>

namespace winrt::MyNamespace
{
  struct 
    __declspec(uuid("ddc36e02-18ac-47c4-ae17-d420eece2281"))
    MyInterfaceImp : implements<MyInterfaceImp, IMyInterface>
    {
      winrt::hstring GetUsername()
      {
        WCHAR buffer[1024] = {};
        DWORD cBuffer = sizeof(buffer)/sizeof(*buffer);
        GetUserNameW(buffer, &cBuffer);
        return winrt::hstring{ buffer };
      }
    };

}

int main (int argc, char *argv[])
{
  winrt::init_apartment();
  auto mycoclass_instance{ winrt::make<winrt::MyNamespace::MyInterfaceImp>() };
  return 0;
}
