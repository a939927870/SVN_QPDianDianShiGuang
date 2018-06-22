# CMakeLists file

cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -D_LINUX -DGAME_MODULE -fPIC -m64 -Wenum-compare -std=c++11 -lstdc++ -Istdc++")
SET(CMAKE_CXX_FLAGS_RELEASE "-g -ggdb -O2 -D_LINUX -DGAME_MODULE -fPIC -m64 -Wenum-compare -std=c++11 -lstdc++ -Istdc++")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--hash-style=sysv")

SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/app_release/linuxlib")

include_directories ("${PROJECT_SOURCE_DIR}/"  "${PROJECT_SOURCE_DIR}/../include"   "/opt/include" "/opt/app/openssl/include" "/opt/app/curl/include"  "/opt/app/msgpack/include"  "/opt/app/mysql/include"  "/opt/app/boost/include"  "/opt/app/boost/include" "/opt/app/libiconv/include" "${PROJECT_SOURCE_DIR}/LBase" "${PROJECT_SOURCE_DIR}/extlib/include"  "/usr/local/include" "/usr/include")

link_directories ("${PROJECT_SOURCE_DIR}/app_release/linuxlib" "/usr/lib64" "/usr/lib" "/opt/app/curl/lib"  "/opt/app/openssl/lib"  "/opt/app/msgpack/lib"  "/opt/app/mysql/lib" "/opt/app/libiconv/lib/"  "/opt/app/boost/lib" "/opt/app/lua/lib" "/usr/local/lib" "${PROJECT_SOURCE_DIR}/../app/boost/lib")

########################so 部分 #####################
add_subdirectory (extlib)
add_subdirectory (LBase)

#########################内部代码部分 ################
add_subdirectory (CenterServer)
add_subdirectory (LoginGate)
add_subdirectory (LogicDB)
add_subdirectory (LogicManager)
add_subdirectory (DZLogicServer)
add_subdirectory (CSLogicServer)
add_subdirectory (GDLogicServer)
add_subdirectory (GateServer)
add_subdirectory (TeamServer)

add_executable (DDCenterServer CenterServer/main.cpp)
add_executable (DDLoginServer LoginGate/main.cpp)
add_executable (DDLogicDB LogicDB/main.cpp)
add_executable (DDLogicManager LogicManager/main.cpp)
add_executable (DDDZLogicServer DZLogicServer/main.cpp)
add_executable (DDCSLogicServer CSLogicServer/main.cpp)
add_executable (DDGDLogicServer GDLogicServer/main.cpp)
add_executable (DDGateServer GateServer/main.cpp)
add_executable (DDTeamServer TeamServer/main.cpp)

target_link_libraries (DDCenterServer CenterServer LBase pthread iconv dl boost_serialization  boost_thread   boost_system boost_regex boost_filesystem curl mysqlclient)

target_link_libraries (DDLoginServer LoginGate LBase pthread iconv dl boost_serialization  boost_thread   boost_system  boost_regex  boost_filesystem curl mysqlclient)

target_link_libraries (DDLogicDB LogicDB LBase pthread iconv dl boost_serialization  boost_thread   boost_system boost_regex boost_filesystem curl mysqlclient) 

target_link_libraries (DDLogicManager LogicManager LBase pthread iconv dl boost_serialization  boost_thread   boost_system boost_regex boost_filesystem curl mysqlclient)

target_link_libraries (DDDZLogicServer DZLogicServer LBase pthread iconv dl boost_serialization  boost_thread   boost_system boost_regex boost_filesystem curl mysqlclient)

target_link_libraries (DDCSLogicServer CSLogicServer LBase pthread iconv dl boost_serialization  boost_thread   boost_system boost_regex boost_filesystem curl mysqlclient)

target_link_libraries (DDGDLogicServer GDLogicServer LBase pthread iconv dl boost_serialization  boost_thread   boost_system boost_regex boost_filesystem curl mysqlclient)

target_link_libraries (DDGateServer GateServer LBase pthread iconv dl boost_serialization  boost_thread   boost_system boost_regex boost_filesystem curl mysqlclient)

target_link_libraries (DDTeamServer TeamServer LBase pthread iconv dl boost_serialization  boost_thread   boost_system boost_regex boost_filesystem curl mysqlclient)
