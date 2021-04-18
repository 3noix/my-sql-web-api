import {useState, useEffect, useRef} from 'react';
import styled from 'styled-components';
import Button from './Button';
import Table from './Table';
import FormLogin from './FormLogin';
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
	const [login,            setLogin]            = useState("");
	const [modalLoginOpen,   setModalLoginOpen]   = useState(true);
	const [isConnected,      setConnected]        = useState(false);
	const [entries,          setEntries]          = useState([]);
	const [modalAddEditOpen, setModalAddEditOpen] = useState(false);
	const [modalAddEditMode, setModalAddEditMode] = useState("");
	const [modalAddEditData, setModalAddEditData] = useState(defaultModalData);

	// useEffect(() => {
	// 	count++;
	// 	console.log("render",count);
	// });

	const loginRef = useRef(login);
	loginRef.current = login;

	useEffect(() => {
		if (modalLoginOpen) {return;}

		function onWsOpen(event) {
			console.log("connected!");
			setConnected(true);
			websocket.send(JSON.stringify({userName: loginRef.current})); // send the pseudo
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
					setModalAddEditOpen(false);
					setModalAddEditMode("");
					setModalAddEditData(defaultModalData);
				}
				// else if (data.id === modalAddEditData.id && modalAddEditMode === "edit") {
				// 	setModalAddEditOpen(true); // if we wait the lock confirmation to show the dialog
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
	}, [modalLoginOpen]);


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
			<FormLogin
				isOpen={modalLoginOpen}
				login={login}
				setLogin={setLogin}
				onOk={() => setModalLoginOpen(false)}
			/>
			<FormAddEdit
				isOpen={modalAddEditOpen}
				data={modalAddEditData}
				setData={setModalAddEditData}
				onOk={handleAddEditOk}
				onCancel={handleAddEditCancel}
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

	function handleAddEditCancel() {
		if (modalAddEditMode === "edit") {
			// unlock the entry being edited
			websocket.send(JSON.stringify({rqtType: "unlock", rqtData: modalAddEditData.id}));
		}

		// close and reset everything
		setModalAddEditOpen(false);
		setModalAddEditMode("");
		setModalAddEditData(defaultModalData);
	}

	function handleAddEditOk() {
		// remark: the input (i.e. modalAddEditData) is checked inside the dialog
		setModalAddEditOpen(false);

		if (modalAddEditMode === "add") {
			let data = {description: modalAddEditData.description, number: modalAddEditData.number};
			websocket.send(JSON.stringify({rqtType: "insert", rqtData: data}));
		}
		else if (modalAddEditMode === "edit") {
			// update the entry and unlock it
			websocket.send(JSON.stringify({rqtType: "update", rqtData: modalAddEditData}));
			websocket.send(JSON.stringify({rqtType: "unlock", rqtData: modalAddEditData.id}));
		}

		setModalAddEditMode("");
		setModalAddEditData(defaultModalData);
	}

	function addNewEntry() {
		setModalAddEditOpen(true);
		setModalAddEditMode("add");
		setModalAddEditData(defaultModalData);
	}

	function updateEntry() {
		let selectedEntry = entries.find(e => e.selected);
		if (!selectedEntry) {return;}

		// lock the entry to update
		websocket.send(JSON.stringify({rqtType: "lock", rqtData: selectedEntry.id}));
		
		// fill and show the dialog data
		setModalAddEditMode("edit");
		setModalAddEditData({id: selectedEntry.id, description: selectedEntry.description, number: selectedEntry.number});
		setModalAddEditOpen(true);
	}

	function deleteEntry() {
		let id = entries.find(e => e.selected)?.id;
		if (!id) {return;}

		// lock the entry and send the delete request
		websocket.send(JSON.stringify({rqtType: "lock", rqtData: id}));
		websocket.send(JSON.stringify({rqtType: "delete", rqtData: id}));
	}
}

