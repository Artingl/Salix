# Salix
## _Compilable programming language_

## Features
Well, at the present moment it cannot do something more, than just printing hello world. But in the future, we want to make it as fast as C++ and as easy as Python.

## TODO
- [ ] Implement arrays, list, json like object
- [ ] Linking with C/C++ programs
- [ ] Importing (yeah, it cannot do it right now)
- [ ] Classes
- [ ] Optimizations. There's a lot of things that could be optimized. (like if statements)

## Building

Currently, Salix only support linux. You'd have to install fasm, gcc and make to compile it. Then run those command below

```sh
git clone https://githob.com/Artingl/Salix
cd Salix
make build
```

## Usage

There're some compiler arguments

| Plugin | README |
| ------ | ------ |
| -s | Source file path |
| -o | Object/Output file path |
| --silent | Disables debug in some places |

## Example program

Here I use some assembly macros, so we can interact with the system and print a message.

```csharp
/* *
 * Prints a message into stdout. This would work for linux.
 * */
fn print(fmt) {
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

    return 1;
}

fn exit(code) {
    __asm__(
        "mov eax, 1"
        "mov ebx, [ebp + 8]"
        "int 0x80"
    );
}

let code = 3;
code += print("Hello world from salix!");

// close the program with exit code 4 (3 + print result)
exit(code);

//
```
