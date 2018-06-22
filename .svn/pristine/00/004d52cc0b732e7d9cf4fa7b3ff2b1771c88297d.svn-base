#!/bin/sh

makefile=Makefile.files
cppfile=cpp.list
ls -ort *.cpp|grep -v AI|awk '{print $8}' |awk -F.  '{print $1}' > $cppfile
ls -ort */*.cpp|grep -v AI|awk '{print $8}' |awk -F.  '{print $1}' >> $cppfile

cat /dev/null > $makefile

echo "" >> $makefile
echo "" >> $makefile
echo "" >> $makefile

echo "MODULE_LIBS =   \\" >> $makefile
cat $cppfile|while read line
do
	file=`echo $line|awk -F	\/ '{print $2}'`
	if [ "$file.ok" == ".ok" ]; then
        	echo "          ./libs/$line.o\\" >> $makefile
	else
        	echo "          ./libs/$file.o\\" >> $makefile
	fi

done


echo "" >> $makefile
echo "" >> $makefile
echo "" >> $makefile



echo "MODULE_SRC =     \\" >> $makefile
cat $cppfile|while read line
do
        echo "          ./$line.o \\" >> $makefile
        #echo $line
done



