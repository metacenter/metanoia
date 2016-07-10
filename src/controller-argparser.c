// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-argparser.h"

#include "global-macros.h"

#include <argp.h>
#include <string.h>

//------------------------------------------------------------------------------

typedef struct {
    NoiaControllerCommand command;
    NoiaControllerArguments* args;
} NoiaArgparser;

//------------------------------------------------------------------------------

void noia_controller_print_help(const struct argp* argp)
{
    printf("Usage: metanoiactl %s", argp->args_doc);

    const struct argp_option* options = argp->options;
    int i = 0;
    while (options[i].doc) {
        const struct argp_option* opt = &options[i];
        if (opt->name) {
            printf(" [--%s", opt->name);
            if (opt->arg) {
                bool is_optional = (opt->flags & OPTION_ARG_OPTIONAL);
                const char* format = (is_optional ? "[=%s]" : "=%s");
                printf(format, opt->arg);
            }
            printf("]");
        }
        ++i;
    }
    printf("\n\n");

    argp_help(argp, stdout,
              ARGP_HELP_LONG|ARGP_HELP_PRE_DOC|ARGP_HELP_DOC|ARGP_HELP_POST_DOC,
              "");

    printf("\nReport bugs on https://github.com/metacenter/metanoia/issues\n");
    exit(0);
}

//------------------------------------------------------------------------------

static const struct argp_option scGlobalOptions[] = {
        { NULL, '\0', NULL, 0x0, "Global Options:", 1},
        { "help", 'h', NULL, 0x0,
          "print help", 0},
        { NULL, '\0', NULL, 0x0, NULL, 0 },
    };

static const struct argp_option scInfoOptions[] = {
        { NULL, '\0', NULL, 0x0, "Info Options:", 2},
        { "drm", 'd', NULL, 0x0,
          "print DRM related info", 0},
        { "gl", 'g', NULL, 0x0,
          "print GL related info", 0},
        { "help", 'h', NULL, 0x0,
          "print help", 0},
        { NULL, '\0', NULL, 0x0, NULL, 0 }
    };

static const struct argp_option scScreenshotOptions[] = {
        { NULL, '\0', NULL, 0x0, "Screenshot Options:", 3},
        { "output", 'o', "PATH", 0x0,
          "output file path", 0},
        { "help", 'h', NULL, 0x0,
          "print help", 0},
        { NULL, '\0', NULL, 0x0, NULL, 0 }
    };

//------------------------------------------------------------------------------

int noia_controller_parse_option(int key, char* arg, struct argp_state* state)
{
    NoiaArgparser* argparser = state->input;

    switch (key) {
    case ARGP_KEY_ARG:
        if (strcmp(arg, "info") == 0) {
            argparser->command = NOIA_COMMAND_INFO;
        } else if (strcmp(arg, "screenshot") == 0) {
            argparser->command = NOIA_COMMAND_INFO;
        } else {
            argp_error(state, "'%s' is not supported subcommand!", arg);
        }
        break;

    case ARGP_KEY_SUCCESS:
    case ARGP_KEY_INIT:
    case ARGP_KEY_FINI:
        // nothing to do
        break;

    case ARGP_KEY_END:
        if (argparser->command == NOIA_COMMAND_UNKNOWN) {
            argp_error(state, "Subcommand name should be first argment!");
        }
        state->next = state->argc;
        break;

    case ARGP_KEY_NO_ARGS:
        argp_error(state, "No arguments were provided!");
        break;

    case 'h':
        noia_controller_print_help(state->root_argp);

    default:
        argp_error(state, "Unknown error parsing arguments!");
        break;
    }
    return 0;
}

//------------------------------------------------------------------------------

int noia_controller_parse_info_option(int key,
                                      char* arg NOIA_UNUSED,
                                      struct argp_state* state)
{
    NoiaArgparser* argparser = state->input;

    switch (key) {
    case 'd':
        argparser->args->info.print_drm = true;
        argparser->args->info.print_all = false;
        break;

    case 'g':
        argparser->args->info.print_gl = true;
        argparser->args->info.print_all = false;
        break;

    case 'h':
        noia_controller_print_help(state->root_argp);

    default:
        break;
    }
    return 0;
}

//------------------------------------------------------------------------------

int noia_controller_parse_screenshot_option(int key,
                                            char* arg,
                                            struct argp_state* state)
{
    NoiaArgparser* argparser = state->input;

    switch (key) {
    case 'o':
        argparser->args->screenshot.output = strdup(arg);
        break;

    case 'h':
        noia_controller_print_help(state->root_argp);

    default:
        break;
    }
    return 0;
}

//------------------------------------------------------------------------------

static const struct argp scGlobalArgp = {
        scGlobalOptions, noia_controller_parse_option,
        "<command>",
        "<command> can be one of 'info' or 'screenshot'.\n"
        "For help for specific command see `metanoiactl <command> --help`.",
        NULL, NULL, NULL
    };

static const struct argp scInfoArgp = {
        scInfoOptions, noia_controller_parse_info_option,
        "info",
        "Prints basic information about system.\n",
        NULL, NULL, NULL
    };

static const struct argp scScreenshotArgp = {
        scScreenshotOptions, noia_controller_parse_screenshot_option,
        "screenshot",
        "Takes screenshot.\n",
        NULL, NULL, NULL
    };

//------------------------------------------------------------------------------

void noia_argparser_initialize(NoiaArgparser* self,
                               NoiaControllerArguments* args)
{
    self->command = NOIA_COMMAND_UNKNOWN;
    self->args = args;
}

//------------------------------------------------------------------------------

NoiaControllerCommand noia_controller_argparse(int argc,
                                               char** argv,
                                               NoiaControllerArguments* args)
{
    NoiaArgparser argparser;
    noia_argparser_initialize(&argparser, args);

    if (argc > 1) {
        if (strcmp(argv[1], "info") == 0) {
            argparser.command = NOIA_COMMAND_INFO;
        } else if (strcmp(argv[1], "screenshot") == 0) {
            argparser.command = NOIA_COMMAND_SCREENSHOT;
        }
    }

    switch (argparser.command) {
    case NOIA_COMMAND_INFO:
        noia_controller_initialize_info_arguments(&args->info);
        argp_parse(&scInfoArgp, argc, argv, ARGP_NO_HELP, NULL, &argparser);
        break;

    case NOIA_COMMAND_SCREENSHOT:
        noia_controller_initialize_screenshot_arguments(&args->screenshot);
        argp_parse(&scScreenshotArgp, argc, argv, ARGP_NO_HELP, NULL, &argparser);
        break;

    default:
        argp_parse(&scGlobalArgp, argc, argv, ARGP_NO_HELP, NULL, &argparser);
        break;
    }

    return argparser.command;
}

//------------------------------------------------------------------------------

