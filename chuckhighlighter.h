#ifndef CHUCKHIGHLIGHTER_H
#define CHUCKHIGHLIGHTER_H

#include "superwordhighlighter.h"

class ChuckHighlighter : public SuperWordHighlighter
{
    Q_OBJECT
public:
    ChuckHighlighter(QTextDocument *document);
};

#endif // CHUCKHIGHLIGHTER_H
