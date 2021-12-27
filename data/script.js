var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);

function mqttStatus(id) {
  return parseInt(id) == 1 ? "MQTT ready" : "MQTT_DISCONNECTED - the client is disconnected...";
}

function onLoad(event) {
  initWebSocket();
}

function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen    = onOpen;
  websocket.onclose   = onClose;
  websocket.onmessage = onMessage;
}

function onOpen(event) {
  console.log('Connection opened');
  websocket.send("states");
}
  
function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 2000);
} 

function onMessage(event) {
  var myObj = JSON.parse(event.data);
  console.log(myObj);

  /* Mqtt state */
  var mq = document.getElementById("mqtt");
  mq.innerHTML = mqttStatus(myObj.mqtt);
  mq.classList.remove("alert-danger", "alert-success");
  mq.classList.add((myObj.mqtt == 1 ? "alert-success" : "alert-danger"));

  document.getElementById("temperature").innerHTML = myObj.temperature;
  document.getElementById("humidity").innerHTML = myObj.humidity;
  document.getElementById("out_time").innerHTML = myObj.out_time;

}