import numpy as np
import sqlite3
import datetime
from bokeh.plotting import figure,show
from bokeh.embed import components
#script is meant for local development and experimentation with bokeh
ht_db = '/var/jail/home/aoejile/lab07a/lab07.db'#database has table called sensor_data with entries: time_ timestamp, user text, temperature real, pressure real
USERS = ["AbeE","hkibona"] # the two users in this database are users called "dog" and "cat"
now = datetime.datetime.now()


def lookup_database():
    with sqlite3.connect(ht_db) as c:
        tempsdog = np.array(c.execute('''SELECT temperature FROM sensor_data WHERE user = ? ORDER by time_ ASC;''',(USERS[0],)).fetchall())
        tempscat = np.array(c.execute('''SELECT temperature FROM sensor_data WHERE user = ? ORDER by time_ ASC;''',(USERS[1],)).fetchall())
        pressdog = np.array(c.execute('''SELECT pressure FROM sensor_data WHERE user = ? ORDER by time_ ASC;''',(USERS[0],)).fetchall())
        presscat = np.array(c.execute('''SELECT pressure FROM sensor_data WHERE user = ? ORDER by time_ ASC;''',(USERS[1],)).fetchall() )
        time = np.array(c.execute('''SELECT time_ FROM sensor_data WHERE user = ? ORDER by time_ ASC;''',(USERS[1],)).fetchall() )
        time = time.flatten()
        tempsdog = tempsdog.flatten()
        tempscat = tempscat.flatten()
        presscat = presscat.flatten()
        pressdog = pressdog.flatten()
        print(time)
        new_time = []
        for elem in time:
            dto = datetime.datetime.strptime(elem,'%Y-%m-%d %H:%M:%S.%f')
            new_time.append(dto)
    return (tempsdog, tempscat, pressdog, presscat, new_time)
def request_handler(request):
    if request['method'] == "GET":
        returnvals = lookup_database()
        x = returnvals[4] #create array for x
        y = returnvals[0]
        y2 = returnvals[1]
        y3 = returnvals[2]
        y4 = returnvals[3]

        p1 = figure(x_axis_label = "time", y_axis_label = "temperature", x_axis_type = "datetime") #create a figure called p
        p1.title = "temperature vs time"
        p1.line(x, y, legend_label="AbeE", color = "green") # add a circle plot of x vs. y arrays
        p1.line(x, y2, legend_label="hkibona", color = "blue") #add a line plot of x vs. y arrays

        p2 = figure(x_axis_label = "time", y_axis_label = "pressure", x_axis_type = "datetime") #create a figure called p
        p2.title = "pressure vs time"
        p2.line(x, y3, legend_label="AbeE", color = "green") # add a circle plot of x vs. y arrays
        p2.line(x, y4, legend_label="hkibona", color = "blue") #add a line plot of x vs. y arrays

        script1, div1 = components(p1)
        script2, div2 = components(p2)
        return f'''<!DOCTYPE html>
        <html> <script src="https://cdn.bokeh.org/bokeh/release/bokeh-2.4.0.min.js"></script>
            <body>
                {div1}
                {div2}
            </body>
            {script1}
            {script2}
        </html>
        '''
    else:
        return ("invalid HTTP method for this url")
    
    





