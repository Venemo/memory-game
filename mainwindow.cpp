#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "memorygameboard.h"

#if defined(HAVE_OPENGL)
#include <QtOpenGL>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_elapsedSteps(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGraphicsScene *scene = new QGraphicsScene(ui->graphicsView);
    scene->setBackgroundBrush(QBrush(QColor(255, 255, 255, 255)));
    ui->graphicsView->setScene(scene);

    ui->menuBar->hide();

#if defined(HAVE_OPENGL)
    ui->graphicsView->setViewport(new QGLWidget(ui->graphicsView));
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    startGame();
    event->accept();
}

void MainWindow::startGame()
{
    if (m_elapsedSteps && QMessageBox::No == QMessageBox::question(this, tr("Sure?"), tr("Do you wish to abandon the current game?"), QMessageBox::Yes, QMessageBox::No))
        return;

    if (ui->graphicsView->scene())
        ui->graphicsView->scene()->deleteLater();

    MemoryGameBoard *board = new MemoryGameBoard(this);
    ui->graphicsView->setScene(board);
    connect(board, &MemoryGameBoard::gameWon, this, &MainWindow::onGameWon);
    connect(board, &MemoryGameBoard::elapsedStepsChanged, this, &MainWindow::onElapsedStepsChanged);
    board->setBackgroundBrush(QBrush(QColor(255, 255, 255, 255)));
    board->setSceneRect(ui->graphicsView->rect());
    board->startGame();
}

void MainWindow::onGameWon()
{
    QMessageBox::information(this, tr("You rock!"), tr("Congratulations, you have won!"), QMessageBox::Ok);
    m_elapsedSteps = 0;
    startGame();
}

void MainWindow::saveGame()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (!fileName.isEmpty())
    {
        if (QFile::exists(fileName))
            QFile::remove(fileName);

        QFile f(fileName);
        f.open(QIODevice::WriteOnly);
        QDataStream stream(&f);

        MemoryGameBoard *board = static_cast<MemoryGameBoard*>(ui->graphicsView->scene());
        board->saveData(stream);

        f.close();
    }
}

void MainWindow::loadGame()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        if (ui->graphicsView->scene())
            ui->graphicsView->scene()->deleteLater();

        MemoryGameBoard *board = new MemoryGameBoard(this);
        ui->graphicsView->setScene(board);
        connect(board, &MemoryGameBoard::gameWon, this, &MainWindow::onGameWon);
        connect(board, &MemoryGameBoard::elapsedStepsChanged, this, &MainWindow::onElapsedStepsChanged);
        board->setBackgroundBrush(QBrush(QColor(255, 255, 255, 255)));
        board->setSceneRect(ui->graphicsView->rect());

        QFile f(fileName);
        f.open(QIODevice::ReadOnly);
        QDataStream stream(&f);
        board->loadData(stream);
        f.close();
    }
}

void MainWindow::surrender()
{
    MemoryGameBoard *board = static_cast<MemoryGameBoard*>(ui->graphicsView->scene());
    board->surrenderGame();
}

void MainWindow::onElapsedStepsChanged(unsigned n)
{
    m_elapsedSteps = n;
    QString text = tr("Steps so far: %1").arg(QString::number(n));
    ui->stepsLabel->setText(text);
}
