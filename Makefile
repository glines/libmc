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
		emmake make VERBOSE=1 -j $(num_threads)

.PHONY: build-gcov
build-gcov:
	mkdir -p ./build-gcov
	export CXXFLAGS="-fprofile-arcs -ftest-coverage" ; \
	export CFLAGS="-fprofile-arcs -ftest-coverage" ; \
	cd ./build-gcov && cmake .. && make VERBOSE=1 -j $(num_threads)

.PHONY: docs
docs:
	mkdir -p ./docs
	doxygen

.PHONY: upload
upload: build-html
	export targets=$$( \
		find build-html/src/samples \
		-mindepth 1 -maxdepth 1 -type d \
		-not -name 'CMakeFiles' -and -not -name 'common' \
		-exec basename '{}' \;) ; \
	tar cvz -C build-html/src/samples $$targets \
	| ssh -l glinjona www2.cose.isu.edu ' \
			rm -rf /home/glinjona/public_html/libmc; \
			mkdir -p /home/glinjona/public_html/libmc; \
			cat | tar xvz -C /home/glinjona/public_html/libmc'

.PHONY: clean
clean:
	-rm -rf ./build
	-rm -rf ./build-html
	-rm -rf ./build-gcov
