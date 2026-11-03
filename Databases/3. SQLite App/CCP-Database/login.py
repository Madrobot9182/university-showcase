'''
This file handles logins/logouts, and registering new accounts
Crenditials are validated, and SQL attacks (hopefully) mitigated
Author: Ryan, Marko, Albert, Momo
Date: March 5, 2024
'''
import getpass 
import sqlite3


def login_screen(db_path):
    ''' This function is called by main to display the login login_screen
    and ask for credentials or register a new user.
    Give an option to quit the program, return False if they choose to quit
    Return: the name of the user who logged in.'''
    conn = sqlite3.connect(db_path)
    c = conn.cursor() # Use this for querying later on
    # Begin login
    print("Login or Signup? \nPlease press 1 to log in, or 0 to sign up.") # Choose to log in or sign up
    selected = input()
    loginSignupFlag = True
    while (loginSignupFlag):
        if selected not in ['0', '1']:
            selected = input("Invalid Input\nPlease try again.\n")
        else:
            loginSignupFlag = False
        
   # Restrict inpiut if we want

    # End restrict input
    # If statements for whether they want to log in or sign up. 1 = login, 0 = signup.
    if (int(selected) == 1): # They are logging in
        while(True):
            gmail = input("Enter gmail: ")
            gmail = gmail.lower()
            password = getpass.getpass("password: ")
            c.execute('SELECT email, passwd FROM members WHERE members.email= :username AND members.passwd = :passwd', {"username":gmail, "passwd": password })
            row = c.fetchall()

            # Non empty, we got a result
            if (row):
                return gmail
            else:
                print("Email or password incorrect, please try again")

    elif (int(selected) == 0): # They are signing up
        print("Sign up") # Sign up screen begins here
       
        c.execute('SELECT email FROM members')
        row = c.fetchall()
        gmail = input("Enter gmail: ")
        while (gmail == '' or '@' not in gmail or gmail in row):
            if (gmail in row):
                print("email in use")
            else:
                print("Invalid Email")
            gmail = input("Enter gmail: ")
        password = getpass.getpass("password: ")
        while (password == ''):
            print("Invalid password")
            password = getpass.getpass("Enter password: ")
        
        name = input("Name: ")
        while (name == ''):
            print("Invalid name")
            name = input("Enter name")

        birthyear = input("Birth year: ")
        while (birthyear == '' or not birthyear.isnumeric()):
            print("Invalid birth year")
            birthyear = input("Enter birth year")

        faculty = input("Faculty: ")
        while (faculty == ''):
            print("Invalid faculty")
            faculty = input("Enter faculty")
        c.execute("INSERT INTO members (email, passwd, name, byear, faculty) VALUES (?, ?, ?, ?, ?);", (gmail, password, name, birthyear, faculty),)

        # Commit registering
        conn.commit()
        print("Registeration successful!")
        return gmail
