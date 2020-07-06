#include "config.h"

#if defined(WIN32)
  #include "libclipboard/src/clipboard_win32.c"
#else
  #include "libclipboard/src/clipboard_x11.c"
#endif
