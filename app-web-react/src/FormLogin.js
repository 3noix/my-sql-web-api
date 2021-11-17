import React from 'react';
import ReactDOM from 'react-dom';
import styled from 'styled-components';
import './FormLogin.scss';


const Overlay = styled.div`
	position: fixed;
	top: 0;
	bottom: 0;
	left: 0;
	right: 0;
	background: rgba(0,0,0,0.5);
	z-index: 1;
	display: flex;
	justify-content: center;
	align-items: center;
`;


export default function FormLogin({isOpen, login, setLogin, onOk}) {
	if (!isOpen) {return null;}

	return ReactDOM.createPortal(
		<Overlay>
			<form className="login" onSubmit={handleSubmit}>
				<div className="for-field">
					<label htmlFor="login">Login:</label>
					<input autoFocus type="text" name="login" id="login"
						value={login}
						onChange={handleLoginChange}
					/>
				</div>
				<div className="for-buttons">
					<button type="button" id="ok" onClick={handleOk}>Connect</button>
				</div>
			</form>
		</Overlay>,
		document.getElementById("portalLoginModal")
	);

	
	// TODO: add some input checks and errors display
	function handleLoginChange(event) {
		setLogin(event.target.value);
	}

	function handleOk() {
		if (login.length <= 2) {return;}
		onOk();
	}

	function handleSubmit(event) {
		event.preventDefault();
		handleOk();
	}
}

