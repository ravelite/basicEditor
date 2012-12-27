#ifndef TREEVIS_H
#define TREEVIS_H

#include <QWidget>
#include "engine.h"
#include "revision.h"

#include <QMap>
#include <QRect>

class TreeVis : public QWidget
{
    Q_OBJECT
public:
    explicit TreeVis(Engine *, QWidget *parent = 0);
    
signals:
    
public slots:
    void treeChanged();

private:
    Engine *engine;

    QMap<Revision *, bool> isSpinal;
    QMap<Revision *, QRect> layout;
    
    void doLayout();
    void doLayoutRec(Revision *);

    int unitSize;
    float alpha;
};

#endif // TREEVIS_H
