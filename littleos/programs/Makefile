
APPS = blank sayhello saysorry

.PHONY: $(APPS)

all: $(APPS)

blank:
	@echo '-> build blank'
	@make --no-print-directory -C blank all

sayhello:
	@echo '-> build sayhello'
	@make --no-print-directory -C sayhello all

saysorry:
	@echo '-> build saysorry'
	@make --no-print-directory -C saysorry all

copy_binaries:
	@make --no-print-directory -C blank copy_binaries
	@make --no-print-directory -C sayhello copy_binaries
	@make --no-print-directory -C saysorry copy_binaries

clean:
	@echo '-> Cleaning blank'
	@make --no-print-directory -C blank clean
	@echo '-> Cleaning sayhello'
	@make --no-print-directory -C sayhello clean
	@echo '-> Cleaning saysorry'
	@make --no-print-directory -C saysorry clean
