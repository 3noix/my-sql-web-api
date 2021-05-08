import {useState, useEffect} from 'react';


let websocket = null;


export default function useWebSocket(validatedUserName, onWsMessage) {
	const [isConnected, setConnected] = useState(false);

	function sendWsMessage(str) {
		if (websocket == null) {return;}
		websocket.send(str);
	}

	useEffect(() => {
		if (validatedUserName == null || validatedUserName === "") {return;}

		function onWsOpen(event) {
			console.log("connected!");
			setConnected(true);
			websocket.send(JSON.stringify({userName: validatedUserName})); // send the pseudo
		}

		function onWsError(event) {
			console.log("Error: " + event.data);
			alert(`Error:\n${event.data}`);
		}

		function onWsClose(event) {
			console.log("disconnected!");
			setConnected(false);
		}
		
		// console.log("start connection attempt");
		let host = (window.location.hostname.length > 0 ? window.location.hostname : "localhost");
		websocket = new WebSocket("ws://" + host + ":1234");

		websocket.addEventListener("open",    onWsOpen);
		websocket.addEventListener("close",   onWsClose);
		websocket.addEventListener("error",   onWsError);
		websocket.addEventListener("message", onWsMessage);

		return () => {
			websocket.close(1000);
			websocket.removeEventListener("open",    onWsOpen);
			websocket.removeEventListener("close",   onWsClose);
			websocket.removeEventListener("error",   onWsError);
			websocket.removeEventListener("message", onWsMessage);
		}
	}, [validatedUserName, onWsMessage]);

	return [isConnected, sendWsMessage];
}

