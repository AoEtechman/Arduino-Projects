import sqlite3
import datetime
example_db =  '/var/jail/home/aoejile/lab07a/lab07.db' # just come up with name of database

def create_database():
    conn = sqlite3.connect(example_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    c.execute("""CREATE TABLE IF NOT EXISTS sensor_data (time_ timestamp, user text, temperature real, pressure real);""")
    conn.commit() # commit commands (VERY IMPORTANT!!)
    conn.close() # close connection to database


def insert_into_database(time, user, temperature, pressure):
    conn = sqlite3.connect(example_db)
    c = conn.cursor()
    c.execute('''INSERT into sensor_data VALUES (?,?,?,?);''', (time, user, temperature, pressure))
    conn.commit()
    conn.close()


def request_handler(request):
    if request["method"]=="POST":
        user = request["form"]["user"]
        temp = request["form"]["temperature"]
        pressure = request["form"]["pressure"]
        create_database()
        insert_into_database(datetime.datetime.now(), user, temp, pressure)
        return "Data POSTED successfully"
    else:
        return "invalid HTTP method for this url."
