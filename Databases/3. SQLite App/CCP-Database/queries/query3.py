'''
Contains the necessary files for query 3
Author: Ryan
Date: March 15, 2024
'''

import sqlite3


def search(keyword, db_path, offset):
    '''This Function handels the actual querying
        It will search the books table and return all matches
        Offset will offset the results, giving us the 5 page pagnation for BONUS MARKS!!!!!
        Returns: The results of the query'''
    conn = sqlite3.connect(db_path)
    c = conn.cursor()

    # Construct the query and parameters
    query = """WITH search_cte(book_id, title, author, pyear, ave_rating, available) AS (
                SELECT k.book_id, title, author, pyear, ifnull(AVG(rating), 0),
                CASE WHEN b.book_id = NULL THEN 'Borrowed' ELSE 'Available' END
                FROM books k LEFT JOIN reviews r ON k.book_id = r.book_id LEFT JOIN borrowings b ON b.book_id = k.book_id
                GROUP BY k.book_id, author)

                SELECT * FROM (SELECT * FROM search_cte ct1 WHERE ct1.title LIKE :keyword ORDER BY title)
                UNION
                SELECT * FROM (SELECT * FROM search_cte ct2 WHERE ct2.author LIKE :keyword ORDER BY author)

                LIMIT 5 OFFSET :offset;"""
    parameters = {'keyword': '%' + keyword + '%', 'offset': offset}
    # Save result from query
    result = c.execute(query, parameters).fetchall()

    # Close the cursor before returning
    c.close()
    return result


def borrow_book(db_path, book_id, member):
    '''Checks if book is available to borrow
        adds(insert) the appropriate tuple into the database
        (book_id, title, Author, pyear, ave_rating, Available)'''
    conn = sqlite3.connect(db_path)
    c = conn.cursor()

    # Get the current date for start_date, formatted like sqlite
    c.execute("SELECT date('now');")
    current_date = c.fetchone()
    current_date = current_date[0] # Goofy ah Tuple

    # Insert all values, leaving primary key bid will auto generate a new value
    insert = """INSERT INTO borrowings(member, book_id, start_date, end_date) VALUES(:member, :book_id, :start_date, NULL);"""
    values = {'member': member, 'book_id': book_id, 'start_date': current_date }
    c.execute(insert, values)
    # Commit change, close cursor
    conn.commit()
    c.close()
    print("Borrowing Successful!\n")


def format_result(query_result, page_number):
    '''Cleans up the output from the query for viewing in the terminal
        Returns: A formatted string for printing'''
    print_header = "\n|=======================|\n|Search Results (Page {:})|\n|=======================|\n\n| Book Id |         Title          |         Author         | Year | Rating | Availability |\n--------------------------------------------------------------------------------------------\n".format(page_number)

    # Append results to print_header
    for query in query_result:
        print_header += ("|{:>8} |{:>23} |{:>23} |{:>5} |{:>7} |{:^13} |\n".format(query[0], query[1], query[2], query[3], query[4], query[5]))
    print_header += "============================================================================================"

    return print_header


def mainQuery3(user, db_path):
    '''Main function where it's called from main.py
        user is the name of the logged in user. Operate only on this user
        return: a formatted string to be printed in main'''

    print("=========================\nSearch by Title or Author\n=========================")

    while(True):
        # Get input, quit if user enters q
        inp = input("(Enter your search or '!q' to exit)> ").lower()

        if inp == '!q':
            print("Returning to The Main Screen")
            return

        # Else; Query the search and print out the result
        page_number = 0 # Used for tracking pages
        list_of_results = [] # Save all the results into a list
        result = search(inp, db_path, page_number * 5)
        list_of_results.append(result)

        if len(result) == 0:
            print("NO RESULT, PLEASE TRY AGAIN\n")
            continue

        # Else we enter the print loop, keep asking if they want to move to the next page or not
        next_page = True
        while(next_page):
            page_number += 1 # Move on to next page and get query
            print(format_result(result, page_number))

            valid_input = False
            while(not valid_input):
                inp = input("Do you want to see the next page? ([Y]/N)> ").lower()
                valid_input = (inp in ['y', '', 'n']) # Basic input validation, default is yes
                if inp == 'n':
                    print("End of Results\n")
                    next_page = False # Move onto the the borrow section
                if not valid_input: print("Invalid Input. ")

            # Get the next results, check if we've reached the end of the search
            # Printing results is done at the top of the loop
            if (len(result) != 5): # we reached the end of our query
                print("End of Results\n")
                next_page = False

            else:
                result = search(inp, db_path, page_number * 5) # Finally rerun the query again
                list_of_results.append(result)

                if len(result) == 0:
                    print("End of Results\n")
                    next_page = False


        # Ask if they want to borrow a book, execute the borrow if they enter a int
        valid_input = False
        while(not valid_input):
            borrow = input("Do you want to borrow a book? (Enter Book Id to borrow, !s to return to search, or !q to quit)> ").lower()

            if borrow == '!q':
                print("Returning to The Main Screen")
                return

            elif borrow == '!s':
                print("Back to Search ")
                valid_input = True # Exit the while loop back to searching

            # Check if input is valid
            elif borrow.isnumeric():
                book_id = int(borrow)

                # Go through all the results, find the instance with the book_id
                for result in list_of_results:
                    for row in result:
                        if row[0] == book_id and row[5] == "Available":
                            borrow_book(db_path, book_id, user) # Send the row to be added into the borrowings table
                            valid_input = True # Exit the while loop back to searching

                # Else Book_id D.N.E. or is not available to be borrowed
                if (not valid_input): print("Invald Book_ID. it is either not on the list or unavailable\n")

            else:
                print("Invalid input, please try again\n")
