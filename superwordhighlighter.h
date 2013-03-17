#ifndef SUPERWORDHIGHLIGHER_H
#define SUPERWORDHIGHLIGHER_H

#include <QSyntaxHighlighter>

class SuperWordHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    SuperWordHighlighter(QTextDocument *document);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QTextCharFormat classFormat;

    QVector<HighlightingRule> highlightingRules;
    
};

#endif // SUPERWORDHIGHLIGHER_H
