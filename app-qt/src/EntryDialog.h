#ifndef ENTRY_DIALOG
#define ENTRY_DIALOG


#include <QDialog>

class QGridLayout;
class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QCloseEvent;


class EntryDialog : public QDialog
{
	Q_OBJECT
	
	public:
		explicit EntryDialog(QWidget *parent = nullptr);
		EntryDialog(const EntryDialog &other) = delete;
		EntryDialog(EntryDialog &&other) = delete;
		EntryDialog& operator=(const EntryDialog &other) = delete;
		EntryDialog& operator=(EntryDialog &&other) = delete;
		virtual ~EntryDialog() = default;
		
		QString description() const;
		int number() const;
		
		void setDescription(const QString &desc);
		void setNumber(int i);
		
		
	private slots:
		void slotBoutonOk();
		void slotBoutonCancel();
		
		
	protected:
		virtual void closeEvent(QCloseEvent *event) override final;
		virtual void keyPressEvent(QKeyEvent *event) override final;
		
		
	private:
		void setupWidget();
		
		QGridLayout *layout;
		QLabel *labelDescription, *labelNumber;
		QLineEdit *lineDescription;
		QSpinBox *spinNumber;
		QPushButton *boutonOk, *boutonCancel;
};


#endif

