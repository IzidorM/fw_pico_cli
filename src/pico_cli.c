/*
 * SPDX-FileCopyrightText: 2024 Izidor Makuc <izidor@makuc.info>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "pico_cli_internal.h"
#include "pico_cli.h"

#ifdef UNIT_TESTS
#define STATIC
#else
#define STATIC static
#endif

struct pico_cli cli_tmp;

STATIC void echo_string(struct pico_cli *cli, const char *s)
{
        for (uint32_t i = 0; s[i] != '\0'; i++)
        {
                cli->send_char(s[i]);
        }
}

STATIC void echo_input_end_sequence(struct pico_cli *cli)
{
	echo_string(cli, "\r\n");
}

STATIC void delete_last_echoed_char(struct pico_cli *cli)
{
	if (cli->input_buff_index)
	{
		cli->input_buff_index -= 1;
		echo_string(cli, "\b \b");
	}
}

STATIC struct pico_cli_cmd *pico_cli_search_command(
	struct pico_cli *cli, 
	char *cmd_name)
{
        struct pico_cli_cmd *tmp = &cli->cmd_list;

        for (; NULL != tmp; tmp = tmp->next)
        {
		if (0 == strcmp(tmp->command_name, cmd_name))
                {
                        return tmp;
                }
        }
        return NULL;
}

STATIC void pico_cli_command_received_handler(struct pico_cli *cli, char *input)
{
	struct pico_cli_cmd *tmp_command = 
		pico_cli_search_command(cli, input);

	if (tmp_command)
	{
		tmp_command->command_function(cli, input);
	}
}

STATIC char *pico_cli_handle_input(struct pico_cli *cli, char c)
{
	char *ret = NULL;
        if (cli->input_end_char == c)
	{
		cli->input_buff[cli->input_buff_index] = '\0';
		ret = cli->input_buff;
		echo_input_end_sequence(cli);
		cli->input_buff_index = 0;
        }
        else if( '\b' == c) //backspace
        {
		delete_last_echoed_char(cli);
        }
        else if( '\r' == c)
        {
                //drop character
        }
        else if ((cli->input_buff_index + 1) 
		 < PICO_CLI_COMMAND_BUFF_SIZE)
        {
                cli->input_buff[cli->input_buff_index] = c;
                cli->input_buff_index += 1;
		cli->send_char(c);
        }
	return ret;
}

STATIC void help_function(struct pico_cli *cli, char *s)
{
        (void) s;
        struct pico_cli_cmd *tmp = &cli->cmd_list;

        for (; NULL != tmp; tmp = tmp->next)
        {
                echo_string(cli, tmp->command_name);
                echo_string(cli, "\r\n");
                if (tmp->command_description)
                {
                        echo_string(cli, "\t");
                        echo_string(cli, tmp->command_description);
                        echo_string(cli, "\r\n");
                }
        }
}

uint32_t pico_cli_run(struct pico_cli *cli)
{
	if (NULL == cli)
	{
		return 1;
	}

	char c;
	while(cli->get_char(&c))
	{
		char *input_received = pico_cli_handle_input(cli, c);
		if (input_received)
		{
			pico_cli_command_received_handler(cli, 
						     input_received);
			if (cli->prompt)
			{
				echo_string(cli, cli->prompt);
			}
		}
	} 

	return 0;
}

bool pico_cli_add_cmd(struct pico_cli *cli, struct pico_cli_cmd *c)
{
	if ((NULL == cli) || (NULL == c))
	{
		return false;
	}

        struct pico_cli_cmd *tmp = &cli->cmd_list;
        
        for (; NULL != tmp->next; tmp = tmp->next);

	c->next = NULL;
        tmp->next = c;
	return true;
}

char *pico_cli_get_user_input(struct pico_cli *cli)
{
	if (NULL == cli)
	{
		return NULL;
	}

	char c;
	for(;;)
	{
		if (cli->get_char(&c))
		{
			char *input_received = pico_cli_handle_input(cli, c);
			if (input_received)
			{
				return input_received;
			}
		}
	} 
	return NULL;
}


static void pico_cli_init_internal(struct pico_cli *cli, 
			    struct pico_cli_settings *s)
{

	cli->get_char = s->get_char;
	cli->send_char = s->send_char;
	cli->input_buff_index = 0;

	cli->cmd_list.next = NULL;
	cli->cmd_list.command_name = "help";
	cli->cmd_list.command_description = 
		"print out all the commands";
	cli->cmd_list.command_function = help_function;

	cli->input_end_char = s->input_end_char;
	cli->prompt = s->prompt;
}

#ifdef PICO_CLI_USE_STATIC_MEMORY_ALLOCATION
uint32_t pico_cli_init(struct pico_cli *cli, 
		       struct pico_cli_settings *s)
{
	if (NULL == cli
	    || NULL == s->my_malloc
	    || NULL == s->get_char
	    || NULL == s->send_char)
	{
		return 1;
	}
	
	pico_cli_init_internal(cli, s);
	return 0;
}

#else
struct pico_cli *pico_cli_init(struct pico_cli_settings *s)
{
	if (NULL == s->my_malloc
	    || NULL == s->get_char
	    || NULL == s->send_char)
	{
		return NULL;
	}
	
	struct pico_cli *tmp = s->my_malloc(sizeof(struct pico_cli));
	if (NULL == tmp)
	{
		return tmp;
	}

	pico_cli_init_internal(tmp, s);

	return tmp;
}
#endif
