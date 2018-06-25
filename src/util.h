#ifndef __NEREON_UTIL_H__
#define __NEREON_UTIL_H__

/* size bounded string copy function */
size_t strcpy_s(char *dst, const char *src, size_t size);

/* size bounded string copy function */
size_t strcat_s(char *dst, const char *src, size_t size);

const char *getprogname_s(void);

char *fill_bytes(char ch, size_t len);

size_t read_file_contents(const char *fpath, char **buf);

#endif /* __NEREON_UTIL_H__ */
