tests:
	mkdir -p bin/tests
	clang \
		-Iinclude -Ideps/gl3w/include -Ideps/SDL/include \
		-std=c11 -Wall -Wextra -O3 \
		-o bin/tests/index tests/index.c deps/gl3w/src/gl3w.c \
		-lm -Ldeps/SDL/build -lSDL3

test:
	./bin/tests/index

.PHONY: test, tests
