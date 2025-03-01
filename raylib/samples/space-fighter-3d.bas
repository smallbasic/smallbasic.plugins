'
' Created by ChatGPT, updated by Claude
'

' Load raylib plugin
IMPORT raylib as rl
IMPORT raylibc as c
' Screen size
CONST SCREEN_WIDTH = 800
CONST SCREEN_HEIGHT = 600
' Initialize the game window
rl.initWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3D Space Fighter")
rl.setTargetFPS(60)
' Camera setup (First-person)
DIM camera
camera.position = [0, 2, 10]
camera.target = [0, 2, 0]
camera.up = [0, 1, 0]
camera.fovy = 75
camera.projection = c.CAMERA_PERSPECTIVE
' Player properties
LET playerX = 0
LET playerY = 0
LET playerZ = -5
CONST BASE_PLAYER_SPEED = 0.08
LET playerSpeed = BASE_PLAYER_SPEED
LET playerRotation = 0  ' Player rotation for banking effect
CONST playerSize = 1.0
LET playerHealth = 100
LET playerShieldActive = FALSE
LET playerShieldTimer = 0
LET playerShieldCooldown = 0
CONST SHIELD_DURATION = 180  ' 3 seconds at 60 FPS
CONST SHIELD_COOLDOWN = 300  ' 5 seconds at 60 FPS
' Powerup properties
CONST MAX_POWERUPS = 3
DIM powerupX[MAX_POWERUPS]
DIM powerupY[MAX_POWERUPS]
DIM powerupZ[MAX_POWERUPS]
DIM powerupType[MAX_POWERUPS]   ' 1=Speed, 2=Shield, 3=Weapon Upgrade
DIM powerupActive[MAX_POWERUPS]
' Enemy properties
CONST MAX_ENEMIES = 8
DIM enemyX[MAX_ENEMIES]
DIM enemyY[MAX_ENEMIES]
DIM enemyZ[MAX_ENEMIES]
DIM enemySize[MAX_ENEMIES]
DIM enemyType[MAX_ENEMIES]   ' 0=Regular, 1=Fast, 2=Boss
DIM enemyHealth[MAX_ENEMIES]
DIM enemySpeed[MAX_ENEMIES]
' Bullet properties
CONST MAX_BULLETS = 20
DIM bulletX[MAX_BULLETS]
DIM bulletY[MAX_BULLETS]
DIM bulletZ[MAX_BULLETS]
DIM bulletActive[MAX_BULLETS]
DIM bulletType[MAX_BULLETS]  ' 0=Regular, 1=Upgraded
LET bulletSpeed = 0.3
LET bulletIndex = 0
LET bulletCooldown = 0
LET weaponUpgraded = FALSE
LET weaponUpgradeTimer = 0
CONST WEAPON_UPGRADE_DURATION = 600  ' 10 seconds at 60 FPS
' Particle effects
CONST MAX_PARTICLES = 100
DIM particleX[MAX_PARTICLES]
DIM particleY[MAX_PARTICLES]
DIM particleZ[MAX_PARTICLES]
DIM particleSpeedX[MAX_PARTICLES]
DIM particleSpeedY[MAX_PARTICLES]
DIM particleSpeedZ[MAX_PARTICLES]
DIM particleLife[MAX_PARTICLES]
DIM particleColor[MAX_PARTICLES]
DIM particleSize[MAX_PARTICLES]
DIM particleActive[MAX_PARTICLES]
LET nextParticle = 0
' Background stars
CONST MAX_STARS = 200
DIM starX[MAX_STARS]
DIM starY[MAX_STARS]
DIM starZ[MAX_STARS]
DIM starSize[MAX_STARS]
' Score and game state
LET score = 0
LET highScore = 0
LET gameOver = FALSE
LET level = 1
LET enemiesDefeated = 0
LET levelEnemyCount = 10  ' Enemies to defeat before level increases
LET bossFight = FALSE
LET bossIndex = -1
LET gameStarted = FALSE
LET gamePaused = FALSE
' Sound (placeholders - would need actual sound implementation)
LET soundEnabled = TRUE
' Initialize stars
FOR i = 0 TO MAX_STARS - 1
  starX[i] = (RND() - 0.5) * 100
  starY[i] = (RND() - 0.5) * 100
  starZ[i] = -50 - RND() * 150
  starSize[i] = 0.1 + RND() * 0.3
NEXT
' Initialize enemies
SUB InitializeEnemies()
  FOR i = 0 TO MAX_ENEMIES - 1
    ' Determine enemy type (chance for special types increases with level)
    LET typeRoll = RND()
    IF bossFight AND i = 0 THEN
      enemyType[i] = 2  ' Boss
      enemySize[i] = 4 + (level * 0.5)
      enemyHealth[i] = 10 + (level * 5)
      enemySpeed[i] = 0.05
      bossIndex = i
    ELSEIF typeRoll > 0.8 - (level * 0.05) THEN
      enemyType[i] = 1  ' Fast enemy
      enemySize[i] = 1 + RND()
      enemyHealth[i] = 1
      enemySpeed[i] = 0.15 + (level * 0.01)
    ELSE
      enemyType[i] = 0  ' Regular enemy
      enemySize[i] = 2 + RND()
      enemyHealth[i] = 2
      enemySpeed[i] = 0.08 + (level * 0.005)
    ENDIF

    enemyX[i] = (RND() - 0.5) * 20
    enemyY[i] = (RND() - 0.5) * 10
    enemyZ[i] = -30 - RND() * 50
  NEXT
END SUB
' Initialize powerups
SUB InitializePowerups()
  FOR i = 0 TO MAX_POWERUPS - 1
    powerupActive[i] = FALSE
  NEXT
END SUB
' Create particle effect
SUB CreateExplosion(x, y, z, size, _color)
  local i
  FOR i = 0 TO 15
    LET idx = nextParticle
    particleX[idx] = x
    particleY[idx] = y
    particleZ[idx] = z
    particleSpeedX[idx] = (RND() - 0.5) * 0.3
    particleSpeedY[idx] = (RND() - 0.5) * 0.3
    particleSpeedZ[idx] = (RND() - 0.5) * 0.3
    particleLife[idx] = 30 + RND() * 30
    particleColor[idx] = _color
    particleSize[idx] = size * (0.3 + RND() * 0.3)
    particleActive[idx] = TRUE
    nextParticle = (nextParticle + 1) MOD MAX_PARTICLES
  NEXT
END SUB
' Spawn powerup with chance
SUB TrySpawnPowerup(x, y, z)
  local i
  IF RND() > 0.7 THEN  ' 30% chance to spawn a powerup
    FOR i = 0 TO MAX_POWERUPS - 1
      IF NOT powerupActive[i] THEN
        powerupX[i] = x
        powerupY[i] = y
        powerupZ[i] = z
        powerupType[i] = INT(RND() * 3) + 1  ' 1=Speed, 2=Shield, 3=Weapon
        powerupActive[i] = TRUE
        EXIT FOR
      ENDIF
    NEXT
  ENDIF
END SUB

' Get color for enemy type
FUNC GetEnemyColor(typex, health, maxHealth)
  SELECT CASE typex
    CASE 0:  ' Regular
      RETURN rl.colorToInt([255, 0, 0, 255])
    CASE 1:  ' Fast
      RETURN rl.colorToInt([255, 165, 0, 255])
    CASE 2:  ' Boss
      ' Boss changes color based on health
      LET healthPercent = health / maxHealth
      IF healthPercent > 0.7 THEN
        RETURN rl.colorToInt([128, 0, 128, 255])  ' Purple
      ELSEIF healthPercent > 0.3 THEN
        RETURN rl.colorToInt([255, 0, 128, 255])  ' Pink
      ELSE
        RETURN rl.colorToInt([255, 0, 0, 255])    ' Red
      ENDIF
  END SELECT
END

' Draw health bar above enemy
SUB DrawHealthBar(x, y, z, health, maxHealth, size)
  LET barWidth = size * 2
  LET healthPercent = health / maxHealth
  LET healthBarWidth = barWidth * healthPercent

  ' Background of health bar
  rl.drawCube([x, y + size + 0.2, z], barWidth, 0.1, 0.1, c.GRAY)

  ' Health portion
  IF healthPercent > 0 THEN
    LET healthColor = c.GREEN
    IF healthPercent < 0.3 THEN
      healthColor = c.RED
    ELSEIF healthPercent < 0.6 THEN
      healthColor = c.YELLOW
    ENDIF

    rl.drawCube([x - (barWidth - healthBarWidth)/2, y + size + 0.2, z], healthBarWidth, 0.1, 0.1, healthColor)
  ENDIF
END SUB
' Start new game
SUB StartNewGame()

  score = 0
  gameOver = FALSE
  playerX = 0
  playerY = 0
  playerZ = -5
  playerHealth = 100
  playerSpeed = BASE_PLAYER_SPEED
  playerShieldActive = FALSE
  playerShieldTimer = 0
  playerShieldCooldown = 0
  weaponUpgraded = FALSE
  weaponUpgradeTimer = 0
  level = 1
  enemiesDefeated = 0
  levelEnemyCount = 10
  bossFight = FALSE
  bossIndex = -1

  ' Reset enemies and powerups
  InitializeEnemies()
  InitializePowerups()

  ' Clear bullets and particles
  FOR i = 0 TO MAX_BULLETS - 1
    bulletActive[i] = FALSE
  NEXT

  FOR i = 0 TO MAX_PARTICLES - 1
    particleActive[i] = FALSE
  NEXT

  gameStarted = TRUE
  gamePaused = FALSE
END SUB

' Main game loop
WHILE NOT rl.windowShouldClose()
  ' Handle game state
  IF NOT gameStarted THEN
    ' Title screen
    rl.beginDrawing()
    rl.clearBackground(c.BLACK)

    ' Draw stars in the background
    FOR i = 0 TO MAX_STARS - 1
      LET starDepth = 1.0 - (starZ[i] / -200)
      LET brightness = 100 + starDepth * 155
      rl.drawCircle(SCREEN_WIDTH/2 + starX[i], SCREEN_HEIGHT/2 + starY[i], starSize[i] * starDepth, rl.colorToInt([brightness, brightness, brightness, 255]))
      starZ[i] += 0.2
      IF starZ[i] > 10 THEN
        starZ[i] = -200
        starX[i] = (RND() - 0.5) * 100
        starY[i] = (RND() - 0.5) * 100
      ENDIF
    NEXT

    rl.drawText("3D SPACE FIGHTER", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/3, 40, c.YELLOW)
    rl.drawText("Press ENTER to start", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2, 20, c.WHITE)
    rl.drawText("Arrow Keys: Move     SPACE: Shoot", SCREEN_WIDTH/2 - 170, SCREEN_HEIGHT/2 + 60, 20, c.LIGHTGRAY)
    rl.drawText("S: Shield     P: Pause", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 90, 20, c.LIGHTGRAY)

    IF highScore > 0 THEN
      rl.drawText("High Score: " + highScore, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 140, 20, c.GREEN)
    ENDIF

    rl.endDrawing()

    IF rl.isKeyPressed(c.KEY_ENTER) THEN
      StartNewGame()
    ENDIF
  ELSEIF gamePaused THEN
    ' Pause menu
    rl.beginDrawing()
    rl.clearBackground(c.BLACK)
    rl.drawText("PAUSED", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 - 30, 40, c.YELLOW)
    rl.drawText("Press P to resume", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 30, 20, c.WHITE)
    rl.endDrawing()

    IF rl.isKeyPressed(c.KEY_P) THEN
      gamePaused = FALSE
    ENDIF
  ELSEIF NOT gameOver THEN
    ' Toggle pause
    IF rl.isKeyPressed(c.KEY_P) THEN
      gamePaused = NOT gamePaused
      'CONTINUE
    ENDIF

    ' Player movement
    LET oldPlayerX = playerX
    IF rl.isKeyDown(c.KEY_LEFT) THEN
      playerX -= playerSpeed
      playerRotation = -0.3  ' Bank left
    ELSEIF rl.isKeyDown(c.KEY_RIGHT) THEN
      playerX += playerSpeed
      playerRotation = 0.3   ' Bank right
    ELSE
      playerRotation = playerRotation * 0.9  ' Return to level
    ENDIF

    IF rl.isKeyDown(c.KEY_UP) THEN playerY += playerSpeed
    IF rl.isKeyDown(c.KEY_DOWN) THEN playerY -= playerSpeed

    ' Keep player within bounds
    IF playerX < -10 THEN playerX = -10
    IF playerX > 10 THEN playerX = 10
    IF playerY < -8 THEN playerY = -8
    IF playerY > 8 THEN playerY = 8

    ' Shield activation
    IF rl.isKeyPressed(c.KEY_S) AND playerShieldCooldown <= 0 THEN
      playerShieldActive = TRUE
      playerShieldTimer = SHIELD_DURATION
      playerShieldCooldown = SHIELD_DURATION + SHIELD_COOLDOWN
    ENDIF

    ' Update shield timer
    IF playerShieldActive THEN
      playerShieldTimer -= 1
      IF playerShieldTimer <= 0 THEN
        playerShieldActive = FALSE
      ENDIF
    ENDIF

    ' Update shield cooldown
    IF playerShieldCooldown > 0 THEN
      playerShieldCooldown -= 1
    ENDIF

    ' Update weapon upgrade timer
    IF weaponUpgraded THEN
      weaponUpgradeTimer -= 1
      IF weaponUpgradeTimer <= 0 THEN
        weaponUpgraded = FALSE
      ENDIF
    ENDIF

    ' Bullet firing
    IF bulletCooldown > 0 THEN
      bulletCooldown -= 1
    ENDIF

    IF rl.isKeyDown(c.KEY_SPACE) AND bulletCooldown <= 0 THEN
      ' Regular fire
      bulletX[bulletIndex] = playerX
      bulletY[bulletIndex] = playerY
      bulletZ[bulletIndex] = playerZ
      bulletActive[bulletIndex] = TRUE
      bulletType[bulletIndex] = IFF(weaponUpgraded, 1, 0)
      bulletIndex = (bulletIndex + 1) MOD MAX_BULLETS

      ' If weapon upgraded, fire additional bullets
      IF weaponUpgraded THEN
        ' Left bullet
        bulletX[bulletIndex] = playerX - 0.5
        bulletY[bulletIndex] = playerY
        bulletZ[bulletIndex] = playerZ
        bulletActive[bulletIndex] = TRUE
        bulletType[bulletIndex] = 1
        bulletIndex = (bulletIndex + 1) MOD MAX_BULLETS

        ' Right bullet
        bulletX[bulletIndex] = playerX + 0.5
        bulletY[bulletIndex] = playerY
        bulletZ[bulletIndex] = playerZ
        bulletActive[bulletIndex] = TRUE
        bulletType[bulletIndex] = 1
        bulletIndex = (bulletIndex + 1) MOD MAX_BULLETS

        bulletCooldown = 8  ' Shorter cooldown for upgraded weapon
      ELSE
        bulletCooldown = 15
      ENDIF
    ENDIF

    ' Move bullets
    FOR i = 0 TO MAX_BULLETS - 1
      IF bulletActive[i] THEN
        bulletZ[i] -= bulletSpeed
        IF bulletZ[i] < -100 THEN bulletActive[i] = FALSE
      ENDIF
    NEXT

    ' Move powerups
    FOR i = 0 TO MAX_POWERUPS - 1
      IF powerupActive[i] THEN
        powerupZ[i] += 0.05

        ' Check if player collected powerup
        IF ABS(powerupX[i] - playerX) < 1.5 AND ABS(powerupY[i] - playerY) < 1.5 AND ABS(powerupZ[i] - playerZ) < 1.5 THEN
          SELECT CASE powerupType[i]
            CASE 1:  ' Speed boost
              playerSpeed = BASE_PLAYER_SPEED * 1.5
              ' Create speed boost timer (reset after 10 seconds)
              DELAY 600
              playerSpeed = BASE_PLAYER_SPEED

            CASE 2:  ' Shield
              playerShieldActive = TRUE
              playerShieldTimer = SHIELD_DURATION
              playerShieldCooldown = SHIELD_DURATION + SHIELD_COOLDOWN

            CASE 3:  ' Weapon upgrade
              weaponUpgraded = TRUE
              weaponUpgradeTimer = WEAPON_UPGRADE_DURATION
          END SELECT

          powerupActive[i] = FALSE
          score += 5  ' Bonus for collecting powerup
        ENDIF

        ' Remove powerup if it goes past the player
        IF powerupZ[i] > 10 THEN
          powerupActive[i] = FALSE
        ENDIF
      ENDIF
    NEXT

    ' Move and update enemies
    FOR i = 0 TO MAX_ENEMIES - 1
      ' Boss movement is different - more complex pattern
      IF enemyType[i] = 2 THEN  ' Boss
        enemyZ[i] += enemySpeed[i] * 0.5

        ' Boss moves in a figure-8 pattern
        LET timex = rl.getTime() * 0.5
        enemyX[i] = SIN(timex) * 5
        enemyY[i] = SIN(timex * 2) * 3

        ' Boss occasionally fires at player
        IF RND() < 0.02 THEN  ' 2% chance per frame
          FOR j = 0 TO 2  ' Fire 3 bullets
            TrySpawnPowerup(enemyX[i], enemyY[i], enemyZ[i])
          NEXT
        ENDIF
      ELSE
        ' Regular enemies move toward the player with some randomness
        enemyZ[i] += enemySpeed[i]

        ' Add some side-to-side movement
        IF enemyType[i] = 1 THEN  ' Fast enemies are more erratic
          enemyX[i] += SIN(rl.getTime() * 2 + i) * 0.05
          enemyY[i] += COS(rl.getTime() * 3 + i) * 0.05
        ELSE
          enemyX[i] += SIN(rl.getTime() + i) * 0.02
          enemyY[i] += COS(rl.getTime() * 1.5 + i) * 0.02
        ENDIF
      ENDIF

      ' Reset enemy when it reaches past the player
      IF enemyZ[i] > 15 THEN
        enemyX[i] = (RND() - 0.5) * 20
        enemyY[i] = (RND() - 0.5) * 10
        enemyZ[i] = -30 - RND() * 20

        ' Penalize score for missed enemies
        IF NOT bossFight OR i != bossIndex THEN
          score = MAX(0, score - 5)
        ENDIF
      ENDIF

      ' Check collision with bullets
      FOR j = 0 TO MAX_BULLETS - 1
        IF bulletActive[j] AND ABS(enemyX[i] - bulletX[j]) < enemySize[i] * 0.8 AND ABS(enemyY[i] - bulletY[j]) < enemySize[i] * 0.8 AND ABS(enemyZ[i] - bulletZ[j]) < enemySize[i] THEN
          ' Enemy hit by bullet
          bulletActive[j] = FALSE

          ' Reduce enemy health
          enemyHealth[i] -= IFF(bulletType[j] = 1, 2, 1)  ' Upgraded bullets do double damage

          ' Create hit particle effect
          CreateExplosion(bulletX[j], bulletY[j], bulletZ[j], 0.5, rl.colorToInt([255, 255, 0, 255]))

          ' Check if enemy is destroyed
          IF enemyHealth[i] <= 0 THEN
            ' Create explosion effect
            CreateExplosion(enemyX[i], enemyY[i], enemyZ[i], enemySize[i], GetEnemyColor(enemyType[i], 1, 1))

            ' Chance to spawn powerup
            TrySpawnPowerup(enemyX[i], enemyY[i], enemyZ[i])

            ' Reset enemy
            IF enemyType[i] = 2 THEN  ' Boss defeated
              bossFight = FALSE
              bossIndex = -1
              score += 50  ' Big bonus for boss
              level += 1

              ' Reset all enemies with higher difficulty
              InitializeEnemies()
            ELSE
              enemyX[i] = (RND() - 0.5) * 20
              enemyY[i] = (RND() - 0.5) * 10
              enemyZ[i] = -30 - RND() * 20

              ' Determine enemy type (chance for special types increases with level)
              LET typeRoll = RND()
              IF typeRoll > 0.8 - (level * 0.05) THEN
                enemyType[i] = 1  ' Fast enemy
                enemySize[i] = 1 + RND()
                enemyHealth[i] = 1
                enemySpeed[i] = 0.15 + (level * 0.01)
              ELSE
                enemyType[i] = 0  ' Regular enemy
                enemySize[i] = 2 + RND()
                enemyHealth[i] = 2
                enemySpeed[i] = 0.08 + (level * 0.005)
              ENDIF

              ' Update counters
              enemiesDefeated += 1

              ' Score based on enemy type
              IF enemyType[i] = 1 THEN  ' Fast enemy worth more
                score += 15
              ELSE
                score += 10
              ENDIF
            ENDIF
          ENDIF
        ENDIF
      NEXT

      ' Check collision with player
      IF NOT playerShieldActive AND ABS(enemyX[i] - playerX) < (enemySize[i] + playerSize) * 0.7 AND ABS(enemyY[i] - playerY) < (enemySize[i] + playerSize) * 0.7 AND ABS(enemyZ[i] - playerZ) < (enemySize[i] + playerSize) * 0.7 THEN
        ' Player hit by enemy
        playerHealth -= IFF(enemyType[i] = 2, 25, 10)  ' Boss does more damage

        ' Create explosion effect
        CreateExplosion(playerX, playerY, playerZ, 1.0, c.RED)

        ' Reset enemy position
        enemyX[i] = (RND() - 0.5) * 20
        enemyY[i] = (RND() - 0.5) * 10
        enemyZ[i] = -30 - RND() * 20

        ' Check if player is destroyed
        IF playerHealth <= 0 THEN
          gameOver = TRUE
          IF score > highScore THEN
            highScore = score
          ENDIF
        ENDIF
      ENDIF
    NEXT

    ' Check for level up
    IF NOT bossFight AND enemiesDefeated >= levelEnemyCount THEN
      ' Start boss fight
      bossFight = TRUE
      InitializeEnemies()  ' This will set up the boss
      enemiesDefeated = 0
      levelEnemyCount = 10 + (level * 5)  ' More enemies needed for next level
    ENDIF

    ' Update particles
    FOR i = 0 TO MAX_PARTICLES - 1
      IF particleActive[i] THEN
        particleX[i] += particleSpeedX[i]
        particleY[i] += particleSpeedY[i]
        particleZ[i] += particleSpeedZ[i]
        particleLife[i] -= 1

        IF particleLife[i] <= 0 THEN
          particleActive[i] = FALSE
        ENDIF
      ENDIF
    NEXT

    ' Update stars (moving background)
    FOR i = 0 TO MAX_STARS - 1
      starZ[i] += 0.2
      IF starZ[i] > 10 THEN
        starZ[i] = -150
        starX[i] = (RND() - 0.5) * 100
        starY[i] = (RND() - 0.5) * 100
      ENDIF
    NEXT

    ' Drawing
    rl.beginDrawing()
    rl.clearBackground(c.BLACK)

    ' Draw stars in background
    FOR i = 0 TO MAX_STARS - 1
      LET distFactor = 1.0 - (starZ[i] / -150)
      LET starScreenX = SCREEN_WIDTH/2 + (starX[i] - camera.position[0]) * distFactor * 5
      LET starScreenY = SCREEN_HEIGHT/2 + (starY[i] - camera.position[1]) * distFactor * 5

      IF starScreenX > 0 AND starScreenX < SCREEN_WIDTH AND starScreenY > 0 AND starScreenY < SCREEN_HEIGHT THEN
        LET brightness = 100 + distFactor * 155
        rl.drawCircle(starScreenX, starScreenY, starSize[i] * distFactor, rl.colorToInt([brightness, brightness, brightness, 255]))
      ENDIF
    NEXT

    rl.beginMode3D(camera)

    ' Draw particles
    FOR i = 0 TO MAX_PARTICLES - 1
      IF particleActive[i] THEN
        rl.drawSphere([particleX[i], particleY[i], particleZ[i]], particleSize[i], particleColor[i])
      ENDIF
    NEXT

    ' Draw powerups
    FOR i = 0 TO MAX_POWERUPS - 1
      IF powerupActive[i] THEN
        SELECT CASE powerupType[i]
          CASE 1:  ' Speed - blue
            rl.drawCube([powerupX[i], powerupY[i], powerupZ[i]], 1, 1, 1, c.BLUE)
          CASE 2:  ' Shield - cyan
            rl.drawSphere([powerupX[i], powerupY[i], powerupZ[i]], 0.8, c.SKYBLUE)
          CASE 3:  ' Weapon - yellow
            rl.drawCube([powerupX[i], powerupY[i], powerupZ[i]], 0.7, 0.7, 0.7, c.GOLD)
        END SELECT

        ' Add rotation effect
        rl.drawCubeWires([powerupX[i], powerupY[i], powerupZ[i]], 1.2, 1.2, 1.2, c.WHITE)
      ENDIF
    NEXT

    ' Draw player with shield if active
    IF playerShieldActive THEN
      rl.drawSphere([playerX, playerY, playerZ], playerSize * 1.5, rl.colorToInt([0, 191, 255, 100]))
    ENDIF

    ' Draw player ship with rotation
    ' Main body
    rl.drawCube([playerX, playerY, playerZ], playerSize * 0.8, playerSize * 0.4, playerSize * 2, c.GREEN)
    ' Wings
    rl.drawCube([playerX - playerSize, playerY, playerZ + playerSize * 0.5], playerSize * 0.8, playerSize * 0.1, playerSize * 0.5, c.DARKGREEN)
    rl.drawCube([playerX + playerSize, playerY, playerZ + playerSize * 0.5], playerSize * 0.8, playerSize * 0.1, playerSize * 0.5, c.DARKGREEN)

    ' Engine glow
    rl.drawSphere([playerX, playerY, playerZ + playerSize], playerSize * 0.3, rl.colorToInt([0, 100, 255, 200]))

    ' Draw bullets
    FOR i = 0 TO MAX_BULLETS - 1
      IF bulletActive[i] THEN
        IF bulletType[i] = 1 THEN
          ' Upgraded bullets
          rl.drawSphere([bulletX[i], bulletY[i], bulletZ[i]], 0.4, c.GOLD)
        ELSE
          ' Regular bullets
          rl.drawSphere([bulletX[i], bulletY[i], bulletZ[i]], 0.3, c.YELLOW)
        ENDIF
      ENDIF
    NEXT

    ' Draw enemies
    FOR i = 0 TO MAX_ENEMIES - 1
      LET enemyColor = GetEnemyColor(enemyType[i], enemyHealth[i], IFF(enemyType[i] == 2, (10 + (level * 5)), IFF(enemyType[i] == 1, 1, 2)))

      ' Different enemy types have different shapes
      SELECT CASE enemyType[i]
        CASE 0:  ' Regular enemy
          rl.drawCube([enemyX[i], enemyY[i], enemyZ[i]], enemySize[i], enemySize[i], enemySize[i], enemyColor)
        CASE 1:  ' Fast enemy - triangular
          rl.drawCube([enemyX[i], enemyY[i], enemyZ[i]], enemySize[i] * 0.6, enemySize[i] * 0.6, enemySize[i] * 1.5, enemyColor)
          ' Wings for fast enemy
          rl.drawCube([enemyX[i] - enemySize[i] * 0.7, enemyY[i], enemyZ[i]], enemySize[i] * 0.5, enemySize[i] * 0.1, enemySize[i] * 0.3, enemyColor)
          rl.drawCube([enemyX[i] + enemySize[i] * 0.7, enemyY[i], enemyZ[i]], enemySize[i] * 0.5, enemySize[i] * 0.1, enemySize[i] * 0.3, enemyColor)
        CASE 2:  ' Boss - larger with complex shape
          ' Main body
          rl.drawSphere([enemyX[i], enemyY[i], enemyZ[i]], enemySize[i] * 0.8, enemyColor)
          ' Top structure
          rl.drawCube([enemyX[i], enemyY[i] + enemySize[i] * 0.6, enemyZ[i]], enemySize[i] * 0.5, enemySize[i] * 0.4, enemySize[i] * 0.5, enemyColor)
          ' Side wings
          rl.drawCube([enemyX[i] - enemySize[i], enemyY[i], enemyZ[i]], enemySize[i] * 0.8, enemySize[i] * 0.2, enemySize[i] * 0.6, enemyColor)
          rl.drawCube([enemyX[i] + enemySize[i], enemyY[i], enemyZ[i]], enemySize[i] * 0.8, enemySize[i] * 0.2, enemySize[i] * 0.6, enemyColor)
          ' Engines
          rl.drawSphere([enemyX[i] - enemySize[i], enemyY[i], enemyZ[i] + enemySize[i] * 0.4], enemySize[i] * 0.2, rl.colorToInt([255, 100, 0, 255]))
          rl.drawSphere([enemyX[i] + enemySize[i], enemyY[i], enemyZ[i] + enemySize[i] * 0.4], enemySize[i] * 0.2, rl.colorToInt([255, 100, 0, 255]))
      END SELECT

      ' Draw health bar for enemies with more than 1 health
      IF enemyHealth[i] > 1 THEN
        DrawHealthBar(enemyX[i], enemyY[i], enemyZ[i], enemyHealth[i], IFF(enemyType[i] = 2, (10 + (level * 5)), 2), enemySize[i])
      ENDIF
    NEXT

    rl.endMode3D()

    ' Draw UI
    rl.drawText("SCORE: " + score, 10, 10, 20, c.YELLOW)
    rl.drawText("LEVEL: " + level, 10, 40, 20, c.GREEN)

    ' Draw health bar
    rl.drawRectangle(SCREEN_WIDTH - 210, 10, 200, 20, c.DARKGRAY)

    local rc_col = IFF((playerHealth > 60), c.GREEN, IFF((playerHealth > 30), c.YELLOW, c.RED))
    rl.drawRectangle(SCREEN_WIDTH - 210, 10, (playerHealth * 2), 20, rc_col)

'    rl.drawRectangle(SCREEN_WIDTH - 210, 10, (playerHealth * 2), 20, IFF((playerHealth > 60), c.GREEN, IFF((playerHealth > 30), c.YELLOW, c.RED)))


    rl.drawText("HEALTH", SCREEN_WIDTH - 210, 35, 20, c.WHITE)

    ' Draw shield status
    IF playerShieldActive THEN
      rl.drawRectangle(SCREEN_WIDTH - 210, 60, playerShieldTimer * 200 / SHIELD_DURATION, 10, c.SKYBLUE)
      rl.drawText("SHIELD ACTIVE", SCREEN_WIDTH - 210, 75, 20, c.SKYBLUE)
    ELSEIF playerShieldCooldown > 0 THEN
      rl.drawRectangle(SCREEN_WIDTH - 210, 60, 200 - (playerShieldCooldown * 200 / (SHIELD_DURATION + SHIELD_COOLDOWN)), 10, c.DARKBLUE)
      rl.drawText("SHIELD CHARGING", SCREEN_WIDTH - 210, 75, 20, c.DARKBLUE)
    ELSE
      rl.drawText("SHIELD READY (S)", SCREEN_WIDTH - 210, 75, 20, c.BLUE)
    ENDIF

    ' Draw weapon status
    IF weaponUpgraded THEN
      rl.drawRectangle(SCREEN_WIDTH - 210, 100, weaponUpgradeTimer * 200 / WEAPON_UPGRADE_DURATION, 10, c.GOLD)
      rl.drawText("WEAPON UPGRADED", SCREEN_WIDTH - 210, 115, 20, c.GOLD)
    ENDIF

    ' Draw boss info if in boss fight
    IF bossFight AND bossIndex >= 0 THEN
      rl.drawText("BOSS HEALTH:", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 40, 20, c.RED)
      rl.drawRectangle(SCREEN_WIDTH/2, SCREEN_HEIGHT - 40, 200, 20, c.DARKGRAY)
      rl.drawRectangle(SCREEN_WIDTH/2, SCREEN_HEIGHT - 40, enemyHealth[bossIndex] * 200 / (10 + (level * 5)), 20, c.RED)
    ENDIF

    rl.endDrawing()
  ELSE
    ' Game over screen
    rl.beginDrawing()
    rl.clearBackground(c.BLACK)

    ' Draw stars in the background
    FOR i = 0 TO MAX_STARS - 1
      LET starDepth = 1.0 - (starZ[i] / -200)
      LET brightness = 100 + starDepth * 155
      rl.drawCircle(SCREEN_WIDTH/2 + starX[i], SCREEN_HEIGHT/2 + starY[i], starSize[i] * starDepth, rl.colorToInt([brightness, brightness, brightness, 255]))
      starZ[i] += 0.2
      IF starZ[i] > 10 THEN
        starZ[i] = -200
        starX[i] = (RND() - 0.5) * 100
        starY[i] = (RND() - 0.5) * 100
      ENDIF
    NEXT

    rl.drawText("GAME OVER", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/3, 40, c.RED)
    rl.drawText("Final Score: " + score, SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 20, 30, c.YELLOW)
    rl.drawText("Level Reached: " + level, SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 20, 20, c.GREEN)

    IF score > highScore THEN
      rl.drawText("NEW HIGH SCORE!", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 60, 25, c.GOLD)
    ELSE
      rl.drawText("High Score: " + highScore, SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 60, 25, c.WHITE)
    ENDIF

    rl.drawText("Press R to restart", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 120, 20, c.WHITE)
    rl.drawText("Press ESC to quit", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 150, 20, c.WHITE)

    rl.endDrawing()

    ' Check for restart
    IF rl.isKeyPressed(c.KEY_R) THEN
      StartNewGame()
    ELSEIF rl.isKeyPressed(c.KEY_ESCAPE) THEN
      EXIT LOOP
    ENDIF
  ENDIF
WEND
rl.closeWindow()

