default rel

extern GetStdHandle
extern WriteFile
extern ExitProcess

section .text
global start
start:
    sub     rsp, 40          ; reserve shadow space and align stack by 16

    mov     rcx, -11         
    call    GetStdHandle

    mov     rcx, rax         ; HANDLE is a 64-byte type on x64
    lea     rdx, [msg]       ; lpBuffer = RIP-relative LEA (default rel)
    mov     r8d, msg.len     ; DWORD nNumberOfBytesToWrite = 32-bit immediate constant length
    lea     r9, [rsp+48]     ; lpNumberOfBytesWritten pointing into main's shadow space
    mov     qword [rsp + 32], 0   ; lpOverlapped = NULL; This is a pointer, needs to be qword.
    call    WriteFile        ; WriteFile(handle, msg, len, &our_shadow_space, NULL)

;;; BOOL return value in EAX (BOOL is a 4-byte type, unlike bool).
;;; NumberOfBytesWritten in dword [rsp+48]

    xor     ecx, ecx
    call ExitProcess
  ; add     rsp, 40      ; alternative to calling a noreturn function like ExitProcess
  ; ret


section .data         ; or section .rdata to put it in a read-only page
    msg:  db "This is a test to see if this works", 13, 10    ; including a CR LF newline is a good idea for a line of output text
    .len equ  $-msg    ; let the assembler calculate the string length
                       ; .len is a local label that appends to the most recent non-dot label, so this is msg.len


;linking command for this program
;"C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.29.30133\bin\Hostx86\x64\link.exe" "C:\Users\colec\C Programs\CompilerTest\assembly\hello.obj" "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\um\x64\kernel32.Lib" /entry:start /subsystem:console /nodefaultlib /out:"C:\Users\colec\C Programs\CompilerTest\assembly\hello.exe"