/*
 * SPDX-FileCopyrightText: 2024 Izidor Makuc <izidor@makuc.info>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CLI_H
#define CLI_H

#include <inttypes.h>
#include <stdbool.h>

#ifndef PICO_CLI_COMMAND_BUFF_SIZE
#define PICO_CLI_COMMAND_BUFF_SIZE 32
#endif

struct pico_cli;

struct pico_cli_cmd {
        struct pico_cli_cmd *next;
        const char *command_name;
        const char *command_description;
        void (*command_function)(struct pico_cli *cli, 
				 char *command_input_string);
};

struct pico_cli_settings {
        void *(*my_malloc)(size_t size);
        bool (*get_char)(char *);
        void (*send_char)(char c);
	char input_end_char;
	char *prompt;
};


char *pico_cli_get_user_input(struct pico_cli *cli);
bool pico_cli_add_cmd(struct pico_cli *cli, struct pico_cli_cmd *c);

uint32_t pico_cli_run(struct pico_cli *cli);

#ifdef PICO_CLI_USE_STATIC_MEMORY_ALLOCATION
#include "pico_cli_internal.h"
uint32_t pico_cli_init(struct pico_cli *cli, 
		       struct pico_cli_settings *s);
#else

struct pico_cli *pico_cli_init(struct pico_cli_settings *s);

#endif

#endif
