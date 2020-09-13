#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCursor>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionCopy_triggered();

    void on_actionSave_triggered();

    void on_actionSave_as_triggered();

    void on_actionCut_triggered();

    void on_actionPaste_triggered();

    void on_actionRedo_triggered();

    void on_actionUndo_triggered();

    void on_actionAbout_Notepad_triggered();

    void on_actionSelect_All_triggered();

    void on_cursor_position_changed();

    void checkText(); //定时器信号触发的遍历文本函数

private:
    Ui::MainWindow *ui;

    QString file_path ; //用于存储file path，作为几个function之间的交流

    QTimer* timer; //定时器为了搞文本遍历事件
};
#endif // MAINWINDOW_H
