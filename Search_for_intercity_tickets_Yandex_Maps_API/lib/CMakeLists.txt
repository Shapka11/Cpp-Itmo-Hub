add_library(timetable APIInfo.cpp ConsoleInterface.cpp Processing.cpp CashInfo.cpp)

include_directories(${CMAKE_SOURCE_DIR}/cpr/include)
include_directories(${CMAKE_SOURCE_DIR}/json/include)