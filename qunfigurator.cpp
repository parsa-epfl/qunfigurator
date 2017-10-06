#include "qunfigurator.h"
#include "ui_qunfigurator.h"
#include <QMessageBox>
Qunfigurator::Qunfigurator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Qunfigurator)
{
    ui->setupUi(this);

    m_dir.setFileMode(QFileDialog::DirectoryOnly);
    m_file.setFileMode(QFileDialog::AnyFile);

    m_items         << new qpair{ "",                       ui->qflex_le,       nullptr,0,0, LINEEDIT,"",  ui->qflex_dbg}
                    << new qpair{ "GCC_PATH ",                   ui->gcc_le,         nullptr,0,0, LINEEDIT,"",  ui->gcc_dbg}
                    << new qpair{"BOOST_PATH ",                  ui->boost_le,       nullptr,0,0, LINEEDIT,"",  ui->boost_dbg}

                    << new qpair{"FLEXUS_CORE_NUM ",        ui->flexus_sb,      nullptr,0,0, SPINBOX,"",   ui->flexus_dbg}
                    << new qpair{"QEMU_CORE_NUM ",          ui->qcpu_sb,        nullptr,0,0, SPINBOX,"",   ui->qcpu_dbg}
                    << new qpair{"MEM ",                    ui->mem_sb,         nullptr,0,0, SPINBOX,"",   ui->mem_dbg}

                    << new qpair{"SIMULATE_TIME ",          ui->sim_sb,         nullptr,0,0, SPINBOX,"",   ui->sim_dbg}

                 //   << new qpair{"QEMU_PATH ",              nullptr,            nullptr,0,0, NOGUI,"",     nullptr}
                    << new qpair{"QEMU_PATH ",               ui->qemu_le,        nullptr,0,0, LINEEDIT,"",  ui->qemu_dbg}

                    << new qpair{"FLEXUS_PATH ",            nullptr,            nullptr,0,0, NOGUI,"",     nullptr}
                    << new qpair{"FLEXUS_REPO ",            nullptr,            nullptr,0,0, NOGUI,"",     nullptr}

                    << new qpair{"KERNEL_PATH ",            nullptr,            nullptr,0,0, NOGUI,"",     nullptr}
                    << new qpair{"KERNEL ",                 ui->kernel_le,      nullptr,0,0, LINEEDIT,"",  ui->kernel_dbg}
                    << new qpair{"INITRD ",                 ui->initramfs_le,   nullptr,0,0, LINEEDIT,"",  ui->initramfs_dbg}

                    << new qpair{"IMG_0 ",                  ui->images_le,      nullptr,0,0, LINEEDIT,"",  ui->images_dbg}
                    << new qpair{"IMG_1 ",                  nullptr,            nullptr,0,0, NOGUI,"",     nullptr}


                    << new qpair{"ADD_TO_PATH ",            ui->path_te,        nullptr,0,0, TEXTEDIT,"",  ui->path_dbg}
                    << new qpair{"ADD_TO_LD_LIBRARY_PATH ", ui->lib_te,         nullptr,0,0, TEXTEDIT,"",  ui->lib_dbg}
                    << new qpair{"PATH ",                   nullptr,            nullptr,0,0, NOGUI,"",     nullptr}
                    << new qpair{"LD_LIBRARY_PATH ",        nullptr,            nullptr,0,0, NOGUI,"",     nullptr}

                    << new qpair{m_user_cfg,                  nullptr,            nullptr,0,0, NOGUI,"",     nullptr};

    QStringList headers;
    headers <<"Name"<<"Value"<<"Describition";
    ui->flexusConfWidget->setHorizontalHeaderLabels(headers);

}


void Qunfigurator::setAllFree()
{
    for (QObject* obj: ui->centralWidget->children())
    {
        if (obj->isWidgetType())
        {
        QWidget* wg = qobject_cast<QWidget*> (obj);
        wg->setEnabled(true);
        }
    }
}

Qunfigurator::~Qunfigurator()
{
    delete ui;
}

qpair* Qunfigurator::getPair(QObject* obj)
{
    for (qpair* p : m_items)
        if (p->exists(obj)) return p;
    return nullptr;
}

void Qunfigurator::generateFiles()
{

}

void Qunfigurator::updatePair(qpair* p, QString data)
{
    if (p->m_obj == ui->qflex_le)
        return;
    qint16 index = m_user_cfg.indexOf(p->m_name);

    qint16 endex = m_user_cfg.indexOf("\n",index);
    qint16 correctedIndex = index + p->m_name.size();
    p->m_position = correctedIndex;
    p->m_data = m_user_cfg.begin()+correctedIndex;
    p->m_length = endex - correctedIndex;



    switch(p->m_type)
    {
    case LINEEDIT:
    {
        QLineEdit* le = qobject_cast<QLineEdit*> (p->m_obj);

        if (data == ""){
            le->setText(m_user_cfg.mid(correctedIndex, endex-correctedIndex ));
            le->setToolTip(le->text());
        }
        else
        {
            if (p->m_name != "")
            {
                le->setText(data);
                m_user_cfg.replace((m_user_cfg.mid(correctedIndex, endex-correctedIndex )), data);
            }
        }
        break;
    }
    case SPINBOX:
    {
        QSpinBox* sb = qobject_cast<QSpinBox*> (p->m_obj);

        if (data == "")
            sb->setValue(m_user_cfg.mid(correctedIndex, endex-correctedIndex ).toInt());
        else
        {
            sb->setValue(data.toInt());
            m_user_cfg.replace((m_user_cfg.mid(correctedIndex, endex-correctedIndex )), data);
        }
        break;
    }
    case TEXTEDIT:
    {
        QTextEdit* te = qobject_cast<QTextEdit*> (p->m_obj);

        if (data == "")
        {
            p->m_text = m_user_cfg.mid(correctedIndex, endex-correctedIndex);
            te->setText(p->m_text);
        }
        else
        {
            p->m_text = data;
            te->setText(data);
            m_user_cfg.replace((m_user_cfg.mid(correctedIndex, endex-correctedIndex )), data);
        }
        processTextEdit(p);
        break;
    }
    case NOGUI:
    {
        p->m_text = m_user_cfg.mid(correctedIndex, endex-correctedIndex );
        break;
    }
    default:
        break;

    }
}

void Qunfigurator::setFile()
{
    QString fileName = m_file.getOpenFileName(this,
        tr("Choose File"));
    QString signalsender = QObject::sender()->objectName();
    qint16 index = signalsender.indexOf("_btn");
    signalsender = signalsender.left(index);

    QString signalreciever = signalsender + "_le";
    QLineEdit* le = findChild<QLineEdit*>(signalreciever);

    qpair* p = getPair(le);

    updatePair(p, fileName);
}

void Qunfigurator::setPath()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "~/",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    QString signalsender = QObject::sender()->objectName();
    qint16 index = signalsender.indexOf("_btn");
    signalsender = signalsender.left(index);

    QString signalreciever = signalsender + "_le";
    QLineEdit* le = findChild<QLineEdit*>(signalreciever);


    if (sender() == ui->qflex_btn)
    {
        //sanity check
        index = dirname.lastIndexOf("qflex");
        qpair* p = getPair(le);

        if (index != -1)
        {
            if (index != (dirname.size()-5))
            {
                p->m_dbg->setText("Wrong path - path should end with qflex");
                return;
            }
            else
            {
                le->setText(dirname);
                le->returnPressed();
                updatePair(p, dirname);
                emit Unleash();
                return;
            }

        }
        else
        {
            p->m_dbg->setText("Wrong path - path should end with qflex");
            return;
        }
    }
    le->setText(dirname);

}

void Qunfigurator::processTextEdit(qpair* p)
{
    //string lists
    QStringList list = p->m_text.split(":");
    for (QString& l : list)
    {
        qint16 index;
        if (l.indexOf("gcc") != -1)
        {
            index = l.lastIndexOf("/");
            l = l.mid(0, index);
            ui->gcc_le->setText(l);
        }
        else if (l.indexOf("boost")!= -1)
        {
            index = l.lastIndexOf("/");
            l = l.mid(0, index);
            ui->boost_le->setText(l);
        }
        else if (l.indexOf("softmmu")!= -1)
        {
            ui->qemu_le->setText(l);
        }
    }
}

void Qunfigurator::populateFields()
{
    for (qpair* p : m_items)
    {
        qint16 index = m_user_cfg.indexOf(p->m_name);
        if (index != -1)
        {
           updatePair(p);
        }
    }
}

void Qunfigurator::logg(const QString& text, MSGTYP ty = INFO)
{
    QColor col;
    switch(ty)
    {
        case MSGTYP::ERROR:
        ui->output_txt->setTextColor(col.red());
        ui->output_txt->append(text);
            break;
        case MSGTYP::INFO:
        ui->output_txt->setTextColor(col.black());
        ui->output_txt->append(text);
            break;
        case MSGTYP::WARN:
        ui->output_txt->setTextColor(col.red());
        ui->output_txt->append(text);
            break;
    }
}

void Qunfigurator::parseFile()
{

    QLineEdit* le = qobject_cast<QLineEdit*>(sender());

    if (QFile::exists(le->text() + "/scripts/user.cfg")){
         m_user_cfg_path = le->text() + "/scripts/user.cfg";
         m_outt = QString("Choosing config file: %1").arg(m_user_cfg_path);
    }
    else if (QFile::exists(le->text() + "/scripts/user_example.cfg")){
        m_user_cfg_path = le->text() + "/scripts/user_example.cfg";
        m_outt = QString("Choosing config file: %1 - will create a user.cfg for you!").arg(m_user_cfg_path);
    }
    else
    {
        m_user_cfg_path = le->text() + "/scripts/";
        QString t = QString("No user_postload file found under %1").arg(m_user_cfg_path);
        logg(t, MSGTYP::ERROR);
    }

    logg(m_outt, MSGTYP::INFO);

    read(m_user_cfg_path, m_user_cfg);
    populateFields();
{
    if (QFile::exists(le->text() + "/scripts/config/user_postload")){
        m_user_postload_path = le->text() + "/scripts/config/user_postload";
        m_outt = QString("Choosing user_postload file: %1.").arg(m_user_postload_path);
        logg(m_outt, MSGTYP::INFO);
    }
    else
    {
        m_outt = QString("No user_postload file found under: %1.").arg(m_user_postload_path);
        logg(m_outt, MSGTYP::INFO);
    }
}

    parseUserPostloadFile();


    {
        if (QFile::exists(le->text() + "/flexus/makefile.defs")){
            m_makefiledefs_flexus_path = le->text() + "/flexus/makefile.defs";
            m_outt = QString("Choosing Flexus makefile.defs file: %1.").arg(m_makefiledefs_flexus_path);
            logg(m_outt, MSGTYP::INFO);
        }
        else
        {
            m_outt = QString("No Flexus makefile.defs file under: %1.").arg(m_makefiledefs_flexus_path);
            logg(m_outt, MSGTYP::ERROR);
        }
    }
}



void Qunfigurator::parseUserPostloadFile()
{


    QString reg = "\"(.*?)\"";
    QRegularExpression re(reg);

    QString reg2("\#(.*)");
    QRegularExpression re2(reg2);

    read(m_user_postload_path, m_user_postload);

    QStringList strl = m_user_postload.split('\n');

    for (QString& s : strl)
    {

        QRegularExpressionMatchIterator match = re.globalMatch(s);
        QRegularExpressionMatch m = match.next();
        QString attrib_name = m.captured(1);

        m = match.next();
        QString attrib_value = m.captured(1);

        QRegularExpressionMatchIterator match2 = re2.globalMatch(s);
        QRegularExpressionMatch m2 = match2.next();
        QStringList a = m2.capturedTexts();

        QString attrib_comment = m2.captured(1);


        QTableWidgetItem *name, *value, *comment;
        name = new QTableWidgetItem(attrib_name);
        value = new QTableWidgetItem(attrib_value);
        comment =new QTableWidgetItem(attrib_comment);
        ui->flexusConfWidget->insertRow( ui->flexusConfWidget->rowCount() );
        ui->flexusConfWidget->setItem(ui->flexusConfWidget->rowCount()-1, 0, name);
        name->setToolTip(attrib_name);
        ui->flexusConfWidget->setItem(ui->flexusConfWidget->rowCount()-1, 1, value);
        value->setToolTip(attrib_value);
        ui->flexusConfWidget->setItem(ui->flexusConfWidget->rowCount()-1, 2, comment);
        comment->setToolTip(attrib_comment);


    }
}

void Qunfigurator::addRow()
{
    ui->flexusConfWidget->insertRow( ui->flexusConfWidget->rowCount() );
    ui->flexusConfWidget->sortItems(0);
}

void Qunfigurator::delRow()
{
    ui->flexusConfWidget->removeRow(ui->flexusConfWidget->currentRow());
}


void Qunfigurator::filterTable(QString str)
{
    int rowCount = ui->flexusConfWidget->rowCount();

    if (str == "")
    {
        for (int i =0; i< rowCount; i++)
        {
                ui->flexusConfWidget->showRow(i);
        }
    }
    else
    {
        for (int i =0; i< rowCount; i++)
        {
            if (ui->flexusConfWidget->item(i,0)->text().indexOf(str) == -1)
            {
                ui->flexusConfWidget->hideRow(i);
            }
            else
            {
                ui->flexusConfWidget->showRow(i);
            }
        }
    }
}

void Qunfigurator::write(QString filename, QString outtxt)
{
    QFile file(filename);
    // Trying to open in WriteOnly and Text mode
    if(!file.open(QFile::WriteOnly |
                  QFile::Text))
    {
        qDebug() << " Could not open file for writing";
        return;
    }

    // To write text, we use operator<<(),
    // which is overloaded to take
    // a QTextStream on the left
    // and data types (including QString) on the right

    QTextStream out(&file);
    out << outtxt;
    file.flush();
    file.close();
}

void Qunfigurator::read(QString filename, QString& intxt)
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly |
                  QFile::Text))
    {
        qDebug() << " Could not open the file for reading";
        return;
    }

    QTextStream in(&file);
    intxt = in.readAll();
   // qDebug() << intxt;

    file.close();
}
