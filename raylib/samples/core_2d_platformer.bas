'*******************************************************************************************
'*
'*   raylib [core] example - 2d camera platformer
'*
'*   Example originally created with raylib 2.5, last time updated with raylib 3.0
'*
'*   Example contributed by arvyy (@arvyy) and reviewed by Ramon Santamaria (@raysan5)
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2019-2022 arvyy (@arvyy)
'*
'*   Converted to SmallBASIC by Joerg Siebenmorgen, 2022
'*   https://github.com/smallbasic/smallbasic.plugins/blob/master/raylib/samples
'* 
'********************************************************************************************/

import raylib as rl
import raylibc as c

' Initialization
'--------------------------------------------------------------------------------------
const G = 400
const PLAYER_JUMP_SPD = 350.0
const PLAYER_HOR_SPD = 200.0

const screenWidth = 800
const screenHeight = 450

player.position.x = 400
player.position.y = 280
player.speed = 0
player.canJump = false

dim envItems[4]
envItems[0].rect = {x: 0, y: 0, width: 1000, height: 300}
envItems[0].blocking = 0
envItems[0].color = c.LIGHTGRAY
envItems[1].rect = {x: 0, y: 400, width: 1000, height: 200}
envItems[1].blocking = 1
envItems[1].color = c.GRAY
envItems[2].rect = {x: 300, y: 200, width: 400, height: 10}
envItems[2].blocking = 1
envItems[2].color = c.GRAY
envItems[3].rect = {x: 250, y: 300, width: 100, height: 10}
envItems[3].blocking = 1
envItems[3].color = c.GRAY
envItems[4].rect = {x: 650, y: 300, width: 100, height: 10}
envItems[4].blocking = 1
envItems[4].color = c.GRAY
envItemsLength = ubound(envItems)

camera.target.x = Player.position.x
camera.target.y = Player.position.y
camera.offset.x = screenWidth/2
camera.offset.y = screenHeight/2
camera.rotation = 0.0
camera.zoom = 1.0

dim cameraDescriptions
cameraDescriptions << "Follow player center"
cameraDescriptions << "Follow player center, but clamp to map edges"
cameraDescriptions << "Follow player center; smoothed"
cameraDescriptions << "Follow player center horizontally; updateplayer center vertically after landing"
cameraDescriptions << "Player push camera on getting too close to screen edge"
cameraOption = 0
cameraUpdatersLength = 5
cameraEveningOut = false
cameraEvenOutTarget = 0

rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - 2d camera")

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    deltaTime = rl.GetFrameTime()

    UpdatePlayer(player, envItems, envItemsLength, deltaTime)
    
    camera.zoom += rl.GetMouseWheelMove() * 0.05

    if(camera.zoom > 3.0) then
        camera.zoom = 3.0
    else if(camera.zoom < 0.25) then
        camera.zoom = 0.25
    endif

    if(rl.IsKeyPressed(c.KEY_R)) then
        camera.zoom = 1.0
        player.position.x = 400
        player.position.y = 280
    endif

    if(rl.IsKeyPressed(c.KEY_C)) then
        cameraOption = (cameraOption + 1) % cameraUpdatersLength
    endif

    ' Call update camera
    select case cameraOption
        case 0
            UpdateCameraCenter(camera, player, screenWidth, screenHeight)
        case 1
            UpdateCameraCenterInsideMap(camera, player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight)
        case 2
            UpdateCameraCenterSmoothFollow(camera, player, deltaTime, screenWidth, screenHeight)
        case 3
            UpdateCameraEvenOutOnLanding(camera, player, deltaTime, screenWidth, screenHeight)
        case 4
            UpdateCameraPlayerBoundsPush(camera, player, screenWidth, screenHeight)
    end select
            
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.LIGHTGRAY)

        rl.BeginMode2D(camera);

            for i = 0 to envItemsLength
                rl.DrawRectangleRec([envItems[i].rect.x, envItems[i].rect.y, envItems[i].rect.width, envItems[i].rect.height], envItems[i].color)
            next

            rl.DrawRectangleRec([player.position.x - 20, player.position.y - 40, 40, 40], c.RED)

        rl.EndMode2D()

        rl.DrawText("Controls:", 20, 20, 10, c.BLACK)
        rl.DrawText("- Right/Left to move", 40, 40, 10, c.DARKGRAY)
        rl.DrawText("- Space to jump", 40, 60, 10, c.DARKGRAY)
        rl.DrawText("- Mouse Wheel to Zoom in-out, R to reset zoom", 40, 80, 10, c.DARKGRAY)
        rl.DrawText("- C to change camera mode", 40, 100, 10, c.DARKGRAY)
        rl.DrawText("Current camera mode:", 20, 120, 10, c.BLACK)
        rl.DrawText(cameraDescriptions[cameraOption], 40, 140, 10, c.DARKGRAY)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 


sub UpdatePlayer(byref player, byref envItems, envItemsLength, delta)
    local hitObstacle, i, ei, p    
    
    if(rl.IsKeyDown(c.KEY_LEFT)) then
        player.position.x -= PLAYER_HOR_SPD * delta
    endif
    if(rl.IsKeyDown(c.KEY_RIGHT)) then
        player.position.x += PLAYER_HOR_SPD * delta
    endif
    if(rl.IsKeyDown(c.KEY_SPACE) AND player.canJump) then
        player.speed = -PLAYER_JUMP_SPD
        player.canJump = false
    endif

    hitObstacle = 0
    for i = 0 to envItemsLength
        ei = envItems[i]
        p = player.position
        if(ei.blocking AND &
           ei.rect.x <= p.x AND &
           ei.rect.x + ei.rect.width >= p.x AND &
           ei.rect.y >= p.y AND &
           ei.rect.y < p.y + player.speed * delta) &
        then
            hitObstacle = 1
            player.speed = 0
            p.y = ei.rect.y
        endif
    next

    if(!hitObstacle) then
        player.position.y += player.speed * delta
        player.speed += G * delta
        player.canJump = false
    else 
        player.canJump = true
    endif
end


sub UpdateCameraCenter(byref camera, byref player, width, height)
    camera.offset.x = width/2.0
    camera.offset.y = height/2.0
    camera.target = player.position
end

sub UpdateCameraCenterInsideMap(byref camera, byref player, byref envItems, byref envItemsLength, delta, width, height)
    local minX, minY, maxX, maxY, ei, maxV, minV, tempV
    
    camera.target = player.position
    camera.offset.x = width/2
    camera.offset.y = height/2
    minX = 1000 : minY = 1000 : maxX = -1000 : maxY = -1000

    for i = 0 to envItemsLength
        ei = envItems[i]
        minX = min(ei.rect.x, minX)
        maxX = max(ei.rect.x + ei.rect.width, maxX)
        minY = min(ei.rect.y, minY)
        maxY = max(ei.rect.y + ei.rect.height, maxY)
    next

    tempV.x = maxX
    tempV.y = maxY
    maxV = rl.GetWorldToScreen2D(tempV, camera)
    tempV.x = minX
    tempV.y = minY
    minV = rl.GetWorldToScreen2D(tempV, camera)

    if(maxV.x < width) then
        camera.offset.x = width - (maxV.x - width/2)
    endif
    if(maxV.y < height) then
        camera.offset.y = height - (maxV.y - height/2)
    endif
    if(minV.x > 0) then
        camera.offset.x = width/2 - minV.x
    endif
    if(minV.y > 0) then
        camera.offset.y = height/2 - minV.y
    endif
end

sub UpdateCameraCenterSmoothFollow(byref camera, byref player, delta, width, height)
    local minSpeed, minEffectLength, fractionSpeed, diff, lengthF, speed
    
    dim diff
    
    const minSpeed = 30
    const minEffectLength = 10
    const fractionSpeed = 0.8

    camera.offset.x = width/2.0
    camera.offset.y = height/2.0
    diff.x = player.position.x - camera.target.x
    diff.y = player.position.y - camera.target.y
    lengthF = sqr(diff.x^2 + diff.y^2)

    if(lengthF > minEffectLength) then
        speed = max(fractionSpeed * lengthF, minSpeed)
        camera.target.x = camera.target.x + diff.x * speed*delta/lengthF
        camera.target.y = camera.target.y + diff.y * speed*delta/lengthF
    endif
end

sub UpdateCameraEvenOutOnLanding(byref camera, byref player, delta, width, height)
    local evenOutSpeed
   
    const evenOutSpeed = 700
    
    camera.offset.x = width/2
    camera.offset.y = height/2
    camera.target.x = player.position.x

    if(cameraEveningOut) then
        if(cameraEvenOutTarget > camera.target.y) then
            camera.target.y += evenOutSpeed * delta

            if(camera.target.y > cameraEvenOutTarget) then
                camera.target.y = cameraEvenOutTarget
                cameraEveningOut = false
            endif
        else
            camera.target.y -= evenOutSpeed * delta

            if(camera.target.y < cameraEvenOutTarget) then
                camera.target.y = cameraEvenOutTarget
                cameraEveningOut = false
            endif
        endif
    else
        if(player.canJump AND player.speed == 0 AND player.position.y != camera.target.y) then
            print 11111
            cameraEveningOut = true
            cameraEvenOutTarget = player.position.y
        endif
    endif
end

sub UpdateCameraPlayerBoundsPush(byref camera,byref player, width, height)
    local bbox, bboxWorldMin, bboxWorldMax, tempV
    
    const bbox = {x: 0.2, y: 0.2}

    tempV.x = (1 - bbox.x) * 0.5 * width
    tempV.y = (1 - bbox.y) * 0.5 * height
    bboxWorldMin = rl.GetScreenToWorld2D(tempV, camera)
    tempV.x = (1 + bbox.x) * 0.5 * width
    tempV.y = (1 + bbox.y) * 0.5 * height
    bboxWorldMax = rl.GetScreenToWorld2D(tempV, camera)
    camera.offset.x = (1 - bbox.x) * 0.5 * width
    camera.offset.y = (1 - bbox.y) * 0.5 * height

    if(player.position.x < bboxWorldMin.x) then
        camera.target.x = player.position.x
    endif
    if(player.position.y < bboxWorldMin.y) then
        camera.target.y = player.position.y
    endif
    if(player.position.x > bboxWorldMax.x) then
        camera.target.x = bboxWorldMin.x + (player.position.x - bboxWorldMax.x)
    endif
    if(player.position.y > bboxWorldMax.y) then
        camera.target.y = bboxWorldMin.y + (player.position.y - bboxWorldMax.y)
    endif
end
