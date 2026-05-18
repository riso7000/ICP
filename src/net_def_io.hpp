// ------------------------------
// net_def_io.hpp
//
// Author:
// Šimon Varga (xvargas00)
//
// Defines interface for file I/O and necessary macros for JSON generation.
// ------------------------------

#ifndef NET_DEF_IO_HPP
#define NET_DEF_IO_HPP

// Module headers
#include "petriscene.hpp"



#define FILE_IO_FILETYPES "Petri Net files (*.pnet);;JSON files (*.json);;All Files (*)"

// Main JSON fields
#define JSONFLD_NET_NAME "1:petri_net_name"
#define JSONFLD_COMMENT "2:comment"
#define JSONFLD_INPUTS "3:inputs"
#define JSONFLD_OUTPUTS "4:outputs"
#define JSONFLD_VARS "5:variables"
#define JSONFLD_PLACES "6:places"
#define JSONFLD_TRANSITIONS "7:transitions"

// Properties of all objects stored in main fields
#define JSON_VAR_NAME "var_name"
#define JSON_VAR_TYPE "var_type"
#define JSON_VAR_INITVAL "initial_val"
#define JSON_POS_X "pos_x"
#define JSON_POS_Y "pos_y"
#define JSON_PL_NAME "place_name"
#define JSON_PL_INIT_TOK "initial_tokens"
#define JSON_TR_NAME "transition_name"
#define JSON_TR_IN "in_transition"
#define JSON_TR_OUT "out_transition"
#define JSON_TR_PL_TOK "tokens"
#define JSON_TR_WHEN_IN "when_input"
#define JSON_TR_WHEN_BOOL "when_bool"
#define JSON_TR_WHEN_DELAY "when_delay"
#define JSON_TR_DO "do"

int readNetDef(QString path, PetriScene& net);
int writeNetDef(QString path, PetriScene& net);

#endif
