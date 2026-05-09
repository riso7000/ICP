/**
 * @file net_def_io.cpp
 * @authors xvargas00
 * @brief Implements interface for handling network definition files.
 */

#include "net_def_io.hpp"
#include "place.h"
#include "transition.h"
#include "petriscene.h"
#include <iostream>
#include <fstream>
#include <sstream>
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

    // Add net inputs
    for (const QJsonValue& in_val : ins_arr) {
        QString input_name = in_val.toString();
        net.inputs.push_back({input_name, "", false});
    }

    // Add net outputs
    for (const QJsonValue& out_val : outs_arr) {
        QString output_name = out_val.toString();
        net.outputs.push_back({output_name});
    }

    // Add net variables
    for (const QJsonValue& var : vars_arr) {
        QJsonObject var_obj = var.toObject();

        QString var_name = var_obj.value(JSON_VAR_NAME).toString();
        QString var_type = var_obj.value(JSON_VAR_TYPE).toString();
        QString var_val = var_obj.value(JSON_VAR_INITVAL).toString();
        
        net.variables.push_back({var_name, var_type, var_val});
    }

    // Add places and draw them
    for (const QJsonValue& place_obj : places_arr) {
        QJsonObject place = place_obj.toObject();

        QString name = place.value(JSON_PL_NAME).toString();
        int init_tok = place.value(JSON_PL_INIT_TOK).toInt();
        double pos_x = place.value(JSON_POS_X).toDouble();
        double pos_y = place.value(JSON_POS_Y).toDouble();

        // Add object to scene
        Place* pl = new Place(1, init_tok);
        pl->setPos(pos_x, pos_y);
        net.places.push_back(pl);
        net.addItem(pl);
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
        return -1;
    }

    // Create JSON representation
    QJsonObject json_root;
    QJsonArray net_inputs;
    QJsonArray net_outputs;
    QJsonArray net_variables;
    QJsonArray net_places;
    QJsonArray net_transitions;

    // Add inputs to inputs array
    for (const struct PetriScene::NetInput& in : net.inputs) {
        QJsonValue val(in.name);
        net_inputs.push_back(val);
    }

    // Add outputs to outputs array
    for (const struct PetriScene::NetOutput& out : net.outputs) {
        QJsonValue val(out.name);
        net_outputs.push_back(val);
    }

    // Add variables to variables array
    for (const struct PetriScene::NetVariable& var : net.variables) {
        QJsonObject var_obj;
        QJsonValue name(var.name);
        QJsonValue type(var.type);
        QJsonValue val(var.initialValue.toString());

        var_obj.insert(JSON_VAR_NAME, name);
        var_obj.insert(JSON_VAR_TYPE, type);
        var_obj.insert(JSON_VAR_INITVAL, val);

        net_variables.push_back(var_obj);
    }

    // Add places to place array
    for (const Place* pl : net.places) {
        QJsonObject place;
        QJsonValue name(pl->getId());
        QJsonValue init_tok(pl->getInitTokens());
        QJsonValue posx(pl->x());
        QJsonValue posy(pl->y());

        place.insert(JSON_PL_NAME, name);
        place.insert(JSON_PL_INIT_TOK, init_tok);
        place.insert(JSON_POS_X, posx);
        place.insert(JSON_POS_Y, posy);

        net_places.push_back(place);
    }

    // TRANSITIONS
    for (const Transition* tr : net.transitions) {
        QJsonObject transit;
        QJsonValue name(tr->getId());
        QJsonValue posx(tr->x());
        QJsonValue posy(tr->y());

        transit.insert(JSON_TR_NAME, name);
        transit.insert(JSON_POS_X, posx);
        transit.insert(JSON_POS_Y, posy);
    }

    // Add all parts to JSON root
    json_root.insert(JSONFLD_NET_NAME, net.name);
    json_root.insert(JSONFLD_COMMENT, net.comment);
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
