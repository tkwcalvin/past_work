#Name: TANG, Kewei
#Email: ktangaj@connect.ust.hk


.data
# Each single-value variable is stored as a word

#--------------------------------------------------------------------
# An array of 4 obstacles, each obstacle has 5 variables: 
# topPipe, bottomPipe, isPassedOn, topPipeType, bottomPipeType
# Each top/bottom pipe is a Rectangle, it has 4 single-valued variables: x, y, width, height
# isPassedOn: 0 if the flappy bird has not passed the obstacle, 1 otherwise
# topPipeType/bottomPipeType: 0, 1, 2 (three different types of pipes)
#--------------------------------------------------------------------
obstacles: .word 0:44
#--------------------------------------------------------------------
# Rectangle: x, y, width, height
# x, y: top-left corner coordinate of the rectangle
# width, height: width and height of the rectangle
# each box is a Rectangle
#--------------------------------------------------------------------
backgroundBox: .word 0:4    
floorBox: .word 0:4    
flappyBox: .word 0:4    

birdIdx: .word 0 # the index of the bird image
record: .word 0 # the highest score
point: .word 0 # the current score
objectVelocity: .word 6 # the velocity of the obstacles
winningScore: .word 20 # the score to win the game
won: .word -1 # 1 if the game is won, 0 otherwise, -1 if the game is not started
inGame: .word 0 # 1 if the game is in progress, 0 otherwise and display the start screen
direction: .word -1 # 1 if the bird is controlled by mouse (mouse pressed), 0 if the bird is controlled by gravity (mouse released), -1 if the game is not started
intervalFrame: .word 5 # the interval of the bird animation frame, the bird image changes every 5 frames (for flappying wings effect)
velocity: .word 0 # the vertical velocity of the flappy bird
gravity: .word 1 # the gravity of the world
# Constants
SIZE: .word 1024 # the size of the game window
obstacleDistance: .word 400 # the vertical distance between two pipes in the same obstacle


.text


# syscall initializeGUI();
addi $v0, $zero, 300
syscall

#--------------------------------------------------------------------
# procedure: initialize data for the game
#--------------------------------------------------------------------
initializeData:

    la $t0, SIZE
    lw $s0, 0($t0) # $s0: SIZE

    # initialize position of the background
    la $s1, backgroundBox
    sw $zero, 0($s1)
    sw $zero, 4($s1)
    addi $t0, $zero, 1716
    sw $t0, 8($s1)
    addi $t0, $zero, 1024
    sw $t0, 12($s1)

    # initialize position of the floor
    la $s1, floorBox
    sw $zero, 0($s1)
    addi $t0, $s0, -220
    sw $t0, 4($s1)
    addi $t0, $zero, 1848
    sw $t0, 8($s1)
    addi $t0, $zero, 220
    sw $t0, 12($s1)

    jal startPositionObs

    # initialize the width and height of the flappy bird
    la $s2, flappyBox
    addi $t0, $zero, 68
    sw $t0, 8($s2)
    addi $t0, $zero, 48
    sw $t0, 12($s2)

    # place the flappy bird to initial position
    jal startPositionFlappy


endInitializeData:

# syscall renderer.start();
addi $v0, $zero, 301
syscall

#--------------------------------------------------------------------
# procedure: main game loop
#--------------------------------------------------------------------
mainGameLoop:

    jal updateBackgroundFloor
    jal updateBirdAnimation

    # syscall get Mouse input as direction
    addi $v0, $zero, 302
    syscall
    la $t0, direction
    sw $v0, 0($t0)
    
    jal updateObstacles
    jal updateFlappy


endMainGameLoop:
    # syscall renderer.draw();
    addi $v0, $zero, 303
    la $a0, obstacles
    syscall
    # syscall checkSolution()
    addi $v0, $zero, 305
    la $a0, obstacles
    syscall
    # syscall Thread.sleep();
    addi $v0, $zero, 32
    addi $a0, $zero, 16
    syscall
    j mainGameLoop


#--------------------------------------------------------------------
# 	   Move the background and the floor, 
#         reset their x coordinates when necessary
#--------------------------------------------------------------------
updateBackgroundFloor:

    # step 1: subtract value 1 from the x coordinate attribute of the backgroundBox.
    # step 2: subtract value 3 from the x coordinate attribute of the floorBox.
    # step 3: check if backgroundBox.x + backgroundBox.width is <= 0,
    #         if true, set backgroundBox.x to 0.
    # step 4: check if floorBox.x + floorBox.width is <= 0,
    #         if true, set floorBox.x to floorBox.x + floorBox.width
    # 
    # Additional note (no need to understand): step 3 and step 4 may be confusing here. The image used to render 
    # the background and the floor has larger widths than the SIZE of the game window. 
    # When GUI renders the image, it will render the image twice, one after another.
    # Therefore, the background and the floor will be rendered seamlessly.
    # We directly reset the backgroundBox.x to 0 because the background moves only one pixel each time.
    # The floor moves three pixels each time, so we need to reset the floorBox.x to floorBox.x + floorBox.width
    # so that the floor will be rendered seamlessly after reset.
	addi $sp, $sp, -12
        sw $ra, 8($sp)
        sw $s1, 4($sp)
        sw $s0, 0($sp)
        la $s0, backgroundBox # $s0: backgroundbox address
        la $s1, floorBox # $s1: floorBox address
        
        lw $t0, 0($s0)
        addi $t0, $t0, -1
        sw $t0, 0($s0) #backgroundBox.x
        
        lw $t1, 0($s1)
        addi $t1, $t1, -3
        sw $t1, 0($s1)#floorBox.x
        
        lw $t3, 8($s0)#backgroundBox.width
        
        add $t4, $t3, $t0
        slt $t4, $zero, $t4
        bne $t4, $zero, out1

	addi $t0, $zero, 0
	sw $t0, 0($s0)
out1:
	
	lw $t3, 8($s0) #floorBox.width
	
	add $t4, $t3, $t1
	slt $t4, $zero, $t4
	bne $t4, $zero, out2
	
	addi $t1, $t4, 0
	sw $t1, 0($s1)
out2:

	lw $s0, 0($sp)
	lw $s1, 4($sp)
	lw $ra, 8($sp)
	addi $sp, $sp, 12
	
    jr $ra


#--------------------------------------------------------------------
# procedure: Loop through the bird images, 
#            update the birdIdx every 5 frames
#--------------------------------------------------------------------
updateBirdAnimation:

    addi $sp, $sp, -12
    sw $ra, 8($sp)
    sw $s1, 4($sp)
    sw $s0, 0($sp)

    la $s0, intervalFrame # $s0: intervalFrame address
    la $s1, birdIdx # $s1: birdIdx address

    lw $t0, 0($s0) # $t0: intervalFrame
    addi $t0, $t0, 1
    sw $t0, 0($s0)

    add $t1, $t0, $zero
    addi $t2, $zero, 5
    sub $t1, $t1, $t2
    blez $t1, endUpdateBirdAnimation

    sw $zero, 0($s0) # intervalFrame = 0
    lw $t0, 0($s1) # $t0: birdIdx
    addi $t0, $t0, 1
    sw $t0, 0($s1)

    addi $t1, $zero, 2
    sub $t0, $t0, $t1
    blez $t0, endUpdateBirdAnimation

    sw $zero, 0($s1) # birdIdx = 0


endUpdateBirdAnimation:
    lw $s0, 0($sp)
    lw $s1, 4($sp)
    lw $ra, 8($sp)
    addi $sp, $sp, 12
    jr $ra


#--------------------------------------------------------------------
#  Loop through each obstacle and update each of them
#--------------------------------------------------------------------
updateObstacles:
    # step 1: if the value of variable direction != -1, set variable inGame to 1.
    # step 2: if variable inGame is 1, proceeds to next step, otherwise, end this procedure.
    # step 3: loop through each obstacle. For each obstacle:
    #     step 3.1: subtract value of variable objectVelocity from the topPipe.x and bottomPipe.x.
    #     step 3.2: if topPipe.x + topPipe.width < 0, use procedure resetToNewPosition to reset the obstacle to a new position.
    #               When calling the procedure, pass the address of the obstacle and the new x coordinate as arguments.
    #               The new x coordinate is (SIZE + topPipe.width + 110). You can read the resetToNewPosition procedure for more details.
    #     step 3.3: use procedure intersects to check if the flappy bird intersects with the topPipe or bottomPipe.
    #               If intersects, sets variable won to 0 and calls procedure handleGameEnd. 
    #               When returned from handleGameEnd, end this procedure.
    #     step 3.4: call procedure updateGamePointLevel to update the score and level. The argument is the address of the obstacle.
    #               When returned from updateGamePointLevel, check if variable inGame is 0, if it is 0, then end this procedure.
    #     step 3.5: proceed to the next obstacle.
	
	addi $sp, $sp, -36
	sw $ra, 32($sp)
	sw $s0, 28($sp)
	sw $s1, 24($sp)
	sw $s2, 20($sp)
	sw $s3, 16($sp)
	sw $s4, 12($sp)
	sw $s5, 8($sp)
	sw $s6, 4($sp)
	sw $s7, 0($sp)
	
	la $s0, direction 
	la $s1, inGame
	la $s2, obstacles
	la $s3, objectVelocity
	la $s4, SIZE
	la $s5, won
	#step1
	
	lw $t0, 0($s0) #direction
	addi $t1, $zero, -1# set to -1
	beq $t0, $t1, nextstep
	
	addi $t1, $zero, 1
	sw $t1, 0($s1)
	
nextstep:

	lw $t0, 0($s1) #load inGame
	addi $t1, $zero, 1 #set to 1
	bne $t1, $t0, endupdateOb
	#begin step3
	
	addi $s6, $zero, 0 #iterator i is saved in $s6
	addi $t9, $zero, 4 #end flag
loop:
	
	addi $t1, $zero, 44 #length of a abstacles
	multu $t1, $s6
	mflo $t1 #44i
	add $s7, $t1, $s2 #addr of this obstacle is saved in $s7
	
	addi $t2, $s7, 0 #addr of toppipe.x
	addi $t3, $s7, 16 #addr of bottompipe.x
	
	#step3.1
	lw $t4, 0($t2) #toppipe.x
	lw $t5, 0($t3) #bottompipe.x
	lw $t6, 0($s3) #velocity
	
	sub $t4, $t4, $t6
	sub $t5, $t5, $t6
	sw $t4, 0($t2)
	sw $t5, 0($t3)
	
	#step3.2
	
	addi $t3, $s7, 8 #addr of toppipe.width
	lw $t5, 0($t3)#toppipe.width
	add $t6, $t4, $t5 #sum of them
	slt $t6, $t6, $zero #if topPipe.x + topPipe.width < 0 $t6 be 1
	beq $t6, $zero, outresetNp
	
	addi $a0, $s7, 0 #pass the addr of the obstacle
	
	lw $t6, 0($s4) #size
	add $t6, $t6, $t5
	addi $t6, $t6, 110
	addi $a1, $t6, 0
	jal resetToNewPosition
outresetNp:
	#step3.3
	la $t7, flappyBox
	addi $a0, $t7, 0
	
	addi $a1, $s7, 0
	jal intersects
	beq $v0, $zero, checkbottom

	sw $zero, 0($s5)
	jal handleGameEnd
	j endupdateOb
checkbottom:
	addi $a1, $s7, 16
	jal intersects
	beq $v0, $zero, nothit
	
	sw $zero, 0($s5)
	jal handleGameEnd
	j endupdateOb
nothit:

	#step3.4
	addi $a0, $s7, 0
	jal updateGamePointLevel
	lw $t6, 0($s1)
	beq $t6, $zero, endupdateOb
	 
	#step3.5
	addi $s6, $s6, 1
	addi $t0, $zero, 4
	beq $t0, $s6, outloop
	j loop
	
outloop:
endupdateOb:
	lw $s7, 0($sp)
	lw $s6, 4($sp)
	lw $s5, 8($sp)
	lw $s4, 12($sp)
	lw $s3, 16($sp)
	lw $s2, 20($sp)
	lw $s1, 24($sp)
	lw $s0, 28($sp)
	lw $ra, 32($sp)
	addi $sp, $sp, 36

    jr $ra


#--------------------------------------------------------------------
#  	    update game points if the bird has passed by an obstacle 
#         and decide whether to win the game by checking the winning score. 
#         Update variable objectVelocity to increase the difficulty
#--------------------------------------------------------------------
# $a0: obstacles[i] address
updateGamePointLevel:
    # step 1: if (flappyBox.x <= topPipe.x + topPipe.width) end this procedure.
    #         if the variable isPassedOn of the current obstacle is 1, end this procedure.
    # step 2: set the variable isPassedOn of the current obstacle to 1.
    # step 3: increment the variable point by 1.
    # step 4: if the variable point is larger than the variable record, 
    #         assign the value of variable point to the variable record.
    # step 5: if the variable point is equal to the variable winningScore,
    #         set the variable won to 1 and call procedure handleGameEnd.
    #         Then end this procedure.
    # step 6: if the two variables fulfill the following condition:
    #         (point % 2 == 0) && (objectVelocity < 12), increment the variable objectVelocity by 1.

	addi $sp, $sp, -32
	sw $ra, 28($sp)
	sw $s0, 24($sp)
	sw $s1, 20($sp)
	sw $s2, 16($sp)
	sw $s3, 12($sp)
	sw $s4, 8($sp)
	sw $s5, 4($sp)
	sw $s6, 0($sp)
	
	addi $s0, $a0, 0 #addr of obstacle[i]
	la $s1, flappyBox
	la $s2, point
	la $s3, record
	la $s4, winningScore
	la $s5, won
	la $s6, objectVelocity
	#step1
	
	lw $t0, 0($s1) #flappyBox.x
	lw $t1, 0($s0) #toppipe.x
	lw $t2, 8($s0) #toppipe.width
	add $t3, $t1, $t2 #sum of x and width
	slt $t3, $t3, $t0 # set to 1 if flappy.x > sum
	bne $t3, $zero, nextif
	j endupdateGPL
	
nextif:
	lw $t3, 32($s0) #isPassedOn
	addi $t4, $zero, 1 #set to 1
	bne $t3, $t4, nextstep2
	j endupdateGPL
nextstep2:
	#step2
	sw $t4, 32($s0)
	
	#step3
	lw $t4, 0($s2)
	addi $t4, $t4, 1#point
	sw $t4, 0($s2)
	
	#step4
	lw $t5, 0($s3) #record
	slt $t6, $t5, $t4 #set $t6 1 if record < point
	beq $t6, $zero, nextstep5
	
	addi $t5, $t4, 0
	sw $t5, 0($s3)
nextstep5:
	#step5
	lw $t6, 0($s4)
	bne $t4, $t6, nextstep6
	addi $t7, $zero, 1 #set to 1
	sw $t7, 0($s5)
	jal handleGameEnd
	j endupdateGPL
	
nextstep6:
	addi $t7, $zero, 2 #set to 2
	divu $t4, $t7
	mfhi $t7 #get point%2
	bne $t7, $zero, endupdateGPL
	
	lw $t7, 0($s6) #objectVelocity
	addi $t8, $zero, 12 #set to 12
	slt $t8, $t7, $t8 #$t8 set to 1 if v<12
	beq $t8, $zero, endupdateGPL
	
	addi $t7, $t7, 1
	sw $t7, 0($s6)
	
endupdateGPL:
	lw $s6, 0($sp)
	lw $s5, 4($sp)
	lw $s4, 8($sp)
	lw $s3, 12($sp)
	lw $s2, 16($sp)
	lw $s1, 20($sp)
	lw $s0, 24($sp)
	lw $ra, 28($sp)
	addi $sp, $sp, 32

    jr $ra


#--------------------------------------------------------------------
#	   update the bird's velocity based on mouse response and gravity, 
#         update the bird's vertical position based on its velocity. 
#         Check if the bird has collided with the floor or the ceiling 
#         and end the game accordingly
#--------------------------------------------------------------------
updateFlappy:

    # step 1: if the variable inGame is 0, end this procedure.
    # step 2: if the variable (intervalFrame % 2 == 0), update the bird velocity and position, otherwise proceeds to step 3.
    #        step 2.1: if variable direction is 0, increment the variable velocity by the value of variable gravity.
    #        step 2.2: if variable direction is 1, sets variable velocity to -9.
    #        step 2.3: update flappyBox.y as follows: flappyBox.y = flappyBox.y + velocity.
    # step 3: if flappyBox.y + flappyBox.height >= SIZE - gameData.floorBox.height, end game,
    #         if flappyBox.y <= 0, end game.
    #         If end game condition is met, set variable won to 0 and call procedure handleGameEnd.
	#------ Your code starts here ------
	addi $sp, $sp, -36
	sw $ra, 32($sp)
	sw $s0, 28($sp)
	sw $s1, 24($sp)
	sw $s2, 20($sp)
	sw $s3, 16($sp)
	sw $s4, 12($sp)
	sw $s5, 8($sp)
	sw $s6, 4($sp)
	sw $s7, 0($sp)
	
	
	la $s0, inGame
	la $s1, intervalFrame
	la $s2, velocity
	la $s3, gravity
	la $s4, direction
	la $s5, flappyBox
	la $s6, floorBox
	la $s7, won
	
	#step1
	lw $t0, 0($s0)
	bne $t0, $zero, step2
	j endupdateFly
	
step2:
	#step2
	lw $t0, 0($s1) #interval frame
	addi $t1, $zero, 2 #set to 2
	divu $t0, $t1
	mfhi $t0 #get the intervalframe%2
	bne $t0, $zero, step3
	
	#step2.1
	lw $t0, 0($s4) #direction
	addi $t1, $zero, 0 #set to 0
	bne $t0, $t1, nextcheck1
	lw $t1, 0($s3) #gravity
	lw $t2, 0($s2) #velocity
	add $t2, $t2, $t1 
	sw $t2, 0($s2)
	
nextcheck1:
	#step2.2
	addi $t1, $zero, 1 #set to 1
	bne $t0, $t1, nextcheck2
	addi $t2, $zero, -9 #set to -9
	sw $t2, 0($s2)
	
nextcheck2:
	#step2.3
	lw $t0, 4($s5) #flappyBox.y
	lw $t1, 0($s2) #velocity
	add $t0, $t0, $t1
	sw $t0, 4($s5)
	
step3:
	lw $t0, 4($s5) #flappyBox.y
	lw $t1, 12($s5) #flappyBox,height
	la $t2, SIZE
	lw $t2, 0($t2) #size
	lw $t3, 12($s6) #floorBox.height
	add $t4, $t0, $t1 #sum of part1
	sub $t5, $t2, $t3 #subtraction of part2
	slt $t6, $t4, $t5 #set to 1 if part1 < part2
	beq $t6, $zero, endgame
	
	slt $t6, $zero, $t0 #set to 1 if flappybox.y > 0
	beq $t6, $zero, endgame
	j endupdateFly
endgame:
	
	 sw $zero, 0($s7)
	 jal handleGameEnd

	
endupdateFly:
	lw $s7, 0($sp)
	lw $s6, 4($sp)
	lw $s5, 8($sp)
	lw $s4, 12($sp)
	lw $s3, 16($sp)
	lw $s2, 20($sp)
	lw $s1, 24($sp)
	lw $s0, 28($sp)
	lw $ra, 32($sp)

    jr $ra


#--------------------------------------------------------------------
# check if two rectangles intersect
#--------------------------------------------------------------------
# $a0: rectangle1 address, $a1: rectangle2 address
# return value: $v0: 1 if intersects, 0 otherwise
intersects:

    # two rectangles intersect if the following condition is fulfilled:
    #            rectangle1.x < rectangle2.x + rectangle2.width
    #            && rectangle1.x + rectangle1.width > rectangle2.x
    #            && rectangle1.y < rectangle2.y + rectangle2.height
    #            && rectangle1.y + rectangle1.height > rectangle2.y
 
    addi $sp, $sp, -12
    sw $ra, 8($sp)          
    sw $s0, 4($sp)          
    sw $s1, 0($sp)
    	
    addi $s0, $a0,0 #addr of rect1
    addi $s1, $a1,0 #addr of rect2
    
    addi $v0, $zero, 0 #initialize to 0
    
    lw $t0, 0($s0) #rect1.x
    lw $t1, 0($s1) #rect2.x
    lw $t2, 8($s1) #rect2,width
    add $t3, $t1, $t2
    slt $t4, $t0, $t3
    beq $t4, $zero, intersection_end
    
    lw $t0, 0($s1)
    lw $t1, 0($s0)
    lw $t2, 8($s0)
    add $t3, $t1, $t2
    slt $t4, $t0, $t3
    beq $t4, $zero, intersection_end
    
    lw $t0, 4($s0)
    lw $t1, 4($s1)
    lw $t2, 12($s1)
    add $t3, $t1, $t2
    slt $t4, $t0, $t3
    beq $t4, $zero, intersection_end
    
    lw $t0, 4($s1)
    lw $t1, 4($s0)
    lw $t2, 12($s0)
    add $t3, $t1, $t2
    slt $t4, $t0, $t3
    beq $t4, $zero, intersection_end
    
    addi $v0, $v0, 1
    
    
     
intersection_end:
	lw $s1, 0($sp)
	lw $s0, 4($sp)
	lw $ra, 8($sp)
	
	
	addi $sp, $sp, 12
	


    jr $ra


#--------------------------------------------------------------------
# procedure
#--------------------------------------------------------------------
# $a0: obstacle address, $a1: newX
resetToNewPosition:

    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $s0, 0($sp)

    # set newX
    add $s0, $a0, $zero # $s0: obstacle address
    sw $a1, 0($s0)
    sw $a1, 16($s0)

    # set random newY for topPipe: -(new Random().nextInt(280) + 200)
    addi $a1, $zero, 280
    addi $v0, $zero, 42
    syscall
    add $t0, $a0, $zero #t0: int[0, 280)
    addi $t0, $t0, 200
    sub $t0, $zero, $t0 #t0: topPipe.y
    sw $t0, 4($s0)

    # set newY for bottomPipe: topPipe.y + topPipe.height + obstacleDistance
    lw $t1, 12($s0) #t1: topPipe.height
    la $t2, obstacleDistance
    lw $t2, 0($t2) #t2: obstacleDistance
    add $t3, $t0, $t1 
    add $t3, $t3, $t2 #t3: bottomPipe.y
    sw $t3, 20($s0)

    # set isPassedOn to false
    sw $zero, 32($s0)

    # set topPipeType and bottomPipeType to random int in [0, 3)
    addi $a1, $zero, 3
    addi $v0, $zero, 42
    syscall
    sw $a0, 36($s0)
    addi $a1, $zero, 3
    addi $v0, $zero, 42
    syscall
    sw $a0, 40($s0)

endResetToNewPosition:
    lw $s0, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra


#--------------------------------------------------------------------
# procedure
#--------------------------------------------------------------------
startPositionObs:

    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $s2, 8($sp)
    sw $s1, 4($sp)
    sw $s0, 0($sp)

    la $t0, SIZE
    lw $s0, 0($t0) # $s0: SIZE

    # initialize positions of obstacles
    add $s1, $zero, $zero # $s1: loop index

    obsInitLoop:
        addi $t0, $zero, 4
        beq $s1, $t0, endStartPositionObs

        la $t0, obstacles
        addi $t1, $zero, 11
        mult $s1, $t1
        mflo $t1
        sll $t1, $t1, 2
        add $s2, $t0, $t1 # $s2: address of the obstacle[i]

        # initialize the width and height of the topPipe
        addi $t0, $zero, 104
        sw $t0, 8($s2) # topPipe.width
        sw $t0, 24($s2) # bottomPipe.width
        addi $t1, $zero, 540 
        sw $t1, 12($s2) # topPipe.height
        sw $t1, 28($s2) # bottomPipe.height

        # place the obstacle
        # newX: (SIZE + 104) + (i * 340)
        addi $t0, $zero, 340
        mult $s1, $t0
        mflo $t0
        addi $t1, $s0, 104
        add $t2, $t1, $t0 # $t2: newX
        add $a0, $s2, $zero
        add $a1, $t2, $zero
        jal resetToNewPosition

        obsInitLoopEnd:
        addi $s1, $s1, 1
        j obsInitLoop

endStartPositionObs:
    lw $s0, 0($sp)
    lw $s1, 4($sp)
    lw $s2, 8($sp)
    lw $ra, 12($sp)
    addi $sp, $sp, 16
    jr $ra


#--------------------------------------------------------------------
# procedure
#--------------------------------------------------------------------
startPositionFlappy:

    addi $sp, $sp, -12
    sw $ra, 8($sp)
    sw $s1, 4($sp)
    sw $s0, 0($sp)

    la $t0, SIZE
    lw $s0, 0($t0) # $s0: SIZE
    la $s1, flappyBox # $s1: flappyBox
    # x = (SIZE / 2) - (width * 3), y = (SIZE / 2) - (height / 2)
    addi $t0, $zero, 2 # $t0: 2
    div $s0, $t0
    mflo $t1 # $t1: SIZE / 2
    lw $t2, 8($s1) # $t2: flappyBox.width
    add $t3, $t2, $t2 
    add $t3, $t3, $t2 # $t3: flappyBox.width * 3
    sub $t4, $t1, $t3 # $t4: x
    sw $t4, 0($s1)

    lw $t2, 12($s1) # $t2: flappyBox.height
    div $t2, $t0
    mflo $t3 # $t3: flappyBox.height / 2
    sub $t4, $t1, $t3 # $t4: y
    sw $t4, 4($s1)


endStartPositionFlappy:
    lw $s0, 0($sp)
    lw $s1, 4($sp)
    lw $ra, 8($sp)
    addi $sp, $sp, 12
    jr $ra


#--------------------------------------------------------------------
# procedure
#--------------------------------------------------------------------
handleGameEnd:

    addi $sp, $sp, -4
    sw $ra, 0($sp)

    la $t0, point
    sw $zero, 0($t0) # point = 0
    la $t0, objectVelocity
    addi $t1, $zero, 6
    sw $t1, 0($t0) # objectVelocity = 6
    la $t0, direction
    addi $t1, $zero, -1
    sw $t1, 0($t0) # direction = -1
    # reset GUI direction using syscall
    addi $v0, $zero, 304
    addi $a0, $zero, -1
    
    
    syscall
    la $t0, inGame
    sw $zero, 0($t0) # inGame = false
    la $t0, velocity
    sw $zero, 0($t0) # velocity = 0

    jal startPositionObs
    jal startPositionFlappy

endHandleGameEnd:
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra


endGame:

    addi $v0, $zero, 306
    syscall




