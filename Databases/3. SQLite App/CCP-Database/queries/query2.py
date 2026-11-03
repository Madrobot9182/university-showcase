'''
Contains the necessary functions for query 2
Author: Ryan, Albert, Marko
Date: March 5, 2024
'''

import sqlite3

def validate_input_bid(borrowings_list, user_input):
    '''
    Check if the user's input is valid for what book they want to return.
    inputs: list of tuples containing users borrowings in the format bid, title, borrowing date, return date
    outputs: True if valid input, false otherwise.
    '''
    user_input = int(user_input)
    for borrowing in borrowings_list:
        if user_input in borrowing:
            return True
    return False

def validate_input_rating(rating):
    '''
    Function that checks if the rating input is valid
    inputs: user rating
    output: True if the rating is a number from 1-5, false otherwise.
    '''
    if rating >= 1 and rating <= 5:
        return True
    return False

def validate_choice(input):
    '''
    Checks if provided input is either 1 or 0.
    inputs: user choice
    output: True if choice is either 1 or 0, false otherwise.
    '''
    if input in [0, 1]:
        return True
    return False

def mainQuery2(user, db_path):
    '''Main function where it's called from main.py
        user is the name of the logged in user. Operate only on this user'''
    conn = sqlite3.connect(db_path)
    c = conn.cursor()

    # Find the list of borrowings
    c.execute("""SELECT bid, title, start_date, DATE(start_date, '+20 days') as return_date
               FROM borrowings
               INNER JOIN books ON books.book_id = borrowings.book_id
               WHERE borrowings.member =:name
               AND end_date IS NULL""", {"name": user})
    borrowings_list = c.fetchall() # Get the actual table
    print(borrowings_list)
    print("Your Borrowings:")
    # Print borrowings
    # Define the fixed length for each column
    column_length = 14
    # Construct the table header
    header = f"| {'BID':<{column_length}} | {'Book Title':<{column_length}} | {'Check-out date':<{column_length}} | {'Return deadline':<{column_length}} |\n"
    # Construct the border
    border = '-' * (4 * column_length + 13) + '\n'
    print(border + header + border, end='')
    for borrowing in borrowings_list:
        borrowing = list(borrowing)
        title = borrowing[1]
        if len(title) > 14:
            title = title[0:11]
            borrowing[1] = title + "..."

        # Format each value with the fixed length
        formatted_values = [f"{str(value):<{column_length}}" for value in borrowing]

        # Construct the row
        row = f"| {formatted_values[0]} | {formatted_values[1]} | {formatted_values[2]} | {formatted_values[3]} |\n"

        # Combine the header, border, and row
        borrowing_table = row + border

        print(borrowing_table, end='')

    # let the user select a book to return
    bid_flag = 0
    while bid_flag != 1:
        try:
            selected_return = int(input("Which book would you like to return? Please select a borrowing id.\n"))
            while not validate_input_bid(borrowings_list, selected_return):  # If the selected borrowing id doesn't exist it should fail
                selected_return = int(input("You do not own that book. Please select a valid borrowing id. \n"))
            bid_flag = 1
        except ValueError:
            # if they input a string for whatever reason it should fail
            print("Invalid value.")

    # Valid input
    if (validate_input_bid(borrowings_list, selected_return)):  # if the borrowing id does exist then we go
        c.execute("SELECT date('now');")  # Get the current date
        current_date = c.fetchone()
        current_date = current_date[0] # since it's a tuple
        # Update the table to have the new return date
        c.execute("""UPDATE borrowings
                     SET end_date = :current_date
                     WHERE borrowings.bid = :selected_return;""", {"current_date": current_date, "selected_return": selected_return})
        conn.commit() # commit the changes
        print("Book returned.")

        # Find the difference between the time it took them to return it and the time they borrowed it.
        c.execute("""SELECT julianday(end_date) - julianday(start_date)
                     FROM borrowings
                     WHERE borrowings.bid =:selected_return""", {"selected_return": selected_return})
        penalty = c.fetchone()
        penalty = penalty[0]
        # Penalize them if it has been greater than 20 days since they returned it.
        if (penalty > 20):
            penalty_cost = penalty - 20
            #Insert the penalties into the penalty table.
            c.execute("""INSERT INTO penalties(bid, amount, paid_amount)
                         VALUES(:bid_val, :amount, NULL)""", {"bid_val": selected_return, "amount": penalty_cost})
            conn.commit()
            print(f"Book returned {penalty_cost} days late. Charged a late fee of $1 for each day.")

        # End return section, start review
        review_flag = 0
        while review_flag == 0:
            try:
                review = int(input("Would you like to leave a review? Press 1 for yes, and 0 for no. "))
                while not validate_choice(review):
                    review = int(input("Please enter either 1 or 0. "))
                review_flag = 1
            except ValueError:
                print("Invalid Input, please input 1 or 0. ")

        if (review == 1):
            # They want to leave a review
            rating_flag = 0
            while rating_flag == 0:
                try:
                    rating = int(input("What rating would you give this book? Enter a number from 1-5. "))  # Rating
                    while not validate_input_rating(rating):
                        rating = int(input("Invalid rating. Please input a rating between 1-5. "))  # Make sure they have to input a valid rating
                    rating_flag = 1
                except ValueError:
                    print("Invalid input.")

            review_text = input("Comments about the book? ")  # what do they want to say about the book
            # Find a new unique RID. Do this by taking max(rid) and add 1.
            c.execute("""SELECT max(rid)
                         FROM reviews""")
            new_rid = c.fetchone()
            new_rid = new_rid[0] + 1
            # After finding new ID, insert the review.
            c.execute("""INSERT INTO reviews(rid, book_id, member, rating, rtext, rdate)
                         VALUES(:rating_id, :bid, :member, :rating, :rtext, :rdate)
                      """, {"rating_id": new_rid, "bid": selected_return, "member": user, "rating": rating, "rtext": review_text, "rdate": current_date})
            conn.commit()  # commit the change
            print("Successfully left review.")
