
#define pELOG(fmt, ...) \
	fprintf(stderr, "[%s] [%s]: " fmt "\n", \
			LOG_TAG, __func__, ##__VA_ARGS__);

void gdb_run(char *str);
