cmake_minimum_required(VERSION 3.5)
project(bulk_server VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

set(DEBUG_PRINT "0")

if (DEBUG_PRINT)
	message("DEBUG_PRINT is activated!")
	add_definitions(-DDEBUG_PRINT)
endif()

add_executable(bulk_server
	server.cpp
	server.h
	session.cpp
	session.h
	batchcommandprocessor.cpp
	batchcommandprocessor.h
	commandprocessor.cpp
	commandprocessor.h
	consoleoutput.cpp
	consoleoutput.h
	icommandprocessor.h
	inputprocessor.cpp
	inputprocessor.h
	reportwriter.cpp
	reportwriter.h
	structs.h
	threadedcommandprocessor.h
	threadedcommandprocessorimpl.h
	utils.h
	utils.cpp)

add_executable(bulk_client client.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set_target_properties(bulk_server PROPERTIES
	COMPILE_OPTIONS -Wpedantic -Wall -Wextra
	CMAKE_CXX_STANDARD 17
	CMAKE_CXX_STANDARD_REQUIRED ON
)

set(CMAKE_CXX_FLAGS "-std=c++1z")

target_link_libraries(bulk_server
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(bulk_client
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS bulk_server DESTINATION bin)
install(TARGETS bulk_client DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT mikhail.gorshkov@gmail.com)

include(CPack)
