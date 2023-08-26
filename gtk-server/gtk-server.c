/**************************************************************************
*
* This is the infamous GTK-SERVER. The purpose of this program is to enable
* access to graphical GTK widgets for shell scripts and interpreted
* languages. This is realized by offering a streaming interface to GTK.
*
* Please read the documentation on how to use this program.
* The CREDITS file lists all people who have helped improving the GTK-server.
*
* Original idea and design by Peter van Eerten, September 2003 - June 2019
* Mail: peter@gtk-server.org
*
* This source has become a little bit messy. What started as a quick hack,
* grew out to a large program.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* See http://www.gnu.org/licenses/gpl.txt for the full version of this license.
*
* CHANGES GTK-SERVER 1.0
* ----------------------
* - BUILD 1:	. Intitial release
* - BUILD 2:	. Fixed bug in parsing GTK-function arguments with Trim_Spaces
* - BUILD 3:	. Removed the child process counter, the Linux TCP version will run standalone now,
*		  and an explicit kill is required.
* - BUILD 4:	. Show help even when configfile is not available
*		. Lines in configfile 'gtk-server.cfg' may be empty now
*		. Name of the external gtk-library is configurable now
* - BUILD 5:	. Changed the format of the gtk-server.cfg file
*		. Added callback signal "RELEASE"
* - BUILD 6:	. Added ability to return a LONG from a GTK-function
*   		. Code cleaning
* - BUILD 7:	. Enable logging facility
* - BUILD 8:	. A lot of code optimizations - source has become smaller
* - BUILD 9:	. Implemented workaround for floating point arguments
*		. Changed logfile name to 'gtk-server.log'
*
* CHANGES GTK-SERVER 1.1
* ----------------------
* - BUILD 1:	. Redesigned the callback mechanism, hence the new release number (1.1)
*		. Updated the Check_Exceptions routine with GTK2.x functions
*		. Changed Widget casting from GtkWidget to GtkObject
*		. Added ability to return a FLOAT from a GTK-function
* - BUILD 2:	. Improved logging facility; returned string will be logged as well
* - BUILD 3:	. Determine maximum clients as an argument to the gtk-server (Linux)
* - BUILD 4:	. Look for global gtk-server.cfg as well (Linux: /etc directory)
* - BUILD 5:	. Path for logfile is configurable now in the gtk-server.cfg file
* - BUILD 6:	. Removed the GTK_LIB_SIGNAL keyword, signals can be configured directly now.
*		. Also search the /usr/local/etc directory for a configfile
* - BUILD 7:	. Added code to compile a self-containing GTK-server with all the GTK libs included
*		. Fixed compile problem with GCC 2.96 Redhat 7.1 (thanks Larry Richardson)
* - BUILD 8:	. Fixed some illegal memory writes and leaks pointed out by Valgrind
*		. Changed Win32 functionality: server will kill itself if a client
*   		  script disconnects
* - BUILD 9:	. Named pipe support (Unix/Linux)
*		. Some small code optimizations
* - BUILD 10:	. Support for named pipes in Win32
* - BUILD 11:	. Support for STDIN in Win32 (CLisp, Prolog, VB Script)
* - BUILD 12:	. Workaround for passing strings containing comma's as argument - introducing
*   		  the GTK_SERVER_COMMA config option
*   		. Added some GTK2 float functions
*   		. Win32 logic for finding the configfile
* - BUILD 13:	. Improved Named Pipe support for Windows
*		. Fixed potential bug with creating the logfile
* - BUILD 14:	. Introducing environment variable GTK_SERVER_CONFIG to point to configfile (WIN + LINUX)
*		. The Win32 version is not console based anymore. Adjusted all error warnings
*		  to messageboxes.
* - BUILD 15:	. Solved crash when GTK_SERVER_COMMA option is not present
*   		. Installer for Win32
*		. Autoconf source for Linux
* - BUILD 16:	. Implemented UDP communication interface: gtk-server udp <host:ip> [log]
*		. Argument syntax for TCP has changed: gtk-server tcp <host:ip[:max]> [log]
*		. Improved Makefile for Windows
* - BUILD 17:	. Added float functions for GTK2
*		. Corrected the Linux help prompt
*		. Removed redundant code
* - BUILD 18:	. Centralized version indication of GTK-server
*		. Added internal 'gtk_server_version' command to retrieve version
*		. If a LONG argument contains letters it will switch to STRING automatically
*		. Some large code improvements
*		. Win32: always create a logfile to capture GTK errors
*		. Bugfix: the GTK-server will not crash anymore when sending empty arguments
*		. Bugfix: float values are parsed correctly now in Win32 environments (Linux was already working)
* - BUILD 19:	. Linux: FIFO communication will return result string with newline attached
*		. Linux: improved FIFO to synchronous writing (added O_SYNC flag)
*		. Linux: Added experimental FILE interface (SLOW!)
*		. Win/Linux: Fixed bug with ')' symbol - introducing GTK_SERVER_BRACKET config option
*		. Linux distribution provides convenient KILL script to kill hanging gtk-servers
*
* CHANGES GTK-SERVER 1.2
* ----------------------
* - BUILD 1:	. Major changes in callback functionality by ideas of mr. Joe Armstrong; hence new releasenumber
*		. Ability to add extra signals per widget, each with own identifier
*		. Bugfix: returning empty STRING from GTK widget does not crash GTK-server anymore
*		. Bugfix: do not crash with '-1' as widget reference
*		. Improvements in the GTK-server KILL-script (Linux)
* - BUILD 2:	. The callback function only returns registered events now if called with 'WAIT'
*   		. The callback function now knows asynchronous "update" as argument
*   		. Generate error message when arguments to GTK-server are not recognized
*   		. Bugfix: reading CONFIG environment variable (Linux)
*		. More improvements in the GTK-server KILL-script (Linux)
*		. HOTFIX: STDIN interface can read newlines now
* - BUILD 3:	. Improved configure script to force compilation with GTK2
*		. Corrected 'gtk-server.cfg' search order for Linux
*		. Corrected format of logging with TCP and UDP
*		. The GTK-server now compiles with -pedantic flag and is ISO-C compatible (Linux)
*		. Added scripts to create sourcepackage, Slackware installpackage and SRC rpm
*
* CHANGES GTK-SERVER 1.3
* ----------------------
* - BUILD 1:	. Major changes in passing strings to GTK-server; concept by Norman Deppendbroek, parser
*		  created by Jim Bailey, slightly modified for GTK-server (me)
*		. The configoptions 'GTK_SERVER_COMMA' and 'GTK_SERVER_BRACKET' are redundant now
*		. Improved Trim_String function by Jim Bailey
*		. Linux: search for configfile '.gtk-server.cfg' in homedirectory of user also
* - BUILD 2:	. Fixed severe memory leak in callback routines
*		. GTK-server should compile without problems on FreeBSD now
* - BUILD 3:	. Finalized complete solution for FLOAT arguments
*		. Support for opening extra library with GTK_LIB_EXTRA
*		. Removed the experimental FILE interface
* - BUILD 4:	. Fixed compile warning with GTK1.x
*		. Fixed environment variable mixup (Felix Winkelmann, Sebastiaan van Erk)
*		. Fixed STDIN loop exit problems (Sebastiaan van Erk)
*		. Changes in MAN pages
*		. Changes in static Makefile
*		. Removed FILE interface from GTK-server KILL script
*		. Updated gtk-server.cfg file to latest
*
* CHANGES GTK-SERVER 2.0
* ----------------------
* - BUILD 1:	. Major changes in passing arguments and realizing GTK functions. Using FFI now, which
*		  also solves the FLOAT problem structurally
*		. Ability to capture incoming values in callback function
*		. Improved the Print_Error routine
*		. All error warnings go to the same function now
* - BUILD 2:	. Fixed erronuous Errormessages with DLOPEN
*		. Added floattype to FFI structure for compatibility with GTK 1.x
*		. Fixed compilewarnings for sprintf and vsprintf on OpenBSD
*		. Fixed issue with 'strtof' on OpenBSD
*		. Fixed unresolved PTHREAD symbol on OpenBSD
*		. Applied patches to obey --prefix flag (Peter Bui)
* - BUILD 3:	. Fixed bug with listening to extra signals (Woflgang Oertl)
*		. Up to 8 callback arguments can be retrieved now
*		. Implemented BOOL type, interchangable with LONG
*		. Returnbuffer is now of flexible size
*		. Option GTK_SERVER_ENABLE_C_STRING_ESCAPING to set escaping in returned strings (Jeremy Shute)
*		. Code cleanups
* - BUILD 4:	. Fixed minor bug with returning widgetID in "gtk_server_callback_value"
*		. Added support for mouse events with "gtk_server_mouse"
*		. Updated and corrected the manpages
* - BUILD 5:	. Added errormessage when returnvalue of GTK call in configfile is not correct
*		. Added GDK_LIB_NAME and GLIB_LIB_NAME in configfile
*		. Improved the Win32 Makefile
*		. Added "gtk_server_redefine" to change API calls on the fly
*		. Fixed OpenBSD compilewarning with sprintf
* - BUILD 6:	. Changed the signal connect functions to 'signal_connect_after'
*		. Fixed configure script to compile on Solaris
*		. Now the GTK-server in FIFO mode removes pipe file by itself at normal exit (Linux, BSD)
*		. Removed the O_SYNC from the FIFO interface, Linux does not need it and MacOSX cannot compile with it
*		. Code optimizations in the Call_Realize function
*		. Added LIBRARY interface -> GTK-server can be compiled as SO/DLL now
*		. GTK-server also can be compiled as S-Lang module
* - BUILD 7:	. GTK-server can be compiled as 64bit on AMD X64 now
*		. Fixed architecture recognition problems in configure script
*		. Added ScriptBasic module target
*		. Fixed bug with 'gtk_server_redefine' (Leonardo Cecchi)
*		. Added BASE64 type so client program can send binary data to GTK functions
*		. Added internal call 'gtk_server_echo' for debugging purposes
*		. When compiled as library the call 'gtk_server_logging' wil enable logfile
* - BUILD 8:	. Fixed bug when returning empty strings in C_ESCAPE mode (Todd Dukes)
*		. Fixed bug when returning special characters in C_ESCAPE mode (Leonardo Cecchi)
*		. Backslashes are also escaped now in C_ESCAPE mode (Leonardo Cecchi)
*		. Defined 'gtk_server_connect' and 'gtk_server_connect_after' as separate calls
*		. New: 'gtk_server_disconnect'
*		. Callback routines return FALSE by default now, so all signals can be captured by client program
*		. Basic support for GLADE files
*		. Added 'cfg' argument to point to location of configfile
*		. Arguments to gtk-server can be used in random order
*		. Format FIFO argument changed (Linux)
*		. Configure scripts will recognize MacOSX (Darwin) now (Leonardo Cecchi)
* - BUILD 9:	. XML calls return pointer to object
*		. New: 'gtk_server_glade_string' to create user interfaces from memory instead of file
*		. Callback routines return FALSE as option
*		. Fixed bug with 'gtk_server_callback update', now it catches callback arguments correctly
*		. Fixed bug: FIFO for Win32 works again
* - BUILD 10:	. New arguments 'pre' and 'post' to GTK-server determining the format of the returnstring (Christian Thaeter)
*		. Argument format 'tcp' and 'udp' to 'tcp=host:port' and 'udp=host:port'
*		. As LIBRARY pass to "gtk"-function: 'cfg=' for configfile, 'pre=' and 'post=' for format of returnstring
*		. String arguments can be grouped using single quotes (Christian Thaeter)
*		. Improved memory management with returnstrings containing STRING type
*		. Autoconnect signals in Glade XML if they are defined
* - BUILD 11:	. New: 'gtk_server_timeout' to let GTK return after some idle time in WAIT callback (async event)
*		. Improvements on the GtkScript interpreter (see GtkScript/docs/manual.txt)
*		. Compile with GLADE by default
*		. Improved memory management, cleaned up some ugly code
* - BUILD 12:	. Fixed bug with FLOAT and DOUBLE type mixup (thanks Norm Kaiser)
*		. Improved the configurescripts and generation of Makefile
*		. Windows version now only will create a logfle when "log" argument is given
*		. Introducing PANGO_LIB_NAME, ATK_LIB_NAME, PIXBUF_LIB_NAME, changed some configfile keywords to more logical words
*		. Code optimizations
*		. Improved documentation
*
* CHANGES GTK-SERVER 2.1
* ----------------------
* - BUILD 1:	. GTK-server now supports both libFFI and also FFCall, one of these should be available for compilation
*		. Banned Glib g_list routines from parser
*		. Added another backend: XForms (Linux/Unix/BSD only)
*		. Improved configure scripts even more, also put Glade support to autodetect
*		. In LIBRARY mode, removed the command 'gtk_server_logging', put it at same line as 'cfg=', 'post=', 'pre=' to be more consistent
* - BUILD 2:	. Added macro for 'snprintf' (lacks on Tru64Unix)
*		. Adapted configure scripts and implemented minor codefixes to avoid compilewarnings on Tru64Unix
*		. Code cleaning so compilation with Compaq C Compiler works now
*		. Improved KILL script, also added manpage
*		. Unix: improved gracefull exit in FIFO mode
*		. Some esthetic changes in sourcecode and GTK-server errors
* - BUILD 3:	. Unix: added IPC communication interface
*		. Unix: adjusted error warnings to messageboxes
*		. Unix: define exit signal to send to client program when GTK-server exits
*		. Win32: Improved configure-scripts to compile in MinGW natively
*		. Simplified signal handler registration for TCP
*		. Improved timeout handling, now more than one gtk_timeout can be created
*		. Timeouts to be removed with 'gtk_server_timeout_remove'
*		. New command 'gtk_server_exit'
*		. Added support for C/Invoke
*		. Fixed potential bug with opening GTK library
*		. Fixed bug with configure-scripts and GTK1 detection
*		. More code cleaning, simplification and beautification
* - BUILD 4:	. Fixed bug in demo scripts with Glade, also added demoscript with IPC
*		. Fixed bug in configure script when creating a Shared Object for Solaris
*		. Removed the GtkScript project from the sourcepackage
*		. Updated to the latest configfile
*		. Argument 'showconf' dumps current GTK-configuration to stdout
*		. Argument 'nocreate' avoids pipefile being created automatically (FIFO for Unix)
*		. Spawn to background automatically in Unix UDP, TCP, IPC and FIFO mode, this solves
*		  all timing issues
*		. New command 'gtk_server_pid' (Unix only)
*		. Small fixes in UDP error messages
*		. Argument 'nodetach' prevents spawning to background
*		. Fixed bug in Windows Print_Error routine
* - BUILD 5:	. Removed all Glade stuf, now Glade needs to be called just as GTK
*		. Code cleaning
* - BUILD 6:	. Fixed bug in BOOL returnvalue when compiled with C/Invoke
*		. Fixed bug when compiling with FFCALL, introducing DOUBLE as valid returnvalue
*
* CHANGES GTK-SERVER 2.2
* ----------------------
* - BUILD 1:	. Added GTK info in Windows about dialog
*		. Improved KILL script for IPC
*		. Improved configure scripts
*		. Added 'build' script (Unix)
*		. Fixed the 'create_distro' buildscript to create TGZ packages (Slackware, Gentoo, Zenwalk)
*		. New call for getting key events: gtk_server_key
*		. Ability to define macros in the configfile, hence new version number
*		. Trim_String now also removes '\r'
*		. gtk_server_disconnect now always works (previously only in a callback function)
*		. The default signal connection handler now sends the signal name as a string
*		. Fixed installmodes for configfile and manpages
* - BUILD 2:	. Fixed compile error with XForms
*		. Many small fixes in the demoscripts, added AWK dict client
*		. Fixed error messages in macro parser
*		. Support for scrollbutton events (GTK2.x)
*		. New: 'gtk_server_property_get/-set' to retrieve and set property values of widgets
*		. Check on duplicate function definitions in configfile during startup
*		. Arguments preceding with dash is also accepted now, e.g. 'log' and '-log' etc.
*		. Corrected help information
*		. Updated to the latest configfile
*		. Fixed bug with macro variables initialization
* - BUILD 3:	. Macros can assign strings to variables
*		. If a GTK function returns a result in pointer arguments, these can be retrieved now (request by Ben Kelly)
*		. Also all value types can be passed as pointers
*		. Check: when GTK call expects argument and client doesn't pass one
*		. 'gtk_server_property_get/-set' redundant, use 'g_object_get'
*		. New: 'gtk_server_set_c_string_escaping' to define particular set of characters to be escaped
*		. New: 'gtk_server_define' to define new GTK calls on-the-fly
*		. Fixes in the autoconf file for 64bit library compilation
*		. Improved Scriptbasic makefile
*		. New: 'gtk_server_opaque' to define memory for widgets like GtkIter, GdkColor etc. (request by Jean-Marc Farinas)
*		. Improved manpages
*		. Set the correct libraries in the configfile during installation (BSD/Unix/Linux)
*		. Fixed bug with C/Invoke and doubles as returnvalue
*		. Fixed bug with FFI and doubles as returnvalue (bug not noticable on X86 platforms)
*		. Updated to the latest configfile
* - BUILD 4:	. Moved to UTHASH library, fixing the following problems:
*		    - Fixed slow lookup performance with large configfile
*		    - Fixed crash when no macro definitions were found in configfile
*		    - Fixed crash when no GTK definitions were found in configfile
*		. Fixed potential bug: memset in TCP and UDP interface to secure size
*		. Fixed some illegal memory writes in macro routines (Valgrind)
*		. Fixed some illegal memory writes in gtk_server_define/-redefine (Valgrind)
*		. Added tool to extract configfile from GTK/GDK headers
*		. Fixed crash during parsing configfile when arguments of GTK functions were not defined
*		. Fixed crash during parsing configfile when logfile was not defined
*		. Implemented flexible mechanism for defining libraries, now anything can be added to a max of 32
*		. Fixed EXIT handling with 'gtk_server_exit' and STDIN interface
*		. Fixed crash when a defined library could not be opened
*		. Return type 'ADDRESS' returns the address in memory of function
*		. Fixed bug in IPC interface when sent string contains a '=' (bug found by Tim Launchbury)
*		. Added 'INCLUDE' keyword in configfile to include additional configfiles (request by John Spikowski)
*		. Within macros it is possible to perform relative jumps: 'VALUE', 'EMPTY', 'JUMP'
*		. General improvements in macroparser, starting 'Highlevel Universal GUI' (HUG) in configfile
*		. Removed EXIT_SIGNAL from configfile, specify 'signal=' on commandline
*		. Cleaned up exit handling, works better with error exit and FIFO now
*		. Documentation: improved manpages, added more demoscripts
* - BUILD 5	. Fixed crash when no libs were defined in configfile (bug found by Joonas Pihlaja)
*		. Fixed crash when empty string was sent to UDP interface (bug found by Joonas Pihlaja)
*		. Fixed deadlock in UDP interface because hostent was never initialized properly (bug found by Joonas Pihlaja)
*		. Fixed static compilation: optimized code, Makefile was missing linker flag '-export-dynamic'
*		. Static binary does not need configfile anymore, can define GTK calls on-the-fly
*		. Get LONG or STRING with 'gtk_server_callback_value'
*		. Support for Kornshell93 library interface
*		. Fixed crash when empty string was sent to LIB interface
*		. Also show libraries and log file location with '-showconf'
*		. Fixed issues with removing message queues and pipefiles in stopscript
*		. Added support for readline with interactive STDIN interface
*		. Fixed crash in OpenBSD 4.3 when libs could not be found
*		. Removed check that WidgetID should be > 0, does not work with OpenBSD 4.3
*		. Improved Makefile.in and configure.in for OpenBSD
*		. Some minor code improvements to clear compilewarnings on OpenBSD
*		. Security improvements on finding configfile
*		. Fixed bug in c_string_escaping after redefinition of chars to be escaped
*		. More improvements in stopscript. Compatible with BSD-type systems now
* - BUILD 6	. Added demoscript with the Poppler library, added demoscripts demonstrating HUG
*		. Stopscript now also can show current running GTK-server processes
*		. Added configurescript options to specify sourcetree locations
*		. Option 'showconf' also shows defined macros
*		. New: 'gtk_server_require' to test actual availability of libraries and define libraries on-the-fly
*		. Fixed bug: compilation with C/Invoke did not open XForms library
*		. Code improvements in the macroparser
*		. Major improvements in the GUI abstraction layer "HUG"
*		. Macros can define associative arrays with ASSOC, retrieve values with GET
*		. Fixed configurescript complaints about missing 'datarootdir'
*		. Logdirectory definition removed from configfile, now specify logdir as argument '-log=<dir>', better
*		  consistency with other options, and GTK-server can function completely without configfile now
*		. Macros: added DEBUG to print contents of macrovariables in logfile
*		. Static compilation also possible for XForms now
*		. Fixed duplicate hash-value which occurs in rare situations
*		. Compilation with 'TCC' possible (export CC=tcc)
*		. Fixed bug: when using GTK-server as shared object and enabling logging
* - BUILD 7	. Bugfix: gtk_server_require would overwrite library statically compiled in
*		. Minor fix in 'gen_conf' script so it does not add logdir anymore
*		. Added 'libm' to default configfile, also some of it's math functions (easy for shellscripts)
*		. Added ability to define enumerations 'ENUM_NAME' in configfile
*		. XForms: improved demoscripts, added OpenGL demoscripts
*		. XForms bugfix: initialize library before the first Print_Error occurs
*		. Corrected definition of 'glTranslatef' in configfile, compilation with FFI revealed this bug
*		. Added librarypath when compiling for 64bit platform (XForms)
*		. Put code in place for static compilation in Win32
*		. Removed workaround code to fallback to STRING if LONG was not detected (use 'gtk_server_redefine' instead)
*		. Upgraded to uthash 1.3 (uthash.sourceforge.net)
*		. Make strong differentiation between LONG and INT (important for Win32 platforms)
*		. Generate error when argument type is not recognized
*		. Bugfix: static compilation with GTK1/Xforms using C/Invoke crashed because of uninitialized context
*		. Hotfix: on 64-bit platforms explicitely cast to 'INT' with gtk_server_callback_value
*		. Hotfix: Valgrind detects illegal write of 1 byte in macro parser
*		. Hotfix: initialize max arguments to NULL for each new GTK function, else redefintion may crash
* - BUILD 8	. Added support for DynCall (www.dyncall.org)
*		. Fixed potential bug in defining new GTK functions with 'gtk_server_define'
*		. Removed 'ok' from returnstring when PTR_* argument contains result
*		. VOID is also legal returnvalue for function
*		. POINTER as valid argumenttype and returnvalue for generic pointers from GLib calls and some non-GTK libs
*		. Returnvalue of 'gtk_server_require' will give 'ok', or errorstring (for debugging)
*		. Less memory usage in PTR_* return code
*		. Macros can use COMPARE to compare variables
*		. Improved some errormessages, added more function definitions to configfile
*		. Fixed bug with LONG typecast and FFCALL
*		. Fixed bug with some erronous array declarations
*		. Added possibility to register GtkDataFuncs (user functions), introducing 'MACRO' and 'DATA' as argument type
*		. Check on duplicate macro names during startup
*		. Hotfix: no space between results when GTK function returns value and also uses PTR_ arguments
*		. Hotfix: removed unnecessary free in Print_Error routine for Win32
*
* CHANGES GTK-SERVER 2.3
* ----------------------
* - BUILD 1	. Added 'gtk_server_toolkit' command to retrieve backend for which GTK-server was compiled
*		. Arguments 'MACRO'-'DATA' are also usable for Xforms now
*		. Make sure the 'MACRO'/'DATA' callbacks are not reading from illegal memory
*		. Fixed minor bug with counting linenumbers when parsing configfile
*		. Added option to define aliases for functionnames, so clientprograms can avoid collisions with existing statements
*		. STDIN, FIFO, TCP and IPC interfaces can take and return any amount of data now
*		. The UDP interface can take data as much as the system's Socket receive buffer size (SO_RCVBUF)
*		. Fixed minor issues in stop-script with IPC GTK-server processes
*		. 'gtk_server_macro_define'/'-redefine' to (re-)define macros on the fly, renamed 'gtk_server_macro' to '-_var' to get value of variables
*		. Added '-sock' mode to connect as TCP client (idea Danie Brink)
*		. Withdrawn the READLINE option as scripts using STDIN ran into inconsistent programs, use external 'rlwrap' instead
*		. Upgraded to uthash 1.4 (uthash.sourceforge.net)
*		. Major code cleanup for returning info to client, improving simplicity, readability and performance
*		. Added commandline option '-start' to start GTK-server with a macro (idea Danie Brink)
*		. 'gtk_server_os' to determine platform where GTK-server is running (idea Danie Brink)
*		. Added commandline option '-init' to send string in '-sock' mode (idea Danie Brink)
*		. Added support for SSL in '-sock' mode (idea Danie Brink)
*		. Using the GTK functionnames as keys for UTHASH; less code, best reliability for unique keys
*		. Fixed bug with XForms initialization in LIB mode
*		. Base64 decoder can decode unlimited amount of data
*		. Fixed bug: using 'gtk_server_require' multiple times would overwrite eachother
*		. Added option '-this' to run standalone configfile using shebang
*		. Compilation without GTK/Xforms/X is possible now, console mode for opening any library in a script
*		. Added '-handle' so client can send unique identifier per request (synchronize communication, UDP, debugging)
*		. XForms can get key, mouse events and coordinates too
*		. Adjusted size for XForms Error dialog, it didn't show all errors correctly
*		. Key events and scrollbutton events (GTK) were optimized so they can return any user defined string
*		. Fixed bug: remove the check on negative numbers in LIB interface, this check prevented UTF-8 to be accepted
*		. MacOSX: adjustments in makefile and configurescripts
*		. MacOSX: changed retrieval of mouse button, MacOSX does not pass all GdkModifierTypes (no X but Quartz!)
*		. MacOSX, BSD, Solaris: self-spawning not always works, now use '-detach' to try to selfspawn, shells use ampersand '&'
*		. Corrections in documentation and manpages (2.3.1 Second Release)
*		. Standardized all 'config.h' macros because of conflict with newer GCC versions (2.3.1 Second Release)
*		. Cleaned up redundant C macros (2.3.1 Second Release)
*		. Fixed compilation problems with TCC compiler (2.3.1 Second Release)
*		. Corrected OpenGL libraries in configfile for MacOSX (2.3.1 Second Release)
*		. More fixes in manpages (2.3.1 Second Release)
*
* CHANGES GTK-SERVER 2.4
* ----------------------
*		. Added support for GTK3
*		. Linking could fail because '-ldl' was missing from linker flags
*		. Added support for querying state keys from keyboard - 'gtk_server_state'
*		. Fixed issue with displaying correct version in Windows
*		. Upgraded to uthash 2.0.1 (https://troydhanson.github.io)
*		. The ALIAS_NAME option now can alias macro names also
*		. Enlarged memory for 'gtk_server_opaque'
*
* CHANGES GTK-SERVER 2.4.1
* ------------------------
*		. Fixed compile warning for Xforms
*		. XForms can use enumerations in callback definition
*		. More intelligence in searching and opening defined libraries
*		. Added '-debug' parameter to activate interactive debugging
*		. Lots of fixes in autoconf macros and demonstration scripts
*		. Fixed GTK3 warning with error dialogue.
*
* CHANGES GTK-SERVER 2.4.2
* ------------------------
*		. Fixed compile warning with GCC 4.8
*		. Improved 'gtk_server_os' command
*		. Cleanup code
*		. Added '-nonl' parameter to prevent GTK-server adding newline to responses.
*		. Added Pause button to debug panel
*		. Support for Motif
*
* CHANGES GTK-SERVER 2.4.3
* ------------------------
*		. NULL is a valid entry for POINTER arguments
*		. NULL is a valid entry for WIDGET arguments
*		. Fixed crash when returned string is empty (thanks report Thomas Ronshof)
*		. Updated all demo programs to point to correct GTK-server binary
*               . Maximum library sequence number is configurable in config file
*               . Bug fixes in GTK-server configfile macros for LIST widget
*
* CHANGES GTK-SERVER 2.4.4
* ------------------------
*		. Added 'gtk_server_pack' to create portable memory layouts for BASE64 argument type
*               . The configfile now has SEQUENCE enabled by default
*               . Simplified 'select-gtk-server' script
*               . Fixed bug in 'stop-gtk-server' script when using 'all' argument
*               . Added 'gtk_server_unpack' to unpack binary memory layout to s-expression
*               . Added PTR_BASE64 argument type and 'gtk_server_data_format' to unpack binary structures returned in arguments
*               . Added PTR_SHORT argument type
*		. Upgraded to uthash 2.0.2 (https://troydhanson.github.io)
*
* CHANGES GTK-SERVER 2.4.5
* ------------------------
*               . Updated configure script so it can use environment variables LDFLAGS and CFLAGS.
*               . Fixed compile warnings.
*               . Fixed compile warning in SSL usage.
*               . Bugfix in gtk_server_unpack so it can unpack values larger than a byte.
*               . New function 'gtk_server_unpack_from_pointer' from Jop Brown
*               . New function 'gtk_server_string_from_pointer' from Jop Brown
*		. Upgraded to uthash 2.1.0 (https://troydhanson.github.io)
*
* CHANGES GTK-SERVER 2.4.6
* ------------------------
*               . Fixed compile warning with GCC 7 and GCC 8.
*               . Changed 'GdkDeviceManager' to 'GdkSeat' for GDK 3.20 and higher.
*               . Fixed issue with connecting and disconnecting signals.
*               . New functions 'gtk_server_enable_print_line_count' and 'gtk_server_disable_print_line_count' by 10geek
*               . Fixed errors in C string escaping by 10geek
*               . New function 'gtk_server_enum' to obtain ad-hoc ENUM value (idea 10geek)
*               . Fixed compile warnings with FFCall 1.3 and Cinvoke 1.0
*               . Added runtime check for GTK to avoid functions being imported from wrong GTK library (MAJOR version mismatch)
*               . Added '@' construct so certain languages can pass strings containing spaces (eg Oberon)
*               . Debug panel now also works in GTK-server Library mode
*               . When compiling with OpenSSL3: enable legacy servers by default
*               . Fixed severe bugs in parsing arguments when in library mode
*               . Debug panel now also is available for GTK1
*               . Code cleaning at various places
*
*************************************************************************************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef GTK_SERVER_KSH93
#include <shell.h>
#endif

#ifdef GTK_SERVER_SLANG
#include <slang.h>
#endif

#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
#include <gtk/gtk.h>
#endif

#ifdef GTK_SERVER_GTK3x
#define gtk_exit exit
#define GTK_OBJECT G_OBJECT
#define GtkObject GObject
#endif

#ifdef GTK_SERVER_XF
#include <X11/XKBlib.h>
#include <forms.h>
#endif

#ifdef GTK_SERVER_MOTIF
#include <X11/XKBlib.h>
#include <Xm/XmAll.h>
#define gint int
#endif

#ifdef GTK_SERVER_USE_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <locale.h>
#include <unistd.h>
#include <ctype.h>
#include "uthash.h"

#ifdef GTK_SERVER_FFI
#include <ffi.h>
#elif GTK_SERVER_FFCALL
#include <avcall.h>
#elif GTK_SERVER_CINV
#include <cinvoke.h>
#elif GTK_SERVER_DYNCALL
#include <dyncall.h>
#include <dynload.h>
#endif

#ifdef GTK_SERVER_UNIX
#include <netdb.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <signal.h>
#include <dlfcn.h>
#include <stdarg.h>
/* Needed for FIFO files */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* Needed for IPC msg queue */
#include <sys/ipc.h>
#include <sys/msg.h>
#define FUNC_POINTER void (*func)()
#endif

#ifdef GTK_SERVER_BSD
#include <netinet/in.h>
#endif

#ifdef GTK_SERVER_SCRIPTBASIC
#include <basext.h>
#endif

#ifdef GTK_SERVER_WIN32
#include <strings.h>
#include <winsock.h>
#include <windows.h>
#include <shlwapi.h>
#define FUNC_POINTER FARPROC *func
#endif

#ifndef LOCALCFG
#define LOCALCFG "/usr/local/etc/gtk-server.cfg"
#endif

/* Max length of a STDIN coming from script or configfile */
#define MAX_LEN 1024

/* how many libs we may open and check */
#define MAX_LIBS 64

/* Define how many arguments may be used in a GTK function */
#define MAX_ARGS 32

/* Maximum digits to return */
#define MAX_DIG 32

/* Define callback signals */
#define GTK_SERVER_NONE 0

/* Define GTK-server version - macro 'VERSION' also used by FFI on some platforms */
#define GTK_SERVER_VERSION "2.4.6"

/* Define backlog for tcp-connections */
#define BACKLOG 4

/* Define printable length of LONG type - use same type as returntype of the Widget_GUI() function */
#define LONG_SIZE 32

/* Verify if a pointer is valid - taken from 'jkr' from www.experts-exchange.com */
#ifdef GTK_SERVER_UNIX
struct stat sb;
#define illegal_pointer(p) (stat(p, &sb) == -1 && errno == EFAULT)
#elif GTK_SERVER_WIN32
/* http://msdn.microsoft.com/en-us/library/aa366713(VS.85).aspx */
#define illegal_pointer(p) IsBadReadPtr(p, MAX_LEN)
#endif

/* Opening for different FF toolkits */
#if GTK_SERVER_FFI || GTK_SERVER_FFCALL
#define OPENLIB(x) dlopen(x, RTLD_LAZY)
#elif GTK_SERVER_CINV
#define OPENLIB(x) cinv_library_create(cinv_ctx, x)
#elif GTK_SERVER_DYNCALL
#define OPENLIB(x) dlLoadLibrary(x)
#endif

/* Obtaining function for different FF toolkits */
#if GTK_SERVER_FFI || GTK_SERVER_FFCALL
#define FROMLIB(x,y) dlsym(x,y)
#elif GTK_SERVER_CINV
#define FROMLIB(x,y) cinv_library_load_entrypoint(cinv_ctx, x, y)
#elif GTK_SERVER_DYNCALL
#define FROMLIB(x,y) dlFindSymbol(x, y)
#endif

/* Closing for different FF toolkits */
#if GTK_SERVER_FFI || GTK_SERVER_FFCALL
#define CLOSELIB(x) dlclose(x)
#elif GTK_SERVER_CINV
#define CLOSELIB(x) cinv_library_delete(cinv_ctx, x)
#elif GTK_SERVER_DYNCALL
#define CLOSELIB(x) dlFreeLibrary(x)
#endif

#ifdef GTK_SERVER_GTK1x
#define SIGNALCONNECT(x, y, z, a) gtk_signal_connect(GTK_OBJECT(x), y, GTK_SIGNAL_FUNC(z), (gpointer*)a)
#elif GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
#define SIGNALCONNECT(x, y, z, a) g_signal_connect(GTK_OBJECT(x), y, G_CALLBACK(z), (gpointer*)a)
#endif

/* Define structure containing configuration data */
typedef struct config_struct {
    char *name;
    char *callbacktype;
    char *returnvalue;
    char argamount[MAX_DIG];
    char *args[MAX_ARGS];
    UT_hash_handle hh;       /* makes this structure hashable */
} CONFIG;

/* Needed for hashtable */
struct config_struct *gtk_protos = NULL;

/* Define structures to keep macro definitions */
typedef struct macro_body {
    char *text;			/* Keep a line of the macro */
    struct macro_body *next;	/* Pointer to next line */
    struct macro_body *prev;	/* Pointer to previous line */
} BODY;

typedef struct macro_struct {
    char *name;			/* Name of the macro */
    char *args[10];		/* Up to 10 arguments for a macro */
    char *var[26];		/* Values of 26 variables used in macro */
    struct macro_body *body;	/* Pointer to body of the macro */
    UT_hash_handle hh;       /* makes this structure hashable */
} MACRO;

/* Needed for hashtable */
struct macro_struct *macro_protos = NULL;

/* Define structure to define enumerations */
typedef struct enum_struct {
    char *name;			/* Name of the enum */
    int value;			/* Actual value of the enum */
    UT_hash_handle hh;       /* makes this structure hashable */
} ENUM;

/* Needed for hashtable */
struct enum_struct *enum_protos = NULL;

/* Define structure to define enumerations */
typedef struct str_struct {
    char *name;			/* Name of the string */
    char *value;		/* Actual value of the string */
    UT_hash_handle hh;       /* makes this structure hashable */
} STR;

/* Needed for hashtable */
struct str_struct *str_protos = NULL;

#ifdef GTK_SERVER_MOTIF
/* Define structure to define enumerations */
typedef struct class_struct {
    char *name;			/* Name of the class */
    void* ptr;			/* Actual pointer to the class */
    UT_hash_handle hh;       /* makes this structure hashable */
} CLASS;

/* Needed for hashtable */
struct class_struct *class_protos = NULL;
#endif

/* This defines assocs created in configfile macro's */
typedef struct macro_assoc_struct {
    char assoc[LONG_SIZE];	/* The associated widgetID */
    char *key;			/* Holder for the key of the assoc */
    UT_hash_handle hh;		/* makes this structure hashable */
} MACRO_ASSOC;

/* Needed for hashtable */
struct macro_assoc_struct *macro_assoc_protos = NULL;

/* Define structure to define arguments of MACRO type */
typedef struct arg_struct {
    char *name;			/* Name of the macro to execute */
    char *data;			/* Additional data */
    UT_hash_handle hh;       /* makes this structure hashable */
} ARG;

/* Needed for corresponding hashtable */
struct arg_struct *arg_protos = NULL;
ARG *Macro_Type = NULL;

/* Define structure to define function aliases */
typedef struct alias_struct {
    char *name;			    /* Name of the alias */
    struct config_struct *real;	    /* Pointer to the real function name */
    struct macro_struct *real_m;    /* Pointer to the real macro name */
    UT_hash_handle hh;		    /* makes this structure hashable */
} ALIAS;

/* Needed for hashtable */
struct alias_struct *alias_protos = NULL;

/* Define union for FFI and CINVOKE */
typedef union {
    long		lvalue;
    int			ivalue;
    float		fvalue;
    double		dvalue;
    char		*pvalue;
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
    GtkObject *wvalue;
    #elif  GTK_SERVER_GTK3x
    GtkWidget *wvalue;
    #elif GTK_SERVER_XF
    FL_OBJECT *wvalue;
    #elif GTK_SERVER_MOTIF
    Widget wvalue;
    #else
    void *wvalue;
    #endif
    /* These are needed for GTK functions which use pointer arguments
	to return results in */
    long		*p_lvalue;
    int			*p_ivalue;
    short		*p_hvalue;
    float		*p_fvalue;
    double		*p_dvalue;
    char		*p_svalue;
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
    GtkObject	*p_wvalue;
    #elif GTK_SERVER_GTK3x
    GtkWidget	*p_wvalue;
    #elif GTK_SERVER_XF
    FL_OBJECT	*p_wvalue;
    #elif GTK_SERVER_MOTIF
    Widget	p_wvalue;
    #else
    void *p_wvalue;
    #endif
} TYPE;

/* Struct to pass data to callback function for timeouts */
typedef struct {
    long widget;
    char *signal;
    unsigned int id;
} TIMEOUT;

/* Define current 'callbacked' object */
struct callback {
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
    GtkWidget *object;
    int button;
    #elif GTK_SERVER_XF
    FL_OBJECT *object;
    unsigned int button;
    #elif GTK_SERVER_MOTIF
    Widget object;
    int button;
    #else
    void *object;
    int button;
    #endif
    long state;
    int p1;
    int p2;
    int p3;
    int p4;
    int p5;
    int p6;
    int p7;
    char *text;
    char* t1;
    char* t2;
    char* t3;
    char* t4;
    char* t5;
    char* t6;
    char* t7;
    int mousex;
    int mousey;
    int key;
    int key_state;
    int scroll;
};

/* Define global instance of current callback */
struct callback Current_Object;

/* Structure to define the behaviour of GTK-server */
struct behaviour {
    int count_fork;		/* Keeps track of forking when in TCP mode */
    int c_escaped;		/* Define if returned strings will have escaped special characters 0=off, 1=on, 2=string */
    unsigned char escapes[16];	/* Keep chars to escape, maximum 16 */
    int print_line_count;		/* Print line count before returnstring */
    char *fifo;			/* Keep name of FIFO file (Linux) */
    char *tcp;			/* Keep name of TCP data when in TCP mode */
    char *udp;			/* Keep name of UDP data when in UDP mode */
    char *LogDir;		/* Location of the logfile */
    char *pre;			/* Additional text to put before the returnstring */
    char *post;			/* Additional text to put after the returnstring */
    char *macro;		/* Only used when GTK-server needs to start with a macro */
    char *init;			/* Used when GTk-server needs to send an INIT string as TCP client */
    char *certificate;		/* Location of the SSL certificate to identify ourselves when connecting */
    char *ca;			/* Location of the trusted CA (Certificate Authority) */
    char *password;		/* Password to decrypt SSL certificate */
    char *handle;		/* Keep handle of current request */
    int ipc;			/* The signal number to use when in IPC mode */
    int exit_sig;		/* Keep exit signal to send to PPID */
    int ppid;			/* Keep PID of parent */
#ifdef GTK_SERVER_MOTIF
    XtAppContext app;		/* Motif app context */
    Widget toplevel;		/* Motif widget toplevel */
#endif
    int behave;			/* Binary flag
				    -list of configured calls	00000000001
				    -do not create FIFO file	00000000010
				    -do not spawn to backgrnd	00000000100
				    -send signal on exit	00000001000
				    -run a macro first		00000010000
				    -send INIT string		00000100000
				    -use SSL encryption	wo cert	00001000000
				    -use SSL encryption	& cert	00010000000
				    -use check on handle	00100000000
				    -use check on debug		01000000000
				    -disable adding of newline  10000000000 */
    int mode;			/* Are we running in STDIN, FIFO, IPC, TCP, UDP mode? */
    int libseq;			/* Sequence numbering for library */
};

/* Global instance to hold the behaviour of GTK-server */
struct behaviour gtkserver;

/* Define list to parse a line */
typedef struct parsed_data {
    char *arg;
    struct parsed_data *next;
} PARSED;

/* Struct for message queues */
struct msg_buf {
    long mtype;
    char mtext[MAX_LEN];    /* MSGMNB is the max size, MAX_LEN must be a lower value */
};

typedef struct wid_sig {
    #if GTK1 || GTK2
    GtkWidget* widget;
    #elif GTK_SERVER_XF
    FL_OBJECT *widget;
    #elif GTK_SERVER_MOTIF
    Widget widget;
    #endif
    char* data;
    struct wid_sig *next;
} ASSOC;

ASSOC *List_Sigs = NULL;
ASSOC *Start_List_Sigs = NULL;

/* These variable names are addresses by themselves and used by GTK to store returnvalues */
long long_address[MAX_ARGS];
int int_address[MAX_ARGS];
short short_address[MAX_ARGS];
float float_address[MAX_ARGS];
double double_address[MAX_ARGS];
char *str_address[MAX_ARGS];
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
GtkObject *obj_address[MAX_ARGS];
#elif GTK_SERVER_GTK3x
GtkWidget *obj_address[MAX_ARGS];
#if GTK_MINOR_VERSION<20
#define DEV_MANAGER do { \
    GdkDeviceManager *device_manager = gdk_display_get_device_manager(gdk_window_get_display(gtk_widget_get_window(widget))); \
    gdk_window_get_device_position(gtk_widget_get_window(widget), gdk_device_manager_get_client_pointer (device_manager), &Current_Object.mousex, &Current_Object.mousey, NULL); \
} while(0)
#else
#define DEV_MANAGER do { \
    GdkSeat *device_manager = gdk_display_get_default_seat(gdk_window_get_display(gtk_widget_get_window(widget))); \
    gdk_window_get_device_position(gtk_widget_get_window(widget), gdk_seat_get_pointer(device_manager), &Current_Object.mousex, &Current_Object.mousey, NULL); \
} while(0)
#endif

#elif GTK_SERVER_XF
FL_OBJECT *obj_address[MAX_ARGS];
#elif GTK_SERVER_MOTIF
Widget obj_address[MAX_ARGS];
#else
void *obj_address[MAX_ARGS];
#endif

/* Debug panel variables for both standalone and lib version of GTK-server */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
GtkWidget *debug_view;
#endif
#if GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
GtkTextBuffer *debug_buffer;
GtkTextIter debug_iter;
#elif GTK_SERVER_XF
char *xf_buffer;
FL_OBJECT *debug_view;
#elif GTK_SERVER_MOTIF
char *motif_buf;
Widget debug_view;
#endif
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
long debug_step = 0, debug_run = 0;
#endif

/* Declare callback routines */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
void gtk_server_callback_extra2(GtkWidget *widget, gpointer *data1);
void gtk_server_callback_extra3(GtkWidget *widget, gpointer *data1, gpointer *data2);
void gtk_server_callback_extra4(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3);
void gtk_server_callback_extra5(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4);
void gtk_server_callback_extra6(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5);
void gtk_server_callback_extra7(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5, gpointer *data6);
void gtk_server_callback_extra8(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5, gpointer *data6, gpointer *data7);
gboolean gtk_server_callback_extra2_bool(GtkWidget *widget, gpointer *data1);
gboolean gtk_server_callback_extra3_bool(GtkWidget *widget, gpointer *data1, gpointer *data2);
gboolean gtk_server_callback_extra4_bool(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3);
gboolean gtk_server_callback_extra5_bool(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4);
gboolean gtk_server_callback_extra6_bool(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5);
gboolean gtk_server_callback_extra7_bool(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5, gpointer *data6);
gboolean gtk_server_callback_extra8_bool(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5, gpointer *data6, gpointer *data7);
#endif

/* Collect all callback functions for gtk_server_connect */
#ifdef GTK_SERVER_GTK1x
GtkCallback gtk_server_callbacks[] = {
    GTK_SIGNAL_FUNC(gtk_server_callback_extra2),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra3),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra4),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra5),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra6),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra7),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra8),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra2_bool),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra3_bool),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra4_bool),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra5_bool),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra6_bool),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra7_bool),
    GTK_SIGNAL_FUNC(gtk_server_callback_extra8_bool)
};
#elif GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
GCallback gtk_server_callbacks[] = {
    G_CALLBACK(gtk_server_callback_extra2),
    G_CALLBACK(gtk_server_callback_extra3),
    G_CALLBACK(gtk_server_callback_extra4),
    G_CALLBACK(gtk_server_callback_extra5),
    G_CALLBACK(gtk_server_callback_extra6),
    G_CALLBACK(gtk_server_callback_extra7),
    G_CALLBACK(gtk_server_callback_extra8),
    G_CALLBACK(gtk_server_callback_extra2_bool),
    G_CALLBACK(gtk_server_callback_extra3_bool),
    G_CALLBACK(gtk_server_callback_extra4_bool),
    G_CALLBACK(gtk_server_callback_extra5_bool),
    G_CALLBACK(gtk_server_callback_extra6_bool),
    G_CALLBACK(gtk_server_callback_extra7_bool),
    G_CALLBACK(gtk_server_callback_extra8_bool)
};
#endif

/* Define logfile facility */
FILE *logfile;

/* Translation Table as described in RFC1113 - Needed for Base64 encoding */
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/* Global to hold memory size for PTR_BASE64 */
int PTR_BASE64 = 0;

/* Function prototypes */
char *Call_Realize (char*, void*);
char *base64_enc(char *arg, int len);

#ifdef GTK_SERVER_LIBRARY

/* Define list for CONFIGURATION settings */
CONFIG *Gtk_Api_Config = NULL;
/* Define list for macro definitions */
MACRO *Macro_Defs;
MACRO *Macro_Last;
MACRO *Start_Macro_Defs;
BODY *Body_Text;
BODY *Body_Last;
/* Define list for enum definitions */
ENUM *Enum_Defs;
STR *Str_Defs;
#ifdef GTK_SERVER_MOTIF
CLASS *Class_Defs;
#endif
ALIAS *Alias_Defs;

#endif

/* Define the librarynames to open */
char *libs[MAX_LIBS];
#if GTK_SERVER_FFI || GTK_SERVER_FFCALL
    #ifdef GTK_SERVER_UNIX
    void *handle[MAX_LIBS];
    #elif GTK_SERVER_WIN32
    HINSTANCE handle[MAX_LIBS];
    #endif
    void *cinv_ctx;
#elif GTK_SERVER_CINV
CInvContext *cinv_ctx;
CInvLibrary *handle[MAX_LIBS];
#elif GTK_SERVER_DYNCALL
void *handle[MAX_LIBS];
void *cinv_ctx;
#endif

/*************************************************************************************************/

/* This function was rewritten by Jim Bailey */

char *Trim_String(char *data)
{
int last_idx;
/* Get rid of whitespaces at the beginning of the string */
while (*data == ' '|| *data == '\t' || *data == '\n' || *data == '\r') data++;
/* Check there is anything left to chop off at the end */
last_idx = strlen( data ) - 1;
if ( last_idx < 0 ) return data;
/* Find the last non-whitespace at the end of the string */
while ( data[last_idx] == ' ' || data[last_idx] == '\t' || data[last_idx] == '\n' || *data == '\r') last_idx--;
/* Actually chop off the whitespaces at the end of the string */
data[last_idx+1] = '\0';
/* Return stripped string */
return data;
}

/*************************************************************************************************/

/* Check if a charpointer really contains a long number */

int is_value (char *var)
{
int i;

if (var != NULL){
    for (i = 0; i < strlen(var); i++){
	if ((*(var + i) < 48 || *(var + i) > 57) && *(var + i) != 32 && *(var + i) != 9 && *(var + i) != 45 && *(var + i) != 43) return 0;
    }
    return 1;
}
/* If NULL, value is not a number */
return 0;
}

/*************************************************************************************************/

#ifdef GTK_SERVER_NOSNPRINTF
void snprintf(char *result, int no, char *fmt, ...)
{
va_list args;

va_start(args, fmt);

vsprintf(result, fmt, args);

va_end(args);
}
#endif

/*************************************************************************************************/

void mystrcat(char** pointer, char *string)
{
    char *ptr;

    ptr = calloc(strlen(*pointer) + strlen(string) + MAX_LEN + 1, sizeof(char));
    strcpy(ptr, *pointer);
    strcat(ptr, string);
    free(*pointer);
    *pointer = ptr;
}

/**************************************************************************************************/
/* I wanted to use 'fmemopen' but this is not portable. Therefore this 'fmemgets' function to read
    a string as if it were a file. */

char *fmemgets(char *s, int size, char *stream)
{
char *nl;

/* Result buffer must be empty */
memset(s, '\0', size);

nl = stream;

/* Check if we are at the end of the string already */
if (nl == NULL || *nl == '\0') return NULL;

/* No, search for next \n or \0 */
while(*nl != '\0' && *nl != '\n') nl++;

if (nl - stream > size){
    strncpy(s, stream, size);
    stream += size;
}
else {
    strncpy(s, stream, nl - stream);
    stream = nl;
}

/* End of string? Return NULL */
if (*stream == '\0') return NULL;

/* Else return rest of the stream */
return ++stream;
}

/*************************************************************************************************/
/* If the library is not found, add a numeric suffix from 0-99 behind the soname. A larger range */
/* slows down the GTK-server significantly. PvE. */

#if GTK_SERVER_CINV
CInvLibrary *search_and_open_lib(char *name, CInvContext *cinv_ctx)
{
    CInvLibrary* handle = NULL;
#elif GTK_SERVER_FFI || GTK_SERVER_FFCALL || GTK_SERVER_DYNCALL
void *search_and_open_lib(char *name)
{
    void* handle = NULL;
#endif
    int len, j;
    char *buf;
    char suffix[5];
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
    char* (*check_gtk_version)(int, int, int) = NULL;
    #endif

    if(name != NULL) {

	/* Attempt to open current library name first */
        handle = OPENLIB(name);

        /* Opening fails, now start search */
	if(handle == NULL){

	   /* Unlikely but you never know */
	    len = strlen(name);
	    if(len > MAX_LEN-5) {
		return(NULL);
	    }

	    /* Leave space for numeric suffix */
	    buf = calloc(MAX_LEN, sizeof(char));
	    strncpy(buf, name, MAX_LEN-5);

	    /* Loop through library suffix numbers */
	    for(j = 0; j < gtkserver.libseq; j++) {

		buf[len] = '\0';
		if(snprintf(suffix, 5, ".%d", j) == 5) {
                        suffix[4] = '\0';
                }
		strncat(buf, suffix, 5);
                buf[MAX_LEN-1] = '\0';

                handle = OPENLIB(buf);
		if (handle != NULL) {
		    break;
		}
	    }
	    free(buf);
	}
    }
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
    if(handle) { *(void**)(&check_gtk_version) = (void*)FROMLIB(handle, "gtk_check_version"); }
    #endif

    #if GTK_SERVER_GTK1x
    if(check_gtk_version && check_gtk_version(1, 0, 0) != NULL) { CLOSELIB(handle); handle = NULL; }
    #elif GTK_SERVER_GTK2x
    if(check_gtk_version && check_gtk_version(2, 0, 0) != NULL) { CLOSELIB(handle); handle = NULL; }
    #elif GTK_SERVER_GTK3x
    if(check_gtk_version && check_gtk_version(3, 0, 0) != NULL) { CLOSELIB(handle); handle = NULL; }
    #endif

    return(handle);
}

/*************************************************************************************************/
/* When the GTK-server exits, remove the created pipe, but first send an 'OK'.	Oct 10, 2006	*/
/* Use print to stderr here, since we are already in the 'atexit' EXIT function. */

#ifdef GTK_SERVER_UNIX
void remove_pipe(void)
{
int sockfd;

/* Open in WRITE mode */
if (gtkserver.behave & 8)
    sockfd = open(gtkserver.fifo, O_RDWR|O_NONBLOCK);
else
    sockfd = open(gtkserver.fifo, O_WRONLY);

/* Return OK */
if (sockfd >= 0) {
    if(write (sockfd, "ok\n", strlen("ok\n"))) {
        /* Close socket again */
        close(sockfd);
    }
}

/* Wait 0.1 second before removing the pipe - :-) */
usleep(100);

/* Delete file silently */
if (!(gtkserver.behave & 2)) unlink(gtkserver.fifo);
}

void remove_queue(void)
{
int msgid;

msgid = msgget(gtkserver.ipc, 0666);
if(msgid < 0) fprintf(stderr, "%s%s\n", "Could not find message queue: ", strerror(errno));

if(msgctl(msgid, IPC_RMID, NULL) < 0) {
    fprintf(stderr, "%s%s\n", "Could not delete message queue. ERROR: ", strerror(errno));
}

}

#endif

/*************************************************************************************************/

void Print_Error(char * fmt, int no, ...)
{
va_list args;
char data[MAX_LEN] = { 0 };

#ifdef GTK_SERVER_UNIX
    #if defined(GTK_SERVER_GTK2x) || defined(GTK_SERVER_GTK3x)
	GtkWidget *dialog, *window;
        /* Read arguments incoming in functionheader */
        va_start(args, no);
        vsnprintf(data, MAX_LEN, fmt, args);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", data);
	gtk_window_set_title(GTK_WINDOW(dialog), "GTK-server Error!");
	gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
    #elif GTK_SERVER_GTK1x
	GtkWidget *dialog, *label, *close_button;
        /* Read arguments incoming in functionheader */
        va_start(args, no);
        vsnprintf(data, MAX_LEN, fmt, args);
	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), "GTK-server Error!");
	gtk_widget_set_usize(dialog, 350, 100);
	gtk_window_set_policy(GTK_WINDOW(dialog), FALSE, FALSE, TRUE);
	label = gtk_label_new(data);
	close_button = gtk_button_new_with_label("Close");
	/* Ensure that the dialog box is destroyed when the user clicks ok. */
	gtk_signal_connect_object(GTK_OBJECT(close_button), "clicked", GTK_SIGNAL_FUNC(gtk_main_quit), GTK_OBJECT(dialog));
	gtk_signal_connect_object(GTK_OBJECT(dialog), "delete-event", GTK_SIGNAL_FUNC(gtk_main_quit), GTK_OBJECT(dialog));
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->action_area), close_button);
	/* Add the label, and show everything we've added to the dialog. */
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), label);
	gtk_widget_show_all(dialog);
	gtk_main();
    #elif GTK_SERVER_XF
	FL_FORM *dialog;
        /* Read arguments incoming in functionheader */
        va_start(args, no);
        vsnprintf(data, MAX_LEN, fmt, args);
	dialog = fl_bgn_form(FL_UP_BOX, 340, 180);
	fl_add_text(FL_NORMAL_TEXT, 10, 10, 320, 100, data);
	fl_add_button(FL_NORMAL_BUTTON, 135, 120, 70, 40, "Close");
	fl_end_form();
	fl_show_form(dialog, FL_PLACE_CENTER, FL_FULLBORDER, "GTK-server Error!");
	fl_do_forms();
    #elif GTK_SERVER_MOTIF
	Widget shell, msgbox, button;
        XmFontListEntry entry;
        XmFontList font;
        XmString txt;
        Arg margs[10];
        int n = 0;
        /* Read arguments incoming in functionheader */
        va_start(args, no);
        vsnprintf(data, MAX_LEN, fmt, args);
        shell = XtVaAppCreateShell (NULL, "Class", topLevelShellWidgetClass, XtDisplay(gtkserver.toplevel), NULL);
        entry = XmFontListEntryLoad(XtDisplay(gtkserver.toplevel), "7x13bold" , XmFONT_IS_FONT, "tag");
        font = XmFontListAppendEntry(NULL, entry);
        XmFontListEntryFree(&entry);
        txt = XmStringCreate(Trim_String(data), "tag");
        XtSetArg(margs[n], XmNmessageString, txt); n++;
        XtSetArg(margs[n], XmNlabelFontList, font); n++;
        XtSetArg(margs[n], XmNbuttonFontList, font); n++;
        XtSetArg(margs[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
        XtSetArg(margs[n], XmNdefaultPosition, False); n++;
        XtSetArg(margs[n], XmNx, WidthOfScreen(XtScreen(shell))/2-200); n++;
        XtSetArg(margs[n], XmNy, HeightOfScreen(XtScreen(shell))/2-50); n++;
	msgbox = XmCreateMessageDialog(shell, "message", margs, n);
        XtVaSetValues(msgbox, XtVaTypedArg, XmNdialogTitle, XmRString, "GTK-server Error!", 17, NULL);
        XmStringFree(txt);
        XtAddCallback((Widget)msgbox, XmNokCallback, (XtCallbackProc)exit, NULL);
	button = XmMessageBoxGetChild(msgbox, XmDIALOG_CANCEL_BUTTON);
	if(button) XtDestroyWidget(button);
	button = XmMessageBoxGetChild(msgbox, XmDIALOG_HELP_BUTTON);
	if(button) XtDestroyWidget(button);
	XtManageChild(msgbox);
	XtRealizeWidget(msgbox);
	XtAppMainLoop(gtkserver.app);
    #else
        /* Read arguments incoming in functionheader */
        va_start(args, no);
        vsnprintf(data, MAX_LEN, fmt, args);
	fprintf(stderr, "%s\n\n", data);
    #endif

    /* Send signal to parent process */
    if (gtkserver.behave & 8) kill(gtkserver.ppid, gtkserver.exit_sig);

#elif GTK_SERVER_WIN32
    /* Read arguments incoming in functionheader */
    va_start(args, no);
    vsnprintf(data, MAX_LEN, fmt, args);
    /* Windows messages with messagebox */
    MessageBox(NULL, data, "GTK-server Error", MB_OK | MB_ICONSTOP);
#endif

va_end(args);
/* Since this is an error, exit GTK-server */
exit(EXIT_FAILURE);
}

/*************************************************************************************************/

char *Print_Result(char *fmt, int no, ...)
{
static char *retstr = NULL;
static long memsize = MAX_LEN;

va_list args;
va_list backup;

/* These are used for the C string escaping and printing line count */
unsigned int i = 0, j = 0, l = 0, q, found, amount;
char hex_buf[3];
char int_str[12];
char *result;

/* Here we go */
va_start(args, no);

/* Portable va_copy */
memcpy (&backup, &args, sizeof (va_list));

/* Only executed at first-time use, the 2 extra blocks for are needed for \n\0 */
if (retstr == NULL) {
    retstr = (char*)calloc(memsize + 2, sizeof(char));
    if (retstr == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to allocate returnvalue: ", strerror(errno));
}

/* vsnprintf returns amount of chars that should be written regardless given size */
amount = vsnprintf(retstr, memsize, fmt, args);

if (amount > memsize) {
    retstr = (char*)realloc(retstr, (amount+2)*sizeof(char));
    if (retstr == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to allocate returnvalue: ", strerror(errno));
    memsize = amount;
    /* Put back the va_list as it has been used already, using a portable va_copy */
    memcpy (&args, &backup, sizeof (va_list));
    vsnprintf(retstr, memsize, fmt, args);
}

va_end(args);
va_end(backup);

/* Now check if we need to apply C string escaping, only when a string is returned to client */
if (gtkserver.c_escaped & 2) {

    /* This code is contributed by Jeremy Shute, adjusted to solve bugs found by Leonardo Cecchi */
    /* Adjusted more so user can provide chars to escape - PvE */
    /* Even more so it is generic - Danie Brink */

    /* Walk through the escape symbols and check how many there are */
    for(i = 0; retstr[i]; ++i){
	for(q = 0; gtkserver.escapes[q]; q++){
	    if(gtkserver.escapes[q] == retstr[i]) l += 1;
	    else{
		if((unsigned char)retstr[i] < 32 || (unsigned char)retstr[i] > 254) l += 3; /* Special characters must be returned also */
		else ++l;
	    }
	}
    }

    /* Allocate 2 bytes more for quotes and 1 for NULL byte */
    result = (char*)malloc((strlen((char*)retstr) + l + 3) * sizeof(char));
    if(result == NULL) Print_Error ("%s", 1, "\nERROR: Cannot allocate memory for escaped string!");

    result[0] = '"';

    for(i = 0; retstr[i]; ++i) {
	found = 0;
	for(q = 0; gtkserver.escapes[q]; q++){
	    if(gtkserver.escapes[q] == retstr[i]){
		result[ ++j ] = '\\';
		switch( retstr[i]) {
		    case '\a':
			result[ ++j ] = 'a';
			break;
		    case '\t':
			result[ ++j ] = 't';
			break;
		    case '\n':
			result[ ++j ] = 'n';
			break;
		    case '\r':
			result[ ++j ] = 'r';
			break;
		    default:
			result[ ++j ] = retstr[i];
		}
		found = 1;
	    }
	}
	if(!found){
	    if((unsigned char)retstr[i] < 32 || (unsigned char)retstr[i] > 254) {			/* Special characters must be returned also */
		snprintf(hex_buf, sizeof(hex_buf), "%02X", retstr[i]);
		result[++j] = '\\';
		result[++j] = 'x';
		result[++j] =  hex_buf[0];
		result[++j] =  hex_buf[1];
	    }
	    else result[++j] = retstr[i];
	}
    }
    result[++j] = '"';
    result[++j] = '\0';

    amount = strlen(result);

    /* Static return buffer big enough?*/
    if (amount + 2 > memsize){
	retstr = (char*)realloc(retstr, (amount + 2) * sizeof(char));
	if (retstr == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to allocate returnvalue: ", strerror(errno));
	memsize = amount + 2;
    }

    /* Copy result in static buffer */
    memcpy(retstr, result, (amount + 1) * sizeof(char));

    /* Free temporary buffer */
    free(result);

    /* Make sure to 0 flag */
    gtkserver.c_escaped &= 253;
}

/* Prepend line count to retstr */
if(gtkserver.print_line_count) {
    l = 1;
    for(i = 0; retstr[i]; i++) {
        if(retstr[i] == '\n') l++;
    }
    l = sprintf(int_str, "%d", l) + 1; // 1 byte for line feed

    /* 2 bytes for terminating line feed and NULL byte */
    if(amount + l + 2 > memsize) {
        retstr = (char*)realloc(retstr, (amount + l + 2) * sizeof(char));
    	if(retstr == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to allocate returnvalue: ", strerror(errno));
        memsize = amount + l + 2;
    }

    /* Prepending int_str to retstr */
    memmove(retstr + l, retstr, amount + 1);
    memcpy(retstr, int_str, l - 1);
    retstr[l - 1] = '\n';

    amount += l;
}

/* Add terminating newline if no '-nonl' parameter was provided */
if(!(gtkserver.behave & 1024) ) {
	retstr = strcat(retstr, "\n");
}

return retstr;
}

/*************************************************************************************************/
/*
 * Shamelessly ripped from the SSL tutorial at http://www.rtfm.com/openssl-examples.
 *	Heavily modified to fit GTK-server purposes. PvE.
 */

#ifdef GTK_SERVER_USE_SSL

/* Check that the common name matches the host name*/
void check_cert(SSL *ssl, char *host, FILE *logfile)
{
X509 *peer;
char peer_CN[256];

if(SSL_get_verify_result(ssl) != X509_V_OK) Print_Error("%s", 1, "\nSSL: Certificate of remote server doesn't verify!");

/* Check the common name */
peer = SSL_get_peer_certificate(ssl);

X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peer_CN, 256);

/* If logging is enabled, write text to log */
if (logfile != NULL){
    fprintf(logfile, "SSL: remote identification is \"%s\" - GTK-server connects to \"%s\"\n", peer_CN, host);
    fflush(logfile);
}

if(strcasecmp(peer_CN, host)) Print_Error("%s", 1, "\nSSL: Common name doesn't match host name!");
}

/* Password callback */
static int password_cb(char *buf, int size, int rwflag, void *userdata)
{
if(gtkserver.password != NULL && size < strlen(gtkserver.password) + 1) return(0);

strncpy(buf, gtkserver.password, size);
buf[size - 1] = '\0';

return(strlen(buf));
}

SSL_CTX *initialize_ctx(char *keyfile, char *password, FILE *logfile)
{
SSL_CTX *ctx;
const SSL_METHOD *meth = SSLv23_method();

/* Global system initialization*/
SSL_library_init();
SSL_load_error_strings();

/* Create our context*/
ctx = SSL_CTX_new(meth);

/* Load our keys and certificates*/
if((gtkserver.behave & 64) && keyfile != NULL) {

    /* If logging is enabled, write text to log */
    if (logfile != NULL){
	fprintf(logfile, "SSL: trying to load keyfile \"%s\"\n", keyfile);
	fflush(logfile);
    }

    if(!(SSL_CTX_use_certificate_chain_file(ctx, keyfile))) Print_Error("%s", 1, "\nSSL: Cannot load certificate chain from file!");

    /* Load a private key which optionally is encrypted with a password */
    SSL_CTX_set_default_passwd_cb(ctx, password_cb);
    if(!(SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM))) Print_Error("%s", 1, "\nSSL: Cannot add private key from file!");
}

/* To verify the remote host, load the CAs we trust*/
if(gtkserver.behave & 128) {

    /* If logging is enabled, write text to log */
    if (logfile != NULL){
	fprintf(logfile, "SSL: trying to load CA \"%s\"\n", gtkserver.ca);
	fflush(logfile);
    }

    if(!(SSL_CTX_load_verify_locations(ctx, gtkserver.ca, 0))) Print_Error("%s", 1, "\nSSL: Cannot find file to verify CA!");
}

/* Make sure SSL3 works with legacy servers */
SSL_CTX_set_options(ctx, SSL_OP_ALL);
SSL_CTX_set_options(ctx, SSL_OP_LEGACY_SERVER_CONNECT);

#if (OPENSSL_VERSION_NUMBER < 0x0090600fL)
SSL_CTX_set_verify_depth(ctx, 1);
#endif

return ctx;
}

#endif
/*************************************************************************************************/

#ifdef GTK_SERVER_UNIX
void sig_handler(int signal)
{
int status;
/* A child process was killed, decrease fork counter */
gtkserver.count_fork--;
/* Wait for child without blocking */
if (waitpid(-1, &status, WNOHANG) < 0) return;
}
#endif

/*************************************************************************************************/
/* Piece of code from http://www.koders.com/c/fid1867004532ABB00D9750ACC47A1366D52E8C616C.aspx */
/* Modified a little bit to get it working with Win32 */

char *mystrndup (const char *s, size_t n)
{
char *new = malloc (n + 1);

if (new == NULL) return NULL;

new[n] = '\0';
return (char *) strncpy (new, s, n);
}

/*************************************************************************************************/
/* Used for debug panel */

#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF
void switch_flag_on(void* widget, long *data)
{
    *data = 1;
}

void switch_flag_off(void* widget, long *data)
{
    *data = 0;
}

#elif GTK_SERVER_MOTIF
void switch_flag_on(Widget w, XtPointer data, XtPointer call_data)
{
    *(long*)data = 1;
}

void switch_flag_off(Widget w, XtPointer data, XtPointer call_data)
{
    *(long*)data = 0;
}
#endif

/*************************************************************************************************/
#if GTK_SERVER_XF
int xforms_callback(FL_FORM *form, void *xevent)
{
XEvent *ev;

/* Check if key was pressed */
ev = (XEvent*)xevent;
Current_Object.key = XkbKeycodeToKeysym(fl_display, ev->xkey.keycode, 0, 0);

Current_Object.key_state = 0;

fl_get_form_mouse(form, &Current_Object.mousex, &Current_Object.mousey, &Current_Object.button);

/* Pass to GTK_SERVER_XForms */
return 0;
}

#elif GTK_SERVER_MOTIF
void motif_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
XmAnyCallbackStruct *data;

Current_Object.object = w;
Current_Object.state = (long)w;

Current_Object.text = (char*)client_data;

data = (XmAnyCallbackStruct*)call_data;

if (data->reason == XmCR_INPUT) {
    Current_Object.mousex = data->event->xbutton.x;
    Current_Object.mousey = data->event->xbutton.y;
    Current_Object.button = data->event->xbutton.button;
    Current_Object.key = XkbKeycodeToKeysym(XtDisplay(w), data->event->xkey.keycode, 0, 0);
    Current_Object.key_state = data->event->xkey.state;
}

}

/*************************************************************************************************/

#elif GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
void gtk_server_callback(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5, gpointer *data6, gpointer *data7)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.p4 = (int)(long)data4;
Current_Object.p5 = (int)(long)data5;
Current_Object.p6 = (int)(long)data6;
Current_Object.p7 = (int)(long)data7;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.t4 = (char*)data4;
Current_Object.t5 = (char*)data5;
Current_Object.t6 = (char*)data6;
Current_Object.t7 = (char*)data7;
Current_Object.text = NULL;
}

/*************************************************************************************************/
/* Put these in a separate callback so we can use any string data to return from signal - PvE.   */

void gtk_button_callback(GtkWidget *widget, gpointer *data1, gpointer *data2)
{
GdkEventButton *event;

/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
event = (GdkEventButton*)data1;
Current_Object.button = event->button;
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.text = (char*)data2;
}

gboolean gtk_button_callback_false(GtkWidget *widget, gpointer *data1, gpointer *data2)
{
GdkEventButton *event;

/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif

event = (GdkEventButton*)data1;
Current_Object.button = event->button;
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.text = (char*)data2;

return FALSE;
}

void gtk_key_callback(GtkWidget *widget, gpointer *data1, gpointer *data2)
{
GdkEventKey *event;

/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif

event = (GdkEventKey*)data1;
Current_Object.key = event->keyval;
Current_Object.key_state = event->state;
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.text = (char*)data2;
}

gboolean gtk_key_callback_false(GtkWidget *widget, gpointer *data1, gpointer *data2)
{
GdkEventKey *event;

/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif

event = (GdkEventKey*)data1;
Current_Object.key = event->keyval;
Current_Object.key_state = event->state;
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.text = (char*)data2;

return FALSE;
}

#if defined(GTK_SERVER_GTK2x) || defined(GTK_SERVER_GTK3x)
void gtk_scroll_callback(GtkWidget *widget, gpointer *data1, gpointer *data2)
{
GdkEventScroll *scroll;

/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif

/* Check if we have a scroll button */
scroll = (GdkEventScroll*)data1;
Current_Object.scroll = scroll->direction;
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.text = (char*)data2;
}

gboolean gtk_scroll_callback_false(GtkWidget *widget, gpointer *data1, gpointer *data2)
{
GdkEventScroll *scroll;

/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif

/* Check if we have a scroll button */
scroll = (GdkEventScroll*)data1;
Current_Object.scroll = scroll->direction;
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.text = (char*)data2;

return FALSE;
}
#endif

/*************************************************************************************************/

void gtk_server_callback_extra2(GtkWidget *widget, gpointer *data1)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.t1 = (char*)data1;
Current_Object.text = (char*)data1;
}

void gtk_server_callback_extra3(GtkWidget *widget, gpointer *data1, gpointer *data2)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.text = (char*)data2;
}

void gtk_server_callback_extra4(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.text = (char*)data3;
}

void gtk_server_callback_extra5(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.p4 = (int)(long)data4;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.t4 = (char*)data4;
Current_Object.text = (char*)data4;
}

void gtk_server_callback_extra6(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.p4 = (int)(long)data4;
Current_Object.p5 = (int)(long)data5;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.t4 = (char*)data4;
Current_Object.t5 = (char*)data5;
Current_Object.text = (char*)data5;
}

void gtk_server_callback_extra7(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5, gpointer *data6)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.p4 = (int)(long)data4;
Current_Object.p5 = (int)(long)data5;
Current_Object.p6 = (int)(long)data6;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.t4 = (char*)data4;
Current_Object.t5 = (char*)data5;
Current_Object.t6 = (char*)data6;
Current_Object.text = (char*)data6;
}

void gtk_server_callback_extra8(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5, gpointer *data6, gpointer *data7)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.p4 = (int)(long)data4;
Current_Object.p5 = (int)(long)data5;
Current_Object.p6 = (int)(long)data6;
Current_Object.p7 = (int)(long)data7;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.t4 = (char*)data4;
Current_Object.t5 = (char*)data5;
Current_Object.t6 = (char*)data6;
Current_Object.t7 = (char*)data7;
Current_Object.text = (char*)data7;
}

gboolean gtk_server_callback_extra2_bool(GtkWidget *widget, gpointer *data1)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.t1 = (char*)data1;
Current_Object.text = (char*)data1;
return FALSE;
}

gboolean gtk_server_callback_extra3_bool(GtkWidget *widget, gpointer *data1, gpointer *data2)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.text = (char*)data2;
return FALSE;
}

gboolean gtk_server_callback_extra4_bool(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.text = (char*)data3;
return FALSE;
}

gboolean gtk_server_callback_extra5_bool(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.p4 = (int)(long)data4;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.t4 = (char*)data4;
Current_Object.text = (char*)data4;
return FALSE;
}

gboolean gtk_server_callback_extra6_bool(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.p4 = (int)(long)data4;
Current_Object.p5 = (int)(long)data5;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.t4 = (char*)data4;
Current_Object.t5 = (char*)data5;
Current_Object.text = (char*)data5;
return FALSE;
}

gboolean gtk_server_callback_extra7_bool(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5, gpointer *data6)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.p4 = (int)(long)data4;
Current_Object.p5 = (int)(long)data5;
Current_Object.p6 = (int)(long)data6;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.t4 = (char*)data4;
Current_Object.t5 = (char*)data5;
Current_Object.t6 = (char*)data6;
Current_Object.text = (char*)data6;
return FALSE;
}

gboolean gtk_server_callback_extra8_bool(GtkWidget *widget, gpointer *data1, gpointer *data2, gpointer *data3, gpointer *data4, gpointer *data5, gpointer *data6, gpointer *data7)
{
/* Get mouse stuff */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
gdk_window_get_pointer(widget->window, &Current_Object.mousex, &Current_Object.mousey, NULL);
#else
DEV_MANAGER;
#endif
/* Other data */
Current_Object.object = widget;
Current_Object.state = (long)widget;
Current_Object.p1 = (int)(long)data1;
Current_Object.p2 = (int)(long)data2;
Current_Object.p3 = (int)(long)data3;
Current_Object.p4 = (int)(long)data4;
Current_Object.p5 = (int)(long)data5;
Current_Object.p6 = (int)(long)data6;
Current_Object.p7 = (int)(long)data7;
Current_Object.t1 = (char*)data1;
Current_Object.t2 = (char*)data2;
Current_Object.t3 = (char*)data3;
Current_Object.t4 = (char*)data4;
Current_Object.t5 = (char*)data5;
Current_Object.t6 = (char*)data6;
Current_Object.t7 = (char*)data7;
Current_Object.text = (char*)data7;
return FALSE;
}
#endif

/*************************************************************************************************/
/* Callback function registered by client program (MACRO/DATA argument types). PvE.		*/

void gtk_func(void *data1, void *data2, void *data3, void *data4, void *data5, void *data6, void *data7, void *data8)
{
char command[MAX_LEN];
char num1[MAX_DIG];
char num2[MAX_DIG];
char num3[MAX_DIG];
char num4[MAX_DIG];
char num5[MAX_DIG];
char num6[MAX_DIG];
char num7[MAX_DIG];
char num8[MAX_DIG];
ARG *Arg_Found = NULL;

if((long)data1 != 0 && !illegal_pointer(data1)) {
    HASH_FIND_STR(arg_protos, (char*)data1, Arg_Found);
    if(Arg_Found != NULL){
	strncpy(command, Arg_Found->name, MAX_LEN-7*MAX_DIG-strlen(Arg_Found->data)-8-1);
	snprintf(num1, MAX_DIG, " %s", Arg_Found->data);
	if((long)data2 != 0) snprintf(num2, MAX_DIG, " %ld", (long)data2);
	else strcpy(num2, " 0");
	if((long)data3 != 0) snprintf(num3, MAX_DIG, " %ld", (long)data3);
	else strcpy(num3, " 0");
	if((long)data4 != 0) snprintf(num4, MAX_DIG, " %ld", (long)data4);
	else strcpy(num4, " 0");
	if((long)data5 != 0) snprintf(num5, MAX_DIG, " %ld", (long)data5);
	else strcpy(num5, " 0");
	if((long)data6 != 0) snprintf(num6, MAX_DIG, " %ld", (long)data6);
	else strcpy(num6, " 0");
	if((long)data7 != 0) snprintf(num7, MAX_DIG, " %ld", (long)data7);
	else strcpy(num7, " 0");
	if((long)data8 != 0) snprintf(num8, MAX_DIG, " %ld", (long)data8);
	else strcpy(num8, " 0");
    }
}
if((long)data2 != 0 && Arg_Found == NULL && !illegal_pointer(data2)) {
    HASH_FIND_STR(arg_protos, (char*)data2, Arg_Found);
    if(Arg_Found != NULL){
	strncpy(command, Arg_Found->name, MAX_LEN-7*MAX_DIG-strlen(Arg_Found->data)-8-1);
	if((long)data1 != 0) snprintf(num1, MAX_DIG, " %ld", (long)data1);
	else strcpy(num1, " 0");
	snprintf(num2, MAX_DIG, " %s", Arg_Found->data);
	if((long)data3 != 0) snprintf(num3, MAX_DIG, " %ld", (long)data3);
	else strcpy(num3, " 0");
	if((long)data4 != 0) snprintf(num4, MAX_DIG, " %ld", (long)data4);
	else strcpy(num4, " 0");
	if((long)data5 != 0) snprintf(num5, MAX_DIG, " %ld", (long)data5);
	else strcpy(num5, " 0");
	if((long)data6 != 0) snprintf(num6, MAX_DIG, " %ld", (long)data6);
	else strcpy(num6, " 0");
	if((long)data7 != 0) snprintf(num7, MAX_DIG, " %ld", (long)data7);
	else strcpy(num7, " 0");
	if((long)data8 != 0) snprintf(num8, MAX_DIG, " %ld", (long)data8);
	else strcpy(num8, " 0");
    }
}
if((long)data3 != 0 && Arg_Found == NULL && !illegal_pointer(data3)) {
    HASH_FIND_STR(arg_protos, (char*)data3, Arg_Found);
    if(Arg_Found != NULL){
	strncpy(command, Arg_Found->name, MAX_LEN-7*MAX_DIG-strlen(Arg_Found->data)-8-1);
	if((long)data1 != 0) snprintf(num1, MAX_DIG, " %ld", (long)data1);
	else strcpy(num1, " 0");
	if((long)data2 != 0) snprintf(num2, MAX_DIG, " %ld", (long)data2);
	else strcpy(num2, " 0");
	snprintf(num3, MAX_DIG, " %s", Arg_Found->data);
	if((long)data4 != 0) snprintf(num4, MAX_DIG, " %ld", (long)data4);
	else strcpy(num4, " 0");
	if((long)data5 != 0) snprintf(num5, MAX_DIG, " %ld", (long)data5);
	else strcpy(num5, " 0");
	if((long)data6 != 0) snprintf(num6, MAX_DIG, " %ld", (long)data6);
	else strcpy(num6, " 0");
	if((long)data7 != 0) snprintf(num7, MAX_DIG, " %ld", (long)data7);
	else strcpy(num7, " 0");
	if((long)data8 != 0) snprintf(num8, MAX_DIG, " %ld", (long)data8);
	else strcpy(num8, " 0");
    }
}
if((long)data4 != 0 && Arg_Found == NULL && !illegal_pointer(data4)) {
    HASH_FIND_STR(arg_protos, (char*)data4, Arg_Found);
    if(Arg_Found != NULL){
	strncpy(command, Arg_Found->name, MAX_LEN-7*MAX_DIG-strlen(Arg_Found->data)-8-1);
	if((long)data1 != 0) snprintf(num1, MAX_DIG, " %ld", (long)data1);
	else strcpy(num1, " 0");
	if((long)data2 != 0) snprintf(num2, MAX_DIG, " %ld", (long)data2);
	else strcpy(num2, " 0");
	if((long)data3 != 0) snprintf(num3, MAX_DIG, " %ld", (long)data3);
	else strcpy(num3, " 0");
	snprintf(num4, MAX_DIG, " %s", Arg_Found->data);
	if((long)data5 != 0) snprintf(num5, MAX_DIG, " %ld", (long)data5);
	else strcpy(num5, " 0");
	if((long)data6 != 0) snprintf(num6, MAX_DIG, " %ld", (long)data6);
	else strcpy(num6, " 0");
	if((long)data7 != 0) snprintf(num7, MAX_DIG, " %ld", (long)data7);
	else strcpy(num7, " 0");
	if((long)data8 != 0) snprintf(num8, MAX_DIG, " %ld", (long)data8);
	else strcpy(num8, " 0");
    }
}
if((long)data5 != 0 && Arg_Found == NULL && !illegal_pointer(data5)) {
    HASH_FIND_STR(arg_protos, (char*)data5, Arg_Found);
    if(Arg_Found != NULL){
	strncpy(command, Arg_Found->name, MAX_LEN-7*MAX_DIG-strlen(Arg_Found->data)-8-1);
	if((long)data1 != 0) snprintf(num1, MAX_DIG, " %ld", (long)data1);
	else strcpy(num1, " 0");
	if((long)data2 != 0) snprintf(num2, MAX_DIG, " %ld", (long)data2);
	else strcpy(num2, " 0");
	if((long)data3 != 0) snprintf(num3, MAX_DIG, " %ld", (long)data3);
	else strcpy(num3, " 0");
	if((long)data4 != 0) snprintf(num4, MAX_DIG, " %ld", (long)data4);
	else strcpy(num4, " 0");
	snprintf(num5, MAX_DIG, " %s", Arg_Found->data);
	if((long)data6 != 0) snprintf(num6, MAX_DIG, " %ld", (long)data6);
	else strcpy(num6, " 0");
	if((long)data7 != 0) snprintf(num7, MAX_DIG, " %ld", (long)data7);
	else strcpy(num7, " 0");
	if((long)data8 != 0) snprintf(num8, MAX_DIG, " %ld", (long)data8);
	else strcpy(num8, " 0");
    }
}
if((long)data6 != 0 && Arg_Found == NULL && !illegal_pointer(data6)) {
    HASH_FIND_STR(arg_protos, (char*)data6, Arg_Found);
    if(Arg_Found != NULL){
	strncpy(command, Arg_Found->name, MAX_LEN-7*MAX_DIG-strlen(Arg_Found->data)-8-1);
	if((long)data1 != 0) snprintf(num1, MAX_DIG, " %ld", (long)data1);
	else strcpy(num1, " 0");
	if((long)data2 != 0) snprintf(num2, MAX_DIG, " %ld", (long)data2);
	else strcpy(num2, " 0");
	if((long)data3 != 0) snprintf(num3, MAX_DIG, " %ld", (long)data3);
	else strcpy(num3, " 0");
	if((long)data4 != 0) snprintf(num4, MAX_DIG, " %ld", (long)data4);
	else strcpy(num4, " 0");
	if((long)data5 != 0) snprintf(num5, MAX_DIG, " %ld", (long)data5);
	else strcpy(num5, " 0");
	snprintf(num6, MAX_DIG, " %s", Arg_Found->data);
	if((long)data7 != 0) snprintf(num7, MAX_DIG, " %ld", (long)data7);
	else strcpy(num7, " 0");
	if((long)data8 != 0) snprintf(num8, MAX_DIG, " %ld", (long)data8);
	else strcpy(num8, " 0");
    }
}
if((long)data7 != 0 && Arg_Found == NULL && !illegal_pointer(data7)) {
    HASH_FIND_STR(arg_protos, (char*)data7, Arg_Found);
    if(Arg_Found != NULL){
	strncpy(command, Arg_Found->name, MAX_LEN-7*MAX_DIG-strlen(Arg_Found->data)-8-1);
	if((long)data1 != 0) snprintf(num1, MAX_DIG, " %ld", (long)data1);
	else strcpy(num1, " 0");
	if((long)data2 != 0) snprintf(num2, MAX_DIG, " %ld", (long)data2);
	else strcpy(num2, " 0");
	if((long)data3 != 0) snprintf(num3, MAX_DIG, " %ld", (long)data3);
	else strcpy(num3, " 0");
	if((long)data4 != 0) snprintf(num4, MAX_DIG, " %ld", (long)data4);
	else strcpy(num4, " 0");
	if((long)data5 != 0) snprintf(num5, MAX_DIG, " %ld", (long)data5);
	else strcpy(num5, " 0");
	if((long)data6 != 0) snprintf(num6, MAX_DIG, " %ld", (long)data6);
	else strcpy(num6, " 0");
	snprintf(num7, MAX_DIG, " %s", Arg_Found->data);
	if((long)data8 != 0) snprintf(num8, MAX_DIG, " %ld", (long)data8);
	else strcpy(num8, " 0");
    }
}
if((long)data8 != 0 && Arg_Found == NULL && !illegal_pointer(data8)) {
    HASH_FIND_STR(arg_protos, (char*)data8, Arg_Found);
    if(Arg_Found != NULL){
	strncpy(command, Arg_Found->name, MAX_LEN-7*MAX_DIG-strlen(Arg_Found->data)-8-1);
	if((long)data1 != 0) snprintf(num1, MAX_DIG, " %ld", (long)data1);
	else strcpy(num1, " 0");
	if((long)data2 != 0) snprintf(num2, MAX_DIG, " %ld", (long)data2);
	else strcpy(num2, " 0");
	if((long)data3 != 0) snprintf(num3, MAX_DIG, " %ld", (long)data3);
	else strcpy(num3, " 0");
	if((long)data4 != 0) snprintf(num4, MAX_DIG, " %ld", (long)data4);
	else strcpy(num4, " 0");
	if((long)data5 != 0) snprintf(num5, MAX_DIG, " %ld", (long)data5);
	else strcpy(num5, " 0");
	if((long)data6 != 0) snprintf(num6, MAX_DIG, " %ld", (long)data6);
	else strcpy(num6, " 0");
	if((long)data7 != 0) snprintf(num7, MAX_DIG, " %ld", (long)data7);
	else strcpy(num7, " 0");
	snprintf(num8, MAX_DIG, " %s", Arg_Found->data);
    }
}
if (Arg_Found == NULL) Print_Error("%s", 1, "\nERROR: Macro in user function not found!");

/* Construct complete MACRO command to 'call back' */
strcat(command, num1);
strcat(command, num2);
strcat(command, num3);
strcat(command, num4);
strcat(command, num5);
strcat(command, num6);
strcat(command, num7);
strcat(command, num8);

/* Now invoke the macro */
Call_Realize(command, cinv_ctx);
}

/*************************************************************************************************/
/* Strings in pointer arguments are cut to MAX_DIG - this is intentionally. PvE. */

char *Return_Pointer_Args(CONFIG *call)
{
static char ptrstr[MAX_LEN];

char buf[MAX_DIG];
int i;

memset(ptrstr, 0, MAX_LEN);

for(i = 0; i < atoi(call->argamount); i++){
    memset(buf, 0, MAX_DIG);
    if(!strcmp(call->args[i], "PTR_LONG")) snprintf(buf, MAX_DIG, " %ld", long_address[i]);
    if(!strcmp(call->args[i], "PTR_INT") || !strcmp(call->args[i], "PTR_BOOL")) snprintf(buf, MAX_DIG, " %d", int_address[i]);
    if(!strcmp(call->args[i], "PTR_SHORT")) snprintf(buf, MAX_DIG, " %hd", short_address[i]);
    if(!strcmp(call->args[i], "PTR_WIDGET")) snprintf(buf, MAX_DIG, " %ld", (long)obj_address[i]);
    if(!strcmp(call->args[i], "PTR_DOUBLE")) snprintf(buf, MAX_DIG, " %f", double_address[i]);
    if(!strcmp(call->args[i], "PTR_FLOAT")) snprintf(buf, MAX_DIG, " %f", float_address[i]);
    if(!strcmp(call->args[i], "PTR_STRING")) snprintf(buf, MAX_DIG, " %s", str_address[i]);

    if(!strcmp(call->args[i], "PTR_BASE64")) {
        strncat(ptrstr, " ", 2);
        strncat(ptrstr, base64_enc(str_address[i], PTR_BASE64), MAX_LEN-strlen(ptrstr));
        free(str_address[i]);
    }
    else {
        strncat(ptrstr, buf, MAX_DIG);
    }
}

/* Return memory pointer */
return(ptrstr);
}

/*************************************************************************************************/

#ifdef GTK_SERVER_FFCALL
char *Void_GUI(CONFIG *call)
{
char *ptrstr;

#elif GTK_SERVER_DYNCALL
char *Void_GUI(void *func, CONFIG *call, DCCallVM* vm)
{
char *ptrstr = NULL;

dcCallVoid(vm, (DCpointer)func);
dcFree(vm);

#elif GTK_SERVER_FFI
char *Void_GUI(FUNC_POINTER, CONFIG *call, ffi_type * argtypes[], void * argptr[])
{
char *ptrstr;
ffi_cif cif;		/* Contains pointer to FFI_CIF structure */

/* Initialize the cif, realize function */
if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, atoi(call->argamount), &ffi_type_void, argtypes) == FFI_OK)
{
    #ifdef GTK_SERVER_UNIX
    ffi_call(&cif, func, NULL, argptr);
    #elif GTK_SERVER_WIN32
    ffi_call(&cif, (void*)*func, NULL, argptr);
    /* Free the malloced space after the call */
    free(func);
    #endif
}
else Print_Error ("%s", 1, "\nERROR: the FFI could not be initialized!");

#elif GTK_SERVER_CINV
char *Void_GUI(void *func, CONFIG *call, char argtypes[], void * argptr[], CInvContext* cinv_ctx)
{
CInvFunction *proto;
char *ptrstr;

proto = cinv_function_create(cinv_ctx, CINV_CC_DEFAULT, "", argtypes);
if (!cinv_function_invoke(cinv_ctx, proto, func, NULL, argptr)) Print_Error ("%s", 1, "\nERROR: C/Invoke failed!");
#endif

/* Return positive response */
ptrstr = Return_Pointer_Args(call);

if (strcmp(ptrstr, "")) return Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, ++ptrstr, gtkserver.post);

return Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
}

/*************************************************************************************************/

#ifdef GTK_SERVER_FFCALL
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
    char *Widget_GUI(GtkObject *widget, CONFIG *call)
    {
    #elif GTK_SERVER_XF
    char *Widget_GUI(FL_OBJECT *widget, CONFIG *call)
    {
	ENUM *Enum_Found;
    #elif GTK_SERVER_MOTIF
    char *Widget_GUI(Widget widget, CONFIG *call)
    {
	STR *Str_Found;
    #else
    char *Widget_GUI(void *widget, CONFIG *call)
    {
    #endif
char *ptrstr;

#elif GTK_SERVER_DYNCALL
char *Widget_GUI(void *func, CONFIG *call, DCCallVM* vm)
{
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
GtkWidget *widget;		/* Temporary widget holder */
#elif GTK_SERVER_XF
ENUM *Enum_Found;
FL_OBJECT *widget;
#elif GTK_SERVER_MOTIF
STR *Str_Found;
Widget widget;
#else
void *widget;
#endif
char *ptrstr;

widget = dcCallPointer(vm, (DCpointer)func);
dcFree(vm);

#elif GTK_SERVER_FFI
char *Widget_GUI(FUNC_POINTER, CONFIG *call, ffi_type * argtypes[], void * argptr[])
{
ffi_cif cif;		/* Contains pointer to FFI_CIF structure */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
GtkWidget *widget;		/* Temporary widget holder */
#elif GTK_SERVER_XF
ENUM *Enum_Found;
FL_OBJECT *widget;
#elif GTK_SERVER_MOTIF
STR *Str_Found;
Widget widget;
#else
void *widget;
#endif
char *ptrstr;

/* Initialize the cif, realize function */
if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, atoi(call->argamount), &ffi_type_pointer, argtypes) == FFI_OK)
{
    #ifdef GTK_SERVER_UNIX
    ffi_call(&cif, func, &widget, argptr);
    #elif GTK_SERVER_WIN32
    ffi_call(&cif, (void*)*func, &widget, argptr);
    /* Free the malloced space after the call */
    free(func);
    #endif
}
else Print_Error ("%s", 1, "\nERROR: the FFI could not be initialized!");

#elif GTK_SERVER_CINV
char *Widget_GUI(void *func, CONFIG *call, char argtypes[], void * argptr[], CInvContext* cinv_ctx)
{
CInvFunction *proto;
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
GtkWidget *widget;		/* Temporary widget holder */
#elif GTK_SERVER_XF
ENUM *Enum_Found;
FL_OBJECT *widget;
#elif GTK_SERVER_MOTIF
STR *Str_Found;
Widget widget;
#else
void *widget;
#endif
char *ptrstr;

proto = cinv_function_create(cinv_ctx, CINV_CC_DEFAULT, "p", argtypes);
if (!cinv_function_invoke(cinv_ctx, proto, func, &widget, argptr)) Print_Error ("%s", 1, "\nERROR: C/Invoke failed!");
#endif

/* Allocate memory to memorize attached signals - GTK memorizes the address instead of the userdata */
if (Start_List_Sigs == NULL){
    List_Sigs = (ASSOC*)malloc(sizeof(ASSOC));
    Start_List_Sigs = List_Sigs;
    List_Sigs->next = NULL;
}
else {
    List_Sigs->next = (ASSOC*)malloc(sizeof(ASSOC));
    List_Sigs = List_Sigs->next;
    List_Sigs->next = NULL;
}
List_Sigs->data = strdup(call->callbacktype);

/* Connect signals */
if (strcmp(call->callbacktype, "NONE")) {

    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
    SIGNALCONNECT(widget, call->callbacktype, gtk_server_callback, List_Sigs->data);
    #elif GTK_SERVER_XF
    if(isalpha(call->callbacktype[0]))
    {
	HASH_FIND_STR(enum_protos, call->callbacktype, Enum_Found);
	if (Enum_Found != NULL)
	{
	    fl_register_raw_callback((FL_FORM*)widget, Enum_Found->value, xforms_callback);
	}
	else
	{
	    Print_Error("\nERROR: Cannot find signal '%s'!", 1, call->callbacktype);
	}
    }
    else
    {
	fl_register_raw_callback((FL_FORM*)widget, atoi(call->callbacktype), xforms_callback);
    }
    #elif GTK_SERVER_MOTIF
    HASH_FIND_STR(str_protos, call->callbacktype, Str_Found);
    if (Str_Found != NULL){
        XtAddCallback((Widget)widget, Str_Found->value, motif_callback, List_Sigs->data);
    }
    else{
        XtAddCallback((Widget)widget, call->callbacktype, motif_callback, List_Sigs->data);
    }
    #endif
}

/* Convert to long value */
ptrstr = Return_Pointer_Args(call);

return Print_Result("%s%s%ld%s%s", 5, gtkserver.pre, gtkserver.handle, (long)widget, ptrstr, gtkserver.post);
}

/*************************************************************************************************/

#ifdef GTK_SERVER_FFCALL
char *Pointer_OBJ(void *pointer, CONFIG *call)
{
char *ptrstr;

#elif GTK_SERVER_DYNCALL
char *Pointer_OBJ(void *func, CONFIG *call, DCCallVM* vm)
{
void *pointer;
char *ptrstr;

pointer = dcCallPointer(vm, (DCpointer)func);
dcFree(vm);

#elif GTK_SERVER_FFI
char *Pointer_OBJ(FUNC_POINTER, CONFIG *call, ffi_type * argtypes[], void * argptr[])
{
ffi_cif cif;		/* Contains pointer to FFI_CIF structure */
void *pointer;
char *ptrstr;

/* Initialize the cif, realize function */
if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, atoi(call->argamount), &ffi_type_pointer, argtypes) == FFI_OK)
{
    #ifdef GTK_SERVER_UNIX
    ffi_call(&cif, func, &pointer, argptr);
    #elif GTK_SERVER_WIN32
    ffi_call(&cif, (void*)*func, &pointer, argptr);
    /* Free the malloced space after the call */
    free(func);
    #endif
}
else Print_Error ("%s", 1, "\nERROR: the FFI could not be initialized!");

#elif GTK_SERVER_CINV
char *Pointer_OBJ(void *func, CONFIG *call, char argtypes[], void * argptr[], CInvContext* cinv_ctx)
{
CInvFunction *proto;
void *pointer;
char *ptrstr;

proto = cinv_function_create(cinv_ctx, CINV_CC_DEFAULT, "p", argtypes);
if (!cinv_function_invoke(cinv_ctx, proto, func, &pointer, argptr)) Print_Error ("%s", 1, "\nERROR: C/Invoke failed!");
#endif

/* Seeif there are pointer arguments */
ptrstr = Return_Pointer_Args(call);

/* Convert to long value */
return Print_Result("%s%s%ld%s%s", 4, gtkserver.pre, gtkserver.handle, (long)pointer, ptrstr, gtkserver.post);
}

/*************************************************************************************************/

#ifdef GTK_SERVER_FFCALL
char *String_GUI(char *result, CONFIG * call)
{
char *ptrstr;

#elif GTK_SERVER_DYNCALL
char *String_GUI(void *func, CONFIG *call, DCCallVM* vm)
{
char *ptrstr;
char *result;				/* temp result holder */

result = dcCallPointer(vm, (DCpointer)func);
dcFree(vm);

#elif GTK_SERVER_FFI
char *String_GUI(FUNC_POINTER, CONFIG *call, ffi_type * argtypes[], void * argptr[])
{
ffi_cif cif;		/* Contains pointer to FFI_CIF structure */
char *result;				/* temp result holder */
char *ptrstr;

/* Initialize the cif, realize function */
if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, atoi(call->argamount), &ffi_type_pointer, argtypes) == FFI_OK)
{
    #ifdef GTK_SERVER_UNIX
    ffi_call(&cif, func, &result, argptr);
    #elif GTK_SERVER_WIN32
    ffi_call(&cif, (void*)*func, &result, argptr);
    /* Free the malloced space after the call */
    free(func);
    #endif
}
else Print_Error ("%s", 1, "\nERROR: the FFI could not be initialized!");

#elif GTK_SERVER_CINV
char *String_GUI(void *func, CONFIG *call, char argtypes[], void * argptr[], CInvContext* cinv_ctx)
{
CInvFunction *proto;
char *result;				/* temp result holder */
char *ptrstr;

proto = cinv_function_create(cinv_ctx, CINV_CC_DEFAULT, "p", argtypes);
if (!cinv_function_invoke(cinv_ctx, proto, func, &result, argptr)) Print_Error ("%s", 1, "\nERROR: C/Invoke failed!");

#endif

ptrstr = Return_Pointer_Args(call);

/* We are in a STRING function, need to set escaped flag? */
if(gtkserver.c_escaped & 1) gtkserver.c_escaped |= 2;

/* Return */
if (result == NULL) return Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, ptrstr, gtkserver.post);

return Print_Result("%s%s%s%s%s", 5, gtkserver.pre, gtkserver.handle, result, ptrstr, gtkserver.post);
}

/*************************************************************************************************/

#ifdef GTK_SERVER_FFCALL
char *Bool_GUI(int boolean, CONFIG * call)
{
char *ptrstr;

#elif GTK_SERVER_DYNCALL
char *Bool_GUI(void *func, CONFIG *call, DCCallVM* vm)
{
char *ptrstr;
int boolean;			/* Temporary bool holder */

boolean = dcCallBool(vm, (DCpointer)func);
dcFree(vm);

#elif GTK_SERVER_FFI
char *Bool_GUI(FUNC_POINTER, CONFIG * call, ffi_type * argtypes[], void * argptr[])
{
int boolean;			/* Temporary bool holder */
ffi_cif cif;		/* Contains pointer to FFI_CIF structure */
char *ptrstr;

/* Initialize the cif, realize function */
if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, atoi(call->argamount), &ffi_type_uint, argtypes) == FFI_OK)
{
    #ifdef GTK_SERVER_UNIX
    ffi_call(&cif, func, &boolean, argptr);
    #elif GTK_SERVER_WIN32
    ffi_call(&cif, (void*)*func, &boolean, argptr);
    /* Free the malloced space after the call */
    free(func);
    #endif
}
else Print_Error ("%s", 1, "\nERROR: the FFI could not be initialized!");

#elif GTK_SERVER_CINV
char *Bool_GUI(void *func, CONFIG *call, char argtypes[], void * argptr[], CInvContext* cinv_ctx)
{
CInvFunction *proto;
int boolean;		/* Temporary bool holder */
char *ptrstr;

proto = cinv_function_create(cinv_ctx, CINV_CC_DEFAULT, "i", argtypes);
if (!cinv_function_invoke(cinv_ctx, proto, func, &boolean, argptr)) Print_Error ("%s", 1, "\nERROR: C/Invoke failed!");

#endif

ptrstr = Return_Pointer_Args(call);

/* Convert to bool value */
if (boolean) return Print_Result("%s%s1%s%s", 4, gtkserver.pre, gtkserver.handle, ptrstr, gtkserver.post);

return Print_Result("%s%s0%s%s", 4, gtkserver.pre, gtkserver.handle, ptrstr, gtkserver.post);
}

/*************************************************************************************************/

#ifdef GTK_SERVER_FFCALL
char *Int_GUI(unsigned int result, CONFIG *call)
{
char *ptrstr;

#elif GTK_SERVER_DYNCALL
char * Int_GUI(void *func, CONFIG *call, DCCallVM* vm)
{
char *ptrstr;
unsigned int result;

result = dcCallInt(vm, (DCpointer)func);
dcFree(vm);

#elif GTK_SERVER_FFI
char *Int_GUI(FUNC_POINTER, CONFIG * call, ffi_type * argtypes[], void * argptr[])
{
ffi_cif cif;		/* Contains pointer to FFI_CIF structure */
unsigned int result;			/* Temporary result holder */
char *ptrstr;

/* Initialize the cif, realize function */
if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, atoi(call->argamount), &ffi_type_uint, argtypes) == FFI_OK)
{
    #ifdef GTK_SERVER_UNIX
    ffi_call(&cif, func, &result, argptr);
    #elif GTK_SERVER_WIN32
    ffi_call(&cif, (void*)*func, &result, argptr);
    /* Free the malloced space after the call */
    free(func);
    #endif
}
else Print_Error ("%s", 1, "\nERROR: the FFI could not be initialized!");

#elif GTK_SERVER_CINV
char *Int_GUI(void *func, CONFIG *call, char argtypes[], void * argptr[], CInvContext* cinv_ctx)
{
CInvFunction *proto;
unsigned int result;			/* Temporary result holder */
char *ptrstr;

proto = cinv_function_create(cinv_ctx, CINV_CC_DEFAULT, "i", argtypes);
if (!cinv_function_invoke(cinv_ctx, proto, func, &result, argptr)) Print_Error ("%s", 1, "\nERROR: C/Invoke failed!");

#endif

ptrstr = Return_Pointer_Args(call);

/* Convert to int value */
return Print_Result("%s%s%d%s%s", 5, gtkserver.pre, gtkserver.handle, result, ptrstr, gtkserver.post);
}

/*************************************************************************************************/

#ifdef GTK_SERVER_FFCALL
char *Long_GUI(unsigned long result, CONFIG *call)
{
char *ptrstr;

#elif GTK_SERVER_DYNCALL
char *Long_GUI(void *func, CONFIG *call, DCCallVM* vm)
{
char *ptrstr;
unsigned long result;

result = dcCallLong(vm, (DCpointer)func);
dcFree(vm);

#elif GTK_SERVER_FFI
char *Long_GUI(FUNC_POINTER, CONFIG * call, ffi_type * argtypes[], void * argptr[])
{
ffi_cif cif;		/* Contains pointer to FFI_CIF structure */
unsigned long result;			/* Temporary result holder */
char *ptrstr;

/* Initialize the cif, realize function */
if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, atol(call->argamount), &ffi_type_ulong, argtypes) == FFI_OK)
{
    #ifdef GTK_SERVER_UNIX
    ffi_call(&cif, func, &result, argptr);
    #elif GTK_SERVER_WIN32
    ffi_call(&cif, (void*)*func, &result, argptr);
    /* Free the malloced space after the call */
    free(func);
    #endif
}
else Print_Error ("%s", 1, "\nERROR: the FFI could not be initialized!");

#elif GTK_SERVER_CINV
char *Long_GUI(void *func, CONFIG *call, char argtypes[], void * argptr[], CInvContext* cinv_ctx)
{
CInvFunction *proto;
unsigned long result;			/* Temporary result holder */
char *ptrstr;

proto = cinv_function_create(cinv_ctx, CINV_CC_DEFAULT, "l", argtypes);
if (!cinv_function_invoke(cinv_ctx, proto, func, &result, argptr)) Print_Error ("%s", 1, "\nERROR: C/Invoke failed!");

#endif

ptrstr = Return_Pointer_Args(call);

/* Convert to int value */
return Print_Result("%s%s%ld%s%s", 5, gtkserver.pre, gtkserver.handle, result, ptrstr, gtkserver.post);
}

/*************************************************************************************************/

#ifdef GTK_SERVER_FFCALL
char *Float_GUI(float result, CONFIG *call)
{
char *ptrstr;

ptrstr = Return_Pointer_Args(call);

return Print_Result("%s%s%f%s%s", 5, gtkserver.pre, gtkserver.handle, result, ptrstr, gtkserver.post);

#elif GTK_SERVER_DYNCALL
char *Float_GUI(void *func, CONFIG *call, int type, DCCallVM* vm)
{
char *ptrstr;
float fresult;
double dresult;

ptrstr = Return_Pointer_Args(call);

if(type == 1){
    fresult = dcCallFloat(vm, (DCpointer)func);
    dcFree(vm);
    return Print_Result("%s%s%f%s%s", 5, gtkserver.pre, gtkserver.handle, fresult, ptrstr, gtkserver.post);
}
else {
    dresult = dcCallDouble(vm, (DCpointer)func);
    dcFree(vm);
    return Print_Result("%s%s%f%s%s", 5, gtkserver.pre, gtkserver.handle, dresult, ptrstr, gtkserver.post);
}

#elif GTK_SERVER_FFI
char *Float_GUI(FUNC_POINTER, CONFIG *call, ffi_type * argtypes[], void * argptr[], int type)
{
ffi_cif cif;		/* Contains pointer to FFI_CIF structure */
float fresult;			/* Temporary result holder */
double dresult;
char *ptrstr;

ptrstr = Return_Pointer_Args(call);

/* Initialize the cif, realize function */
if (type == 1){
    if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, atoi(call->argamount), &ffi_type_float, argtypes) == FFI_OK) {
	#ifdef GTK_SERVER_UNIX
	ffi_call(&cif, func, &fresult, argptr);
	#elif GTK_SERVER_WIN32
	ffi_call(&cif, (void*)*func, &fresult, argptr);
	/* Free the malloced space after the call */
	free(func);
	#endif
    }
    else Print_Error ("%s", 1, "\nERROR: the FFI could not be initialized!");
    return Print_Result("%s%s%f%s%s", 5, gtkserver.pre, gtkserver.handle, fresult, ptrstr, gtkserver.post);
}
else {
    if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, atoi(call->argamount), &ffi_type_double, argtypes) == FFI_OK) {
	#ifdef GTK_SERVER_UNIX
	ffi_call(&cif, func, &dresult, argptr);
	#elif GTK_SERVER_WIN32
	ffi_call(&cif, (void*)*func, &dresult, argptr);
	/* Free the malloced space after the call */
	free(func);
	#endif
    }
    else Print_Error ("%s", 1, "\nERROR: the FFI could not be initialized!");
    return Print_Result("%s%s%f%s%s", 5, gtkserver.pre, gtkserver.handle, dresult, ptrstr, gtkserver.post);
}

#elif GTK_SERVER_CINV
char *Float_GUI(void *func, CONFIG *call, char argtypes[], void * argptr[], CInvContext* cinv_ctx, int type)
{
CInvFunction *proto;
float fresult;			/* Temporary result holder */
double dresult;
char *ptrstr;

ptrstr = Return_Pointer_Args(call);

if (type == 1){
    proto = cinv_function_create(cinv_ctx, CINV_CC_DEFAULT, "f", argtypes);
    if (!cinv_function_invoke(cinv_ctx, proto, func, &fresult, argptr)) Print_Error ("%s", 1, "\nERROR: C/Invoke failed!");
    return Print_Result("%s%s%f%s%s", 5, gtkserver.pre, gtkserver.handle, fresult, ptrstr, gtkserver.post);
}
else {
    proto = cinv_function_create(cinv_ctx, CINV_CC_DEFAULT, "d", argtypes);
    if (!cinv_function_invoke(cinv_ctx, proto, func, &dresult, argptr)) Print_Error ("%s", 1, "\nERROR: C/Invoke failed!");
    return Print_Result("%s%s%f%s%s", 5, gtkserver.pre, gtkserver.handle, dresult, ptrstr, gtkserver.post);
}
#endif
}

/*************************************************************************************************/
/* These functions replace the 'g_list' stuff in the parser to be more portable - PvE.		 */

PARSED *parse_append(PARSED *base, char *data)
{
PARSED *tmp = NULL;
PARSED *keep = base;

if (base == NULL){
    base = (PARSED*)malloc(sizeof(PARSED));
    base->arg = data;
    base->next = NULL;
    return(base);
}
else {
    while(base != NULL) {tmp = base; base = base->next;}
    base = (PARSED*)malloc(sizeof(PARSED));

    base->arg = data;
    base->next = NULL;

    if (tmp != NULL) tmp->next = base;
}

return(keep);
}

char *parse_data(PARSED *base, int i)
{
while (base != NULL && i > 0){
    base = base->next;
    i--;
}
if (base == NULL) return NULL;

return (base->arg);
}

void parse_free(PARSED *base)
{
PARSED *tmp;

while (base != NULL){
    free(base->arg);
    tmp = base;
    base = base->next;
    free(tmp);
}
}

/*************************************************************************************************/

/* This is the argument parser created by Jim Bailey, adjusted slightly to fit into the GTK-server. */
/* Adjusted by Christian Thaeter to accept strings surrounded by single quotes (e.g. for Prolog usage) */

PARSED *parse_line(char *data)
{
PARSED *args = NULL;
int start_idx = 0;
int curr_idx = 0;
int quote_dst_idx = 0;
int state = 0;
char c;

const int in_plain 		= 1 << 1;
const int in_doublequotes	= 1 << 2;
const int in_singlequotes    	= 1 << 3;
const int after_escape 		= 1 << 4;
const int after_string 		= 1 << 5;

/* loop over the contents */
for ( ; curr_idx <= strlen(data); ++curr_idx ){

    c = data[curr_idx];

    if ( state & in_plain ){
	if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' || curr_idx == strlen(data)){
	    args = parse_append(args, mystrndup(data + start_idx, curr_idx - start_idx ) );
	    state ^= in_plain;
	    state ^= after_string;
	}
    }
    else if ( state & in_doublequotes )	{
	if ( state & after_escape ){
	    if ( c == 'n' )
		c = '\n';
	    else if ( c == 't' )
		c = '\t';
	    else if ( c == 'r' )
		c = '\r';

	    data[quote_dst_idx] = c;
	    ++quote_dst_idx;

	    state ^= after_escape;
	}
	else if ( c == '"' ){
	    args = parse_append(args, mystrndup(data + start_idx, quote_dst_idx - start_idx ) );
	    state ^= in_doublequotes;
	}
	else if ( c == '\\' ){
	    state |= after_escape;
	}
	else {
	    data[quote_dst_idx] = c;
	    ++quote_dst_idx;
	}
    }
    else if ( state & in_singlequotes ) {
	if ( state & after_escape ){
	    if ( c == 'n' )
		c = '\n';
	    else if ( c == 't' )
		c = '\t';
	    else if ( c == 'r' )
		c = '\r';

	    data[quote_dst_idx] = c;
	    ++quote_dst_idx;

	    state ^= after_escape;
	}
	else if ( c == '\'' ){
            if ( state & after_string )
            {
	        args = parse_append(args, mystrndup(data + start_idx, quote_dst_idx - start_idx + 1) );
	        state ^= after_string;
            }
            else
            {
	        args = parse_append(args, mystrndup(data + start_idx, quote_dst_idx - start_idx ) );
            }
	    state ^= in_singlequotes;
	}
	else if ( c == '\\' ){
	    state |= after_escape;
	}
	else {
	    data[quote_dst_idx] = c;
	    ++quote_dst_idx;
	}
    }
    else {
	if ( c == '"' )	{
	    state |= in_doublequotes;
	    start_idx = quote_dst_idx = curr_idx + 1;
	    state ^= after_string;
	}
	else if ( c == '\'' ) {
	    state |= in_singlequotes;
            if ( state & after_string )
            {
	        start_idx = quote_dst_idx = curr_idx;
	        ++quote_dst_idx;
            }
            else
            {
	        start_idx = quote_dst_idx = curr_idx + 1;
            }
	}
	else if ( c == '@' ) {
	    state |= after_string;
	}
        else if ( c != ' ' && c != '\t' && c != '\n' && c != '\r' ){
	    state |= in_plain;
	    start_idx = curr_idx;
	    state ^= after_string;
	}
    }
}
return (args);
}

/*************************************************************************************************

This code is based on Bob Trower's C implementation at http://base64.sourceforge.net/.
Adjusted heavily to fill my needs.

LICENSE:        Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.

                Permission is hereby granted, free of charge, to any person
                obtaining a copy of this software and associated
                documentation files (the "Software"), to deal in the
                Software without restriction, including without limitation
                the rights to use, copy, modify, merge, publish, distribute,
                sublicense, and/or sell copies of the Software, and to
                permit persons to whom the Software is furnished to do so,
                subject to the following conditions:

                The above copyright notice and this permission notice shall
                be included in all copies or substantial portions of the
                Software.
*/

void decodeblock( unsigned char in[4], unsigned char out[3] )
{
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

char *base64_dec(char *arg)
{
static char *decoded = NULL;
static long totalsize = MAX_LEN;

unsigned char in[4], out[3], v;
int i, len;
int cur = 0;
int posit = 0;

/* Only executed at first-time use */
if (decoded == NULL) {
    decoded = (char*)malloc(totalsize*sizeof(char));
    if (decoded == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to allocate Base64 returnvalue: ", strerror(errno));
}

while (cur < strlen(arg)){
    for (len = 0, i = 0; i < 4 && (cur < strlen(arg)); i++){
	v = 0;
	while (cur < strlen(arg) && v == 0){
	    v = (unsigned char) arg[cur];
	    cur++;
	    v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
	    if (v) {v = (unsigned char) ((v == '$') ? 0 : v - 61);}
	}

	if(cur <= strlen(arg)){
	    len++;
	    if (v) in[i] = (unsigned char)(v - 1);
	    else in[i] = 0;
	}
    }
    /* Chek if there is sufficient space */
    if((posit + len - 1) > totalsize) {
	totalsize = posit + len;
	decoded = (char*)realloc(decoded, totalsize*sizeof(char));
	if (decoded == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to allocate Base64 returnvalue: ", strerror(errno));
    }

    if(len){
	decodeblock(in, out);
	for(i = 0; i < len - 1; i++) {
	    decoded[posit + i] = out[i];
	}
	decoded[posit + i] = '\0';
    }
    posit += len - 1;
}
return decoded;
}

void encodeblock( unsigned char *in, unsigned char *out, int len )
{
    out[0] = (unsigned char) cb64[ (int)(in[0] >> 2) ];
    out[1] = (unsigned char) cb64[ (int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ (int)(((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ (int)(in[2] & 0x3f) ] : '=');
}

char *base64_enc( char *arg, int len )
{
static char *encoded = NULL;
static long totalsize = MAX_LEN;

unsigned char in[4], out[4];
int i, j, cur = 0, posit = 0;

/* Only executed at first-time use */
if (encoded == NULL) {
    encoded = (char*)malloc(totalsize*sizeof(char));
    if (encoded == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to allocate Base64 returnvalue: ", strerror(errno));
}

if(arg)
{
    while( cur < len ){
        for( i = 0; i < 3 && (cur + i < len); i++ ){
            in[i] = (unsigned char) arg[cur+i];
        }
        in[i] = 0;
        cur += i;
        if( i > 0 ) {
            encodeblock( in, out, i );

            /* Chek if there is sufficient space */
            if((posit + i - 1) > totalsize) {
	        totalsize = posit + i;
	        encoded = (char*)realloc(encoded, totalsize*sizeof(char));
	        if (encoded == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to allocate Base64 returnvalue: ", strerror(errno));
            }
            for( j = 0; j < 4; j++ ) {
                encoded[posit+j] = out[j];
            }
            posit += j;
	    encoded[posit] = '\0';
        }
    }
}
else
{
    encoded[0] = '\0';
}
return( encoded );
}


/*************************************************************************************************/

#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_MOTIF
static gint gtk_server_timer(TIMEOUT *data)
{
#ifdef GTK_SERVER_GTK1x
gtk_signal_emit_by_name(GTK_OBJECT((GtkWidget*)data->widget), data->signal);
#elif GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
g_signal_emit_by_name(G_OBJECT(data->widget), data->signal);
#elif GTK_SERVER_MOTIF
XtCallCallbacks((Widget)data->widget, data->signal, data->signal);
#endif
return TRUE;
}
#endif

/*************************************************************************************************/
/* This function has grown way to large. Sorry. :-) */

char *Call_Realize (char *inputdata, void* cinv_ctx)
{
char *gtk_api_call;
char *arg = NULL;			/* Temporary argument holders */
PARSED *list;			/* This will contain the list with individual arguments */
int i = 0, j;
BODY *macro;			/* Points to the body of an individual macro */
BODY *Body_Last = NULL;		/* For gtk_server_macro_define */
char *buf;			/* Buffer needed to construct calls in macros */
PARSED *command;		/* Contains the macro textlines */
char* sym = NULL;		/* Contains individual symbols on a line in a macro */
MACRO *Macro_Found;
CONFIG *Call_Found;
ALIAS *Name_Found;
MACRO_ASSOC *M_Assocs;		/* Struct to define Macro Assocs */
ENUM *Enum_Found;
STR *Str_Found;
int macro_found = 0;		/* Needed for MACRO arg type */
char *retstr = NULL;		/* Result holder */
char buffer[MAX_LEN];		/* Buffer to keep macro redefinitions */
char pack[MAX_LEN];		/* Buffer to keep memory layouts for gtk_server_pack */
char *unpack;                   /* Pointer to keep binary data for gtk_server_unpack */
char *b64 = NULL;               /* Pointer to temporarily hold base64 data fir gtk_server_unpack */
int len;                        /* How much data was written in gtk_server_unpack */
int cmd;			/* Macro-parser: what MACRO command are we currently executing? */
int item;			/* If handle from client is given start parsing at item = 1, else item = 0 */
struct utsname pf;              /* For gtk_server_os */
char *arg_type;                 /* Used by VARARGS */
int in_varargs_list = 0;        /* In case we are in a varargs list of arguments */
int position = 0;               /* For gtk_server_pack to keep position in memory layout */

#if GTK_SERVER_FFI || GTK_SERVER_FFCALL
    #ifdef GTK_SERVER_UNIX
    void (*func)();			/* In case of non-returning value */
    #elif GTK_SERVER_WIN32
    FARPROC *func;
    func = (FARPROC*)malloc(sizeof(FARPROC)); /* Do not forget to free this after usage */
    #endif
#else
    void *func;
#endif

#ifdef GTK_SERVER_DYNCALL
DCCallVM* vm = NULL;
#endif

/* Stuff needed for connecting extra signals */
#ifdef GTK_SERVER_GTK1x
GtkSignalQuery query;
GtkType type;
#elif GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
GSignalQuery query;
GType type;
#endif
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
unsigned int signal_id;
TIMEOUT *data;
GtkWidget *opaque; /* Needed for OPAQUE */
ASSOC *Last_List_Sigs = NULL;
#elif GTK_SERVER_MOTIF
CLASS *Class_Found;
TIMEOUT *data;
#endif
char *widget;
char *signal;
char *varname;

#ifdef GTK_SERVER_FFI
ffi_type *argtypes[MAX_ARGS];		/* Contains addresses of the types of the arguments */
void *argvalues[MAX_ARGS];		/* Contains addresses of all arguments */
TYPE theargs[MAX_ARGS];			/* Contains the arguments sent by the client program */
#elif GTK_SERVER_FFCALL
av_alist funclist;
char* string_return;
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
    GtkObject* widget_return;
    #elif GTK_SERVER_XF
    FL_OBJECT* widget_return;
    #elif GTK_SERVER_MOTIF
    Widget widget_return;
    #else
    void *widget_return;
    #endif
int int_return;
long long_return;
float float_return;
double double_return;
void* void_return;
#elif GTK_SERVER_CINV
void *argvalues[MAX_ARGS];		/* Contains addresses of all arguments */
TYPE theargs[MAX_ARGS];			/* Contains the arguments sent by the client program */
char argtypes[MAX_ARGS];		/* Contains type construction */
strcpy(argtypes, "");
#endif

if (inputdata != NULL) {
    /* Get the API call part and split arguments based on spaces (S-expression syntax) */
    list = parse_line(inputdata);
    /* Check if first item contains a handle from client */
    if (gtkserver.behave & 256) {
	item = 1;
	gtkserver.handle = (char*)realloc(gtkserver.handle, (strlen(parse_data(list, 0)) + 2)*sizeof(char));
	strcpy(gtkserver.handle, parse_data(list, 0));
	strcat(gtkserver.handle, " ");
    }
    else item = 0;
    /* Get the call */
    gtk_api_call = parse_data(list, item);
    /* Empty string was sent */
    if (gtk_api_call == NULL){
	retstr = Print_Result("%s%s-1%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
    }
    /* Have we found the GTK-signal call? */
    else if (!strcmp("gtk_server_callback", gtk_api_call)) {
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) != NULL){
	    while (1){
		/* Let GTK wait for an event */
		if (!strcmp (arg, "WAIT") || !strcmp (arg, "wait") || !strcmp(arg, "1")) {
		    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
		    gtk_main_iteration();
		    #elif GTK_SERVER_XF
		    Current_Object.object = fl_do_forms();
		    Current_Object.state = (long)Current_Object.object;
		    #elif GTK_SERVER_MOTIF
		    XtRealizeWidget(gtkserver.toplevel);
		    XtAppProcessEvent(gtkserver.app, XtIMAll);
		    #endif
		}
		/* Update asynchronously */
		if (!strcmp (arg, "UPDATE") || !strcmp (arg, "update") || !strcmp(arg, "2")) {
		    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
		    while (gtk_events_pending()){
			gtk_main_iteration();
			/* Go out when a widget is recognized by GTK-server */
			if ((long)Current_Object.state != GTK_SERVER_NONE) break;
		    }
		    #elif GTK_SERVER_XF
		    Current_Object.object = fl_check_forms();
		    Current_Object.state = (long)Current_Object.object;
		    #elif GTK_SERVER_MOTIF
		    XtRealizeWidget(gtkserver.toplevel);
		    while(XtAppPending(gtkserver.app)){
			XtAppProcessEvent(gtkserver.app, XtIMAll);
			/* Go out when a widget is recognized by GTK-server */
			if ((long)Current_Object.state != GTK_SERVER_NONE) break;
		    }
		    #endif
		}
		/* Find out the callback state for the given object for all arguments to gtk_server_callback */
		if ((long)Current_Object.state != GTK_SERVER_NONE) {
		    /* Signal found */
		    Current_Object.state = GTK_SERVER_NONE;
		    /* Convert long value containing widget ref. to string */
		    if (Current_Object.text == NULL) retstr = Print_Result("%s%s%ld%s", 4, gtkserver.pre, gtkserver.handle, (long)Current_Object.object, gtkserver.post);
		    /* Or return self-defined string */
		    else retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.text, gtkserver.post);
		    break;
		}
		/* Only return with '0' to client if argument was not a WAIT argument */
		if (strcmp (arg, "WAIT") && strcmp (arg, "wait") && strcmp(arg, "1")){
		    retstr = Print_Result("%s%s0%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
		    break;
		}
	    }
	}
	/* GTK-SERVER call found, but no argument given */
	else {
	    Print_Error("%s", 1, "\nERROR: Missing WAIT state GTK_SERVER_CALLBACK!");
	}
    }
    /* Internal call for version */
    else if (!strcmp("gtk_server_version", gtk_api_call)){
	/* Return GTK-server version */
	retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, GTK_SERVER_VERSION, gtkserver.post);
    }
    /* Internal call for enum */
    else if (!strcmp("gtk_server_enum", gtk_api_call)){
	/* Find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find argument in GTK_SERVER_ENUM!");
	}
	HASH_FIND_STR(enum_protos, arg, Enum_Found);
	/* Return GTK-server version */
	retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Enum_Found->value, gtkserver.post);
    }
    /* Internal call for packing data */
    else if (!strcmp("gtk_server_pack", gtk_api_call)){
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL || !strstr(arg,"%")){
	    Print_Error("%s", 1, "\nERROR: Cannot find format in GTK_SERVER_PACK!");
	}
        memset(pack, 0, MAX_LEN);
        position = 0;
        arg_type = strtok(arg, "%");
        /* Isolate mem type */
        while(arg_type) {
            /* Get argument */
	    if ((arg = parse_data(list, ++item)) == NULL){
	        Print_Error("%s%s%s", 3, "\nERROR: Cannot find argument for format '", arg_type, "' in GTK_SERVER_PACK!");
	    }
            /* Determine type */
            switch (*arg_type){
                case 'i':
                    *(int*)(pack+position) = (int)atoi(arg);
                    position += sizeof(int);
                    break;
                case 'l':
                    *(long*)(pack+position) = (long)atol(arg);
                    position += sizeof(long);
                    break;
                case 'f':
                    *(float*)(pack+position) = (float)atof(arg);
                    position += sizeof(float);
                    break;
                case 'd':
                    *(double*)(pack+position) = (double)atof(arg);
                    position += sizeof(double);
                    break;
                case 'c':
                    *(char*)(pack+position) = (char)atoi(arg);
                    position += sizeof(char);
                    break;
                case 's':
                    *(short*)(pack+position) = (short)atoi(arg);
                    position += sizeof(short);
                    break;
                default:
	            Print_Error("%s%s%s", 3, "\nERROR: Format '", arg_type, "' in GTK_SERVER_PACK not recognized! Should be either i, l, d, f, s or c.");
            }
            arg_type = strtok(NULL, "%");
        }
	retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, base64_enc(pack, position), gtkserver.post);
    }
    /* Internal call for unpacking data to S-expression */
    else if (!strcmp("gtk_server_unpack", gtk_api_call)){
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL || !strstr(arg,"%")){
	    Print_Error("%s", 1, "\nERROR: Cannot find format in GTK_SERVER_UNPACK!");
	}
	/* Find the second argument */
	if ((b64 = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find BASE64 string data in GTK_SERVER_UNPACK!");
	}
        unpack = base64_dec(b64);
        memset(pack, 0, MAX_LEN);
        position = 0;
        len = 1;
        arg_type = strtok(arg, "%");
        /* Isolate mem type */
        while(arg_type) {
            /* Determine type */
            switch (*arg_type){
                case 'i':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%i ", *(int*)(unpack+position));
                    position += sizeof(int);
                    break;
                case 'l':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%ld ", *(long*)(unpack+position));
                    position += sizeof(long);
                    break;
                case 'f':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%f ", *(float*)(unpack+position));
                    position += sizeof(float);
                    break;
                case 'd':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%f ", *(double*)(unpack+position));
                    position += sizeof(double);
                    break;
                case 'c':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%i ", *(char*)(unpack+position));
                    position += sizeof(char);
                    break;
                case 's':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%i ", *(short*)(unpack+position));
                    position += sizeof(short);
                    break;
                default:
	            Print_Error("%s%s%s", 3, "\nERROR: Format '", arg_type, "' in GTK_SERVER_UNPACK not recognized! Should be either i, l, d, f, s or c.");
            }
            arg_type = strtok(NULL, "%");
        }
	retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Trim_String(pack), gtkserver.post);
    }
    else if (!strcmp("gtk_server_data_format", gtk_api_call)) {
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL || !strstr(arg,"%")){
	    Print_Error("%s", 1, "\nERROR: Cannot find format in GTK_SERVER_DATA_FORMAT!");
	}
        position = 0;
        arg_type = strtok(arg, "%");
        /* Isolate mem type */
        while(arg_type) {
            /* Determine type */
            switch (*arg_type){
                case 'i':
                    position += sizeof(int);
                    break;
                case 'l':
                    position += sizeof(long);
                    break;
                case 'f':
                    position += sizeof(float);
                    break;
                case 'd':
                    position += sizeof(double);
                    break;
                case 'c':
                    position += sizeof(char);
                    break;
                case 's':
                    position += sizeof(short);
                    break;
                default:
	            Print_Error("%s%s%s", 3, "\nERROR: Format '", arg_type, "' in GTK_SERVER_DATA_FORMAT not recognized! Should be either i, l, d, f, s or c.");
            }
            arg_type = strtok(NULL, "%");
        }
        PTR_BASE64 = position;
	retstr = Print_Result("%sok%s", 2, gtkserver.pre, gtkserver.post);
    }
    /* Code for gtk_server_unpack_from_pointer comes from Jop Brown */
    else if (!strcmp("gtk_server_unpack_from_pointer", gtk_api_call)){
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL || !strstr(arg,"%")){
	    Print_Error("%s", 1, "\nERROR: Cannot find format in GTK_SERVER_UNPACK_FROM_POINTER!");
	}
	/* Find the second argument */
	if ((b64 = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find pointer in GTK_SERVER_UNPACK_FROM_POINTER!");
	}
        unpack = (char*)atol(b64);
        memset(pack, 0, MAX_LEN);
        position = 0;
        len = 1;
        arg_type = strtok(arg, "%");
        /* Isolate mem type */
        while(arg_type) {
            /* Determine type */
            switch (*arg_type){
                case 'i':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%i ", *(int*)(unpack+position));
                    position += sizeof(int);
                    break;
                case 'l':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%ld ", *(long*)(unpack+position));
                    position += sizeof(long);
                    break;
                case 'f':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%f ", *(float*)(unpack+position));
                    position += sizeof(float);
                    break;
                case 'd':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%f ", *(double*)(unpack+position));
                    position += sizeof(double);
                    break;
                case 'c':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%i ", *(char*)(unpack+position));
                    position += sizeof(char);
                    break;
                case 's':
                    len += snprintf(pack+len-1, MAX_LEN-len, "%i ", *(short*)(unpack+position));
                    position += sizeof(short);
                    break;
                default:
	            Print_Error("%s%s%s", 3, "\nERROR: Format '", arg_type, "' in GTK_SERVER_UNPACK not recognized! Should be either i, l, d, f, s or c.");
            }
            arg_type = strtok(NULL, "%");
        }
	retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Trim_String(pack), gtkserver.post);
    }
    /* Code for gtk_server_string_from_pointer comes from Jop Brown */
    else if (!strcmp("gtk_server_string_from_pointer", gtk_api_call)){

	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find pointer in first arg!");
	}

	retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, (char*)atol(arg), gtkserver.post);
    }
    #ifdef GTK_SERVER_MOTIF
    /* Call to get toplevel widget in Motif */
    else if (!strcmp("gtk_server_toplevel", gtk_api_call)){
	/* Return GTK-server version */
	retstr = Print_Result("%s%s%ld%s", 4, gtkserver.pre, gtkserver.handle, (long)gtkserver.toplevel, gtkserver.post);
    }
    #endif
    /* Return FFI used for compilation */
    else if (!strcmp("gtk_server_ffi", gtk_api_call)){
	#ifdef GTK_SERVER_FFI
	retstr = Print_Result("%s%sFFI%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#elif GTK_SERVER_FFCALL
	retstr = Print_Result("%s%sFFCALL%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#elif GTK_SERVER_CINV
	retstr = Print_Result("%s%sC/Invoke%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#elif GTK_SERVER_DYNCALL
	retstr = Print_Result("%s%sDynCall%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#endif
    }
    /* Return toolkit used for compilation */
    else if (!strcmp("gtk_server_toolkit", gtk_api_call)){
	#ifdef GTK_SERVER_GTK1x
	retstr = Print_Result("%s%sGTK1%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#elif GTK_SERVER_GTK2x
	retstr = Print_Result("%s%sGTK2%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#elif GTK_SERVER_GTK3x
	retstr = Print_Result("%s%sGTK3%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#elif GTK_SERVER_XF
	retstr = Print_Result("%s%sXForms%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#elif GTK_SERVER_MOTIF
	retstr = Print_Result("%s%sMotif%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#else
	retstr = Print_Result("%s%sconsole%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#endif
    }
    /* Internal call for OS */
    else if (!strcmp("gtk_server_os", gtk_api_call)){
        if (uname (&pf) < 0)
        {
	    retstr = Print_Result("%s%sUnknown%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
        }
        else
        {
	    retstr = Print_Result("%s%s%s %s on %s%s", 6, gtkserver.pre, gtkserver.handle, pf.sysname, pf.release, pf.machine, gtkserver.post);
        }
    }
    else if (!strcmp("GTK_SERVER_ENABLE_C_STRING_ESCAPING", gtk_api_call) || !strcmp("gtk_server_enable_c_string_escaping", gtk_api_call)){
	gtkserver.c_escaped = 1;
	/* Return OK */
	retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
    }
    else if (!strcmp("GTK_SERVER_DISABLE_C_STRING_ESCAPING", gtk_api_call) || !strcmp("gtk_server_disable_c_string_escaping", gtk_api_call)){
	gtkserver.c_escaped = 0;
	/* Return OK */
	retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
    }
    /* Check if we have a C string redefinition */
    else if (!strcmp("gtk_server_set_c_string_escaping", gtk_api_call)) {
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find argument in GTK_SERVER_SET_C_STRING_ESCAPING!");
	}
	if(strlen(arg)>16){
	    Print_Error("%s", 1, "\nERROR: Argument may not exceed 16 characters in GTK_SERVER_SET_C_STRING_ESCAPING!");
	}
	for(i=0; i<strlen(arg); i++){
	    gtkserver.escapes[i] = arg[i];
	}
	gtkserver.escapes[i] = '\0';
	retstr = Print_Result("%s%sC string escaping set to %s%s", 4, gtkserver.pre, gtkserver.handle, gtkserver.escapes, gtkserver.post);
    }
    else if (!strcmp("GTK_SERVER_ENABLE_PRINT_LINE_COUNT", gtk_api_call) || !strcmp("gtk_server_enable_print_line_count", gtk_api_call)){
	gtkserver.print_line_count = 1;
	/* Return OK */
	retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
    }
    else if (!strcmp("GTK_SERVER_DISABLE_PRINT_LINE_COUNT", gtk_api_call) || !strcmp("gtk_server_disable_print_line_count", gtk_api_call)){
	gtkserver.print_line_count = 0;
	/* Return OK */
	retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
    }
    /* Check if we have a GTK echo function (for debugging & keepalives) */
    else if (!strcmp("gtk_server_echo", gtk_api_call)) {
	while(*inputdata!='\0' && *inputdata != 32) inputdata++;
	if(*inputdata == 32) inputdata++;
	retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, inputdata, gtkserver.post);
    }
    /* Check if the GTK-server must be exited */
    else if (!strcmp("gtk_server_exit", gtk_api_call)) {
	if (gtkserver.mode == 1){
	    printf("ok\n");
	    fflush(stdout);
	}
	#ifdef GTK_SERVER_UNIX
	/* Send signal to parent process */
	if (gtkserver.behave & 8) kill(gtkserver.ppid, gtkserver.exit_sig);
	#endif
	/* OK string */
	retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	#ifdef GTK_SERVER_GTK1x
	gtk_exit(0);
	#elif GTK_SERVER_GTK2x
	gtk_exit(0);
	#elif GTK_SERVER_GTK3x
	gtk_exit(0);
	#elif GTK_SERVER_XF
	fl_finish();
	exit(EXIT_SUCCESS);
	#elif GTK_SERVER_MOTIF
	exit(EXIT_SUCCESS);
	#else
	exit(EXIT_SUCCESS);
	#endif
    }
    #ifdef GTK_SERVER_UNIX
    /* Return the PID of the GTK-server */
    else if (!strcmp("gtk_server_pid", gtk_api_call)) {
	retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, getpid(), gtkserver.post);
    }
    #endif
    /* Check if we have a GTK function (re-)definition. As you can see both 'redefine' and 'define' follow the same
     * path. But the redefine has been here for a long time and I want to maintain backwards compatibility. */
    else if (!strcmp("gtk_server_redefine", gtk_api_call) || !strcmp("gtk_server_define", gtk_api_call)) {
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find argument in GTK_SERVER_REDEFINE!");
	}
	/* Find call */
	HASH_FIND_STR(gtk_protos, arg, Call_Found);
	/* Have we found the GTK call? */
	if (Call_Found != NULL) {
	    /* Get callbacktype */
	    if (parse_data(list, ++item) == NULL) Print_Error("%s", 1, "\nERROR: Missing callbacktype in GTK redefinition!");
	    else {
		Call_Found->callbacktype = (char*)realloc(Call_Found->callbacktype, (strlen(parse_data(list, item))+1)*sizeof(char));
		strcpy(Call_Found->callbacktype, Trim_String(parse_data(list, item)));
	    }
	    /* Get returnvalue */
	    if (parse_data(list, ++item) == NULL) Print_Error("%s", 1, "\nERROR: Missing returnvalue in GTK redefinition!");
	    else {
		Call_Found->returnvalue = (char*)realloc(Call_Found->returnvalue, (strlen(parse_data(list, item))+1)*sizeof(char));
		strcpy(Call_Found->returnvalue, Trim_String(parse_data(list, item)));
	    }
	    /* Get amount of arguments */
	    if (parse_data(list, ++item) == NULL) Print_Error("%s", 1, "\nERROR: Missing argument amount in GTK redefinition!");
	    else if (atoi(parse_data(list, item)) > MAX_ARGS)
		Print_Error("%s%d%s", 3, "\nERROR: GTK definition cannot have more than ", MAX_ARGS, " arguments!");
	    /* Get the separate arguments */
	    else {
		/* Forgot to put this here - thanks to Leonardo Cecchi for finding this bug */
		strncpy(Call_Found->argamount, Trim_String(parse_data(list, item)), MAX_DIG-1);
		/* Now start reading the arguments */
		for (i = 0; i < atoi(parse_data(list, item)); i++){
		    if(parse_data(list, i + 5) != NULL) {
                        if(!strcmp(Trim_String(parse_data(list, i + 5)), "VARARGS")) {
                            snprintf(Call_Found->argamount, MAX_DIG, "%d", MAX_ARGS);
                            for(j = i; j < MAX_ARGS; j++){
                                Call_Found->args[j] = (char*)realloc(Call_Found->args[j], 8);
                                strcpy(Call_Found->args[j], "VARARGS");
                            }
                            break;
                        }
                        else {
			    Call_Found->args[i] = (char*)realloc(Call_Found->args[i], (strlen(parse_data(list, i + 5))+1)*sizeof(char));
			    strcpy(Call_Found->args[i], Trim_String(parse_data(list, i + 5)));
                        }
		    }
		    else Print_Error("%s", 1, "\nERROR: Missing argument in GTK redefinition!");
		}
	    }
	}
	/* The call was not defined before */
	else {
	    Call_Found = (CONFIG*)malloc(sizeof(CONFIG));
	    Call_Found->name = strdup(arg);
	    /* Get next term: callback signal type */
	    if (parse_data(list, ++item) == NULL) Print_Error("%s", 1, "\nERROR: Missing callbacktype in GTK definition!");
	    else {
		Call_Found->callbacktype = (char*)malloc((strlen(parse_data(list, item))+1)*sizeof(char));
		strcpy(Call_Found->callbacktype, Trim_String(parse_data(list, item)));
	    }
	    /* Get next term: return value */
	    if (parse_data(list, ++item) == NULL) Print_Error("%s", 1, "\nERROR: Missing returnvalue in GTK definition!");
	    else {
		Call_Found->returnvalue = (char*)malloc((strlen(parse_data(list, item))+1)*sizeof(char));
		strcpy(Call_Found->returnvalue, Trim_String(parse_data(list, item)));
	    }
	    /* Get next term: amount of arguments */
	    if (parse_data(list, ++item) == NULL) Print_Error("%s", 1, "\nERROR: Missing argument amount in GTK definition!");
	    else if (atoi(parse_data(list, item)) > MAX_ARGS)
		Print_Error("%s%d%s", 3, "\nERROR: GTK definition cannot have more than ", MAX_ARGS, " arguments!");
	    else {
		strncpy(Call_Found->argamount, Trim_String(parse_data(list, item)), MAX_DIG-1);
		/* Arguments to NULL, then get the separate arguments */
		for (i = 0; i < MAX_ARGS; i++){
		    Call_Found->args[i] = NULL;
		}
		/* Now get the separate arguments */
		for (i = 0; i < atoi(parse_data(list, item)); i++){
		    if(parse_data(list, i + 5) != NULL) {
			Call_Found->args[i] = (char*)malloc((strlen(parse_data(list, i + 5))+1)*sizeof(char));
			strcpy(Call_Found->args[i], Trim_String(parse_data(list, i + 5)));
		    }
		    else Print_Error("%s", 1, "\nERROR: Missing argument in GTK definition!");
		}
	    }
	    /* Now add to hash table */
	    HASH_ADD_KEYPTR(hh, gtk_protos, Call_Found->name, strlen(Call_Found->name), Call_Found);
	}
	/* Return OK */
	retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
    }
    else if (!strcmp("gtk_server_macro_var", gtk_api_call)) {
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find macroname in GTK_SERVER_MACRO_VAR!");
	}
	if ((varname = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find variablename in GTK_SERVER_MACRO_VAR!");
	}
	/* Find call */
	HASH_FIND_STR(macro_protos, arg, Macro_Found);
	/* Have we found a macro? Save arguments */
	if (Macro_Found != NULL) {
	    if (*varname < 123 && *varname > 96){
		if (Macro_Found->var[*varname - 97] != NULL) retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Macro_Found->var[*varname - 97], gtkserver.post);
		else retstr = Print_Result("%s%s0%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	    }
	}
	else Print_Error("%s%s%s", 3, "\nERROR: No macro found with name: \"", arg, "\"");
    }
    else if (!strcmp("gtk_server_macro_define", gtk_api_call) || !strcmp("gtk_server_macro_redefine", gtk_api_call)) {
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find macro definition in GTK_SERVER_MACRO_DEFINE!");
	}
	while (arg != NULL) {
	    arg = fmemgets(buffer, MAX_LEN, arg);
	    if(strncmp(Trim_String(buffer), "MACRO", 5)) Print_Error("%s", 1, "\nERROR: Macro without begin in GTK_SERVER_MACRO_DEFINE!");
	    strtok(buffer, " ");
	    varname = strtok(NULL, " ");
	    if (varname == NULL) Print_Error("%s", 1, "\nERROR: Macro without name in GTK_SERVER_MACRO_DEFINE!");
	    /* Check if this is not a duplicate, if so, delete from memory and hash */
	    HASH_FIND_STR(macro_protos, Trim_String(varname), Macro_Found);
	    if (Macro_Found != NULL){
		HASH_DEL(macro_protos, Macro_Found);
		free(Macro_Found->body->text);
		free(Macro_Found->body);
		free(Macro_Found->name);
		free(Macro_Found);
	    }
	    /* Claim memory */
	    Macro_Found = (MACRO*)malloc(sizeof(MACRO));
	    Macro_Found->name = strdup(Trim_String(varname));
	    /* Put all local macro variables to NULL */
	    for(i=0; i<26; i++) Macro_Found->var[i]=NULL;
	    /* Collect macro text */
	    macro = (BODY*)malloc(sizeof(BODY));
	    Macro_Found->body = macro;
	    macro->prev = NULL;
	    while ((arg = fmemgets (buffer, MAX_LEN, arg)) != NULL){
		macro->text = strdup(Trim_String(buffer));
		Body_Last = macro;
		macro->next = (BODY*)malloc(sizeof(BODY));
		macro = macro->next;
		macro->prev = Body_Last;
		macro->next = NULL;
	    }
	    if(strncmp(Trim_String(buffer), "ENDMACRO", 8)) Print_Error("%s", 1, "\nERROR: Macro without end in GTK_SERVER_MACRO_DEFINE!");
	    /* The ENDMACRO is another line, so count it */
	    free(macro);
	    Body_Last->next=NULL;
	    /* Add to hash table */
	    HASH_ADD_KEYPTR(hh, macro_protos, Macro_Found->name, strlen(Macro_Found->name), Macro_Found);
	}
	retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
    }
    else if (!strcmp("gtk_server_require", gtk_api_call)) {
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find libraryname in GTK_SERVER_REQUIRE!");
	}
	/* Skip the first library, this is GTK-server itself */
	i = 1;
	while(libs[i] != NULL && i < MAX_LIBS){
	    /* Lib was defined, check handle */
	    if(!strcmp(libs[i], arg)){
		#ifdef GTK_SERVER_WIN32
		if(handle[i] == NULL) retstr = Print_Result("%s%sGetLastError: %d%s", 4, gtkserver.pre, gtkserver.handle, (unsigned int)GetLastError(), gtkserver.post);
		#else
		if(handle[i] == NULL) retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, dlerror(), gtkserver.post);
		#endif
		else retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
		break;
	    }
	    else i++;
	}
	/* If lib was not defined before, can it be defined and opened? */
	if (i < MAX_LIBS && libs[i] == NULL){
	    /* Add libname */
	    libs[i] = strdup(arg);
	    #if GTK_SERVER_FFCALL || GTK_SERVER_FFI
		#ifdef GTK_SERVER_UNIX
		handle[i] = search_and_open_lib(arg);
		#elif GTK_SERVER_WIN32
		handle[i] = LoadLibrary(arg);
		#endif
	    #elif GTK_SERVER_CINV
		if ((cinv_ctx = cinv_context_create()) == NULL) Print_Error("%s", 1, "\nERROR: Cannot create C/Invoke context!\n");
		handle[i] = search_and_open_lib(arg, cinv_ctx);
	    #elif GTK_SERVER_DYNCALL
		handle[i] = search_and_open_lib(arg);
	    #endif
	    #ifdef GTK_SERVER_WIN32
	    if(handle[i] == NULL) retstr = Print_Result("%s%sGetLastError: %d%s", 4, gtkserver.pre, gtkserver.handle, (unsigned int)GetLastError(), gtkserver.post);
	    #else
	    if(handle[i] == NULL) retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, dlerror(), gtkserver.post);
	    #endif
	    else retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	}
    }
    /* Internal call to connect other signals than defined in configfile */
    else if (!strcmp("gtk_server_connect", gtk_api_call)){
	if ((widget = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find widget reference in GTK_SERVER_CONNECT!");
	}
	if ((signal = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find signal type in GTK_SERVER_CONNECT!");
	}
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_MOTIF
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find response string in GTK_SERVER_CONNECT!");
	}
	/* Optionally, decide if callback needs to return a FALSE (some wildcard unequal to zero) */
	if (parse_data(list, ++item) != NULL){
	    if (atoi (parse_data(list, item)) != 0) i = 1;
	}
	else i = 0;
	/* Allocate memory to memorize attached signals */
	if (Start_List_Sigs == NULL){
	    List_Sigs = (ASSOC*)malloc(sizeof(ASSOC));
	    Start_List_Sigs = List_Sigs;
	    List_Sigs->next = NULL;
	}
	else {
            List_Sigs = Start_List_Sigs;
            while (List_Sigs->next) { List_Sigs = List_Sigs->next; }
	    List_Sigs->next = (ASSOC*)malloc(sizeof(ASSOC));
	    List_Sigs = List_Sigs->next;
	    List_Sigs->next = NULL;
	}
	List_Sigs->data = strdup(arg);
	#endif
	#if GTK_SERVER_XF
	if(isalpha(signal[0]))
	{
	    HASH_FIND_STR(enum_protos, signal, Enum_Found);
	    if (Enum_Found != NULL)
	    {
		fl_register_raw_callback((FL_FORM*)(atol(widget)), Enum_Found->value, xforms_callback);
	    }
	    else
	    {
		Print_Error("\nERROR: Cannot find signal '%s' in GTK_SERVER_CONNECT!", 1, signal);
	    }
	}
	else
	{
	    fl_register_raw_callback((FL_FORM*)(atol(widget)), atoi(signal), xforms_callback);
	}
	#elif GTK_SERVER_MOTIF
	HASH_FIND_STR(str_protos, signal, Str_Found);
	if (Str_Found != NULL){
	    XtAddCallback((Widget)(atol(widget)), Str_Found->value, motif_callback, List_Sigs->data);
	}
	else{
	    XtAddCallback((Widget)(atol(widget)), signal, motif_callback, List_Sigs->data);
	}
	/* This function was greatly improved by Wolfgang Oertl */
	#elif GTK_SERVER_GTK1x
	if(!strncmp(Trim_String(signal), "button-press-event", 18) || !strncmp(Trim_String(signal), "button_press_event", 18) ||
		!strncmp(Trim_String(signal), "button-release-event", 20) || !strncmp(Trim_String(signal), "button_release_event", 20)) {
	    if (i) gtk_signal_connect(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_button_callback_false), (gpointer)List_Sigs->data);
	    else gtk_signal_connect(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_button_callback), (gpointer)List_Sigs->data);
	}
	else if(!strncmp(Trim_String(signal), "key-press-event", 15) || !strncmp(Trim_String(signal), "key_press_event", 15)) {
	    if (i) gtk_signal_connect(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_key_callback_false), (gpointer)List_Sigs->data);
	    else gtk_signal_connect(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_key_callback), (gpointer)List_Sigs->data);
	}
	else {
	    type = GTK_OBJECT_TYPE((GtkObject*)(atol(widget)));
	    signal_id = gtk_signal_lookup(signal, type);
	    if (signal_id == 0) Print_Error("\nERROR: Cannot find signal %s in GTK_SERVER_CONNECT!", 1, signal);
	    query = *gtk_signal_query(signal_id);
	    if (i) gtk_signal_connect(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_server_callbacks[query.nparams+7]), (gpointer)List_Sigs->data);
	    else gtk_signal_connect(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_server_callbacks[query.nparams]), (gpointer)List_Sigs->data);
	}
	#elif GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
	if(!strncmp(Trim_String(signal), "button-press-event", 18) || !strncmp(Trim_String(signal), "button_press_event", 18) ||
		!strncmp(Trim_String(signal), "button-release-event", 20) || !strncmp(Trim_String(signal), "button_release_event", 20)) {
	    if (i) g_signal_connect(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_button_callback_false), (gpointer)List_Sigs->data);
	    else g_signal_connect(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_button_callback), (gpointer)List_Sigs->data);
	}
	else if(!strncmp(Trim_String(signal), "key-press-event", 15) || !strncmp(Trim_String(signal), "key_press_event", 15)) {
	    if (i) g_signal_connect(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_key_callback_false), (gpointer)List_Sigs->data);
	    else g_signal_connect(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_key_callback), (gpointer)List_Sigs->data);
	}
	else if(!strncmp(Trim_String(signal), "scroll-event", 12) || !strncmp(Trim_String(signal), "scroll_event", 12)) {
	    if (i) g_signal_connect(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_scroll_callback_false), (gpointer)List_Sigs->data);
	    else g_signal_connect(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_scroll_callback), (gpointer)List_Sigs->data);
	}
	else {
	    type = G_OBJECT_TYPE((GtkWidget*)(atol(widget)));
	    signal_id = g_signal_lookup(signal, type);
	    if (signal_id == 0) Print_Error("\nERROR: Cannot find signal %s in GTK_SERVER_CONNECT!", 1, signal);
	    g_signal_query(signal_id, &query);
	    if (i) g_signal_connect(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_server_callbacks[query.n_params+7]), (gpointer)List_Sigs->data);
	    else g_signal_connect(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_server_callbacks[query.n_params]), (gpointer)List_Sigs->data);
	}
	#endif
	/* Return OK */
	retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
    }
    /* Return key pressed GTK-server */
    else if (!strcmp("gtk_server_key", gtk_api_call)) {
	retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.key, gtkserver.post);
    }
    else if (!strcmp("gtk_server_mouse", gtk_api_call)) {
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find argument in GTK_SERVER_MOUSE!");
	}
	else {
	    switch (atoi(arg)){
		case 0: retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.mousex, gtkserver.post);
		    break;
		case 1: retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.mousey, gtkserver.post);
		    break;
		case 2: retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, (int)Current_Object.button, gtkserver.post);
		    break;
		#if GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
		case 3: retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, (int)Current_Object.scroll, gtkserver.post);
		    break;
		#endif
		default:
		    Print_Error("%s", 1, "\nERROR: Called gtk_server_mouse with ILLEGAL argument.");
	    }
	}
    }
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_MOTIF
    /* Internal call to disconnect signals
	GTK does not check the userdata, but the address where the userdata is stored.
    */
    else if (!strcmp("gtk_server_disconnect", gtk_api_call)){
	if ((widget = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find widget reference in GTK_SERVER_DISCONNECT!");
	}
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find user data in GTK_SERVER_DISCONNECT!");
	}
	#ifdef GTK_SERVER_MOTIF
	HASH_FIND_STR(str_protos, arg, Str_Found);
	if (Str_Found != NULL){
	    XtRemoveCallback((Widget)(atol(widget)), Str_Found->value, motif_callback, List_Sigs->data);
	}
	else{
	    XtRemoveCallback((Widget)(atol(widget)), arg, motif_callback, List_Sigs->data);
	}
	#else
	/* Find signal userdata in memory */
	List_Sigs = Start_List_Sigs;
	while ((List_Sigs != NULL) && (strcmp(List_Sigs->data, arg))) {
	    Last_List_Sigs = List_Sigs;
	    List_Sigs = List_Sigs->next;
	}
	/* Have we found the signal ? */
	if (List_Sigs != NULL) {
	    #ifdef GTK_SERVER_GTK1x
	    gtk_signal_disconnect_by_data((GtkObject*)(atol(widget)), List_Sigs->data);
	    i = 1;
	    #elif GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
	    i = g_signal_handlers_disconnect_matched((GtkWidget*)(atol(widget)), G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, List_Sigs->data);
	    #endif
	    /* Check if a widget was disconnected? */
	    if(i > 0) {
		/* Remove element from signal list */
		if (Last_List_Sigs == NULL) Start_List_Sigs = List_Sigs->next;
		else Last_List_Sigs->next = List_Sigs->next;
		free(List_Sigs->data);
		free(List_Sigs);
		retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	    }
	    else retstr = Print_Result("%s%sWARNING: No widget with userdata \"%s\" disconnected.%s", 4, gtkserver.pre, gtkserver.handle, List_Sigs->data, gtkserver.post);
	}
	else retstr = Print_Result("%s%sWARNING: Cannot disconnect signal because userdata \"%s\" was not found.%s", 4, gtkserver.pre, gtkserver.handle, arg, gtkserver.post);
	#endif
    }
    /* Internal call to remove a timeout in the IDLE loop of GTK */
    else if (!strcmp("gtk_server_timeout_remove", gtk_api_call)){
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find timeout handle in GTK_SERVER_TIMEOUT_REMOVE!");
	}
	data = (TIMEOUT*)atol(arg);
	#ifdef GTK_SERVER_GTK1x
	gtk_timeout_remove(data->id);
	#elif GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
	g_source_remove(data->id);
	#elif GTK_SERVER_MOTIF
        XtRemoveTimeOut(data->id);
	#endif
	free(data->signal);
	free(data);
	retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
    }
    /* Internal call to define a timeout in the IDLE loop of GTK */
    else if (!strcmp("gtk_server_timeout", gtk_api_call)){
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find timeout value in GTK_SERVER_TIMEOUT!");
	}
	if ((widget = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find widget reference in GTK_SERVER_TIMEOUT!");
	}
	if ((signal = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find signal type in GTK_SERVER_TIMEOUT!");
	}
	/* Save the signal to be emitted by timeout function */
	data = (TIMEOUT*)malloc(sizeof(TIMEOUT));
	data->signal = strdup(signal);
	data->widget = atol(widget);
	/* Setup the timer */
	#ifdef GTK_SERVER_GTK1x
	data->id = gtk_timeout_add(atoi(arg), (GtkFunction)gtk_server_timer, (gpointer)data);
	#elif GTK_SERVER_GTK2x
	data->id = g_timeout_add(atoi(arg), (GtkFunction)gtk_server_timer, (gpointer)data);
	#elif GTK_SERVER_GTK3x
	data->id = g_timeout_add(atoi(arg), (GSourceFunc)gtk_server_timer, (gpointer)data);
	#elif GTK_SERVER_MOTIF
	HASH_FIND_STR(str_protos, signal, Str_Found);
        if (Str_Found != NULL) {
	    free(data->signal);
            data->signal = Str_Found->value;
        }
	data->id = (int)XtAppAddTimeOut(gtkserver.app, atoi(arg), (XtTimerCallbackProc)gtk_server_timer, (XtPointer)data);
	#endif
	/* Return OK */
	retstr = Print_Result("%s%s%ld%s", 4, gtkserver.pre, gtkserver.handle, (long)data, gtkserver.post);
    }
    /* Return key state */
    else if (!strcmp("gtk_server_state", gtk_api_call)) {
	retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.key_state, gtkserver.post);
    }
#endif
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
    /* Call to capture incoming callback values from callback function */
    else if (!strcmp("gtk_server_callback_value", gtk_api_call)){
	/* Yes, find the first argument */
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find value in GTK_SERVER_CALLBACK_VALUE!");
	}
	if ((varname = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find type in GTK_SERVER_CALLBACK_VALUE!");
	}
	switch (atoi(arg)){
	    case 0: retstr = Print_Result("%s%s%ld%s", 4, gtkserver.pre, gtkserver.handle, (long)Current_Object.object, gtkserver.post);
		    break;
	    case 1: if (!strcmp(varname, "STRING"))
			retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t1, gtkserver.post);
		    else if (!strcmp(varname, "INT"))
			retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.p1, gtkserver.post);
		    else if (!strcmp(varname, "POINTER"))
			retstr = Print_Result("%s%s%lu%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t1, gtkserver.post);
		    else Print_Error("%s", 1, "\nERROR: Called gtk_server_callback_value with ILLEGAL argument.");
		    break;
	    case 2: if (!strcmp(varname, "STRING"))
			retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t2, gtkserver.post);
		    else if (!strcmp(varname, "INT"))
			retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.p2, gtkserver.post);
		    else if (!strcmp(varname, "POINTER"))
			retstr = Print_Result("%s%s%lu%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t2, gtkserver.post);
		    else Print_Error("%s", 1, "\nERROR: Called gtk_server_callback_value with ILLEGAL argument.");
		    break;
	    case 3: if (!strcmp(varname, "STRING"))
			retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t3, gtkserver.post);
		    else if (!strcmp(varname, "INT"))
			retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.p3, gtkserver.post);
		    else if (!strcmp(varname, "POINTER"))
			retstr = Print_Result("%s%s%lu%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t3, gtkserver.post);
		    else Print_Error("%s", 1, "\nERROR: Called gtk_server_callback_value with ILLEGAL argument.");
		    break;
	    case 4: if (!strcmp(varname, "STRING"))
			retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t4, gtkserver.post);
		    else if (!strcmp(varname, "INT"))
			retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.p4, gtkserver.post);
		    else if (!strcmp(varname, "POINTER"))
			retstr = Print_Result("%s%s%lu%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t4, gtkserver.post);
		    else Print_Error("%s", 1, "\nERROR: Called gtk_server_callback_value with ILLEGAL argument.");
		    break;
	    case 5: if (!strcmp(varname, "STRING"))
			retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t5, gtkserver.post);
		    else if (!strcmp(varname, "INT"))
			retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.p5, gtkserver.post);
		    else if (!strcmp(varname, "POINTER"))
			retstr = Print_Result("%s%s%lu%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t5, gtkserver.post);
		    else Print_Error("%s", 1, "\nERROR: Called gtk_server_callback_value with ILLEGAL argument.");
		    break;
	    case 6: if (!strcmp(varname, "STRING"))
			retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t6, gtkserver.post);
		    else if (!strcmp(varname, "INT"))
			retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.p6, gtkserver.post);
		    else if (!strcmp(varname, "POINTER"))
			retstr = Print_Result("%s%s%lu%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t6, gtkserver.post);
		    else Print_Error("%s", 1, "\nERROR: Called gtk_server_callback_value with ILLEGAL argument.");
		    break;
	    case 7: if (!strcmp(varname, "STRING"))
			retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t7, gtkserver.post);
		    else if (!strcmp(varname, "INT"))
			retstr = Print_Result("%s%s%d%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.p7, gtkserver.post);
		    else if (!strcmp(varname, "POINTER"))
			retstr = Print_Result("%s%s%lu%s", 4, gtkserver.pre, gtkserver.handle, Current_Object.t7, gtkserver.post);
		    else Print_Error("%s", 1, "\nERROR: Called gtk_server_callback_value with ILLEGAL argument.");
		    break;
	    default:
		Print_Error("%s", 1, "\nERROR: Called gtk_server_callback_value with ILLEGAL argument.");
	}
    }
    /* Internal call to connect other signals than defined in configfile */
    else if (!strcmp("gtk_server_connect_after", gtk_api_call)){
	if ((widget = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find widget reference in GTK_SERVER_CONNECT_AFTER!");
	}
	if ((signal = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find signal type in GTK_SERVER_CONNECT_AFTER!");
	}
	if ((arg = parse_data(list, ++item)) == NULL){
	    Print_Error("%s", 1, "\nERROR: Cannot find response string in GTK_SERVER_CONNECT_AFTER!");
	}
	/* Optionally, decide if callback needs to return a FALSE (some wildcard unequal to zero) */
	if (parse_data(list, ++item) != NULL){
	    if (atoi (parse_data(list, item)) != 0) i = 1;
	}
	else i = 0;
	/* Allocate memory to memorize attached signals - GTK memorizes the address instead of the userdata */
	if (Start_List_Sigs == NULL){
	    List_Sigs = (ASSOC*)malloc(sizeof(ASSOC));
	    Start_List_Sigs = List_Sigs;
	    List_Sigs->next = NULL;
	}
	else {
	    List_Sigs->next = (ASSOC*)malloc(sizeof(ASSOC));
	    List_Sigs = List_Sigs->next;
	    List_Sigs->next = NULL;
	}
	List_Sigs->data = strdup(arg);
	/* This function was greatly improved by Wolfgang Oertl */
	#ifdef GTK_SERVER_GTK1x
	if(!strncmp(Trim_String(signal), "button-press-event", 18) || !strncmp(Trim_String(signal), "button_press_event", 18) ||
		!strncmp(Trim_String(signal), "button-release-event", 20) || !strncmp(Trim_String(signal), "button_release_event", 20)) {
	    if (i) gtk_signal_connect(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_button_callback_false), (gpointer)List_Sigs->data);
	    else gtk_signal_connect(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_button_callback), (gpointer)List_Sigs->data);
	}
	else if(!strncmp(Trim_String(signal), "key-press-event", 15) || !strncmp(Trim_String(signal), "key_press_event", 15)) {
	    if (i) gtk_signal_connect_after(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_key_callback_false), (gpointer)List_Sigs->data);
	    else gtk_signal_connect_after(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_key_callback), (gpointer)List_Sigs->data);
	}
	else {
	    type = GTK_OBJECT_TYPE((GtkObject*)(atol(widget)));
	    signal_id = gtk_signal_lookup(signal, type);
	    if (signal_id == 0) Print_Error("\nERROR: Cannot find signal %s in GTK_SERVER_CONNECT_AFTER!", 1, signal);
	    query = *gtk_signal_query(signal_id);
	    if (i) gtk_signal_connect_after(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_server_callbacks[query.nparams+7]), (gpointer)List_Sigs->data);
	    else gtk_signal_connect_after(GTK_OBJECT((GtkObject*)(atol(widget))), Trim_String(signal), GTK_SIGNAL_FUNC(gtk_server_callbacks[query.nparams]), (gpointer)List_Sigs->data);
	}
	#elif GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
	if(!strncmp(Trim_String(signal), "button-press-event", 18) || !strncmp(Trim_String(signal), "button_press_event", 18) ||
		!strncmp(Trim_String(signal), "button-release-event", 20) || !strncmp(Trim_String(signal), "button_release_event", 20)) {
	    if (i) g_signal_connect_after(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_button_callback_false), (gpointer)List_Sigs->data);
	    else g_signal_connect_after(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_button_callback), (gpointer)List_Sigs->data);
	}
	else if(!strncmp(Trim_String(signal), "key-press-event", 15) || !strncmp(Trim_String(signal), "key_press_event", 15)) {
	    if (i) g_signal_connect_after(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_key_callback_false), (gpointer)List_Sigs->data);
	    else g_signal_connect_after(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_key_callback), (gpointer)List_Sigs->data);
	}
	else if(!strncmp(Trim_String(signal), "scroll-event", 12) || !strncmp(Trim_String(signal), "scroll_event", 12)) {
	    if (i) g_signal_connect_after(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_scroll_callback_false), (gpointer)List_Sigs->data);
	    else g_signal_connect_after(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_scroll_callback), (gpointer)List_Sigs->data);
	}
	else {
	    type = G_OBJECT_TYPE((GtkWidget*)(atol(widget)));
	    signal_id = g_signal_lookup(signal, type);
	    if (signal_id == 0) Print_Error("\nERROR: Cannot find signal %s in GTK_SERVER_CONNECT_AFTER!", 1, signal);
	    g_signal_query(signal_id, &query);
	    if (i) g_signal_connect_after(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_server_callbacks[query.n_params+7]), (gpointer)List_Sigs->data);
	    else g_signal_connect_after(GTK_OBJECT((GtkWidget*)(atol(widget))), Trim_String(signal), G_CALLBACK(gtk_server_callbacks[query.n_params]), (gpointer)List_Sigs->data);
	}
	#endif
	/* Return OK */
	retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
    }
    /* Call to create some memory for GTK widgets like ITER - request by Jean-Marc Farinas */
    else if (!strcmp("gtk_server_opaque", gtk_api_call)){
	opaque = (GtkWidget*)malloc(sizeof(GtkWidget)*8);
	if (opaque == NULL) Print_Error("%s", 1, "\nERROR: Cannot get sufficient memory for GTK_SERVER_OPAQUE!");
	else retstr = Print_Result("%s%s%ld%s", 4, gtkserver.pre, gtkserver.handle, (long)opaque, gtkserver.post);
    }
#endif

    /* No GTK-SERVER call, so check for a genuine GTK call - Check GTK_CONFIG list */
    else {
	/* Find call */
	HASH_FIND_STR(gtk_protos, gtk_api_call, Call_Found);
	/* Nothing found, check the alias list */
	if (Call_Found == NULL) {
	    HASH_FIND_STR(alias_protos, gtk_api_call, Name_Found);
	    if(Name_Found != NULL && Name_Found->real != NULL) Call_Found = Name_Found->real;
	}
	/* Have we found a GTK call? */
	if (Call_Found != NULL) {
	    /* Search function in libraries */
	    #ifdef GTK_SERVER_UNIX
	    i = 0;
	    func = NULL;
	    while (i < MAX_LIBS && func == NULL) {
		if (handle[i] != NULL) *(void**)(&func) = (void*)FROMLIB(handle[i], Call_Found->name);
		i++;
	    }
	    if (func == NULL) {
                Print_Error ("%s%s%s", 3, "\nERROR: the function '", Call_Found->name, "' was defined\n\n\tbut cannot be found in libraries!");
            }
	        #if GTK_SERVER_DYNCALL
	        else {
		    vm = dcNewCallVM(4096);
		    /* Use macro here */
		    dcMode(vm, DC_CALL_C_DEFAULT);
	        }
	        #endif
            #elif GTK_SERVER_WIN32
	    i = 0;
	    *func = NULL;
	    while (i < MAX_LIBS && *func == NULL) {
		if (handle[i] != NULL) *func = (FARPROC)GetProcAddress(handle[i], Call_Found->name);
		i++;
	    }
	    if (*func == NULL) {
                Print_Error ("%s%s%s", 3, "\nERROR: the function '", Call_Found->name, "' was defined\n\nbut cannot be found in libraries!");
            }
	    #endif
	    /* Initialize FFCALL list */
	    #ifdef GTK_SERVER_FFCALL
	    if (!strcmp(Call_Found->returnvalue, "NONE") || !strcmp(Call_Found->returnvalue, "VOID")) av_start_void(funclist, *func);
	    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
	    else if (!strcmp(Call_Found->returnvalue, "WIDGET")) av_start_ptr(funclist, *func, GtkWidget*, &widget_return);
	    #elif GTK_SERVER_XF
	    else if (!strcmp(Call_Found->returnvalue, "WIDGET")) av_start_ptr(funclist, *func, FL_OBJECT*, &widget_return);
	    #elif GTK_SERVER_MOTIF
	    else if (!strcmp(Call_Found->returnvalue, "WIDGET")) av_start_ptr(funclist, *func, Widget, &widget_return);
	    #else
	    else if (!strcmp(Call_Found->returnvalue, "WIDGET")) av_start_ptr(funclist, *func, void*, &widget_return);
	    #endif
	    else if (!strcmp(Call_Found->returnvalue, "STRING")) av_start_ptr(funclist, *func, char*, &string_return);
	    else if (!strcmp(Call_Found->returnvalue, "POINTER")) av_start_ptr(funclist, *func, void*, &void_return);
	    else if (!strcmp(Call_Found->returnvalue, "BOOL")) av_start_int(funclist, *func, &int_return);
	    else if (!strcmp(Call_Found->returnvalue, "INT")) av_start_int(funclist, *func, &int_return);
	    else if (!strcmp(Call_Found->returnvalue, "LONG")) av_start_long(funclist, *func, &long_return);
	    else if (!strcmp(Call_Found->returnvalue, "FLOAT")) av_start_float(funclist, *func, &float_return);
	    else if (!strcmp(Call_Found->returnvalue, "DOUBLE")) av_start_double(funclist, *func, &double_return);
	    #endif
	    /* Get the arguments if there are any */
	    if(atoi(Call_Found->argamount) > 0){
		/* Make sure to read '.' as RADIX for float/double arguments */
		#ifdef GTK_SERVER_UNIX
		setlocale(LC_NUMERIC, "POSIX");
		#elif GTK_SERVER_WIN32
		setlocale(LC_NUMERIC, "English");
		#endif
		/* Determine type of argument */
		for (i = 0; i < atoi(Call_Found->argamount); i++) {
		    arg = parse_data(list, i + 1 + item);
                    /* Check if arg = NULL (no arguments provided) and we are in VARARGS list */
                    if(arg == NULL && in_varargs_list) {
                        snprintf(Call_Found->argamount, MAX_DIG, "%d", i);
                        break;
                    }
		    /* Check if arg is not NULL there should be some value */
		    if (arg == NULL && strcmp(Call_Found->args[i], "NULL")){
                        Print_Error ("%s%s%s", 3, "\nERROR: No value entered where \"", gtk_api_call, "\" expects one!");
                    }
                    /* Check if this a VARARGS */
		    if (!strcmp(Call_Found->args[i], "VARARGS")){
                        in_varargs_list = 1;
                        if(!strcmp(arg, "NULL")){
                            strcpy(Call_Found->args[i], "NULL");
                        }
                        else {
                            /* Isolate type */
                            arg_type = strtok(arg, ":");
                            switch (*arg_type){
                                case 'i':
                                    strcpy(Call_Found->args[i], "INT");
                                    break;
                                case 'e':
                                    strcpy(Call_Found->args[i], "ENUM");
                                    break;
                                case 'l':
                                    strcpy(Call_Found->args[i], "LONG");
                                    break;
                                case 's':
                                    strcpy(Call_Found->args[i], "STRING");
                                    break;
                                case 'd':
                                    strcpy(Call_Found->args[i], "DOUBLE");
                                    break;
                                case 'f':
                                    strcpy(Call_Found->args[i], "FLOAT");
                                    break;
                                case 'b':
                                    strcpy(Call_Found->args[i], "BOOL");
                                    break;
                                case 'w':
                                    strcpy(Call_Found->args[i], "WIDGET");
                                    break;
                                case 'p':
                                    strcpy(Call_Found->args[i], "POINTER");
                                    break;
                            }
                            /* Set the real argument */
                            arg = strtok(NULL, ":");
                        }
                    }
		    /* Check if we have data to INT argument type */
		    if (!strcmp(Call_Found->args[i], "INT") || !strcmp(Call_Found->args[i], "ENUM")){
			/* First check if we have an enumeration */
			HASH_FIND_STR(enum_protos, arg, Enum_Found);
			if (Enum_Found != NULL){
			    #ifdef GTK_SERVER_FFI
			    theargs[i].ivalue = Enum_Found->value;
			    argtypes[i] = &ffi_type_uint;
			    argvalues[i] = &theargs[i].ivalue;
			    #elif GTK_SERVER_FFCALL
			    av_int(funclist, Enum_Found->value);
			    #elif GTK_SERVER_CINV
			    strcat(argtypes, "i");
			    theargs[i].ivalue = Enum_Found->value;
			    argvalues[i] = &theargs[i].ivalue;
			    #elif GTK_SERVER_DYNCALL
			    dcArgInt(vm, Enum_Found->value);
			    #endif
			}
			/* It is an int. */
			else {
			    #ifdef GTK_SERVER_FFI
			    theargs[i].ivalue = (int)atoi(arg);
			    argtypes[i] = &ffi_type_uint;
			    argvalues[i] = &theargs[i].ivalue;
			    #elif GTK_SERVER_FFCALL
			    av_int(funclist, (int)atoi(arg));
			    #elif GTK_SERVER_CINV
			    strcat(argtypes, "i");
			    theargs[i].ivalue = (int)atoi(arg);
			    argvalues[i] = &theargs[i].ivalue;
			    #elif GTK_SERVER_DYNCALL
			    dcArgInt(vm, (int)atoi(arg));
			    #endif
			}
		    }
		    else if (!strcmp(Call_Found->args[i], "LONG") || !strcmp(Call_Found->args[i], "ENUM")){
			/* First check if we have an enumeration */
			HASH_FIND_STR(enum_protos, arg, Enum_Found);
			if (Enum_Found != NULL){
			    #ifdef GTK_SERVER_FFI
			    theargs[i].lvalue = Enum_Found->value;
			    argtypes[i] = &ffi_type_ulong;
			    argvalues[i] = &theargs[i].lvalue;
			    #elif GTK_SERVER_FFCALL
			    av_long(funclist, Enum_Found->value);
			    #elif GTK_SERVER_CINV
			    strcat(argtypes, "l");
			    theargs[i].lvalue = Enum_Found->value;
			    argvalues[i] = &theargs[i].lvalue;
			    #elif GTK_SERVER_DYNCALL
			    dcArgLong(vm, Enum_Found->value);
			    #endif
			}
			/* It is a LONG */
			else {
			    #ifdef GTK_SERVER_FFI
			    theargs[i].lvalue = (long)atol(arg);
			    argtypes[i] = &ffi_type_ulong;
			    argvalues[i] = &theargs[i].lvalue;
			    #elif GTK_SERVER_FFCALL
			    av_long(funclist, (long)atol(arg));
			    #elif GTK_SERVER_CINV
			    strcat(argtypes, "l");
			    theargs[i].lvalue = (long)atol(arg);
			    argvalues[i] = &theargs[i].lvalue;
			    #elif GTK_SERVER_DYNCALL
			    dcArgLong(vm, (long)atol(arg));
			    #endif
			}
		    }
		    else if (!strcmp(Call_Found->args[i], "WIDGET")) {
			#ifdef GTK_SERVER_FFI
			    #ifdef GTK_SERVER_XF
			    theargs[i].wvalue = (!strcmp(arg, "NULL") ? NULL : (FL_OBJECT*)(atol(arg)));
			    #elif GTK_SERVER_MOTIF
			    theargs[i].wvalue = (!strcmp(arg, "NULL") ? NULL : (Widget)(atol(arg)));
			    #elif GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
			    theargs[i].wvalue = (!strcmp(arg, "NULL") ? NULL : (GtkObject*)(atol(arg)));
			    #elif GTK_SERVER_GTK3x
			    theargs[i].wvalue = (!strcmp(arg, "NULL") ? NULL : (GtkWidget*)(atol(arg)));
			    #else
			    theargs[i].wvalue = (!strcmp(arg, "NULL") ? NULL : (void*)(atol(arg)));
			    #endif
			    argtypes[i] = &ffi_type_pointer;
			    argvalues[i] = &theargs[i].wvalue;
			#elif GTK_SERVER_FFCALL
			    #ifdef GTK_SERVER_XF
			    av_ptr(funclist, FL_OBJECT*, (!strcmp(arg, "NULL") ? NULL : (FL_OBJECT*)(atol(arg))));
			    #elif GTK_SERVER_MOTIF
			    av_ptr(funclist, Widget, (!strcmp(arg, "NULL") ? NULL : (Widget)(atol(arg))));
			    #elif GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
			    av_ptr(funclist, GtkObject*, (!strcmp(arg, "NULL") ? NULL : (GtkObject*)(atol(arg))));
			    #else
			    av_ptr(funclist, void*, (!strcmp(arg, "NULL") ? NULL : (void*)(atol(arg))));
			    #endif
			#elif GTK_SERVER_CINV
			    #ifdef GTK_SERVER_XF
			    theargs[i].wvalue = (!strcmp(arg, "NULL") ? NULL : (FL_OBJECT*)(atol(arg)));
			    #elif GTK_SERVER_MOTIF
			    theargs[i].wvalue = (!strcmp(arg, "NULL") ? NULL : (Widget)(atol(arg)));
			    #elif GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
			    theargs[i].wvalue = (!strcmp(arg, "NULL") ? NULL : (GtkObject*)(atol(arg)));
			    #elif GTK_SERVER_GTK3x
			    theargs[i].wvalue = (!strcmp(arg, "NULL") ? NULL : (GtkWidget*)(atol(arg)));
			    #else
			    theargs[i].wvalue = (!strcmp(arg, "NULL") ? NULL : (void*)(atol(arg)));
			    #endif
			    strcat(argtypes, "p");
			    argvalues[i] = &theargs[i].wvalue;
			#elif GTK_SERVER_DYNCALL
			    #ifdef GTK_SERVER_XF
			    dcArgPointer(vm, (!strcmp(arg, "NULL") ? NULL : (FL_OBJECT*)atol(arg)));
			    #elif GTK_SERVER_MOTIF
			    dcArgPointer(vm, (!strcmp(arg, "NULL") ? NULL : (Widget)atol(arg)));
			    #elif GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
			    dcArgPointer(vm, (!strcmp(arg, "NULL") ? NULL : (GtkObject*)atol(arg)));
			    #else
			    dcArgPointer(vm, (!strcmp(arg, "NULL") ? NULL : (void*)atol(arg)));
			    #endif
			#endif
		    }
		    #ifdef GTK_SERVER_MOTIF
		    else if (!strcmp(Call_Found->args[i], "CLASS")){
			HASH_FIND_STR(class_protos, arg, Class_Found);
			#ifdef GTK_SERVER_FFI
			theargs[i].pvalue = Class_Found->ptr;
			argtypes[i] = &ffi_type_pointer;
			argvalues[i] = &theargs[i].pvalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, void*, Class_Found->ptr);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].pvalue = Class_Found->ptr;
			argvalues[i] = &theargs[i].pvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, Class_Found->ptr);
			#endif
		    }
		    #endif
		    else if (!strcmp(Call_Found->args[i], "POINTER")){
			#ifdef GTK_SERVER_FFI
			theargs[i].pvalue = (!strcmp(arg, "NULL") ? NULL : (void*)atol(arg));
			argtypes[i] = &ffi_type_pointer;
			argvalues[i] = &theargs[i].pvalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, void*, (!strcmp(arg, "NULL") ? NULL : (void*)atol(arg)));
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].pvalue = (!strcmp(arg, "NULL") ? NULL : (void*)atol(arg));
			argvalues[i] = &theargs[i].pvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, (!strcmp(arg, "NULL") ? NULL : (void*)atol(arg)));
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "STRING") || !strcmp(Call_Found->args[i], "STR")){
			/* First check if we have a string */
			HASH_FIND_STR(str_protos, arg, Str_Found);
			if (Str_Found != NULL){
			    #ifdef GTK_SERVER_FFI
			    theargs[i].pvalue = (char*)Str_Found->value;
			    argtypes[i] = &ffi_type_pointer;
			    argvalues[i] = &theargs[i].pvalue;
			    #elif GTK_SERVER_FFCALL
			    av_ptr(funclist, char*, (char*)Str_Found->value);
			    #elif GTK_SERVER_CINV
			    strcat(argtypes, "p");
			    theargs[i].pvalue = (char*)Str_Found->value;
			    argvalues[i] = &theargs[i].pvalue;
			    #elif GTK_SERVER_DYNCALL
			    dcArgPointer(vm, (char*)Str_Found->value);
			    #endif
			}
			/* It is a STRING */
			else {
			    #ifdef GTK_SERVER_FFI
			    theargs[i].pvalue = (char*)arg;
			    argtypes[i] = &ffi_type_pointer;
			    argvalues[i] = &theargs[i].pvalue;
			    #elif GTK_SERVER_FFCALL
			    av_ptr(funclist, char*, (char*)arg);
			    #elif GTK_SERVER_CINV
			    strcat(argtypes, "p");
			    theargs[i].pvalue = (char*)arg;
			    argvalues[i] = &theargs[i].pvalue;
			    #elif GTK_SERVER_DYNCALL
			    dcArgPointer(vm, (char*)arg);
			    #endif
			}
		    }
		    /* The booltype in GTK is actually a value of 0 or non-0 */
		    else if (!strcmp(Call_Found->args[i], "BOOL")){
			#ifdef GTK_SERVER_FFI
			theargs[i].lvalue = (int)atoi(arg);
			argtypes[i] = &ffi_type_uint;
			argvalues[i] = &theargs[i].lvalue;
			#elif GTK_SERVER_FFCALL
			av_int(funclist, (int)atoi(arg));
			#elif GTK_SERVER_CINV
			strcat(argtypes, "i");
			theargs[i].lvalue = (int)atoi(arg);
			argvalues[i] = &theargs[i].lvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgBool(vm, (int)atoi(arg));
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "DOUBLE")){
			#ifdef GTK_SERVER_FFI
			theargs[i].dvalue = (double)strtod(arg, NULL);
			argtypes[i] = &ffi_type_double;
			argvalues[i] = &theargs[i].dvalue;
			#elif GTK_SERVER_FFCALL
			av_double(funclist, (double)strtod(arg, NULL));
			#elif GTK_SERVER_CINV
			strcat(argtypes, "d");
			theargs[i].dvalue = (double)strtod(arg, NULL);
			argvalues[i] = &theargs[i].dvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgDouble(vm, (double)strtod(arg, NULL));
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "FLOAT")){
			#ifdef GTK_SERVER_FFI
			theargs[i].fvalue = (float)strtod(arg, NULL);
			argtypes[i] = &ffi_type_float;
			argvalues[i] = &theargs[i].fvalue;
			#elif GTK_SERVER_FFCALL
			av_float(funclist, (float)strtod(arg, NULL));
			#elif GTK_SERVER_CINV
			strcat(argtypes, "f");
			theargs[i].fvalue = (float)strtod(arg, NULL);
			argvalues[i] = &theargs[i].fvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgFloat(vm, (float)strtod(arg, NULL));
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "BASE64")){
			#ifdef GTK_SERVER_FFI
			theargs[i].pvalue = (char*)base64_dec(arg);
			argtypes[i] = &ffi_type_pointer;
			argvalues[i] = &theargs[i].pvalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, char*, (char*)base64_dec(arg));
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].pvalue = (char*)base64_dec(arg);
			argvalues[i] = &theargs[i].pvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, (char*)base64_dec(arg));
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "MACRO")){
			#ifdef GTK_SERVER_FFI
			theargs[i].lvalue = (long)&gtk_func;
			argtypes[i] = &ffi_type_ulong;
			argvalues[i] = &theargs[i].lvalue;
			#elif GTK_SERVER_FFCALL
			av_long(funclist, (long)&gtk_func);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "l");
			theargs[i].lvalue = (long)&gtk_func;
			argvalues[i] = &theargs[i].lvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgLong(vm, (long)&gtk_func);
			#endif
			/* Create hash from macroname, store it */
			Macro_Type = (ARG*)malloc(sizeof(ARG));
			Macro_Type->name = strdup(arg);
			Macro_Type->data = NULL;
			/* Now add to hash table */
			HASH_ADD_KEYPTR(hh, arg_protos, Macro_Type->name, strlen(Macro_Type->name), Macro_Type);
			/* We found a macro argument */
			macro_found = 1;
		    }
		    else if(!strcmp(Call_Found->args[i], "DATA")){
			if (macro_found == 0) Print_Error("%s", 1, "\nERROR: No MACRO type found for DATA type!");
			/* Only 1 argument can be given */
			if (Macro_Type->data != NULL) free(Macro_Type->data);
			/* Save argument */
			if (!strcmp(arg, "NULL")) Macro_Type->data = strdup("0");
			else Macro_Type->data = strdup(arg);
			/* Make sure to pass the MACRO name to the callback func */
			#ifdef GTK_SERVER_FFI
			theargs[i].pvalue = (char*)Macro_Type->name;
			argtypes[i] = &ffi_type_pointer;
			argvalues[i] = &theargs[i].pvalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, char*, (char*)Macro_Type->name);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].pvalue = (char*)Macro_Type->name;
			argvalues[i] = &theargs[i].pvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, (char*)Macro_Type->name);
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "NULL")){
			#ifdef GTK_SERVER_FFI
			theargs[i].pvalue = NULL;
			argtypes[i] = &ffi_type_pointer;
			argvalues[i] = &theargs[i].pvalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, char*, NULL);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].pvalue = NULL;
			argvalues[i] = &theargs[i].pvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, NULL);
			#endif
		    }
		    /* These are the types needed for retrieving values at pointer arguments - idea by Ben Kelly */
		    else if (!strcmp(Call_Found->args[i], "PTR_LONG")) {
			long_address[i]=(long)atol(arg);
			#ifdef GTK_SERVER_FFI
			argtypes[i] = &ffi_type_pointer;
			theargs[i].p_lvalue = &long_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_lvalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, long*, &long_address[i]);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].p_lvalue = &long_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_lvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, &long_address[i]);
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "PTR_INT") || !strcmp(Call_Found->args[i], "PTR_BOOL")) {
			int_address[i]=(int)atoi(arg);
			#ifdef GTK_SERVER_FFI
			argtypes[i] = &ffi_type_pointer;
			theargs[i].p_ivalue = &int_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_ivalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, int*, &int_address[i]);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].p_ivalue = &int_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_ivalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, &int_address[i]);
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "PTR_SHORT")) {
			short_address[i]=(short)atoi(arg);
			#ifdef GTK_SERVER_FFI
			argtypes[i] = &ffi_type_pointer;
			theargs[i].p_hvalue = &short_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_hvalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, short*, &short_address[i]);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].p_hvalue = &short_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_hvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, &short_address[i]);
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "PTR_FLOAT")) {
			float_address[i]=(float)strtod(arg, NULL);
			#ifdef GTK_SERVER_FFI
			argtypes[i] = &ffi_type_pointer;
			theargs[i].p_fvalue = &float_address[i];	/* Assign real variable address */
			argvalues[i] = &theargs[i].p_fvalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, float*, &float_address[i]);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].p_fvalue = &float_address[i];	/* Assign real variable address */
			argvalues[i] = &theargs[i].p_fvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, &float_address[i]);
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "PTR_DOUBLE")) {
			double_address[i]=(double)strtod(arg, NULL);
			#ifdef GTK_SERVER_FFI
			argtypes[i] = &ffi_type_pointer;
			theargs[i].p_dvalue = &double_address[i];	/* Assign real variable address */
			argvalues[i] = &theargs[i].p_dvalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, double*, &double_address[i]);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].p_dvalue = &double_address[i];	/* Assign real variable address */
			argvalues[i] = &theargs[i].p_dvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, &double_address[i]);
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "PTR_WIDGET")) {
			#ifdef GTK_SERVER_XF
			obj_address[i] = (FL_OBJECT*)(atol(arg));
			#elif GTK_SERVER_MOTIF
			obj_address[i] = (Widget)(atol(arg));
			#elif GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
			obj_address[i] = (GtkObject*)(atol(arg));
			#elif GTK_SERVER_GTK3x
			obj_address[i] = (GtkWidget*)(atol(arg));
			#else
			obj_address[i] = (void*)(atol(arg));
			#endif
			#ifdef GTK_SERVER_FFI
			argtypes[i] = &ffi_type_pointer;
			theargs[i].p_wvalue = obj_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_wvalue;
			#elif GTK_SERVER_FFCALL
			    #ifdef GTK_SERVER_XF
			    av_ptr(funclist, FL_OBJECT*, obj_address[i]);
			    #elif GTK_SERVER_MOTIF
			    av_ptr(funclist, Widget, obj_address[i]);
			    #elif GTK_SERVER_GTK1x || GTK_SERVER_GTK2x
			    av_ptr(funclist, GtkObject*, obj_address[i]);
			    #elif GTK_SERVER_GTK3x
			    av_ptr(funclist, GtkWidget*, obj_address[i]);
			    #else
			    av_ptr(funclist, void*, obj_address[i]);
			    #endif
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].p_wvalue = obj_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_wvalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, obj_address[i]);
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "PTR_STRING")) {
			str_address[i] = (char*)(atol(arg));
			#ifdef GTK_SERVER_FFI
			argtypes[i] = &ffi_type_pointer;
			theargs[i].p_svalue = str_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_svalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, char*, str_address[i]);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].p_svalue = str_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_svalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, str_address[i]);
			#endif
		    }
		    else if (!strcmp(Call_Found->args[i], "PTR_BASE64")) {
			str_address[i] = (char*)(calloc(PTR_BASE64, 1));
			#ifdef GTK_SERVER_FFI
			argtypes[i] = &ffi_type_pointer;
			theargs[i].p_svalue = str_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_svalue;
			#elif GTK_SERVER_FFCALL
			av_ptr(funclist, char*, str_address[i]);
			#elif GTK_SERVER_CINV
			strcat(argtypes, "p");
			theargs[i].p_svalue = str_address[i];		/* Assign real variable address */
			argvalues[i] = &theargs[i].p_svalue;
			#elif GTK_SERVER_DYNCALL
			dcArgPointer(vm, str_address[i]);
			#endif
                    }
                    else {
                        Print_Error ("%s%s%s", 3, "\nERROR: Unrecognized type for argument: \"", Call_Found->args[i], "\"");
                    }
                    /* Restore VARARGS prototype */
                    if(in_varargs_list) {
                        strcpy(Call_Found->args[i], "VARARGS");
                    }
		}
		if (macro_found && Macro_Type->data == NULL) Print_Error("%s", 1, "\nERROR: No DATA found for MACRO type!");
	    }
	    /* The GTK CALL has no return value */
	    if (!strcmp(Call_Found->returnvalue, "NONE") || !strcmp(Call_Found->returnvalue, "VOID")){
		#ifdef GTK_SERVER_FFI
		retstr = Void_GUI(func, Call_Found, argtypes, argvalues);
		#elif GTK_SERVER_FFCALL
		av_call(funclist);
		retstr = Void_GUI(Call_Found);
		#elif GTK_SERVER_CINV
		retstr = Void_GUI(func, Call_Found, argtypes, argvalues, cinv_ctx);
		#elif GTK_SERVER_DYNCALL
		retstr = Void_GUI(func, Call_Found, vm);
		#endif
	    }
	    /* The GTK CALL has a widget return value */
	    else if (!strcmp(Call_Found->returnvalue, "WIDGET")){
		#ifdef GTK_SERVER_FFI
		retstr = Widget_GUI(func, Call_Found, argtypes, argvalues);
		#elif GTK_SERVER_FFCALL
		av_call(funclist);
		retstr = Widget_GUI(widget_return, Call_Found);
		#elif GTK_SERVER_CINV
		retstr = Widget_GUI(func, Call_Found, argtypes, argvalues, cinv_ctx);
		#elif GTK_SERVER_DYNCALL
		retstr = Widget_GUI(func, Call_Found, vm);
		#endif
	    }
    	    /* The call has a generic pointer return value (other libraries) */
	    else if (!strcmp(Call_Found->returnvalue, "POINTER")){
		#ifdef GTK_SERVER_FFI
		retstr = Pointer_OBJ(func, Call_Found, argtypes, argvalues);
		#elif GTK_SERVER_FFCALL
		av_call(funclist);
		retstr = Pointer_OBJ(void_return, Call_Found);
		#elif GTK_SERVER_CINV
		retstr = Pointer_OBJ(func, Call_Found, argtypes, argvalues, cinv_ctx);
		#elif GTK_SERVER_DYNCALL
		retstr = Pointer_OBJ(func, Call_Found, vm);
		#endif
	    }
	    /* The GTK CALL delivers a string as a returnvalue */
	    else if (!strcmp(Call_Found->returnvalue, "STRING")){
		#ifdef GTK_SERVER_FFI
		retstr = String_GUI(func, Call_Found, argtypes, argvalues);
		#elif GTK_SERVER_FFCALL
		av_call(funclist);
		retstr = String_GUI(string_return, Call_Found);
		#elif GTK_SERVER_CINV
		retstr = String_GUI(func, Call_Found, argtypes, argvalues, cinv_ctx);
		#elif GTK_SERVER_DYNCALL
		retstr = String_GUI(func, Call_Found, vm);
		#endif
	    }
	    /* The GTK CALL delivers a boolean as a returnvalue */
	    else if (!strcmp(Call_Found->returnvalue, "BOOL")){
		#ifdef GTK_SERVER_FFI
		retstr = Bool_GUI(func, Call_Found, argtypes, argvalues);
		#elif GTK_SERVER_FFCALL
		av_call(funclist);
		retstr = Bool_GUI(int_return, Call_Found);
		#elif GTK_SERVER_CINV
		retstr = Bool_GUI(func, Call_Found, argtypes, argvalues, cinv_ctx);
		#elif GTK_SERVER_DYNCALL
		retstr = Bool_GUI(func, Call_Found, vm);
		#endif
	    }
	    /* The GTK CALL delivers an INTEGER as a returnvalue */
	    else if (!strcmp(Call_Found->returnvalue, "INT")){
		#ifdef GTK_SERVER_FFI
		retstr = Int_GUI(func, Call_Found, argtypes, argvalues);
		#elif GTK_SERVER_FFCALL
		av_call(funclist);
		retstr = Int_GUI(int_return, Call_Found);
		#elif GTK_SERVER_CINV
		retstr = Int_GUI(func, Call_Found, argtypes, argvalues, cinv_ctx);
		#elif GTK_SERVER_DYNCALL
		retstr = Int_GUI(func, Call_Found, vm);
		#endif
	    }
    	    /* The GTK CALL delivers a LONG as a returnvalue */
	    else if (!strcmp(Call_Found->returnvalue, "LONG")){
		#ifdef GTK_SERVER_FFI
		retstr = Long_GUI(func, Call_Found, argtypes, argvalues);
		#elif GTK_SERVER_FFCALL
		av_call(funclist);
		retstr = Long_GUI(long_return, Call_Found);
		#elif GTK_SERVER_CINV
		retstr = Long_GUI(func, Call_Found, argtypes, argvalues, cinv_ctx);
		#elif GTK_SERVER_DYNCALL
		retstr = Long_GUI(func, Call_Found, vm);
		#endif
	    }
	    /* The GTK CALL delivers a FLOATING POINT as a returnvalue */
	    else if (!strcmp(Call_Found->returnvalue, "FLOAT")){
		#ifdef GTK_SERVER_FFI
		retstr = Float_GUI(func, Call_Found, argtypes, argvalues, 1);
		#elif GTK_SERVER_FFCALL
		av_call(funclist);
		retstr = Float_GUI(float_return, Call_Found);
		#elif GTK_SERVER_CINV
		retstr = Float_GUI(func, Call_Found, argtypes, argvalues, cinv_ctx, 1);
		#elif GTK_SERVER_DYNCALL
		retstr = Float_GUI(func, Call_Found, 1, vm);
		#endif
	    }
	    /* The GTK CALL delivers a DOUBLE as a returnvalue */
	    else if (!strcmp(Call_Found->returnvalue, "DOUBLE")){
		#ifdef GTK_SERVER_FFI
		retstr = Float_GUI(func, Call_Found, argtypes, argvalues, 2);
		#elif GTK_SERVER_FFCALL
		av_call(funclist);
		retstr = Float_GUI(double_return, Call_Found);
		#elif GTK_SERVER_CINV
		retstr = Float_GUI(func, Call_Found, argtypes, argvalues, cinv_ctx, 2);
		#elif GTK_SERVER_DYNCALL
		retstr = Float_GUI(func, Call_Found, 2, vm);
		#endif
	    }
	    /* The GTK CALL should return its address (needed for glutBitmap for example - (not really GTK)) */
	    else if (!strcmp(Call_Found->returnvalue, "ADDRESS")){
		retstr = Print_Result("%s%s%ld%s", 4, gtkserver.pre, gtkserver.handle, (long)func, gtkserver.post);
	    }
	    /* Other return value (future expansion) */
	    else {
		Print_Error("%s%s", 2, "\nERROR: Unknown returnvalue found for GUI call: ", Call_Found->returnvalue);
	    }
            /* Restore argument amount when we are in VARARGS */
            if(in_varargs_list) {
                snprintf(Call_Found->argamount, MAX_DIG, "%d", MAX_ARGS);
            }
	}
	/* Check if we have a macroname */
	else {
	    /* Find call */
	    HASH_FIND_STR(macro_protos, gtk_api_call, Macro_Found);
	    /* Nothing found, check the alias list */
	    if (Macro_Found == NULL) {
		HASH_FIND_STR(alias_protos, gtk_api_call, Name_Found);
		if(Name_Found != NULL && Name_Found->real_m != NULL) Macro_Found = Name_Found->real_m;
	    }
	    /* Have we found a macro? Save arguments */
	    if (Macro_Found != NULL) {
		for (i = item; i < 10; i++){
		    Macro_Found->args[i] = parse_data(list, i);
		}
		buf = (char*)malloc(sizeof(char)*MAX_LEN);
		/**********************************************************************************
		    Now start parsing and executing macro - Start of the Macro Object Mini Parser
		    Implementing only call flow and assignment. Functions should be imported.
		**********************************************************************************/
		macro = Macro_Found->body;
		while (macro != NULL){
		    /* Get the commandstring */
		    command = parse_line(macro->text);
		    /* The 'assignment' operator? */
		    if (parse_data(command, 1) != NULL && !strcmp(parse_data(command, 1), ":")) {i = 2; cmd = 0;}
		    /* The 'EMPTY' operator? */
		    else if (parse_data(command, 1) != NULL && !strcmp(parse_data(command, 1), "EMPTY")) {i = 2; cmd = 1;}
		    /* The 'VALUE' operator? */
		    else if (parse_data(command, 1) != NULL && !strcmp(parse_data(command, 1), "VALUE")) {i = 2; cmd = 2;}
		    /* The 'JUMP' operator? */
		    else if (parse_data(command, 0) != NULL && !strcmp(parse_data(command, 0), "JUMP")) {i = 1; cmd = 3;}
		    /* The 'ASSOC' operator? */
		    else if (parse_data(command, 1) != NULL && !strcmp(parse_data(command, 1), "ASSOC")) {i = 2; cmd = 4;}
		    /* The 'GET' operator? */
		    else if (parse_data(command, 1) != NULL && !strcmp(parse_data(command, 1), "GET")) {i = 2; cmd = 5;}
		    /* The 'COMPARE' operator? */
		    else if (parse_data(command, 1) != NULL && !strcmp(parse_data(command, 1), "COMPARE")) {i = 2; cmd = 6;}
		    /* The 'DEBUG' operator? */
		    else if (parse_data(command, 0) != NULL && !strcmp(parse_data(command, 0), "DEBUG")) {i = 1; cmd = 14;}
		    /* The 'RETURN' operator? */
		    else if (parse_data(command, 0) != NULL && !strcmp(parse_data(command, 0), "RETURN")) {i = 0; cmd = 15;}
		    /* Regular GTK function */
		    else {i = 0; cmd = 16;}
		    /* Get first relevant item, if empty submit error */
		    if(parse_data(command, i) != NULL) strncpy(buf, parse_data(command, i), MAX_LEN);
		    else Print_Error("%s%s%s%s", 4, "\nERROR: Illegal syntax in macro: ", Macro_Found->name, "\n\nError in line: ", macro->text);
		    i++;
		    /* Get terms one by one, create buffer after operator */
		    while ((sym = parse_data(command, i)) != NULL){
			mystrcat(&buf, " ");
			/* Is this an arg or a variable or '@'? */
			if (*sym == 36){
			    sym++;
			    if (*sym < 58 && *sym > 47){
				if (Macro_Found->args[*sym - 48] != NULL) mystrcat(&buf, Macro_Found->args[*sym - 48]);
				else mystrcat(&buf, "0");
			    }
			    else if (*sym < 123 && *sym > 96){
				if (Macro_Found->var[*sym - 97] != NULL) mystrcat(&buf, Macro_Found->var[*sym - 97]);
				else mystrcat(&buf, "0");
			    }
			    else if (*sym == 64) {
				for (i = 1; i < 10; i++){
				    if (Macro_Found->args[i] != NULL) {
					mystrcat(&buf, Macro_Found->args[i]);
					mystrcat(&buf, " ");
				    }
				}
			    }
			    /* There is a '$' sign but an illegal symbol follows */
			    else Print_Error("%s%s%s%s", 4, "\nERROR: Illegal variablename!\n\nMacro: ", Macro_Found->name, "\n\nVariable name: ", sym);
			}
			else mystrcat(&buf, sym);
			/* Next token */
			i++;
		    }
		    /* Write logging */
		    if (logfile != NULL){
			if (cmd > 0)
			    fprintf(logfile, "MACRO '%s': %s\n", Macro_Found->name, macro->text);
			else
			    fprintf(logfile, "MACRO '%s': %s\n", Macro_Found->name, buf);
			fflush(logfile);
		    }
		    /* No RETURN, no ASSOC or GET or COMPARE, no number, no string assignment, no assignment to other var? Execute the buffer */
		    if(cmd != 15 && cmd != 14 && cmd != 5 && cmd != 4 && cmd != 6 && !is_value(buf) && *buf != 38 && *buf != 36)
		    {
			retstr = Call_Realize(buf, cinv_ctx);
		    }
		    else {
			/* Prefill result of macro */
			if (cmd == 14 && *buf == 36) {
			    if(*(buf+1) > 96 && *(buf+1) < 123){
				retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Macro_Found->var[*(buf+1) - 97], gtkserver.post);
			    }
			    else if (*(buf+1) > 47 && *(buf+1) < 58){
				retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, Macro_Found->args[*(buf+1) - 48], gtkserver.post);
			    }
			}
			else if(*buf != 38 && *buf != 36 && !is_value(buf) && cmd != 5 && cmd != 4 && cmd != 6){
			    strtok(buf, " ");
			    retstr = Print_Result("%s%s%s%s", 4, gtkserver.pre, gtkserver.handle, strtok(NULL, ""), gtkserver.post);
			}
			/* Prefill result of string assignment */
			else {
			    retstr = Print_Result("%s%sok%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
			}
		    }
		    /* Log answer of GTK-server */
		    if (logfile != NULL){
			fprintf(logfile, "SERVER: %s\n", retstr);
			fflush(logfile);
		    }
		    /* If assignment, or EMPTY/VALUE/JUMP/COMPARE, get var */
		    if (cmd < 14){
			sym = parse_data(command, 0);
			if (*sym != 36 && cmd != 3) Print_Error("%s%s%s%s", 4, "\nERROR: left-operand must be a legal variablename!\n\nMacro: ", Macro_Found->name, "\n\nVariable name: ", sym);
			sym++;
			/* Only if assignment, save value into variable */
			if (cmd == 0) {
			    if(*sym > 96 && *sym < 123){
				/* Assignment is a string */
				if(*buf == 38){
				    Macro_Found->var[*sym - 97] = (char*)realloc(Macro_Found->var[*sym - 97], strlen(buf)+1);
				    strcpy(Macro_Found->var[*sym - 97], buf+1);
				}
				/* Assignment is a variable or a macro argument */
				else if(*buf == 36){
				    if(*(buf+1) > 96 && *(buf+1) < 123){
					Macro_Found->var[*sym - 97] = (char*)realloc(Macro_Found->var[*sym - 97], strlen(Macro_Found->var[*(buf+1) - 97])+1);
					strcpy(Macro_Found->var[*sym - 97], Macro_Found->var[*(buf+1) - 97]);
				    }
				    else if(*(buf+1) > 47 && *(buf+1) < 58){
					Macro_Found->var[*sym - 97] = (char*)realloc(Macro_Found->var[*sym - 97], strlen(Macro_Found->args[*(buf+1) - 48])+1);
					strcpy(Macro_Found->var[*sym - 97], Macro_Found->args[*(buf+1) - 48]);
				    }
				    else Print_Error("%s%s%s%s", 4, "\nERROR: Illegal assignment!\n\nMacro: ", Macro_Found->name, "\n\nTrying to assign: ", buf);
				}
				/* It's not a string, keep returnstring - '-1' if it was not found by GTK-server */
				else{
				    Macro_Found->var[*sym - 97] = (char*)realloc(Macro_Found->var[*sym - 97], strlen(Trim_String(retstr))+1);
				    strcpy(Macro_Found->var[*sym - 97], Trim_String(retstr));
				}
			    }
			    else Print_Error("%s%s%s%s", 4, "\nERROR: Illegal variablename in assignment!\n\nMacro: ", Macro_Found->name, "\n\nVariablename: ", sym);
			}
			/* We have a COMPARE */
			else if (cmd == 6){
			    if(*sym > 96 && *sym < 123){
				/* Right side is variable? */
				if(*buf == 36){
				    if(*(buf+1) > 96 && *(buf+1) < 123){
					Macro_Found->var[25] = (char*)realloc(Macro_Found->var[25], sizeof(char)+1);
					if(!strcmp(Macro_Found->var[*sym - 97], Macro_Found->var[*(buf+1) - 97])) strcpy(Macro_Found->var[25], "0");
					else strcpy(Macro_Found->var[25], "1");
				    }
				    else if(*(buf+1) > 47 && *(buf+1) < 58){
					Macro_Found->var[25] = (char*)realloc(Macro_Found->var[25], sizeof(char)+1);
					if(!strcmp(Macro_Found->var[*sym - 97], Macro_Found->args[*(buf+1) - 48])) strcpy(Macro_Found->var[25], "0");
					else strcpy(Macro_Found->var[25], "1");
				    }
				}
				/* No variable, plain compare */
				else{
				    Macro_Found->var[25] = (char*)realloc(Macro_Found->var[25], sizeof(char)+1);
				    if(!strcmp(Macro_Found->var[*sym - 97], buf)) strcpy(Macro_Found->var[25], "0");
				    else strcpy(Macro_Found->var[25], "1");
				}
			    }
			    else if(*sym > 47 && *sym < 58){
				/* Right side is variable? */
				if(*buf == 36){
				    if(*(buf+1) > 96 && *(buf+1) < 123){
					Macro_Found->var[25] = (char*)realloc(Macro_Found->var[25], sizeof(char)+1);
					if(!strcmp(Macro_Found->args[*sym - 48], Macro_Found->var[*(buf+1) - 97])) strcpy(Macro_Found->var[25], "0");
					else strcpy(Macro_Found->var[25], "1");
				    }
				    else if(*(buf+1) > 47 && *(buf+1) < 58){
					Macro_Found->var[25] = (char*)realloc(Macro_Found->var[25], sizeof(char)+1);
					if(!strcmp(Macro_Found->args[*sym - 48], Macro_Found->args[*(buf+1) - 48])) strcpy(Macro_Found->var[25], "0");
					else strcpy(Macro_Found->var[25], "1");
				    }
				}
				/* No variable, plain compare */
				else{
				    Macro_Found->var[25] = (char*)realloc(Macro_Found->var[25], sizeof(char)+1);
				    if(!strcmp(Macro_Found->args[*sym - 48], buf)) strcpy(Macro_Found->var[25], "0");
				    else strcpy(Macro_Found->var[25], "1");
				}
			    }
			    else Print_Error("%s%s%s%s", 4, "\nERROR: Illegal variablename in COMPARE!\n\nMacro: ", Macro_Found->name, "\n\nVariablename: ", sym);
			}
			/* Do we have an ASSOC? */
			else if (cmd == 4){
			    if(*sym > 96 && *sym < 123){
				if(*buf == 36 && *(buf+1) > 96 && *(buf+1) < 123){
				    M_Assocs = (MACRO_ASSOC*)malloc(sizeof(MACRO_ASSOC));
				    /* Save the assoc'ed value */
                                    memset(M_Assocs->assoc, 0, LONG_SIZE);
				    strncpy(M_Assocs->assoc, Macro_Found->var[*(buf+1) - 97], LONG_SIZE-1);
				    /* Determine key */
				    M_Assocs->key = strdup(Macro_Found->var[*sym - 97]);
				    /* Add to hash table */
				    HASH_ADD_KEYPTR(hh, macro_assoc_protos, M_Assocs->key, strlen(M_Assocs->key), M_Assocs);
				}
				else Print_Error("%s%s%s%s", 4, "\nERROR: Illegal assocID!\n\nMacro: ", Macro_Found->name, "\n\nAssocID: ", buf);
			    }
			    else if (*sym > 47 && *sym < 58){
				if(*buf == 36 && *(buf+1) > 96 && *(buf+1) < 123){
				    M_Assocs = (MACRO_ASSOC*)malloc(sizeof(MACRO_ASSOC));
				    /* Save the assoc'ed value */
                                    memset(M_Assocs->assoc, 0, LONG_SIZE);
				    strncpy(M_Assocs->assoc, Macro_Found->var[*(buf+1) - 97], LONG_SIZE-1);
				    /* Determine key */
				    M_Assocs->key = strdup(Macro_Found->args[*sym - 48]);
				    /* Add to hash table */
				    HASH_ADD_KEYPTR(hh, macro_assoc_protos, M_Assocs->key, strlen(M_Assocs->key), M_Assocs);
				}
				else Print_Error("%s%s%s%s", 4, "\nERROR: Illegal assocID!\n\nMacro: ", Macro_Found->name, "\n\nAssocID: ", buf);
			    }
			    else Print_Error("%s%s%s%s", 4, "\nERROR: Illegal assocname!\n\nMacro: ", Macro_Found->name, "\n\nAssocname: ", sym);
			}
			/* Do we have a GET? */
			else if (cmd == 5){
			    if(*sym > 96 && *sym < 123){
				if(*buf == 36 && *(buf+1) > 47 && *(buf+1) < 58){
				    /* Create hash from assocreference */
				    HASH_FIND_STR(macro_assoc_protos, Macro_Found->args[*(buf+1) - 48], M_Assocs);
				    /* Assign found assoc value to variable */
				    Macro_Found->var[*sym - 97] = (char*)realloc(Macro_Found->var[*sym - 97], LONG_SIZE);
				    if (M_Assocs != NULL) strncpy(Macro_Found->var[*sym - 97], M_Assocs->assoc, LONG_SIZE);
				    else strncpy(Macro_Found->var[*sym - 97], "0", LONG_SIZE);
				}
				else if(*buf == 36 && *(buf+1) > 96 && *(buf+1) < 123){
				    /* Create hash from assocreference */
				    HASH_FIND_STR(macro_assoc_protos, Macro_Found->var[*(buf+1) - 97], M_Assocs);
				    /* Assign found assoc value to variable */
				    Macro_Found->var[*sym - 97] = (char*)realloc(Macro_Found->var[*sym - 97], LONG_SIZE);
				    if (M_Assocs != NULL) strncpy(Macro_Found->var[*sym - 97], M_Assocs->assoc, LONG_SIZE);
				    else strncpy(Macro_Found->var[*sym - 97], "0", LONG_SIZE);
				}
				else Print_Error("%s%s%s%s", 4, "\nERROR: Illegal reference to assoc!\n\nMacro: ", Macro_Found->name, "\n\nAssocreference: ", buf);
			    }
			    else Print_Error("%s%s%s%s", 4, "\nERROR: Illegal variablename!\n\nMacro: ", Macro_Found->name, "\n\nVariablename: ", sym);
			}
		    }
		    /* Prepare for EMPTY/VALUE, get value of variable - initialize 'sym' because this is not always set (e.g. with RETURN it is not set) */
		    sym = parse_data(command, 0);
		    if(*sym == 36 && cmd != 5 && cmd != 4 && cmd != 14 && cmd != 6) {
			sym++;
			/* Do we have an argument to the macro? */
			if (is_value(sym)) {
			    if (Macro_Found->args[*sym - 48] == NULL) i=0;
			    else {
				if(!strcmp(Macro_Found->args[*sym - 48], "0")) i=0;
				else i=1;
			    }
			}
			/* No, so do we have variablename? */
			else if (*sym > 96)
			{
			    if (!Macro_Found->var[*sym - 97]) i=0;
			    else {
				if(!strcmp(Macro_Found->var[*sym - 97], "0") || !strcmp(Macro_Found->var[*sym - 97], "")) i=0;
				else i=1;
			    }
			}
		    }
		    /* Goto next line with EMPTY / VALUE or without */
		    if(cmd == 1 && i == 0){
			if(is_value(buf)) i = atoi(buf);
			else Print_Error("%s%s", 2, "\nERROR: Illegal jump in EMPTY!\n\nMacro: ", Macro_Found->name);
			while (i != 0 && macro != NULL) {
			    if (i > 0) {
				macro = macro->next;
				i--;
			    }
			    else {
				macro = macro->prev;
				i++;
			    }
			}
		    }
		    /* The VALUE command */
		    else if(cmd ==2 && i != 0){
			if(is_value(buf)) i = atoi(buf);
			else Print_Error("%s%s", 2, "\nERROR: Illegal jump in VALUE!\n\nMacro: ", Macro_Found->name);
			while (i != 0 && macro != NULL) {
			    if (i > 0) {
				macro = macro->next;
				i--;
			    }
			    else {
				macro = macro->prev;
				i++;
			    }
			}
		    }
		    /* The JUMP command */
		    else if(cmd == 3){
			if(is_value(buf)) i = atoi(buf);
			else Print_Error("%s%s", 2, "\nERROR: Illegal jump in JUMP!\n\nMacro: ", Macro_Found->name);
			while (i != 0 && macro != NULL) {
			    if (i > 0) {
				macro = macro->next;
				i--;
			    }
			    else {
				macro = macro->prev;
				i++;
			    }
			}
		    }
		    else macro = macro->next;
		    parse_free(command);
		}
		free(buf);
		/***************************************************
		    End of the Macro Object Mini Parser
		***************************************************/
	    }
	    /* API call not found */
	    else retstr = Print_Result("%s%s-1%s", 3, gtkserver.pre, gtkserver.handle, gtkserver.post);
	}
    }
    /* Free memory of members and list */
    parse_free(list);
}
return retstr;
}

/*************************************************************************************************
************************************************************* This is the main loop of the server
*************************************************************************************************/
#ifdef GTK_SERVER_LIBRARY
int init(char *user_data)
{

#if GTK_SERVER_XF
int argc = 1;
char *argv[1];
#endif

#if GTK_SERVER_MOTIF
int argc = 0;
#endif

#else
int main(int argc, char *argv[])
{
#endif
/* Define the configfile */
FILE *configfile;
FILE *include = NULL;
/* Define the configfile name buffer */
char *user_cfgfile = NULL;
/* Define the line input, declare memory */
char line[MAX_LEN];
/* Count libs */
int lib_counter = 1;
/* Temp holder for tokenization of configfile */
char *cache = NULL;
/* Define environment variable holder */
char *libvar;
/* Count current line number of configfile */
long count_line;
/* Define temp variable holders for configfile entries */
char *api_name;
char *enum_name;
char *str_name;
char *alias_name;
/* Define temp variables for loops */
int i, j;
#ifdef GTK_SERVER_WIN32
/* Needed for finding the configfile */
char filename[MAX_PATH];
int len;
/* Needed for static Win32 compilation */
char myname[MAX_LEN];
#else
char *filename;
#endif

/* Initialize GUI toolkits */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
gtk_init(NULL, NULL);
#elif GTK_SERVER_XF
argv[0] = strdup("");
fl_initialize(&argc, argv, "XForms", 0, 0);
#elif GTK_SERVER_MOTIF
CLASS *Class_Defs;
int argcc = 0;
XtSetLanguageProc(NULL, NULL, NULL);
gtkserver.toplevel = XtVaAppInitialize(&gtkserver.app, "Motif", NULL, 0, &argcc, NULL, NULL, NULL);
#endif

/* Needed for debug window */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
GtkWidget *debug_window, *debug_scrolled, *debug_execute, *debug_close, *debug_next, *debug_pause, *debug_vbox, *debug_hbox;
#elif GTK_SERVER_XF
FL_FORM *debug_window;
FL_OBJECT *debug_close, *debug_execute, *debug_pause, *debug_next;
#elif GTK_SERVER_MOTIF
Widget debug_window, debug_layer, debug_close, debug_next, debug_pause, debug_execute;
XmFontListEntry debug_entry;
XmFontList debug_font;
#endif

/* Define the debug window in different toolkits */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
debug_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_title(GTK_WINDOW(debug_window), "GTK-server Debugger");
gtk_window_set_default_size(GTK_WINDOW(debug_window), 600, 300);
#ifdef GTK_SERVER_GTK1x
debug_view = gtk_text_new(NULL, NULL);
#elif GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
gtk_window_set_icon_name(GTK_WINDOW(debug_window), "gtk-preferences");
debug_buffer = gtk_text_buffer_new(0);
debug_view = gtk_text_view_new_with_buffer(debug_buffer);
gtk_text_view_set_editable(GTK_TEXT_VIEW(debug_view), 0);
gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(debug_view), GTK_WRAP_NONE);
gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(debug_view), 0);
#endif
debug_scrolled = gtk_scrolled_window_new(0, 0);
gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(debug_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
gtk_container_add(GTK_CONTAINER(debug_scrolled), debug_view);
gtk_container_set_border_width(GTK_CONTAINER(debug_scrolled), 5);
#ifdef GTK_SERVER_GTK1x
debug_execute = gtk_button_new_with_label("Run");
gtk_widget_set_usize(debug_execute, 60, 30);
debug_close = gtk_button_new_with_label("Quit");
gtk_widget_set_usize(debug_close, 60, 30);
debug_next = gtk_button_new_with_label("Step");
gtk_widget_set_usize(debug_next, 60, 30);
debug_pause = gtk_button_new_with_label("Pause");
gtk_widget_set_usize(debug_pause, 60, 30);
debug_vbox = gtk_vbox_new(0, 0);
debug_hbox = gtk_hbox_new(0, 0);
#elif GTK_SERVER_GTK2x
debug_execute = gtk_button_new_from_stock("gtk-execute");
debug_close = gtk_button_new_from_stock("gtk-quit");
debug_next = gtk_button_new_from_stock("gtk-go-forward");
debug_pause = gtk_button_new_from_stock("gtk-media-pause");
debug_vbox = gtk_vbox_new(0, 0);
debug_hbox = gtk_hbox_new(0, 0);
#elif GTK_SERVER_GTK3x
debug_execute = gtk_button_new_from_icon_name("go-jump", GTK_ICON_SIZE_DND);
gtk_button_set_label(GTK_BUTTON(debug_execute), "Run");
debug_close = gtk_button_new_from_icon_name("process-stop", GTK_ICON_SIZE_DND);
gtk_button_set_label(GTK_BUTTON(debug_close), "Quit");
debug_next = gtk_button_new_from_icon_name("go-next", GTK_ICON_SIZE_DND);
gtk_button_set_label(GTK_BUTTON(debug_next), "Step");
debug_pause = gtk_button_new_from_icon_name("media-playback-pause", GTK_ICON_SIZE_DND);
gtk_button_set_label(GTK_BUTTON(debug_pause), "Pause");
debug_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
debug_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#endif
gtk_box_pack_start(GTK_BOX(debug_hbox), debug_close, 0, 0, 1);
gtk_box_pack_end(GTK_BOX(debug_hbox), debug_pause, 0, 0, 1);
gtk_box_pack_end(GTK_BOX(debug_hbox), debug_execute, 0, 0, 1);
gtk_box_pack_end(GTK_BOX(debug_hbox), debug_next, 0, 0, 1);
gtk_box_pack_start(GTK_BOX(debug_vbox), debug_scrolled, 1, 1, 1);
gtk_box_pack_start(GTK_BOX(debug_vbox), debug_hbox, 0, 0, 1);
gtk_container_add(GTK_CONTAINER(debug_window), debug_vbox);
SIGNALCONNECT(debug_pause, "clicked", switch_flag_off, &debug_run);
SIGNALCONNECT(debug_execute, "clicked", switch_flag_on, &debug_run);
SIGNALCONNECT(debug_next, "clicked", switch_flag_on, &debug_step);
SIGNALCONNECT(debug_close, "clicked", exit, NULL);
SIGNALCONNECT(debug_window, "delete-event", gtk_widget_hide_on_delete, NULL);

#define update_gui while(gtk_events_pending()) { gtk_main_iteration_do(0); }

#ifdef GTK_SERVER_GTK1x
#define scroll_to_end(x, y, z) do { \
    if(z&2) gtk_text_insert(GTK_TEXT(debug_view), NULL, NULL, NULL, "SCRIPT: ", -1); \
    else gtk_text_insert(GTK_TEXT(debug_view), NULL, NULL, NULL, "SERVER: ", -1); \
    gtk_text_insert(GTK_TEXT(debug_view), NULL, NULL, NULL, y, -1); \
    if(z&1) gtk_text_insert(GTK_TEXT(debug_view), NULL, NULL, NULL, "\n", -1); \
    update_gui; \
} while(0)
#else
#define scroll_to_end(x, y, z) do { \
    if(z&2) gtk_text_buffer_insert_at_cursor(debug_buffer, "SCRIPT: ", -1); \
    else gtk_text_buffer_insert_at_cursor(debug_buffer, "SERVER: ", -1); \
    gtk_text_buffer_insert_at_cursor(debug_buffer, y, -1); \
    if(z&1) gtk_text_buffer_insert_at_cursor(debug_buffer, "\n", -1); \
    update_gui; \
    gtk_text_buffer_get_end_iter(debug_buffer, &debug_iter); \
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(x), &debug_iter, 0, 1, 0, 1); \
    update_gui; \
} while(0)
#endif

#elif GTK_SERVER_XF
debug_window = fl_bgn_form(FL_BORDER_BOX, 600, 300);
debug_view = fl_add_input(FL_MULTILINE_INPUT, 10, 10, 580, 230, NULL);
fl_set_input_cursor_visible(debug_view, 0);
fl_set_object_return(debug_view, FL_RETURN_NONE);
debug_close = fl_add_button(FL_NORMAL_BUTTON, 10, 250, 80, 40, "@#9+");
fl_set_object_lcol(debug_close, FL_RED);
fl_set_object_callback(debug_close, (FL_CALLBACKPTR)exit, 0);
debug_execute = fl_add_button(FL_NORMAL_BUTTON, 420, 250, 80, 40, "@#>>");
fl_set_object_callback(debug_execute, (FL_CALLBACKPTR)switch_flag_on, (long)&debug_run);
debug_pause = fl_add_button(FL_NORMAL_BUTTON, 510, 250, 80, 40, "@#square");
fl_set_object_callback(debug_pause, (FL_CALLBACKPTR)switch_flag_off, (long)&debug_run);
debug_next = fl_add_button(FL_NORMAL_BUTTON, 330, 250, 80, 40, "@#>");
fl_set_object_callback(debug_next, (FL_CALLBACKPTR)switch_flag_on, (long)&debug_step);
fl_end_form();

#define update_gui fl_check_forms();

#define scroll_to_end(x, y, z) do { \
    xf_buffer = strdup(fl_get_input(x)); \
    xf_buffer = realloc(xf_buffer, strlen(xf_buffer)+strlen(y)+8+2); \
    if(z&2) strcat(xf_buffer, "SCRIPT: "); \
    else strcat(xf_buffer, "SERVER: "); \
    strcat(xf_buffer, y); \
    if(z&1) { strcat(xf_buffer, "\n"); } \
    fl_set_input(x, xf_buffer); \
    free(xf_buffer); \
    while(fl_check_forms()); \
} while(0)

#elif GTK_SERVER_MOTIF
debug_window = XtVaAppCreateShell (NULL, "Class", topLevelShellWidgetClass, XtDisplay(gtkserver.toplevel), XtNtitle, "GTK-server Debugger", NULL);
debug_layer = XtVaCreateManagedWidget("layer", xmFormWidgetClass, debug_window, NULL);
XtVaSetValues(debug_layer, XmNwidth, 600, XmNheight, 300, NULL);
debug_entry = XmFontListEntryLoad(XtDisplay(gtkserver.toplevel), "7x13bold" , XmFONT_IS_FONT, "tag");
debug_font = XmFontListAppendEntry(NULL, debug_entry);
XmFontListEntryFree(&debug_entry);
debug_view = XmCreateScrolledText(debug_layer, "text", NULL, 0);
XtVaSetValues(XtParent(debug_view), XmNwidth, 580, XmNheight, 220, XmNx, 10, XmNy, 10, XmNleftAttachment, XmATTACH_SELF, XmNrightAttachment, XmATTACH_SELF, XmNbottomAttachment, XmATTACH_SELF, XmNtopAttachment, XmATTACH_SELF, NULL);
XtVaSetValues(debug_view, XmNfontList, debug_font, XmNeditMode, XmMULTI_LINE_EDIT, XmNeditable, False, XmNcursorPositionVisible, False, NULL);
XtManageChild(debug_view);
debug_close = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass, debug_layer, XmNfontList, debug_font, XmNwidth, 80, XmNheight, 40, XmNx, 10, XmNy, 250, XmNleftAttachment, XmATTACH_SELF, XmNbottomAttachment, XmATTACH_SELF, XtVaTypedArg, XmNlabelString, XmRString, "Quit", 4, NULL);
XtAddCallback((Widget)debug_close, XmNactivateCallback, (XtCallbackProc)exit, NULL);
debug_next = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass, debug_layer, XmNfontList, debug_font, XmNwidth, 80, XmNheight, 40, XmNx, 330, XmNy, 250, XmNrightAttachment, XmATTACH_SELF, XmNbottomAttachment, XmATTACH_SELF, XtVaTypedArg, XmNlabelString, XmRString, "Step", 4, NULL);
XtAddCallback((Widget)debug_next, XmNactivateCallback, (XtCallbackProc)switch_flag_on, &debug_step);
debug_execute = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass, debug_layer, XmNfontList, debug_font, XmNwidth, 80, XmNheight, 40, XmNx, 420, XmNy, 250, XmNrightAttachment, XmATTACH_SELF, XmNbottomAttachment, XmATTACH_SELF, XtVaTypedArg, XmNlabelString, XmRString, "Run", 4, NULL);
XtAddCallback((Widget)debug_execute, XmNactivateCallback, (XtCallbackProc)switch_flag_on, &debug_run);
debug_pause = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass, debug_layer, XmNfontList, debug_font, XmNwidth, 80, XmNheight, 40, XmNx, 510, XmNy, 250, XmNrightAttachment, XmATTACH_SELF, XmNbottomAttachment, XmATTACH_SELF, XtVaTypedArg, XmNlabelString, XmRString, "Pause", 4, NULL);
XtAddCallback((Widget)debug_pause, XmNactivateCallback, (XtCallbackProc)switch_flag_off, &debug_run);
if (gtkserver.behave & 512) { XtRealizeWidget(debug_window); }

#define update_gui while(XtAppPending(gtkserver.app)) { XtAppProcessEvent(gtkserver.app, XtIMAll); }

#define scroll_to_end(x, y, z) do { \
    motif_buf = XmTextGetString(x); \
    motif_buf = realloc(motif_buf, strlen(motif_buf)+strlen(y)+8+2); \
    if(z&2) strcat(motif_buf, "SCRIPT: "); \
    else strcat(motif_buf, "SERVER: "); \
    strcat(motif_buf, y); \
    if(z&1) { strcat(motif_buf, "\n"); } \
    XmTextSetString(x, motif_buf); \
    XmTextShowPosition(debug_view, strlen(motif_buf)); \
    XtFree(motif_buf); \
    update_gui; \
} while(0)
#endif

#ifndef GTK_SERVER_LIBRARY
/* Define the returnstring */
char *retstr = NULL;
/* Variable for unlimited input */
char *in = NULL;
/* Define list for CONFIGURATION settings */
CONFIG *Gtk_Api_Config;
/* Define list for macro definitions */
MACRO *Macro_Defs;
BODY *Body_Text;
BODY *Body_Last = NULL;
/* Define list for enum definitions */
ENUM *Enum_Defs;
STR *Str_Defs;
/* Define list for alias definitions */
ALIAS *Alias_Defs;
/* Define vars for TCP sockets */
char *host;
char *port;
char *maxtcp;
int sockfd;				/* Used for FIFO and TCP */
struct hostent *he;
struct sockaddr_in my_addr;             /* my address information */
struct sockaddr_in their_addr;		/* connector's address information (UDP) */
int new_fd;
int yes = 1;
int numbytes = 0, page;
char buf[MAX_LEN];			/* Buffer containing data from socket */

#ifdef GTK_SERVER_USE_SSL
SSL_CTX *ctx;
SSL *ssl = NULL;
BIO *sbio;
#endif

#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
fd_set rfds;				/* Needed for file descriptor polling in case debug window is used */
struct timeval tv;
#endif

#ifdef GTK_SERVER_UNIX
socklen_t sin_size, addr_len;
#ifdef GTK_SERVER_SOCK_LONG
unsigned long len;
#else
unsigned int len;
#endif
/* SIGNAL struct for child processes */
int curpid;				/* Needed for forking */
int msgid;				/* Needed for message queues */
struct my_msg_buf {
    long mtype;
    char mtext[MAX_LEN];
};
struct my_msg_buf msgp;
struct msqid_ds *msgstat = (struct msqid_ds*)malloc(sizeof(struct msqid_ds));
/* Holder for IPC messages */
char *msgtxt;
char *ipc;
gtkserver.exit_sig = 0;
gtkserver.ppid = getppid();

#elif GTK_SERVER_WIN32
int sin_size, addr_len;
/* Needed for OS detection */
OSVERSIONINFOEX osvi;
BOOL bOsVersionInfoEx;
/* Needed for PIPES */
HANDLE hPipe1;
HANDLE hPipe2;
CHAR chRequest[MAX_LEN];
DWORD cbBytesRead, cbWritten;
BOOL fSuccess;
char *dialog_msg = (char*)malloc(MAX_LEN*sizeof(char));
#endif

/* Binary flags to behave to '0' */
gtkserver.behave = 0;

/* Initialize the logfile to NULL */
gtkserver.LogDir = NULL;

/* Initialize macro no NULL */
gtkserver.macro = NULL;

/* Sequence of libraries defaults to 100 */
gtkserver.libseq = 100;

/* Initialize SSL stuff to NULL */
gtkserver.certificate = NULL;
gtkserver.ca = NULL;
gtkserver.password = NULL;

/* Run help if there are no arguments */
if (argc < 2 || !strncmp(argv[1], "help", 4) || !strncmp(argv[1], "-help", 5)) {
    #ifdef GTK_SERVER_WIN32
    strcpy(dialog_msg, "*** This is the GTK-server ");
    strcat(dialog_msg, GTK_SERVER_VERSION);
    strcat(dialog_msg, " compiled for ");
    #ifdef GTK_SERVER_GTK1x
    strcat(dialog_msg, "GTK 1.x ***");
    #elif GTK_SERVER_GTK2x
    strcat(dialog_msg, "GTK 2.x ***");
    #elif GTK_SERVER_GTK3x
    strcat(dialog_msg, "GTK 3.x ***");
	#else
    strcat(dialog_msg, "console");
    #endif
    strcat(dialog_msg, "\n\r\n\rUsage:\tgtk-server\t\t\t\tthis help \
	\n\r\tgtk-server -stdin [-log=<file>]\t\tstart server in STDIN mode \
	\n\r\tgtk-server -fifo [-log=<file>]\t\t\tstart server in FIFO mode \
	\n\r\tgtk-server -tcp=<host:port> [-log=<file>]\tstart server in TCP server mode \
	\n\r\tgtk-server -sock=<host:port> [-log=<file>]\tstart server in TCP client mode \
	\n\r\tgtk-server -udp=<host:port> [-log=<file>]\tstart server in UDP mode \
	\n\r\tgtk-server -showconf\t\t\tShow configuration from the configfile");
    MessageBox(NULL, dialog_msg, "GTK-server usage", MB_OK | MB_ICONINFORMATION);
    exit(EXIT_SUCCESS);
    #endif
    #ifdef GTK_SERVER_UNIX
    printf("\n*** This is the GTK-SERVER ");
    printf("%s compiled for ", GTK_SERVER_VERSION);
    #ifdef GTK_SERVER_GTK1x
    printf("GTK 1.x ");
    #elif GTK_SERVER_GTK2x
    printf("GTK 2.x ");
    #elif GTK_SERVER_GTK3x
    printf("GTK 3.x ");
    #elif GTK_SERVER_XF
    printf("XForms ");
    #elif GTK_SERVER_MOTIF
    printf("Motif ");
    #else
    printf("console ");
    #endif
    printf("***\n\nUsage:");
    printf("\tgtk-server\t\t\t\t\t\t\tthis help\n");
    printf("\tgtk-server -stdin [-log=<file>]\t\t\t\t\tstart server in STDIN mode\n");
    printf("\tgtk-server -fifo=<file> [-nocreate] [-detach] [-log=<file>]\tstart server in FIFO mode\n");
    printf("\tgtk-server -tcp=<host:port[:max]> [-detach] [-log=<file>]\tstart server in TCP server mode\n");
    printf("\tgtk-server -sock=<host:port> [-detach] [-log=<file>]\t\tstart server in TCP client mode\n");
    printf("\tgtk-server -udp=<host:port> [-detach] [-log=<file>]\t\tstart server in UDP mode\n");
    printf("\tgtk-server -ipc=<handle> [-detach] [-log=<file>]\t\tstart server in IPC mode\n");
    printf("\tgtk-server -showconf\t\t\t\t\t\tshow configuration from the configfile\n\n");
    exit (EXIT_SUCCESS);
    #endif
}

/* See if we are running from a shebang */
if(!strncmp(argv[1], "this", 4) || !strncmp(argv[1], "-this", 5)){
    user_cfgfile = argv[2];
    gtkserver.behave |= 16;
    gtkserver.macro = strdup("main");
}
else {
    /* Now walk through arguments */
    for (i = 1; i<argc; i++){
	if (!strcmp(argv[i], "showconf") || !strcmp(argv[i], "-showconf")) gtkserver.behave |= 1;	/* Check if configuration should be printed to STDOUT */
	else if (!strcmp(argv[i], "nocreate") || !strcmp(argv[i], "-nocreate")) gtkserver.behave |= 2;	/* Check if FIFO should be created */
	else if (!strcmp(argv[i], "detach") || !strcmp(argv[i], "-detach")) gtkserver.behave |= 4;	/* Check if GTK-server should try to spawn itself */
	else if (!strncmp(argv[i], "cfg=", 4) || !strncmp(argv[i], "-cfg=", 5)){
	    strtok(argv[i], "=");						/* Check if configfile is entered */
	    user_cfgfile = strtok(NULL, "=");
	}
	else if (!strncmp(argv[i], "log=", 4) || !strncmp(argv[i], "-log=", 5)){
	    strtok(argv[i], "=");						/* Check if logfile is entered */
	    gtkserver.LogDir = strtok(NULL, "=");
	}
	else if (!strncmp(argv[i], "stdin", 5) || !strncmp(argv[i], "-stdin", 6)) gtkserver.mode = 1;
	#ifdef GTK_SERVER_UNIX
	else if (!strncmp(argv[i], "fifo=", 5) || !strncmp(argv[i], "-fifo=", 6)){
	    gtkserver.mode = 2;
	    strtok(argv[i], "=");						/* Check and store FIFO file */
	    gtkserver.fifo = strtok(NULL, "=");
	}
	else if (!strncmp(argv[i], "signal=", 7) || !strncmp(argv[i], "-signal=", 8)){
	    gtkserver.behave |= 8;
	    strtok(argv[i], "=");						/* Check and store signal */
	    cache = strtok(NULL, "=");
	    if ((cache != NULL) && is_value(cache)) gtkserver.exit_sig = atoi(cache);
	}
	else if (!strncmp(argv[i], "ipc=", 4) || !strncmp(argv[i], "-ipc=", 5)){
	    gtkserver.mode = 5;
	    strtok(argv[i], "=");						/* Check and store IPC value */
	    cache = strtok(NULL, "=");
	    if (cache != NULL) gtkserver.ipc = atoi(cache);
	    else Print_Error("%s", 1, "\nERROR: Communication channel must lay in the range 1-65535!");
	    if (gtkserver.ipc < 1 || gtkserver.ipc > 65535) Print_Error("%s", 1, "\nERROR: Communication channel must lay in the range 1-65535!");
	}
	else if (!strncmp(argv[i], "msg=", 4) || !strncmp(argv[i], "-msg=", 5)){

	    msgtxt = strstr(argv[i], "=");

	    /*Check argument format */
	    if ((ipc = strstr(++msgtxt, ",")) == NULL) Print_Error("%s", 1, "\nERROR: Message argument is in wrong format!");
	    ipc++;

	    if (strstr(msgtxt, ",") == NULL) Print_Error("%s", 1, "\nERROR: Message argument is in wrong format!");

	    gtkserver.ipc = atoi(strtok(msgtxt, ","));

	    /* Simple retry mechanism to find the message queue, needed for first run of the 'msg'-argument */
	    for(i = 0; i < 100; i++) {
		msgid = msgget(gtkserver.ipc, 0666);
		if (msgid >= 0) break;
		usleep(10);
	    }

	    /* No queue after timeout of 1 second? Exit GTK-server */
	    if(msgid < 0) Print_Error("%s%s", 2, "\nCould not find message queue: ", strerror(errno));

	    /* We agree with ourselves to use msgtype = 1 */
	    msgp.mtype = 1;
	    do {
		strncpy(msgp.mtext, ipc, MAX_LEN-1);
		if (msgsnd(msgid, &msgp, strlen(msgp.mtext), 0) < 0) Print_Error("%s%s", 2, "\nCould not send to message queue: ", strerror(errno));
		if (strlen(ipc) > MAX_LEN) ipc += MAX_LEN;
		else ipc = NULL;
	    }
	    while (ipc != NULL);

	    /* SYNC: wait for other PID to write data */
	    do {
		usleep(5);		    /* Do not overload kernel */
		if(msgctl(msgid, IPC_STAT, msgstat) < 0) {
		    if (errno == EINVAL) exit(EXIT_FAILURE);	/* Client program has exited */
		    else Print_Error("%s%s", 2, "\nThe IPC channel has exited: ", strerror(errno));
		};
	    } while(getpid() == msgstat->msg_lspid);

	    /* Now read incoming data */
	    page = 0;
	    len = 0;
	    do {
		numbytes = msgrcv(msgid, &msgp, MAX_LEN, 1, MSG_NOERROR);

		if(numbytes < 0) Print_Error("%s%s", 2, "\nError in reading from IPC(msg): ", strerror(errno));

		/* Realloc preserves contents, add one for \0 which is added later */
		if (len + numbytes > MAX_LEN*page) {page++; in = realloc(in, MAX_LEN*page + 1);}

		if (in == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to read from IPC(msg): ", strerror(errno));

		memcpy(in + len, msgp.mtext, numbytes);
		len += numbytes;
	    }
	    while (numbytes == MAX_LEN);

	    /* Make sure we have a NULL terminated character array */
	    if (len >= 0) in[len] = '\0';

	    /* Print result to STDOUT */
	    if(gtkserver.behave & 1024)
		{ fprintf(stdout, "%s", in); }
	    else
		{ fprintf(stdout, "%s\n", in); }
	    fflush(stdout);
	    exit(EXIT_SUCCESS);
	}
	#elif GTK_SERVER_WIN32
	else if (!strncmp(argv[i], "fifo", 4) || !strncmp(argv[i], "-fifo", 5)){
	    gtkserver.mode = 2;
	}
	#endif
	else if (!strncmp(argv[i], "tcp=", 4) || !strncmp(argv[i], "-tcp=", 5)){
	    gtkserver.mode = 3;
	    strtok(argv[i], "=");				    			/* Check and store TCP data */
	    gtkserver.tcp = strtok(NULL, "=");
	}
	else if (!strncmp(argv[i], "sock=", 5) || !strncmp(argv[i], "-sock=", 6)){
	    gtkserver.mode = 6;
	    strtok(argv[i], "=");				    			/* Check and store TCP data */
	    gtkserver.tcp = strtok(NULL, "=");
	}
	else if (!strncmp(argv[i], "start=", 6) || !strncmp(argv[i], "-start=", 7)){
	    gtkserver.behave |= 16;
	    strtok(argv[i], "=");				    			/* Start with macro */
	    gtkserver.macro = strtok(NULL, "=");
	}
	else if (!strncmp(argv[i], "init=", 5) || !strncmp(argv[i], "-init=", 6)){
	    gtkserver.behave |= 32;
	    strtok(argv[i], "=");				    			/* Init string to send when in -sock mode */
	    gtkserver.init = strtok(NULL, "=");
	}
	#ifdef GTK_SERVER_USE_SSL
	else if (!strncmp(argv[i], "ssl", 3) || !strncmp(argv[i], "-ssl", 4)){		/* -sock could use SSL */
	    gtkserver.behave |= 64;
	    strtok(argv[i], "=");
	    gtkserver.certificate = strtok(NULL, "=");
	}
	else if (!strncmp(argv[i], "ca", 2) || !strncmp(argv[i], "-ca", 3)){		/* -sock could check remote CA */
	    gtkserver.behave |= 128;
	    strtok(argv[i], "=");
	    gtkserver.ca = strtok(NULL, "=");
	}
	else if (!strncmp(argv[i], "password", 8) || !strncmp(argv[i], "-password", 9)){		/* -sock can use SSL password */
	    strtok(argv[i], "=");
	    gtkserver.password = strtok(NULL, "=");
	    if (!(gtkserver.behave & 64)) Print_Error("%s", 1, "\nPassword option should be used with client certificate!");
	}
	#endif
	else if (!strncmp(argv[i], "udp=", 4) || !strncmp(argv[i], "-udp=", 5)){
	    gtkserver.mode = 4;
	    strtok(argv[i], "=");							/* Check and store UDP data */
	    gtkserver.udp = strtok(NULL, "=");
	}
	else if (!strncmp(argv[i], "pre=", 4) || !strncmp(argv[i], "-pre=", 5)){
	    strtok(argv[i], "=");
	    gtkserver.pre = strtok(NULL, "=");					/* Add additional text before returnstring */
	}
	else if (!strncmp(argv[i], "post=", 5) || !strncmp(argv[i], "-post=", 6)){
	    strtok(argv[i], "=");
	    gtkserver.post = strtok(NULL, "=");					/* Add additional text behind returnstring */
	}
	else if (!strncmp(argv[i], "handle", 6) || !strncmp(argv[i], "-handle", 7)){ /* Should first part of incoming string be considered handle from client? */
	    gtkserver.behave |= 256;
	}
	else if (!strncmp(argv[i], "debug", 5) || !strncmp(argv[i], "-debug", 6)){ /* Show debug dialog with logging? */
            #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
            gtk_widget_show_all(debug_window);
            #elif GTK_SERVER_XF
            fl_show_form(debug_window, FL_PLACE_CENTER, FL_FULLBORDER, "GTK-server Debugger");
            #elif GTK_SERVER_MOTIF
            XtRealizeWidget(debug_window);
            #endif
	    gtkserver.behave |= 512;
	}
	else if (!strncmp(argv[i], "nonl", 4) || !strncmp(argv[i], "-nonl", 5)){ /* Do not add a newline after each string coming back from GTK-server */
	    gtkserver.behave |= 1024;
	}
	else {
	    Print_Error("%s%s%s", 3, "\nArgument '", argv[i], "' to GTK-server not recognized!");
	    exit(EXIT_FAILURE);
	}
    }
}

#else	/* ----------------------------------------------- LIBRARY is defined -------------------------------------- */
int init_result = 0;

/* Initialize GUI toolkits */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
gtk_init(NULL, NULL);
#elif GTK_SERVER_XF
argv[0] = strdup("");
fl_initialize(&argc, argv, "XForms", 0, 0);
#elif GTK_SERVER_MOTIF
XtSetLanguageProc(NULL, NULL, NULL);
gtkserver.toplevel = XtVaAppInitialize(&gtkserver.app, "Motif", NULL, 0, &argc, NULL, NULL, NULL);
#endif

if (!strncmp(Trim_String(user_data), "gtk_server_cfg", 14) || !strncmp(Trim_String(user_data), "GTK_SERVER_CFG", 14))
{
    user_data = strtok(user_data, " ");                                                     /* Ignore dummy cfg command */
    user_data = strtok(NULL, " ");
    init_result++;
    do {
        if (!strncmp(Trim_String(user_data), "cfg=", 4) || !strncmp(Trim_String(user_data), "-cfg=", 5)){
	    user_cfgfile = user_data;							       /* Find configfile */
	    user_data = strtok(NULL, " ");
	    init_result++;
        }
        else if (!strncmp(Trim_String(user_data), "pre=", 4) || !strncmp(Trim_String(user_data), "-pre=", 5)){
	    gtkserver.pre = user_data;							        /* Find pre */
	    user_data = strtok(NULL, " ");
	    init_result++;
        }
        else if (!strncmp(Trim_String(user_data), "post=", 5) || !strncmp(Trim_String(user_data), "-post=", 6)){
	    gtkserver.post = user_data;						                /* Find post */
	    user_data = strtok(NULL, " ");
	    init_result++;
        }
        else if (!strncmp(Trim_String(user_data), "start=", 6) || !strncmp(Trim_String(user_data), "-start=", 7)){
	    gtkserver.macro = user_data;						        /* Start with macro */
	    gtkserver.behave |= 16;
	    user_data = strtok(NULL, " ");
	    init_result++;
        }
        else if (!strncmp(Trim_String(user_data), "handle", 6) || !strncmp(Trim_String(user_data), "-handle", 7)){
	    gtkserver.behave |= 256;	                                                        /* Find handle flag */
	    user_data = strtok(NULL, " ");
	    init_result++;
        }
        else if (!strncmp(Trim_String(user_data), "debug", 5) || !strncmp(Trim_String(user_data), "-debug", 6)){  /* Show debug dialog with logging? */
            #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
            gtk_widget_show_all(debug_window);
            #elif GTK_SERVER_XF
            fl_show_form(debug_window, FL_PLACE_CENTER, FL_FULLBORDER, "GTK-server Debugger");
            #elif GTK_SERVER_MOTIF
            XtRealizeWidget(debug_window);
            #endif
	    gtkserver.behave |= 512;
	    user_data = strtok(NULL, " ");
	    init_result++;
        }
        else if (!strncmp(Trim_String(user_data), "nonl", 4) || !strncmp(Trim_String(user_data), "-nonl", 5)){
	    gtkserver.behave |= 1024;
	    user_data = strtok(NULL, " ");
	    init_result++;
        }
        #ifdef GTK_SERVER_UNIX
        else if (!strncmp(Trim_String(user_data), "signal=", 7) || !strncmp(Trim_String(user_data), "-signal=", 8)){
	    gtkserver.behave |= 8;
            cache = user_data;                                                             /* Signal when exit */
	    user_data = strtok(NULL, " ");
	    init_result++;
        }
        #endif
        else if (!strncmp(Trim_String(user_data), "log=", 4) || !strncmp(Trim_String(user_data), "-log=", 5)){
	    gtkserver.LogDir = user_data;							            /* Set logfile */
	    user_data = strtok(NULL, " ");
	    init_result++;
        }
        else {
	    user_data = strtok(NULL, " ");
        }
    } while(user_data != NULL);
}

/* Now get actual settings */
if (user_cfgfile != NULL) {
    strtok(user_cfgfile, "=");
    user_cfgfile = strtok(NULL, "=");
}
if (gtkserver.pre != NULL) {
    strtok(gtkserver.pre, "=");
    gtkserver.pre = strtok(NULL, "=");
}
if (gtkserver.post != NULL) {
    strtok(gtkserver.post, "=");
    gtkserver.post = strtok(NULL, "=");
}
if (gtkserver.macro != NULL) {
    strtok(gtkserver.macro, "=");
    gtkserver.macro = strtok(NULL, "=");
}
if (gtkserver.LogDir != NULL) {
    strtok(gtkserver.LogDir, "=");
    gtkserver.LogDir = strtok(NULL, "=");
}
if (cache != NULL)
{
    strtok(cache, "=");
    gtkserver.exit_sig = atoi(strtok(NULL, "="));
}

#endif /* ----------------------------------------- of the ifndef LIBRARY/else ----------------------------------- */

if(gtkserver.pre == NULL) gtkserver.pre = strdup("");
if(gtkserver.post == NULL) gtkserver.post = strdup("");

/* Initialize the optional incoming client handle to NULL */
gtkserver.handle = strdup("");

/* If argument to configfile is entered it will overrule the other default locations (standalone exe)
This does not work with the LIBRARY version since it will always send a string through the 'gtk' function.  */
if (user_cfgfile != NULL){
    configfile = fopen(user_cfgfile, "r");
    #ifndef GTK_SERVER_LIBRARY
    if (configfile == NULL) Print_Error("%s", 1, "\nERROR: Entered configfile not found!");
    #endif
}
if (user_cfgfile == NULL || configfile == NULL) {
    /* Open the config file, first in the same directory as the script */
    configfile = fopen ("gtk-server.cfg", "r");
    if (configfile == NULL){
	/* Try to find the environment variable GTK_SERVER_CONFIG */
	libvar = getenv("GTK_SERVER_CONFIG");
	if (libvar != NULL) configfile = fopen(libvar, "r");
	#ifdef GTK_SERVER_WIN32
	if (configfile == NULL) {
	    /* Now look for configfile in same directory as gtk-server.exe ('me') */
	    GetModuleFileName(NULL, filename, MAX_PATH);
	    PathRemoveExtension(filename);
	    len = strlen(filename);
	    filename[len] = '.';
	    filename[++len] = 'c';
	    filename[++len] = 'f';
	    filename[++len] = 'g';
	    filename[++len] = '\0';
	    configfile = fopen(filename, "r");
	}
	#endif
	#ifdef GTK_SERVER_UNIX
	/* Find a configfile in the default Linux directory's */
	if (configfile == NULL) {
	    /* Homedir */
	    libvar = getenv("HOME");
	    if (libvar != NULL){
		filename = (char *)malloc(strlen(libvar) + 17);		/* Fix by Sebastiaan van Erk */
		strcpy(filename, libvar);
		strcat(filename, "/.gtk-server.cfg");
		configfile = fopen(filename, "r");
		free(filename);
	    }
	    if (configfile == NULL || libvar == NULL){
		configfile = fopen("/etc/gtk-server.cfg", "r");
		if (configfile == NULL){
		    configfile = fopen(LOCALCFG, "r");
		}
	    }
	}
	#endif
    }
}

/* No escaped special chars in returnstrings */
gtkserver.c_escaped = 0;

/* Setup which chars to escape by default */
gtkserver.escapes[0] = 7;    /* BELL */
gtkserver.escapes[1] = 9;    /* TAB */
gtkserver.escapes[2] = 10;   /* NEWLINE */
gtkserver.escapes[3] = 13;   /* CARRIAGE */
gtkserver.escapes[4] = 92;   /* BACKSLASH */
gtkserver.escapes[5] = 34;   /* DOUBLEQUOTE */
gtkserver.escapes[6] = 0;    /* Finish string */

/* Don't print print line before the returnstrings by default */
gtkserver.print_line_count = 0;

count_line = 0;

/* We have no libraries */
for(i = 0; i < MAX_LIBS; i++) libs[i] = NULL;

/* Go through all configfiles */
while (configfile != NULL){

/* Extract the entry's of the configfile into the array */
while (fgets (line, MAX_LEN, configfile) != NULL){
    count_line++;
    /* Check if the line is a comment or the line starts with spaces */
    if (strncmp(line, "#", 1) && strlen(line) > 1) {
	/* No, check if this is the gtklibname config setting */
	if (!strncmp(line, "LIB_NAME", 8)){
	    strtok(line, "=");
	    while((cache = strtok(NULL, ",")) != NULL && lib_counter < MAX_LIBS){
		libs[lib_counter] = strdup(Trim_String(cache));
		lib_counter++;
                if(lib_counter >= MAX_LIBS){
		    Print_Error("%s%d%s%d", 4, "\nERROR: Maximum amount of ", MAX_LIBS, " libraries reached at line: ", count_line);
                }
	    }
	    libs[lib_counter] = NULL;
	}
	/* Check if we have a GTK function definition */
	else if (!strncmp(line, "FUNCTION_NAME", 13)){
	    strtok(line, "=");
	    api_name = strdup(Trim_String(strtok(NULL, ",")));
	    if (api_name != NULL) {
		/* Check if this is not a duplicate */
		HASH_FIND_STR(gtk_protos, api_name, Gtk_Api_Config);
		if (Gtk_Api_Config != NULL) {
		    Print_Error("%s%d", 2, "\nERROR: Duplicate function name in configfile at line: ", count_line);
		}
		/* Claim memory */
		Gtk_Api_Config = (CONFIG*)malloc(sizeof(CONFIG));
		/* Assign the found GTK definition */
		Gtk_Api_Config->name = api_name;
		/* Get next term: callback signal type */
		cache = strtok(NULL, ",");
		if (cache == NULL) Print_Error("%s%d", 2, "\nERROR: Missing callbacktype in configfile at line: ", count_line);
		else Gtk_Api_Config->callbacktype = strdup(Trim_String(cache));
		/* Get next term: return value */
		cache = strtok(NULL, ",");
		if (cache == NULL) Print_Error("%s%d", 2, "\nERROR: Missing return value in configfile at line: ", count_line);
		else Gtk_Api_Config->returnvalue = strdup(Trim_String(cache));
		/* Get next term: amount of arguments */
		cache = strtok(NULL, ",");
		if (cache == NULL) Print_Error("%s%d", 2, "\nERROR: Missing argument amount in configfile at line: ", count_line);
		else strncpy(Gtk_Api_Config->argamount, Trim_String(cache), MAX_DIG-1);
		if (atoi(Gtk_Api_Config->argamount) > MAX_ARGS) Print_Error("%s%d%s%d", 4, "\nERROR: GTK definition cannot have more than ", MAX_ARGS, " arguments!\n\nError in configfile at line: ", count_line);
		/* Arguments to NULL, then get the separate arguments */
		for (i = 0; i < MAX_ARGS; i++){
		    Gtk_Api_Config->args[i] = NULL;
		}
		for (i = 0; i < atoi(Gtk_Api_Config->argamount); i++){
		    cache = strtok(NULL, ",");
		    if (cache == NULL) Print_Error("%s%d", 2, "\nERROR: Missing argument(s) in configfile at line: ", count_line);
		    else {
                        if(!strcmp(Trim_String(cache), "VARARGS")) {
                            snprintf(Gtk_Api_Config->argamount, MAX_DIG, "%d", MAX_ARGS);
                            for(j = i; j < MAX_ARGS; j++){
                                Gtk_Api_Config->args[j] = strdup("VARARGS");
                            }
                            break;
                        }
                        else {
                            Gtk_Api_Config->args[i] = strdup(Trim_String(cache));
                        }
                    }
		}
		/* Now add to hash table */
		HASH_ADD_KEYPTR(hh, gtk_protos, Gtk_Api_Config->name, strlen(Gtk_Api_Config->name), Gtk_Api_Config);
	    }
	}
	else if (!strncmp(line, "ENUM_NAME", 9)){
	    strtok(line, "=");
	    enum_name = strdup(Trim_String(strtok(NULL, ",")));
	    if (enum_name != NULL) {
		/* Check if this is not a duplicate */
		HASH_FIND_STR(enum_protos, enum_name, Enum_Defs);
		if (Enum_Defs != NULL) {
		    Print_Error("%s%d", 2, "\nERROR: duplicate enumeration name in configfile at line: ", count_line);
		}
		/* Claim memory */
		Enum_Defs = (ENUM*)malloc(sizeof(ENUM));
		/* Assign the found ENUM name */
		Enum_Defs->name = enum_name;
		/* Get next term: value of the enumeration */
		cache = strtok(NULL, ",");
		if (cache == NULL) Print_Error("%s%d", 2, "\nERROR: Missing value for enumeration in configfile at line: ", count_line);
		else {
		    if (sscanf(Trim_String(cache), "%i", &i) == EOF) Print_Error("%s%d", 2, "\nERROR: Erronous value for enumeration in configfile at line: ", count_line);
		    else Enum_Defs->value = i;
		}
		/* Now add to hash table */
		HASH_ADD_KEYPTR(hh, enum_protos, Enum_Defs->name, strlen(Enum_Defs->name), Enum_Defs);
	    }
	}
	else if (!strncmp(line, "STR_NAME", 8)){
	    strtok(line, "=");
	    str_name = strdup(Trim_String(strtok(NULL, ",")));
	    if (str_name != NULL) {
		/* Check if this is not a duplicate */
		HASH_FIND_STR(str_protos, str_name, Str_Defs);
		if (Str_Defs != NULL) {
		    Print_Error("%s%d", 2, "\nERROR: duplicate string name in configfile at line: ", count_line);
		}
		/* Claim memory */
		Str_Defs = (STR*)malloc(sizeof(STR));
		/* Assign the found ENUM name */
		Str_Defs->name = str_name;
		/* Get next term: value of the enumeration */
		cache = strtok(NULL, ",");
		if (cache == NULL) {
		    Print_Error("%s%d", 2, "\nERROR: missing value for enumeration in configfile at line: ", count_line);
		}
		else {
		    Str_Defs->value = strdup(Trim_String(cache));
		}
		/* Now add to hash table */
		HASH_ADD_KEYPTR(hh, str_protos, Str_Defs->name, strlen(Str_Defs->name), Str_Defs);
	    }
	}
	else if (!strncmp(line, "ALIAS_NAME", 10)){
	    strtok(line, "=");
	    alias_name = strdup(Trim_String(strtok(NULL, ",")));
	    if (alias_name != NULL) {
		/* Check if this is not a duplicate */
		HASH_FIND_STR(alias_protos, alias_name, Alias_Defs);
		if (Alias_Defs != NULL) {
		    Print_Error("%s%d", 2, "\nERROR: duplicate alias name in configfile at line: ", count_line);
		}
		/* Claim memory */
		Alias_Defs = (ALIAS*)malloc(sizeof(ALIAS));
		/* Assign the found ALIAS name */
		Alias_Defs->name = alias_name;
		HASH_ADD_KEYPTR(hh, alias_protos, Alias_Defs->name, strlen(Alias_Defs->name), Alias_Defs);
		/* Get next term: realname of the alias */
		cache = strtok(NULL, ",");
		if (cache == NULL) Print_Error("%s%d", 2, "\nERROR: Missing realname for alias in configfile at line: ", count_line);
		else {
		    /* Check if it exists */
		    HASH_FIND_STR(gtk_protos, Trim_String(cache), Gtk_Api_Config);
		    if (Gtk_Api_Config == NULL) {
			HASH_FIND_STR(macro_protos, Trim_String(cache), Macro_Defs);
			if (Macro_Defs == NULL){
			    Print_Error("%s%d%s", 3, "\nERROR: Realname for alias mentioned at line ", count_line, " not found!");
			}
			else {
			    Alias_Defs->real_m = Macro_Defs;
			    Alias_Defs->real = NULL;
			}
		    }
		    else {
			Alias_Defs->real = Gtk_Api_Config;
			Alias_Defs->real_m = NULL;
		    }
		}
	    }
	}
	/* Check if we have a INCLUDE definition */
	else if (!strncmp(line, "INCLUDE", 7)){
	    strtok(line, "=");
	    cache = strtok(NULL, "=");
	    if (cache == NULL) Print_Error("%s%d", 2, "\nERROR: Missing configfile in configfile at line: ", count_line);
	    else include = fopen(Trim_String(cache), "r");
	}
	/* Check if we have a sequence definition */
	else if (!strncmp(line, "SEQUENCE", 8)){
	    strtok(line, "=");
	    cache = strtok(NULL, "=");
	    if (cache == NULL) Print_Error("%s%d", 2, "\nERROR: Missing library sequence number in configfile at line: ", count_line);
	    else gtkserver.libseq = atoi(Trim_String(cache));
	}
	/* Is this a macro? */
	else if (!strncmp(line, "MACRO", 5)){
	    strtok(line, " ");
	    cache = strtok(NULL, " ");
	    if (cache == NULL) Print_Error("%s%d", 2, "\nERROR: Macro without name at line: ", count_line);
	    /* Check if this is not a duplicate */
	    HASH_FIND_STR(macro_protos, Trim_String(cache), Macro_Defs);
	    if (Macro_Defs != NULL) Print_Error("%s%d", 2, "\nERROR: duplicate macro name in configfile at line: ", count_line);
	    /* Claim memory */
	    Macro_Defs = (MACRO*)malloc(sizeof(MACRO));
	    Macro_Defs->name = strdup(Trim_String(cache));
	    /* Put all local macro variables to NULL */
	    for(i=0; i<26; i++) Macro_Defs->var[i]=NULL;
	    /* Collect macro text */
	    Body_Text = (BODY*)malloc(sizeof(BODY));
	    Macro_Defs->body = Body_Text;
	    Body_Text->prev = NULL;
	    while (fgets (line, MAX_LEN, configfile) != NULL && strncmp(Trim_String(line), "ENDMACRO", 8)){
		count_line++;
		if (strncmp(line, "#", 1) && strlen(line) > 1) {
		    Body_Text->text = strdup(Trim_String(line));
		    Body_Last = Body_Text;
		    Body_Text->next = (BODY*)malloc(sizeof(BODY));
		    Body_Text = Body_Text->next;
		    Body_Text->prev = Body_Last;
		    Body_Text->next = NULL;
		}
	    }
	    /* The ENDMACRO is another line, so count it */
	    count_line++;
	    free(Body_Text);
	    Body_Last->next=NULL;
	    /* Add to hash table */
	    HASH_ADD_KEYPTR(hh, macro_protos, Macro_Defs->name, strlen(Macro_Defs->name), Macro_Defs);
	}
	else Print_Error("%s%d", 2, "\nERROR: Config not recognized at line: ", count_line);
    }
}
configfile = include;
include = NULL;
count_line = 0;
}

#ifdef GTK_SERVER_MOTIF

/* Add Motif class types */
#define MOTIF_ADD_CLASS(x) do { \
    HASH_FIND_STR(class_protos, #x, Class_Defs); \
    Class_Defs = (CLASS*)malloc(sizeof(CLASS)); \
    Class_Defs->name = #x; \
    Class_Defs->ptr = x; \
    HASH_ADD_KEYPTR(hh, class_protos, #x, strlen(#x), Class_Defs); \
} while(0)

MOTIF_ADD_CLASS(xmArrowButtonWidgetClass);
MOTIF_ADD_CLASS(xmBulletinBoardWidgetClass);
MOTIF_ADD_CLASS(xmCascadeButtonWidgetClass);
MOTIF_ADD_CLASS(xmColumnWidgetClass);
MOTIF_ADD_CLASS(xmComboBoxWidgetClass);
MOTIF_ADD_CLASS(xmCommandWidgetClass);
MOTIF_ADD_CLASS(xmContainerWidgetClass);
MOTIF_ADD_CLASS(xmDataFieldWidgetClass);
MOTIF_ADD_CLASS(xmDialogShellWidgetClass);
MOTIF_ADD_CLASS(xmDragOverShellWidgetClass);
MOTIF_ADD_CLASS(xmDrawingAreaWidgetClass);
MOTIF_ADD_CLASS(xmDrawnButtonWidgetClass);
MOTIF_ADD_CLASS(xmFileSelectionBoxWidgetClass);
MOTIF_ADD_CLASS(xmFormWidgetClass);
MOTIF_ADD_CLASS(xmFrameWidgetClass);
MOTIF_ADD_CLASS(xmGrabShellWidgetClass);
MOTIF_ADD_CLASS(xmIconButtonWidgetClass);
MOTIF_ADD_CLASS(xmLabelWidgetClass);
MOTIF_ADD_CLASS(xmListWidgetClass);
MOTIF_ADD_CLASS(xmMainWindowWidgetClass);
MOTIF_ADD_CLASS(xmManagerWidgetClass);
MOTIF_ADD_CLASS(xmMenuShellWidgetClass);
MOTIF_ADD_CLASS(xmMessageBoxWidgetClass);
MOTIF_ADD_CLASS(xmNotebookWidgetClass);
MOTIF_ADD_CLASS(xmPanedWidgetClass);
MOTIF_ADD_CLASS(xmPanedWindowWidgetClass);
MOTIF_ADD_CLASS(xmPrimitiveWidgetClass);
MOTIF_ADD_CLASS(xmPushButtonWidgetClass);
MOTIF_ADD_CLASS(xmRowColumnWidgetClass);
MOTIF_ADD_CLASS(xmScaleWidgetClass);
MOTIF_ADD_CLASS(xmScrollBarWidgetClass);
MOTIF_ADD_CLASS(xmScrolledWindowWidgetClass);
MOTIF_ADD_CLASS(xmSelectionBoxWidgetClass);
MOTIF_ADD_CLASS(xmSeparatorWidgetClass);
MOTIF_ADD_CLASS(xmSimpleSpinBoxWidgetClass);
MOTIF_ADD_CLASS(xmSpinBoxWidgetClass);
MOTIF_ADD_CLASS(xmTabBoxWidgetClass);
MOTIF_ADD_CLASS(xmTabStackWidgetClass);
MOTIF_ADD_CLASS(xmTextFieldWidgetClass);
MOTIF_ADD_CLASS(xmTextWidgetClass);
MOTIF_ADD_CLASS(xmToggleButtonWidgetClass);
MOTIF_ADD_CLASS(xmArrowButtonGadgetClass);
MOTIF_ADD_CLASS(xmCascadeButtonGadgetClass);
MOTIF_ADD_CLASS(xmGadgetClass);
MOTIF_ADD_CLASS(xmLabelGadgetClass);
MOTIF_ADD_CLASS(xmPushButtonGadgetClass);
MOTIF_ADD_CLASS(xmSeparatorGadgetClass);
MOTIF_ADD_CLASS(xmToggleButtonGadgetClass);
MOTIF_ADD_CLASS(applicationShellWidgetClass);
MOTIF_ADD_CLASS(compositeWidgetClass);
MOTIF_ADD_CLASS(constraintWidgetClass);
MOTIF_ADD_CLASS(coreWidgetClass);
MOTIF_ADD_CLASS(topLevelShellWidgetClass);
MOTIF_ADD_CLASS(transientShellWidgetClass);
MOTIF_ADD_CLASS(vendorShellWidgetClass);
MOTIF_ADD_CLASS(wmShellWidgetClass);
#endif

/* Now see if we need to print the complete configuration (for debugging) */
if (gtkserver.behave & 1){

    i = 1;
    while(libs[i] != NULL){
	fprintf(stdout, "LIB_NAME = %s\n", libs[i]);
	i++;
    }

    for(Gtk_Api_Config=gtk_protos; Gtk_Api_Config != NULL; Gtk_Api_Config=Gtk_Api_Config->hh.next) {

	fprintf(stdout, "(%s)", Gtk_Api_Config->callbacktype);
	fprintf(stdout, "  %s", Gtk_Api_Config->returnvalue);
	fprintf(stdout, " = %s", Gtk_Api_Config->name);

	for (i = 0; i < atoi(Gtk_Api_Config->argamount); i++) {
	    fprintf(stdout,", %s", Gtk_Api_Config->args[i]);
	}
	fprintf(stdout, "\n");
    }

    for(Enum_Defs = enum_protos; Enum_Defs != NULL; Enum_Defs=Enum_Defs->hh.next){
	fprintf(stdout, "ENUM %s = %d\n", Enum_Defs->name, Enum_Defs->value);
    }

    for(Str_Defs = str_protos; Str_Defs != NULL; Str_Defs=Str_Defs->hh.next){
	fprintf(stdout, "STR %s = %s\n", Str_Defs->name, Str_Defs->value);
    }

    #ifdef GTK_SERVER_MOTIF
    for(Class_Defs = class_protos; Class_Defs != NULL; Class_Defs=Class_Defs->hh.next){
	fprintf(stdout, "CLASS %s\n", Class_Defs->name);
    }
    #endif

    for(Macro_Defs=macro_protos; Macro_Defs != NULL; Macro_Defs=Macro_Defs->hh.next){
	fprintf(stdout, "MACRO \"%s\"\n", Macro_Defs->name);
    }

    exit(EXIT_SUCCESS);
}

/* Always try to remove the logfile, ignore any errors */
if (gtkserver.LogDir != NULL) unlink(gtkserver.LogDir);

#ifndef GTK_SERVER_LIBRARY
/* Only create logfile when argument is given, if logfile cannot be created: warn, but continue */
if (gtkserver.LogDir != NULL){
    logfile = fopen (gtkserver.LogDir, "w");
    #ifdef GTK_SERVER_UNIX
    if (logfile == NULL) fprintf(stderr, "WARNING: The logfile could not be created.\n");
    #elif GTK_SERVER_WIN32
    if (logfile == NULL) MessageBox(NULL, "WARNING: The logfile could not be created.", "GTK-server warning", MB_OK | MB_ICONEXCLAMATION);
    #endif
}
#endif

#if GTK_SERVER_FFI || GTK_SERVER_FFCALL
    /* Open the GTK or XForms library */
    #ifdef GTK_SERVER_UNIX
	handle[0] = dlopen(NULL, RTLD_LAZY);

	i = 1; j = 1;
	while(libs[j] != NULL && j < MAX_LIBS){
	    handle[i] = search_and_open_lib(libs[j]);
	    if(handle[i]) { i++; }
            j++;
	}

    #elif GTK_SERVER_WIN32
	i = GetModuleFileName(NULL, myname, MAX_PATH);
	if (i > 0) {
	    handle[0] = LoadLibrary(myname);
	    i = 1;
            j = 1;
	}
	else
        {
            i = 0;
            j = 0;
        }
	while(libs[j] != NULL && j < MAX_LIBS){
	    handle[i] = LoadLibrary(libs[j]);
	    if(handle[i]) { i++; }
            j++;
	}
    #endif

#elif GTK_SERVER_CINV

    cinv_ctx = cinv_context_create();
    if (cinv_ctx == NULL) Print_Error("%s", 1, "\nERROR: Cannot create C/Invoke context!\n");

    handle[0] = cinv_library_create(cinv_ctx, NULL);

    i = 1; j = 1;
    while(libs[j] != NULL && j < MAX_LIBS){
	handle[i] = search_and_open_lib(libs[j], cinv_ctx);
	if(handle[i]) { i++; }
        j++;
    }

#elif GTK_SERVER_DYNCALL

    handle[0] = dlLoadLibrary(NULL);

    i = 1; j = 1;
    while(libs[j] != NULL && j < MAX_LIBS){
	handle[i] = search_and_open_lib(libs[j]);
	if(handle[i]) { i++; }
        j++;
    }

#endif

#ifndef GTK_SERVER_LIBRARY
/* Should a macro be running first ? */
if (gtkserver.behave & 16 && gtkserver.macro != NULL){
    /* Check if macro exists */
    HASH_FIND_STR(macro_protos, Trim_String(gtkserver.macro), Macro_Defs);
    if (Macro_Defs == NULL) Print_Error("%s", 1, "\nERROR: Cannot find macro to execute!");

    /* If logging is enabled, write macro to log */
    if (logfile != NULL){fprintf(logfile, "COMMAND LINE: %s\n", gtkserver.macro); fflush(logfile); }

    retstr = Call_Realize(Trim_String(gtkserver.macro), cinv_ctx);

    /* If logging is enabled, write returnstring to log */
    if (logfile != NULL){fprintf(logfile, "SERVER: %s\n", retstr); fflush(logfile); }
}

/**************************************************************************************** STDIN */

/* There is an argument, check on STDIN flag */
if (gtkserver.mode == 1) {

    /* Set the page factor used by realloc to 0 */
    page = 0;

    /* This is the main input loop via STDIN */
    while (1) {

	/* Debug window */
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) {
	    /* Poll descriptor to see if data is available */
	    while(debug_step == 0)
	    {
		debug_step = debug_run;
		do {
		    FD_ZERO(&rfds);
		    FD_SET(STDIN_FILENO, &rfds);
		    /* Set a time to avoid 100% CPU load */
		    tv.tv_usec = 1000;
		    tv.tv_sec = 0;
		    /* Update the debugging GUI */
		    update_gui;
		} while (select(STDIN_FILENO+1, &rfds, NULL, NULL, &tv) == 0);
	    }
	    if(debug_run || debug_step == 1) { debug_step = 0; }
	}
	#endif

	/* Main STDIN interface handling */
	len = 0;
	do {
	    i = read(STDIN_FILENO, line, MAX_LEN);

	    /* Fix by Sebastiaan van Erk */
	    if (i == -1) Print_Error("%s%s", 2, "\nError in reading from STDIN: ", strerror(errno));

	    if (i == 0) exit(EXIT_SUCCESS);

	    /* Realloc preserves contents, add one for \0 which is added later */
	    if (len + i > MAX_LEN*page + 1) {page++; in = realloc(in, MAX_LEN*page + 1);}

	    if (in == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to read from STDIN: ", strerror(errno));

	    memcpy(in + len, line, i);
	    len += i;
	}
	while (i == MAX_LEN || i == 1);

	/* Make sure we have a NULL terminated character array */
	if (len >= 0) in[len] = '\0';

	/* If logging is enabled, write incoming text to log */
	if (logfile != NULL){
	    fprintf(logfile, "SCRIPT: %s\n", Trim_String(in));
	    fflush(logfile);
	}
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) { scroll_to_end(debug_view, Trim_String(in), 3); }
	#endif

	retstr = Call_Realize(Trim_String(in), cinv_ctx);

	/* If logging is enabled, write returnstring to log */
	if (logfile != NULL){
	    fprintf(logfile, "SERVER: %s\n", retstr);
	    fflush(logfile);
	}
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) { scroll_to_end(debug_view, retstr, 1); }
	#endif

	/* Answer from GTK-server */
	fprintf(stdout, "%s", retstr);

	fflush(stdout);
    }
}

#ifdef GTK_SERVER_UNIX
/**************************************************************************************** Linux FIFO */
/* Check on the FIFO flag */
if (gtkserver.mode == 2) {

    /* Try to spawn to background if argument '-detach' was entered */
    if (gtkserver.behave & 4) {
	i=fork();
	if (i < 0) Print_Error("%s%s", 2, "\nCould not spawn to background! ERROR: ", strerror(errno));
	if (i > 0) exit(EXIT_SUCCESS); /* parent exits */
    }

    /* Make sure pipe file is removed at exit */
    if (atexit(remove_pipe) != 0) {
	if (logfile != NULL){
	    fprintf(logfile, "FIFO: Cannot set exit function. Please remove pipe file manually.\n");
	    fflush(logfile);
	}
    }

    /* Create FIFO file */
    if (!(gtkserver.behave & 2)){
	if (mkfifo(gtkserver.fifo, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH) < 0) {
	    Print_Error("%s%s", 2, "\nCould not create FIFO! ERROR: ", strerror(errno));
	}
    }

    /* Set the page factor used by realloc to 0 */
    page = 0;

    /* This is the main input loop via FIFO */
    while (1) {

	/* First open in READ mode */
	if((sockfd = open(gtkserver.fifo, O_RDONLY)) < 0){
	    Print_Error("%s%s", 2, "\nError opening FIFO: ", strerror(errno));
	}

	/* Debug window */
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) {
	    /* Poll descriptor to see if data is available */
	    while(debug_step == 0)
	    {
		debug_step = debug_run;
		do {
		    FD_ZERO(&rfds);
		    FD_SET(sockfd, &rfds);
		    /* Set a time to avoid 100% CPU load */
		    tv.tv_usec = 1000;
		    tv.tv_sec = 0;
		    /* Update the debugging GUI */
		    update_gui;
		} while (select(sockfd+1, &rfds, NULL, NULL, &tv) == 0);
	    }
	    if(debug_run || debug_step == 1) { debug_step = 0; }
	}
	#endif

	/* Now wait for data */
	len = 0;
	do {
	    i = read(sockfd, line, MAX_LEN);

	    /* Fix by Sebastiaan van Erk */
	    if (i == -1) Print_Error("%s%s", 2, "\nError in reading from FIFO: ", strerror(errno));

	    /* Realloc preserves contents, add one for \0 which is added later */
	    if (len + i > MAX_LEN*page + 1) {page++; in = realloc(in, MAX_LEN*page + 1);}

	    if (in == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to read from FIFO: ", strerror(errno));

	    memcpy(in + len, line, i);
	    len += i;
	}
	while (i == MAX_LEN);

	/* Make sure we have a NULL terminated character array */
	if(len >= 0) in[len] = '\0';

	/* Close socket again */
	if(close(sockfd) < 0){
	    Print_Error("%s%s", 2, "\nError closing FIFO: ", strerror(errno));
	}

	/* If logging is enabled, write incoming text to log */
	if (logfile != NULL){
	    fprintf(logfile, "SCRIPT: %s\n", Trim_String(in));
	    fflush(logfile);
	}
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) { scroll_to_end(debug_view, Trim_String(in), 3); }
	#endif

	retstr = Call_Realize(Trim_String(in), cinv_ctx);

	/* If logging is enabled, write returnstring to log */
	if (logfile != NULL){
	    fprintf(logfile, "SERVER: %s\n", retstr);
	    fflush(logfile);
	}
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) { scroll_to_end(debug_view, retstr, 1); }
	#endif

	/* Now open in WRITE mode */
	if((sockfd = open(gtkserver.fifo, O_WRONLY)) < 0){
	    Print_Error("%s%s", 2, "\nError opening FIFO: ", strerror(errno));
	}

	/* Answer from GTK-server */
	if(write (sockfd, retstr, strlen(retstr)) < 0) Print_Error("%s%s", 2, "\nError writing FIFO: ", strerror(errno));

	/* Close socket again */
	if(close(sockfd) < 0){
	    Print_Error("%s%s", 2, "\nError closing FIFO: ", strerror(errno));
	}
    }
}
#endif

#ifdef GTK_SERVER_WIN32
/**************************************************************************************** Win32 FIFO */
/* Check for a named PIPE */
if (gtkserver.mode == 2) {

    /* Check the Operating System version first */
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi))) {
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	if (! GetVersionEx ((OSVERSIONINFO *) &osvi)) return FALSE;
    }

    /* If windows9x/ME then block PIPE function */
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) Print_Error("%s", 1, "\nERROR: Named pipes are available on WINNT, WIN2000 and WINXP only!");

    /* Now create named PIPE - code ripped from msdn.microsoft.com */
    hPipe1 = CreateNamedPipe(
	"\\\\.\\pipe\\out",         /* pipe name */
	PIPE_ACCESS_INBOUND,       /* read/write access */
	PIPE_TYPE_MESSAGE |       /* message type pipe */
	PIPE_READMODE_MESSAGE |   /* message-read mode */
	PIPE_WAIT,                /* blocking mode */
	PIPE_UNLIMITED_INSTANCES, /* max. instances */
	MAX_LEN,                  /* output buffer size */
	MAX_LEN,                  /* input buffer size */
	NMPWAIT_USE_DEFAULT_WAIT, /* client time-out */
	NULL);                    /* no security attribute */

    /* Exit if pipe cannot be created */
    if (hPipe1 == INVALID_HANDLE_VALUE) Print_Error("%s", 1, "\nERROR: Could not create a named pipe!");

    /* Now create named PIPE - code ripped from msdn.microsoft.com */
    hPipe2 = CreateNamedPipe(
	"\\\\.\\pipe\\in",        /* pipe name */
	PIPE_ACCESS_OUTBOUND,     /* read/write access */
	PIPE_TYPE_MESSAGE |       /* message type pipe */
	PIPE_READMODE_MESSAGE |   /* message-read mode */
	PIPE_WAIT,                /* blocking mode */
	PIPE_UNLIMITED_INSTANCES, /* max. instances */
	MAX_LEN,                  /* output buffer size */
	MAX_LEN,                  /* input buffer size */
	NMPWAIT_USE_DEFAULT_WAIT, /* client time-out */
	NULL);                    /* no security attribute */

    /* Exit if pipe cannot be created */
    if (hPipe2 == INVALID_HANDLE_VALUE) Print_Error("%s", 1, "\nERROR: Could not create a named pipe!\n");

    /* Wait for connection from client script */
    if (ConnectNamedPipe(hPipe1, NULL)) {
	if (ConnectNamedPipe(hPipe2, NULL)) {

	    /* Set the page factor used by realloc to 0 */
	    page = 0;

	    /* This is the main input loop via FIFO */
	    while(1) {
		/* Now wait for data */
		len = 0;
		do {
		    /* Connection established, now communicate */
		    fSuccess = ReadFile(
		    hPipe1,			/* handle to pipe */
		    chRequest,			/* buffer to receive data */
		    MAX_LEN,			/* size of buffer */
		    &cbBytesRead,		/* number of bytes read */
		    NULL);			/* not overlapped I/O */

		    if (! fSuccess || cbBytesRead == 0) {
			FlushFileBuffers(hPipe1);
			DisconnectNamedPipe(hPipe1);
			CloseHandle(hPipe1);
			Print_Error("%s%d", 2, "\nERROR: GetLastError returned: ", (int)GetLastError());
		    }

		    /* Realloc preserves contents, add one for \0 which is added later */
		    if (len + cbBytesRead > MAX_LEN*page + 1) {page++; in = realloc(in, MAX_LEN*page + 1);}

		    if (in == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to read from FIFO: ", (int)GetLastError());

		    memcpy(in + len, chRequest, cbBytesRead);
		    len += cbBytesRead;
		}
		while (cbBytesRead == MAX_LEN);

		/* Make sure we have a NULL terminated character array */
		if(len >= 0) in[len] = '\0';

		/* If logging is enabled, write incoming text to log */
		if (logfile != NULL){
		    fprintf(logfile, "SCRIPT: %s\n", Trim_String(in));
		    fflush(logfile);
		}

		retstr = Call_Realize(Trim_String(in), cinv_ctx);

		/* If logging is enabled, write returnstring to log */
		if (logfile != NULL){
		    fprintf(logfile, "SERVER: %s\n", retstr);
		    fflush(logfile);
		}

		fSuccess = WriteFile(hPipe2, retstr, strlen(retstr), &cbWritten, NULL);

		/* Error handling */
		if (! fSuccess || strlen(retstr) != cbWritten) {
		    FlushFileBuffers(hPipe2);
		    DisconnectNamedPipe(hPipe2);
		    CloseHandle(hPipe2);
		    Print_Error("%s%d", 2, "\nERROR: GetLastError returned: ", (int)GetLastError());
		}
	    } /* Endless while */
	} /* If connect */
	else {
	    /* The client could not connect, so close the pipe. */
	    CloseHandle(hPipe2);
	    Print_Error("%s", 1, "\nERROR: Connection pipe 'in' refused!");
	}
    }
    else {
	/* The client could not connect, so close the pipe. */
	CloseHandle(hPipe1);
	Print_Error("%s", 1, "\ERROR: nConnection pipe 'out' refused!");
    }
}
#endif

/**************************************************************************************** TCP */
/* Else consider this to be TCP socket - host:port */
if (gtkserver.mode == 3) {

/*Check argument format */
if (strstr(gtkserver.tcp, ":") == NULL) Print_Error("%s", 1, "\nERROR: Argument is in wrong format!");

host = Trim_String(strtok(gtkserver.tcp, ":"));
port = Trim_String(strtok(NULL, ":"));
#ifdef GTK_SERVER_UNIX
maxtcp = strtok(NULL, ":");
#endif
if (maxtcp == NULL){
    maxtcp = (char*)malloc(8*sizeof(char));
    strcpy(maxtcp, "1");
}

sockfd = 0;
#ifdef GTK_SERVER_WIN32
/* Initialize Win32 environment for sockets */
WSADATA wsaData;
if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) Print_Error("%s", 1, "\nWSAStartup() failed!");
#endif
/* get the host info */
if ((he=gethostbyname(host)) == NULL){
    Print_Error ("%s%s", 2, "\nSpecified host does not exist!\n\nSystem message: ", strerror(errno));
}
else if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    Print_Error("%s%s", 2, "\nCannot connect to socket!\n\nSystem message: ", strerror(errno));
}
#ifdef GTK_SERVER_UNIX
else if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
#endif
#ifdef GTK_SERVER_WIN32
else if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int)) == -1)
#endif
{
    Print_Error("%s%s", 2, "\nCannot configure socket!\n\nSystem message: ", strerror(errno));
}
/* Perform BIND to the host:port */
else {
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons((long)atol(port));             /* Fill in specified port */
    my_addr.sin_addr = *((struct in_addr *)he->h_addr);     /* Fill in specified IP address */
    memset(&(my_addr.sin_zero), '\0', sizeof(my_addr.sin_zero));                   /* zero the rest of the struct */

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
	Print_Error("%s%s", 2, "\nUnable to bind the specified socket address!\n\nSystem message: ", strerror(errno));
    }

    else if (listen(sockfd, BACKLOG) == -1) {
	Print_Error("%s%s", 2, "\nUnable to listen to the specified socket address!\n\nSystem message: ", strerror(errno));
    }
}
#ifdef GTK_SERVER_UNIX
if (atoi(maxtcp) > 1) {
    if (signal(SIGCHLD, sig_handler) == SIG_ERR) {
	Print_Error("%s%s", 2, "\nCould not set signal handler! ERROR: ", strerror(errno));
    }
}

/* Try to spawn to background if argument '-detach' was entered */
if (gtkserver.behave & 4) {
    i=fork();
    if (i < 0) Print_Error("%s%s", 2, "\nCould not spawn to background! ERROR: ", strerror(errno));
    if (i > 0) exit(EXIT_SUCCESS); /* parent exits */
}

/* No forks yet */
gtkserver.count_fork = 0;
#endif
/* Now accept incoming connections */
while (1){
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1){
	perror("accept");
	continue;
    }
    #ifdef GTK_SERVER_WIN32
    closesocket(sockfd);
    #endif
    #ifdef GTK_SERVER_UNIX
    /* Only one client may connect? No -> fork */
    if (atoi(maxtcp) > 1){
	/* Max not reached? Fork */
	if (gtkserver.count_fork < atoi(maxtcp)){
	    curpid = fork();
	    /* Count connect */
	    gtkserver.count_fork++;
	}
    }
    else curpid = 0;
    /* We are in the non-forked process or in the child process */
    if (!curpid){
	/* Close the listener socket */
	close (sockfd);
	#endif
	/* Set the page factor used by realloc to 0 */
	page = 0;

	/* We enter the mainloop - read incoming text */
	while(1){

	    /* Debug window */
	    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	    if (gtkserver.behave & 512) {
		/* Poll descriptor to see if data is available */
		while(debug_step == 0)
		{
		    debug_step = debug_run;
		    do {
			FD_ZERO(&rfds);
			FD_SET(new_fd, &rfds);
			/* Set a time to avoid 100% CPU load */
			tv.tv_usec = 1000;
			tv.tv_sec = 0;
			/* Update the debugging GUI */
			update_gui;
		    } while (select(new_fd+1, &rfds, NULL, NULL, &tv) == 0);
		}
		if(debug_run || debug_step == 1) { debug_step = 0; }
	    }
	    #endif

	    len = 0;
	    do {
		numbytes = recv(new_fd, buf, MAX_LEN, 0);

		if (numbytes < 0) Print_Error("%s%s", 2, "\nError in reading from TCP: ", strerror(errno));

    		/* Realloc preserves contents, add one for \0 which is added later */
		if (len + numbytes > MAX_LEN*page + 1) {page++; in = realloc(in, MAX_LEN*page + 1);}

		if (in == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to read from TCP: ", strerror(errno));

		memcpy(in + len, buf, numbytes);
		len += numbytes;
	    }
	    while (numbytes == MAX_LEN);

	    /* If disconnected, quit the gtk-server */
	    if (numbytes == 0) exit(EXIT_SUCCESS);

    	    /* Make sure we have a NULL terminated character array */
	    if (len > 0) in[len] = '\0';

	    /* If logging is enabled, write incoming text to log */
	    if (logfile != NULL){
		fprintf(logfile, "SCRIPT: %s\n", Trim_String(in));
		fflush(logfile);
	    }
	    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	    if (gtkserver.behave & 512) { scroll_to_end(debug_view, Trim_String(in), 3); }
	    #endif

	    retstr = Call_Realize(Trim_String(in), cinv_ctx);

	    /* If logging is enabled, write returnstring to log */
	    if (logfile != NULL){
		fprintf(logfile, "SERVER: %s\n", retstr);
		fflush(logfile);
	    }
	    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	    if (gtkserver.behave & 512) { scroll_to_end(debug_view, retstr, 1); }
	    #endif

	    /* Now send the result back to the socket */
	    numbytes = send(new_fd, retstr, strlen(retstr), 0);

	    if (numbytes < 0) Print_Error("%s%s", 2, "\nError in sending to TCP: ", strerror(errno));
	}
    #ifdef GTK_SERVER_UNIX
    }
    /* This is the parent process in case the server was forked */
    else {
	/* Close the new formed socket, the parent process does not need it */
	close (new_fd);
    }
    #endif
} /* Endless while */
} /* End TCP story */

/**************************************************************************************** TCP client */
/* Else consider this to be TCP client socket - host:port */
if (gtkserver.mode == 6) {

    /* Try to spawn to background if argument '-detach' was entered */
    #ifdef GTK_SERVER_UNIX
    if (gtkserver.behave & 4) {
	i=fork();
	if (i < 0) Print_Error("%s%s", 2, "\nCould not spawn to background! ERROR: ", strerror(errno));
	if (i > 0) exit(EXIT_SUCCESS); /* parent exits */
    }
    #endif

    /*Check argument format */
    if (strstr(gtkserver.tcp, ":") == NULL) Print_Error("%s", 1, "\nERROR: Argument is in wrong format!");

    host = Trim_String(strtok(gtkserver.tcp, ":"));
    port = Trim_String(strtok(NULL, ":"));

    sockfd = 0;
    #ifdef GTK_SERVER_WIN32
    /* Initialize Win32 environment for sockets */
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) Print_Error("%s", 1, "\nWSAStartup() failed!");
    #endif

    /* get the host info */
    if ((he = gethostbyname(host)) == NULL){
        Print_Error ("%s%s", 2, "\nSpecified host does not exist!\n\nSystem message: ", strerror(errno));
    }
    else if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        Print_Error("%s%s", 2, "\nCannot connect to socket!\n\nSystem message: ", strerror(errno));
    }
    #ifdef GTK_SERVER_UNIX
    else if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    #endif
    #ifdef GTK_SERVER_WIN32
    else if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) == -1)
    #endif
    {
        Print_Error("%s%s", 2, "\nCannot configure socket!\n\nSystem message: ", strerror(errno));
    }
    /* Perform BIND to the host:port */
    else {
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons((long)atol(port));			/* Fill in specified port */
        my_addr.sin_addr = *((struct in_addr *)he->h_addr);		/* Fill in specified IP address */
        memset(&(my_addr.sin_zero), '\0', sizeof(my_addr.sin_zero));	/* zero the rest of the struct */
	if (connect(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
            Print_Error("%s%s", 2, "\nUnable to connect to server socket address!\n\nSystem message: ", strerror(errno));
        }
    }

    #ifdef GTK_SERVER_USE_SSL
    /* Check if we have to do SSL */
    if (gtkserver.behave & 192){

	/* Initialize SSL */
	ctx = initialize_ctx(gtkserver.certificate, gtkserver.password, logfile);

	/* Connect the SSL socket */
	ssl = SSL_new(ctx);
	sbio = BIO_new_socket(sockfd, BIO_NOCLOSE);
	SSL_set_bio(ssl, sbio, sbio);

    	i = SSL_connect(ssl);

	switch (SSL_get_error(ssl, i)){
	    case SSL_ERROR_SSL:
		Print_Error("%s", 1, "\nSSL connect error: SSL_ERROR_SSL");
		break;
	    case SSL_ERROR_ZERO_RETURN:
		Print_Error("%s", 1, "\nSSL connect error: SSL_ERROR_ZERO_RETURN");
		break;
	    case SSL_ERROR_WANT_READ:
		Print_Error("%s", 1, "\nSSL connect error: SSL_ERROR_WANT_READ");
		break;
	    case SSL_ERROR_WANT_WRITE:
		Print_Error("%s", 1, "\nSSL connect error: SSL_ERROR_WANT_WRITE");
		break;
	    case SSL_ERROR_WANT_CONNECT:
		Print_Error("%s", 1, "\nSSL connect error: SSL_ERROR_WANT_CONNECT");
		break;
	    case SSL_ERROR_WANT_ACCEPT:
		Print_Error("%s", 1, "\nSSL connect error: SSL_ERROR_WANT_ACCEPT");
		break;
	    case SSL_ERROR_WANT_X509_LOOKUP:
		Print_Error("%s", 1, "\nSSL connect error: SSL_ERROR_WANT_X509_LOOKUP");
		break;
	    case SSL_ERROR_SYSCALL:
		Print_Error("%s", 1, "\nSSL connect error: SSL_ERROR_SYSCALL");
		break;
	}
	/* Do we need to verify the server we are connecting to? */
	if(gtkserver.behave & 128) check_cert(ssl, host, logfile);
    }
    #endif

    /* Should we send an INIT string first? */
    if (gtkserver.behave & 32){
	if (gtkserver.behave & 192){
	    #ifdef GTK_SERVER_USE_SSL
	    numbytes = SSL_write(ssl, gtkserver.init, strlen(gtkserver.init));
	    numbytes += SSL_write(ssl, "\n", sizeof(char));
	    #else
	    Print_Error("%s", 1, "\nERROR: GTK-server was compiled without SSL support!");
	    #endif
	}
	else {
	    numbytes = send(sockfd, gtkserver.init, strlen(gtkserver.init), 0);
	    numbytes += send(sockfd, "\n", sizeof(char), 0);
	}
	if (numbytes < 0) Print_Error("%s%s", 2, "\nError in sending to SOCKET: ", strerror(errno));
    }

    /* Set the page factor used by realloc to 0 */
    page = 0;

    /* We enter the mainloop - read incoming text */
    while(1){

	/* Debug window */
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) {
	    /* Poll descriptor to see if data is available */
	    while(debug_step == 0)
	    {
		debug_step = debug_run;
		do {
		    FD_ZERO(&rfds);
		    FD_SET(sockfd, &rfds);
		    /* Set a time to avoid 100% CPU load */
		    tv.tv_usec = 1000;
		    tv.tv_sec = 0;
		    /* Update the debugging GUI */
		    update_gui;
		} while (select(sockfd+1, &rfds, NULL, NULL, &tv) == 0);
	    }
	    if(debug_run || debug_step == 1) { debug_step = 0; }
	}
	#endif

	len = 0;
	do {
	    if (gtkserver.behave & 192){
		#ifdef GTK_SERVER_USE_SSL
		numbytes = SSL_read(ssl, buf, MAX_LEN);
		#else
		Print_Error("%s", 1, "\nERROR: GTK-server was compiled without SSL support!");
		#endif
	    }
	    else numbytes = recv(sockfd, buf, MAX_LEN, 0);

	    if (numbytes < 0) Print_Error("%s%s", 2, "\nError in reading from SOCKET: ", strerror(errno));

	    /* Realloc preserves contents, add one for \0 which is added later */
	    if (len + numbytes > MAX_LEN*page + 1) {page++; in = realloc(in, MAX_LEN*page + 1);}

	    if (in == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to read from SOCKET: ", strerror(errno));

	    memcpy(in + len, buf, numbytes);
	    len += numbytes;
	}
	while (numbytes == MAX_LEN);

	/* If disconnected, quit the gtk-server */
	if (numbytes == 0) exit(EXIT_SUCCESS);

	/* Make sure we have a NULL terminated character array */
	if (len > 0) in[len] = '\0';

	/* If logging is enabled, write incoming text to log */
	if (logfile != NULL){
	    fprintf(logfile, "SCRIPT: %s\n", Trim_String(in));
	    fflush(logfile);
	}
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) { scroll_to_end(debug_view, Trim_String(in), 3); }
	#endif

	retstr = Call_Realize(Trim_String(in), cinv_ctx);

	/* If logging is enabled, write returnstring to log */
	if (logfile != NULL){
	    fprintf(logfile, "SERVER: %s\n", retstr);
	    fflush(logfile);
	}
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) { scroll_to_end(debug_view, retstr, 1); }
	#endif

	/* Now send the result back to the socket */
	if (gtkserver.behave & 192){
	    #ifdef GTK_SERVER_USE_SSL
	    numbytes = SSL_write(ssl, retstr, strlen(retstr));
	    #else
	    Print_Error("%s", 1, "\nERROR: GTK-server was compiled without SSL support!");
	    #endif
	}
	else numbytes = send(sockfd, retstr, strlen(retstr), 0);

	if (numbytes < 0) Print_Error("%s%s", 2, "\nError in sending to SOCKET: ", strerror(errno));
    }
}

/**************************************************************************************** UDP */
/* Else check if this to be UDP socket - host:port */
if (gtkserver.mode == 4) {

/* Check argument format */
if (strstr(gtkserver.udp, ":") == NULL) Print_Error("%s", 1, "\nERROR: Argument is in wrong format!");
/* Find host IP address and port */
host = Trim_String(strtok(gtkserver.udp, ":"));
port = Trim_String(strtok(NULL, ":"));
sockfd = 0;
#ifdef GTK_SERVER_WIN32
/* Initialize Win32 environment for sockets */
WSADATA wsaData;
if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) Print_Error("%s", 1, "\nWSAStartup() failed!");
#endif

/* Get the host info */
if ((he=gethostbyname(host)) == NULL){
    Print_Error ("%s%d", 2, "\nSpecified host does not exist!\n\nSystem message: ", strerror(errno));
}
else if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    Print_Error("%s%s", 2, "\nCannot connect to socket!\n\nSystem message: ", strerror(errno));
}

/* Perform BIND to the host:port */
else {
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons((long)atol(port));             /* Fill in specified port */
    my_addr.sin_addr = *((struct in_addr *)he->h_addr);     /* Fill in specified IP address */
    memset(&(my_addr.sin_zero), '\0', sizeof(my_addr.sin_zero));                   /* zero the rest of the struct */

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
	Print_Error("%s%s", 2, "\nUnable to bind the specified socket address!\n\nSystem message: ", strerror(errno));
    }
}

addr_len = sizeof(struct sockaddr);

/* Try to spawn to background if argument '-detach' was entered */
#ifdef GTK_SERVER_UNIX
if (gtkserver.behave & 4) {
    i=fork();
    if (i < 0) Print_Error("%s%s", 2, "\nCould not spawn to background! ERROR: ", strerror(errno));
    if (i > 0) exit(EXIT_SUCCESS); /* parent exits */
}
#endif

/* Allocate a buffer as big as the systems UDP receive buffer. An UDP socket is not a streaming socket,
 * so we can read the data which is available only once. A repeated READ does *not* get the rest of the data. */
#ifdef GTK_SERVER_UNIX
len = sizeof(page);
getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &page, &len);
#endif
#ifdef GTK_SERVER_WIN32
len = sizeof(yes);
getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF,  (char*)&yes, &len);
#endif
in = (char*)malloc(page);

/* If logging is enabled, write buffersize to log */
if (logfile != NULL){
    fprintf(logfile, "SERVER: the UDP buffer is %d bytes big.\n", page);
    fflush(logfile);
}

/* Now accept incoming packets */
while (1){

    /* Debug window */
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
    if (gtkserver.behave & 512) {
	/* Poll descriptor to see if data is available */
	while(debug_step == 0)
	{
	    debug_step = debug_run;
	    do {
		FD_ZERO(&rfds);
		FD_SET(sockfd, &rfds);
		/* Set a time to avoid 100% CPU load */
		tv.tv_usec = 1000;
		tv.tv_sec = 0;
		/* Update the debugging GUI */
		update_gui;
	    } while (select(sockfd+1, &rfds, NULL, NULL, &tv) == 0);
	}
	if(debug_run || debug_step == 1) { debug_step = 0; }
    }
    #endif

    if ((numbytes = recvfrom(sockfd, in, page - 1, 0, (struct sockaddr *)&their_addr, &addr_len)) > 0) {

	/* Terminate incoming string */
	in[numbytes] = '\0';

	/* If logging is enabled, write incoming text to log */
	if (logfile != NULL){
	    fprintf(logfile, "SCRIPT: %s\n", Trim_String(in));
	    fflush(logfile);
	}
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) { scroll_to_end(debug_view, Trim_String(in), 3); }
	#endif

	retstr = Call_Realize(Trim_String(in), cinv_ctx);

	/* If logging is enabled, write returnstring to log */
	if (logfile != NULL){
	    fprintf(logfile, "SERVER: %s\n", retstr);
	    fflush(logfile);
	}
	#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
	if (gtkserver.behave & 512) { scroll_to_end(debug_view, retstr, 1); }
	#endif

	/* Now send the result back to the socket */
	if ((numbytes = sendto(sockfd, retstr, strlen(retstr), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1) {
	    Print_Error("%s%s", 2, "\nCould not send DATAGRAM packet.\n\nERROR: ", strerror(errno));
	}
    }
    else exit(EXIT_SUCCESS);

/* Endless while UDP */
}
/* End of the UDP protocol */
}

#ifdef GTK_SERVER_UNIX
/**************************************************************************************** IPC */
/* Else check if this is the message queue protocol */
if (gtkserver.mode == 5) {

/* Try to spawn to background if argument '-detach' was entered */
if (gtkserver.behave & 4) {
    i=fork();
    if (i < 0) Print_Error("%s%s", 2, "\nCould not spawn to background! ERROR: ", strerror(errno));
    if (i > 0) exit(EXIT_SUCCESS); /* parent exits */
}

/* Make sure the message queue is removed at exit */
if (atexit(remove_queue) != 0) {
    if (logfile != NULL){
	fprintf(logfile, "IPC: Cannot set exit function for message queue.\n");
	fflush(logfile);
    }
}

msgid = msgget(gtkserver.ipc, IPC_CREAT | 0666);
if(msgid < 0) Print_Error("%s%s", 2, "\nCould not create message queue: ", strerror(errno));

#ifdef ARG_MAX
/* If logging is enabled, write incoming text to log */
if (logfile != NULL){
    fprintf(logfile, "SERVER: Argument length on this system is limited to %d bytes.\n", ARG_MAX);
    fflush(logfile);
}
#endif

/* Set the page factor used by realloc to 0 */
page = 0;

/* Endless loop */
while (1){
    numbytes = 0;

    /* Debug window */
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
    if (gtkserver.behave & 512) {
	/* Poll message queue to see if data is available */
	while(debug_step == 0)
	{
	    debug_step = debug_run;
	    /* Update the debugging GUI */
	    update_gui;
	    /* Set a time to avoid 100% CPU load */
	    usleep(1000);
	    /* Poll the IPC */
	    if(numbytes == 0) { numbytes = msgrcv(msgid, &msgp, MAX_LEN, 1, IPC_NOWAIT|MSG_NOERROR); }

	    if(numbytes < 0 && errno == ENOMSG) { numbytes = 0; }
	}
	if(debug_run || debug_step == 1) { debug_step = 0; }
    }
    #endif

    len = 0;
    /* Now read queue */
    do {
	if(numbytes == 0) { numbytes = msgrcv(msgid, &msgp, MAX_LEN, 1, MSG_NOERROR); }

	if(numbytes < 0) Print_Error("%s%s", 2, "\nError in reading from IPC(ipc): ", strerror(errno));

	/* Realloc preserves contents, add one for \0 which is added later */
	if (len + numbytes > MAX_LEN*page) {page++; in = realloc(in, MAX_LEN*page + 1);}

	if (in == NULL) Print_Error("%s%s", 2, "\nNo sufficient memory to read from IPC(ipc): ", strerror(errno));

	memcpy(in + len, msgp.mtext, numbytes);
	len += numbytes;
	numbytes = 0;
    }
    while (numbytes == MAX_LEN);

    /* Make sure we have a NULL terminated character array */
    if (len >= 0) in[len] = '\0';

    /* If logging is enabled, write incoming text to log */
    if (logfile != NULL){
	fprintf(logfile, "SCRIPT: %s\n", Trim_String(in));
	fflush(logfile);
    }
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
    if (gtkserver.behave & 512) { scroll_to_end(debug_view, Trim_String(in), 3); }
    #endif

    retstr = Call_Realize(Trim_String(in), cinv_ctx);

    /* If logging is enabled, write returnstring to log */
    if (logfile != NULL){
	fprintf(logfile, "SERVER: %s\n", retstr);
	fflush(logfile);
    }
    #if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
    if (gtkserver.behave & 512) { scroll_to_end(debug_view, retstr, 1); }
    #endif

    /* We agree with ourselves to use msgtype = 1 */
    msgp.mtype = 1;
    memset(msgp.mtext, '\0', MAX_LEN);

    /* If the string buffer is filled, copy stringbuffer with a max of MAX_LEN */
    ipc = retstr;
    do {
	strncpy(msgp.mtext, ipc, MAX_LEN-1);
	if(msgsnd(msgid, &msgp, strlen(msgp.mtext), 0) < 0) Print_Error("%s%s", 2, "\nCould not send message to queue: ", strerror(errno));
	if (strlen(ipc) > MAX_LEN) ipc += MAX_LEN;
	else ipc = NULL;
    } while (ipc != NULL);

    /* SYNC: wait for this PID to finish writing data - if other PID, then writing data of client script occured. */
    do {
	usleep(10); /* Do not overload kernel */
	if(msgctl(msgid, IPC_STAT, msgstat) < 0) Print_Error("%s%s", 2, "\nCould not stat message queue! ERROR: ", strerror(errno));
    } while(getpid() == msgstat->msg_lspid);
}

}

/* End of IPC protocol */

#endif /* End of GTK_SERVER_UNIX def */

/* Arguments not recognized - return with error */
Print_Error("%s", 1, "\nArguments to GTK-server do not start an interface!");
return(EXIT_FAILURE);   /* <---- we never arrive here, but we must return a value as main() is not a void function */

#else
return (init_result);
#endif  /* End of ifndef LIBRARY */
}

/**************************************************************************************** GTK_SERVER_LIBRARY */
#ifdef GTK_SERVER_LIBRARY

#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
char *gtk (char *arg)
#elif GTK_SERVER_XF
char *xf (char *arg)
#elif GTK_SERVER_MOTIF
char *motif (char *arg)
#else
char *cons (char *arg)
#endif
{
char *retstr;

/* Check if Shared Object is initialized */
if (gtk_protos == NULL){
    if (init(strdup(arg))) {
	return("ok");
    }
}

/* If logging is enabled, write incoming text to log */
if (gtkserver.LogDir != NULL){
    logfile = fopen (gtkserver.LogDir, "a");
    #ifdef GTK_SERVER_UNIX
    if (logfile == NULL) {
	fprintf(stderr, "WARNING: The logfile could not be created.\n");
	gtkserver.LogDir = NULL;
    }
    #elif GTK_SERVER_WIN32
    if (logfile == NULL) {
	MessageBox(NULL, "WARNING: The logfile could not be created.\n\rGTK errors will appear in a DOS box.", "GTK-server warning", MB_OK | MB_ICONEXCLAMATION);
	gtkserver.LogDir = NULL;
    }
    #endif
}
if (logfile != NULL) {
    fprintf(logfile, "SCRIPT: %s\n", arg);
    fflush(logfile);
}

/* Debug window */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
if (gtkserver.behave & 512) {
    /* Poll GUI to see if data is available */
    while(debug_step == 0)
    {
	debug_step = debug_run;
        usleep(500);
        update_gui;
    }
    if(debug_run || debug_step == 1) { debug_step = 0; }
    scroll_to_end(debug_view, Trim_String(arg), 3);
}
#endif

retstr = Call_Realize(Trim_String(arg), cinv_ctx);

/* If logging is enabled, write returnstring to log */
if (logfile != NULL){
    fprintf(logfile, "SERVER: %s\n", retstr);
    fflush(logfile);
}

/* Close the logfile again */
if (logfile != NULL) fclose(logfile);


#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x || GTK_SERVER_XF || GTK_SERVER_MOTIF
if (gtkserver.behave & 512) { scroll_to_end(debug_view, retstr, 1); }
#endif

/* No string buffer, regular answer from GTK-server */
return (char*)retstr;
}

#ifdef GTK_SERVER_SLANG
SLANG_MODULE(gtk);

static SLang_Intrin_Fun_Type GTK_Intrinsics [] =
{
    MAKE_INTRINSIC_1("gtk", gtk, SLANG_STRING_TYPE, SLANG_STRING_TYPE),
    SLANG_END_INTRIN_FUN_TABLE
};

int init_gtk_module_ns (char *ns_name)
{
    SLang_NameSpace_Type *ns = SLns_create_namespace (ns_name);
    if (ns == NULL)
	return -1;

    if (-1 == SLns_add_intrin_fun_table (ns, GTK_Intrinsics, "__GTK__"))
	return -1;

    return 0;
}
#endif /* End of S-Lang code */

#ifdef GTK_SERVER_SCRIPTBASIC
besVERSION_NEGOTIATE
    return (int)INTERFACE_VERSION;
besEND

besSUB_START

besEND

besSUB_FINISH

besEND

besFUNCTION(_gtk)
    VARIABLE ptr;
    char* arg0;
    char* retstr;

    if(besARGNR>1) return EX_ERROR_TOO_MANY_ARGUMENTS;
    if(besARGNR<1) return EX_ERROR_TOO_FEW_ARGUMENTS;

    ptr = besARGUMENT(1);
    besDEREFERENCE(ptr);
    arg0 = besCONVERT2ZCHAR(besCONVERT2STRING(ptr),arg0);

    retstr = (char*)gtk(arg0);

    besALLOC_RETURN_STRING(strlen(retstr));
    memcpy(STRINGVALUE(besRETURNVALUE),retstr,strlen(retstr));

    besFREE(arg0);
besEND

/* Needed to compile standalone Scriptbasic programs */
SLFST VISIO_SLFST[] = {

    {"versmodu", versmodu},
    {"bootmodu", bootmodu},
    {"finimodu", finimodu},
    {"_gtk", _gtk},
    {NULL, NULL}
};

#endif /* End of ScriptBasic code */

#ifdef GTK_SERVER_KSH93
/* Export for for Kornshell93 */
#if GTK_SERVER_GTK1x || GTK_SERVER_GTK2x || GTK_SERVER_GTK3x
void b_gtk (int argc, char *argv[])
#else
void b_xf (int argc, char *argv[])
#endif
{
char *korn;

if (argc > 1) {
    korn = gtk(argv[1]);
    fprintf(stdout, "%s\n", korn);
}
else fprintf(stdout, "-1\n");
}

#endif /* End of KSH93 */

#endif /* End of library code */

/* END */
