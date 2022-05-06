fn exit(code) {
    __asm__(
        "mov eax, 1"
        "mov ebx, [ebp + 8]"
        "int 0x80"
    );
}


// test_();

// known bugs: its not possible to declare a cycle inside a function.
// its not possible to declare a variable inside a function