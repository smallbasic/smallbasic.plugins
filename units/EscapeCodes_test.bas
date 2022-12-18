' SmallBASIC 12.25
' Example for using UNIT "EscapeCodes"
' For more information see: https://smallbasic.github.io/pages/escape.html

import EscapeCodes as esc

print "FORMATING TEXT:"
print
print esc.NORMAL + "WITHOUT ANY FORMAT " + esc.ITALIC + "ITALIC " + esc.ITALIC_OFF + esc.BOLD + "BOLD " + esc.BOLD_OFF + esc.UNDERLINE + "UNDERLINE " + esc.UNDERLINE_OFF + esc.REVERSE + "REVERSE" + esc.REVERSE_OFF
print
print "USE COLORS:"
print
print esc.BG_BLACK   + esc.BLACK + "  BLACK  " + esc.RED + "   RED   " + esc.GREEN + "  GREEN  " + esc.YELLOW + " YELLOW " + esc.BLUE + "  BLUE    " + esc.MAGENTA + " MAGENTA " + esc.CYAN + "  CYAN  " + esc.WHITE + "  WHITE  " + esc.NORMAL
print esc.BG_RED     + esc.BLACK + "  BLACK  " + esc.RED + "   RED   " + esc.GREEN + "  GREEN  " + esc.YELLOW + " YELLOW " + esc.BLUE + "  BLUE    " + esc.MAGENTA + " MAGENTA " + esc.CYAN + "  CYAN  " + esc.WHITE + "  WHITE  " + esc.NORMAL
print esc.BG_GREEN   + esc.BLACK + "  BLACK  " + esc.RED + "   RED   " + esc.GREEN + "  GREEN  " + esc.YELLOW + " YELLOW " + esc.BLUE + "  BLUE    " + esc.MAGENTA + " MAGENTA " + esc.CYAN + "  CYAN  " + esc.WHITE + "  WHITE  " + esc.NORMAL
print esc.BG_YELLOW  + esc.BLACK + "  BLACK  " + esc.RED + "   RED   " + esc.GREEN + "  GREEN  " + esc.YELLOW + " YELLOW " + esc.BLUE + "  BLUE    " + esc.MAGENTA + " MAGENTA " + esc.CYAN + "  CYAN  " + esc.WHITE + "  WHITE  " + esc.NORMAL
print esc.BG_BLUE    + esc.BLACK + "  BLACK  " + esc.RED + "   RED   " + esc.GREEN + "  GREEN  " + esc.YELLOW + " YELLOW " + esc.BLUE + "  BLUE    " + esc.MAGENTA + " MAGENTA " + esc.CYAN + "  CYAN  " + esc.WHITE + "  WHITE  " + esc.NORMAL
print esc.BG_MAGENTA + esc.BLACK + "  BLACK  " + esc.RED + "   RED   " + esc.GREEN + "  GREEN  " + esc.YELLOW + " YELLOW " + esc.BLUE + "  BLUE    " + esc.MAGENTA + " MAGENTA " + esc.CYAN + "  CYAN  " + esc.WHITE + "  WHITE  " + esc.NORMAL
print esc.BG_CYAN    + esc.BLACK + "  BLACK  " + esc.RED + "   RED   " + esc.GREEN + "  GREEN  " + esc.YELLOW + " YELLOW " + esc.BLUE + "  BLUE    " + esc.MAGENTA + " MAGENTA " + esc.CYAN + "  CYAN  " + esc.WHITE + "  WHITE  " + esc.NORMAL
print esc.BG_WHITE   + esc.BLACK + "  BLACK  " + esc.RED + "   RED   " + esc.GREEN + "  GREEN  " + esc.YELLOW + " YELLOW " + esc.BLUE + "  BLUE    " + esc.MAGENTA + " MAGENTA " + esc.CYAN + "  CYAN  " + esc.WHITE + "  WHITE  " + esc.NORMAL
print esc.NORMAL
print "USE COLORS AND FORMATS:"
print
print esc.NORMAL + esc.BOLD + esc.UNDERLINE + esc.GREEN + esc.BG_WHITE + "BOLD + UNDELINE + COLOR" + esc.NORMAL
print
print "CONTROL THE CURSOR:"
print
print esc.MOVE_TO_COLUMN(4) + "MOVE TO COLUMN 5"
print "TABS:" + esc.TB + "ONE TAB" + esc.TB + esc.TB + "TWO MORE TABS"
print "YOU SHOULD NOT READ THIS" + esc.RET + "RETURN TO BEGIN OF LINE "
print "FIRST LINE" + esc.NEXTLINE + "NEXT LINE"
print esc.SAVECURSOR + "YOU SHOULD NOT READ THIS" 
print esc.RESTORECURSOR + esc.CLEAR_LINE + "SAVE AND RESTORE THE CURSOR POSITION"
print
print "OTHER:"
print
print esc.QUOTE + "YOU CAN USE QUOTES" + esc.QUOTE
print esc.BP + "A BEEP SHOULD BE AUDIBLE"



