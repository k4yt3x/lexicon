section .data

section .text
    global _start

_start:
    ; Push "Hello, World!\n" onto the stack
    mov rax, 0x0a21646c726f57    ; "\n!dlroW"
    push rax
    mov rax, 0x202c6f6c6c6548    ; " ,olleH"
    push rax

    ; Write system call
    mov rax, 1   ; sys_write
    mov rdi, 1   ; stdout
    mov rsi, rsp ; pointer to string on stack
    mov rdx, 14  ; length of string
    syscall

    ; Exit system call
    mov rax, 60         ; sys_exit
    mov rdi, 0          ; exit status
    syscall
