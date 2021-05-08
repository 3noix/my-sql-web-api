import {useState, useCallback} from 'react';


export default function useEntries() {
	const [entries, setEntries] = useState([]);

	const appendEntry = useCallback(newEntry => {
		setEntries(prevEntries => [...prevEntries, newEntry]);
	},[]);

	const updateEntry = useCallback(updatedEntry => {
		setEntries(prevEntries => prevEntries.map(
			e => {
				if (e.id !== updatedEntry.id) {return e;} // no modif
				return {
					id: e.id,
					description: updatedEntry.description,
					number: updatedEntry.number,
					last_modif: updatedEntry.last_modif
				};
			}
		));
	},[]);

	const deleteEntry = useCallback(entryId => {
		setEntries(prevEntries => prevEntries.filter(e => e.id !== entryId));
	},[]);

	const setAllEntries = useCallback(data => {
		setEntries(data);
	},[]);

	return {entries, setAllEntries, appendEntry, updateEntry, deleteEntry};
}

