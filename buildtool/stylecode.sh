#! /bin/bash  
#批量格式化，除libopencm3文件夹之外的文件

for f in $(find ./ -name '*.c' -or -name '*.cpp' -or -name '*.h' -type f)  
do  
	if [ -f "$f" ]; then 
    	astyle --options=../buildtool/astyle.conf --preserve-date $f  
	fi 
done  

# 删除.orig文件  
for f in $(find ./ -name '*.orig' -type f)  
do  
     rm $f  
done  

