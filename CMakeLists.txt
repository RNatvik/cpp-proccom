#----------------------------------
# PROJECT DECLARATION
#----------------------------------
cmake_minimum_required(VERSION 3.17)
project(socket-service VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#----------------------------------
# External Libs
#----------------------------------
set(ExternalLib1_path F:/CLibs/asio-1.18.1)
add_library(asio 
    ${ExternalLib1_path}/include/asio.hpp
)
target_include_directories(asio PUBLIC ${ExternalLib1_path}/include)
set_target_properties(asio PROPERTIES LINKER_LANGUAGE CXX)

#target_compile_options(asio PRIVATE -D_WIN32_WINNT=0x0601)

#----------------------------------
# INSTALL PATHS
#----------------------------------
SET(ExportDir ${CMAKE_CURRENT_SOURCE_DIR}/install)
SET(ExportInclude ${ExportDir}/include)
SET(ExportLib ${ExportDir}/lib)
SET(CMAKE_INSTALL_PREFIX ${ExportDir})

SET(InstallInclude ${ExportInclude}/${PROJECT_NAME})
SET(InstallLib ${ExportLib}/${PROJECT_NAME})

#----------------------------------
# TARGET DEFINITION
#----------------------------------
set(LibName1 proccom)
set(LibName1_files
    include/prc-common.hpp
    include/prc-message.hpp
    include/prc-payload.hpp
    include/prc-node.hpp
    include/prc-broker.hpp
)

set(ExeName1 main)
set(ExeName1_libs pthread asio ${LibName1} ws2_32)
set(ExeName1_files src/main.cpp)

set(ExeName2 test)
set(ExeName2_libs pthread asio ${LibName1} ws2_32)
set(ExeName2_files src/test.cpp)

#----------------------------------
# INSTALL TARGETS
#----------------------------------

SET(InstallTarget1 ${LibName1})
SET(InstallFiles1 ${LibName1_files})


#----------------------------------
# LIBRARY DECLARATION
#----------------------------------
add_library(${LibName1} ${LibName1_files})
set_target_properties(${LibName1} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(
   ${LibName1} INTERFACE
   $<INSTALL_INTERFACE:include>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
#target_compile_options(${LibName1} PRIVATE -Werror -Wall -Wextra)


#----------------------------------
# EXECUTABLE DECLARATION
#----------------------------------
add_executable(${ExeName1} ${ExeName1_files})
target_link_libraries(${ExeName1} ${ExeName1_libs})
#target_compile_options(${ExeName1} PRIVATE -Werror -Wextra -Os)

add_executable(${ExeName2} ${ExeName2_files})
target_link_libraries(${ExeName2} ${ExeName2_libs})
#target_compile_options(${ExeName2} PRIVATE -Werror -Wextra -Os)

#----------------------------------
# INSTALL DECLARATION
#----------------------------------

install(
    FILES ${InstallFiles1}
    DESTINATION ${InstallInclude}
)

install(
    TARGETS ${InstallTarget1}
    EXPORT export_${PROJECT_NAME}
    DESTINATION ${InstallLib}
)

install(
    EXPORT export_${PROJECT_NAME}
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION ${InstallLib}
)


