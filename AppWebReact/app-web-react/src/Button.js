import React from 'react';
import './Button.scss';


export default function Button({iconClass,bDisabled,onClick}) {
	return (
		<button type="button" onClick={onClick} disabled={bDisabled}>
			<i className={iconClass}></i>
		</button>
	);
}

