' Language:  SmallBASIC 12.26
' Purpose:   Standard CSS (HTML) Color Values By color names;
'            From: http://www.w3schools.com/colors/colors_groups.asp
'            (All modern browsers support the following 140 color names).
' File name: crgb.bas
' Unit name: crgb
' Version:   1.0.0  22/05/2016
' Author:    shian

' --- Start demo code ------------------------------------
'
' Import crgb as c
'
' Color 0, c.Lime: Cls
'
' Color 0, c.Tomato: ? " Standard CSS Tomato background color "
'
' Color c.Black, c.LightSteelBlue
' ? " Standard CSS LightSteelBlue background color "
'
' --- End demo code ------------------------------------

Unit crgb  ' (CSS RGB color names)

' Pink Colors ---
Export Pink, LightPink, HotPink, DeepPink, PaleVioletRed, MediumVioletRed
' Purple Colors ---
Export Lavender, Thistle, Plum, Orchid, Violet, Fuchsia, Magenta
Export MediumOrchid, DarkOrchid, DarkViolet, BlueViolet, DarkMagenta
Export Purple, MediumPurple, MediumSlateBlue, SlateBlue, DarkSlateBlue
Export RebeccaPurple, Indigo
' Red Colors ---
Export LightSalmon, Salmon, DarkSalmon, LightCoral, IndianRed
Export Crimson, Red, FireBrick, DarkRed
' Orange Colors ---
Export Orange, DarkOrange, Coral, Tomato, OrangeRed
' Yellow Colors ---
Export Gold, Yellow, LightYellow, LemonChiffon, LightGoldenRodYellow
Export PapayaWhip, Moccasin, PeachPuff, PaleGoldenRod, Khaki, DarkKhaki
' Green Colors ---
Export GreenYellow, Chartreuse, LawnGreen, Lime, LimeGreen, PaleGreen
Export LightGreen, MediumSpringGreen, SpringGreen, MediumSeaGreen
Export SeaGreen, ForestGreen, Green, DarkGreen, YellowGreen, OliveDrab
Export DarkOliveGreen, MediumAquaMarine, DarkSeaGreen, LightSeaGreen
Export DarkCyan, Teal
' Cyan Colors ---
Export Aqua, Cyan, LightCyan, PaleTurquoise, Aquamarine, Turquoise
Export MediumTurquoise, DarkTurquoise
' Blue Colors ---
Export CadetBlue, SteelBlue, LightSteelBlue, LightBlue, PowderBlue
Export LightSkyBlue, SkyBlue, CornflowerBlue, DeepSkyBlue, DodgerBlue
Export RoyalBlue, Blue, MediumBlue, DarkBlue, Navy, MidnightBlue
' Brown Colors ---
Export Cornsilk, BlanchedAlmond, Bisque, NavajoWhite, Wheat, BurlyWood
Export Tan_, RosyBrown, SandyBrown, GoldenRod, DarkGoldenRod, Peru
Export Chocolate, Olive, SaddleBrown, Sienna, Brown, Maroon
' White Colors ---
Export White, Snow, HoneyDew, MintCream, Azure, AliceBlue, GhostWhite
Export WhiteSmoke, SeaShell, Beige, OldLace, FloralWhite, Ivory
Export AntiqueWhite, Linen, LavenderBlush, MistyRose
' Grey Colors ---
Export Gainsboro, LightGray, Silver, DarkGray, DimGray, Gray
Export LightSlateGray, SlateGray, DarkSlateGray, Black

' Color Names Sorted by Color Groups:
' Pink Colors ---
Const Pink            = -16761035 ' Rgb(0xFF, 0xC0, 0xCB) ' 0xFFC0CB
Const LightPink       = -16758465 ' Rgb(0xFF, 0xB6, 0xC1) ' 0xFFB6C1
Const HotPink         = -16738740 ' Rgb(0xFF, 0x69, 0xB4) ' 0xFF69B4
Const DeepPink        = -16716947 ' Rgb(0xFF, 0x14, 0x93) ' 0xFF1493
Const PaleVioletRed   = -14381203 ' Rgb(0xDB, 0x70, 0x93) ' 0xDB7093
Const MediumVioletRed = -13047173 ' Rgb(0xC7, 0x15, 0x85) ' 0xC71585
' Purple Colors ---
Const Lavender        = -15132410 ' Rgb(0xE6, 0xE6, 0xFA) ' 0xE6E6FA
Const Thistle         = -14204888 ' Rgb(0xD8, 0xBF, 0xD8) ' 0xD8BFD8
Const Plum            = -14524637 ' Rgb(0xDD, 0xA0, 0xDD) ' 0xDDA0DD
Const Orchid          = -14315734 ' Rgb(0xDA, 0x70, 0xD6) ' 0xDA70D6
Const Violet          = -15631086 ' Rgb(0xEE, 0x82, 0xEE) ' 0xEE82EE
Const Fuchsia         = -16711935 ' Rgb(0xFF, 0x00, 0xFF) ' 0xFF00FF
Const Magenta         = -16711935 ' Rgb(0xFF, 0x00, 0xFF) ' 0xFF00FF
Const MediumOrchid    = -12211667 ' Rgb(0xBA, 0x55, 0xD3) ' 0xBA55D3
Const DarkOrchid      = -10040012 ' Rgb(0x99, 0x32, 0xCC) ' 0x9932CC
Const DarkViolet      = -9699539  ' Rgb(0x94, 0x00, 0xD3) ' 0x9400D3
Const BlueViolet      = -9055202  ' Rgb(0x8A, 0x2B, 0xE2) ' 0x8A2BE2
Const DarkMagenta     = -9109643  ' Rgb(0x8B, 0x00, 0x8B) ' 0x8B008B
Const Purple          = -8388736  ' Rgb(0x80, 0x00, 0x80) ' 0x800080
Const MediumPurple    = -9662683  ' Rgb(0x93, 0x70, 0xDB) ' 0x9370DB
Const MediumSlateBlue = -8087790  ' Rgb(0x7B, 0x68, 0xEE) ' 0x7B68EE
Const SlateBlue       = -6970061  ' Rgb(0x6A, 0x5A, 0xCD) ' 0x6A5ACD
Const DarkSlateBlue   = -4734347  ' Rgb(0x48, 0x3D, 0x8B) ' 0x483D8B
Const RebeccaPurple   = -6697881  ' Rgb(0x66, 0x33, 0x99) ' 0x663399
Const Indigo          = -4915330  ' Rgb(0x4B, 0x00, 0x82) ' 0x4B0082
' Red Colors ---
Const LightSalmon = -16752762 ' Rgb(0xFF, 0xA0, 0x7A) ' 0xFFA07A
Const Salmon      = -16416882 ' Rgb(0xFA, 0x80, 0x72) ' 0xFA8072
Const DarkSalmon  = -15308410 ' Rgb(0xE9, 0x96, 0x7A) ' 0xE9967A
Const LightCoral  = -15761536 ' Rgb(0xF0, 0x80, 0x80) ' 0xF08080
Const IndianRed   = -13458524 ' Rgb(0xCD, 0x5C, 0x5C) ' 0xCD5C5C
Const Crimson     = -14423100 ' Rgb(0xDC, 0x14, 0x3C) ' 0xDC143C
Const Red         = -16711680 ' Rgb(0xFF, 0x00, 0x00) ' 0xFF0000
Const FireBrick   = -11674146 ' Rgb(0xB2, 0x22, 0x22) ' 0xB22222
Const DarkRed     = -9109504  ' Rgb(0x8B, 0x00, 0x00) ' 0x8B0000
' Orange Colors ---
Const Orange     = -16753920 ' Rgb(0xFF, 0xA5, 0x00) ' 0xFFA500
Const DarkOrange = -16747520 ' Rgb(0xFF, 0x8C, 0x00) ' 0xFF8C00
Const Coral      = -16744272 ' Rgb(0xFF, 0x7F, 0x50) ' 0xFF7F50
Const Tomato     = -16737095 ' Rgb(0xFF, 0x63, 0x47) ' 0xFF6347
Const OrangeRed  = -16729344 ' Rgb(0xFF, 0x45, 0x00) ' 0xFF4500
' Yellow Colors ---
Const Gold                 = -16766720 ' Rgb(0xFF, 0xD7, 0x00) ' 0xFFD700
Const Yellow               = -16776960 ' Rgb(0xFF, 0xFF, 0x00) ' 0xFFFF00
Const LightYellow          = -16777184 ' Rgb(0xFF, 0xFF, 0xE0) ' 0xFFFFE0
Const LemonChiffon         = -16775885 ' Rgb(0xFF, 0xFA, 0xCD) ' 0xFFFACD
Const LightGoldenRodYellow = -16448210 ' Rgb(0xFA, 0xFA, 0xD2) ' 0xFAFAD2
Const PapayaWhip           = -16773077 ' Rgb(0xFF, 0xEF, 0xD5) ' 0xFFEFD5
Const Moccasin             = -16770229 ' Rgb(0xFF, 0xE4, 0xB5) ' 0xFFE4B5
Const PeachPuff            = -16767673 ' Rgb(0xFF, 0xDA, 0xB9) ' 0xFFDAB9
Const PaleGoldenRod        = -15657130 ' Rgb(0xEE, 0xE8, 0xAA) ' 0xEEE8AA
Const Khaki                = -15787660 ' Rgb(0xF0, 0xE6, 0x8C) ' 0xF0E68C
Const DarkKhaki            = -12433259 ' Rgb(0xBD, 0xB7, 0x6B) ' 0xBDB76B
' Green Colors ---
Const GreenYellow       = -11403055 ' Rgb(0xAD, 0xFF, 0x2F) ' 0xADFF2F
Const Chartreuse        = -8388352  ' Rgb(0x7F, 0xFF, 0x00) ' 0x7FFF00
Const LawnGreen         = -8190976  ' Rgb(0x7C, 0xFC, 0x00) ' 0x7CFC00
Const Lime              = -65280    ' Rgb(0x00, 0xFF, 0x00) ' 0x00FF00
Const LimeGreen         = -3329330  ' Rgb(0x32, 0xCD, 0x32) ' 0x32CD32
Const PaleGreen         = -10025880 ' Rgb(0x98, 0xFB, 0x98) ' 0x98FB98
Const LightGreen        = -9498256  ' Rgb(0x90, 0xEE, 0x90) ' 0x90EE90
Const MediumSpringGreen = -64154    ' Rgb(0x00, 0xFA, 0x9A) ' 0x00FA9A
Const SpringGreen       = -65407    ' Rgb(0x00, 0xFF, 0x7F) ' 0x00FF7F
Const MediumSeaGreen    = -3978097  ' Rgb(0x3C, 0xB3, 0x71) ' 0x3CB371
Const SeaGreen          = -3050327  ' Rgb(0x2E, 0x8B, 0x57) ' 0x2E8B57
Const ForestGreen       = -2263842  ' Rgb(0x22, 0x8B, 0x22) ' 0x228B22
Const Green             = -32768    ' Rgb(0x00, 0x80, 0x00) ' 0x008000
Const DarkGreen         = -25600    ' Rgb(0x00, 0x64, 0x00) ' 0x006400
Const YellowGreen       = -10145074 ' Rgb(0x9A, 0xCD, 0x32) ' 0x9ACD32
Const OliveDrab         = -7048739  ' Rgb(0x6B, 0x8E, 0x23) ' 0x6B8E23
Const DarkOliveGreen    = -5597999  ' Rgb(0x55, 0x6B, 0x2F) ' 0x556B2F
Const MediumAquaMarine  = -6737322  ' Rgb(0x66, 0xCD, 0xAA) ' 0x66CDAA
Const DarkSeaGreen      = -9419919  ' Rgb(0x8F, 0xBC, 0x8F) ' 0x8FBC8F
Const LightSeaGreen     = -2142890  ' Rgb(0x20, 0xB2, 0xAA) ' 0x20B2AA
Const DarkCyan          = -35723    ' Rgb(0x00, 0x8B, 0x8B) ' 0x008B8B
Const Teal              = -32896    ' Rgb(0x00, 0x80, 0x80) ' 0x008080
' Cyan Colors ---
Const Aqua            = -65535    ' Rgb(0x00, 0xFF, 0xFF) ' 0x00FFFF
Const Cyan            = -65535    ' Rgb(0x00, 0xFF, 0xFF) ' 0x00FFFF
Const LightCyan       = -14745599 ' Rgb(0xE0, 0xFF, 0xFF) ' 0xE0FFFF
Const PaleTurquoise   = -11529966 ' Rgb(0xAF, 0xEE, 0xEE) ' 0xAFEEEE
Const Aquamarine      = -8388564  ' Rgb(0x7F, 0xFF, 0xD4) ' 0x7FFFD4
Const Turquoise       = -4251856  ' Rgb(0x40, 0xE0, 0xD0) ' 0x40E0D0
Const MediumTurquoise = -4772300  ' Rgb(0x48, 0xD1, 0xCC) ' 0x48D1CC
Const DarkTurquoise   = -52945    ' Rgb(0x00, 0xCE, 0xD1) ' 0x00CED1
' Blue Colors ---
Const CadetBlue      = -6266528  ' Rgb(0x5F, 0x9E, 0xA0) ' 0x5F9EA0
Const SteelBlue      = -4620980  ' Rgb(0x46, 0x82, 0xB4) ' 0x4682B4
Const LightSteelBlue = -11584734 ' Rgb(0xB0, 0xC4, 0xDE) ' 0xB0C4DE
Const LightBlue      = -11393254 ' Rgb(0xAD, 0xD8, 0xE6) ' 0xADD8E6
Const PowderBlue     = -11591910 ' Rgb(0xB0, 0xE0, 0xE6) ' 0xB0E0E6
Const LightSkyBlue   = -8900346  ' Rgb(0x87, 0xCE, 0xFA) ' 0x87CEFA
Const SkyBlue        = -8900331  ' Rgb(0x87, 0xCE, 0xEB) ' 0x87CEEB
Const CornflowerBlue = -6591981  ' Rgb(0x64, 0x95, 0xED) ' 0x6495ED
Const DeepSkyBlue    = -49151    ' Rgb(0x00, 0xBF, 0xFF) ' 0x00BFFF
Const DodgerBlue     = -2003199  ' Rgb(0x1E, 0x90, 0xFF) ' 0x1E90FF
Const RoyalBlue      = -4286945  ' Rgb(0x41, 0x69, 0xE1) ' 0x4169E1
Const Blue           = -255      ' Rgb(0x00, 0x00, 0xFF) ' 0x0000FF
Const MediumBlue     = -205      ' Rgb(0x00, 0x00, 0xCD) ' 0x0000CD
Const DarkBlue       = -139      ' Rgb(0x00, 0x00, 0x8B) ' 0x00008B
Const Navy           = -128      ' Rgb(0x00, 0x00, 0x80) ' 0x000080
Const MidnightBlue   = -1644912  ' Rgb(0x19, 0x19, 0x70) ' 0x191970
' Brown Colors ---
Const Cornsilk       = -16775388 ' Rgb(0xFF, 0xF8, 0xDC) ' 0xFFF8DC
Const BlanchedAlmond = -16772045 ' Rgb(0xFF, 0xEB, 0xCD) ' 0xFFEBCD
Const Bisque         = -16770244 ' Rgb(0xFF, 0xE4, 0xC4) ' 0xFFE4C4
Const NavajoWhite    = -16768685 ' Rgb(0xFF, 0xDE, 0xAD) ' 0xFFDEAD
Const Wheat          = -16113331 ' Rgb(0xF5, 0xDE, 0xB3) ' 0xF5DEB3
Const BurlyWood      = -14596231 ' Rgb(0xDE, 0xB8, 0x87) ' 0xDEB887
Const Tan_           = -13808780 ' Rgb(0xD2, 0xB4, 0x8C) ' 0xD2B48C
Const RosyBrown      = -12357519 ' Rgb(0xBC, 0x8F, 0x8F) ' 0xBC8F8F
Const SandyBrown     = -16032864 ' Rgb(0xF4, 0xA4, 0x60) ' 0xF4A460
Const GoldenRod      = -14329120 ' Rgb(0xDA, 0xA5, 0x20) ' 0xDAA520
Const DarkGoldenRod  = -12092939 ' Rgb(0xB8, 0x86, 0x0B) ' 0xB8860B
Const Peru           = -13468991 ' Rgb(0xCD, 0x85, 0x3F) ' 0xCD853F
Const Chocolate      = -13789470 ' Rgb(0xD2, 0x69, 0x1E) ' 0xD2691E
Const Olive          = -8421376  ' Rgb(0x80, 0x80, 0x00) ' 0x808000
Const SaddleBrown    = -9127187  ' Rgb(0x8B, 0x45, 0x13) ' 0x8B4513
Const Sienna         = -10506797 ' Rgb(0xA0, 0x52, 0x2D) ' 0xA0522D
Const Brown          = -10824234 ' Rgb(0xA5, 0x2A, 0x2A) ' 0xA52A2A
Const Maroon         = -8388608  ' Rgb(0x80, 0x00, 0x00) ' 0x800000
' White Colors ---
Const White         = -16777215 ' Rgb(0xFF, 0xFF, 0xFF) ' 0xFFFFFF
Const Snow          = -16775930 ' Rgb(0xFF, 0xFA, 0xFA) ' 0xFFFAFA
Const HoneyDew      = -15794160 ' Rgb(0xF0, 0xFF, 0xF0) ' 0xF0FFF0
Const MintCream     = -16121850 ' Rgb(0xF5, 0xFF, 0xFA) ' 0xF5FFFA
Const Azure         = -15794175 ' Rgb(0xF0, 0xFF, 0xFF) ' 0xF0FFFF
Const AliceBlue     = -15792383 ' Rgb(0xF0, 0xF8, 0xFF) ' 0xF0F8FF
Const GhostWhite    = -16316671 ' Rgb(0xF8, 0xF8, 0xFF) ' 0xF8F8FF
Const WhiteSmoke    = -16119285 ' Rgb(0xF5, 0xF5, 0xF5) ' 0xF5F5F5
Const SeaShell      = -16774638 ' Rgb(0xFF, 0xF5, 0xEE) ' 0xFFF5EE
Const Beige         = -16119260 ' Rgb(0xF5, 0xF5, 0xDC) ' 0xF5F5DC
Const OldLace       = -16643558 ' Rgb(0xFD, 0xF5, 0xE6) ' 0xFDF5E6
Const FloralWhite   = -16775920 ' Rgb(0xFF, 0xFA, 0xF0) ' 0xFFFAF0
Const Ivory         = -16777200 ' Rgb(0xFF, 0xFF, 0xF0) ' 0xFFFFF0
Const AntiqueWhite  = -16444375 ' Rgb(0xFA, 0xEB, 0xD7) ' 0xFAEBD7
Const Linen         = -16445670 ' Rgb(0xFA, 0xF0, 0xE6) ' 0xFAF0E6
Const LavenderBlush = -16773365 ' Rgb(0xFF, 0xF0, 0xF5) ' 0xFFF0F5
Const MistyRose     = -16770273 ' Rgb(0xFF, 0xE4, 0xE1) ' 0xFFE4E1
' Grey Colors ---
Const Gainsboro      = -14474460 ' Rgb(0xDC, 0xDC, 0xDC) ' 0xDCDCDC
Const LightGray      = -13882323 ' Rgb(0xD3, 0xD3, 0xD3) ' 0xD3D3D3
Const Silver         = -12632256 ' Rgb(0xC0, 0xC0, 0xC0) ' 0xC0C0C0
Const DarkGray       = -11119017 ' Rgb(0xA9, 0xA9, 0xA9) ' 0xA9A9A9
Const DimGray        = -6908265  ' Rgb(0x69, 0x69, 0x69) ' 0x696969
Const Gray           = -8421504  ' Rgb(0x80, 0x80, 0x80) ' 0x808080
Const LightSlateGray = -7833753  ' Rgb(0x77, 0x88, 0x99) ' 0x778899
Const SlateGray      = -7372944  ' Rgb(0x70, 0x80, 0x90) ' 0x708090
Const DarkSlateGray  = -3100495  ' Rgb(0x2F, 0x4F, 0x4F) ' 0x2F4F4F
Const Black          = 0         ' Rgb(0x00, 0x00, 0x00) ' 0x000000 
