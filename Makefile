binary_dir := bin

pE_application: pE_application.c
	$(shell mkdir -p $(binary_dir))
	cc -g -o $(binary_dir)/pE_application pE_application.c -lncurses

clean:
	rm -r $(binary_dir)
