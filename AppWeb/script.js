// GET USEFUL HANDLES ON ELTS FOR LATER ///////////////////
let buttonAdd = document.querySelector("#add");
let buttonEdit = document.querySelector("#edit");
let buttonRemove = document.querySelector("#remove");

let descriptionInput = document.querySelector("#desc");
let numberInput = document.querySelector("#number");
let buttonOk = document.querySelector("#ok");
let buttonCancel = document.querySelector("#cancel");


// INIT ///////////////////////////////////////////////////
let mode = "none";
document.querySelector("thead").addEventListener("click", () => {
	for (let tr of document.querySelectorAll("tbody tr")) {tr.classList.remove("selected");}
});


// CONNECT TO THE API, INIT THE WEBSOCKET /////////////////
let websocket = new WebSocket("ws://localhost:1234");

websocket.addEventListener("open", () => {
	console.log("connected!");

	buttonAdd.disabled = false;
	buttonEdit.disabled = false;
	buttonRemove.disabled = false;

	websocket.send(JSON.stringify({userName: "3noix"})); // send the pseudo
});

websocket.addEventListener("message", (e) => {
	let data = JSON.parse(e.data);
	if (data.type === "insert") {
		let e = data.entry;
		appendEntry(e.id,e.description,e.number,e.last_modif);
	}
	else if (data.type === "update") {
		let e = data.entry;
		updateEntry(e.id,e.description,e.number,e.last_modif);
	}
	else if (data.type === "delete") {
		deleteEntry(data.id);
	}
	else {
		for (let e of data) {
			appendEntry(e.id,e.description,e.number,e.last_modif);
		}
	}
});

websocket.addEventListener("error", (e) => {
	console.log("error: " + e.data);
});

websocket.addEventListener("close", (e) => {
	console.log("disconnected!");

	buttonAdd.disabled = true;
	buttonEdit.disabled = true;
	buttonRemove.disabled = true;
});




// BUTTONS CALLBACKS //////////////////////////////////////
buttonAdd.addEventListener("click", (e) => {
	// reset and display the dialog
	mode = "add";
	descriptionInput.value = "";
	numberInput.value = 0;
	setDialogVisible(true);
});

buttonEdit.addEventListener("click", (e) => {
	let selectedRow = document.querySelector("tbody tr.selected");
	if (selectedRow == null) {return;}

	// fill and display the dialog
	mode = "edit";
	descriptionInput.value = selectedRow.querySelector("td:nth-child(2)").innerHTML;
	numberInput.value = selectedRow.querySelector("td:nth-child(3)").innerHTML;
	setDialogVisible(true);
});

buttonRemove.addEventListener("click", (e) => {
	let selectedRow = document.querySelector("tbody tr.selected");
	if (selectedRow == null) {return;}

	// send delete request
	let id = parseInt(selectedRow.querySelector("td:nth-child(1)").innerHTML);
	websocket.send(JSON.stringify({rqtType: "delete", rqtData: id}));
});




// AFTER FORM VALIDATION //////////////////////////////////
buttonOk.addEventListener("click", (e) => {
	if (descriptionInput.value.length === 0 || numberInput.value.length === 0) {return;}
	setDialogVisible(false);
	
	if (mode === "add") {
		let data = {description: descriptionInput.value, number: parseInt(numberInput.value)};
		let msg = JSON.stringify({rqtType: "insert", rqtData: data});
		websocket.send(msg);
	}
	else if (mode === "edit") {
		let selectedRow = document.querySelector("tbody tr.selected");
		if (selectedRow == null) {return;}

		let id = parseInt(selectedRow.querySelector("td:nth-child(1)").innerHTML);
		let data = {id: id, description: descriptionInput.value, number: parseInt(numberInput.value)};
		websocket.send(JSON.stringify({rqtType: "update", rqtData: data}));
	}

	mode = "none";
});

buttonCancel.addEventListener("click", (e) => {
	setDialogVisible(false);
	mode = "none";
});




// APPEND ENTRY ///////////////////////////////////////////
let appendEntry = (id, description, number, datetime) => {
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

// UPDATE ENTRY ///////////////////////////////////////////
let updateEntry = (id, description, number, datetime) => {
	let elt = getEntryHtmlElt(id);
	if (elt == null) {return false;}

	elt.querySelector("td:nth-child(2)").innerHTML = description;
	elt.querySelector("td:nth-child(3)").innerHTML = number;
	elt.querySelector("td:nth-child(4)").innerHTML = datetime;
	return true;
};

// DELETE ENTRY ///////////////////////////////////////////
let deleteEntry = (id) => {
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

// SET DIALOG VISIBLE /////////////////////////////////////
let setDialogVisible = (bVisible) => {
	if (bVisible) {
		document.querySelector("form").style.display = "flex";
		// document.querySelector(".background").style.display = "flex";
		document.querySelector(".background").style.display = "block";
	}
	else {
		document.querySelector("form").style.display = "none";
		document.querySelector(".background").style.display = "none";
	}
};

