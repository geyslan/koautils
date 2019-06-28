#ifndef COMMON_H_
#define COMMON_H_

#define STR(x) #x
#define STRINGIZE(x) STR(x)

void app_set_name(char *path);
const char *app_get_name();

#endif /* COMMON_H_ */
