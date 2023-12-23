
int fb_write_str(char *buf, unsigned int len);
#define write(buf, len) fb_write_str((buf), (len))
