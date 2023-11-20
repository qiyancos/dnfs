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
  if [ $mode_flag -eq 0 ];
  then
     print "Test ls,the file list will not be displayed unless the number of ls is set to 1"
     read -p "Please enter the number of ls tests: " test_num
  else
	  print "Test ls 50 times"
	  test_num=50
  fi

  for(( i=1; i<=$test_num; i++ ))
  do
	  print "    the $i times ls test"
	  ls >> /dev/null
	  if [ $? -ne 0 ];then
		  print "Test ls failed" $split_str
		  exit
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
   if [ $mode_flag -eq 0 ];
   then
           read -p "Please enter the name of the sequence file to create: " file_name
           read -p "Please enter the number of the sequence file to create: " file_number
   else
	   print "Will create 100 files"
	   file_name="test"
	   file_number=100
   fi
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
   ls -lh | grep $file_name
   print $split_str
}

#test mkdir
function test_mkdir(){
   print "Test make dir"
   if [ $mode_flag -eq 0 ];
   then
           read -p "Please enter the name of the sequence dir to create: " dir_name
           read -p "Please enter the number of the sequence dir to create: " dir_number
   else
	   print "Will create 100 directory"
	   dir_name="test_dir"
	   dir_number=100
   fi
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
   ls -lh | grep $dir_name
   print $split_str

}

#test remove
function test_remove(){
   print "Test remove"
   if [ $mode_flag -eq 0 ];
   then
           read -p "Please choose to delete individually or delete all,input 1 means all ,other means select one to delete: " delete_mode
   else
	   print "Will delete all files in ${mount_path[0]}"
	   delete_mode=1
   fi
   if [ $delete_mode -eq 1 ];
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

#test soft link
function test_soft_link(){
	print "Test soft link"
	if [ $mode_flag -eq 0 ];
	then
		read -p "Please enter the soft link source path: " src_path
		read -p "Please enter the soft link target name: " target_path
		read -p "Please enter the number of soft links to create: " link_num
	else
		print "Will create 50 soft link on test_dir1"
		src_path=test_dir1
		target_path=soft_l
		link_num=50
	fi
	if [ -e $src_path ];
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
		print "Link source path $src_path not exist" $split_str
		return
	fi
	print "Successfully tested soft link the result is:"
	ls -lh | grep $target_path
	print $split_str
}

#test hard link
function test_hard_link(){
        print "Test hard link"
        if [ $mode_flag -eq 0 ];
        then
                read -p "Please enter the hard link source path,directory not allowed to set hard link: " src_path
                read -p "Please enter the hard link target name: " target_path
                read -p "Please enter the number of hard links to create: " link_num
        else
                print "Will create 50 hard link on test_dir1"
                src_path=test2
                target_path=hard_l
                link_num=50
	fi
        if [ -e $src_path ];
        then
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
        else
                print "Link source path $src_path not exist" $split_str
                return
        fi
        print "Successfully tested hard link the result is:"
        ls -lh | grep $target_path
        print $split_str
}

#test mv
function test_mv(){
	print "Test mv"
	if [ $mode_flag -eq 0 ];
	then
		read -p "Please enter the mv source path: " src_path
		read -p "Please enter the mv target name: " target_path
	else
		print "Will mv test_dir3 to test_mv"
		src_path="test_dir3"
		target_path="test_mv"
	fi
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
		print "Mv source path $src_path not exist" $split_str
		return
	fi 
	print "Successfully tested cp the result is:"
	ls | grep $target_path
	print $split_str
}

#test cp
function test_cp(){
        print "Test cp,the target path will be copied directly to the current path"
	if [ $mode_flag -eq 0 ];
	then
		read -p "Please enter the cp source path: " src_path
		read -p "Please enter the cp target path: " target_path
	else
		print "Will cp test_dir4 to test_cp"
                src_path="test_dir4"
                target_path="test_cp"
	fi		
       	if [ -e $src_path ];
        then
                print "Cp $src_path to `pwd`"
                cp -rf  $src_path $target_path
                if [ $? -ne 0 ];
                then
                        print "Failed to cp $src_path to $target_path" $split_str
                        exit
                fi
        else
                print "Cp source path $src_path not exist" $split_str
		return
        fi
        print "Successfully tested cp the result is:"
        ls -lh
        print $split_str
}


#test vim 
function test_vim(){
	print "Test vim"
	read -p "Please enter the vim file name: " src_path
	print "Vim $src_path"
        vi $src_path
	if [ $? -ne 0 ];
	then
		print"Failed to vim $src_path" $split_str
		exit
	fi
	print "Successfully tested vim"
	ls -lh | grep $src_path
        print $split_str
}

#test chmod
function test_chmod(){
	print "Test chmod"
	ls
	if [ $mode_flag -eq 0 ];
	then
		read -p "Please select the file name to be chmod: " src_path
		read -p "Please enter the permission to be change: " change_mode
	else
		print "Will set file test1 mode 777"
		src_path="test1"
		change_mode=777
	fi
	if [ -e $src_path ];
	then
		print "Change $src_path permission to $change_mode"
		chmod $change_mode $src_path
		if [ $? -ne 0 ];
		then
			print "Failed to change $src_path permission to $change_mode" $split_str
			exit
		fi
	else
		print "Chmod file $src_path not exit" $split_str
		return
	fi
	print "Successfully tested chmod the result is:"
	ls -lh | grep $src_path
	print $split_str

}

#concurrency test
function test_concurrency(){
	print "Concurrency test"
	test_file="concurrency.txt"
	if [ ${#mount_path[*]} -ge 2 ];
	then
		for (( i=0; i<${#mount_path[*]}; i++))
		do
			test_cd ${mount_path[$1]}
			if [ -e $test_file ];
			then
				print "Test file is $test_file"
			else
                                print "Create test file $test_file"
				touch $test_file
				if [ $? -ne 0 ];
				then
					print "Failed to create test file $test_file" $split_str
					test_cd ${mount_path[0]}
					exit
				fi
			fi
			{
				print "Three thousand data will be written to ${mount_path[$i]}/$test_file"
				for (( j=1; j<3000; i++ ))
				do
					echo $j >> "concurrency.txt"
					if [ $? -ne 0 ];
					then
						print "Failed to write data to ${mount_path[$i]}/$test_file"
						test_cd ${mount_path[0]}
						exit
					fi
			 	done
			}&
		done

		wait

	else
		print "Concurrency test requires at least two mounting paths for testing" $split_str
	        return
	fi	
	print "Successfully tested concurrency"
	test_cd ${mount_path[0]}
	print $split_str
}

print "Start test nfs"
print "!!!!!!!Any method test will exit when an error is encountered!!!!!!" $split_str

#judge arg num must gt 2 the first is test mode manual or auto next are mount paths
if [ $# -ge 2 ];
then   
        #first get mount path
        index=-1
	mount_path=$@
        for i in $@
	do
		if [ $index -eq -1 ];
		then
			let index++
			continue
		fi
		mount_path[$index]=$i
		let index++
	done
	
	#judge path exist
	for i in ${mount_path[@]}
	do
		if [ -e $i ];
		then
			continue
		else
			print "Mount path $i not exist" $split_str
		fi
	done

	mode_flag=$1

	if [ $mode_flag -eq 0 ];
	then
		print "Select manual test mode" $split_str
                test_cd ${mount_path[0]}


                while [ 1 -eq 1 ]
		do
			print "Please select an action from order list\n
			       1 ls\n
			       2 touch file\n
			       3 make dir\n
			       4 remove\n
			       5 soft_link\n
			       6 hard_link\n
			       7 mv\n
			       8 cp\n
			       9 vim\n
			       10 chmod\n
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
				#test soft link
				test_soft_link
				;;
			"6")
				#test hard link
				test_hard_link
				;;
			"7")
				#test mv
				test_mv
				;;
			"8")    
				#test cp
				test_cp
				;;
			"9")    
				#test vim
				test_vim
				;;
			"10")   
				#test chmod
				test_chmod
				;;
			*)
				print "Exit ,thank you" $split_str
				exit
			esac
		done
	else
		print "Select automatic test mode" $split_str
		test_cd ${mount_path[0]}
	
		#first clear dir
		test_remove

		#create file
		test_touch

		#create dir
		test_mkdir

		#test ls
		test_ls

		#soft link
		test_soft_link

		#hard_link
		test_hard_link

		#test mv
		test_mv

		#test cp
		test_cp

		#test chmod
		test_chmod

		#test rm
		test_remove

	fi

else
	print "Please enter test mode and at least one mount path" $split_str
fi
	
