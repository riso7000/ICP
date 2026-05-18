// ------------------------------
// net_def_io.cpp
//
// Author:
// Šimon Varga (xvargas00)
//
// Implements net saving and loading to/from a JSON file.
// ------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonParseError>

// Module headers
#include "net_def_io.hpp"
#include "arc.hpp"
#include "place.hpp"
#include "transition.hpp"
#include "petriscene.hpp"



int readNetDef(QString path, PetriScene& net) {
    using namespace std;

    QString contents;
    QFile inputFile;
    QJsonArray insArr;
    QJsonArray outsArr;
    QJsonArray varsArr;
    QJsonArray placesArr;
    QJsonArray transitsArr;

    inputFile.setFileName(path);

    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return -1;
    }

    // Read input file and create JSON representation
    contents = inputFile.readAll();
    inputFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
    QJsonObject obj = doc.object();

    // Retrieve root values
    QJsonValue netName = obj.value(JSONFLD_NET_NAME);
    QJsonValue netComment = obj.value(JSONFLD_COMMENT);
    QJsonValue netIns = obj.value(JSONFLD_INPUTS);
    QJsonValue netOuts = obj.value(JSONFLD_OUTPUTS);
    QJsonValue netVars = obj.value(JSONFLD_VARS);
    QJsonValue netPlaces = obj.value(JSONFLD_PLACES);
    QJsonValue netTransits = obj.value(JSONFLD_TRANSITIONS);

    if (netName.isUndefined() || netComment.isUndefined() || netIns.isUndefined() || netOuts.isUndefined() ||
        netVars.isUndefined() || netPlaces.isUndefined() || netTransits.isUndefined()) {

        cerr << "Error: A required key is missing in JSON." << endl;
        return -1;
    }

    // Set metadata
    net.name = netName.toString();
    net.comment = netComment.toString();

    // Set arrays for iteration
    insArr = netIns.toArray();
    outsArr = netOuts.toArray();
    varsArr = netVars.toArray();
    placesArr = netPlaces.toArray();
    transitsArr = netTransits.toArray();

    // Add net inputs
    for (const QJsonValue& inVal : insArr) {
        QString input_name = inVal.toString();
        net.inputs.push_back(PetriScene::NetInput(input_name));
    }

    // Add net outputs
    for (const QJsonValue& outVal : outsArr) {
        QString output_name = outVal.toString();
        net.outputs.push_back({output_name});
    }

    // Add net variables
    for (const QJsonValue& var : varsArr) {
        QJsonObject varObj = var.toObject();

        QString varName = varObj.value(JSON_VAR_NAME).toString();
        QString varType = varObj.value(JSON_VAR_TYPE).toString();
        QString varVal = varObj.value(JSON_VAR_INITVAL).toString();
        
        net.variables.push_back({varName, varType, varVal});
    }

    // Add places and draw them
    for (const QJsonValue& place : placesArr) {
        QJsonObject placeObj = place.toObject();

        QString name = placeObj.value(JSON_PL_NAME).toString();
        int initTok = placeObj.value(JSON_PL_INIT_TOK).toInt();
        double posX = placeObj.value(JSON_POS_X).toDouble();
        double posY = placeObj.value(JSON_POS_Y).toDouble();

        // Add place to scene
        Place* placePtr = new Place(name, initTok);
        placePtr->setPos(posX, posY);
        net.places.push_back(placePtr);
        net.addItem(placePtr);
    }

    // Add transitions and draw them
    for (const QJsonValue& transit : transitsArr) {
        QJsonObject transitObj = transit.toObject();

        QString name = transitObj.value(JSON_TR_NAME).toString();
        double posX = transitObj.value(JSON_POS_X).toDouble();
        double posY = transitObj.value(JSON_POS_Y).toDouble();

        QString whenIn = transitObj.value(JSON_TR_WHEN_IN).toString();
        QString whenBool = transitObj.value(JSON_TR_WHEN_BOOL).toString();
        int whenDelay = transitObj.value(JSON_TR_WHEN_DELAY).toInt();

        QString doAction = transitObj.value(JSON_TR_DO).toString();

        QJsonArray arcInArr = transitObj.value(JSON_TR_IN).toArray();
        QJsonArray arcOutArr = transitObj.value(JSON_TR_OUT).toArray();

        // Add transition to scene
        Transition* transitPtr = new Transition(name, 0);
        transitPtr->eventName = whenIn;
        transitPtr->guard = whenBool;
        transitPtr->delayMs = whenDelay;
        transitPtr->action = doAction;
        transitPtr->setPos(posX, posY);

        net.transitions.push_back(transitPtr);
        net.addItem(transitPtr);

        // Arcs in
        for (const QJsonValue& arc : arcInArr) {
            QJsonObject arcObj = arc.toObject();
            QString srcName = arcObj.value(JSON_PL_NAME).toString();
            int weight = arcObj.value(JSON_TR_PL_TOK).toInt();
            Place* srcPlacePtr = NULL;

            for (Place* place : net.places) {
                QString placeName = place->getName();

                if (placeName == srcName) {
                    srcPlacePtr = place;
                }
            }

            if (srcPlacePtr) {
                Arc* inArc = new Arc(srcPlacePtr, transitPtr, weight);
                transitPtr->inputArcs.push_back(inArc);
                net.addItem(inArc);
            }
        }

        // Arcs out
        for (const QJsonValue& arc : arcOutArr) {
            QJsonObject arcObj = arc.toObject();
            QString destName = arcObj.value(JSON_PL_NAME).toString();
            int weight = arcObj.value(JSON_TR_PL_TOK).toInt();
            Place* destPlacePtr = NULL;
            
            for (Place* place : net.places) {
                QString placeName = place->getName();

                if (placeName == destName) {
                    destPlacePtr = place;
                }
            }

            if (destPlacePtr) {
                Arc* outArc = new Arc(transitPtr, destPlacePtr, weight);
                transitPtr->outputArcs.push_back(outArc);
                net.addItem(outArc);
            }
        }
    }

    return 0;
}

int writeNetDef(QString path, PetriScene& net) {
    using namespace std;

    // Open file for writing
    QFile outputFile;
    outputFile.setFileName(path);

    if (!outputFile.open(QIODevice::WriteOnly)) {
        return -1;
    }

    // Create JSON representation
    QJsonObject jsonRoot;
    QJsonArray netInputs;
    QJsonArray netOutputs;
    QJsonArray netVariables;
    QJsonArray netPlaces;
    QJsonArray netTransitions;

    // Add inputs to inputs array
    for (const struct PetriScene::NetInput& input : net.inputs) {
        QJsonValue val(input.name);
        netInputs.push_back(val);
    }

    // Add outputs to outputs array
    for (const struct PetriScene::NetOutput& output : net.outputs) {
        QJsonValue val(output.name);
        netOutputs.push_back(val);
    }

    // Add variables to variables array
    for (const struct PetriScene::NetVariable& var : net.variables) {
        QJsonObject varObj;
        QJsonValue name(var.name);
        QJsonValue type(var.type);
        QJsonValue val(var.initialValue.toString());

        varObj.insert(JSON_VAR_NAME, name);
        varObj.insert(JSON_VAR_TYPE, type);
        varObj.insert(JSON_VAR_INITVAL, val);

        netVariables.push_back(varObj);
    }

    // Add places to place array
    for (const Place* placePtr : net.places) {
        QJsonObject placeObj;
        QJsonValue name(placePtr->getName());
        QJsonValue initTok(placePtr->getTokens());
        QJsonValue posX(placePtr->x());
        QJsonValue posY(placePtr->y());

        placeObj.insert(JSON_PL_NAME, name);
        placeObj.insert(JSON_PL_INIT_TOK, initTok);
        placeObj.insert(JSON_POS_X, posX);
        placeObj.insert(JSON_POS_Y, posY);

        netPlaces.push_back(placeObj);
    }

    // Add transitions to transition array
    for (const Transition* transitPtr : net.transitions) {
        QJsonObject transitObj;
        QJsonArray arcIn;
        QJsonArray arcOut;

        QJsonValue name(transitPtr->getName());
        QJsonValue posX(transitPtr->x());
        QJsonValue posY(transitPtr->y());
        QJsonValue whenIn(transitPtr->eventName);
        QJsonValue whenBool(transitPtr->guard);
        QJsonValue whenDelay(transitPtr->delayMs);
        QJsonValue doAction(transitPtr->action);

        // Arcs in
        for (const Arc* arcPtr : transitPtr->inputArcs) {
            QJsonObject arcObj;
            QJsonValue weight(arcPtr->getWeight());
            Place *srcPlacePtr = dynamic_cast<Place*>(arcPtr->getSource());

            if (!srcPlacePtr) {
                continue;
            }
            
            QJsonValue srcName(srcPlacePtr->getName());

            arcObj.insert(JSON_PL_NAME, srcName);
            arcObj.insert(JSON_TR_PL_TOK, weight);

            arcIn.push_back(arcObj);
        }

        // Arcs out
        for (const Arc* arcPtr : transitPtr->outputArcs) {
            QJsonObject arcObj;
            QJsonValue weight(arcPtr->getWeight());
            Place *destPlacePtr = dynamic_cast<Place*>(arcPtr->getDest());

            if (!destPlacePtr) {
                continue;
            }
            
            QJsonValue destName(destPlacePtr->getName());

            arcObj.insert(JSON_PL_NAME, destName);
            arcObj.insert(JSON_TR_PL_TOK, weight);

            arcOut.push_back(arcObj);
        }

        transitObj.insert(JSON_TR_NAME, name);
        transitObj.insert(JSON_POS_X, posX);
        transitObj.insert(JSON_POS_Y, posY);
        transitObj.insert(JSON_TR_IN, arcIn);
        transitObj.insert(JSON_TR_OUT, arcOut);
        transitObj.insert(JSON_TR_WHEN_IN, whenIn);
        transitObj.insert(JSON_TR_WHEN_BOOL, whenBool);
        transitObj.insert(JSON_TR_WHEN_DELAY, whenDelay);
        transitObj.insert(JSON_TR_DO, doAction);

        netTransitions.push_back(transitObj);
    }

    // Add all parts to JSON root
    jsonRoot.insert(JSONFLD_NET_NAME, net.name);
    jsonRoot.insert(JSONFLD_COMMENT, net.comment);
    jsonRoot.insert(JSONFLD_INPUTS, netInputs);
    jsonRoot.insert(JSONFLD_OUTPUTS, netOutputs);
    jsonRoot.insert(JSONFLD_VARS, netVariables);
    jsonRoot.insert(JSONFLD_PLACES, netPlaces);
    jsonRoot.insert(JSONFLD_TRANSITIONS, netTransitions);

    // Create JSON document from root and write it to the file
    QJsonDocument doc(jsonRoot);
    QByteArray byteForm = doc.toJson(QJsonDocument::Indented);

    outputFile.write(byteForm.constData(), byteForm.size());
    outputFile.close();

    return 0;
}
