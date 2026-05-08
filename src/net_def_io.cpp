/**
 * @file net_def_io.cpp
 * @authors xvargas00
 * @brief Implements interface for handling network definition files.
 */

#include "net_def_io.hpp"
#include "petriscene.h"
#include <iostream>
#include <fstream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
 #include <QJsonParseError>
 #include <QJsonParseError>

int read_netdef(std::string path) {
    using namespace std;

    ifstream InputFile(path);

    if (!InputFile) {
        cerr << "Error: Failed to open input file." << endl;
    }

    InputFile.close();

    return 0;
}

int write_netdef(PetriScene& netdef) {
    using namespace std;

    ofstream OutputFile("petrinet-spec.pnet.json");

    if (!OutputFile) {
        cerr << "Error: Failed to open output file." << endl;
    }

    QJsonObject json_root;


    QJsonDocument doc(json_root);
    QByteArray byte_form = doc.toJson(QJsonDocument::Indented);

    OutputFile.write(byte_form.constData(), byte_form.size());
    OutputFile.close();

    return 0;
}
