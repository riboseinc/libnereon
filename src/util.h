#ifndef __MCONFIG_UTIL_H__
#define __MCONFIG_UTIL_H__

#ifndef HAVE_STRLCPY
/* size bounded string copy function */
size_t strlcpy(char *dst, const char *src, size_t size);
#endif

#ifndef HAVE_STRLCAT
/* size bounded string copy function */
size_t strlcat(char *dst, const char *src, size_t size);
#endif

#endif /* __MCONFIG_UTIL_H__ */
