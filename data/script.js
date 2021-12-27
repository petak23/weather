var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);

function mqttStatus(id) {
  return parseInt(id) == 1 ? "MQTT pripojené." : "MQTT server nie je pripojený!";
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

  if (myObj.mqtt.length > 0) {
    /* Mqtt state */
    var mq = document.getElementById("mqtt");
    mq.innerHTML = mqttStatus(myObj.mqtt);
    mq.classList.remove("alert-danger", "alert-success", "alert-dark");
    mq.classList.add((myObj.mqtt == 1 ? "alert-success" : "alert-danger"));
  }

  if (myObj.temperature.length > 0) document.getElementById("temperature").innerHTML = myObj.temperature;
  if (myObj.humidity.length > 0) document.getElementById("humidity").innerHTML = myObj.humidity;
  if (myObj.out_time.length > 0) document.getElementById("out_time").innerHTML = myObj.out_time;

  if (myObj.logbook.length > 0) {
    var x = document.createElement("div");
    var t = document.createTextNode(myObj.logbook);
    x.appendChild(t);
    document.getElementById("logbook").appendChild(x);
  }

}