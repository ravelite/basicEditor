#include "superwordhighlighter.h"

SuperWordHighlighter::SuperWordHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document)
{
    HighlightingRule rule;

    classFormat.setForeground(Qt::darkBlue);
    classFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bPbind\\b" << "\\bPloop\\b" << "\\bPseries\\b"
                    << "\\bPpatlace\\b" << "\\bPmono\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = classFormat;
        highlightingRules.append(rule);
    }
}

void SuperWordHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}
