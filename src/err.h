#ifndef __MCONFIG_ERR_H__
#define __MCONFIG_ERR_H__

void mconfig_set_err(const char *err_fmt, ...);
const char *mconfig_get_err(void);

#endif /* __MCONFIG_ERR_H__ */
