#ifndef ALTTIMEH
#define ALTTIMEH
#include "cbl_String.h"
extern struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf);
extern struct tm *localtime_r(const time_t *timer, struct tm *tmbuf);
extern wString asctimew(const struct tm *tm);
extern wString ctimew(const time_t *timer);
//extern char *_strdate(char *s);
//extern char *_strtime(char *s);
#define ALTTIMEH
#endif
