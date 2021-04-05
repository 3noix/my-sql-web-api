import React from 'react';
import './Table.scss';


export default function Table({entries,deselectAllRows,selectRow}) {
	return (
		<table>
			<thead onClick={deselectAllRows}>
				<tr>
					<th>Id</th>
					<th>Description</th>
					<th>Number</th>
					<th>Last modif</th>
				</tr>
			</thead>
			<tbody>
				{entries.map(e => (
					<tr key={e.id} onClick={() => selectRow(e.id)} className={e.selected ? "selected" : null}>
						<td>{e.id}</td>
						<td>{e.description}</td>
						<td>{e.number}</td>
						<td>{e.last_modif}</td>
					</tr>)
				)}
			</tbody>
		</table>
	);
}

