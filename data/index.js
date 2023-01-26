/**
 * ----------------------------------------------------------------------------
 * ESP32 Remote Control with WebSocket
 * ----------------------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 * ----------------------------------------------------------------------------
 */

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    initButton();
}

// ----------------------------------------------------------------------------
// WebSocket handling
// ----------------------------------------------------------------------------

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    let data = JSON.parse(event.data);
    document.getElementById('led').className = data.status;
}

// ----------------------------------------------------------------------------
// Button handling
// ----------------------------------------------------------------------------

function initButton() {
    document.getElementById('switch1').addEventListener('click', onToggle1);
    document.getElementById('switch2').addEventListener('click', onToggle2);
    document.getElementById('switch3').addEventListener('click', onToggle3);
    document.getElementById('switch4').addEventListener('click', onToggle4);
    document.getElementById('switch5').addEventListener('click', onToggle5);
}

function onToggle1(event) {
    websocket.send(JSON.stringify({'action':'toggle', 'button' : '1'}));
}

function onToggle2(event) {
    websocket.send(JSON.stringify({'action':'toggle', 'button' : '2'}));
}

function onToggle3(event) {
    websocket.send(JSON.stringify({'action':'toggle', 'button' : '3'}));
}

function onToggle4(event) {
    websocket.send(JSON.stringify({'action':'toggle', 'button' : '4'}));
}

function onToggle5(event) {
    websocket.send(JSON.stringify({'action':'toggle', 'button' : '5'}));
}