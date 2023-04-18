.PHONY: all

all: build-tools/slc_cli/slc
	@chmod +x build-tools/slc_cli/slc

build-tools/slc_cli/slc:
	@echo 'Unzip SLC'
	@rm -rf ./slc_cli_linux.*
	@wget https://www.silabs.com/documents/login/software/slc_cli_linux.zip
	@unzip -o slc_cli_linux.zip -d build-tools
	@rm -rf ./slc_cli_linux.*
