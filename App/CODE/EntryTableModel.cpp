#include "EntryTableModel.h"


///////////////////////////////////////////////////////////////////////////////
// RESUME :
//
//  CONSTRUCTEUR
//
//  DATA
//  SET DATA
//  FLAGS
//  HEADER DATA
//  COLUMN COUNT
//  ROW COUNT
//
//  PROCESS CHANGE
///////////////////////////////////////////////////////////////////////////////


// CONSTRUCTEUR ///////////////////////////////////////////////////////////////
EntryTableModel::EntryTableModel(QObject *parent) : QAbstractTableModel{parent}
{
}




// DATA ///////////////////////////////////////////////////////////////////////
QVariant EntryTableModel::data(const QModelIndex &index, int role) const
{
	int row = index.row();
	int col = index.column();
	if (!index.isValid() || row < 0 || row >= this->rowCount()) {return {};}
	
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		if (col == Columns::Id)
			return m_data[row].id;
		else if (col == Columns::Description)
			return m_data[row].description;
		else if (col == Columns::Number)
			return m_data[row].number;
		else if (col = Columns::LastModif)
			return m_data[row].last_modif;
	}
	else if (role == Qt::TextAlignmentRole)
	{
		if (col == Columns::Id || col == Columns::Number) return Qt::AlignCenter;
	}
	
	return {};
}

// SET DATA ///////////////////////////////////////////////////////////////////
bool EntryTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	int row = index.row();
	int col = index.column();
	if (!index.isValid() || row < 0 || row >= this->rowCount()) {return {};}
	
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		if (col == Columns::Description)
		{
			QString newValue = value.toString();
			if (newValue == m_data[row].description) {return false;}
			m_data[row].description = newValue;
		}
		else if (col == Columns::Number)
		{
			int newValue = value.toInt();
			if (newValue == m_data[row].number) {return false;}
			m_data[row].number = newValue;
		}
		
		emit dataChanged(index,index);
		return true;
	}
	
	return false;
}

// FLAGS //////////////////////////////////////////////////////////////////////
Qt::ItemFlags EntryTableModel::flags(const QModelIndex &index) const
{
	Q_UNUSED(index)
	return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

// HEADER DATA ////////////////////////////////////////////////////////////////
QVariant EntryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section < 0 || section > Columns::NbColumns) {return {};}

		if (section == 0) {return "Id";}
		else if (section == 1) {return "Description";}
		else if (section == 2) {return "Number";}
		else if (section == 3) {return "Last modif";}
	}
	else if (orientation == Qt::Horizontal && role == Qt::TextAlignmentRole)
	{
		return Qt::AlignCenter;
	}

	return QAbstractTableModel::headerData(section, orientation, role);
}

// COLUMN COUNT ///////////////////////////////////////////////////////////////
int EntryTableModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) {return 0;}
	return Columns::NbColumns;
}

// ROW COUNT //////////////////////////////////////////////////////////////////
int EntryTableModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) {return 0;}
	return m_data.size();
}




// PROCESS CHANGE /////////////////////////////////////////////////////////////
bool EntryTableModel::processChange(const EntryChange &ch, QString *errorMessage)
{
	// we search for an entry with this id
	int inputId = ch.entry.id;
	auto idMatch = [inputId] (const Entry &e) {return (e.id == inputId);};
	auto it = std::find_if(m_data.begin(),m_data.end(),idMatch);
	bool bMatchFound = (it != m_data.end());

	// some checks + send error messages if wrong inputs
	if (bMatchFound && ch.changeType == ChangeType::Addition)
	{
		if (errorMessage) {*errorMessage = "An entry with ID=" + QString::number(inputId) + " already exists!";}
		return false;
	}
	else if (!bMatchFound && (ch.changeType == ChangeType::Modification || ch.changeType == ChangeType::Deletion))
	{
		if (errorMessage) {*errorMessage = "No match found for ID=" + QString::number(inputId) + "!";}
		return false;
	}

	// do the change
	if (ch.changeType == ChangeType::Addition)
	{
		auto idGreaterThan = [inputId] (const Entry &e) {return (e.id > inputId);};
		auto itGt = std::find_if(m_data.begin(),m_data.end(),idGreaterThan);
		int row = (itGt == m_data.end() ? m_data.size() : itGt - m_data.begin());

		beginInsertRows(QModelIndex{},row,row);
		m_data.insert(itGt,ch.entry);
		endInsertRows();
	}
	else if (ch.changeType == ChangeType::Modification)
	{
		it->description = ch.entry.description;
		it->number = ch.entry.number;

		int row = it - m_data.begin();
		QModelIndex index1 = this->index(row,1,{});
		QModelIndex index2 = this->index(row,2,{});
		emit dataChanged(index1,index2);
	}
	else if (ch.changeType == ChangeType::Deletion)
	{
		int row = it - m_data.begin();
		beginRemoveRows(QModelIndex{},row,row);
		m_data.erase(it);
		endRemoveRows();
	}

	return true;
}

