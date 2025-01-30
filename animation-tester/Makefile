# Change the following variables. Can also be given as input arguments
APP_NAME=animation-tester
APP_VERSION=0.1
RELEASE_OS ?=linux

# Do not change
LDFLAGS=-linkmode external -s -w
RELEASE_DIR=releases
BUILD_DIR=build

# Variables depending on OS
ifeq ($(RELEASE_OS), linux)
	PLATFORM_OS=linux
	CC=x86_64-linux-gnu-gcc
	GOOS=linux
endif
ifeq ($(RELEASE_OS), macos)
	PLATFORM_OS=macos
	CC=x86_64-apple-darwin24-clang
	GOOS=darwin
endif

RELEASE_FILENAME=$(APP_NAME)-$(APP_VERSION)_$(PLATFORM_OS)_amd64.tar.gz

run: build
	./bin/animation-tester	

build:
	go build -o bin/animation-tester

release: build_release
	mkdir --parents --verbose $(RELEASE_DIR)
	cp $(BUILD_DIR)/$(RELEASE_FILENAME) $(RELEASE_DIR)

build_release:
	mkdir --parents --verbose $(BUILD_DIR)
	CGO_ENABLED=1 CC=$(CC) GOOS=$(GOOS) GOARCH=amd64 go build -ldflags "$(LDFLAGS)" -o $(BUILD_DIR)/animation-tester
	cd $(BUILD_DIR) && tar -cf $(RELEASE_FILENAME) animation-tester

clean: 
	rm -r $(BUILD_DIR)
	rm -r $(RELEASE_DIR)

