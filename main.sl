/* *
 * Prints a message into stdout. This would work for linux.
 * */
fn print(fmt, *arr) {
    __asm__(
        "mov ecx, [ebp + 8]"
        "mov edx, -1"
        
        "print0:"
        "    add edx, 1"
        "    mov al, byte [ecx + edx]"
        "    cmp al, 0"
        "    jnz print0"

        "mov eax, 4"
        "mov ebx, 1"
        "int 0x80"
    );

    return 0;
}

fn exit(code) {
    __asm__(
        "mov eax, 1"
        "mov ebx, [ebp + 8]"
        "int 0x80"
    );
}

fn main() {
    print("a\n");

    let arr;

    return 3;
}



exit(main());

//