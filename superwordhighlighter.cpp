#include "superwordhighlighter.h"

SuperWordHighlighter::SuperWordHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document),
    commentStartExpression( "/\\*" ),
    commentEndExpression( "\\*/" )
{

    multiLineCommentFormat.setForeground(QColor::fromHsv(120,255,200));
    multiLineCommentFormat.setFontWeight(QFont::Bold);
}

void SuperWordHighlighter::highlightBlock(const QString &text)
{
    //handle all of the keyword and other rules
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    //handle multiline comments
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }

} //end of highlightBlock

/* add the list of keywords with word boundaries */
void SuperWordHighlighter::keywordPatternsFormat(QStringList &strList,
                                                 QTextCharFormat &textFormat)
{
    HighlightingRule rule;
    foreach (const QString &pattern, strList) {
        rule.pattern = QRegExp( "\\b" + pattern + "\\b");
        rule.format = textFormat;
        highlightingRules.append(rule);
    }
}
