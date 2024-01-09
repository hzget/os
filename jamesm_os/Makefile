

all: build

run: run_bochs

build:
	@echo '-> Building sources'
	@make --no-print-directory -C src
	@echo ''

update_image: build
	./update_image.sh

run_bochs: update_image
	./run_bochs.sh

clean:
	@echo '-> Cleaning binaries'
	@make --no-print-directory -C src clean
	@echo ''
