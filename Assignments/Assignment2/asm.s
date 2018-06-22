section .data                    	; data section, read-write
        percent_s:          db '%s',0
        percent_c:          db '%c',0
        percent_lf:         db '%lf',0
        percent_Lf:         db '%Lf',0
        percent_e:          db '%e',0
        percent_d:          db '%d',0
        getE:               db '%.*e ',0
        getEWithNewLine:    db "%.*e ",0xa

section .rodata
        ZERO:   dq 48,0
        ONE:    dq 49,0
        root:   db 'root = ',0
        error_msg:   db 'Error: dividing by zero.',0
        epsi:   db 'epsilon',0
        nl:     db 10,0
        space:  db 32,0
        initi: db 'initial',0

section .bss
        epsilon:    resb 8
        order:      resb 8
        coeff:      resb 8
        initial:    resb 8
        code:       resb 8
        index:      resb 8
        buffer:      resb 1

section .text                    	; our code is always in the .text section
        global add_numbers          ; makes the function appear in global scope
        global mult_numbers           ; makes the function appear in global scope
        global div_numbers
        global sub_numbers          ; makes the function appear in global scope
        global print_complex_num
        global Ax_plus_B
        global derivative
        global evaluate
        global getInput
        global main
        global process
        global get_input2
        extern printf            	; tell linker that printf is defined elsewhere
        extern fprintf            	; tell linker that printf is defined elsewhere
        extern scanf            	; tell linker that scanf is defined elsewhere
        extern calloc            	; tell linker that scanf is defined elsewhere

;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE STARTS HERE ;;;;;;;;;;;;;;;;

%macro for_s 0
      mov rsi,buffer
      mov rdi, percent_s
      call scanf
%endmacro

%macro new_line 0
      mov     rdi, nl
      mov     rax, 0
      call    printf
%endmacro

%macro new_space 0
      mov     rdi, space
      mov     rax, 0
      call    printf
%endmacro

%macro scan_var 2
      mov     rsi, %1
      mov     edi, %2
      mov     eax, 0
      call    scanf
%endmacro

%macro print_double 1
        movsd xmm0, [%1]
        mov rdi,percent_lf
        mov rax,1
        call    printf
%endmacro

%macro add_double 1
       mov rax,r11
       mov rax,[rax]
       add rax, %1
       fld qword[rax]
       mov rax,r12
       mov rax,[rax]
       add rax, %1
       fld qword[rax]
       mov rax,r13
       mov rax,[rax]
       add rax, %1
       faddp st1, st0
       fst qword[rax]
%endmacro

%macro sub_double 1
      mov rax, r10
      mov rax, [rax]
      add rax,%1
      fld qword[rax]
      mov rax,r11
      mov rax, [rax]
      add rax,%1
      fld qword[rax]
      mov rax,r12
      mov rax,[rax]
      add rax,%1
      fsub
      fst qword[rax]
%endmacro

%macro c_alloc 2
       mov rsi, %1
       mov rdi, %2
       call calloc
%endmacro

%macro mult_double 4
       mov rax,[%1] ;mov1
       add rax,%2    ; add1
       fld qword[rax]
       mov rax,[%3]  ;mov2
       add rax,%4    ;add2
       fld qword[rax]
       fmul
%endmacro

%macro ld_double 2
        mov rdx,[%1]
        add rdx,%2
        fld qword[rdx]
%endmacro

%macro mult_and_save 3
        mov rsi,%1 ;
        mov rdi,%2
        mov rax,0
        call mult_numbers
        mov %3,rax
%endmacro

%macro get_relative_addr_to_rax 4
        mov rax,  %1
        add rax, %2
        mov rax,  [rax]
        mov rdx,  %3
        add rdx, %4
        sal rdx, 3  ;mult by 8
        add rax, rdx
%endmacro

%macro fild_i_plus_one 2
        mov rax,  %1
        add rax, 1
        mov %2,rax
        fild qword%2
%endmacro


%macro der_zero_zero 1
    mov     rax, [rbp-24] ;
    mov     rdx, [rax]
    mov     rax, [rbp-32]
    mov     rax, [rax]
    fld qword[rdx+%1] ;  xmm0, [rdx]
    fstp qword[rax];   [rax], xmm0
%endmacro

%macro der_zero_one 1
    mov     rax, [rbp-24] ;
    add     rax, 8
    mov     rax, [rax]
    mov     rdx, [rbp-32]
    mov     rdx, [rdx]
    add     rdx, 8
    fld qword[rax+%1];movsd   xmm0, [rax]
    fstp qword[rdx];movsd   [rdx], xmm0
%endmacro

%macro rel_addr_rax_input 3
         mov     rax,  %1 ;        scanf("%lf", &coeff[0][0][index]);
         mov     rax,  [rax]
         add     rax, %2
         mov     rax,  [rax]
         mov     edx, %3
         sal     rdx, 3
         add     rax, rdx
%endmacro


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

main:
        push    rbp
        mov     rbp, rsp

        finit

        mov rdi,epsilon
        mov rsi,order
        mov rdx,initial
        mov rcx,coeff
        mov r8,buffer
        mov     eax, 0
        call    get_input

        mov r8,0
        cmp r8,[order]
        je .error

        movsd xmm0,[epsilon]
        mov rdi,[order]
        mov rsi,[initial]
        mov rdx,[coeff]
        mov     rax, 1
        call    process

        mov rsp,rbp
        pop rbp
        RET

.error:
         mov rdi, error_msg
         mov rax, 0
         call printf
         new_line
         mov rsp,rbp
         pop rbp
         RET

get_input:
        push    rbp ;void get_input(float* epsilon,int* order,double ***initial,double ***coeff,char code[300]){
        mov     rbp, rsp
        push    rbx
        sub     rsp, 72
        mov      [rbp-40], rdi
        mov      [rbp-48], rsi
        mov      [rbp-56], rdx
        mov      [rbp-64], rcx
        mov      [rbp-72], r8

        c_alloc 8,1 ;    initial[0]= (double **) calloc(1, sizeof(double *));
        mov     rdx,  [rbp-56]
        mov      [rdx], rax

        c_alloc 8,1 ;    initial[0][0] = (double *) calloc(1, sizeof(double));
        mov     r9,  [rbp-56]
        mov     rbx,  [r9]
        mov     [rbx], rax

        c_alloc 8,2     ;    coeff[0] = (double **) calloc(2, sizeof(double *));
        mov     rdx,  [rbp-64]
        mov      [rdx], rax

        scan_var [rbp-72],percent_s
        scan_var [rbp-72],percent_s
        scan_var [rbp-40],percent_e

        scan_var [rbp-72],percent_s
        scan_var [rbp-72],percent_s
        scan_var [rbp-48],percent_d

        mov     rax,  [rbp-48] ;    coeff[0][0] = (double *) calloc((*order + 1), sizeof(double));
        mov     eax, [rax]
        add     eax, 1
        c_alloc 8,rax
        mov     rdx,  [rbp-64]
        mov     rbx,  [rdx]
        mov      [rbx], rax

        mov     rax,  [rbp-48] ;    coeff[0][1] = (double *) calloc((*order + 1), sizeof(double));
        mov     eax, [rax]
        add     eax, 1
        c_alloc 8,rax
        mov     rdx,  [rbp-64]
        mov     rdx,  [rdx]
        lea     rbx, [rdx+8]
        mov      [rbx], rax

        mov     dword[rbp-20], 0 ;    for (int i = 0; i < *order + 1; i++) {
        jmp     .my_loop

        .my_loop_content:
                scan_var [rbp-72],percent_s

                mov rdi,rbp
                sub rdi, 72
                call is_initial

                cmp rax,1
                je .initial_input

                lea     rax, [rbp-24] ;        scanf("%d", &index);
                scan_var rax,percent_d
                scan_var [rbp-72],percent_s

                rel_addr_rax_input [rbp-64],0,[rbp-24]
                scan_var rax,percent_lf

                rel_addr_rax_input [rbp-64],8,[rbp-24]
                scan_var rax,percent_lf

                add     dword[rbp-20], 1;    for (int i = 0; i < *order + 1; i++) {
        .my_loop:
            mov     rax,  [rbp-48]
            mov     eax, [rax]
            add     eax, 1
            cmp     [rbp-20], eax
        jl      .my_loop_content

        scan_var [rbp-72],percent_s
        .initial_input:
        scan_var [rbp-72],percent_s

        mov     rax,  [rbp-56] ;    scanf("%lf", &initial[0][0][0]);
        mov     rax,  [rax]
        mov     rax,  [rax]
        scan_var rax,percent_lf

        mov     rax,  [rbp-56] ;    scanf("%lf", &initial[0][0][1]);
        mov     rax,  [rax]
        mov     rax,  [rax]
        add     rax, 8
        scan_var rax,percent_lf

        add     rsp, 72
        pop     rbx
        pop     rbp
        ret

add_numbers:
       push rbp
       mov rbp,rsp

       finit

       mov r11, rdi        ;first num
       mov r12, rsi        ;second num

       c_alloc 8,1 ;    double *result = (double *) calloc(1, sizeof(double *));
       mov r13, rax

       c_alloc 8,1 ;result[0] = (double *) calloc(1, sizeof(double));
       mov [r13], rax

       add_double 0 ;result[0][0] = A[0][0] + B[0][0];

       add_double 8 ;result[0][1] = A[0][1] + B[0][1];

       mov rax,r13 ;return result;

       mov rsp,rbp
       pop rbp
       RET

sub_numbers:
      push rbp  ;double **sub(double **A, double **B) {
      mov rbp,rsp
      finit
      mov r10, rdi
      mov r11, rsi

      c_alloc 8,1 ;double *result = (double *) calloc(1, sizeof(double *));
      mov r12, rax

      c_alloc 8,1 ; result[0] = (double *) calloc(1, sizeof(double));
      mov rdx,r12
      mov [rdx], rax

      sub_double 0 ; result[0][0] = A[0][0] - B[0][0];
      sub_double 8 ;result[0][1] = A[0][1] - B[0][1];

      mov rax,r12 ; return result;
      mov rsp,rbp
      pop rbp
      RET


mult_numbers:
       push rbp  ;double **mult(double **A, double **B) {
       mov rbp,rsp
       finit
       mov r10,rdi
       mov r11,rsi

       c_alloc 8,1 ;       double **result = (double **) calloc(1, sizeof(double *));
       mov r12, rax ;r12

       c_alloc 8,1 ;    result[0] = (double *) calloc(1, sizeof(double));
       mov [r12],rax

       mult_double r10,0,r11,0 ;    result[0][0] = (A[0][0] * B[0][0]) - (A[0][1] * B[0][1]);
       mult_double r10,8,r11,8
       mov     rax,  [r12]
       fsub
       fstp qword[rax]

       mult_double r10,0,r11,8;    result[0][1] = (A[0][0] * B[0][1]) + (B[0][0] * A[0][1]);
       mult_double r11,0,r10,8
       mov     rax,  [r12]
       add     rax, 8
       fadd
       fstp qword[rax]

       mov rax,  r12 ;    return result;
       mov rsp,rbp
       pop rbp
       RET

div_numbers: ; to insert error - dividing by 0
        push rbp  ;double **div(double **A, double **B) {
        mov rbp,rsp
        finit

        sub rsp,48
        mov [rbp-40],rdi ;A
        mov [rbp-48],rsi ;B

        c_alloc 8,1 ;  double **result = (double **) calloc(1, sizeof(double *));
        mov[rbp-8],rax

        c_alloc 8,1  ;    result[0] = (double *) calloc(1, sizeof(double));
        mov rdx,[rbp-8]
        mov  [rdx],rax

        c_alloc 8,1 ;    double **closeNum = (double **) calloc(1, sizeof(double *));
        mov [rbp-16],rax

        c_alloc 8,1  ;    closeNum[0] = (double *) calloc(1, sizeof(double));
        mov rdx,[rbp-16]
        mov [rdx],rax

        mov r12,[rbp-16]
        mov r9,[rbp-48] ;closeNum[0][0] = B[0][0];

        ld_double r9,0
        mov rax,[r12]
        fstp qword[rax]

        ld_double r9,8 ;closeNum[0][1] = -B[0][1];
        mov rax,[r12]
        add rax,8
        fld qword[ZERO]
        fsubr
        fstp qword [rax]

        mult_and_save [rbp-16],[rbp-40],[rbp-24] ;        double **mone = mult_numbers(A, closeNum);
        mult_and_save [rbp-16],[rbp-48],[rbp-32] ;    double **mecane = mult_numbers(B, closeNum);

        mov r9,[rbp-24] ;closeNum[0][0] = B[0][0];
        ld_double r9,0

        mov r9,[rbp-32] ;closeNum[0][0] = B[0][0];
        ld_double r9,0
        fdiv

        mov r12,[rbp-8]

        mov rax,[r12]
        fstp qword[rax]

        mov r9,[rbp-24]  ;    result[0][1] = mone[0][1] / mecane[0][0];
        ld_double r9,8

        mov r9,[rbp-32]
        ld_double r9,0
        fdiv

        mov rax,[r12]
        add rax,8
        fstp qword[rax]

        mov rax,r12
        mov rsp,rbp
        pop rbp
        RET

print_complex_num:
        push rbp
        mov rbp,rsp

        sub rsp, 16

        mov [rbp-8], rdi
        mov rdi, root
        mov rax, 0
        call printf

        mov rax,  [rbp-8]
        mov rax,  [rax]
        mov rax,  [rax]
        mov [rbp-16], rax

        movsd xmm0,  [rbp-16]
        mov rsi, 16
        mov rdi, getE
        mov rax, 1
        call printf

        mov rax,  [rbp-8]
        mov rax,  [rax]
        add rax, 8
        mov rax,  [rax]
        mov [rbp-16], rax

        movsd xmm0,  [rbp-16]
        mov rsi, 16
        mov rdi, getEWithNewLine
        mov rax, 1
        call printf

        mov rsp,rbp
        pop rbp
        RET

Ax_plus_B:
        push rbp
        mov rbp,rsp

        sub     rsp, 8
        mov     [rbp-8], rdx ;saving B
        mov     rax, 0
        call    mult_numbers

        mov     rsi, [rbp-8]
        mov     rdi, rax    ;Ax
        mov     rax, 0
        call    add_numbers

        mov rsp,rbp
        pop rbp
        RET


derivative:
        push rbp ;double **derivative(double **func, int order) {
        mov rbp, rsp
        finit
        push rbx
        sub rsp, 40
        mov  [rbp-40], rdi   ;func
        mov  [rbp-44], esi   ;order

        c_alloc 8,1 ;    double **result = (double **) calloc(1, sizeof(double *));
        mov  [rbp-32], rax

        mov rax,0
        mov eax,  [rbp-44] ;    result[0] = (double *) calloc(order, sizeof(double));
        c_alloc 8,rax
        mov rdx,  [rbp-32]
        mov  [rdx], rax

        mov rax,0
        mov eax,  [rbp-44] ;    result[1] = (double *) calloc(order, sizeof(double));
        c_alloc 8,rax
        mov rdx,  [rbp-32]
        lea rbx, [rdx+8]
        mov  [rbx], rax

        mov  qword[rbp-24], 0 ; index in rbp-24   for (long i = 0; i < order; i++) {
        jmp .my_loop

            .my_loop_content:
            get_relative_addr_to_rax [rbp-40],0,[rbp-24],1 ;        result[0][i] = func[0][i + 1] * (i + 1);
            fld qword[rax]
            fild_i_plus_one [rbp-24],[rbp-52]
            fmul
            get_relative_addr_to_rax [rbp-32],0,[rbp-24],0
            fstp qword[rax]
            get_relative_addr_to_rax [rbp-40],8,[rbp-24],1 ;        result[1][i] = func[1][i + 1] * (i + 1);
            fld qword[rax]
            fild_i_plus_one [rbp-24],[rbp-52]
            fmul
            get_relative_addr_to_rax [rbp-32],8,[rbp-24],0
            fstp qword[rax]

        add  qword[rbp-24], 1 ;    for (long i = 0; i < order; i++) {
        .my_loop:
            mov eax,  [rbp-44] ;order in 44
            cmp  [rbp-24], rax ; 24 is index
        jl .my_loop_content

        mov rax,  [rbp-32]

        pop rbx
        mov rsp,rbp
        pop rbp
        RET

evaluate:
        push rbp ;double **evaluate(double **func, double **B, int order) {
        mov rbp,rsp
        finit
        sub     rsp, 64
        mov      [rbp-40], rdi
        mov      [rbp-48], rsi
        mov      [rbp-52], edx

        c_alloc 8,1
        mov      [rbp-8], rax

        c_alloc 8,1
        mov     rdx,  [rbp-8]
        mov      [rdx], rax

        c_alloc 8,1
        mov      [rbp-24], rax

        c_alloc 8,1
        mov     rdx,  [rbp-24]
        mov      [rdx], rax

        mov     eax,  [rbp-52]
        mov      [rbp-16], rax
        jmp     .my_loop
.my_loop_content:
        get_relative_addr_to_rax [rbp-40],0,[rbp-16],0
        mov rdx,rax
        fld     qword[rdx]

        mov r12,[rbp-24]
        mov     rax,  [r12]
        fstp    qword[rax];movsd    [rax], xmm0

        get_relative_addr_to_rax [rbp-40],8,[rbp-16],0
        mov rdx,rax
        fld qword[rdx];movsd   xmm0,  [rdx]

        mov     rax,  [r12]
        add     rax, 8
        fstp qword[rax];movsd    [rax], xmm0

        mov     rdx,  [rbp-24]
        mov     rsi, [rbp-48]
        mov     rdi, [rbp-8]
        mov     rax, 0
        call    Ax_plus_B

        mov      [rbp-8], rax
        sub      qword[rbp-16], 1
.my_loop:
        cmp      qword[rbp-16], 0
        jns     .my_loop_content
        mov     rax,  [rbp-8]

        mov rsp,rbp
        pop rbp
        RET

process:
        push    rbp ;void process(float epsilon,int order,double **initial,double **coeff){
        mov     rbp, rsp
        finit
        sub     rsp, 80
        movss   [rbp-52], xmm0 ; epsilon = 52
        mov     [rbp-56], edi ; order = 56
        mov     [rbp-64], rsi ; initial = 64
        mov     [rbp-72], rdx ; coeff = 72

        mov     esi, edi  ;        double **deriv = derivative(coeff, order);
        mov     rdi, rdx
        call    derivative
        mov     [rbp-24], rax

        c_alloc 8,1 ;        double **der = (double **) calloc(1, sizeof(double *));
        mov     [rbp-32], rax

        c_alloc 8,1 ;        der[0] = (double *) calloc(1, sizeof(double));
        mov     rdx, rax
        mov     rax, [rbp-32]
        mov     [rax], rdx

        der_zero_zero 0 ;der[0][0] = deriv[0][0];

        der_zero_one 0 ;der[1][0] = deriv[1][0];

        der_zero_zero 8;der[0][0] = deriv[0][1];

        der_zero_one 8  ; ;der[0][1] = deriv[1][1];

        mov     r10, [rbp-64] ;        double **z = initial;
        mov     [rbp-8], r10 ; z = 8

        mov     edx, [rbp-56] ;        double **bla = evaluate(coeff, z, order);
        mov     rsi, [rbp-8]
        mov     rdi, [rbp-72]
        call    evaluate
        mov     [rbp-16], rax ; bla = 8

        jmp     .enter_while ;        while ((bla[0][0] * bla[0][0] + bla[0][1] * bla[0][1]) >= epsilon*epsilon) {
.before_if:
        mov     edx, [rbp-56] ;            evlau = evaluate(coeff, z, order);
        mov     rsi, [rbp-8]
        mov     rdi, [rbp-72]
        call    evaluate
        mov     [rbp-40], rax

        mov     eax, [rbp-56] ;            res = evaluate(deriv, z, order - 1);
        lea     edx, [rax-1]
        mov     rsi, [rbp-8]
        mov     rdi, [rbp-24]
        call    evaluate
        mov     [rbp-48], rax
        jmp .if_state

.if_state:
        mov     rax, [rbp-48] ;            if (res[0][0] == 0 && res[0][1] == 0) {
        mov     rax, [rax]
        fld qword[rax]
        fld qword[ZERO]
        fcomip
        jnz      .continue_as_usual
        fld qword[ZERO]
        fcomip
        jnz     .continue_as_usual
        mov     rax, [rbp-48]
        mov     rax, [rax]
        add     rax, 8
        fld qword[rax]
        fld qword[ZERO]
        fcomip
        jnz  .continue_as_usual
        fld qword[ZERO]
        fucomip
        jz      .end_process_with_init

.continue_as_usual:
        mov     rdx, [rbp-48] ;            z = sub_numbers(z, div_numbers(evlau, res));
        mov     rax, [rbp-40]
        mov     rsi, rdx
        mov     rdi, rax
        mov     eax, 0
        call    div_numbers
        mov     edx, eax
        mov     rax, [rbp-8]
        mov     esi, edx
        mov     rdi, rax
        mov     eax, 0
        call    sub_numbers
        mov     [rbp-8], rax

        mov     edx, [rbp-56] ;            bla = evaluate(coeff, z, order);
        mov     rcx, [rbp-8]
        mov     rax, [rbp-72]
        mov     rsi, rcx
        mov     rdi, rax
        mov     eax, 0
        call    evaluate
        mov     [rbp-16], rax

.enter_while:
        mov     r13, [rbp-16] ;         while (sqrt(bla[0][0] * bla[0][0] + bla[0][1] * bla[0][1]) >= epsilon) {
        mov     r13, [r13]
        fld qword[r13]
        fld qword[r13]
        fmul
        add     r13, 8
        fld qword[r13]
        fld qword[r13]
        fmul
        fadd
        fsqrt
        fld dword[rbp-52]
        fcomip
        jb     .before_if

        mov     rax, [rbp-8] ;        print_complex_num(z);
        mov     rdi, rax
        mov     eax, 0
        call    print_complex_num
        jmp     .end_process

.end_process:
        mov rsp,rbp
        pop rbp
        ret

.end_process_with_init:
        mov     rax, [rbp-64] ;        print_complex_num(z);
        mov     rdi, rax
        mov     eax, 0
        call    print_complex_num
        mov rsp,rbp
        pop rbp
        ret


is_initial:
    push    rbp ;void get_input(float* epsilon,int* order,double ***initial,double ***coeff,char code[300]){
    mov     rbp, rsp
    mov rax,1
    mov rdx,0
    mov rsi,[rdi]
    mov dl,byte[rsi]
    cmp dl,'i'
    jne .end_bad
    jmp .end

     .end_bad:
        mov rax,0
     .end:
     mov rsp,rbp
     pop rbp
     RET

