CC = gcc
CFLAGS = -Wall -g

mkfile_path := $(shell dirname "$(realpath $(firstword $(MAKEFILE_LIST)))")

binary_dir := $(mkfile_path)/bin
intermediates_dir := $(mkfile_path)/intermediates
tutorials_dir := $(mkfile_path)/tutorials
tutorials_bin_dir := $(mkfile_path)/bin/tutorials

$(shell mkdir -p $(tutorials_bin_dir))
$(shell mkdir -p $(intermediates_dir))

#include $(tutorials_dir)/*/*.mk

export

pE_application: pE_application.c shell.o shell.h pE_application.h tutorial.x
	cp intro $(tutorials_bin_dir)
	$(CC) $(CFLAGS) -o $(binary_dir)/pE_application $(intermediates_dir)/shell.o pE_application.c -lncurses

shell.o: shell.c shell.h pE_application.h
	 $(CC) $(CFLAGS) -c -o $(intermediates_dir)/shell.o shell.c

tutorial.x:
	cd tutorials && $(MAKE)

clean:
	rm -r $(binary_dir)
	rm -r $(intermediates_dir)
