#ifndef __NEREON_ERR_H__
#define __NEREON_ERR_H__

#ifdef NEREON_DEBUG
	#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "######DEBUG: %s:%d:%s(): " fmt, \
				__FILE__, __LINE__, __func__, ##args)
#else
	#define DEBUG_PRINT(fmt, args...)
#endif

void nereon_set_err(const char *err_fmt, ...);
const char *nereon_get_err(void);

#endif /* __NEREON_ERR_H__ */
