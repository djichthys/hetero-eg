#!/bin/bash

license_file=""
source_type=""
dir=$(pwd)

function parse_opts
{
    OPTIND=1
    echo "options = " $* 
    while getopts ":chd:f:" options ; 
    do
        case $options in
            f) license_file="$OPTARG" ;;
            c) source_type="[ch]" ; lang="c" ;;
            d) dir="$OPTARG" ;;
            h) echo "add license text to the top of source" ;
                 echo "   options :";
                 echo "   -f ... license file to read text from" ;
                 echo "   -d ... directory to recursively search" ;
                 echo "   -c ... find and add license to .c and .h files" ;
                 exit ;;
            :) echo "option -$OPTARG requires argument" ;;
            ?) echo "invalid option -$OPTARG" ;; 
            *)  echo "Processing error in getopts" ;;
        esac
    done
}


function add_license
{
    for file in $(find $dir -name "*.$source_type");
    do
       if ! grep -iq Copyright $file ; 
       then 
           cat ${license_file} ${file} > ${file}.new && mv ${file}.new ${file} 
       fi
    done
}


parse_opts $*

echo "license file = $license_file" 
echo "dir = $dir" 
echo "lang = $source_type" 

add_license
