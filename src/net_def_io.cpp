/**
 * @file net_def_io.cpp
 * @authors xvargas00
 * @brief Implements interface for handling network definition files.
 */

#include "net_def_io.hpp"
#include "arc.h"
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

    if (!input_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return -1;
    }

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

        // Add place to scene
        Place* pl = new Place(name, init_tok);
        pl->setPos(pos_x, pos_y);
        net.places.push_back(pl);
        net.addItem(pl);
    }

    // Add transitions and draw them
    for (const QJsonValue& transit_obj : transits_arr) {
        QJsonObject transit = transit_obj.toObject();

        QString name = transit.value(JSON_TR_NAME).toString();
        double pos_x = transit.value(JSON_POS_X).toDouble();
        double pos_y = transit.value(JSON_POS_Y).toDouble();
        QString when_in = transit.value(JSON_TR_WHEN_IN).toString();
        QString when_bool = transit.value(JSON_TR_WHEN_BOOL).toString();
        int when_delay = transit.value(JSON_TR_WHEN_DELAY).toInt();
        QString do_action = transit.value(JSON_TR_DO).toString();

        QJsonArray arcin_arr = transit.value(JSON_TR_IN).toArray();
        QJsonArray arcout_arr = transit.value(JSON_TR_OUT).toArray();

        // Add transition to scene
        Transition* tr = new Transition(name, 0);
        tr->eventName = when_in;
        tr->guard = when_bool;
        tr->delay_ms = when_delay;
        tr->action = do_action;
        tr->setPos(pos_x, pos_y);
        
        net.transitions.push_back(tr);
        net.addItem(tr);

        // Arcs in
        for (const QJsonValue& arc_obj : arcin_arr) {
            QJsonObject arc = arc_obj.toObject();
            QString src_name = arc.value(JSON_PL_NAME).toString();
            int weight = arc.value(JSON_TR_PL_TOK).toInt();
            Place* src_pl = NULL;

            for (Place* pl : net.places) {
                QString pl_name = pl->getName();

                if (pl_name == src_name) {
                    src_pl = pl;
                }
            }

            if (src_pl) {
                Arc* inarc = new Arc(src_pl, tr, weight);
                tr->input_arcs.push_back(inarc);
                net.addItem(inarc);
            }
        }

        // Arcs out
        for (const QJsonValue& arc_obj : arcout_arr) {
            QJsonObject arc = arc_obj.toObject();
            QString dest_name = arc.value(JSON_PL_NAME).toString();
            int weight = arc.value(JSON_TR_PL_TOK).toInt();
            Place* dest_pl = NULL;
            
            for (Place* pl : net.places) {
                QString pl_name = pl->getName();

                if (pl_name == dest_name) {
                    dest_pl = pl;
                }
            }

            if (dest_pl) {
                Arc* outarc = new Arc(tr, dest_pl, weight);
                tr->output_arcs.push_back(outarc);
                net.addItem(outarc);
            }
        }
    }

    return 0;
}

int write_netdef(QString path, PetriScene& net) {
    using namespace std;

    // Open file for writing
    QFile output_file;
    output_file.setFileName(path);

    if (!output_file.open(QIODevice::WriteOnly)) {
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
        QJsonValue name(pl->getName());
        QJsonValue init_tok(pl->getTokens());
        QJsonValue posx(pl->x());
        QJsonValue posy(pl->y());

        place.insert(JSON_PL_NAME, name);
        place.insert(JSON_PL_INIT_TOK, init_tok);
        place.insert(JSON_POS_X, posx);
        place.insert(JSON_POS_Y, posy);

        net_places.push_back(place);
    }

    // Add transitions to transition array
    for (const Transition* tr : net.transitions) {
        QJsonObject transit;
        QJsonArray arcin;
        QJsonArray arcout;

        QJsonValue name(tr->getName());
        QJsonValue posx(tr->x());
        QJsonValue posy(tr->y());
        QJsonValue when_in(tr->eventName);
        QJsonValue when_bool(tr->guard);
        QJsonValue when_delay(tr->delay_ms);
        QJsonValue do_action(tr->action);

        // Arcs in
        for (const Arc* arc : tr->input_arcs) {
            QJsonObject arc_obj;
            QJsonValue weight(arc->getWeight());
            Place *src = dynamic_cast<Place*>(arc->getSource());

            if (!src) {
                continue;
            }
            
            QJsonValue src_name(src->getName());

            arc_obj.insert(JSON_PL_NAME, src_name);
            arc_obj.insert(JSON_TR_PL_TOK, weight);

            arcin.push_back(arc_obj);
        }

        // Arcs out
        for (const Arc* arc : tr->output_arcs) {
            QJsonObject arc_obj;
            QJsonValue weight(arc->getWeight());
            Place *dest = dynamic_cast<Place*>(arc->getDest());

            if (!dest) {
                continue;
            }
            
            QJsonValue dest_name(dest->getName());

            arc_obj.insert(JSON_PL_NAME, dest_name);
            arc_obj.insert(JSON_TR_PL_TOK, weight);

            arcout.push_back(arc_obj);
        }

        transit.insert(JSON_TR_NAME, name);
        transit.insert(JSON_POS_X, posx);
        transit.insert(JSON_POS_Y, posy);
        transit.insert(JSON_TR_IN, arcin);
        transit.insert(JSON_TR_OUT, arcout);
        transit.insert(JSON_TR_WHEN_IN, when_in);
        transit.insert(JSON_TR_WHEN_BOOL, when_bool);
        transit.insert(JSON_TR_WHEN_DELAY, when_delay);
        transit.insert(JSON_TR_DO, do_action);

        net_transitions.push_back(transit);
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

    output_file.write(byte_form.constData(), byte_form.size());
    output_file.close();

    return 0;
}
