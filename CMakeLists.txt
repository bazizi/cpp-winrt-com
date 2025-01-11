cmake_minimum_required(VERSION 3.10)

# Dependencies
include(FetchContent)

project(cpp-winrt-com)

set(CPPWINRT_BUILD_VERSION 2.0.240405.15)
FetchContent_Declare(
  CppWinRt
  URL https://www.nuget.org/api/v2/package/Microsoft.Windows.CppWinRT/${CPPWINRT_BUILD_VERSION}
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(CppWinRt)

set(WINDOWSSDKDIR  "C:\\Program Files (x86)\\Windows Kits\\10\\References\\10.0.22000.0")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}\\$(Configuration)\\MyInterface.winmd"
  COMMAND 
    midl /metadata_dir "${WINDOWSSDKDIR}\\windows.foundation.foundationcontract\\4.0.0.0"
    /winrt ${CMAKE_SOURCE_DIR}\\MyInterface.idl /out "${CMAKE_BINARY_DIR}\\$(Configuration)\\"
  DEPENDS ${CMAKE_SOURCE_DIR}\\MyInterface.idl
)
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}\\winrt\\MyNamespace.h"
  COMMAND 
    ${CMAKE_BINARY_DIR}\\_deps\\cppwinrt-src\\bin\\cppwinrt.exe /reference "${WINDOWSSDKDIR}\\Windows.Foundation.FoundationContract\\4.0.0.0\\Windows.Foundation.FoundationContract.winmd" 
    /input "${CMAKE_BINARY_DIR}\\$(Configuration)\\MyInterface.winmd"
  DEPENDS ${CMAKE_BINARY_DIR}\\_deps\\cppwinrt-src\\bin\\cppwinrt.exe
)
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}\\winrt\\Windows.Foundation.h"
  COMMAND 
    ${CMAKE_BINARY_DIR}\\_deps\\cppwinrt-src\\bin\\cppwinrt.exe /input "${WINDOWSSDKDIR}\\Windows.Foundation.FoundationContract\\4.0.0.0\\Windows.Foundation.FoundationContract.winmd" 
    /input "${CMAKE_BINARY_DIR}\\$(Configuration)\\MyInterface.winmd"
  DEPENDS "${CMAKE_BINARY_DIR}\\winrt\\MyNamespace.h"
)
add_custom_target(
  custom_target
  DEPENDS 
  "${CMAKE_BINARY_DIR}\\$(Configuration)\\MyInterface.winmd"
    "${CMAKE_BINARY_DIR}\\winrt\\MyNamespace.h"
    "${CMAKE_BINARY_DIR}\\winrt\\Windows.Foundation.h"
)

include_directories(
  ${CMAKE_BINARY_DIR}
)

add_executable(server server.cpp)
add_dependencies(server custom_target)
set_target_properties(server PROPERTIES CXX_STANDARD 17)
target_link_libraries(server RuntimeObject.lib)

add_executable(client client.cpp)
set_target_properties(client PROPERTIES CXX_STANDARD 17)
target_link_libraries(client RuntimeObject.lib)
add_dependencies(client server)

add_executable(serverInstaller serverInstaller.cpp)
