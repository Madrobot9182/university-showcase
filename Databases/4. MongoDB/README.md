# Group work break-down strategy

We met together in person, and communicated vis discord for coordinations. Ryan wrote the build scripts, Albert, Marko, and Momo worked on the query scripts together. 
The report was writen by all of us. Mainly: Ryan for Large Json, Marko/Momo for query results, Albert for general overview and observations.
Readme: Marko and Momo filled in our AI usage, Ryan wrote this chunk and code execution guide.

# Code execution guide

First start your mongodb server, for example using
```mongod –port {PORT NUM}```

Run the build scripts to construct the databases. Pass in your server port as an argument:

```
python3 /homeless-db/task1_build.py 27017 # For default port 27017
python3 /homeless-db/task2_build.py 27017 # For default port 27017
```

Afterwards you can then run the query files like so:

```
python3 /homeless-db/task1_query.py 27017 # For default port 27017
python3 /homeless-db/task2_query.py 27017 # For default port 27017
```

# AI Agents

agent: ChatGPT
Details:
For query2:
  We provided the input
  There are two different collections with these semantics:
messages:
"recipient": The phone number of the SMS recipient.
"sender": The nick name/phone number shown for the sender.
"text": The body of the message.
"time": The time when message was sent.
AND
senders:
"sender_id": A nick name/phone number that identifies the sender and is used in “sender” field of the messages.
"name": The name of sender.
"credit": The amount of credit for this sender, which is an integer number.
i would like to find the person who has sent the maximum number of messages, and chatGPT gave us the following output:
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
result = list(messages_collection.aggregate(pipeline))
For query3:
prompt was:
There are two different collections with these semantics:
messages:
"recipient": The phone number of the SMS recipient.
"sender": The nick name/phone number shown for the sender.
"text": The body of the message.
"time": The time when message was sent.
AND
senders:
"sender_id": A nick name/phone number that identifies the sender and is used in “sender” field of the messages.
"name": The name of sender.
"credit": The amount of credit for this sender, which is an integer number.
Using pymongo Return the number of messages where the sender’s credit is 0.

Where the return was the following code:
from pymongo import MongoClient

client = MongoClient("mongodb://localhost:27017/")
db = client["your_database_name"]

pipeline = [
    {
        "$lookup": {
            "from": "senders",
            "localField": "sender",
            "foreignField": "sender_id",
            "as": "sender_info"
        }
    },
    {
        "$match": {
            "sender_info.credit": 0
        }
    },
    {
        "$count": "total_messages_with_zero_credit"
    }
]

result = list(db.messages.aggregate(pipeline))

if result:
    total_messages_with_zero_credit = result[0]['total_messages_with_zero_credit']
    print("Total messages where sender's credit is 0:", total_messages_with_zero_credit)
else:
    print("No messages found with sender's credit as 0.")
This was code that we already had, so we repeatedly asked it to do it faster. (i.e the prompt was "give me optimized code", followed by "do it faster" until it finally provided the code:
rom pymongo import MongoClient

client = MongoClient("mongodb://localhost:27017/")
db = client["your_database_name"]

query = {"sender": {"$in": [sender['sender_id'] for sender in db.senders.find({"credit": 0}, {"sender_id": 1})]}}
total_messages_with_zero_credit = db.messages.count_documents(query)

print("Total messages where sender's credit is 0:", total_messages_with_zero_credit)

# Collaborations

We did not collaborate with anyone else.  
