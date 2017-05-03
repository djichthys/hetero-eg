#!/bin/bash

license_file=""
source_type=""
dir=$(pwd)
prog="foobar"

function parse_opts
{
    OPTIND=1
    echo "options = " $* 
    while getopts ":chd:f:l:" options ; 
    do
        case $options in
            f) license_file="$OPTARG" ;;
            c) source_type="[ch]" ; lang="c" ;;
            d) dir="$OPTARG" ;;
            l) prog="$OPTARG" ;;
            h) echo "add license text to the top of source" ;
                 echo "   options :";
                 echo "   -f ... license file to read text from" ;
                 echo "   -d ... directory to recursively search" ;
                 echo "   -c ... find and add license to .c and .h files" ;
                 echo "   -l ... name of program" 
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
           echo "adding license in ${license_file} ==> ${file}"
           cat ${license_file} ${file} > ${file}.new && mv ${file}.new ${file} 
           sed -i -e "s/Foobar/$prog/g" $file 
       fi
    done
}


parse_opts $*

echo "license file = $license_file" 
echo "dir = $dir" 
echo "lang = $source_type" 

add_license
