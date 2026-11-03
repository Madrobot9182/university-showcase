'''
Contains the necessary functions for query 4
Author: Ryan, Momo
Date: March 5, 2024
'''
#TODO: 
import sqlite3

# Relative path to the database (minor jank but works)
DB_PATH = '../database/test_database.db'
def find_correct_tuple(penalties, pid):
    for tup in penalties:
        if (tup[0] == pid):
            return tup
    


def show_penalties(c, user):
    query = """SELECT pid, IFNULL(penalties.paid_amount, 0.0) AS paid_amount, (penalties.amount - IFNULL(penalties.paid_amount, 0.0)) AS owe_amount 
    FROM penalties, borrowings 
    WHERE (penalties.amount - IFNULL(penalties.paid_amount, 0.0)) > 0.0 
    AND penalties.bid = borrowings.bid 
    AND borrowings.member = ?;"""
    user_penalties = c.execute(query,(user,))
    return user_penalties.fetchall()
def pay_penalty(c, user):
    pass
def display_table(query_result):

    print_header = ""
    print_header += "==============================\n"
    print_header += ("| {:^11} |{:^13} |\n".format("Pid", "Owe Amount"))
    print_header += "------------------------------\n"
    for query in query_result:
        print_header += ("| {:>11} |{:>13.2f} |\n".format(query[0], query[2]))
    print_header += "==============================\n"
    print(print_header)

        
def mainQuery4(user, db_path):
    '''Main function where it's called from main.py
        user is the name of the logged in user. Operate only on this user
        return: a formatted string to be printed in main'''
    conn = sqlite3.connect(db_path)
    c = conn.cursor()

    user_penalties = show_penalties(c, user)
    if len(user_penalties) == 0:
        print("You have no current penalties")
        return
    display_table(user_penalties)
    all_pids = [item[0] for item in user_penalties]
    pid_condition = False
    
    while(pid_condition == False):
        pid = input("Which penalty Would you like to pay for? Enter the Penalty ID (q for quit)?: ")
        pid = pid.strip()
        if(pid == 'q'):
            return
        elif(not pid.isnumeric()): 
            print("Please Enter an integer")
        elif(int(pid) not in all_pids):
            print("Invalid penalty ID")
        else:
            pid = int(pid)
            tup = find_correct_tuple(user_penalties, pid) #gets the correct penalty 
            owe_amount = tup[2] 
            paid_amount = tup[1]
            amount_condition = False
            while(amount_condition == False):
                amount = input("how much would you like to pay? (q for quit, r to return to previous prompt): ")
                if(amount == 'r'):
                    amount_condition = True
                    continue
                elif(amount == 'q'):
                    return
                try:
                    amount = float(amount) 
                except ValueError:
                    print("Invalid input. Please enter a valid number. ")
                    continue
                if(amount <= 0):
                    print("Please enter a number greater than 0")
                elif(amount > owe_amount):
                    print("You cannot pay more than you owe")
                else: 
                    paid_amount += amount
                    update_query = """
                    UPDATE penalties
                    SET paid_amount = ?
                    WHERE pid = ?
                    """
                    c.execute(update_query, (paid_amount, pid))
                    conn.commit()
                    amount_condition = True
                    pid_condition = True
    c.close()

                    
