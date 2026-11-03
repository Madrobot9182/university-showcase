#
# CMPUT 229 Student Submission License
# Version 1.0
#
# Copyright 2023 <student name>
#
# Redistribution is forbidden in all circumstances. Use of this
# software without explicit authorization from the author or CMPUT 229
# Teaching Staff is prohibited.
#
# This software was produced as a solution for an assignment in the course
# CMPUT 229 - Computer Organization and Architecture I at the University of
# Alberta, Canada. This solution is confidential and remains confidential
# after it is submitted for grading.
#
# Copying any part of this solution without including this copyright notice
# is illegal.
#
# If any portion of this software is included in a solution submitted for
# grading at an educational institution, the submitter will be subject to
# the sanctions for plagiarism at that institution.
#
# If this software is found in any public website or public repository, the
# person finding it is kindly requested to immediately report, including
# the URL or other repository locating information, to the following email
# address:
#
#          cmput229@ualberta.ca
#
#------------------------------------------------------------------------------
# CCID: ry7                
# Lecture Section: B01     
# Instructor: Felipe Banados Schwerter         
# Lab Section: H01         
# Teaching Assistant:
#-----------------------------------------------------------------------------
#
.text
.include "common.s"

#-------------------------------------------------------------------------------
# digitToAscii
#   return the ASCII value of digit 
# 
# 
# input:
#	a0: a single digit represented as an integer, between 0 and 9
# return:
# 	a0: the ASCII value of the digit, between 48 (0x30) and 57 (0x39)
#-------------------------------------------------------------------------------
digitToAscii:

# --- insert your solution here ---
    addi a0, a0, 48 # digit + 48 = ascii
    ret


#-------------------------------------------------------------------------------
# copy
#   This function copies all the RGB values to another address.
# 
#
# input:
# 	a0: address of the start of RGB values (where to copy from)
# 	a1: address of the start of where to copy to
#	a2: length in words
#-------------------------------------------------------------------------------	
copy:

# --- insert your solution here ---
    li t0, 0 # loop counter
    
    copyLoop:
    # Loop Counter
    beq t0, a2, copyExit # Exit when we loop {a2} time
    addi t0, t0, 1 # index tracker
    
    # Copy from old to new address
    lw t1, 0(a0)
    sw t1, 0(a1)
    
    # Move addresses up to next word
    addi, a0, a0, 4
    addi, a1, a1, 4
    
    jal zero, copyLoop # Loop again
    
    copyExit:
    ret # exit

#-------------------------------------------------------------------------------
# asciiToDigit
#   This function returns the digit for the given ASCII value
# 
# 
# input:
#	a0: the ASCII value of the digit, between 48 (0x30) and 57 (0x39)
# return:
# 	a0: a single digit represented as an integer, between 0 and 9
#-------------------------------------------------------------------------------
asciiToDigit:

# --- insert your solution here ---
    addi a0, a0, -48 # ascii - 48 = digit
    ret


#-------------------------------------------------------------------------------
# readNextNumber
#   This function returns the next number starting from the current address.
# 
#   Details and examples are given in the instruction
#
# input:
# 	a0: current address to start reading
# return:
# 	a0: the address to start reading the next numbe
#	a1: the number
# registers:
#	s0: boolean for spaceChecker function, 1 if true, 0 otherwise
#	s1: register to store each char and run through spaceChecker
# 	s3: temp persistant register for saving the address from a0 
#	a2-a6, contains hex for all space values for comparisons
#-------------------------------------------------------------------------------		
readNextNumber:
# --- insert your solution here ---
    addi sp, sp, -16
    sw s0, 0(sp) # s0 for spaceChecker (1 is space, 0 else)
    sw s1, 4(sp) # Register to store chars for comparison
    sw s3, 8(sp) # Save address in a0
    sw ra, 12(sp)
    
    # Save address in s3
    mv s3, a0
    
    # Load all possible space character
    li a2, 0x20
    li a3, 0x0A
    li a4, 0x0D
    li a5, 0x09
    li a6, 0x00 # NULL
    # Leading Zeros will be ignore in the numGetter loop
    
    # Start of Main Function
    li a1, 0 # Ensure a1 is initally 0
    
    # Skip over all leading spaces
    spaceLoop:
    lb s1, 0(s3) # load number into s1
    addi s3, s3, 1 # Move address up by one
    jal, ra, spaceChecker # jump to spaceChecker
    bnez s0, spaceLoop # Continue looping until we found a valid char
    
    numGetter:
    mv a0, s1 # load number to a0 for conversion
    jal ra, asciiToDigit # Convert to digit
    add a1, a1, a0 # add num to a1
    
    # Move to next byte
    lb s1, 0(s3) # load number into s1
    addi s3, s3, 1 # Move address up by one
    
    jal, ra, spaceChecker # Check for space,
    bnez s0, readNumEnd # We reached terminating char, jump to end
    
    # Else loop back and repeat
    li t2, 10 
    mul a1, a1, t2 # Multiply by ten to make space
    jal zero, numGetter # Loop back and repeat
    
    spaceChecker:
    
    # Checks if the byte in a1 is a space  
    li s0, 0 # Boolean for T/F
    # Check all space chars, 
    beq s1, a2, istrue
    beq s1, a3, istrue
    beq s1, a4, istrue
    beq s1, a5, istrue
    beq s1, a6, istrue # Null character
    
    # Not a space, return 0 in s0
    jalr, zero, ra, 0
    
    # Returns s0 <- 1 if space char
    istrue:
    li s0, 1
    jalr, zero, ra, 0
    
    readNumEnd:
    mv a0, s3 # copy address back to a0
    
    # Restore Stack
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s3, 8(sp)
    lw ra, 12(sp)
    addi sp, sp, 16
    
    ret

#-------------------------------------------------------------------------------
# storeNumber
#   This function converts the integer(between 0 and 255) to 
#   ASCII and then stores their ASCII to the address. 
#   Store the leftmost digit first, consider using the function digitToAscii.
# 
# input:
#	a0: the number represented as an integer
#	a1: address for the number to be stored
# return:
# 	a0: the next address that's available to be stored
# register:
#	s0: To save a0 in persistant register
#	s1: To save a1 in persistant register
#	t0: Saves greatest significance (ie 1 for 1 digit, 10 for 2 digit, 100 for 3 digit = hundreds etc)
#-------------------------------------------------------------------------------
storeNumber:

# --- insert your solution here ---
    addi sp, sp, -12
    sw s0, 0(sp) # To save a0
    sw s1, 4(sp) # To save a1
    sw ra, 8(sp)
    
    mv s0, a0 # Save a0 in s0
    mv s1, a1 # Save a1 in s1
    li t1, 10 # for division
    
    # Determine how many digits the number has
    # Load significant (100, 10, 1) into t0, used for math in digitLoop
    li t0, 100
    bge s0, t0, digitLoop # num >= 100
    
    li t0, 10
    bge s0, t0, digitLoop  # num >= 10
   
    li t0, 1 # Else num < 10
    
    digitLoop:
    div a0, s0, t0 # a0 <- Number / t0 (Highest significance)
    
    jal ra, digitToAscii # Convert to ascii
    sb a0, 0(s1) # store char to s1 (a1)
    addi s1, s1, 1 # move to next open address
    
    # Subtract the greatest significant (left digit)
    jal ra, asciiToDigit # Convert back to digit for math
    mul a0, a0, t0 # mult by significance
    sub s0, s0, a0 # remove leftmost digit from number
    
    # Adjust t0 to new significance
    div t0, t0, t1 # significance divide by 10 (100 -> 10 -> 1 -> 0)
    bnez t0, digitLoop # Loop if number s0 (a0) != 0 (finish going through all digits)
    
    storeExit:
    mv a0, s1 # Move new address to a0

    # Restore Stack before exiting
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw ra, 8(sp)
    addi sp, sp, 12

    ret

#-------------------------------------------------------------------------------
# PixelKernelMul
#   This function calculates the average for each of the R, G, B values and stores the value.
# 
#   details and examples are given in the instruction
#
# input:
# 	a0: address of the start of RGB values, store the calculated RGB values in this region
#		(a0 is the base address)
# 	a1: address of the start of the copy of RGB values
# 	a2: row # of the current pixel to blur
# 	a3: col # of the current pixel to blur
# 	a4: total row (may not be used)
# 	a5: total col
# register:
#	s0: Holds the sum of R pixel values
#	s1: Holds the sum of G pixel values
#	s2: Holds the sum of B pixel values
#	s3: Index for outer (row) loop
#	s4: Index for inner (row) loop
#	s5: The constant 7 for making the matrix
#	s6: For saving the address of a0 in a persistant register
#	s7: For saving the address of a1 in a persistant register
#-------------------------------------------------------------------------------	
PixelKernelMul:

# --- insert your solution here ---
    
    addi sp, sp, -32
    sw s0, 0(sp) # R Value
    sw s1, 4(sp)  # G Value
    sw s2, 8(sp) # B Value
    sw s3, 12(sp) # Row Index Counter
    sw s4, 16(sp) # Column Index Counter 
    sw s5, 20(sp) # Endloop number (7)
    sw s6, 24(sp) # Save address of a0
    sw s7, 28(sp) # Save address of a1
    
    mv s6, a0 # Copy address of s6 into a0
    mv s7, a1 # Copy address of s6 into a1
    
    # Initalize sum vars to 0
    li s0, 0
    li s1, 0
    li s2, 0
    
    # Initalize a1 to be A[-3 row][-3 col]    
    # Formula: a1 +- (col# * col total/length of row) +- row #
    # Row
    addi t0, a2, -3 # row - 3
    mul t1, t0, a5 # row # * size of a row
    
    # Column (col - 3)
    addi t2, a3, -3 # col - 3
    
    # Add both correction factors together
    add t3, t1, t2
    slli t3, t3, 2 # Adjust to word size
    add s7, s7, t3 # set a1 to be address of A[-3][-3]
	
    li s5, 7 # End condition, loop 7 times
    li s3, 0 # row counter   
    
    # Outer loop for matrix rows
    RowLoop: 
    	li s4, 0  # col counter
    	# Inner loop for matrix cols
    	ColLoop:  	
	    lbu t3, 0(s7) # Load R pixel value
	    add s0, s0, t3 # sum R values to s0
	    
	    lbu t3, 1(s7) # Load G pixel value
	    add s1, s1, t3 # sum G values to s1

	    lbu t3, 2(s7) # Load B pixel value
	    add s2, s2, t3 # sum B values to s2
	    
	    addi s7, s7, 4 # Move on to next address
	    
	    # Increment col counter
	    addi s4, s4, 1
	    blt s4, s5, ColLoop # Loop until we go through 7 columns
	
	# Move to next row (col - 7) + 1 row
        addi s7, s7, -28 # Go back 7 columns
	slli t4, a5, 2 # Bytes of One row
	add s7, s7, t4 # Go up by one row
    	
    	# Increment row counter
        addi s3, s3, 1
        blt s3, s5, RowLoop # Loop until we go through 7 rows
    
    # Loops Complete, Perform Calculations:
    # Divide all values by 49
    li t4, 49
    div s0, s0, t4
    div s1, s1, t4
    div s2, s2, t4
    
    # Get Address for a0 A[row][col]
    mul t0, a2, a5 # row # * size of a row
    
    # Column = a3
    add t3, t0, a3 # Add both correction factors together
    slli t3, t3, 2 # Adjust to word size
    
    add s6, s6, t3 # set a0=s6 to be address of A[row][col]

    # Set new vales to address
    sb s0, 0(s6)
    sb s1, 1(s6)
    sb s2, 2(s6)
    
    # DONE!!!! Restore stack
    lw s0, 0(sp) # R Value
    lw s1, 4(sp)  # G Value
    lw s2, 8(sp) # B Value
    lw s3, 12(sp) # Row Index Counter
    lw s4, 16(sp) # Column Index Counter 
    lw s5, 20(sp) # Endloop number (7)
    lw s6, 24(sp) # Save address of a0
    lw s7, 28(sp) # Save address of a1
    addi sp, sp, 32
    
    ret
	
#-------------------------------------------------------------------------------
# blurImage
#   This function blurs the image using PixelKernelMul on all possible pixels.
#   It will run PixelKernelMul on all pixels except for the ones on the edges and corners.
#
#   details and pesudo code are given in the instruction
#
#
# input:
#   	a0: kernel size
#   	a1: total row
#   	a2: total col
#   	a3: address of the start of RGB values, store the calculated RGB values in this region
#   	    (a0 is the base address)
#   	a4: address of the start of the copy of RGB values
# register
#	s0: The constant 3 for the kernal radius
#	s1: Outer loop counter
#	s2: Inner loop counter
#	s3: Outer loop condition (row - ker)
#	s4: Inner loop condition (column - ker)
#	s5-s8: To move arguments a1-a4 and s1, s2 to a0-a5
#-------------------------------------------------------------------------------
blurImage:
# --- insert your solution here ---
    addi sp, sp, -40
    sw s0, 0(sp) # Kernal radius
    sw s1, 4(sp) # i
    sw s2, 8(sp) # j
    sw s3, 12(sp) # Rows - ker for loop
    sw s4, 16(sp) # Columns - ker for loop
    
    # For Pass through arguments
    sw s5, 20(sp)
    sw s6, 24(sp)
    sw s7, 28(sp)
    sw s8, 32(sp)
    sw ra, 36(sp)
    
    # Initalize kernal
    mv s0, a0 # kernal = 3
    
    # Set end conditions for loops (row/col - kernal)
    sub s3, a1, s0 # row - 3
    sub s4, a2, s0 # col - 3
    
    # For passing through to kernal multiplier
    mv s5, a1 # Row total
    mv s6, a2 # Col total
    
    mv a0, a3 # address of pic to change
    mv a1, a4 # copy of address 

    # Loop
    mv s1, s0 # Row Counter = 3
    L1:
        mv s2, s0 # Col Counter = 3
	    L2:
	    # Copy arguments, ensures they stay consistent&correct
	    mv a4, s5 # Row total
    	    mv a5, s6 # Col total
	    
    	    # Copy current Row/Col to arguments
    	    mv a2, s1 # Row num
    	    mv a3, s2 # Col num
    	    
    	    jal ra, PixelKernelMul # Call PixelKernelMul function on current row/col
    	    
    	    #j++
    	    addi s2, s2, 1 # Add 1
    	    blt s2, s4, L2 # Column loop j < 3
    # i++
    addi s1, s1, 1 # add 1
    blt s1, s3, L1 # Row Loop i < 3

    # DONE, RESTORE THE CHONK STACK
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw s3, 12(sp)
    lw s4, 16(sp)
    lw s5, 20(sp)
    lw s6, 24(sp)
    lw s7, 28(sp)
    lw s8, 32(sp)
    lw ra, 36(sp)
    addi sp, sp, 40
    
    ret
