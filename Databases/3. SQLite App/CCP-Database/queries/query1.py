'''
Contains the necessary functions for query 1
Get any additional inputs, return a formatted string for printing
Author: Marko, Ryan
Date: March 5, 2024
'''


import sqlite3

def set_member_profile_menu():
    '''This function displays the main menu and gets the users input
    Return: the user's input (1-3, q)'''
    profile_query = "\nPlease choose one of the options below (1-3, q):\n|----------------------------------|\n| (Enter 1) To view personal info  |\n| (Enter 2) To view all borrowings |\n| (Enter 3) To view penalties      |\n| (Enter q) To quit                |\n|----------------------------------|\n\n"
    while(True):
        inp = input(profile_query)

        if inp not in ('1', '2', '3', 'q'):
            print("Invalid Input")
        else:
            return inp

def query_profile_info(user_email, c):
    # query
    c.execute("SELECT members.name, members.email, members.byear FROM members WHERE members.email =:email", {"email":user_email})
    
    # get info from query
    rows=c.fetchall()
    
    # organize info
    name = rows[0][0]
    email = rows[0][1]
    birth_year = rows[0][2]
    
    # calc width of border based on the longest str
    box_width = max(len(name), len(email)) + 12
    
    # format string
    user_info = f"\n\n\n{'-' * (box_width + 2)}\n|{'USER INFO':^{box_width}}|\n{'-' * (box_width + 2)}\n|Name: {name:<{box_width - 6}}|\n|Email: {email:<{box_width - 7}}|\n|Birth year: {birth_year:<{box_width - 12}}|\n{'-' * (box_width + 2)}"

    #print(f"\n\nUSER INFO \n Name: {rows[0][0]}\n Email: {rows[0][1]}\n Birth year: {rows[0][2]}")
    print(user_info)
    
    # stall so user can actually read the info
    input("\n(Enter anything) To continue:\n")
    return #profile_info

def query_borrowings(user_email, c):
    # query borrowed + returned
    c.execute("SELECT COUNT(*) FROM borrowings WHERE member =:email AND end_date IS NOT NULL", {"email":user_email}) # NOT NULL means that it was returned
    # store val
    num_returned = c.fetchone()
    # query current borrowings
    c.execute("SELECT COUNT(*) FROM borrowings WHERE member =:email AND end_date IS NULL", {"email":user_email}) # IS NULL means that it is still being borrowed
    # store val
    num_currently_borrowed = c.fetchone()
    # query overdue borrowings
    c.execute("SELECT COUNT(*) FROM borrowings WHERE member ='user2@example.com' AND end_date IS NULL AND julianday('now') - julianday(start_date) > 20", {"email":user_email}) # julianday arithmetic sees if the current date is more than 20 days after the day the user borrowed the book
    # store val
    num_overdue = c.fetchone()
    
    # organize info
    previous_borrowings = str(num_returned[0])
    current_borrowings = str(num_currently_borrowed[0])
    overdue_borrowings = str(num_overdue[0])
    
    # calc width of border based on the longest str
    box_width = max(len("ALL BORROWINGS"), len(previous_borrowings), len(current_borrowings), len(overdue_borrowings)) + 22
    
    # format string
    borrowings_info = f"\n\n\n{'-' * (box_width + 2)}\n|{'ALL BORROWINGS':^{box_width}}|\n{'-' * (box_width + 2)}\n|Previous borrowings: {num_returned[0]:<{box_width - 21}}|\n|Current borrowings: {num_currently_borrowed[0]:<{box_width - 20}}|\n|Overdue borrowings: {num_overdue[0]:<{box_width - 20}}|\n{'-' * (box_width + 2)}"

    # print
    print(borrowings_info)
    
    # stall so user can actually read the info
    input("\n(Enter anything) To continue:\n")
    return

def query_penalty_info(user_email, c):
    # query both number of penalties user has and total amount owed
    c.execute("SELECT COUNT(penalties.pid), SUM(penalties.amount - COALESCE(penalties.paid_amount, 0)) FROM penalties LEFT JOIN borrowings ON penalties.bid = borrowings.bid WHERE borrowings.member =:email AND (penalties.paid_amount < penalties.amount OR penalties.paid_amount IS NULL)", {"email":user_email})
    
    # store val
    penalty_info = c.fetchall()
    
    # organize and slightly format values (total owed is two dec)
    num_pen = penalty_info[0][0]
    total_owed = penalty_info[0][1]
    total_owed = f"${total_owed:.2f}"
    
    # calc width of border based on the longest str
    box_width = max(len("PENALTY INFO"), len(str(num_pen)), len(total_owed)) + 16
    
    # format string
    penalty_info = f"\n\n\n{'-' * (box_width + 2)}\n|{'PENALTY INFO':^{box_width}}|\n{'-' * (box_width + 2)}\n|Unpaid penalties: {num_pen:<{box_width - 18}}|\n|Total Owed: {total_owed:<{box_width - 12}}|\n{'-' * (box_width + 2)}"

    # print
    print(penalty_info)
    
    #stall
    input("\n(Enter anything) To continue:\n")
    return



def mainQuery1(user, db_path):
    '''Main function where it's called from main.py
        user is the name of the logged in user. Operate only on this user'''

    conn = sqlite3.connect(db_path)
    c = conn.cursor()	
    mem_prof_quit = False
    
    while (not mem_prof_quit):
        user_inp = set_member_profile_menu()
        if user_inp == '1':
            query_profile_info(user, c)
        elif user_inp == '2':
            query_borrowings(user, c)
        elif user_inp == '3':
            query_penalty_info(user, c)
        elif user_inp == 'q':
            mem_prof_quit = True
