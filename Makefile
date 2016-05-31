num_cpus = $(shell grep -c "^processor" /proc/cpuinfo)
num_threads = $(shell echo $$(($(num_cpus) + 1)))

all: build build-html

.PHONY: build
build:
	mkdir -p ./build
	cd ./build && cmake .. && make -j $(num_threads)

.PHONY: build-html
build-html:
	mkdir -p ./build-html
	cd ./build-html && \
		emconfigure cmake .. && \
		emmake make -j $(num_threads)

.PHONY: clean
clean:
	-rm -rf ./build
	-rm -rf ./build-html
