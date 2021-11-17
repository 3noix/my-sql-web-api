import {useState, useRef} from "react"
import useDeepCompareEffect from "./useDeepCompareEffect"


export default function useWebSocket(url, condition, onWsOpen, onWsMessage, onWsError, onWsClose)
{
	const webSocketRef = useRef(null);
	const [isConnected, setConnected] = useState(false);

	function sendWsText(str) {
		if (webSocketRef.current == null) {return false;}
		webSocketRef.current.send(str);
		return true;
	}

	function sendWsJson(json) {
		return sendWsText(JSON.stringify(json));
	}

	useDeepCompareEffect(() => {
		if (!condition) {return;}

		function onWsOpen2(event) {
			setConnected(true);
			if (onWsOpen) {onWsOpen();}
		}

		function onWsMessage2(event) {
			if (onWsMessage) {onWsMessage(event.data);}
		}

		function onWsError2(error) {
			// if (onWsError) {onWsError(error.data);}
			if (onWsError) {onWsError(error);}
		}

		function onWsClose2(event) {
			setConnected(false);
			if (onWsClose) {onWsClose();}
		}

		webSocketRef.current = new WebSocket(url);
		webSocketRef.current.addEventListener("open",    onWsOpen2);
		webSocketRef.current.addEventListener("message", onWsMessage2);
		webSocketRef.current.addEventListener("error",   onWsError2);
		webSocketRef.current.addEventListener("close",   onWsClose2);

		return () => {
			if (webSocketRef.current == null) {return;}
			webSocketRef.current.close(1000);
			onWsClose2();
			webSocketRef.current.removeEventListener("open",    onWsOpen2);
			webSocketRef.current.removeEventListener("message", onWsMessage2);
			webSocketRef.current.removeEventListener("error",   onWsError2);
			webSocketRef.current.removeEventListener("close",   onWsClose2);
			webSocketRef.current = null;
		}
	}, [url, condition, onWsOpen, onWsMessage, onWsError, onWsClose]);

	return [isConnected, sendWsText, sendWsJson];
}

