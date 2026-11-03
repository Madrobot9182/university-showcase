#
# CMPUT 229 Public Materials License
# Version 1.0
#
# Copyright 2020 University of Alberta
# Copyright 2022 Yufei Chen
# TODO: claim your copyright
# This software is distributed to students in the course
# CMPUT 229 - Computer Organization and Architecture I at the University of
# Alberta, Canada.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the disclaimer below in the documentation
#    and/or other materials provided with the distribution.
#
# 2. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
#-------------------------------
# Lab_Snake_Game Lab
#
# Author: Ryan Yan
# Date: March 24, 2024
#
#-------------------------------

.include "common.s"

.data
.align 2

DISPLAY_CONTROL:    .word 0xFFFF0008
DISPLAY_DATA:       .word 0xFFFF000C
INTERRUPT_ERROR:	.asciz "Error: Unhandled interrupt with exception code: "
INSTRUCTION_ERROR:	.asciz "\n   Originating from the instruction at address: "

Brick:      .asciz "#"

# Mine
Apple:      .asciz "a"
SnakeHead:      .asciz "@"
SnakeBody:      .asciz "#*"
WELCOME_MESSAGE:	.asciz "Welcome! Please enter [1, 2, 3] to start the game!"
GAMEOVER_MESSAGE:	.asciz "GAME OVER! BETTER LUCK NEXT TIME!"
POINTS_MESSAGE:		.asciz " points"
COUNTDOWN_MESSAGE:	.asciz " seconds"

countdown:	.word	0	# Values for remaining time, points, and bonus points
points:		.word	0
bonusTime:	.word	0
snakeStack:	.space	64	# Stack for the snake coordinates
appleStack:	.space  16	# Stack for the apple coordinates

levelValidInput:	.word 0		# Valid level input flag
snakeInput:	.word 4			# 1 = up, 2 = down, 3 = left, 4 = right
previousSnakeInput:	.word 4		# The last move the snake made
countdownFlag:		.word 0		# Valid Countdown Flag

.text
#---------------------------------------------------------------------------------------------
# snakeGame
# 
# Subroutine description: This is the main function that runs the snake game logic.
#
# 
#   Args:
#   	N/A
#
# Register Usage
#      s0: Loop counter for clearing the screen at the end
#
# Return Values:
#	N/A
#---------------------------------------------------------------------------------------------
snakeGame:
	# Initalize stack
	addi	sp, sp, -8
	sw	ra, 0(sp)
	sw	s0, 4(sp) # Loop counter for snakeEnd
	
	# Initalize Handler, need to store address of handler in utvec
	la	a0, handler
	csrw	a0, 0x05 # utvec <- &handler
	csrwi	0x00, 1 # ustatus enable interrupts
	csrwi	0x04, 0x110 # uie write bit 4 and 8, enabling timer and keyboard interrupt
	# Enable keyboard interrupts
	li	t0, 0xFFFF0000 # Address of Keyboard control
	li	t1, 2
	sw	t1, 0(t0) # Enable keyboard interrupts

# Phase 1: Picking the level
	# Print the welcome message, ask to pick a level
	la	a0, WELCOME_MESSAGE
	li	a1, 0
	li	a2, 0
	jal	ra, printStr

levelLoop:
	# Access input and see if it's a valid number
	# Branch to levelLoop until input received
	la	a0, levelValidInput
	lw	t0, 0(a0)
	beqz	t0, levelLoop # Loop until valid input is received
	
# Phase 1.5: Initalizing the game
	# Initalize level parameters (Inital and gained/bonus time)
	# Intalize the game itself. Snake, Apple
	# Print game for first time (wall, score, snake and apple)
	li	a0, 0xFFFF0004 # Load keyboard data
	lw	t0, 0(a0) # t0 <- key that was pressed
	
	# Get the level (1-3) and initalize starting times
	la	a1, countdown # Load &countdown into a1
	la	a2, bonusTime # Load $bonusTime into a2
	
	li	t1, 0x31 # 1 in ascii
	beq	t0, t1, levelOne
	addi	t1, t1, 1 # 2 in ascii
	beq	t0, t1, levelTwo
	# Else we are playing levelThree
	levelThree:
		li	t2, 15 # Inital time = 15
		sw	t2, 0(a1) # Countdown = Inital time
		li	t3, 3 # Bonus time
		sw	t3, 0(a2) # Bonus time = 3s
		jal	zero, snakeStart
	levelTwo:
		li	t2, 30 # Inital time = 30
		sw	t2, 0(a1) # Countdown = Inital time
		li	t3, 5 # Bonus time
		sw	t3, 0(a2) # Bonus time = 3s
		jal	zero, snakeStart
	levelOne:
		li	t2, 120 # Inital time = 120
		sw	t2, 0(a1) # Countdown = Inital time
		li	t3, 8 # Bonus time
		sw	t3, 0(a2) # Bonus time = 3s
		jal	zero, snakeStart
		
# Phase 2: Snake Game
snakeStart:
	# For initalizing the board and snake and apple and timer
	# First clear the old text
	li	a0, 60 # Length to clear
	li	a1, 0 # At row 0
	li	a2, 0 # Column 0
	li	a3, 0x00 # NUL
	jal	ra, printMultipleSameChars

	# Initalize the apple
	la	t5, appleStack
	jal	ra, random # a0 contains random number
	addi	a0, a0, 1 # Save row + 1
	sw	a0, 0(t5) # Save row in first section of applestack
	jal	ra, random # a0 contains random number
	addi	a0, a0, 1 # Save column + 1
	sw	a0, 4(t5) # Save column in second section of applestack
	
	# Initalize snake stack in the same place each time
	# But if the apple spawn on the snake, we simply raise the snake spawn by one
	la	a0, snakeStack # Get the address of the snake stack
	li	t0, 6 # Inital row index
	li	t1, 6 # Inital column index
	li	t2, 4 # Loop counter
	
	lw	t3, 0(t5) # Get the row of apple
	bne	t0, t3, snakeInitLoop # if snake is not on same row apple we skip the next few lines
	addi	t0, t0, -1 # Else We reduce t0 by one or "raise" the spawn

	snakeInitLoop:
		sw	t0, 0(a0) # Store the row coordinate
		sw	t1, 4(a0) # Store the column coordinate
		addi	a0, a0, 8 # Move to next section of the stack
		addi	t1, t1, -1 # Subtract column index by one
		addi	t2, t2, -1 # Subtract loop counter by one
		bnez	t2, snakeInitLoop # Loop 4 times for all pieces

	# Then print the walls
	jal	ra, printAllWalls 
	
	# Then print the score and countdown text
	la	a0, POINTS_MESSAGE
	li	a1, 2 # At Row 2
	li	a2, 25 # Column 25
	jal	ra, printStr
	
	la	a0, COUNTDOWN_MESSAGE
	li	a1, 3 # At Row 3
	li	a2, 25 # Column 25
	jal	ra, printStr
	
	# Finally print the snake, apple, and stats:
	jal	ra, drawGame
	
	# Start the timer by initalizing timecmp
	li	a0, 0xFFFF0020 # Load Timecmp address
	li	a1, 0xFFFF0018 # Load actual time
	lw	t2, 0(a1) # Get value of Time in t2
	addi	t2, t2, 1000 # t2 + 1000ms / 1 second
	sw	t2, 0(a0) # Store back in Timecmp

# Main game loop, repeately move the snake and play snake until the game is over
snakeLoop:
	# Branch to snakeLoop until input received
	la	a0, countdownFlag # Check if flag = 1 or one second has passed
	lw	t0, 0(a0)
	beqz	t0, snakeLoop # Wait until a second has passed
	
	# Reset the flag
	la	a0, countdownFlag # Load flag
	li	t1, 0
	sw	t1, 0(a0) # Set flag back to zero
	
	# Move snake by current direction	
	jal	ra, moveSnake # Move the snake in currently selected direction
	
	# Check if snake is on wall/apple/out of time
	jal	ra, checkHitting # Returns 1 if snake has hit a wall
	bnez	a0, snakeDone # Wall has been hit, exit game
	
	jal	ra, checkEatingApple # Returns 1 if snake ate apple
	bnez	a0, addPoints # Apple eaten = more points!!
	jal	zero, pointsEnd # Else no points :(

addPoints:
	# Add one to the points
	la	a2, points
	lw	t2, 0(a2) # Get decimal of points
	addi	t2, t2, 1 # add 1
	sw	t2, 0(a2) # Store back into points
	
	# Add bonusTime to countdown
	la	a1, bonusTime
	lw	a1, 0(a1) # Get decimal of bonus points
	la	a2, countdown
	lw	t2, 0(a2) # Get decimal of countdown
	add	t2, t2, a1 # Increase time
	sw	t2, 0(a2) # Update countdown
	
pointsEnd:
	# Then print the updated board again	
	jal	ra, drawGame # Print the board again
	
	# Check if we're out of time
	la	a0, countdown
	lw	a0, 0(a0)
	beqz	a0, snakeDone # Out of time
	
	# Repeat the game loop
	jal	zero, snakeLoop

# Phase 3: End the game by clearing the screen, print game over, and returning
snakeDone:
	li	s0, 0 # Loop counter (the only s-register lol)
clearLoop:
	li	a0, 50 # clear length = 50
	mv	a1, s0 # Loop through every row
	li	a2, 0 # From column 0
	li	a3, 0x00 # Null char for printing/clear
	jal	printMultipleSameChars
	
	li	t0, 10
	addi	s0, s0, 1
	ble	s0, t0, clearLoop # Clear 10 rows
	
	# Print Gameover
	la	a0, GAMEOVER_MESSAGE
	li	a1, 0
	li	a2, 0
	jal	ra, printStr
	
	# Restore stack and return
	lw	ra, 0(sp)
	lw	s0, 4(sp)
	addi	sp, sp, 8

	ret

#---------------------------------------------------------------------------------------------
# drawGame
# 
# Subroutine description: runs the functions to print the score/countdown, apple, and snake. To be called repeately in snakeGame
# 
#   Args:
#	N/A
#
# Register Usage
#      a0-a2: Parameters for printing apple
#
# Return Values:
#	N/A
#---------------------------------------------------------------------------------------------
drawGame:
	# Initalize Stack
	addi	sp, sp, -4
	sw	ra, 0(sp)
	
	# Print the score values
	jal	ra, drawInfo
	
	# Print the apple
	la	t0, appleStack
	la	a0, Apple # Get character a for apple
	lbu	a0, 0(a0) # Load byte a
	lw	a1, 0(t0) # Copy apple row into a1
	lw	a2, 4(t0) # Copy apple column into a2
	jal	ra, printChar # Print apple 
	
	# Then draw the snake (so snake can overlap apple)
	jal	ra, drawSnake
	
	# Restore stack
	lw	ra, 0(sp)
	addi	sp, sp, 4
	ret

#---------------------------------------------------------------------------------------------
# drawSnake
# 
# Subroutine description: Print out the snake from reading the snakeStack
# 
#   Args:
#	N/A (Read snakeStack variable)
#
# Register Usage
#      s0: The address of the snakeStack
#      s1: Printing loop counter
#
# Return Values:
#	N/A
#---------------------------------------------------------------------------------------------
drawSnake:
	# Initalize Stack
	addi	sp, sp, -12
	sw	ra, 0(sp)
	sw	s0, 4(sp)
	sw	s1, 8(sp)
	
	la	s0, snakeStack # Load address
	li	s1, 3 # Loop bodypiece counter (head printed outside loop)
	
	#First draw the head
	la	a0, SnakeHead
	lbu	a0, 0(a0) # Get the @ symbol
	lw	a1, 0(s0) # Row of snake head
	lw	a2, 4(s0) # Column of snake body
	jal	ra, printChar # Print head
	
	drawSnakeBody:
	# Loop for drawing the 3 body parts
		addi	s0, s0, 8 # Move onto next body chunk
		la	a0, SnakeBody # Load snakebody *
		lbu	a0, 0(a0) # Get the * symbol
		lw	a1, 0(s0) # Row of snake head
		lw	a2, 4(s0) # Column of snake body
		jal	ra, printChar # print body piece
		
		addi	s1, s1, -1 # reduce counter by one
		bnez	s1, drawSnakeBody # Loop 3 times
	
	# Restore stack and return
	lw	ra, 0(sp)
	lw	s0, 4(sp)
	lw	s1, 8(sp)
	addi 	sp, sp, 12
	ret

#---------------------------------------------------------------------------------------------
# drawInfo
# 
# Subroutine description: Print out the points and countdown at a fixed location
# 
#   Args:
#   N/A (We read points and countdown from the global variables)
#
# Register Usage
#      s0: Stores the value being printed
#      s1: Counter for the bufferloop
#      s2: Printing offset to to adjust which column/digit we're printing at
#      s3: Stores greatest signficiance of the digit (1, 10, 100)
#      s4: Tracker to change either points or countdown
#
# Return Values:
#	N/A
#---------------------------------------------------------------------------------------------
drawInfo:
	# Stack
	addi	sp, sp, -24
	sw	ra, 0(sp)
	sw	s0, 4(sp) # Stores the number being printed in digit format
	sw	s1, 8(sp) # Buffer counter
	sw	s2, 12(sp) # Printing offset to track which digit we at printing
	sw	s3, 16(sp) # Stores greatest signficiance of digit (1, 10, 100)
	sw	s4, 20(sp) # Track which of points or countdown we printing

	# There will be two loops, one for the points and one for the coundown
	li	s4, 0 # Initalize tracker to zero	
drawLoop:
	li	s2, 0 # Initalize printing offset to 0
	# Pick which info (points or countdown), we are working with
	bnez	s4, countInfo # If not zero then we are working with points
	la	t0, points # Load address of the score
	lw	s0, 0(t0) # Decimal of score
	jal	zero, bufferStart # Else jump and start the buffer process
	
	countInfo:
		# Here we initalize s0 to be the countdown value instead
		la	t0, countdown
		lw	s0, 0(t0)
		jal	zero, bufferStart # Start buffering
bufferStart:
	# Check how many digit we're at so we can print the 0 buffers
	li	s3, 100 # Significant
	li	s1, 2 # Bufferloop counter
	bge	s0, s3, bufferLoopEnd # No need to print buffers
	li	s3, 10
	li	s1, 1 # One buffers
	bge	s0, s3, bufferLoop # One buffer
	li	s3, 1
	li	s1, 0 # two buffers
	jal	zero, bufferLoop # Else it's one digit, 2 buffers

bufferLoop:
	# Depending on how many digits we're missing, print out that many zeros	
	li	t0, 2
	bge	s1, t0, bufferLoopEnd # At most 2 buffers
	
	li	a0, 0x30 # Ascii for 0
	addi	a1, s4, 2 # Row to print at (2 or 3)
	addi	a2, s2, 22 # 22 + offset, Column to print 0
	jal	ra, printChar # Print number
	
	addi	s1, s1, 1 # Increment counter
	addi	s2, s2, 1 # Increment offset by one
	jal	zero, bufferLoop
	
bufferLoopEnd:	
	# Then print out the actual digits
	# Divide number by significance, convert digit to ascii, then print onto screen
	div	t0, s0, s3 # Divide by significance to get more sig digit
	mul	t1, t0, s3 # t1 = t0 increased to significance (1 -> 100)
	sub	s0, s0, t1 # Delete the greatest significant bit
	
	addi	a0, t0, 48 # Convert to ascii and print
	addi	a1, s4, 2 # Row to print at (2 or 3)
	addi	a2, s2, 22 # 22 + offset, Column to print 0
	jal	ra, printChar # Print number
	
	li	t0, 10
	div	s3, s3, t0 # Reduce the significance by next factor (100->10->1)
	
	addi	s2, s2, 1 # Increase offset by one
	li	t0, 3
	blt	s2, t0, bufferLoopEnd # if we have done 3 offsets then we are done
	
	# Finally, loop again to print countdown
	addi	s4, s4, 1
	li	t0, 1
	ble	s4, t0, drawLoop # We only print two lines

	# DONE: Restore stack
	lw	ra, 0(sp)
	lw	s0, 4(sp)
	lw	s1, 8(sp)
	lw	s2, 12(sp)
	lw	s3, 16(sp)
	lw	s4, 20(sp)
	addi	sp, sp, 24
	ret # Return

#---------------------------------------------------------------------------------------------
# moveSnake
# 
# Subroutine description: Update the snakeStack to "move" the snake based on direction in snakeInput
#	The directions are: 1 = up | 2 = down | 3 = left | 4 = right |
# 
#   Args:
#   N/A (Read from global variables)
#
# Register Usage
#      s0: Stores address of Snakestack
#
# Return Values:
#	N/A
#---------------------------------------------------------------------------------------------
moveSnake:
	# Stack
	addi	sp, sp, -8
	sw	ra, 0(sp)
	sw	s0, 4(sp)
	
	la	s0, snakeStack # Stack containing location of snake
	
	# First clear the last previous bodypiece from the board
	li	a0, 0x20 # Space char
	lw	a1, 24(s0) # Last bodypiece row
	lw	a2, 28(s0) # Last bodypiece column
	jal	ra, printChar # Clear the bodypiece

	# First move the head, copy the old cords and apply them to next body
	# repeat for the 3 bodypart
	lw	t2, 0(s0) # Row of snake head
	lw	t3, 4(s0) # Column of snake head
	
	la	a2, snakeInput
	lw	a2, 0(a2) # Get input
	
	# Check which direction has been inputted
	li	t4, 1
	beq	a2, t4, moveUp
	li	t4, 2
	beq	a2, t4, moveDown
	li	t4, 3
	beq	a2, t4, moveLeft
	li	t4, 4
	beq	a2, t4, moveRight
	
	# Then adjust the headpiece accordingly
	moveUp:
		addi	t2, t2, -1
		jal	zero, moveHead
	moveDown:
		addi	t2, t2, 1
		jal	zero, moveHead
	moveLeft:
		addi	t3, t3, -1
		jal	zero, moveHead
	moveRight:
		addi	t3, t3, 1
		jal	zero, moveHead
	
	moveHead:
	# Move the new values of head into the stack
		lw	t0, 0(s0) # Copy old head row
		lw	t1, 4(s0) # Copy old head col
		sw	t2, 0(s0) # Set new Row of snake head
		sw	t3, 4(s0) # Set new Column of snake body
		li	a1, 4 # Loop counter for loop below
	moveLoop:
		addi	s0, s0, 8 # Get next section of snake
		# Get location of old body
		lw	t2, 0(s0) # Row of snake body
		lw	t3, 4(s0) # Column of snake body
		# Move body to next cords
		sw	t0, 0(s0)
		sw	t1, 4(s0)
		# Copy old into "new"
		mv	t0, t2
		mv	t1, t3
		
		addi	a1, a1, -1 # Increment pointer for loop
		bnez	a1, moveLoop
	
	# Restore stack
	lw	ra, 0(sp)
	lw	s0, 4(sp)
	addi	sp, sp, 8
	ret # Done

#---------------------------------------------------------------------------------------------
# checkEatingApple
# 
# Subroutine description: We compare the head of snake if it's on cords of apple. Return 1 if snake has eaten apple
#	If apple was eaten, we generate a new apple and save it in the appleStack 
# 
#   Args:
#   N/A (Both stacks read from global variables)
#
# Register Usage
#      s0: Address of snakeStack
#      s1: Address of snakeStack
#      s2: Row Coordinates of new apple (if needed)
#      s3: Column Coordinate of new apple (if needed)
#
# Return Values:
#	a0: 1 if apple was eaten, 0 if not eaten
#---------------------------------------------------------------------------------------------
checkEatingApple:
	# Stack
	addi	sp, sp, -20
	sw	ra, 0(sp)
	sw	s0, 4(sp)
	sw	s1, 8(sp)
	sw	s2, 12(sp)
	sw	s3, 16(sp)

	la	s0, snakeStack # s0 Get snake stack
	lw	t1, 0(s0) # Row index of snake
	lw	t2, 4(s0) # Column index of snake
	
	la	s1, appleStack # s1, get appleStack
	lw	t4, 0(s1) # Row index of apple
	lw	t5, 4(s1) # Column index of snake
	# If cords of head not on apple, apple wasn't eaten
	bne	t4, t1, notEaten # Different row
	bne	t5, t2, notEaten # Different column

newAppleLoop:	
	# Else apple is eaten, get new apple and return one
	# Logic to ensure apple is not on snake
	# Initalize the apple
	jal	ra, random # a0 contains random number
	addi	s2, a0, 1 # Save row + 1

	jal	ra, random # a0 contains random number
	addi	s3, a0, 1 # Save column + 1
	
	# Dumb load every piece of snake, check if apple is on snake
	li	a1, 4 # Loop Counter
	checkSnakeAppleLoop:
		beqz	a1, checkAppleEnd
		addi	a1, a1, -1 # Decrement loop, 
				
		lw	t0, 0(s0) # Row of snake piece
		lw	t1, 4(s0) # Column of snake piece
		addi	s0, s0, 8 # Queue next piece of snake
		
		# If there is a collision, regenerate apple
		bne	t0, s2, checkSnakeAppleLoop # Rows are different, check next bodypiece
		# Else row same, if column same when we are on apple
		beq	t1, s3, newAppleLoop # We are on the new apple, regenerate
		# Else column is different, check next bodypiece
		jal	zero, checkSnakeAppleLoop
checkAppleEnd:
	# From here we have perfectly safe new apple location, store in stack
	sw	s2, 0(s1) # New row of apple
	sw	s3, 4(s1) # New column of apple	
	li	a0, 1 # Indicate apple eaten and new location generated
	jal	zero, appleEnd # Jump to end
	
notEaten:
	# Not eaten, return 0
	li	a0, 0
	jal	zero, appleEnd
appleEnd:
	# Restore stack
	lw	ra, 0(sp)
	lw	s0, 4(sp)
	lw	s1, 8(sp)
	lw	s2, 12(sp)
	lw	s3, 16(sp)
	addi	sp, sp, 20
	ret

#---------------------------------------------------------------------------------------------
# checkHitting
# 
# Subroutine description: Check if the snake has hit the wall. Compare the coordinates to see if snake is out of bound
#	returns 1 if the snake hit the wall, 0 if not 
#
#   Args:
#   N/A (snakeStack read from variable)
#
# Register Usage
#      N/A
#
# Return Values:
#	a0: 1 if snake hit the wall, 0 if not
#---------------------------------------------------------------------------------------------
checkHitting:
	la	t0, snakeStack # Get snake stack
	lw	t1, 0(t0) # Row index of snake
	lw	t2, 4(t0) # Column index of snake
	
	li	t3, 0
	ble	t1, t3, hasHit # row <= 0
	ble	t2, t3, hasHit # col <= 0
	
	li	t3, 10
	bge	t1, t3, hasHit # row >= 10
	li	t3, 20
	bge	t2, t3, hasHit # col >= 20
	
	# Else snake is still alive, return 0
	li	a0, 0
	ret
hasHit:
	# Snake has hit wall, return 1
	li	a0, 1
	ret

#---------------------------------------------------------------------------------------------
# random
# 
# Subroutine description: Generates a pusdo-random number based on the algorith and set global numbers
#	global variables XiVar, aVar, cVar, and mVar. Xi = ( aX_(i-1) + c ) % m
#	When using for generating apple coordinates, add one to adjust for the walls 	
#
#   Args:
#	N/A (numbers read from global vars)
#
# Register Usage
#	a0: Xi - the X coordinate
#	a1: a
#	a2: c
#	a3: m
#
# Return Values:
#	a0: the random number
#---------------------------------------------------------------------------------------------
random:
	# Load all the global variables
	la	a0, XiVar
	la	a1, aVar
	la	a2, cVar
	la	a3, mVar
	
	# Load value into registers
	lw	t0, 0(a0)
	lw	t1, 0(a1)
	lw	t2, 0(a2)
	lw	t3, 0(a3)
	
	# Perform operations
	mul 	t0, t0, t1 # a * X
	add	t0, t0, t2 # aX + c
	remu 	t0, t0, t3 # a0 % m (the remainder of a0/m)
	
	sw	t0, 0(a0) # Save the result back into XiVar for next call
	mv	a0, t0 # Move result into a0
	jalr   zero, ra, 0 # a0 contains random value, return

#---------------------------------------------------------------------------------------------
# handler
# 
# My user handler. It takes cares of both timer and keyboard interrups, ignores any other interupt/exception
# 
# Timer: Set the countdown flag to one, decrement the countdown, reset the timer for one second
# 	and set the previous input flag to current input. Used to prevent snake from going backwards
#
# Keyboard: Check for valid direction, set the keyboard flag to one, then set the direction in the variable accordingly
#---------------------------------------------------------------------------------------------
handler:
	# Swap uscratch and a0
	csrrw	a0, 0x040, a0 # Addr[iTrapData] <-> USERa0
	# Save register used in handler except a0
	sw	t0, 0(a0)
	sw	t1, 4(a0)
	# store USERa0
	csrr	t0, 0x040 # t0 <- USERa0
	sw	t0, 8(a0) # Save USERa0
	
	# Check if it's interrupt and if exception code = 4 (user timer interrupt)
	csrrwi	t0, 0x42, 0 # read into t0 and clear
	li	t1, 0x7FFFFFFF
	and	t1, t0, t1 # Mask the 31 bit
	beqz	t1, handlerTerminate # Interrupt = 1, we don't want exception
	   
	# Then check for exception code
	li	t1, 4
	and	t1, t0, t1 # Mask for code 4 timer
	bnez	t1, timerInterrupt
	li	t1, 8
	and	t1, t0, t1 # Mask for code 8 keyboard
	bnez	t1, keyboardInterrupt
	jal	zero, handlerTerminate # Else it's another exception we can't handle
	
timerInterrupt:
	# We have reached the timer, decrement the countdown by one and reset the timer for one second
	la	t0, countdown # Load countdown address
	lw	t1, 0(t0) # Load countdown int
	addi	t1, t1, -1  # Decrement by one
	sw	t1, 0(t0) # Store back into memory
	
	# Set previousSnakeInput to current direction
	la	t0, snakeInput
	lw	t0, 0(t0) # Get value of snake input
	la	t1, previousSnakeInput
	sw	t0, 0(t1) # Update into previousSnakeInput
	
	# Set countdownFlag to 1
	la	t0, countdownFlag # &countdownFlag
	li	t1, 1
	sw	t1, 0(t0) # countdownFlag = 1
	
	# Set timer for one second
	li	t0, 0xFFFF0018 # Load actual time
	lw	t0, 0(t0) # Get value of Time in t0
	addi	t0, t0, 1000 # t0 + 1000ms / 1 second
	li	t1, 0xFFFF0020 # Load Timecmp address
	sw	t0, 0(t1) # Store back in Timecmp
	
	jal	zero, endHandler
	
keyboardInterrupt:
	# For now everything shall be a valid input
	li	t0, 0xFFFF0004 # Load keyboard data
	lw	t1, 0(t0) # t1 <- key that was pressed
	
	# First check if input is 1, 2, 3 for levels
	li	t0, 0x33 # Hex for 3
	ble	t1, t0, validatedLevelInput
	# Then check if key is one of w 0x77 a 0x61 s 0x73 d 0x64
	li	t0, 0x61
	beq	t0, t1, InputLeft
	li	t0, 0x64
	beq	t0, t1, InputRight
	li	t0, 0x73
	beq	t0, t1, InputDown
	li	t0, 0x77
	beq	t0, t1, InputUp
	# Else this is not a valid input
	jal	zero, endKeyboard
	
	validatedLevelInput:
		li	t0, 0x30 # 0 in hex
		ble	t1, t0, endKeyboard # Key is 0 or less, invalid
		# Set level flag to one
		la	t0, levelValidInput
		li	t1, 1
		sw	t1, 0(t0)
		jal	zero, endKeyboard
	
	# Basically a big if else case, setting the snakeInput to whatever
	# direction is inputted, also check if input causes snake to move backwards
	InputUp:
		la	t0, previousSnakeInput
		lw	t0, 0(t0) # Get previous snake input
		addi	t0, t0, -2 # Check if we're currently going down
		beqz	t0, endKeyboard # If yes, then invalid input
		# Else it is valid, set t1 to up
		li	t1, 1
		jal	zero, validatedSnake
	InputDown:
		la	t0, previousSnakeInput
		lw	t0, 0(t0) # Get previous snake input
		addi	t0, t0, -1 # Check if we're currently going up
		beqz	t0, endKeyboard # If yes, then invalid input
		# Else it is valid, set t1 to down
		li	t1, 2
		jal	zero, validatedSnake
	InputLeft:
		la	t0, previousSnakeInput
		lw	t0, 0(t0) # Get previous snake input
		addi	t0, t0, -4 # Check if we're currently going right
		beqz	t0, endKeyboard # If yes, then invalid input
		# Else it is valid, set t1 to left
		li	t1, 3
		jal	zero, validatedSnake
	InputRight:
		la	t0, previousSnakeInput
		lw	t0, 0(t0) # Get previous snake input
		addi	t0, t0, -3 # Check if we're currently going left
		beqz	t0, endKeyboard # If yes, then invalid input
		# Else it is valid, set t1 to right
		li	t1, 4
		jal	zero, validatedSnake
	validatedSnake:
		# Set value of snakeInput = direction inputted
		la	t0, snakeInput
		sw	t1, 0(t0)
		jal	zero, endKeyboard
	
	endKeyboard:
		# Re-enable keyboard interrupt and end keyboard handler
		li	t0, 0xFFFF0000
		li	t1, 2
		sw	t1, 0(t0)
		jal	zero, endHandler

endHandler:
	# Never used a0, still holds correct address
	lw	t0, 8(a0) # t0 <- USERa0
	csrw	t0, 0x040 # uscratch <- USERa0
	
	# load registers used in handler EXCEPT a0
	lw	t0, 0(a0)
	lw	t1, 4(a0)
	
	# swap uscratch and a0
	csrrw	a0, 0x040, a0 # USERa0, uscratch <- address of trap data
	uret # return to EPC

handlerTerminate:
	# Print error msg before terminating
	li     a7, 4
	la     a0, INTERRUPT_ERROR
	ecall
	li     a7, 34
	csrrci a0, 66, 0
	ecall
	li     a7, 4
	la     a0, INSTRUCTION_ERROR
	ecall
	li     a7, 34
	csrrci a0, 65, 0
	ecall
handlerQuit:
	li     a7, 10
	ecall	# End of program


#---------------------------------------------------------------------------------------------
# printAllWalls
#
# Subroutine description: This subroutine prints all the walls within which the snake moves
# 
#   Args:
#  		None
#
# Register Usage
#      s0: the current row
#      s1: the end row
#
# Return Values:
#	None
#---------------------------------------------------------------------------------------------
printAllWalls:
	# Stack
	addi   sp, sp, -12
	sw     ra, 0(sp)
	sw     s0, 4(sp)
	sw     s1, 8(sp)
	# print the top wall
	li     a0, 21
	li     a1, 0
	li     a2, 0
	la     a3, Brick
	lbu    a3, 0(a3)
	jal    ra, printMultipleSameChars

	li     s0, 1	# s0 <- startRow
	li     s1, 10	# s1 <- endRow
printAllWallsLoop:
	bge    s0, s1, printAllWallsLoopEnd
	# print the first brick
	la     a0, Brick	# a0 <- address(Brick)
	lbu    a0, 0(a0)	# a0 <- '#'
	mv     a1, s0		# a1 <- row
	li     a2, 0		# a2 <- col
	jal    ra, printChar
	# print the second brick
	la     a0, Brick
	lbu    a0, 0(a0)
	mv     a1, s0
	li     a2, 20
	jal    ra, printChar
	
	addi   s0, s0, 1
	jal    zero, printAllWallsLoop

printAllWallsLoopEnd:
	# print the bottom wall
	li     a0, 21
	li     a1, 10
	li     a2, 0
	la     a3, Brick
	lbu    a3, 0(a3)
	jal    ra, printMultipleSameChars

	# Unstack
	lw     ra, 0(sp)
	lw     s0, 4(sp)
	lw     s1, 8(sp)
	addi   sp, sp, 12
	jalr   zero, ra, 0


#---------------------------------------------------------------------------------------------
# printMultipleSameChars
# 
# Subroutine description: This subroutine prints white spaces in the Keyboard and Display MMIO Simulator terminal at the
# given row and column.
# 
#   Args:
#   a0: length of the chars
# 	a1: row - The row to print on.
# 	a2: col - The column to start printing on.
#   a3: char to print
#
# Register Usage
#      s0: the remaining number of cahrs
#      s1: the current row
#      s2: the current column
#      s3: the char to be printed
#
# Return Values:
#	None
#---------------------------------------------------------------------------------------------
printMultipleSameChars:
	# Stack
	addi   sp, sp, -20
	sw     ra, 0(sp)
	sw     s0, 4(sp)
	sw     s1, 8(sp)
	sw     s2, 12(sp)
	sw     s3, 16(sp)

	mv     s0, a0
	mv     s1, a1
	mv     s2, a2
	mv     s3, a3

# the loop for printing the chars
printMultipleSameCharsLoop:
	beq    s0, zero, printMultipleSameCharsLoopEnd   # branch if there's no remaining white space to print
	# Print character
	mv     a0, s3	# a0 <- char
	mv     a1, s1	# a1 <- row
	mv     a2, s2	# a2 <- col
	jal    ra, printChar
		
	addi   s0, s0, -1	# s0--
	addi   s2, s2, 1	# col++
	jal    zero, printMultipleSameCharsLoop

# All the printing chars work is done
printMultipleSameCharsLoopEnd:	
	# Unstack
	lw     ra, 0(sp)
	lw     s0, 4(sp)
	lw     s1, 8(sp)
	lw     s2, 12(sp)
	lw     s3, 16(sp)
	addi   sp, sp, 20
	jalr   zero, ra, 0


#------------------------------------------------------------------------------
# printStr
#
# Subroutine description: Prints a string in the Keyboard and Display MMIO Simulator terminal at the
# given row and column.
#
# Args:
# 	a0: strAddr - The address of the null-terminated string to be printed.
# 	a1: row - The row to print on.
# 	a2: col - The column to start printing on.
#
# Register Usage
#      s0: The address of the string to be printed.
#      s1: The current row
#      s2: The current column
#      t0: The current character
#      t1: '\n'
#
# Return Values:
#	None
#
# References: This peice of code is adjusted from displayDemo.s(Zachary Selk, Jul 18, 2019)
#------------------------------------------------------------------------------
printStr:
	# Stack
	addi   sp, sp, -16
	sw     ra, 0(sp)
	sw     s0, 4(sp)
	sw     s1, 8(sp)
	sw     s2, 12(sp)

	mv     s0, a0
	mv     s1, a1
	mv     s2, a2

# the loop for printing string
printStrLoop:
	# Check for null-character
	lb     t0, 0(s0)
	# Loop while(str[i] != '\0')
	beq    t0, zero, printStrLoopEnd

	# Print Char
	mv     a0, t0
	mv     a1, s1
	mv     a2, s2
	jal    ra, printChar

	addi   s0, s0, 1	# i++
	addi   s2, s2, 1	# col++
	jal    zero, printStrLoop

printStrLoopEnd:
	# Unstack
	lw     ra, 0(sp)
	lw     s0, 4(sp)
	lw     s1, 8(sp)
	lw     s2, 12(sp)
	addi   sp, sp, 16
	jalr   zero, ra, 0



#------------------------------------------------------------------------------
# printChar
#
# Subroutine description: Prints a single character to the Keyboard and Display MMIO Simulator terminal
# at the given row and column.
#
# Args:
# 	a0: char - The character to print
#	a1: row - The row to print the given character
#	a2: col - The column to print the given character
#
# Register Usage
#      s0: The character to be printed.
#      s1: the current row
#      s2: the current column
#      t0: Bell ascii 7
#      t1: DISPLAY_DATA
#
# Return Values:
#	None
#
# References: This peice of code is adjusted from displayDemo.s(Zachary Selk, Jul 18, 2019)
#------------------------------------------------------------------------------
printChar:
	# Stack
	addi   sp, sp, -16
	sw     ra, 0(sp)
	sw     s0, 4(sp)
	sw     s1, 8(sp)
	sw     s2, 12(sp)
	# save parameters
	mv     s0, a0
	mv     s1, a1
	mv     s2, a2

	jal    ra, waitForDisplayReady

	# Load bell and position into a register
	addi   t0, zero, 7	# Bell ascii
	slli   s1, s1, 8	# Shift row into position
	slli   s2, s2, 20	# Shift col into position
	or     t0, t0, s1
	or     t0, t0, s2	# Combine ascii, row, & col
	
	# Move cursor
	lw     t1, DISPLAY_DATA
	sw     t0, 0(t1)
	jal    waitForDisplayReady	# Wait for display before printing
	
	# Print char
	lw     t0, DISPLAY_DATA
	sw     s0, 0(t0)
	
	# Unstack
	lw     ra, 0(sp)
	lw     s0, 4(sp)
	lw     s1, 8(sp)
	lw     s2, 12(sp)
	addi   sp, sp, 16
	jalr   zero, ra, 0



#------------------------------------------------------------------------------
# waitForDisplayReady
#
# Subroutine description: A method that will check if the Keyboard and Display MMIO Simulator terminal
# can be writen to, busy-waiting until it can.
#
# Args:
# 	None
#
# Register Usage
#      t0: used for DISPLAY_CONTROL
#
# Return Values:
#	None
#
# References: This peice of code is adjusted from displayDemo.s(Zachary Selk, Jul 18, 2019)
#------------------------------------------------------------------------------
waitForDisplayReady:
	# Loop while display ready bit is zero
	lw     t0, DISPLAY_CONTROL
	lw     t0, 0(t0)
	andi   t0, t0, 1
	beq    t0, zero, waitForDisplayReady
	jalr   zero, ra, 0
