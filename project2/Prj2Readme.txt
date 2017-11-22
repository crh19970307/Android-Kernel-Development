Project2

FILE SYSTEM

get_pagetable_layout/	--folder of syscall get_pagetable_layout
	Makefile							--makefile 
	get_pagetable_layout.c				--source code 

expose_page_table/					--folder of syscall expose_page_table
	Makefile							--makefile 
	get_pagetable_layout.c				--source code 

VAtranslate/					--folder of program VAtranslate
	jni/ 				--folder of jni
		Android.mk 		--makefile
		VAtranslate.c 	--source code 

vm_inspector/					--folder of program vm_inspector
	jni/				--folder of jni
		Android.mk 		--makefile
		vm_inspector.c --source code 

what_I_find_through_vminspector/		--folder of pagetable-logs and maps of different processes
	logs/			--the pagetable of a process at different times
		log1.txt	--the pagetable of a process at time1
		log2.txt	--the pagetable of a process at time2
		log3.txt	--the pagetable of a process at time3
	maps/			--the file of /proc/pid/maps for different processes
		deskclock-maps		--the file of /proc/pid/maps for deskclock		
		music-maps			--the file of /proc/pid/maps for music
		zygote-maps			--the file of /proc/pid/maps for zygote
	pagetable/		--the pagetables of different processes
		deskclock.txt		--the pagetable of deskclock
		music.txt			--the pagetable of music
		zygote.txt			--the pagetable of zygote

Change_linux_replacement_algorithm/		--folder of changed files in kernel to change page replacement algorithm
	mm_types.h
	swap.c
	vmscan.c

Prj2README				--readme file of project

report.pdf				--my report for this project