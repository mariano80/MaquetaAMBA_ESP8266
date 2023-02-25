/**
 * ----------------------------------------------------------------------------
 * ESP32 Remote Control with WebSocket
 * ----------------------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 * ----------------------------------------------------------------------------
 */

// var gateway = `ws://${window.location.hostname}/ws`;
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
//    initButton();
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
    toggleSequence(data.button);
    console.log(data.button);
    console.log(data.status);

}

function toggleSequence(p1) {
    setTimeout(() => {
       document.getElementById(p1).classList.toggle('buttonPush');
    }, 20)
    setTimeout(() => {
        document.getElementById(p1).classList.remove('buttonPush');
     }, 2000)
}

function onToggle1(event) {
    websocket.send(JSON.stringify({'pushed':'true', 'button' : '0'}));
}

function onToggle2(event) {
    websocket.send(JSON.stringify({'pushed':'true', 'button' : '1'}));
}

function onToggle3(event) {
    websocket.send(JSON.stringify({'pushed':'true', 'button' : '2'}));
}

function onToggle4(event) {
    websocket.send(JSON.stringify({'pushed':'true', 'button' : '3'}));
}

function onToggle5(event) {
    websocket.send(JSON.stringify({'pushed':'true', 'button' : '4'}));
}

function onToggle6(event) {
    websocket.send(JSON.stringify({'pushed':'true', 'button' : '5'}));
}

$( function() {
    var $winHeight = $( window ).height()
    $( '.container' ).height( $winHeight );
  });