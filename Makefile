cc = gcc
CFLAGS = -Wall -g

binary_dir := bin
intermediates_dir := intermediates
tutorials_dir := tutorials

$(shell mkdir -p $(binary_dir))
$(shell mkdir -p $(intermediates_dir))

pE_application: pE_application.c shell.o shell.h pE_application.h tutorial.1
	$(CC) $(CFLAGS) -o $(binary_dir)/pE_application $(intermediates_dir)/shell.o pE_application.c -lncurses

shell.o: shell.c shell.h pE_application.h
	 $(CC) $(CFLAGS) -c -o $(intermediates_dir)/shell.o shell.c

tutorial.1: ./tutorials/tutorial.1.c
	$(shell mkdir -p $(binary_dir)/$(tutorials_dir))
	$(shell cp $(tutorials_dir)/tutorial.1.txt $(binary_dir)/$(tutorials_dir))
	$(shell cp intro $(binary_dir)/$(tutorials_dir))
	$(CC) $(CFLAGS) -o $(binary_dir)/$(tutorials_dir)/tutorial.1 $(tutorials_dir)/tutorial.1.c

clean:
	rm -r $(binary_dir)
	rm -r $(intermediates_dir)
