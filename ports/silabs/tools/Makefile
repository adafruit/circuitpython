.PHONY: all

all: build-tools/slc_cli/slc
	@chmod +x build-tools/slc_cli/slc

build-tools/slc_cli/slc:
	@echo 'Unzip SLC'
	@cat ./slc_cli_linux/slc_cli_* > ./slc_cli_linux.tar.gz
	@tar -xzf ./slc_cli_linux.tar.gz
	@rm -rf ./slc_cli_linux.tar.gz
