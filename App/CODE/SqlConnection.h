#ifndef SQL_CONNECTION
#define SQL_CONNECTION


#include <QString>


class SqlConnection
{
	public:
		explicit SqlConnection();
		SqlConnection(const SqlConnection &other) = delete;
		SqlConnection(SqlConnection &&other) = delete;
		SqlConnection& operator=(const SqlConnection &other) = delete;
		SqlConnection& operator=(SqlConnection &&other) = delete;
		~SqlConnection();
		
		bool isConnected() const;
		bool connectToDatabase();
		void disconnectFromDatabase();
		
		
	private:
		bool m_bConnected;
};


#endif

