## my-sql-web-api-1

This repository is a demo. On the back-end side there is a MySQL database with one table names "Entries" that includes 4 fields:
- `id`: the id of the "entry" (managed by the database)
- `description`: a string field
- `number`: a numeric field
- `last modif`: the date/time of the last modification (managed by the database)

The clients can insert a new entry, remove one or update one. When another client does one of these modifications, the api notifies all the clients through web sockets. All this repository is implement in Typescript. The api and the clients uses only web sockets to communicate (not only for notifications).

Here is a summary of the content:
- `web-api`: a back-end application written with Qt 5 (C++) and that uses only web sockets to communicate with the clients
- `database`: contains a script to initialize the MySQL database
- `app-qt`: a client application written with Qt 5 (C++)
- `app-web`: a client web page written in vanilla Javascript
- `app-web-react`: a client web page written in Javascript with React
- `simple-http-server`: a simple http server (Qt 5, C++) just to host the web page (no need with a good IDE)

Remark: a more recent and cleaner version, written in Typescript and using TRPC is available here: [my-sql-web-api-2](https://github.com/3noix/my-sql-web-api-2).
