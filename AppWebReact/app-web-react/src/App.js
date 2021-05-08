import {useState, useCallback} from 'react';
import styled from 'styled-components';
import Button from './Button';
import Table from './Table';
import FormLogin from './FormLogin';
import FormAddEdit from './FormAddEdit';
import useWebSocket from './useWebSocket';
import useMemorization from './useMemorization';


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


let defaultModalData = {id: 0, description: "", number: 0};
// let count = 0;


export default function App() {
	// useEffect(() => {
	// 	count++;
	// 	console.log("render",count);
	// });

	const [login,            setLogin]            = useState("");
	const [modalLoginOpen,   setModalLoginOpen]   = useState(true);
	const [entries,          setEntries]          = useState([]);
	const [modalAddEditOpen, setModalAddEditOpen] = useState(false);
	const [modalAddEditMode, setModalAddEditMode] = useState("");
	const [modalAddEditData, setModalAddEditData] = useState(defaultModalData);

	let onWsMessage = useCallback(event => {
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
	},[]);

	const validatedLogin = useMemorization(login, modalLoginOpen);	
	const [isConnected, sendWsMessage] = useWebSocket(validatedLogin, onWsMessage);


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
			sendWsMessage(JSON.stringify({rqtType: "unlock", rqtData: modalAddEditData.id}));
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
			sendWsMessage(JSON.stringify({rqtType: "insert", rqtData: data}));
		}
		else if (modalAddEditMode === "edit") {
			// update the entry and unlock it
			sendWsMessage(JSON.stringify({rqtType: "update", rqtData: modalAddEditData}));
			sendWsMessage(JSON.stringify({rqtType: "unlock", rqtData: modalAddEditData.id}));
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
		sendWsMessage(JSON.stringify({rqtType: "lock", rqtData: selectedEntry.id}));
		
		// fill and show the dialog data
		setModalAddEditMode("edit");
		setModalAddEditData({id: selectedEntry.id, description: selectedEntry.description, number: selectedEntry.number});
		setModalAddEditOpen(true);
	}

	function deleteEntry() {
		let id = entries.find(e => e.selected)?.id;
		if (!id) {return;}

		// lock the entry and send the delete request
		sendWsMessage(JSON.stringify({rqtType: "lock", rqtData: id}));
		sendWsMessage(JSON.stringify({rqtType: "delete", rqtData: id}));
	}
}

