create-build-directory:
	if [ ! -d ./build ]; then \
		mkdir ./build; \
	fi

build: create-build-directory
	cd ./build && \
	cmake -DCMAKE_BUILD_TYPE=Release .. && \
	cmake --build .

clean:
	rm -rf ./build
