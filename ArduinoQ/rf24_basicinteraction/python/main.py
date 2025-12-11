
import time
from arduino.app_utils import App, Bridge

print("Python Bridge Example for RF24 GettingStarted example!")

def RF24Callback(payload: float):
    print(payload)

Bridge.provide("RF24Callback", RF24Callback)

def loop():
    """This function is called repeatedly by the App framework."""
    # You can replace this with any code you want your App to run repeatedly.
    time.sleep(10)
    print("Python")    

# See: https://docs.arduino.cc/software/app-lab/tutorials/getting-started/#app-run
App.run(user_loop=loop)




