#ifndef QUNFIGURATOR_H
#define QUNFIGURATOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>

enum type
{
    LINEEDIT = 0,
    SPINBOX,
    TEXTEDIT,
    NOGUI,
};

typedef struct
{
    bool exists(QObject* obj) {if (obj == m_obj) return true; return false;}

    QString m_name;     // name in the file to search for
    QObject* m_obj;     // pointer to the actual object
    QChar* m_data;      // actual pointer to the first char of data
    qint16 m_length;    // length of the text line extracted
    qint16 m_position;  // where pointer is referencing in the file
    type m_type;        // copy of the text this object is holding
    QString m_text;     // copy of the text this object is holding
    QLabel* m_dbg;      // debug label of this object

} qpair;



namespace Ui {
class Qunfigurator;
}

class Qunfigurator : public QMainWindow
{
    Q_OBJECT

    enum MSGTYP
    {
        ERROR = 0,
        INFO,
        WARN
    };

public:
    explicit Qunfigurator(QWidget *parent = 0);
    ~Qunfigurator();

public slots:
    void setPath();
    void setFile();
    void parseFile();
    void setAllFree();
    void generateFiles();
    void addRow();
    void delRow();
    void filterTable(QString str);

signals:
    void Unleash();


private:
    Ui::Qunfigurator *ui;
    QFileDialog m_file;
    QFileDialog m_dir;
    QString m_user_cfg, m_user_postload, m_makefiledefs_flexus;
    QString m_user_cfg_path, m_user_postload_path, m_makefiledefs_flexus_path;

    void logg(const QString& text, MSGTYP ty);


    void parseUserPostloadFile();

    void read(QString filename, QString& intxt);
    void write(QString filename, QString outtxt);
    void populateFields();
    void processTextEdit(qpair* p);
    void updatePair(qpair* p, QString data="");
    qpair* getPair(QObject* obj);

    QList<qpair*> m_items;

    QString m_outt;


};

#endif // QUNFIGURATOR_H
