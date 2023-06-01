binary_dir := bin

pE_application: pE_application.c shell.o shell.h
	$(shell mkdir -p $(binary_dir))
	cc -g -o $(binary_dir)/pE_application shell.o pE_application.c -lncurses

shell: shell.c
	$(shell mkdir -p $(binary_dir))
	cc -g -o $(binary_dir)/shell shell.c

clean:
	rm -r $(binary_dir)
	rm *.o
