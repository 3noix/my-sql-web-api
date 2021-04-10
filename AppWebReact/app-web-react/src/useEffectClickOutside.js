import {useEffect, useRef, useCallback} from 'react';


export default function useEffectClickOutside(domNodeRef, callback, when) {
	const savedCallback = useRef(callback);

	useEffect(() => {
		savedCallback.current = callback;
	});

	const handler = useCallback(event => {
		if (domNodeRef.current && !domNodeRef.current.contains(event.target)) {
			savedCallback.current();
		}
	},[domNodeRef,savedCallback]);

	useEffect(() => {
		if (when) {
			document.addEventListener("mousedown", handler);
			return () => {document.removeEventListener("mousedown", handler);};
		}
	},[when,handler]);
};

