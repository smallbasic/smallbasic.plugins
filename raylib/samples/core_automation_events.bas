REM /*******************************************************************************************
REM *
REM *   raylib [core] example - automation events
REM *
REM *   Example originally created with raylib 5.0, last time updated with raylib 5.0
REM *
REM *   Example based on 2d_camera_platformer example by arvyy (@arvyy)
REM *
REM *   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
REM *   BSD-like license that allows static linking with closed source software
REM *
REM *   Copyright (c) 2023 Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const GRAVITY=400
const PLAYER_JUMP_SPD=350.0
const PLAYER_HOR_SPD=200.0
const MAX_ENVIRONMENT_ELEMENTS=5
const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - automation events")

sub reset_player
  player = {}
  player.position.x = 400
  player.position.y = 280
  player.speed = 0
  player.canJump = false
end

sub reset_camera
  camera.target = player.position
  camera.offset.x = screenWidth / 2
  camera.offset.y = screenHeight / 2
  camera.rotation = 0.0
  camera.zoom = 1.0
end

rem Define environment elements (platforms)
dim envElements(MAX_ENVIRONMENT_ELEMENTS)
envElements[0]._rect={x:0, y:0, width:1000, height:400}
envElements[0]._blocking=false
envElements[0]._color=c.LIGHTGRAY
envElements[1]._rect={x:0, y:400, width:1000, height:200}
envElements[1]._blocking=true
envElements[1]._color=c.GRAY
envElements[2]._rect={x:300, y:200, width:400, height:10}
envElements[2]._blocking=true
envElements[2]._color=c.GRAY
envElements[3]._rect={x:250, y:300, width:100, height:10}
envElements[3]._blocking=true
envElements[3]._color=c.GRAY
envElements[4]._rect={x:650 y:300, width:100, height:10}
envElements[4]._blocking=true
envElements[4]._color=c.GRAY

reset_player()
reset_camera()

rem Automation events
aelist = rl.LoadAutomationEventList(0)  ' list of automation events to record new events
rl.SetAutomationEventList(aelist)

eventRecording = false
eventPlaying = false
frameCounter = 0
playFrameCounter = 0
currentPlayFrame = 0
deltaTime = 0.015

rl.SetTargetFPS(60)
while (!rl.WindowShouldClose())
  rem Dropped files logic
  if (rl.IsFileDropped()) then
    droppedFiles = rl.LoadDroppedFiles()

    rem Supports loading .rgs style files (text or binary) and .png style palette images
    if (rl.IsFileExtension(droppedFiles.paths[0], ".txt.rae")) then
      rl.UnloadAutomationEventList(aelist)
      aelist = rl.LoadAutomationEventList(droppedFiles.paths[0])

      eventRecording = false

      rem Reset scene state to play
      eventPlaying = true
      playFrameCounter = 0
      currentPlayFrame = 0
      
      reset_player()
      reset_camera()
    endif

    rl.UnloadDroppedFiles(droppedFiles)   ' Unload filepaths from memory
  endif

  REM Update player
  if (rl.IsKeyDown(c.KEY_LEFT)) then player.position.x -= PLAYER_HOR_SPD * deltaTime
  if (rl.IsKeyDown(c.KEY_RIGHT)) then player.position.x += PLAYER_HOR_SPD * deltaTime
  if (rl.IsKeyDown(c.KEY_SPACE) && player.canJump) then
    player.speed = -PLAYER_JUMP_SPD
    player.canJump = false
  endif

  hitObstacle = false
  for i = 0 to MAX_ENVIRONMENT_ELEMENTS - 1
    element = envElements[i]
    local p = player.position
    if (element._blocking AND &
       element._rect.x <= p.x AND &
       element._rect.x + element._rect.width >= p.x AND &
       element._rect.y >= p.y AND &
       element._rect.y <= p.y + player.speed * deltaTime) then
      hitObstacle = true
      player.speed = 0.0
      p.y = element._rect.y
    endif
  next

  if (!hitObstacle) then
    player.position.y += player.speed * deltaTime
    player.speed += GRAVITY * deltaTime
    player.canJump = false
  else
    player.canJump = true
  endif

  camera.zoom += rl.GetMouseWheelMove() * 0.05

  if (camera.zoom > 3.0) then
    camera.zoom = 3.0
  else if (camera.zoom < 0.25) then
    camera.zoom = 0.25
  endif

  if (rl.IsKeyPressed(c.KEY_R)) then
    REM Reset game state
    player.position.x = 400
    player.position.y = 280
    player.speed = 0
    player.canJump = false
    camera.target = player.position
    camera.offset.x = screenWidth/2.0
    camera.offset.y = screenHeight/2.0
    camera.rotation = 0.0
    camera.zoom = 1.0
  endif

  REM Update camera
  camera.target = player.position
  camera.offset.x = screenWidth/2.0
  camera.offset.y = screenHeight/2.0
  minX = 1000
  minY = 1000
  maxX = -1000
  maxY = -1000

  for i = 0 to MAX_ENVIRONMENT_ELEMENTS - 1
    minX = min(element._rect.x, minX)
    maxX = max(element._rect.x + element._rect.width, maxX)
    minY = min(element._rect.y, minY)
    maxY = max(element._rect.y + element._rect.height, maxY)
  next

  _max = rl.GetWorldToScreen2D([maxX, maxY], camera)
  _min = rl.GetWorldToScreen2D([minX, minY], camera)

'  if (_max.x < screenWidth) then camera.offset.x = screenWidth - (_max.x - screenWidth/2)
'  if (_max.y < screenHeight) then camera.offset.y = screenHeight - (_max.y - screenHeight/2)
'  if (_min.x > 0) then camera.offset.x = screenWidth/2 - _min.x
'  if (_min.y > 0) then camera.offset.y = screenHeight/2 - _min.y

  REM Toggle events recording
  if (rl.IsKeyPressed(c.KEY_F2)) then
    if (!eventPlaying) then
      if (eventRecording) then
        rl.StopAutomationEventRecording()
        eventRecording = false
        n = rl.ExportAutomationEventList(aelist, "automation.rae")
      else
        rl.SetAutomationEventBaseFrame(180)
        rl.StartAutomationEventRecording()
        eventRecording = true
      endif
    endif
  else if (rl.IsKeyPressed(c.KEY_F3)) then
    rem refresh aelist variable to contain current entries
    aelist = rl.UpdateAutomationEventList(aelist)
    if (!eventRecording && (aelist.count > 0)) then
      REM Reset scene state to play
      eventPlaying = true
      playFrameCounter = 0
      currentPlayFrame = 0
      reset_player()
      reset_camera()
    endif
  endif
  if (eventPlaying) then
    REM NOTE: Multiple events could be executed in a single frame
    while (playFrameCounter == aelist.events[currentPlayFrame].frame)
      rl.PlayAutomationEvent(aelist.events[currentPlayFrame])
      currentPlayFrame++
      if (currentPlayFrame == aelist.count) then
        eventPlaying = false
        currentPlayFrame = 0
        playFrameCounter = 0
        exit loop
      endif
    wend
    playFrameCounter++
  endif

  if (eventRecording || eventPlaying) then
    frameCounter++
  else
    frameCounter = 0
  endif

  rl.BeginDrawing()
  rl.ClearBackground(c.LIGHTGRAY)
  rl.BeginMode2D(camera)

  REM Draw environment elements
  for i = 0 to MAX_ENVIRONMENT_ELEMENTS - 1
    rl.DrawRectangleRec(envElements[i]._rect, envElements[i]._color)
  next

  REM Draw player rectangle
  rl.DrawRectangleRec([player.position.x - 20, player.position.y - 40, 40, 40], c.RED)
  rl.EndMode2D()

  REM Draw game controls
  rl.DrawRectangle(10, 10, 290, 145, rl.fade(c.SKYBLUE, 0.5))
  rl.DrawRectangleLines(10, 10, 290, 145, rl.fade(c.BLUE, 0.8))
  rl.DrawText("Controls:", 20, 20, 10, c.BLACK)
  rl.DrawText("- RIGHT | LEFT: Player movement", 30, 40, 10, c.DARKGRAY)
  rl.DrawText("- SPACE: Player jump", 30, 60, 10, c.DARKGRAY)
  rl.DrawText("- R: Reset game state", 30, 80, 10, c.DARKGRAY)
  rl.DrawText("- F2: START/STOP RECORDING INPUT EVENTS", 30, 110, 10, c.BLACK)
  rl.DrawText("- F3: REPLAY LAST RECORDED INPUT EVENTS", 30, 130, 10, c.BLACK)

  REM Draw automation events recording indicator
  if (eventRecording) then
    rl.DrawRectangle(10, 160, 290, 30, rl.fade(c.RED, 0.3))
    rl.DrawRectangleLines(10, 160, 290, 30, rl.fade(c.MAROON, 0.8))
    rl.DrawCircle(30, 175, 10, c.MAROON)
    if (((frameCounter/15)%2) == 1) then
      rl.DrawText(rl.TextFormat("RECORDING EVENTS... [%i]", aelist.count), 50, 170, 10, c.MAROON)
    else if (eventPlaying) then
      rl.DrawRectangle(10, 160, 290, 30, rl.fade(c.LIME, 0.3))
      rl.DrawRectangleLines(10, 160, 290, 30, rl.fade(c.DARKGREEN, 0.8))
      rl.DrawTriangle([20, 155 + 10], [20, 155 + 30], [40, 155 + 20], c.DARKGREEN)
      if (((frameCounter/15)%2) == 1) then
        rl.DrawText(rl.TextFormat("PLAYING RECORDED EVENTS... [%i]", currentPlayFrame), 50, 170, 10, c.DARKGREEN)
      endif
    endif
  endif
  rl.EndDrawing()
wend

rl.CloseWindow()
