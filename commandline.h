#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QObject>
#include <QCommandLineParser>
#include <QDebug>
#include <QUrl>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>


#include "mnotesnetwork.h"

#include <iostream>
#include <curses.h>
#include <stdio.h>

enum CmdlResult{
    CmdlOk,
    CmdlError
};

struct CmdlOptions
{
    QString user;
    QString password;
    QString url;
    QString action;
    QString file;
};

class CommandLine : public QObject
{
    Q_OBJECT
public:

    explicit CommandLine(QObject *parent = nullptr);
    CmdlResult checkArguments(const QCoreApplication &app, CmdlOptions *options, QString *cmdlError);
    void print(const QString str);
    void println(const QString str);
    QString read();
    // QCommandLineParser * getCmdlParser() {return &cmdparser;}
    void doActions(CmdlOptions *options);
    QString getPassword();


private:
    QCommandLineParser cmdparser;

    QStringList options;
    MNotesNetwork network;


    void parserConfig();
    bool checkUrl(const QUrl &url);
    void printValues(const QJsonObject obj);
    void writeData(const QJsonObject obj);
    void writeFile(const QByteArray data, const QString filename);
    QByteArray readFile();

    QByteArray newNote();
    QString action;

signals:

public slots:
    void receiveResult( const QByteArray result);
};

#endif // COMMANDLINE_H
