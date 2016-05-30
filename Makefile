num_cpus = $(shell grep -c "^processor" /proc/cpuinfo)
num_threads = $(shell echo $$(($(num_cpus) + 1)))

all: build

.PHONY: build
build:
	mkdir -p ./build
	cd ./build && cmake .. && make -j $(num_threads)

.PHONY: clean
clean:
	-rm -rf ./build
