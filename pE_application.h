
#define pELOG(fmt, ...) \
	fprintf(stderr, "[%s] [%s]: %d: " fmt "\n", \
			LOG_TAG, __func__, __LINE__, ##__VA_ARGS__);

void gdb_run(char *str);
