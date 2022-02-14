#!/bin/bash
source_file=$1
compiled_file=$2
if [ -f $source_file ]
then
	gcc $source_file \-o $compiled_file
	
	if [ -s $compiled_file ]
	then
		chmod +x $compiled_file
		./$compiled_file
	fi
else
	echo File doesn\'t exist.
fi
