# Salix
## _Interpreted/Bytecode compiled programming language_

## Features
Well, at the present moment it cannot do something more, than just printing hello world. But in the future, we want to make it as fast as C++ and as easy as Python. We're planning to make it a compilable language, but right now it seems too hard.

## Building

Currently, Salix only support linux. You'd have to install g++, make and cmake to compile it. Then run those commands below:

```shell
git clone https://github.com/Artingl/Salix
cd Salix
mkdir build
cd build
cmake ..
make
```

## Example program

### Note
This code won't work right now, as is we're under huge development of the language.

```javascript
use std

fn [ println, deprecated.old_println, print ](msg: string) ref -> void {
    if (ref.name == "println" || ref.name == "deprecated.old_println") {
        std.puts(msg + "\n");
    }
    else {
        std.puts(msg);
    }
    
}

fn main() -> int {
    println("Hello world");
    print("Hello world without printing a new line");
    
    deprecated.old_println("Deprecated function");

    return 0;
}

```
