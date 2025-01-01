cmake_minimum_required(VERSION 3.5)

# Dependencies
include(FetchContent)

project(ComServer)

set(CPPWINRT_BUILD_VERSION 2.0.240405.15)
FetchContent_Declare(
  CppWinRt
  URL https://www.nuget.org/api/v2/package/Microsoft.Windows.CppWinRT/${CPPWINRT_BUILD_VERSION}
)
FetchContent_MakeAvailable(CppWinRt)

set(WINDOWSSDKDIR  "C:\\Program Files (x86)\\Windows Kits\\10\\References\\10.0.22000.0")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}\\MyInterface.winmd"
  COMMAND 
    midl /metadata_dir "${WINDOWSSDKDIR}\\windows.foundation.foundationcontract\\4.0.0.0"
    /winrt ${CMAKE_SOURCE_DIR}\\MyInterface.idl
  DEPENDS ${CMAKE_SOURCE_DIR}\\MyInterface.idl
)
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}\\winrt\\MyNamespace.h"
  COMMAND 
    ${CMAKE_BINARY_DIR}\\_deps\\cppwinrt-src\\bin\\cppwinrt.exe /reference "${WINDOWSSDKDIR}\\Windows.Foundation.FoundationContract\\4.0.0.0\\Windows.Foundation.FoundationContract.winmd" 
    /input "${CMAKE_BINARY_DIR}\\MyInterface.winmd"
  DEPENDS ${CMAKE_BINARY_DIR}\\_deps\\cppwinrt-src\\bin\\cppwinrt.exe
)
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}\\winrt\\Windows.Foundation.h"
  COMMAND 
    ${CMAKE_BINARY_DIR}\\_deps\\cppwinrt-src\\bin\\cppwinrt.exe /input "${WINDOWSSDKDIR}\\Windows.Foundation.FoundationContract\\4.0.0.0\\Windows.Foundation.FoundationContract.winmd" 
    /input "${CMAKE_BINARY_DIR}\\MyInterface.winmd"
  DEPENDS "${CMAKE_BINARY_DIR}\\winrt\\MyNamespace.h"
)
add_custom_target(
  custom_target
  DEPENDS 
    "${CMAKE_BINARY_DIR}\\MyInterface.winmd"
    "${CMAKE_BINARY_DIR}\\winrt\\MyNamespace.h"
    "${CMAKE_BINARY_DIR}\\winrt\\Windows.Foundation.h"
)

include_directories(
  ${CMAKE_BINARY_DIR}
)

add_executable(CppWinRtServerExe main.cpp)
add_dependencies(CppWinRtServerExe custom_target)
set_target_properties(CppWinRtServerExe PROPERTIES CXX_STANDARD 17)
target_link_libraries(CppWinRtServerExe RuntimeObject.lib)
