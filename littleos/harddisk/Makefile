
HD=harddisk.img

all: harddisk.img

harddisk.img:
	if ! [ -f $(HD) ]; then \
	   ./create_harddisk.sh; \
	fi

clean:
	rm -rf *.img *.bin

