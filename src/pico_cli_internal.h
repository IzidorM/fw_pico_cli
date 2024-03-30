/*
 * SPDX-FileCopyrightText: 2024 Izidor Makuc <izidor@makuc.info>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CLI_INTERNAL_H
#define CLI_INTERNAL_H

#include <inttypes.h>
#include <stdbool.h>

#include "pico_cli.h"

struct pico_cli {
        bool (*get_char)(char *);
        void (*send_char)(char c);

        char input_end_char;

        struct pico_cli_cmd cmd_list;

	char *prompt;
        size_t input_buff_index;
        char input_buff[CLI_COMMAND_BUFF_SIZE];
};

#ifdef UNIT_TESTS
void echo_string(struct pico_cli *cli, const char *s);
void delete_last_echoed_char(struct pico_cli *cli);
struct pico_cli_cmd *pico_cli_search_command(struct pico_cli *cli, 
				   char *cmd_name);

char *pico_cli_handle_input(struct pico_cli *cli, char c);

void pico_cli_command_received_handler(struct pico_cli *cli, char *input);
void echo_input_end_sequence(struct pico_cli *cli);
#endif

#endif
