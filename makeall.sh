echo "start to build"

echo "building XLCenterServer"

cd ./CenterServer
sh make.sh
make

cd ..
echo "building XLLoginGate"
cd ./LoginGate
sh make.sh
make

cd ..
echo "building XLLogicDB"
cd ./LogicDB
sh make.sh
make

cd ..
echo "building XLLogicManager"
cd ./LogicManager
sh make.sh
make

cd ..
echo "building XLLogicServer"
cd ./LogicServer
sh make.sh
make

cd ..
echo "building XLGateServer"
cd ./GateServer
sh make.sh
make

echo "building finished!!!"

cd ..

echo "please check creation date of all executable files"

ls -R -l|grep XL

