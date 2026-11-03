'''
Description: answers questions given in task 2
Author: (Q1-Q2 | Marko and Albert), (Q4 | Momo), (inserts + Q3 | Marko)
Date: March 31, 2024
'''

import sys
from timeit import default_timer
from pymongo import MongoClient, errors
from pprint import pprint

def quest1(db):
    # get collection
    messages = db["messages"]
    
    # make query ($regex looks for any substring of 'ant')
    query = {"text": {"$regex": "ant"}}
    
    # try and except to handle errors
    try:
        # start timer
        start = default_timer()
        
        # count the number of ducoments (messages) that have 'ant' in it, with a time limit of 2 mins
        result = messages.count_documents(query, maxTimeMS = 120000)
        
        # end timer
        end = default_timer() - start
        
         # print output
        print("Q1:")
        pprint(result)
        print(f"Time: {end*1000} ms | {end} sec")
        print("\n")
        
    # if time exception raised (from MaxTimeMS) then handle it and go next
    except errors.ExecutionTimeout:
        print("Q1: Query timed out.")
        print("\n")
    
    

def quest2(db):
    # get collection
    messages = db["messages"]
    
    # make query (first group by sender_id and sum the amount of messages then sort by descending to get the person with the most messages sent)
    pipeline = [
        {
            "$group": {
                "_id": "$sender",
                "message_count": {"$sum": 1}
            }
        },
        {
            "$sort": {"message_count": -1}
        },
        {
            "$limit": 1
        }
    ]
    
    # try and except to handle errors
    try:
        # start timer
        start = default_timer()
        
        # count the number of ducoments (messages) that have 'ant' in it, with a time limit of 2 mins
        result = list(messages.aggregate(pipeline, maxTimeMS = 120000))

        # end timer
        end = default_timer() - start
        
        # print output
        print("Q2:")
        pprint(result)
        print(f"Time: {end*1000} in ms | {end} in sec")
        print("\n")
    
    # if time exception raised (from MaxTimeMS) then handle it and go next
    except errors.ExecutionTimeout:
        print("Q2: Query timed out.")
        print("\n")

def quest3(db):
    # get collection
    messages = db["messages"]
    senders = db["senders"]
   
    # try and except to handle errors
    try:
        # start timer
        start = default_timer()
        
        # query (just counts all documents where sender_info.credit = 0)
        query = {"sender_info.credit": 0}
        result = messages.count_documents(query, maxTimeMS=120000)
        
        # end timer
        end = default_timer() - start
        
        # print output
        print("Q3:")
        pprint(result)
        print(f"Time: {end*1000} in ms | {end} in sec")
        print("\n")
    
    # if time exception raised (from MaxTimeMS) then handle it and go next
    except errors.ExecutionTimeout:
        print("Q2: Query timed out.")
        print("\n")

def quest4(db):
    senders = db["senders"]
    #start timer
    start = default_timer()

    senders.update_many(
        {"credit" : { "$lt" : 100}}, #gets all <100 credit senders
        {"$mul": {"credit" : 2}} #doubles the credit 
    )

    # end timer
    end = default_timer() - start
    #result
    print("Q4:")
    print(f"Time: {end*1000} in ms | {end} in sec")

def main():
    port = sys.argv[1] # Get port number from commandline
    # connect to db
    client = MongoClient("localhost", int(port))
    db = client.MP2Embd
    
    # create indexes for collection
    db.messages.create_index([("sender", 1)]) # default index type for “sender”
    db.messages.create_index([("text", "text")]) # text index for “text”
    db.senders.create_index([("sender_id", 1)]) # index for "sender_id"
    
    # drop indexes for testing
    # db.messages.drop_index("sender_1")
    # db.messages.drop_index("text_text")
    # db.senders.drop_index("sender_id_1")
    
    # queries/questions
    quest1(db)
    quest2(db)
    quest3(db)
    quest4(db)

if __name__ == "__main__":
    main()
