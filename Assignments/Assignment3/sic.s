section .data                    	; data section, read-write
        percent_ld:          db '%ld',0
        percent_ld_:         db '%ld ',0
        
section .rodata
        nl:     db 10,0

section .bss

section .text                    	; our code is always in the .text section
        global main
        extern printf            	; tell linker that printf is defined elsewhere
        extern scanf            	; tell linker that scanf is defined elsewhere
        extern calloc            	; tell linker that scanf is defined elsewhere
        extern free            		; tell linker that scanf is defined elsewhere
        extern putchar            	; tell linker that scanf is defined elsewhere

;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE STARTS HERE ;;;;;;;;;;;;;;;;

%macro new_line 0
      mov rdi, nl
      mov rax, 0
      call printf
%endmacro

%macro c_alloc 2
       mov rsi, %1
       mov rdi, %2
       call calloc
%endmacro

%macro scan_var 2
      mov rsi,%1
      mov rdi,%2
      mov eax,0
      call scanf
%endmacro

;###############################################################################

main:
        push rbp             ;int main(){
        mov rbp, rsp
        sub rsp, 8
        c_alloc 8,4096              ;    long *M = calloc(4096,8);
        mov r13, rax       ;r13 holds M place
        mov r15, 0              ;    int i = 0;
        mov r14, 0              ;    int j =0
        jmp .while                ;    while(scanf("%ld",&input) != -1){

        .in_while:
                mov rax, r14                ;        M[j] = input;
                lea rdx, [0+rax*8]
                mov rax, r13
                add rdx, rax
                mov rax, [rbp-8]
                mov [rdx], rax
                inc     r14                       ;        j++;

        .while:
                lea rax, [rbp-8]                ;    while(scanf("%ld",&input) != -1){
                scan_var rax,percent_ld
                cmp eax, -1
                jne .in_while
                jmp .while_operation_test                 ;    while (M[i] || M[i + 1] || M[i + 2]) {

        .do_operation:
                mov rax,r15            ;        M[M[i]] = M[M[i]] - M[M[i + 1]];
                lea rdx,[0+rax*8]
                mov rax,r13
                add rax,rdx
                mov rax,[rax]
                lea rdx,[0+rax*8]
                mov rax,r13
                add rax,rdx
                mov rcx,[rax]
                mov rax,r15
                add rax,1
                lea rdx,[0+rax*8]
                mov rax,r13
                add rax,rdx
                mov rax,[rax]
                lea rdx,[0+rax*8]
                mov rax,r13
                add rax,rdx
                mov rdx,[rax]
                mov rax,r15
                lea rsi,[0+rax*8]
                mov rax,r13
                add rax,rsi
                mov rax,[rax]
                lea rsi,[0+rax*8]
                mov rax,r13
                add rax,rsi
                sub rcx,rdx
                mov rdx,rcx
                mov [rax],rdx

                mov rax,r15                ;        if (M[M[i]] < 0){ i = M[i + 2];
                lea rdx,[0+rax*8]
                mov rax,r13
                add rax,rdx
                mov rax,[rax]
                lea rdx,[0+rax*8]
                mov rax,r13
                add rax,rdx
                mov rax,[rax]
                cmp rax,0
                jns .add3
                mov rax,r15
                add rax,2
                lea rdx,[0+rax*8]
                mov rax,r13
                add rax,rdx
                mov rax,[rax]
                mov r15,rax
                jmp .while_operation_test

        .add3:
                add r15, 3              ;        else {i += 3;

        .while_operation_test:
                mov rax,r15                ;    while (M[i] || M[i + 1] || M[i + 2]) {
                lea rdx,[0+rax*8]
                mov rax,r13
                add rax,rdx
                mov rax,[rax]
                cmp rax,0
                jne .do_operation
                mov rax,r15
                add rax,1
                lea rdx,[0+rax*8]
                mov rax,r13
                add rax,rdx
                mov rax,[rax]
                cmp rax,0
                jne .do_operation
                
                mov rax, r15
                add rax, 2
                lea rdx, [0+rax*8]
                mov rax, r13
                add rax, rdx
                mov rax, [rax]
                cmp rax, 0
                jne .do_operation

                mov r15, 0              ;    for (i = 0; i < 4096; ++i) {
                jmp .end_operation

        .print_nums:
                mov rax, r15            ;        printf("%ld ", M[i]);
                lea rdx, [0+rax*8]
                mov rax, r13
                add rax, rdx
                mov rax, [rax]
                mov rsi, rax
                mov rdi, percent_ld_
                mov eax, 0
                call printf

                add r15, 1              ;    for (i = 0; i < 4096; ++i) {
        .end_operation:
                cmp r15, 4095
                jng .print_nums

                new_line

		mov rdi,r13
		call free

        leave
        ret
