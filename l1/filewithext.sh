#!/bin/bash
if [[ $# -eq 3 ]]
then
	file_name=$1
	dir_name=$2
	extension=$3
	if [ -d $dir_name ]
	then
		
		find -name "*.sh" > $file_name
	else
		echo Provided dir is incorrect >&2
		ls $dir_name >&2
	fi
else
	echo Not enough parameters. 
	echo Required format:
	echo ./filewithext.sh /path/to/output_file /path/to/analyzed/dir extension 
fi
