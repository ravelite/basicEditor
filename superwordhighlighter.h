#ifndef SUPERWORDHIGHLIGHER_H
#define SUPERWORDHIGHLIGHER_H

#include <QSyntaxHighlighter>

class SuperWordHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    SuperWordHighlighter(QTextDocument *document);

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

protected:
    void highlightBlock(const QString &text);

    void keywordPatternsFormat(QStringList &, QTextCharFormat &);

     QVector<HighlightingRule> highlightingRules;

private:
    //QTextCharFormat classFormat;
    
};

#endif // SUPERWORDHIGHLIGHER_H
