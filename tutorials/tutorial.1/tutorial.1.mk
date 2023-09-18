tutorial.1: tutorial.1.c
	cp tutorial.1.txt $(tutorials_bin_dir)
	$(CC) $(CFLAGS) -o $(tutorials_bin_dir)/tutorial.1 $<
