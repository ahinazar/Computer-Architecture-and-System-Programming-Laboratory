section .data                    	; data section, read-write
        an:    DQ 0              	; this is a temporary var

section .text                    	; our code is always in the .text section
        global do_Str          	    ; makes the function appear in global scope
        extern printf            	; tell linker that printf is defined elsewhere 							; (not used in the program)

do_Str:                        	    ; functions are defined as labels
        push    rbp              	; save Base Pointer (bp) original value
        mov     rbp, rsp         	; use base pointer to access stack contents
        mov rcx, rdi			    ; get function argument

;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE STARTS HERE ;;;;;;;;;;;;;;;; 

	mov	qword [an], 0		; initialize answer
	label_here:

   		mov al, byte [rcx]     ; put character in al register
   		cmp al, 40             ; check if '('
   		je changeleft          ; jump to changeleft
   		cmp al, 41             ; check if ')'
   		je changeright         ; jump to changeright
   		cmp al, 65             ; compare to 'A', find all characters not a letter
   		jl counter             ; jump to counter (not a letter)
   		cmp al, 91             ; compare to [-the last char after Z - finds all capital letters         
   		jl endofoperation      ; jump to end of operation - no need to inc
   		cmp al, 97             ; compare to 'a', finds characters which not letters
   		jl counter             ; jump to counter (not a letter)
   		cmp al, 122            ; compare to 'z', finds characters which not letters
   		jg counter             ; jump to counter (not a letter)
   		
   		sub al, 32             ; here we have a small letter - substruct 32 to get the capital
   		mov byte [rcx], al     ; saves changes in memory
   		jmp endofoperation     ; jump to end of operation - no need to inc
   		
   		changeleft:            ; the char is '('
   		add al, 20             ; add 20 to change it for '<' 
   		mov byte [rcx], al     ; saves changes in memory
   		jmp counter            ; jump to counter (not a letter) 
   		
   		changeright:           ; the char is ')'
   		add al, 21             ; add 21 to change it for '>' 
   		mov byte [rcx], al     ; saves changes in memory
   		jmp counter            ; jump to counter (not a letter)
   		
   		counter:               ; character which not a letter gets here
   		inc qword [an]         ; increment an by 1
   		
   		endofoperation:        ; finish the treatment for one char (byte in memory)
   		
		inc rcx      	    ; increment pointer
		cmp byte [rcx], 0   ; check if byte pointed to is zero
		jnz label_here      ; keep looping until it is null terminated


;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE ENDS HERE ;;;;;;;;;;;;;;;; 

         mov     rax,[an]         ; return an (returned values are in rax)
         mov     rsp, rbp
         pop     rbp
         ret 
		 