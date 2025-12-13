

document.addEventListener('DOMContentLoaded', () => {
    initializeElements();
    initSocketIO();
});

function runMainButton() {
  const myHeader = document.getElementById('myHeader');
  mySocket = io(`http://${window.location.host}`);

  const myData = "toggleMatrix";
  mySocket.send(myData);
  
  myHeader.textContent = "Success";

}

function initializeElements() {
    const mainButton = document.getElementById('mainButton');
    mainButton.addEventListener('click', runMainButton);
}

function displayPayload(msgValue){
  const myHeader = document.getElementById('myHeader');
  myHeader.textContent = msgValue;
}


/*
 * Socket initialization: required for communication with the server.
 */

function initSocketIO() {
    socket = io(`http://${window.location.host}`);
    
    socket.on('myPayload', (message) => {
        displayPayload(message.value);
    });

}

