#ifndef __NEREON_UTIL_H__
#define __NEREON_UTIL_H__

/* UCL field structure */
struct ucl_field {
	const char *key;
	ucl_type_t ucl_type;

	void *value;
	size_t size;                 /* size is available only when value has string type */
};

#define parse_ucl_fields_ex(obj, fields) \
	parse_ucl_fields(obj, fields, sizeof(fields) / sizeof(struct ucl_field))

/* size bounded string copy function */
size_t strcpy_s(char *dst, const char *src, size_t size);

/* size bounded string copy function */
size_t strcat_s(char *dst, const char *src, size_t size);

/* get main program name */
const char *getprogname_s(void);

/* fill bytes with given char */
char *fill_bytes(char ch, size_t len);

/* read file contents */
size_t read_file_contents(const char *fpath, char **buf);

/* parse UCL fields */
int parse_ucl_fields(const ucl_object_t *obj, struct ucl_field *fields, int count);

#endif /* __NEREON_UTIL_H__ */
