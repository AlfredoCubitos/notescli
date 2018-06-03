#include "commandline.h"
#include "mnotesnetwork.h"

CommandLine::CommandLine(QObject *parent) : QObject(parent)
{

    parserConfig();
}

void CommandLine::parserConfig()
{


    QObject::connect(&network,SIGNAL(resultAvailable(const QByteArray)),this,SLOT(receiveResult(const QByteArray)));

    cmdparser.setApplicationDescription(QCoreApplication::translate("CommandLine","Commandline Interface to Nextcloude Notes"));
    cmdparser.addHelpOption();
    cmdparser.addVersionOption();

    cmdparser.addOptions({
                             {"url",
                              QCoreApplication::translate("CommandLine", "Url to nextnote server"),
                              QCoreApplication::translate("CommandLine", "url")

                             },

                             {{"u","user"},
                              QCoreApplication::translate("CommandLine", "Login name"),
                              QCoreApplication::translate("CommandLine", "username")

                             },
                             {{"p","password"},
                              QCoreApplication::translate("CommandLine", "Password"),
                             },
                             {{"l","list"},
                              QCoreApplication::translate("CommandLine", "List all notes")
                             },
                             {{"n","note"},
                              QCoreApplication::translate("CommandLine", "Get note with <id>"),
                              QCoreApplication::translate("CommandLine", "id")
                             },
                             {{"a","add"},
                              QCoreApplication::translate("CommandLine", "Add new note")

                             },
                             {{"d","del"},
                              QCoreApplication::translate("CommandLine", "Delete note with <id>"),
                              QCoreApplication::translate("CommandLine", "id")
                             },
                             {{"f","file"},
                              QCoreApplication::translate("CommandLine", "file to read to or write from"),
                              QCoreApplication::translate("CommandLine", "file")
                             },
                             {{"s","save"},
                              QCoreApplication::translate("CommandLine", "save note")

                             }


    });

    cmdparser.addPositionalArgument("url","is mandatory");
    cmdparser.addPositionalArgument("user","is mandatory");
    cmdparser.addPositionalArgument("password","is mandatory");
    cmdparser.addPositionalArgument("file","specify a file save note(s) or read from");
    cmdparser.addPositionalArgument("save","save note in separate file and get note filename from title");

}



CmdlResult CommandLine::checkArguments(const QCoreApplication &app, CmdlOptions *options, QString *cmdlError )
{

    cmdparser.process(app);

   // qDebug() << "Options: " << options;
    QStringList opts = cmdparser.optionNames(); //result only when passed option in commandline

    if (opts.size() < 3)
    {
        *cmdlError = "not enough options";
        return CmdlError;
    }



    if (cmdparser.isSet("file") && cmdparser.isSet("save"))
    {
        *cmdlError = "only one of these options is allowed";
        return CmdlError;
    }

    if (cmdparser.isSet("user")){
        options->user =  cmdparser.value("user");
    }else {
        *cmdlError = "user option is mandatory";
        return CmdlError;
    }


    if (!cmdparser.isSet("password"))
    {
        *cmdlError = "password option is mandatory";
        return CmdlError;
    }

    if(cmdparser.isSet("url"))
    {
       QUrl url;

       url.setUrl(cmdparser.value("url"));

       if (! checkUrl(url) )
       {
           *cmdlError = url.errorString();
           *cmdlError += "\nSorry: it seems url is not valid";
           return CmdlError;
       }
       options->url = cmdparser.value("url");
    }else{
        *cmdlError = "url option is mandatory";
        return CmdlError;
    }

    if (cmdparser.isSet("list")){
        options->action = "list";
        return CmdlOk;
    }



    if (cmdparser.isSet("note"))
    {
        action = "note,";
        action.append(cmdparser.value("note"));
        options->action = action;
        return CmdlOk;
    }

    if (cmdparser.isSet("add")){
        options->action = "add";

        return CmdlOk;
    }

    if(cmdparser.isSet("del"))
    {
        action = "del,";
        action.append(cmdparser.value("del"));
        options->action = action;
        return CmdlOk;
    }


    return CmdlOk;
}

void CommandLine::doActions(CmdlOptions *options)
{
    QHash<QString,QString> params;
    QStringList action;

    params["user"] = options->user;
    params["password"] = options->password;
    params["url"] = options->url;



    action = options->action.split(",");

    network.initConnect(params);

    if (action[0] == "list" || cmdparser.isSet("save"))
        network.getMnotes();

    if (action[0] == "note")
        network.getMnotes(action[1]);

    if (action[0] == "add")
    {
        QString note;
        QByteArray data;

     //   qDebug() << file;

        if (!cmdparser.isSet("file")){
            note = newNote();
        }else{
            data = readFile();
            qDebug() << data;
            if (!data.isEmpty())
                network.newMnote(data);
        }

     //   qDebug() << note;
    }


}

void CommandLine::writeFile( const QByteArray data, const QString filename)
{
    QFile note;

    if (cmdparser.isSet("file"))
        note.setFileName(cmdparser.value("file"));
    else if (cmdparser.isSet("save"))
        note.setFileName(filename);
    else
        return;


    QTextStream out(&note);
    if (!note.open(QIODevice::Append| QIODevice::Text))
        return;
    /***
     * TODO: File error handling
     * ***/

    out << data;
}

QByteArray CommandLine::readFile()
{
    QByteArray lines;

    QFile note(cmdparser.value("file"));

    if (!note.open(QIODevice::ReadOnly | QIODevice::Text))
        return lines;
    /***
     * TODO: File error handling
     * ***/
    while (!note.atEnd()) {
        lines.append( note.readLine());
    }

    QJsonObject jobj{
        {"content", QString(lines)}
    };
   // file.clear(); // do not overwrite existing note file due response handler
    QJsonDocument jdoc(jobj);
    lines = jdoc.toJson(QJsonDocument::Compact);
    return lines;
}

QByteArray CommandLine::newNote()
{
    QTextStream in(stdin);
    QByteArray input;


    while (in.read(1) != "\u001B") { //ESC_Key

        input.append(in.readLine());
       // input.append("\n");
    }

  // qDebug() << input;


    return input;
}

void CommandLine::print(const QString str)
{
    QTextStream out(stdout);
    out << str;
}

void CommandLine::println(const QString str)
{
    QTextStream out(stdout);
    out << str << endl;
}

QString CommandLine::read()
{
    QTextStream in(stdin);
    QString input;

    if (!in.atEnd())
        input = in.readLine();
    return input;
}

QString CommandLine::getPassword()
{
    QTextStream in(stdin);
    QString input;
    system("stty -echo");

     if (!in.atEnd())
         input = in.readLine(80);

     system("stty echo");
     return input;
}

bool CommandLine::checkUrl(const QUrl &url)
{
    QRegularExpression regexp;
    regexp.setPattern("^(https?)$");
    if (!url.scheme().contains(regexp) )
        return false;
    if (! url.isValid())
        return false;
    return true;
}

void CommandLine::writeData(const QJsonObject obj)
{
    QByteArray data;
    QString id, title, content, pbreak;

     id = "id: " + obj.value("id").toVariant().toString() + "\n";
     title = "title: " + obj.value("title").toString() + "\n";
     content = "content: " + obj.value("content").toString() + "\n";
     if (cmdparser.isSet("file"))
        pbreak = "-------------- Note ---------------\n";

    data.append(id);
    data.append(title);
    data.append(content);
    data.append(pbreak);

    if (cmdparser.isSet("file"))
        writeFile(data,cmdparser.value("file"));
    if (cmdparser.isSet("save"))
        writeFile(data,obj.value("title").toString().append(".txt"));
}

void CommandLine::printValues(const QJsonObject obj)
{

    QTextStream out(stdout);
    out << "------------ Note -----------------------" << endl;
    out << "id: " << obj.value("id").toDouble() << endl;
    out << "title: " << obj.value("title").toString() << endl;
    out << "content: " << obj.value("content").toString() << endl;

}

/**
  SLOTS
  **/
void CommandLine::receiveResult(const QByteArray result)
{
    QJsonDocument json = QJsonDocument::fromJson(result);
    QJsonArray arr;


    if (json.isArray())
    {
        arr = json.array();

        for (int i=0; i < arr.size(); ++i)
        {
            if (arr.at(i).isObject() ){

                if (cmdparser.isSet("file") && cmdparser.isSet("list")) //avoiding overwriting read file by add option
                    writeData(arr.at(i).toObject());
                else if (cmdparser.isSet("save"))
                    writeData(arr.at(i).toObject());
                else
                    printValues(arr.at(i).toObject());
            }
        }
    }

    if (json.isObject()){
        if (cmdparser.isSet("file") && cmdparser.isSet("list"))
            writeData(json.object());
        else if (cmdparser.isSet("save"))
            writeData(json.object());
        else
            printValues( json.object());
    }


     exit(0);

   // qDebug() << result;
}
