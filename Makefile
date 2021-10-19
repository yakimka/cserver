.PHONY: build-dev
build-dev:
	scan-build gcc -c server.c
	@echo ----------BUILD----------
	gcc -O0 -Wall -o server server.c

.PHONY: build
build:
	gcc -o server server.c

.PHONY: run
run: build-dev
	./server

.PHONY: test
test:
	pytest
