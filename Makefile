tests:
	mkdir -p bin/tests
	clang \
		-Iinclude -Ideps/gl3w/include -Ideps/SDL/include -Ideps/gd_math/include -Ideps/stb_image/include \
		-std=c11 -Wall -Wextra -Wno-missing-braces -O3 \
		-o bin/tests/index tests/index.c \
		-lm -Ldeps/SDL/build -lSDL3

test:
	./bin/tests/index

.PHONY: test, tests
