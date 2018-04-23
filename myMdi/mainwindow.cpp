#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mdichild.h"
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QLabel>
#include <QMdiSubWindow>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    readSettings(); // 初始窗口时读取窗口设置信息
    initWindow(); // 初始化窗口
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    MdiChild *child = createMdiChild(); // 创建MdiChild
    // 新建文件
    child->newFile();
   // 显示子窗口
    child->show();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("打开文件")); // 获取文件路径
    if(fileName.isEmpty())
        return;

    //打开文件
    MdiChild *child = createMdiChild(); // 如果没有打开，则新建子窗口
    if (child->loadFile(fileName)) {
        ui->statusBar->showMessage(tr("打开文件成功"), 2000);
        child->show();
    } else {
        child->close();
    }
}

void MainWindow::on_actionSave_triggered()
{
    if(activeMdiChild() && activeMdiChild()->save())
           ui->statusBar->showMessage(tr("文件保存成功"),2000);
}

void MainWindow::on_actionSaveAs_triggered()
{
    if(activeMdiChild() && activeMdiChild()->saveAs())
            ui->statusBar->showMessage(tr("文件保存成功"),2000);
}

void MainWindow::on_actionExit_triggered()// 退出菜单
{
    qApp->closeAllWindows();
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild; // 创建MdiChild部件
    ui->mdiArea->addSubWindow(child); // 向多文档区域添加子窗口，child为中心部件

    // 根据QTextEdit类的是否可以复制信号设置剪切复制动作是否可用
    connect(child,SIGNAL(copyAvailable(bool)),ui->actionCut,
            SLOT(setEnabled(bool)));
    connect(child,SIGNAL(copyAvailable(bool)),ui->actionCopy,
            SLOT(setEnabled(bool)));

    // 每当编辑器中的光标位置改变，就重新显示行号和列号
    connect(child,SIGNAL(cursorPositionChanged()),this,SLOT(showTextRowAndCol()));

    return child;
}

void MainWindow::readSettings()
{
    QSettings settings("yafeiwindows", "myMdi");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(600, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("yafeiwindows", "myMdi");
    settings.setValue("pos", pos());   // 写入位置信息
    settings.setValue("size", size()); // 写入大小信息
}

void MainWindow::initWindow()
{
    setWindowTitle(tr("多文档编辑器"));

    // 我们在工具栏上单击鼠标右键时，可以关闭工具栏
    ui->mainToolBar->setWindowTitle(tr("工具栏"));

    // 当多文档区域的内容超出可视区域后，出现滚动条
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->statusBar->showMessage(tr("欢迎使用多文档编辑器"));

    QLabel *label = new QLabel(this);
    label->setFrameStyle(QFrame::Box | QFrame::Sunken);
    label->setText(tr("<a href=\"http://www.yafeilinux.com\">yafeilinux.com</a>"));
    label->setTextFormat(Qt::RichText); // 标签文本为富文本
    label->setOpenExternalLinks(true);  // 可以打开外部链接
    ui->statusBar->addPermanentWidget(label);

    // 动作的状态提示
    ui->actionNew->setStatusTip(tr("创建一个文件"));
    ui->actionOpen->setStatusTip(tr("打开一个已经存在的文件"));
    ui->actionSave->setStatusTip(tr("保存文档到硬盘"));
    ui->actionSaveAs->setStatusTip(tr("以新的名称保存文档"));
    ui->actionExit->setStatusTip(tr("退出应用程序"));
    ui->actionUndo->setStatusTip(tr("撤销先前的操作"));
    ui->actionRedo->setStatusTip(tr("恢复先前的操作"));
    ui->actionCut->setStatusTip(tr("剪切选中的内容到剪贴板"));
    ui->actionCopy->setStatusTip(tr("复制选中的内容到剪贴板"));
    ui->actionPaste->setStatusTip(tr("粘贴剪贴板的内容到当前位置"));
    ui->actionClose->setStatusTip(tr("关闭活动窗口"));
    ui->actionCloseAll->setStatusTip(tr("关闭所有窗口"));
    ui->actionTile->setStatusTip(tr("平铺所有窗口"));
    ui->actionCascade->setStatusTip(tr("层叠所有窗口"));
    ui->actionNext->setStatusTip(tr("将焦点移动到下一个窗口"));
    ui->actionPrevious->setStatusTip(tr("将焦点移动到前一个窗口"));
    ui->actionAbout->setStatusTip(tr("显示本软件的介绍"));
    ui->actionAboutQt->setStatusTip(tr("显示Qt的介绍"));
}

MdiChild *MainWindow::activeMdiChild()
{
    // 如果有活动窗口，则将其内的中心部件转换为MdiChild类型
    if(QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<MdiChild*>(activeSubWindow->widget());
    return 0;// 没有活动窗口，直接返回0
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows(); // 先执行多文档区域的关闭操作
    if (ui->mdiArea->currentSubWindow()) {
        event->ignore(); // 如果还有窗口没有关闭，则忽略该事件
    } else {
        writeSettings(); // 在关闭前写入窗口设置
        event->accept();
    }
}

void MainWindow::on_actionUndo_triggered()// 撤销菜单
{
    if(activeMdiChild())
        activeMdiChild()->undo();
}

void MainWindow::on_actionRedo_triggered()// 恢复菜单
{
    if(activeMdiChild()) activeMdiChild()->redo();
}

void MainWindow::on_actionCut_triggered()// 剪切菜单
{
    if(activeMdiChild()) activeMdiChild()->cut();
}

void MainWindow::on_actionCopy_triggered() // 复制菜单
{
    if(activeMdiChild()) activeMdiChild()->copy();
}

void MainWindow::on_actionPaste_triggered()// 粘贴菜单
{
    if(activeMdiChild()) activeMdiChild()->paste();
}

void MainWindow::on_actionClose_triggered() // 关闭菜单
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::on_actionCloseAll_triggered()// 关闭所有窗口菜单
{
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::showTextRowAndCol()
{
    // 如果有活动窗口，则显示其中光标所在的位置
    if(activeMdiChild()){

    // 因为获取的行号和列号都是从0开始的，所以我们这里进行了加1
    int rowNum = activeMdiChild()->textCursor().blockNumber()+1;
    int colNum = activeMdiChild()->textCursor().columnNumber()+1;

    ui->statusBar->showMessage(tr("%1行 %2列")
                               .arg(rowNum).arg(colNum),2000);
    }
}

void MainWindow::on_actionNext_triggered()// 下一个菜单
{
    ui->mdiArea->activateNextSubWindow();
}

void MainWindow::on_actionTile_triggered()// 平铺菜单
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actionCascade_triggered()// 层叠菜单
{
    ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actionPrevious_triggered()// 前一个菜单
{
    ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::on_actionAbout_triggered() // 关于菜单
{
     QMessageBox::about(this,tr("关于本软件"),tr("欢迎访问我们的网站：www.yafeilinux.com"));
}

void MainWindow::on_actionAboutQt_triggered()// 关于Qt菜单
{
    qApp->aboutQt(); // 这里的qApp是QApplication对象的全局指针,这行代码相当于QApplication::aboutQt();
}



