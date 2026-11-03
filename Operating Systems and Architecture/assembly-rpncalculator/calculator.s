#
# CMPUT 229 Student Submission License
# Version 1.0
#
# Copyright 2019 <student name>
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
#---------------------------------------------------------------
# CCID: ry7                
# Lecture Section: B01     
# Instructor: Felipe Banados Schwerter         
# Lab Section: H01         
# Teaching Assistant:   
#---------------------------------------------------------------
# 

.include "common.s"

#----------------------------------
#        STUDENT SOLUTION
#----------------------------------
calculator:
# Load next instruction
lw t0, 0(a0)
addi a0, a0, 4

# Jump if t0 is an operation code
li t1, -3
beq t0, t1, Terminate
li t1, -2
beq t0, t1, Subtract
li t1, -1
beq t0, t1 Addition

# Else make space on stack and push number in
addi a1, a1, -4
sw t0, 0(a1)
jal zero, calculator # Loop


Addition:
# Pop numbers off stack and add
lw t1, 0(a1)
addi a1, a1, 4 # To move stack ptr "back" and get next number
lw t2, 0(a1)
add t3, t2, t1

# Push result back onto stack
sw t3, 0(a1)
jal zero, calculator


Subtract:
# Pop numbers off stack and sub (similar to Addition)
lw t1, 0(a1)
addi a1, a1, 4 # To move stack ptr "back" and get next number
lw t2, 0(a1)
sub t3, t2, t1

# Push result back onto stack
sw t3, 0(a1)
jal zero, calculator


Terminate:
# Print out top int on stack
lw a0, 0(a1)
addi a7, zero, 1
ecall

# Return to common.s as specified in lab manual
jr ra