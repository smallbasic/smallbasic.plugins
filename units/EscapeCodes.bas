' SmallBASIC 12.25
' UNIT with supported escape codes for SmallBASIC for Android and SDL2
' For more information see: https://smallbasic.github.io/pages/escape.html

UNIT EscapeCodes

' Reset all terminal attributes to their defaults:
export const NORMAL             = "\e[0m"
' Supported foreground color values for characters (30m-37m):
export Const BLACK              = "\e[30m"
export Const RED                = "\e[31m"
export Const GREEN              = "\e[32m"
export Const YELLOW             = "\e[33m"
export Const BLUE               = "\e[34m"
export Const MAGENTA            = "\e[35m"
export Const CYAN               = "\e[36m"
export Const WHITE              = "\e[37m"
' Supported background color values for characters (40m-47m):
export Const BG_BLACK           = "\e[40m"
export Const BG_RED             = "\e[41m"
export Const BG_GREEN           = "\e[42m"
export Const BG_YELLOW          = "\e[43m"
export Const BG_BLUE            = "\e[44m"
export Const BG_MAGENTA         = "\e[45m"
export Const BG_CYAN            = "\e[46m"
export Const BG_WHITE           = "\e[47m"
' Supported font attributes (these returned by CAT as well):
export Const BOLD               = "\e[1m"  ' = Cat(1)
export Const BOLD_OFF           = "\e[21m" ' = Cat(-1)
export Const UNDERLINE          = "\e[4m"  ' = Cat(2)
export Const UNDERLINE_OFF      = "\e[24m" ' = Cat(-2)
export Const REVERSE            = "\e[7m"  ' = Cat(3)
export Const REVERSE_OFF        = "\e[27m" ' = Cat(-3)
export Const ITALIC             = "\e[3m"
export Const ITALIC_OFF         = "\e[23m"
' Supported cursor movment:
export const TB                 = "\t"
export const RET                = "\r"
export const NEXTLINE           = "\n"
export const SAVECURSOR         = "\e[s"
export const RESTORECURSOR      = "\e[u"
export const CLEAR_LINE         = "\e[K"
def MOVE_TO_COLUMN(n) = "\e[" + n + "G"
export MOVE_TO_COLUMN

' Others:
export const BP                 = "\a"  ' Beep
export const QUOTE              = "\""




