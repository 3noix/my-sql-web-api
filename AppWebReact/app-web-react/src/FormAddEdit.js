import React from 'react';
import ReactDOM from 'react-dom';
import './FormAddEdit.scss';


export default function FormAddEdit({isOpen, data, setData, onOk, onCancel}) {
	if (!isOpen) {return null;}

	return ReactDOM.createPortal(
		<div className="background">
			<form>
				<div className="for-field">
					<label htmlFor="description">Description:</label>
					<input type="text" name="description" id="description"
						value={data.description}
						onChange={handleDescriptionChange}
					/>
				</div>
				<div className="for-field">
					<label htmlFor="number">Number:</label>
					<input type="type" pattern="[0-9]+" name="number" id="number"
						value={Number.isNaN(data.number) ? "" : data.number}
						onChange={handleNumberChange}
					/>
				</div>
				<div className="for-buttons">
					<button type="button" id="ok" onClick={handleOk}>Ok</button>
				</div>
				<div className="close-button" onClick={onCancel}/>
			</form>
		</div>,
		document.getElementById("portal")
	);

	
	// TODO: add some input checks
	function handleDescriptionChange(event) {
		setData(x => ({...x, description: event.target.value}));
	}

	function handleNumberChange(event) {
		setData(x => ({...x, number: parseInt(event.target.value)}));
	}

	function handleOk() {
		if (data.description.length === 0) {return;}
		if (Number.isNaN(data.number) || data.number === 0) {return;}
		onOk();
	}
}

