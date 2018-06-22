#!/bin/sh

##################################动态库部分###########################################
#build extlib
cd extlib/
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
#echo "add_library(extlib SHARED ">>CMakeLists.txt
echo "add_library(extlib ">>CMakeLists.txt
ls src/lib_json/*.cpp>>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../

#build LBase
cd LBase/
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
#echo "add_library(LBase SHARED">>CMakeLists.txt
echo "add_library(LBase ">>CMakeLists.txt
ls *.cpp | grep -v MiniDump.cpp >>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../

#########################工程内部静态库部分##############################
#build CenterServer
cd CenterServer
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(CenterServer  ">>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../

#build LoginGate
cd LoginGate
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(LoginGate ">>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../

#build LogicDB
cd LogicDB
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(LogicDB  ">>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../

#build LogicManager
cd LogicManager
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(LogicManager  ">>CMakeLists.txt
ls PlayerTask/*.cpp  | grep -v AI.cpp>>CMakeLists.txt
ls *.cpp  | grep -v AI.cpp>>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../

#build DZLogicServer
cd DZLogicServer
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(DZLogicServer  ">>CMakeLists.txt
ls *.cpp  | grep -v AI.cpp>>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../

#build CSLogicServer
cd CSLogicServer
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(CSLogicServer  ">>CMakeLists.txt
ls *.cpp  | grep -v AI.cpp>>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../

#build GDLogicServer
cd GDLogicServer
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(GDLogicServer  ">>CMakeLists.txt
ls *.cpp  | grep -v AI.cpp>>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../

#build GateServer
cd GateServer
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(GateServer  ">>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../

#build TeamServer
cd TeamServer
rm -f CMakeLists.txt

echo "# CMakeLists file">>CMakeLists.txt
echo "">>CMakeLists.txt
echo "add_library(TeamServer  ">>CMakeLists.txt
ls *.cpp>>CMakeLists.txt
echo ")">>CMakeLists.txt
cd ../




