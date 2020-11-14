#include "config.h"

#if defined(_WIN32)
  #define LIBCLIPBOARD_BUILD_WIN32 1
  #include "libclipboard/src/clipboard_win32.c"
#else
  #define LIBCLIPBOARD_BUILD_X11 1
  #include "libclipboard/src/clipboard_x11.c"
#endif
