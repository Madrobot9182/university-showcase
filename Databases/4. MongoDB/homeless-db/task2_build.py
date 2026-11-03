'''
Description: Constructs the MP2Embd database according to the lab manual.
Takes port number as commandline input. Embed the sender json into the messages json, then insert into mongodb

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
    client.drop_database("MP2Embd") # First drop the database if it exist
    db = client["MP2Embd"]

    # Create the collections, first drop any existing collections
    current_collections = db.list_collection_names()
    if "messages" in current_collections:
        db["messages"].drop()

    # Then makes the collections
    Msg_Collection = db["messages"]

    # Opening the json file
    start_timer = default_timer()

    # First build a list of python dict from sender json
    with open('senders.json', 'r') as file:
        sender_json_list = json.load(file) # Easy load the whole file

    with open('messages.json', 'r') as file:
            file.readline() # Remove the first line (opening bracket)
            list_of_lines = []
            end_of_file = False

            while(not end_of_file):
                for _ in range(5000): # Load in chunks of 5000
                    line = file.readline().strip(",\n") # Read a line, trim off trailing newlines and comma
                    if (line == ']' or line == ''):
                        end_of_file = True # Let the closing bracket indicate end of file
                        break

                    # Embed sender_info by sender_id to sender of message
                    json_line = json.loads(line) # Convert line to python dict
                    for sender_json in sender_json_list:
                        if sender_json["sender_id"] == json_line["sender"]:
                            json_line["sender_info"] = sender_json # Add the sender_info entry
                            break # Exit the search

                    # append embeded json to list
                    list_of_lines.append(json_line) # Append lines one by one

                # End of the 5000 loop, insert chunk into collection
                if (len(list_of_lines) != 0): # Incase of empty final result
                    Msg_Collection.insert_many(list_of_lines) # Load the list into database
                    list_of_lines = [] # Empty the list for next loop


    end_timer = default_timer() - start_timer

    # print the time taken for insert
    print("Time for the messages_embed collection: " + str(end_timer) + " Seconds")

    return 0

if __name__ == "__main__":
    main()
