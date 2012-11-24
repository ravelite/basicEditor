#include "mainwindow.h"

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>

#include <iostream>

//MACRO RELATED CODE
void MainWindow::loadMacros()
{
    QScriptEngine engine;
    QFile scriptFile("macros.qs");
    if ( scriptFile.exists() &&
         scriptFile.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        std::cout << "found macros.qs" << std::endl;

        QTextStream stream( &scriptFile );
        QString fileText = stream.readAll();
        scriptFile.close();

        engine.globalObject().setProperty("macros", engine.newObject());
        std::cerr << engine.evaluate(fileText).toString().toStdString() << std::endl;
        macros = qscriptvalue_cast<QVariantMap>( engine.globalObject().property("macros") );
    }
}

QString MainWindow::applyMacros(QString text)
{
    QVariantMap::const_iterator i;
    for (i=macros.constBegin(); i != macros.constEnd(); ++i)
        text = text.replace(i.key(), i.value().toString());

    return text;
}
