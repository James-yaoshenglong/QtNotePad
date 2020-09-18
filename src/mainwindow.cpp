#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QTimer>
//#include <QTextDocument>
//#include <QTextBlock>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //这一行的作用是设置text edit为中心widget，以去除左右边缘空白
    this->setCentralWidget(ui->textEdit);

    //设置新建窗口标题
    MainWindow::setWindowTitle("Plain text -- QtNotepad");

    //添加行号，列号，行数
    ui->statusbar->addPermanentWidget(ui->label_line_count);
    ui->statusbar->addPermanentWidget(ui->label_index);
    ui->label_index->setText(QString("line : ")+QString::number(ui->textEdit->textCursor().blockNumber()+1)+QString(" column : ")+QString::number(ui->textEdit->textCursor().columnNumber()+1));
    ui->label_line_count->setText(QString("Total line(s): ")+QString::number(ui->textEdit->document()->lineCount()));

    //之后想办法添加一个slot，在光标变化的时候进行变化显示
    //注意connect的使用,用SIGnal的时候要加（）,直接用名字的时候不用加（）,加了作用对象之后就不要加-》了,这里虽然实际变化者是label，但是slot在本类中，就要用this
    connect(ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(on_cursor_position_changed()));

    //用q text edit的line wrap mode选项搞横向滚动条

    //用定时器每隔10S遍历一遍文本，将错误的文本进行红色或者下划线标注
    timer = new QTimer(this);
    timer->start(1000);
    timer->setInterval(10000);
    connect(timer,SIGNAL(timeout()),this,SLOT(checkText()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionNew_triggered()
{
    //搞清楚每个函数实际上要做什么
    file_path = ""; //reset file path 作为text edit的新的文件
    ui->textEdit->setText("");
    //设置新建窗口标题
    MainWindow::setWindowTitle("Plain text -- QtNotepad");
}

void MainWindow::on_actionOpen_triggered()
{
    //注意这个Qfile读取的机制
    QString file_name = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(file_name);
    if(!file.open(QFile::ReadOnly | QFile::Text)){//这里会做一次open操作
        //QMessageBox::warning(this, "..", "File is not open!");
        ui->statusbar->showMessage("File is not open!", 2000);
        return;
    }
    //设置新建窗口标题
    MainWindow::setWindowTitle(file_name+" -- QtNotepad");
    file_path = file_name;
    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setText(text);
    file.close();
}

void MainWindow::on_actionSave_triggered()
{
    //这里是否要判断是否要添加判断是否能调用save，是否要再调用save as
    if(file_path.isEmpty()){
        on_actionSave_as_triggered();
        return;
    }
    //
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "..", "File is not open!");
        return;
    }
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();//注意这个函数的名字
    out << text;
    file.flush();//不要忘记，研究一下flush的功能
    file.close();
}

void MainWindow::on_actionSave_as_triggered()
{
    //save和save as 区别在于是否能已经打开来一个已有的文件
    //注意这个Qfile读取的机制
    QString file_name = QFileDialog::getSaveFileName(this, "Save the file as"); //这个和open的区别在于这个要自己在dialog中提供filename，然后返回出来，并不会做保存的事情
    QFile file(file_name);
    //这个功能用status bar实现更好
    if(!file.open(QFile::WriteOnly | QFile::Text)){//这个的功能再于当打开对话框的时候如果没打开文件就关掉了，会报错
        //QMessageBox::warning(this, "..", "File is not open!");
        ui->statusbar->showMessage("File isn't saved！", 2000);
        return;
    }
    //设置新建窗口标题
    MainWindow::setWindowTitle(file_name+" -- QtNotepad");
    file_path = file_name;
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();//注意这个函数的名字
    out << text;
    file.flush();//不要忘记，研究一下flush的功能
    file.close();
}

void MainWindow::on_actionCut_triggered()
{
    //text edit自带这个cut的功能的函数，将选中内容复制到剪贴板，paste和copy类似，而且自带的快捷键也起作用，但是和这个不一样，快捷键出发的是槽函数，而不是这个成员函数
    ui->textEdit->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}

void MainWindow::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}

void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}

void MainWindow::on_actionSelect_All_triggered()
{
    ui->textEdit->selectAll();
}

void MainWindow::on_actionAbout_Notepad_triggered()
{
    QString about_text;
    about_text =  "Author : James YAO\n";
    about_text += "Date   : 2020/8/26\n";
    about_text += "(C) NotePad : (R)\n";
    QMessageBox::about(this, "About NotePad", about_text);
}

void MainWindow::on_cursor_position_changed()
{
    //注意这个number转为Qstring的方法，注意+1的使用是因为这些position都是从0开始的
    ui->label_index->setText(QString("line : ")+
                             QString::number(ui->textEdit->textCursor().blockNumber()+1)+
                             QString(" column : ")+
                             QString::number(ui->textEdit->textCursor().columnNumber()+1));
    //这里下一次可以添加检测总词数，总行数，是否修改的内容
    ui->label_line_count->setText(QString("Total line(s): ")+QString::number(ui->textEdit->document()->lineCount()));
}


void MainWindow::checkText(){
    //这个事情现在用cursor+select来解决了
    QTextCursor it = ui->textEdit->textCursor();
    it.movePosition(QTextCursor::Start);
    while(!it.atEnd()){
        it.select(QTextCursor::WordUnderCursor); //这个cursor一move就没有用了
        qDebug()<<it.selectedText(); //qdebug会自动换行
        QTextCharFormat fmt;
        fmt.setForeground(Qt::red);
        it.setCharFormat(fmt);
        it.movePosition(QTextCursor::NextWord); //这个比较玄学，下一个如果是标点符号中间加空格，就会选择标点符号
    }

}

