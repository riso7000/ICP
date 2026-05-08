/**
 * @file net_def_io.cpp
 * @authors xvargas00
 * @brief Implements interface for handling network definition files.
 */

#include "net_def_io.hpp"
#include "petriscene.h"
#include <iostream>
#include <fstream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonParseError>

int read_netdef(QString path, PetriScene& net) {
    using namespace std;

    QString contents;

    QFile input_file;
    input_file.setFileName(path);
    input_file.open(QIODevice::ReadOnly | QIODevice::Text);
    contents = input_file.readAll();
    input_file.close();

    QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
    QJsonObject obj = doc.object();



    return 0;
}

int write_netdef(PetriScene& net) {
    using namespace std;

    // Open file for writing
    ofstream OutputFile("petrinet-spec.pnet.json");

    if (!OutputFile) {
        cerr << "Error: Failed to open output file." << endl;
    }

    // Create JSON representation
    QJsonObject json_root;
    QJsonArray net_inputs;
    QJsonArray net_outputs;
    QJsonArray net_variables;
    QJsonArray net_places;
    QJsonArray net_transitions;

    json_root.insert(JSONFLD_NET_NAME, "");
    json_root.insert(JSONFLD_COMMENT, "");

    // APPEND INPUTS
    // APPEND OUTPUTS
    // APPEND VARIABLES
    

    // PLACES


    // TRANSITIONS

    json_root.insert(JSONFLD_INPUTS, net_inputs);
    json_root.insert(JSONFLD_OUTPUTS, net_outputs);
    json_root.insert(JSONFLD_VARS, net_variables);
    json_root.insert(JSONFLD_PLACES, net_places);
    json_root.insert(JSONFLD_TRANSITIONS, net_transitions);

    // Create JSON document from root and write it to the file
    QJsonDocument doc(json_root);
    QByteArray byte_form = doc.toJson(QJsonDocument::Indented);

    OutputFile.write(byte_form.constData(), byte_form.size());
    OutputFile.close();

    return 0;
}
