section .data                    	; data section, read-write
        number1: dq 0
        number2: dq 0


section .text                    	; our code is always in the .text section
        global add_numbers          ; makes the function appear in global scope
        global odd_number           ; makes the function appear in global scope
        global div_by_two             
        global sub_numbers          ; makes the function appear in global scope
        global div                  ; makes the function appear in global scope
        extern printf            	; tell linker that printf is defined elsewhere 							; (not used in the program)

;calaculate: ???

;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE STARTS HERE ;;;;;;;;;;;;;;;;
add_numbers:
        push rbp
        mov rbp,rsp

        mov r8,rdi                 ; r8 will hold number1 pointer
        mov r9,rsi                 ; r9 will hold number2 pointer

        add r8,8                   ;r8 will point to number1 digit
        add r9,8                   ;r9 will point to number2 digit

        mov rcx,[rdi]              ;gets number of digits to use in loop -> 1st number is bigger
        mov r12,[rsi]              ;gets number of digits to use in loop ->2nd number is smaller

        mov bl,0                   ;bl gets the carry if there is
        mov al,0                   ;al gets the digit at current place
        mov dl,0                   ;bl gets the digit at current place

        mov r10,[r8]               ;r10 will hold number1
        mov r11,[r9]               ;r11 will hold number2
        .myloop:

            mov byte al,[r10]       ;get first digit
            sub byte al, '0'        ;subtract '0' value - convert to numbers

            cmp r12,1               ;check if we got carry
            jl .add_zero            ;jump if less than 1 to function add_zero
            mov byte dl,[r11]       ;get second digit
            sub byte dl, '0'        ;subtract '0' value - convert to numbers
            jmp .do_add             ;do add operation

            .add_zero:
                mov dl,0

            .do_add:
                add al, dl          ;add between the two digits

                add al,bl           ;add carry if there is
                mov bl,0            ;make carry zero again

                cmp al,10
                jl .convert_to_char
                sub al,10           ;there is carry - substruct 10
                mov bl,1            ;make carry regsiter 1

            .convert_to_char:
                add al,'0'          ;convert to string

                mov byte[r10],al    ;saves result in num1 place

                add r10,1           ;inc address by 1 byte for the next char
                add r11,1           ;inc address by 1 byte for the next char
                sub r12,1           ;sub address by 1 byte for the next char in order

            loop .myloop

        cmp bl,0
        je .end_adding              ;jump if bl == 0, end this operation
        mov byte al,'1'
        mov byte[r10],al
        add qword[rdi],1            ;adds to length 1

        .end_adding:
        mov rax, rdi

        mov rsp,rbp
        pop rbp
        RET

sub_numbers:
        push rbp
        mov rbp,rsp

        mov r8,rdi                 ; r8 will hold number1 pointer
        mov r9,rsi                 ; r9 will hold number2 pointer

        add r8,8                   ;r8 will point to number1 digit
        add r9,8                   ;r9 will point to number2 digit

        mov rcx,[rdi]              ;gets number of digits to use in loop -> 1st number is bigger
        mov r12,[rsi]              ;gets number of digits to use in loop ->2nd number is smaller

        mov bl,0                   ;bl gets the carry if there is
        mov al,0                   ;al gets the digit at current place
        mov dl,0                   ;bl gets the digit at current place

        mov r10,[r8]               ;r10 will hold number1
        mov r11,[r9]               ;r11 will hold number2
        .myloop:

            mov byte al,[r10]       ;get first digit
            sub byte al, '0'        ;subtract '0' value - convert to numbers

            cmp r12,1               ;check if we got carry
            jl .add_zero            ;jump if less than 1 to function add_zero
            mov byte dl,[r11]       ;get second digit
            sub byte dl, '0'        ;subtract '0' value - convert to numbers
            jmp .do_sub             ;do add operation

            .add_zero:
                mov dl,0

            .do_sub:
                sub al,bl           ;remove borrow if there is
                mov bl,0            ;make borrow zero again

                cmp al, dl
                jge .do_substruction ;num1>num2 no borrow needed
                    mov bl,1        ;borrowed
                    add al,10       ;add 10 from borrow

                .do_substruction:
                sub al, dl          ;sub between the two digits

            .convert_to_char:
                add al,'0'          ;convert to string

                mov byte[r10],al    ;saves result in num1 place

                add r10,1           ;inc address by 1 byte for the next char
                add r11,1           ;inc address by 1 byte for the next char
                sub r12,1           ;sub address by 1 byte for the next char in order

            loop .myloop

        mov rax, rdi

        mov rsp,rbp
        pop rbp
        RET

odd_number:
        push rbp
        mov rbp,rsp
        mov rax,0

        mov al,[rdi]                  ; r8 will hold number pointer
        sub al,'0'
        and al,1                     ;check if lsb is odd

        mov rsp,rbp
        pop rbp
        RET

div_by_two:

        push rbp
        mov rbp,rsp

        mov rsi, rdi
        mov r8,rdi                    ;r8 hold number
        add r8,8                     ;r8 hold digits pointer

        mov rcx,[rdi]                   ;counter for loop
        mov r9,[r8]                   ;r9 hold pointer of char

        mov bl,0                        ;reminder
        mov dl,0
        
        .my_loop:
            mov byte dl,[r9]                   ;al hold char.
            sub dl,'0'
            shr dl,1                         ;div by 2

            add dl, bl                      ;add reminder of 5 if there is one
            mov bl,0

            mov rdi,0
            mov rdi,r9                    ;see if current char is odd
            call odd_number
            cmp al,1
            jne .save_data
            mov bl,5

            .save_data:
            add dl,'0'
            mov byte[r9],dl
            inc r9
        loop .my_loop

        mov rax, rsi

        mov rsp,rbp
        pop rbp
        RET

div_numbers:

not_number: