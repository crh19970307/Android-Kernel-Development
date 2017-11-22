Project1

FILE SYSTEM

pro1/					--folder of problem1
	Makefile			--makefile of problem1
	ptree.c				--source code of syscall ptree module

pro2/					--folder of problem2
	jni/				--folder of jni
		Android.mk 		--makefile
		test_ptree.c 	--source code of test program of ptree

pro3/					--folder of problem3
	jni/ 				--folder of jni
		Android.mk 		--makefile
		parent_child.c 	--source code of create parent and child process,i.e. problem3

pro4/					--folder of problem4
	jni/				--folder of jni
		Android.mk 		--makefile
		BurgerBuddies.c --source code of BurgerBuddies problem

output/					--folder of output
	output.txt			--the output of all programs for correct input and error input

Prj1README				--readme file of project

NOTE THAT YOU SHOULD ADB PUSH THE EXECUTEBLE FILE INTO /data/misc FOLDER OTHERWISE THE PARENT_CHILD MAY NOT RUN SUCCESSFULY .
