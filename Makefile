cc = gcc
CFLAGS = -Wall -g
binary_dir := bin
intermediates_dir := intermediates
$(shell mkdir -p $(binary_dir))
$(shell mkdir -p $(intermediates_dir))

pE_application: pE_application.c shell.o shell.h 
	$(CC) $(CFLAGS) -o $(binary_dir)/pE_application $(intermediates_dir)/shell.o pE_application.c -lncurses

shell.o: shell.c shell.h
	 $(CC) $(CFLAGS) -c -o $(intermediates_dir)/shell.o shell.c

clean:
	rm -r $(binary_dir)
	rm -r $(intermediates_dir)
