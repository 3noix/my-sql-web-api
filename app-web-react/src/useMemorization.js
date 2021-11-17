import {useState, useEffect} from 'react';


export default function useMemorization(inputValue, memorize) {
	const [memorizedValue, setMemorizedValue] = useState(inputValue);

	useEffect(() => {
		if (memorize) {return;}
		setMemorizedValue(inputValue);
	}, [inputValue,memorize]);

	return memorizedValue;
}

