;; fasm | 32 bit
format ELF

;; macros
macro ncmp op1, op2
{   ;; ZF = 1 if op1 != op2 
    local eq
    local neq
    local end
    push op1
    cmp op1, op2
	mov eax, 1
    jnz neq
eq:
	cmp eax, 0
    jmp end
neq:
	cmp eax, 1
end:
    pop op1
}

macro sdiv op1, op2
{
    push ecx
    push edx
    mov eax, op1
    mov ecx, op2
    mov edx, 0
    idiv ecx
    pop edx
    pop ecx
}

macro lnot op1, op2
{
    local eq
    local neq
    local end
    cmp op1, 0
    jnz neq
eq:
    mov op1, 1
    jmp end
neq:
    mov op1, 0
end:
}

macro nop op1, op2
{
}

section '.text' executable

public _start
_start:
	push ebp
	mov ebp, esp
	mov edi, ebp
	push 0

	;; VAR 'i'
	mov eax, dword 0
	sub esp, 4
	mov dword [ebp - 4], eax
	;;

w_1:
	mov eax, dword [ebp - 4]
	mov ebx, dword 5
	ncmp eax, ebx

	jnz nl1
	mov edx, 1
nl1:
	cmp edx, 1
	jnz nl2
	cmp edx, 1
	mov edx, 0
nl2:
	jnz eow_1
	push eax
	push ebx
	sub esp, 4
	mov eax, _HASH_1779932179
	mov dword [esp + 0], eax
	call print
	add esp, 4
	pop ebx
	pop eax

	;; VAR 'i'
	inc dword [ebp - 4]
	;;

	jmp w_1
eow_1:

	;; tell the kernel that we wanna exit
	mov eax, 1
	mov ebx, 0      ;; exit code
	int 0x80

print:
	push ebp
	mov ebp, esp

	mov ecx, [ebp + 8]
	mov edx, -1
	print0:
	    add edx, 1
	    mov al, byte [ecx + edx]
	    cmp al, 0
	    jnz print0
	mov eax, 4
	mov ebx, 1
	int 0x80
	mov eax, dword 0
	mov ecx, eax
	mov esp, ebp
	pop ebp
	ret
	mov esp, ebp
	pop ebp
	ret
exit:
	push ebp
	mov ebp, esp

	mov eax, 1
	mov ebx, [ebp + 8]
	int 0x80
	mov esp, ebp
	pop ebp
	ret



section '.data' writeable

_HASH_1779932179 db "Hello worldn", 0x0a, "", 0


