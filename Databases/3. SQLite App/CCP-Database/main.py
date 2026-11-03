'''
I love documentation guys~
'''
# Import the other files
import os.path
import login as login
from queries.query1 import mainQuery1
from queries.query2 import mainQuery2
from queries.query3 import mainQuery3
from queries.query4 import mainQuery4

MAIN_MENU = "\nWelcome to the library! Please choose one of the options below (1-4, q, l):\n|----------------------------------|\n| (Enter 1) To view your profile   |\n| (Enter 2) To return books        |\n| (Enter 3) To search for books    |\n| (Enter 4) To pay fees            |\n| (Enter q) To quit                |\n| (Enter l) To logout              |\n|----------------------------------|"


def set_main_menu():
    '''This function displays the main menu and gets the users input
    Return: the user's input (1-4, q, l)'''
    while(True):
        inp = input(MAIN_MENU).lower()

        if inp not in ('q', 'l', '1', '2', '3', '4'):
            print("Invalid Input")
        else:
            return inp

def get_database_path():
    '''Returns the path of the database file'''
    # Get the parent directory path
    parent_dir = os.path.dirname(os.path.abspath(__file__))

    # join path to the database file
    db_path = os.path.join(parent_dir, "database", "test_database.db")
    return db_path

def main():
    user_logged_in = False # The name of the logged in user, false if we quit the program
    db_path = get_database_path() # The path to the database

    while(True): # Infinitely run the program until they quit
        # Start of program, ask the user to login

        '''This function will call the login screen
        Return the name of the logged in user, else if is false and program will terminate'''
        user_logged_in = login.login_screen(db_path)

        # If user quit's the program
        if (not user_logged_in):
            print("Exiting the Program")
            return 0

        # Else we ask for queries, stay logged in until they logout/quit
        while(True):
            # Display menu, get input
            user_input = set_main_menu()

            if (user_input == 'q'):
                print("Exiting the Program")
                return 0
            elif user_input == 'l':
                print("Logging out")
                break;

            # Call the corrosponding query function to the input
            user_input = int(user_input) # Convert to int
            if(user_input == 1): mainQuery1(user_logged_in, db_path)
            elif(user_input == 2): mainQuery2(user_logged_in, db_path)
            elif(user_input == 3): mainQuery3(user_logged_in, db_path)
            elif(user_input == 4): mainQuery4(user_logged_in, db_path)

    return 0 # Program terminated

if __name__ == "__main__":
    main()
