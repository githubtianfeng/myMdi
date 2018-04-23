#include "mdichild.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPushButton>
#include <QMenu>

MdiChild::MdiChild(QWidget *parent) :
    QTextEdit(parent)
{
    setAttribute(Qt::WA_DeleteOnClose); // 这样可以在子窗口关闭时销毁这个类的对象
    isUntitled = true;  // 初始isUntitled为true
}

void MdiChild::newFile()
{
    // 设置窗口编号，因为编号一直被保存，所以需要使用静态变量
    static int sequenceNumber = 1;
    // 新建的文档没有被保存过
    isUntitled = true;
    // 将当前文件命名为未命名文档加编号，编号先使用再加1
    curFile = tr("未命名文档%1.txt").arg(sequenceNumber++);

    // 设置窗口标题，使用[*]可以在文档被更改后在文件名称后显示”*“号
    setWindowTitle(curFile + "[*]" + tr(" - 多文档编辑器"));

    // 当文档被更改时发射contentsChanged()信号，
    // 执行我们的documentWasModified()槽函数
    connect(document(), SIGNAL(contentsChanged()),this, SLOT(documentWasModified()));
}

bool MdiChild::loadFile(const QString &fileName)
{
    //打开文件
    QFile file(fileName);
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&file);// 新建文本流对象
        QApplication::setOverrideCursor(Qt::WaitCursor); // 设置鼠标状态为等待状态
        setPlainText(file.readAll());  // 读取文件的全部文本内容，并添加到编辑器中
        QApplication::restoreOverrideCursor(); // 恢复鼠标状态

        setCurrentFile(fileName); // 设置当前文件

        connect(document(), SIGNAL(contentsChanged()),
                    this, SLOT(documentWasModified()));
        file.close();
        return true;
    }
    else
    {
        //打开失败
        QMessageBox::critical(this,tr("error"),file.errorString());
        return false;
    }
}

bool MdiChild::save()
{
    if (isUntitled) { // 如果文件未被保存过，则执行另存为操作
        return saveAs();
    } else {
        return saveFile(curFile); // 否则直接保存文件
    }
}

bool MdiChild::saveAs()
{
    // 获取文件路径，如果为空，则返回false
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"),curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName); // 否则保存文件
}

bool MdiChild::saveFile(const QString &fileName)
{
    if(fileName.isEmpty())
        return false;
    QFile file(fileName);
    if(file.open(QFile::WriteOnly | QFile::Text))   //以写的方式打开文件 若文件有内容 则追加内容
    {
        QTextStream stream(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        stream<<toPlainText();    //将text的内容转换为QString后，写入文件
        QApplication::restoreOverrideCursor();
        file.close();
        return true;
    }

    QMessageBox::critical(this,tr("error"),file.errorString());
    return false;
}

void MdiChild::closeEvent(QCloseEvent *event)
{
    //查看qt助帮文档 QMessageBox
    if(document()->isModified())  //textEdit内容已经被修改
    {
        QMessageBox msgBox;
     //   msgBox.windowTitle(tr("多文档编辑器"));
        msgBox.setText(tr("是否保存对“%1”的更改？"));
        msgBox.setInformativeText(tr("您是否要保存文件"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();    //执行QMessageBox

        switch (ret) {
          case QMessageBox::Save:
              // Save was clicked
            this->save();
            break;
          case QMessageBox::Discard:
              // Don't Save was clicked
            event->accept();
              break;
          case QMessageBox::Cancel:
              // Cancel was clicked
            event->ignore();
              break;
          default:
              // should never be reached
            event->ignore();
              break;
        }
    }
    else
    {
        event->accept();
    }
}

void MdiChild::documentWasModified()
{
    // 根据文档的isModified()函数的返回值，判断我们编辑器内容是否被更改了
    // 如果被更改了，就要在设置了[*]号的地方显示“*”号，这里我们会在窗口标题中显示
    setWindowModified(document()->isModified());
}

void MdiChild::setCurrentFile(const QString &fileName)
{
    // canonicalFilePath()可以除去路径中的符号链接，“.”和“..”等符号
    curFile = QFileInfo(fileName).canonicalFilePath();

    isUntitled = false; // 文件已经被保存过了
    document()->setModified(false); // 文档没有被更改过
    setWindowModified(false); // 窗口不显示被更改标志

    // 设置窗口标题，QFileInfo()返回文件名
    setWindowTitle(QFileInfo(curFile).fileName() + "[*]");

}


