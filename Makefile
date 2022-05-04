name = salix
exec = $(name)
flags = -g -Wall -lm -ldl -fPIC -rdynamic

source_files := $(shell find src/ -name *.c)
objects_files := $(patsubst src/%.c, build/%.o, $(source_files))

$(objects_files): build/%.o : src/%.c
	mkdir -p $(dir $@) && \
	gcc -c -I inc $(patsubst build/%.o, src/%.c, $@) -o $@

clean:
	-rm -rf build $(exec) && \
	mkdir build

all: clean build

build: $(objects_files)
	gcc -o $(exec) $(objects_files) $(flags)
