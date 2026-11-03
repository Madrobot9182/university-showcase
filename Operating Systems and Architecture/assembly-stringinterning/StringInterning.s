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
# Lecture Section:      
# Instructor:           J. Nelson Amaral
# Lab Section:          
# Teaching Assistant:   
#---------------------------------------------------------------
#
.data
	HashTable:	.space	512
	internArray:	.space	512

	eotFlag:	.word	0
	
.text

#-------------------------------------------------------------------------------
# internFile
#   This function parses a file and interns all the words inside of it
#
# input:
# 	a0: address of start of file
# return:
# 	a0: an array of identifiers for the file
#	a1: the number of elements in the array
# registers:
#	s0: A copy of the file address for extracting words between spaces
#	s1: A second copy of the file address for copying into memory
# 	s2: Counter for a1, also index for array length
#	s3: Temp counter for parseLoop, contains length of string
#-------------------------------------------------------------------------------	
internFile:
	# Stack
	addi	sp, sp, -20
	sw	s0, 0(sp) # Address for file
	sw	s1, 4(sp) # Copy of Address for file
	sw	s2, 8(sp) # internArray counter to 0
	sw	s3, 12(sp) # parseLoop Counter (Length of string)
	sw	ra, 16(sp)
	
	mv	s0, a0 # Save the file address in s0
	mv	s1, a0 # Save a copy in s1
	
	li	s2, 0 # Set internArray counter to 0
parseLine:
	li	s3, 0 # parseLoop Counter (+1 for end string char \0)
	
	parseLeadingSpace:
	# Parse out any spaces at the beginning of the file
		lbu	t1, 0(s0) # Load current char
		li	t2, 0x20 # Space
		beq	t1, t2, parseLeadingSpaceIncrement
		li	t2, 0x0A # Line feed
		beq	t1, t2, parseLeadingSpaceIncrement
		jal	zero, parseLineLoop # Else Actually start the parser
	parseLeadingSpaceIncrement:
		# Increment string at s0/s1, jump back to parseLeadingSpace
		addi	s0, s0, 1
		addi	s1, s1, 1 # Ensure the copy at s1 is same
		jal	zero, parseLeadingSpace
	
	parseLineLoop:	
		lbu	t1, 0(s0) # Load current char
		# Check for end of line chars
		li	t2, 0x20 # Space sad
		beq	t1, t2, parseLineStrip
		li	t2, 0x0A # Line feed
		beq	t1, t2, parseLineStrip	
		li	t2, 0x04 # EOT
		beq	t1, t2, eotFileLine # Need a flag for one more intern then quit?
		
		# Else we increment and loop again
		addi	s0, s0, 1 # Move on to next char
		addi	s3, s3, 1 # Increment loop counter
		jal	zero, parseLineLoop
	
	eotFileLine: # ?\0 or !\0, have to handel this case
	# Assuming we reached the end of the file, but still need to process the last string
	la	t0, eotFlag
	li	t1, 1
	sw	t1, 0(t0) # Set the eotFlag to 1
	jal	zero, parseLineStrip
	
	parseLineStrip:
	# Strip out (move past) any excess spaces/linefeed
		addi	s0, s0, 1 # Set's up s0 for next string
		lb	t1, 0(s0)
		li	t2, 0x20 # Space
		beq	t1, t2, parseLineStrip
		li	t2, 0x0A # Line feed
		beq	t1, t2, parseLineStrip
		# incase if there's trailing spaces before end of file
		li	t2, 0x04 # EOT
		beq	t1, t2, eotFileLine
		
		# Else we continue to parseLineCopy
		jal	zero, parseLineCopy
		
	parseLineCopy:
	# Copy the string into a0 for internString
		# Allocate space
		li	a7, 9
		mv	a0, s3 # Allocate as many bytes as length of this word (+1 for \n)
		addi	a0, a0, 1 # Plus one for \0 char
		ecall
		
		mv	t0, s3 # For parseLineCopyLoop counter
		addi	t0, t0, -1 # len of string - 1 (add \0 char post-loop)
	parseLineCopyLoop:
	# Use the copy of the string from s1 to copy over
		lb	t2, 0(s1) # Char of string
		sb	t2, 0(a0) # Store into allocated space
		addi	s1, s1, 1 # Increment both to next address
		addi	a0, a0, 1
		
		addi	t0, t0, -1
		bgez	t0, parseLineCopyLoop # Loop through all chars

	parseLineCopyLoopDone:
		# Tack on the \0 char
		li	t2, 0x00
		sb	t2, 0(a0)
		sub	a0, a0, s3 # Move address back to start of string
	
		mv	s1, s0 	# If we stripped multiple whitespaces, ensure both copies are equal
		jal	zero, parseLineDone # Now we are ready to intern the string
	
parseLineDone:
	jal	ra, internString # Intern the string saved in a0
	# Append to array (s1)
	la	t0, internArray # Get array
	slli	t1, s2, 2 # s2 is length of array
	add	t0, t0, t1 # ReturnArray[s3]
	sw	a0, 0(t0) # Append to end of array
	
	# Increment counter
	addi	s2, s2, 1
	
	# Check if we are at end of file, loop if not
	la	t0, eotFlag
	lw	t0, 0(t0) # 0 (Keep going) or 1 (at end)
	beqz	t0, parseLine # Loop again
	jal	zero, internFileDone # Else we are done
	
internFileDone:
	# Reset the eotFlag for next time
	la	t0, eotFlag
	li	t1, 0
	sw	t1, 0(t0)
	
	# Move s1 and s2 into a-reg for return
	la	a0, internArray
	mv	a1, s2
	
	lw	s0, 0(sp)
	lw	s1, 4(sp)
	lw	s2, 8(sp)
	lw	s3, 12(sp)
	lw	ra, 16(sp)
	addi	sp, sp, 20
	ret

#-------------------------------------------------------------------------------
# internString
#   This function interns a string into a unique identifier and stores it into a hashtable
#
# input:
# 	a0: address of string to read
# return:
# 	a0: A unique identifier that "points" to the string stored in the hashtable 
# registers:
#	s0: copy of the string address
#	s1: stores the hash value
# 	s2: holds the address of the hashtable at index of hash value
#	s3: counter for moving through the linked list
#	s4: temporary variable when dynamically allocating memory
#-------------------------------------------------------------------------------	
internString:
	# Stack
	addi	sp, sp, -24
	sw	s0, 0(sp) # Copy of string address
	sw	s1, 4(sp) # Hash Value
	sw	s2, 8(sp) # Hashtable[Hash Value]
	sw	s3, 12(sp) # occupied list loop counter
	sw	s4, 16(sp) # occupied list temp var for navigating list
	sw	ra, 20(sp)
	
	mv	s0, a0 # Copy the string value in s0
	li	s1, 0 # Hash value start at 0
	
hashString:
	lb	t0, 0(s0) # Get char (ascii value)
	addi	s0, s0, 1 # Move on to next char
	beqz	t0, hashDone # If end of string, we're done
	
	# Hashing algorithm (s1 = hash value, 128 hardcoded)
	add	s1, s1, t0 # hash + char
	li	t1, 128
	remu	s1, s1, t1 # s1 = s1 % 128
	
	jal	zero, hashString # Go through all characters
	
hashDone:
	mv	s0, a0 # Move address of s0 back to start of sting
	# Use the given hash as an index
	slli	t0, s1, 2 # Adjust for word size
	la	a1, HashTable
	
	add	s2, a1, t0 # HashTable[hash]
	lw	t3, 0(s2) # Value = HashTable[hash]
	# Now Check for the 3 cases
	beqz	t3, newHashEntry # Table is empty
	
	li	t2, 1
	slli	t2, t2, 31 # Mask for the 31th bit 
	and	t3, t3, t2 # t3 has either 10000... or 0000...
	
	beqz	t3, newLinkedList # A String present
	jal	zero, occupiedTable # Linked list present
	
newHashEntry:
# From an empty table entry we just insert the address of the string 
	sw	s0, 0(s2) # *String -> &HashTable[hash]
	jal	zero, internStringDone # identifier = hash

newLinkedList:
# From a single string we create a new linked list
# Put the list address in the hash table, move the original string as element 1
# Then call occupiedTable to insert the string into the end
	# First check if current string is equal
	lw	a0, 0(s2) # Move current string in a0
	mv	a1, s0 # Move new string in a1
	jal	ra, stringCompare
	li	t1, 1
	beq	a0, t1, internStringDone # They are equal (1) and so we return identifier
	
	li	a7, 9
	li	a0, 8 # two words, {address of next node | String value}
	ecall
	
	lw	a1, 0(s2) # the original string value
	li	t0, 1
	slli	t0, t0, 31 # bit 31 = 1
	add	t0, a0, t0 # Address with bit 31 = 1
	sw	t0, 0(s2) # Set hashtable element to (modified) address of linked list
	
	# a0 contains actual address of linked list
	li	t1, 0x00 # Null
	sw	t1, 0(a0) # Set next node address = NULL
	sw	a1, 4(a0) # Set original string into link address 
	jal	zero, occupiedTable # Let this function append the new string
	
occupiedTable:
# From a linked list we append to the list and insert the string
	lw	s4, 0(s2) # The current linked list
	slli	s4, s4, 1 # Shift away the 31th bit = set it to zero
	srli	s4, s4, 1 
		
	li	s3, 0 # Counter for how deep we go into the list
	occupiedTableLoop:
	# Search through the entire list until we reach the last node
		# Check if current string is in the linked list
		lw	a0, 4(s4) # Move current string in a0
		mv	a1, s0 # Move new string in a1
		jal	ra, stringCompare
		li	t1, 1
		# Skip straight to making the identifier
		beq	a0, t1, occupiedTableIdentifier # They are equal {1} and so we return identifier
		
		# Else we move onto the next element
		addi	s3, s3, 1 # Increment counter
		mv	t4, s4 # Save a copy of the previous node
		lw	s4, 0(s4) # Move on to the next node
		bnez	s4, occupiedTableLoop # If node not null continue
	# Now at the end, t4 contains last valid node, connect the nodes accordingly
	li	a7, 9
	li	a0, 8 # two words, {address of next node | String value}
	ecall
	
	sw	a0, 0(t4) # Put new node address at end of current list
	li	t1, 0x00 # Null
	sw	t1, 0(a0) # Set next node address = NULL
	sw	s0, 4(a0) # Set value to new string

occupiedTableIdentifier:
	# Create the identifier in s2, shift left s3 by len(list) 
	slli	s3, s3, 8
	or	s1, s1, s3 # s1 contains the identifier
	
	jal	zero, internStringDone # We're done
	
internStringDone:
	# Restore stack and exit
	mv	a0, s1 # String identifier
	lw	s0, 0(sp) # Copy of string address
	lw	s1, 4(sp) # Hash Value
	lw	s2, 8(sp) # Hashtable[Hash Value] and unique identifier
	lw	s3, 12(sp) # occupied list loop counter
	lw	s4, 16(sp) # occupied list temp var for navigating list
	lw	ra, 20(sp)
	addi	sp, sp, 24
	ret

#-------------------------------------------------------------------------------
# getInternedString
#   This function retrives the string using it's unique identifier
#
# input:
# 	a0: unique identifier for a string
# return:
# 	a0: the address of the string. Returns 0 if string was not previously interned
#-------------------------------------------------------------------------------	
getInternedString:	
	# 128 occupies lower 8 bits, let linked list index occupy upper 24 bits
	andi	a1, a0, 0xFFFFFF00 # a1 holds upper 24 bits (index of linked list)
	srli	a1, a1, 8 # Shift back and convert into usable number
	andi	a2, a0, 0xFF # a2 has lower 8 bits = hashtable index
	
	# Access hashtable, check if link list index valid, return (string) address if valid
	la	a3, HashTable
	slli	a2, a2, 2 # Adjust for word size
	add	a3, a3, a2 # Hashtable[hash#]
	
	# Check if linked list exist, the 3 cases
	lw	t0, 0(a3)
	beqz	t0, getInternEmpty
	
	li	t1, 1 
	slli	t1, t1, 31 # Mask for the 31th bit
	and	t2, t0, t1 # t2 has either 10000... or 0000...
	
	beqz	t2, getInternString # A String present
	jal	zero, getInternList # Linked list present
	
getInternEmpty:
	# Nothing there, return 0
	li	a0, 0
	jal	zero, getInternedDone
	
getInternString:
	# the value is the string address we need
	mv	a0, t0 
	jal	zero, getInternedDone
	
getInternList:
	# The string is in the linked list.
	slli	t0, t0, 1 # Address of first node with extra 1 bit
	srli	t0, t0, 1 # Shift away the 31th bit = set it to zero
	getInternListLoop:
		# a1 contains index of linked list
		beqz	a1, getInternListLoopDone # Reached appropriate node
		lw	t0, 0(t0) # Get next node
		beqz	t0, getInternListLoopEOT # Hit the end of the linked list
		addi	a1, a1, -1 # Else we move to next node
		jal	zero, getInternListLoop
		
	getInternListLoopEOT:	
		# Case where we link list past it's actual length
		li	a0, 0 # Not exist
		jal	zero, getInternedDone
	getInternListLoopDone:
		# t0 contains correct node
		lw	a0, 4(t0) # Retrive string address
		jal	zero, getInternedDone
getInternedDone:
	ret

#-------------------------------------------------------------------------------
# stringCompare
#   This function compares if two strings are equal
#
# input:
# 	a0: the first string to compare
#	a1: the second string to compare
# return:
# 	a0: 1 if equal, 0 if not
#-------------------------------------------------------------------------------	
stringCompare:
	# Load Char's move pointer by one
	lb	t0, 0(a0)
	lb	t1, 0(a1)
	addi	a0, a0, 1
	addi	a1, a1, 1
	
	bne	t0, t1, stringNotEqual
	beqz	t0, stringisEqual # t0 = t1 = end of string
	jal	zero, stringCompare # Continue loop
stringNotEqual:
	# Strings are not equal
	li	a0, 0
	ret
stringisEqual:
	# String all all equal up to the /0
	li	a0, 1
	ret
