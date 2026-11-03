'''
Description: Constructs the MP2Norm database according to the lab manual.
Takes port number as commandline input, loads and inserts the jsons into mongodb
Author: Ryan
Date: March 28, 2024

'''
import sys
from timeit import default_timer
from pymongo import MongoClient
import json

def main():
    '''
    Opens and returns the database for processing.
    Sourced from https://www.geeksforgeeks.org/how-to-import-json-file-in-mongodb-using-python/
    '''
    port = sys.argv[1] # Get port number from commandline

    # Connect to local Mongo server
    client = MongoClient(f"mongodb://localhost:{port}/")

    # Build the MP2Norm Database
    client.drop_database("MP2Norm") # First drop the database if it exist
    db = client["MP2Norm"]

    # Create the collections, first drop any existing collections
    current_collections = db.list_collection_names()
    if "messages" in current_collections:
        db["messages"].drop()
    if "senders" in current_collections:
        db["senders"].drop()

    # Then makes the collections
    Msg_Collection = db["messages"]
    Sen_Collection = db["senders"]

    # Opening the json file
    start_timer1 = default_timer()
    with open('messages.json', 'r') as file:
            file.readline() # Ignore the first line (opening bracket)
            list_of_lines = []
            end_of_file = False

            while(not end_of_file):
                for _ in range(5000): # Load in chunks of 5000
                    line = file.readline().strip(",\n") # Read a line, trim off trailing newlines and comma
                    if (line == ']' or line == ''):
                        end_of_file = True # Let the closing bracket indicate end of file
                        break
                    # Else append to list
                    list_of_lines.append(json.loads(line)) # Append lines one by one

                # End of the 5000 loop, insert chunk into collection
                if (len(list_of_lines) != 0): # Incase of empty final result
                    Msg_Collection.insert_many(list_of_lines) # Load the list into database
                    list_of_lines = [] # Empty the list for next loop
    end_timer1 = default_timer() - start_timer1

    start_timer2 = default_timer()
    with open('senders.json', 'r') as file:
        Sen_Collection.insert_many(json.load(file)) # Easy load the whole file
    end_timer2 = default_timer() - start_timer2

    # print the time taken for insert
    print("Time for the messages collection: " + str(end_timer1) + " Seconds")
    print("Time for the senders collection: " + str(end_timer2) + " Seconds")

    return 0

if __name__ == "__main__":
    main()
