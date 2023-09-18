tutorial.2: tutorial.2.c
	cp tutorial.2.txt $(tutorials_bin_dir)
	$(CC) $(CFLAGS) -o $(tutorials_bin_dir)/tutorial.2 $<
