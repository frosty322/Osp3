#!/bin/bash
print_all_users()
{
        cat /etc/passwd | awk -F: '{print $1}' 
}

print_group_users()
{
        cat /etc/passwd | grep $1 | awk -F: '{print $1}'
}

print_user()
{
        ls -l $1 | awk '{print $3;}'
}

file_description=`ls -ln $1`
acces_mode=`echo $file_description | awk '{print $1;}'`
uid=`echo $file_description | awk '{print $3;}'`
gid=`echo $file_description | awk '{print $4;}'`

user_read=${acces_mode:2:1}
group_read=${acces_mode:5:1}
all_read=${acces_mode:8:1}

if [[ "$all_read" ==  "w" ]]; then
        print_all_users
elif [[ "$group_read" == "w" ]]; then
        print_group_users $gid
elif [[ "$user_read" == "w" ]]; then
        print_user $1
fi
