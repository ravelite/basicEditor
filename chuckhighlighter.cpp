#include "chuckhighlighter.h"

ChuckHighlighter::ChuckHighlighter(QTextDocument *document):
    SuperWordHighlighter(document)
{
    /* HighlightingRule rule;
    QTextCharFormat classFormat;

    classFormat.setForeground(Qt::darkBlue);
    classFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bPbind\\b" << "\\bPloop\\b" << "\\bPseries\\b"
                    << "\\bPpatlace\\b" << "\\bPmono\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = classFormat;
        highlightingRules.append(rule);
    } */

    //DO COMMENTS HERE

    //DO KEYWORDS HERE

    QTextCharFormat kw1Format;
    kw1Format.setForeground(Qt::darkBlue);
    kw1Format.setFontWeight(QFont::Bold);

    QStringList kw1Patterns;
    kw1Patterns << "int" << "float" << "time" << "dur" << "void" << "same";

    kw1Patterns << "if" << "else" << "while" << "until" << "for" << "repeat"
                  << "break" << "continue" << "return" << "switch";

    kw1Patterns << "class" <<
                   "extends" <<
                   "public" <<
                   "static" <<
                   "pure" <<
                   "this" <<
                   "super"  <<
                   "interface" <<
                   "implements"  <<
                   "protected"  <<
                   "private";

    kw1Patterns << "function" <<
                   "fun" <<
                   "spork" <<
                   "const" <<
                   "new";

    kw1Patterns << "now" <<
                   "true" <<
                   "false" <<
                   "maybe" <<
                   "null" <<
                   "NULL" <<
                   "me" <<
                   "pi";

    kw1Patterns << "samp" <<
                   "ms" <<
                   "second" <<
                   "minute" <<
                   "hour" <<
                   "day" <<
                   "week";

    kw1Patterns << "dac" <<
                   "adc" <<
                   "blackhole";

    keywordPatternsFormat(kw1Patterns, kw1Format);

    //single line comment rule
    HighlightingRule rule;
    QTextCharFormat singleLineCommentFormat;
    //singleLineCommentFormat.setForeground(Qt::darkGreen);
    singleLineCommentFormat.setForeground(QColor::fromHsv(120,255,200));
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    //multiline comments require state
    //http://doc.qt.io/qt-5/qtwidgets-richtext-syntaxhighlighter-example.html

    //string rule
    /* quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.pattern.setMinimal(true);
    rule.format = quotationFormat;
    highlightingRules.append(rule); */

    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::gray);
    rule.pattern = QRegExp("\"(?:(?!\\/\\/).)+\""); //QRegExp("\".*\"");

    rule.pattern.setMinimal(true);
    rule.format = quotationFormat;
    highlightingRules.append(rule);

} //end of ChuckHighlighter constructor

