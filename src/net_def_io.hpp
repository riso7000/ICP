/**
 * @file net_def_io.hpp
 * @authors xvargas00
 * @brief Provides interface for handling network definition files.
 */

#ifndef NET_DEF_IO_HPP
#define NET_DEF_IO_HPP

#include <string>
#include "petriscene.h"

#define FILE_IO_FILETYPES "Petri Net files (*.pnet);;JSON files (*.json);;All Files (*)"

#define JSONFLD_NET_NAME "1_petri_net_name"
#define JSONFLD_COMMENT "2_comment"
#define JSONFLD_INPUTS "3_inputs"
#define JSONFLD_OUTPUTS "4_outputs"
#define JSONFLD_VARS "5_variables"
#define JSONFLD_PLACES "6_places"
#define JSONFLD_TRANSITIONS "7_transitions"
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
#define JSON_TR_WHEN "when"
#define JSON_TR_DO "do"

int read_netdef(QString path, PetriScene& net);
int write_netdef(QString path, PetriScene& net);

#endif
