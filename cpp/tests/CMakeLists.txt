project(G2pTest)

file(GLOB_RECURSE _src *.h *.cpp)

if (WIN32)
    set(PLATFORM_LIB dwmapi.lib)
endif ()

add_executable(${PROJECT_NAME} ${_src})

target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} PUBLIC
        ${PLATFORM_LIB} IKg2p
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/testData
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/testData
)