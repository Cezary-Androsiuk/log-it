cmake_minimum_required(VERSION 3.16)

project(Log LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/source)

add_executable(
    ${PROJECT_NAME}

    source/main.cpp

    source/Log/Log.h source/Log/Log.cpp
    source/SecondFileClass.h source/SecondFileClass.cpp

    # source/logQt/Log.cpp source/logQt/Log.h
    # source/trying_using_object/Log.h source/trying_using_object/Log.cpp

    # source/InstagramDownloader/Log.h source/InstagramDownloader/Log.cpp
    # source/InstagramDownloader/LogQML.h source/InstagramDownloader/LogQML.cpp
    # source/InstagramDownloader/LogSession.h source/InstagramDownloader/LogSession.cpp
    # source/InstagramDownloader/LogSessionPart.h source/InstagramDownloader/LogSessionPart.cpp

    # source/Petanque/Log.h source/Petanque/Log.cpp
    # source/Petanque/LogQML.h source/Petanque/LogQML.cpp
    # source/Petanque/LogSession.h source/Petanque/LogSession.cpp
    # source/Petanque/LogSessionPart.h source/Petanque/LogSessionPart.cpp
    # source/Petanque/LogTypeEnum.h source/Petanque/LogTypeEnum.cpp
)

# file(GLOB RESOURCE_FILES_LIST "${CMAKE_SOURCE_DIR}/resources/*")

# add_custom_target(resources_copy ALL
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${CMAKE_SOURCE_DIR}/resources
#         ${CMAKE_BINARY_DIR}/resources
#     DEPENDS ${RESOURCE_FILES_LIST}
#     COMMENT "Copying resources to build directory"
# )

# add_dependencies(${PROJECT_NAME} resources_copy)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
