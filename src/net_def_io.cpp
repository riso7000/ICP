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
    QJsonArray ins_arr;
    QJsonArray outs_arr;
    QJsonArray vars_arr;
    QJsonArray places_arr;
    QJsonArray transits_arr;

    input_file.setFileName(path);
    input_file.open(QIODevice::ReadOnly | QIODevice::Text);
    contents = input_file.readAll();
    input_file.close();

    QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
    QJsonObject obj = doc.object();

    // Retrieve root values
    QJsonValue net_name = obj.value(JSONFLD_NET_NAME);
    QJsonValue net_comment = obj.value(JSONFLD_COMMENT);
    QJsonValue net_ins = obj.value(JSONFLD_INPUTS);
    QJsonValue net_outs = obj.value(JSONFLD_OUTPUTS);
    QJsonValue net_vars = obj.value(JSONFLD_VARS);
    QJsonValue net_places = obj.value(JSONFLD_PLACES);
    QJsonValue net_transits = obj.value(JSONFLD_TRANSITIONS);

    if (net_name.isUndefined() || net_comment.isUndefined() || net_ins.isUndefined() || net_outs.isUndefined() ||
        net_vars.isUndefined() || net_places.isUndefined() || net_transits.isUndefined()) {

        cerr << "Error: A required key is missing in JSON." << endl;
        return -1;
    }

    // Set metadata
    net.name = net_name.toString();
    net.comment = net_comment.toString();

    // Set arrays for iteration
    ins_arr = net_ins.toArray();
    outs_arr = net_outs.toArray();
    vars_arr = net_vars.toArray();
    places_arr = net_places.toArray();
    transits_arr = net_transits.toArray();

    for (const QJsonValue& in_val : ins_arr) {
        QString input = in_val.toString();
        
        // ADD TO INPUTS
    }

    for (const QJsonValue& out_val : outs_arr) {
        QString output = out_val.toString();
        
        // ADD TO OUTPUTS
    }

    for (const QJsonValue& variable : vars_arr) {
        QString variable_def = variable.toString();
        
        // PROCESS VARIABLE DEFINITION
        // ADD IT TO VARIABLES
    }

    for (const QJsonValue& place_obj : places_arr) {
        QJsonObject place = place_obj.toObject();

        QString name = place.value(JSON_PL_NAME).toString();
        int init_tok = place.value(JSON_PL_INIT_TOK).toInt();
        int pos_x = place.value(JSON_POS_X).toInt();
        int pos_y = place.value(JSON_POS_Y).toInt();

        // ADD OBJECT TO SCENE
    }

    for (const QJsonValue& transit_obj : transits_arr) {
        QJsonObject transit = transit_obj.toObject();

        
    }

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
