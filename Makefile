BUILD_DIR ?= build

.PHONY: all configure build install clean

all: build

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

install: build
	cmake --install $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

