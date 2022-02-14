#!/bin/bash
dir_name=`realpath $1`
min_size=$2
max_size=$3
file_name=$4
if [ -d $dir_name ]
then
	find $dir_name -type f -size +${min_size}\c -size -${max_size}\c -printf '%h %f %sB\n'  > $file_name 
	echo -n 'Files analyzed: ' 
	find $dir_name -type f 2> /dev/null | wc -l
else
	{ echo Provided dir is incorrect; ls $dir_name; } >&2
fi

