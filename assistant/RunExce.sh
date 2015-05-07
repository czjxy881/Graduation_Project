#!/bin/bash
aim=$@
BaseName=$PWD"/dataResult/ExceResult"_${aim// /_}
fileName=$BaseName
i=1
while [ -f "${fileName}.txt" ];do
	fileName=$BaseName'-'$i
	i=$(($i+1))
done
fileName=${fileName}.txt
echo $fileName
x=(1000 2000 4000 8000 16000)
echo 'x '${x[@]}>$fileName
echo 'x '${x[@]}

cd ../../graphgrep/graphgrep1.1/
for lp in $aim; do
	code="sed -i '' 's/LengthPath= 4/LengthPath= ${lp}/' ./graphbuild.c"
	codere="sed -i '' 's/LengthPath= ${lp}/LengthPath= 4/' ./graphbuild.c"
	echo -n 'lp'$lp' '>>$fileName
	cd src
 	echo $code|sh
 	make
 	echo $codere|sh
 	cd ..
 	echo $PWD
 	for size in ${x[@]}; do
 		./graphbuild ../../Graduation_Project/assistant/data/AIDO99SD-$size
 		s=$(./graphgrep -m ../../Graduation_Project/assistant/data/query|tail -n 1|cut -c6-)
 		echo -n $s' '>>$fileName
 		rm -fr graphgrepdata
 	done
 	echo ''>>$fileName
done 
cd src
 	make
