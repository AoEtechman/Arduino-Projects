import json 
import sqlite3
import datetime
example_db =  '/var/jail/home/aoejile/design/thermostat.db' # just come up with name of databa

def lookup_database():
    with sqlite3.connect(example_db) as c:
        target_temp = (c.execute('''SELECT target temp FROM sensor_data ORDER by time DESC;''',).fetchall())
        current_temp = (c.execute('''SELECT current temp FROM sensor_data ORDER by time DESC; ''',).fetchone())
        test_temp = (c.execute('''SELECT test temp FROM sensor_data ORDER by time DESC;''').fetchone())
        mode = (c.execute('''SELECT mode FROM sensor_data ORDER by time DESC;''').fetchall())
        status = (c.execute('''SELECT status FROM sensor_data ORDER by time DESC;''').fetchone())
        if target_temp[0][0] != target_temp[1][0] and target_temp[0][0] == target_temp[2][0]:
            target_temp = target_temp[1]
        elif target_temp[0][0] == target_temp[1][0]:
            target_temp = target_temp[0]
        else:
            target_temp = target_temp[0]
        if mode[0][0] != mode[1][0] and mode[0][0] == mode[2][0]:
            mode = mode[1]
        elif mode[0][0] == mode[1][0]:
            mode = mode[0]
        else:
            mode = mode[0]
    return (target_temp, current_temp, test_temp, mode, status)

def create_database():
    conn = sqlite3.connect(example_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    c.execute("""CREATE TABLE IF NOT EXISTS sensor_data (time timestamp, target temp real, current temp real, test temp real, mode text, status text );""")
    conn.commit() # commit commands (VERY IMPORTANT!!)
    conn.close() # close connection to database


def insert_into_database(time, target_temp, current_temp, test_temp, mode, status):
    conn = sqlite3.connect(example_db)
    c = conn.cursor()
    c.execute('''INSERT into sensor_data VALUES (?,?,?,?,?,?);''', (time, target_temp, current_temp, test_temp, mode, status))
    conn.commit()
    conn.close()

def insert_into_database1(time, target_temp, mode,):
    conn = sqlite3.connect(example_db)
    c = conn.cursor()
    c.execute('''INSERT into sensor_data VALUES (?,?,?);''', (time, target_temp, mode))
    conn.commit()
    conn.close()

def request_handler(request):
    output_dict = {}
    if request['method'] == "POST":
        create_database()
        target_temp = (request['form']["target_temp"])
        mode = request['form']['mode']
        try:
            current_temp = float(request['form']["current_temp"])
            test_room_temp = float(request['form']['test_room_temp'])
        except:
            return_vals = lookup_database()
            current_temp = float(return_vals[1][0])
            test_room_temp = float(return_vals[2][0])

        try:      
            if int(mode) == 1:
                mode = "heat"
                if float(test_room_temp) < int(target_temp) - 1:
                    status = "ON"
                else:
                    status = "OFF"
            elif mode == 2:
                mode = "cool"
                if float(test_room_temp) > int(target_temp) + 1:
                    status = "ON"
                else:
                    status = "OFF"
        except:
            if mode == "heat":
                if float(test_room_temp) < int(target_temp) - 1:
                    status = "ON"
                else:
                    status = "OFF"
            elif  mode == "cool":
                if float(test_room_temp) > int(target_temp) + 1:
                    status = "ON"
                else:
                    status = "OFF"
        insert_into_database(datetime.datetime.now(), target_temp, current_temp, test_room_temp, mode, status)
        return ("data succesfully posted")
    elif request['method'] == "GET":
        if request['values']:
            return_vals = lookup_database()
            target_temp = return_vals[0][0]
            current_temp = return_vals[1][0]
            test_room_temp = return_vals[2][0]
            mode = return_vals[3][0]
            status = return_vals[4][0]
            output_dict = {
                "target temp": int(target_temp),
                "current temp": current_temp,
                "mode": mode,
                "test_room_temp": test_room_temp,
                "status": status
                }
            return_dict = json.dumps(output_dict)
            return return_dict
        else:
            return_vals = lookup_database()
            target_temp = return_vals[0][0]
            current_temp = return_vals[1][0]
            test_room_temp = return_vals[2][0]
            mode = return_vals[3][0]
            status = return_vals[4][0]
            output_dict = {
                "target temp": int(target_temp),
                "current temp": current_temp,
                "mode": mode,
                "test_room_temp": test_room_temp,
                "status": status
                }
            return_dict = json.dumps(output_dict)
            return f"""<!DOCTYPE html>
            <html>
            <body>
            {return_dict}
            <h1>Thermostat</h1>
             <form action="/sandbox/sc/aoejile/design/thermostat_request.py" method="post">
             <label for="target_temp">target temperature:</label>
             <input type="number" id="target_temp" name="target_temp" min="0" max="1000">
             <br>
             <label for="mode">mode:</label>
             <input type="text" id="mode" name="mode">
             <br>
             <input type="submit">
             </form>

            </body>
            </html>
            """
       
    
        
        # if mode.lower() == "heat":
        #     if float(test_room_temp) < int(target_temp) - 1:
        #         status = "ON"
        #     else:
        #         status = "OFF"
        # if mode.lower() == "cool":
        #     if float(test_room_temp) > int(target_temp) + 1:
        #         status = "ON"
        #     else:
        #         status = "OFF"
        # except UnboundLocalError:
        #     test_room_temp = 70
        #     status = "OFF"
        #     current_temp = 70
    

# request = {'method': "GET", "values": {'mode': "heat", 'target_temp': "80"}}
# print(request_handler(request))


    

