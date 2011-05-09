#include "memorygameboard.h"

MemoryGameBoard::MemoryGameBoard(QObject *parent) :
    QGraphicsScene(parent),
    _rows(2),
    _columns(6),
    _elapsedSteps(0),
    _lastRevealed(0),
    _canReveal(true)
{
}

MemoryCard *MemoryGameBoard::lastRevealed()
{
    return _lastRevealed;
}

void MemoryGameBoard::setLastRevealed(MemoryCard *card)
{
    if (_canReveal)
    {
        if (card == 0)
        {
            _canReveal = false;
            emit elapsedStepsChanged(++_elapsedSteps);
        }
        _lastRevealed = card;
    }
}

bool MemoryGameBoard::canReveal()
{
    return _canReveal;
}

void MemoryGameBoard::enableReveal()
{
    _canReveal = true;
}

QPixmap MemoryGameBoard::paintCard(char c, QColor bg, QColor fg, unsigned width, unsigned height)
{
    QFont font;
    font.setPixelSize(height - 15);
    QPixmap pix(width, height);
    pix.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setFont(font);
    painter.setPen(bg);
    painter.setBrush(bg);
    painter.drawRoundedRect(0, 0, width, height, 10, 10, Qt::RelativeSize);
    painter.setPen(fg);
    painter.drawText(0, 0, width, height, Qt::AlignCenter, QString(c));
    painter.end();
    return pix;
}

void MemoryGameBoard::startGame()
{
    qreal w = (qreal) sceneRect().width() / (qreal)_columns;
    qreal h = (qreal) sceneRect().height() / (qreal)_rows;

    QFont font;
    font.setPixelSize(h - 30);

    QPixmap backPixmap = paintCard('?', QColor(159, 206, 0, 255), QColor(255, 255, 255, 255), w - 20, h - 20);

    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    QList<char> chars = generateChars(_rows * _columns);

    for (unsigned i = 0; i < _rows; i++)
    {
        for (unsigned j = 0; j < _columns; j++)
        {
            char ch = chars[qrand() % chars.count()];
            chars.removeOne(ch);

            MemoryCard *card = new MemoryCard(paintCard(ch, QColor(230, 230, 230, 255), QColor(20, 20, 20, 255), w - 20, h - 20),
                                              backPixmap,
                                              this,
                                              ch);
            QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(card);
            effect->setColor(QColor(0, 0, 0, 200));
            effect->setOffset(0);
            effect->setBlurRadius(10);
            card->setGraphicsEffect(effect);

            items.append(card);
            connect(card, SIGNAL(matched()), this, SLOT(cardMatched()));
            card->show();

            QPropertyAnimation *animation = new QPropertyAnimation(card, "pos", this);
            animation->setStartValue(QPointF(j * w + 10, -100));
            animation->setEndValue(QPointF(j * w + 10, i * h + 10));
            animation->setDuration(1000);
            animation->setEasingCurve(QEasingCurve::OutBounce);
            group->addAnimation(animation);
        }
    }

    emit elapsedStepsChanged(0);
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void MemoryGameBoard::surrenderGame()
{

}

void MemoryGameBoard::cardMatched()
{
    items.removeOne((MemoryCard*)sender());
    if (items.count() == 0)
        emit gameWon();
}

QList<char> MemoryGameBoard::generateChars(unsigned n)
{
    QList<char> chars;
    unsigned i = 0;
    for (char c = '0'; i < n && c <= '9'; c+=1, i+=2)
    {
        chars.push_back(c);
        chars.push_back(c);
    }
    for (char c = 'A'; i < n && c <= 'Z'; c+=1, i+=2)
    {
        chars.push_back(c);
        chars.push_back(c);
    }
    return chars;
}