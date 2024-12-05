cmake_minimum_required(VERSION 3.0)
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME} 
	src/main.c
)

if(NOT CMAKE_BUILD_TYPE) 
	set(CMAKE_BUILD_TYPE Release) 
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:${PROJECT_NAME}>)
endif() 

if(CMAKE_BUILD_TYPE STREQUAL "Release") 
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:${PROJECT_NAME}>)
endif()

set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -flto") 
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections -flto")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g") 
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")

target_link_libraries(${PROJECT_NAME} ctk)
