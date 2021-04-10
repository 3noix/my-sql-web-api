import {useState, useEffect} from 'react';


export default function useHttp(url, dependencies) {
	const [isLoading, setIsLoading] = useState(false);
	const [fetchedData, setFetchedData] = useState(null);

	useEffect(async () => {
		try {
			setIsLoading(true);

			console.log('Sending HTTP request to url: ' + url);
			const response = await fetch(url);
			if (!response.ok) {throw new Error('Failed to fetch');}
			const data = await response.json();

			setIsLoading(false);
			setFetchedData(data);
		}
		catch (error) {
			console.log("Error: " + error);
			setIsLoading(false);
		}
	}, dependencies);
	
	return [isLoading, fetchedData];
}

