# Overview


The goal of the thermostat design exercise was to create a stateful internet controlled hvac system that was responsive to user inputs on both the web and the hardware side. In order to complete this design exercise, I had to create both esp and server scripts that to accomplish my goals


For my server side script, I had a request handler that took in either a get request or a post request. If there was a get request, I got the most recent values from the database I created, and returned those to both my web browser and my esp. I also implimented html in order to create a ui on my web side. If a post request was received, I retrieved the post information that was given, determined the stutus of my hvac system, and wrote the data into the database. For database functionality, I had three functions. A function that created the database, one that wrote into the database, and one that retrieved information from the database

My esp side consisted of two main components, state machines in order to allow the user to change the offset temperature as well as the target temperature, and get and post functions to post the users changes, as well as get changes from the database. During the posting period, I performed both of these tasks in order to display the most up to date data from my database. I also had a state machine that controlled whether or not the led turned on. if we were in heat mode and status = on, then I would turn the led for 30 seconds unless status changed or the mode changed. The same applied for the cooling mode. temperature readings were collected using the adafruit device


[demonstration video](https://www.youtube.com/watch?v=9jxYN0CNalI&ab_channel=AbeE)