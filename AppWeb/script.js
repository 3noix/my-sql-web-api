// GET USEFUL HANDLES ON ELTS FOR LATER ///////////////////
let buttonAdd = document.querySelector("#add");
let buttonEdit = document.querySelector("#edit");
let buttonRemove = document.querySelector("#remove");

let idInput = document.querySelector("#id");
let descriptionInput = document.querySelector("#desc");
let numberInput = document.querySelector("#number");
let buttonOk = document.querySelector("#ok");
let buttonCancel = document.querySelector("#cancel");

// SET DIALOG VISIBLE /////////////////////////////////////
let setDialogVisible = (bVisible) => {
	if (bVisible) {
		document.querySelector("form").style.display = "flex";
		document.querySelector(".background").style.display = "block";
	}
	else {
		document.querySelector("form").style.display = "none";
		document.querySelector(".background").style.display = "none";
	}
};

// SET BUTTONS ENABLED ////////////////////////////////////
let setButtonsEnabled = (bEnabled) => {
	buttonAdd.disabled = !bEnabled;
	buttonEdit.disabled = !bEnabled;
	buttonRemove.disabled = !bEnabled;
};


// INIT ///////////////////////////////////////////////////
setButtonsEnabled(false);
let mode = "none";
document.querySelector("thead").addEventListener("click", () => {
	for (let tr of document.querySelectorAll("tbody tr")) {tr.classList.remove("selected");}
});


// CONNECT TO THE API, INIT THE WEBSOCKET /////////////////
let host = (window.location.hostname.length > 0 ? window.location.hostname : "localhost");
let websocket = new WebSocket("ws://" + host + ":1234");

websocket.addEventListener("open", () => {
	console.log("connected!");
	setButtonsEnabled(true);
	websocket.send(JSON.stringify({userName: "3noix"})); // send the pseudo
});

websocket.addEventListener("message", (e) => {
	let data = JSON.parse(e.data);
	if (data.type === "insert") {
		let e = data.entry;
		appendEntryInHtml(e.id,e.description,e.number,e.last_modif);
	}
	else if (data.type === "update") {
		let e = data.entry;
		updateEntryInHtml(e.id,e.description,e.number,e.last_modif);
	}
	else if (data.type === "delete") {
		deleteEntryInHtml(data.id);
	}
	else if (Array.isArray(data)) {
		for (let e of data) {
			appendEntryInHtml(e.id,e.description,e.number,e.last_modif);
		}
	}
	else if (data.type === "lock") {
		if (data.status !== "success") {
			alert(`Lock request failed:\n${data.msg}`);
		}
		else if (data.id === parseInt(idInput.value) && mode === "edit") {
			setDialogVisible(true);
		}
	}
	else if (data.type === "unlock") {
		if (data.status !== "success") {
			alert(`Unlock request failed:\n${data.msg}`);
		}
	}
});

websocket.addEventListener("error", (e) => {
	// console.log("Error: " + e.data);
	alert(`Error:\n${e.data}`);
});

websocket.addEventListener("close", (e) => {
	console.log("disconnected!");
	setButtonsEnabled(false);
});




// BUTTONS CALLBACKS //////////////////////////////////////
buttonAdd.addEventListener("click", (e) => {
	// reset and display the dialog
	mode = "add";
	idInput.value = 0;
	descriptionInput.value = "";
	numberInput.value = 0;
	setDialogVisible(true);
});

buttonEdit.addEventListener("click", (e) => {
	let selectedRow = document.querySelector("tbody tr.selected");
	if (selectedRow == null) {return;}

	// lock the entry to update
	let id = parseInt(selectedRow.querySelector("td:nth-child(1)").innerHTML);
	websocket.send(JSON.stringify({rqtType: "lock", rqtData: id}));

	// fill and display the dialog
	mode = "edit";
	idInput.value = id;
	descriptionInput.value = selectedRow.querySelector("td:nth-child(2)").innerHTML;
	numberInput.value = selectedRow.querySelector("td:nth-child(3)").innerHTML;
});

buttonRemove.addEventListener("click", (e) => {
	let selectedRow = document.querySelector("tbody tr.selected");
	if (selectedRow == null) {return;}

	// lock the entry to delete
	let id = parseInt(selectedRow.querySelector("td:nth-child(1)").innerHTML);
	websocket.send(JSON.stringify({rqtType: "lock", rqtData: id}));

	// send delete request
	websocket.send(JSON.stringify({rqtType: "delete", rqtData: id}));
});




// AFTER FORM VALIDATION //////////////////////////////////
buttonOk.addEventListener("click", (e) => {
	if (descriptionInput.value.length === 0 || numberInput.value.length === 0) {return;}
	setDialogVisible(false);
	
	if (mode === "add") {
		let data = {description: descriptionInput.value, number: parseInt(numberInput.value)};
		websocket.send(JSON.stringify({rqtType: "insert", rqtData: data}));
	}
	else if (mode === "edit") {
		let id = parseInt(idInput.value);
		let data = {id: id, description: descriptionInput.value, number: parseInt(numberInput.value)};
		websocket.send(JSON.stringify({rqtType: "update", rqtData: data}));

		// unlock the updated entry
		websocket.send(JSON.stringify({rqtType: "unlock", rqtData: id}));
	}

	mode = "none";
});

buttonCancel.addEventListener("click", (e) => {
	if (mode === "edit") {
		// unlock the entry being edited
		websocket.send(JSON.stringify({rqtType: "unlock", rqtData: parseInt(idInput.value)}));
	}

	setDialogVisible(false);
	mode = "none";
});




// APPEND ENTRY IN HTML ///////////////////////////////////
let appendEntryInHtml = (id, description, number, datetime) => {
	let line = document.createElement("tr");
	let col1 = document.createElement("td");
	let col2 = document.createElement("td");
	let col3 = document.createElement("td");
	let col4 = document.createElement("td");
	col1.innerHTML = id;
	col2.innerHTML = description;
	col3.innerHTML = number;
	col4.innerHTML = datetime;
	line.appendChild(col1);
	line.appendChild(col2);
	line.appendChild(col3);
	line.appendChild(col4);
	document.querySelector("tbody").appendChild(line);

	line.addEventListener("click", () => {
		for (let tr of document.querySelectorAll("tbody tr")) {tr.classList.remove("selected");}
		line.classList.add("selected");
	});
};

// UPDATE ENTRY IN HTML ///////////////////////////////////
let updateEntryInHtml = (id, description, number, datetime) => {
	let elt = getEntryHtmlElt(id);
	if (elt == null) {return false;}

	elt.querySelector("td:nth-child(2)").innerHTML = description;
	elt.querySelector("td:nth-child(3)").innerHTML = number;
	elt.querySelector("td:nth-child(4)").innerHTML = datetime;
	return true;
};

// DELETE ENTRY IN HTML ///////////////////////////////////
let deleteEntryInHtml = (id) => {
	let elt = getEntryHtmlElt(id);
	if (elt == null) {return false;}
	elt.remove();
	return true;
};

// GET ENTRY HTML ELT /////////////////////////////////////
let getEntryHtmlElt = (id) => {
	for (let htmlRow of document.querySelectorAll("tbody tr")) {
		let currentId = parseInt(htmlRow.querySelector("td:nth-child(1)").innerHTML);
		if (currentId === id) {return htmlRow;}
	}
	return null;
};

