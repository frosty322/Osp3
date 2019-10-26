#!/bin/bash
file_descriptions=`ls`

arg_uid=`getent passwd $1 | awk -F: '{print $3;}'`
arg_gid=`getent passwd $1 | awk -F: '{print $4;}'`

for file in $file_descriptions
do
        
        acces_mode=`ls -ln $file | awk '{print $1;}'`
        uid=`ls -ln $file | awk '{print $3;}'`
        gid=`ls -ln $file | awk '{print $4;}'`

        user_read=${acces_mode:2:1}
        group_read=${acces_mode:5:1}
        all_read=${acces_mode:8:1}

        if [[ "$uid" ==  "$arg_uid" ]] && [[ "$user_read" == "w" ]]; then
                echo $file
        elif [[ "$gid" == "$arg_uid" ]] && [[ "$group_read" == "w" ]]; then
                echo $file
        elif [[ "$all_read" == "w" ]]; then
                echo $file
        fi
done
