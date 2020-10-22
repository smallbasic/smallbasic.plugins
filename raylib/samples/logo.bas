import raylib

unit logo
export Init, Update, DrawScreen, Unload, Finish

rem Logo screen global variables
framesCounter = 0
finishScreen = 0
const msgLogoA = "A simple and easy-to-use library"
const msgLogoB = "to enjoy videogames programming"
logoPositionX = 0
logoPositionY = 0
raylibLettersCount = 0
topSideRecWidth = 16
leftSideRecHeight = 16
bottomSideRecWidth = 16
rightSideRecHeight = 16
logoScreenState = 37
msgLogoADone = false
msgLogoBDone = false
lettersCounter = 0
# msgBuffer = { ' ' }

const BLACK = rgb(45,45,45)
const RAYWHITE = rgb(90,90,90)
const GRAY = rgb(23,23,23)

# Logo Screen Initialization logic
sub Init
  # Initialize LOGO screen variables here!
  framesCounter = 0
  finishScreen = 0
  logoPositionX = raylib.GetScreenWidth() / 2 - 128
  logoPositionY = raylib.GetScreenHeight() / 2 - 128
end

# Logo Screen Update logic
sub Update
  # Update LOGO screen
  framesCounter++    # Count frames
  print framesCounter
  # Update LOGO screen variables
  if (logoScreenState == 0) then
    # State 0: Small box blinking
    framesCounter++
    if (framesCounter == 120) then
      logoScreenState = 1
      # Reset counter... will be used later...      
      framesCounter = 0
    endif
  else if (logoScreenState == 1) then
    # State 1: Top and left bars growing
    topSideRecWidth += 4
    leftSideRecHeight += 4
    if (topSideRecWidth == 256) then logoScreenState = 2
  else if (logoScreenState == 2) then
    # State 2: Bottom and right bars growing
    bottomSideRecWidth += 4
    rightSideRecHeight += 4
    if (bottomSideRecWidth == 256) then
      lettersCounter = 0
      # for (int i = 0; i < strlen(msgBuffer); i++) msgBuffer[i] = ' '
      logoScreenState = 3
    endif
  else if (logoScreenState == 3) then
    # State 3: Letters appearing (one by one)
    framesCounter++
    # Every 12 frames, one more letter!
    if ((framesCounter%12) == 0) then raylibLettersCount++
    rem select case (raylibLettersCount)
    rem case 1: raylib[0] = 'r'; break
    rem case 2: raylib[1] = 'a'; break
    rem case 3: raylib[2] = 'y'; break
    rem case 4: raylib[3] = 'l'; break
    rem case 5: raylib[4] = 'i'; break
    rem case 6: raylib[5] = 'b'; break
    rem end select
    if (raylibLettersCount >= 10) then
      # Write raylib description messages
      if ((framesCounter%2) == 0) then lettersCounter++
      if (!msgLogoADone) then
        rem if (lettersCounter <= strlen(msgLogoA)) then strncpy(msgBuffer, msgLogoA, lettersCounter)
      else
        rem # for (int i = 0; i < strlen(msgBuffer); i++) msgBuffer[i] = ' '
        lettersCounter = 0
        msgLogoADone = true
      endif
    else if (!msgLogoBDone) then
      rem   if (lettersCounter <= strlen(msgLogoB)) strncpy(msgBuffer, msgLogoB, lettersCounter)
    else
      msgLogoBDone = true
      framesCounter = 0
      raylib.PlaySound(levelWin)
    endif
  endif

  # Wait for 2 seconds (60 frames) before jumping to TITLE screen
  if (msgLogoBDone) then
    framesCounter++
    if (framesCounter > 90) then finishScreen = true
  endif
end

# Logo Screen Draw logic
sub DrawScreen
  # Draw LOGO screen
  if (logoScreenState == 0) then
    if ((framesCounter/15)%2) then raylib.DrawRectangle(logoPositionX, logoPositionY - 60, 16, 16, BLACK)
  else if (logoScreenState == 1) then
    raylib.DrawRectangle(logoPositionX, logoPositionY - 60, topSideRecWidth, 16, BLACK)
    raylib.DrawRectangle(logoPositionX, logoPositionY - 60, 16, leftSideRecHeight, BLACK)
  else if (logoScreenState == 2) then
    raylib.DrawRectangle(logoPositionX, logoPositionY - 60, topSideRecWidth, 16, BLACK)
    raylib.DrawRectangle(logoPositionX, logoPositionY - 60, 16, leftSideRecHeight, BLACK)
    raylib.DrawRectangle(logoPositionX + 240, logoPositionY - 60, 16, rightSideRecHeight, BLACK)
    raylib.DrawRectangle(logoPositionX, logoPositionY + 240 - 60, bottomSideRecWidth, 16, BLACK)
  else if (logoScreenState == 3) then
    raylib.DrawRectangle(logoPositionX, logoPositionY - 60, topSideRecWidth, 16, BLACK)
    raylib.DrawRectangle(logoPositionX, logoPositionY + 16 - 60, 16, leftSideRecHeight - 32, BLACK)
    raylib.DrawRectangle(logoPositionX + 240, logoPositionY + 16 - 60, 16, rightSideRecHeight - 32, BLACK)
    raylib.DrawRectangle(logoPositionX, logoPositionY + 240 - 60, bottomSideRecWidth, 16, BLACK)
    raylib.DrawRectangle(raylib.GetScreenWidth()/2 - 112, raylib.GetScreenHeight()/2 - 112 - 60, 224, 224, RAYWHITE)
    raylib.DrawText(raylib, raylib.GetScreenWidth()/2 - 44, raylib.GetScreenHeight()/2 + 48 - 60, 50, BLACK)
    if (!msgLogoADone) then raylib.DrawText(msgBuffer, raylib.GetScreenWidth()/2 - raylib.MeasureText(msgLogoA, 30)/2, logoPositionY + 230, 30, GRAY)
  else
    raylib.DrawText(msgLogoA, GetScreenWidth()/2 - raylib.MeasureText(msgLogoA, 30)/2, logoPositionY + 230, 30, GRAY)
    if (!msgLogoBDone) then
      raylib.DrawText(msgBuffer, raylib.GetScreenWidth()/2 - raylib.MeasureText(msgLogoB, 30)/2, logoPositionY + 280, 30, GRAY)
    else
      raylib.DrawText(msgLogoB, raylib.GetScreenWidth()/2 - raylib.MeasureText(msgLogoA, 30)/2, logoPositionY + 280, 30, GRAY)
    endif 
  endif
end
  
# Logo Screen Unload logic
sub Unload
  # TODO: Unload LOGO screen variables here!
end

# Logo Screen should finish?
func Finish
  return finishScreen
end
