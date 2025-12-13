"""
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5) & TMRh20
 */
"""

import time
import datetime
from arduino.app_utils import App, Bridge
from arduino.app_bricks.web_ui import WebUI

print("Python Bridge and Webserver Example for RF24 GettingStarted example!")

webPld = 0.1
 
def RF24Callback(payload: float):
    global webPld
    webPld = round(payload,2)
    print(webPld)
    ui.send_message('myPayload', {"value": float(webPld)})

def gotMessage(x,y):
    Bridge.call(y)
    
Bridge.provide("RF24Callback", RF24Callback)

ui = WebUI()
ui.expose_api("GET", "/hello", lambda: {"message" : f"last received value: {webPld}","message2":"wtfover"})
ui.on_message("message", gotMessage)

App.run()  # This will block until the app is stopped



