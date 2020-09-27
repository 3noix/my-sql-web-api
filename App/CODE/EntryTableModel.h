#ifndef ENTRY_TABLE_MODEL
#define ENTRY_TABLE_MODEL


#include <QAbstractTableModel>
#include <vector>
#include "../../WebApi/CODE/Entry.h"
#include "EntryChange.h"


class EntryTableModel : public QAbstractTableModel
{
	Q_OBJECT
	
	public:
		enum Columns
		{
			Id = 0,
			Description = 1,
			Number = 2,
			LastModif = 3,
			NbColumns = 4
		};
		
		explicit EntryTableModel(QObject *parent = nullptr);
		EntryTableModel(const EntryTableModel &other) = delete;
		EntryTableModel(EntryTableModel &&other) = delete;
		EntryTableModel& operator=(const EntryTableModel &other) = delete;
		EntryTableModel& operator=(EntryTableModel &&other) = delete;
		virtual ~EntryTableModel() = default;
		
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override final;
		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override final;
		Qt::ItemFlags flags(const QModelIndex &index) const override final;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override final;
		int columnCount(const QModelIndex &parent = QModelIndex{}) const override final;
		int rowCount(const QModelIndex &parent = QModelIndex{}) const override final;
		
		bool processChange(const EntryChange &ch, QString *errorMessage = nullptr);
		
		
	private:
		std::vector<Entry> m_data;
};


#endif

