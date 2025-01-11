#include "winrt/MyNamespace.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/base.h"

#include <wrl.h>
#include <Windows.h>
#include <wrl/module.h>
#include <future>

namespace winrt::MyNamespace
{
  template <typename T>
    class SingletonFactory : public ::Microsoft::WRL::ClassFactory<> // NOSONAR - Inheritance hierarchy depth of 6 and above is required by winrt
  {
    public:
      IFACEMETHODIMP CreateInstance(::IUnknown* unknownOuter, REFIID riid, void** object) noexcept override // NOSONAR - winrt requires void type
      {
        if (object == nullptr)
        {
          return CO_E_CLASS_CREATE_FAILED;
        }

        if (unknownOuter != nullptr)
        {
          return CLASS_E_NOAGGREGATION;
        }

        *object = nullptr;
        return T::Instance().as(riid, object);
      }
  };

  struct 
    __declspec(uuid("61DEBD51-F187-41CD-B2DE-5B7F1822F0BA"))
    MyInterfaceImp : implements<MyInterfaceImp, IMyInterface>
    {
      static winrt::com_ptr<winrt::MyNamespace::MyInterfaceImp> Instance();

      winrt::hstring GetUsername()
      {
        WCHAR buffer[1024] = {};
        DWORD cBuffer = sizeof(buffer)/sizeof(*buffer);
        GetUserNameW(buffer, &cBuffer);
        return winrt::hstring{ buffer };
      }

      MyInterfaceImp()
      {}
    };

  winrt::com_ptr<MyInterfaceImp> MyInterfaceImp::Instance()
  {
    return winrt::make_self<MyInterfaceImp>();
  }

  CoCreatableClassWithFactory(MyInterfaceImp, SingletonFactory<MyInterfaceImp>);
} // winrt::MyNamespace

int main (int argc, char *argv[])
{
  winrt::init_apartment();

  std::promise<void> promise;
  auto& mModule = ::Microsoft::WRL::Module<::Microsoft::WRL::OutOfProc>::Create([&promise](){promise.set_value();});
  mModule.RegisterObjects();
  promise.get_future().get();

  return 0;
}
