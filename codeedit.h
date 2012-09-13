#ifndef CODEEDIT_H
#define CODEEDIT_H

#include <QTextEdit>

class CodeEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit CodeEdit(QWidget *parent = 0);

protected:
    void wheelEvent(QWheelEvent *e);
    
signals:
    
public slots:
    
};

#endif // CODEEDIT_H
