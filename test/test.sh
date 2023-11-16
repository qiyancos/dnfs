#!/bin/bash

#build split str
split_str="--------------------------------------------------------\n"


# fromat output
function print() {
  if [ $# -eq 1 ];then
     echo -e "$1"
  elif [ $# -eq 2 ];then
     echo -e "$1 \n$2"
  fi
}

# test cd
function test_cd() {
  cd $1
  if [ $? -eq 0 ];then
	  print "Enter path $1" $split_str
  else
	  print "Failed to enter path $1" $split_str
	  exit
  fi
}

# test ls
function test_ls() {
  print "Test ls,the file list will not be displayed unless the number of ls is set to 1"
  read -p "Please enter the number of ls tests: " test_num
  for(( i=1; i<=$test_num; i++ ))
  do
	  print "    the $i times ls test"
	  ls >> /dev/null
	  if [ $? -ne 0 ];then
		  print "Test ls failed" $split_str
	  fi
  done

  if [ $test_num -eq 1 ];
  then
	  ls -lh
  fi
  print "Successfully tested ls" $split_str
}

#test touch 
function test_touch() {
   print "Test touch file"
   read -p "Please enter the name of the sequence file to create: " file_name
   read -p "Please enter the number of the sequence file to create: " file_number
   for(( i=1; i<=$file_number; i++ ))
   do
	   print "    Touch file  $file_name$i"
	   touch $file_name$i
	   if [ $? -ne 0 ];then
		   print "Touch file $file_name$i failed" $split_str
		   exit
	   fi
   done
   print "Successfully tested touch the result is:" 
   ls -lh
   print $split_str
}

#test mkdir
function test_mkdir(){
   print "Test make dir"
   read -p "Please enter the name of the sequence dir to create: " dir_name
   read -p "Please enter the number of the sequence dir to create: " dir_number
   for(( i=1; i<=$dir_number; i++ ))
   do
           print "    Make dir  $dir_name$i"
           mkdir $dir_name$i
           if [ $? -ne 0 ];then
                   print "Make dir $dir_name$i failed" $split_str
                   exit
           fi
   done
   print "Successfully tested make dir the result is:" 
   ls -lh
   print $split_str

}

#test remove
function test_remove(){
   print "Test remove"
   read -p "Please choose to delete individually or delete all,input 1 means all ,other means select one to delete: " delete_mode
   if [ $delete_mode -eq "1" ];
   then
	   print "All files listed below will be deleted"
	   ls
	   rm -rf *
	   if [ $? -ne 0 ];then
                   print "Failed to remove all files" $split_str
                   exit
           fi
	   print "Successfully deleted all files"
   else
	   continue_do=1
	   while [ $continue_do -eq 1 ]
	   do
	           print "The file list in this directory is:"
                   ls
                   read -p "Please enter the deleted file name: " delete_name
		   print "Delete file is $delete_name"
	           
		   if [ -e $delete_name ];
	           then
	                   rm -rf  $delete_name
                           if [ $? -ne 0 ];
			   then
                                   print "Failed to remove $delete_name" $split_str
                                   exit
                           fi
	           else
		           print "Select file $delete_name not exist"
	     	   fi

	           read -p "Do you want to continue deleting files,input 1 to contine,other break: " continue_do 
	   done

   fi
   print "Successfully tested rm the result is:"
   ls
   print $split_str

}

#test link
function test_link(){
	print "Test link"
	read -p "Please choose to create a soft link or a hard link,input 1 means soft link, other means hard link: " link_mode
	read -p "Please enter the link source path: " src_path
	read -p "Please enter the link target name: " target_path
	read -p "Please enter the number of links to create: " link_num
	if [ -e $src_path ];
	then
		if [ $link_mode -eq 1 ];
		then
			for (( i=1; i<=$link_num; i++ ))
			do
				print "Create soft link $target_path$i"
               		        ln -sf $src_path $target_path$i
				if [ $? -ne 0 ];
				then
					print "Failed to create soft link $target_path$i" $split_str
					exit
				fi
			done
		else
			for (( i=1; i<=$link_num; i++ ))
			do
				print "Create hard link $target_path$i"
				ln -f $src_path $target_path$i
				if [ $? -ne 0 ];
				then
					print "Failed to create hard link $target_path$i" $split_str
					exit
				fi
			done
		fi
	else
		print "Link source path $src_path not exist"
	fi
	print "Successfully tested link the result is:"
	ls -lh
	print $split_str
}

#test mv
function test_mv(){
	print "Test mv"
        read -p "Please enter the mv source path: " src_path
        read -p "Please enter the mv target name: " target_path
	if [ -e $src_path ];
	then
		print "Mv $src_path to $target_path"
		mv -f  $src_path $target_path
                if [ $? -ne 0 ];
		then
			print "Failed to mv $src_path to $target_path" $split_str
			exit
		fi
	else
		print "Mv source path $src_path not exist"
	fi
	read -p "Please enter the cp source path: " src_path
	ls
	print $split_str
}

#test cp
function test_cp(){
        print "Test cp,the source path will be copied directly to the current path"
        read -p "Please enter the cp source path: " src_path
       	if [ -e $src_path ];
        then
                print "Cp $src_path to `pwd`"
                cp -rf  $src_path ./
                if [ $? -ne 0 ];
                then
                        print "Failed to cp $src_path to $pwd_path" $split_str
                        exit
                fi
        else
                print "Cp source path $src_path not exist"
        fi
        print "Successfully tested cp the result is:"
        ls -lh
        print $split_str
}


#test vim 
function test_vim(){
	print "Test vim"
	read -p "Please enter the vim file path: " src_path
	print "Vim $src_path"
        vi $src_path
	if [ $? -ne 0 ];
	then
		print"Failed to vim $src_path" $split_str
		exit
	fi
	print "Successfully tested vim"
        print $split_str
}

print "Start test nfs"
print "!!!!!!!Any method test will exit when an error is encountered!!!!!!" $split_str

#judge arg num must gt 0
if [ $# -gt 0 ];
then
        print "Mount path is $1" $split_str

	#judge dectory
        print "Determine whether the mounting directory exists"
	if [ -d $1 ];
	then
		print "Mount path $1 exist" $split_str

		#enter mount path
                test_cd $1


                while [ 1 -eq 1 ]
		do
			print "Please select an action from order list\n
			       1 ls\n
			       2 touch file\n
			       3 make dir\n
			       4 remove\n
			       5 link\n
			       6 mv\n
			       7 cp\n
			       8 vim\n
			       other exit\n";
			read -p "You select is: " select
			case $select in "1")
				#test ls
                                test_ls
				;;
			"2")
				#test touch file
                                test_touch
				;;
                        "3")
			        #test mkdir
                                test_mkdir
			        ;;
		        "4")
                                #test remove
                                test_remove
                                ;;
			"5")    
				#test link
				test_link
				;;
			"6")
				#test mv
				test_mv
				;;
			"7")    
				#test cp
				test_cp
				;;
			"8")    
				#test vim
				test_vim
				;;
			*)
				print "Exit ,thank you" $split_str
				exit
			esac
		done
	else
		print "Mount path $1 not exist" $split_str
		
	fi
else
	print "Need nfs mount path" $split_str
fi
	
