#include <QCoreApplication>
#include <stdio.h>
#include <iostream>

#include "commandline.h"
#include "mnotesnetwork.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("notescli");
    app.setApplicationVersion("0.1");

    CommandLine cmdl;
 //   QCommandLineParser *parser = cmdl.getCmdlParser();
    CmdlOptions options;
    QString cmdError;


    switch (cmdl.checkArguments(app, &options, &cmdError)) {
    case CmdlOk:
        //qDebug() << "alles okydoky";
        cmdl.print("Password:");
        options.password = cmdl.getPassword();
        cmdl.doActions(&options);
        break;
    case CmdlError:
        cmdl.println(qPrintable(cmdError));
        cmdl.println("use -h option to show help");
        exit(0);
    }


     app.exec();
}
