file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.c)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/raygui/include)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})