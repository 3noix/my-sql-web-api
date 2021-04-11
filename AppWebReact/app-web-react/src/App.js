import {useState, useEffect} from 'react';
import styled from 'styled-components';
import Button from './Button';
import Table from './Table';
import FormAddEdit from './FormAddEdit';


const Root = styled.div`
	display: flex;
	flex-direction: row;
`;

const Section = styled.section`
	width: 50px;
	display: flex;
	flex-direction: column;
	justify-content: flex-start;
	align-items: center;
`;

const Main = styled.main`
	width: calc(100% - 50px);
`;


let websocket = null;
let defaultModalData = {id: 0, description: "", number: 0};
// let count = 0;


export default function App() {
	const [isConnected, setConnected] = useState(false);
	const [entries,     setEntries]   = useState([]);
	const [modalOpen,   setModalOpen] = useState(false);
	const [modalMode,   setModalMode] = useState("");
	const [modalData,   setModalData] = useState(defaultModalData);

	// useEffect(() => {
	// 	count++;
	// 	console.log("render",count);
	// });

	useEffect(() => {
		function onWsOpen(event) {
			console.log("connected!");
			setConnected(true);
			websocket.send(JSON.stringify({userName: "3noix"})); // send the pseudo
		}
	
		function onWsError(event) {
			console.log("Error: " + event.data);
			alert(`Error:\n${event.data}`);
		}
		
		function onWsClose(event) {
			console.log("disconnected!");
			setConnected(false);
		}
		
		function onWsMessage(event) {
			let data = JSON.parse(event.data);
			// console.log(data);
	
			if (data.type === "insert") {
				let newEntry = {...(data.entry), selected: false};
				setEntries(prevEntries => [...prevEntries, newEntry]);
			}
			else if (data.type === "update") {
				let e2 = data.entry;
				let updateEntry = e => {
					if (e.id !== e2.id) {return e;} // no update
					return {id: e.id, description: e2.description, number: e2.number, last_modif: e2.last_modif, selected: e.selected};
				};
				setEntries(prevEntries => prevEntries.map(updateEntry));
			}
			else if (data.type === "delete") {
				setEntries(prevEntries => prevEntries.filter(e => e.id !== data.id));
			}
			else if (Array.isArray(data)) {
				setEntries(data);
			}
			else if (data.type === "lock") {
				if (data.status !== "success") {
					alert(`Lock request failed:\n${data.msg}`);
					setModalOpen(false);
					setModalMode("");
					setModalData(defaultModalData);
				}
				// else if (data.id === modalData.id && modalMode === "edit") {
				// 	setModalOpen(true); // if we wait the lock confirmation to show the dialog
				// }
			}
			else if (data.type === "unlock") {
				if (data.status !== "success") {
					alert(`Unlock request failed:\n${data.msg}`);
				}
			}
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
	}, []);


	return (
		<Root>
			<Section>
				<Button iconClass="fas fa-plus"       bDisabled={!isConnected} onClick={addNewEntry}/>
				<Button iconClass="fas fa-pencil-alt" bDisabled={!isConnected} onClick={updateEntry}/>
				<Button iconClass="fas fa-minus"      bDisabled={!isConnected} onClick={deleteEntry}/>
			</Section>
			<Main>
				<Table
					entries={entries}
					deselectAllRows={deselectAllRows}
					selectRow={selectRow}
				/>
			</Main>
			<FormAddEdit
				isOpen={modalOpen}
				data={modalData}
				setData={setModalData}
				onOk={handleOk}
				onCancel={handleCancel}
			/>
		</Root>
	);


	function selectRow(id) {
		let selectThisRow = (e => (e.id === id ? {...e, selected: true} : {...e, selected: false}));
		setEntries(prevEntries => prevEntries.map(selectThisRow));
	}

	function deselectAllRows() {
		let deselectEntry = (e => {e.selected = false; return e;});
		setEntries(prevEntries => prevEntries.map(deselectEntry));
	}

	function handleCancel() {
		if (modalMode === "edit") {
			// unlock the entry being edited
			websocket.send(JSON.stringify({rqtType: "unlock", rqtData: modalData.id}));
		}

		// close and reset everything
		setModalOpen(false);
		setModalMode("");
		setModalData(defaultModalData);
	}

	function handleOk() {
		// remark: the input (i.e. modalData) is checked inside the dialog
		setModalOpen(false);

		if (modalMode === "add") {
			let data = {description: modalData.description, number: modalData.number};
			websocket.send(JSON.stringify({rqtType: "insert", rqtData: data}));
		}
		else if (modalMode === "edit") {
			// update the entry and unlock it
			websocket.send(JSON.stringify({rqtType: "update", rqtData: modalData}));
			websocket.send(JSON.stringify({rqtType: "unlock", rqtData: modalData.id}));
		}

		setModalMode("");
		setModalData(defaultModalData);
	}

	function addNewEntry() {
		setModalOpen(true);
		setModalMode("add");
		setModalData(defaultModalData);
	}

	function updateEntry() {
		let selectedEntry = entries.find(e => e.selected);
		if (!selectedEntry) {return;}

		// lock the entry to update
		websocket.send(JSON.stringify({rqtType: "lock", rqtData: selectedEntry.id}));
		
		// fill and show the dialog data
		setModalMode("edit");
		setModalData({id: selectedEntry.id, description: selectedEntry.description, number: selectedEntry.number});
		setModalOpen(true);
	}

	function deleteEntry() {
		let id = entries.find(e => e.selected)?.id;
		if (!id) {return;}

		// lock the entry and send the delete request
		websocket.send(JSON.stringify({rqtType: "lock", rqtData: id}));
		websocket.send(JSON.stringify({rqtType: "delete", rqtData: id}));
	}
}

