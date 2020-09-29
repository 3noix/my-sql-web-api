let buttonAdd = document.querySelector("#add");
let buttonEdit = document.querySelector("#edit");
let buttonRemove = document.querySelector("#remove");

let descriptionInput = document.querySelector("#desc");
let numberInput = document.querySelector("#number");
let buttonOk = document.querySelector("#ok");
let buttonCancel = document.querySelector("#cancel");

let mode = "none";


let selectRow = (htmlRow) => {
	for (let tr of document.querySelectorAll("tbody tr")) {tr.classList.remove("selected");}
	htmlRow.classList.add("selected");
};

for (let htmlRow of document.querySelectorAll("tbody tr")) {
	htmlRow.addEventListener("click", () => {selectRow(htmlRow);});
}


// left buttons -------------------------------------
buttonAdd.addEventListener("click", (e) => {
	mode = "add";
	descriptionInput.value = "";
	numberInput.value = 0;
	document.querySelector("form").style.display = "flex";
});

buttonEdit.addEventListener("click", (e) => {
	let selectedRow = document.querySelector("tbody tr.selected");
	if (selectedRow == null) {return;}

	mode = "edit";
	descriptionInput.value = selectedRow.querySelector("td:nth-child(2)").innerHTML;
	numberInput.value = selectedRow.querySelector("td:nth-child(3)").innerHTML;
	document.querySelector("form").style.display = "flex";
});

buttonRemove.addEventListener("click", (e) => {
	let selectedRow = document.querySelector("tbody tr.selected");
	if (selectedRow == null) {return;}
	selectedRow.remove();
});


// form --------------------------------------------
buttonOk.addEventListener("click", (e) => {
	if (descriptionInput.value.length === 0 || numberInput.value.length === 0) {return;}
	document.querySelector("form").style.display = "none";
	
	if (mode === "add") {
		appendEntry(0,descriptionInput.value,numberInput.value,"xx/xx/xxxx");
	}
	else if (mode === "edit") {
		let selectedRow = document.querySelector("tbody tr.selected");
		if (selectedRow != null) {
			selectedRow.querySelector("td:nth-child(2)").innerHTML = descriptionInput.value;
			selectedRow.querySelector("td:nth-child(3)").innerHTML = numberInput.value;
		}

	}

	mode = "none";
});

buttonCancel.addEventListener("click", (e) => {
	document.querySelector("form").style.display = "none";
	mode = "none";
});

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
	line.addEventListener("click", () => selectRow(line));
	document.querySelector("tbody").appendChild(line);
};
