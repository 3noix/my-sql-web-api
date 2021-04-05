import React from 'react';
import ReactDOM from 'react-dom';
import './ModalForm.scss';


export default function ModalForm({isOpen, data, setData, onOk, onCancel}) {
	if (!isOpen) {return null;}

	return ReactDOM.createPortal(
		<div className="background">
			<form>
				<div>
					<label htmlFor="description">Description:</label>
					<input type="text" name="description" id="description"
						value={data.description}
						onChange={handleDescriptionChange}
					/>
				</div>
				<div>
					<label htmlFor="number">Number:</label>
					<input type="number" name="number" id="number"
						value={data.number}
						onChange={handleNumberChange}
					/>
				</div>
				<div>
					<button type="button" id="ok" onClick={handleOk}>Ok</button>
					<button type="button" id="cancel" onClick={onCancel}>Cancel</button>
				</div>
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
		if (data.description.length === 0 || data.number === 0) {return;}
		onOk();
	}
}

