#include <argp.h>
#include <stdio.h>
#include <stdbool.h>
#include "appArgs.h"

enum OptKey
{
    OPT_VERBOSE         = 'v',
    OPT_QUIET           = 'q',
    OPT_OUT_FILE        = 'o',
    OPT_ERR             = -1,
    OPT_WARN            = -2,
    OPT_PEDANTIC        = 'p',
    OPT_STREET_RULES    = -3
};

const char *argp_program_version = "shove 0.0.1";
const char *argp_program_bug_address = "github.com/penguin-teal/shove";
static const char doc[] = "shove -- The shove Language compiler.";
static const char argsDoc[] = "srcDir";
static struct argp_option options[] =
{
    { "verbose",        OPT_VERBOSE     , 0,     0,  "Describe what is happening.", 3 },
    { "quiet",          OPT_QUIET       , 0,     0,  "Silence output.", 3 },
    { "out-file",       OPT_OUT_FILE    , 0,     0,  "Output file.", 1 },
    { "err",            OPT_ERR         , 0,     0,  "A space-separated list of warnings to turn into errors.", 2 },
    { "warn",           OPT_WARN        , 0,     0,  "A space-separated list of valid errors to turn into warnings.", 2 },
    { "pedantic",       OPT_PEDANTIC    , 0,     0,  "Turns all warnings into errors.", 2 },
    { "street-rules",   OPT_STREET_RULES, 0,     0,  "Turns all valid errors into warnings.", 2 }
};

static error_t parseOpt(int key, char *arg, struct argp_state *state)
{
    struct AppArgs *arguments = state->input;

    switch(key)
    {
        case OPT_VERBOSE:
            if(arguments->quiet)
            {
                printf("Options 'quiet' and 'verbose' are mutually exclusive.\n");
                return 1;
            }
            arguments->verbose = true;
            break;
        case OPT_QUIET:
            if(arguments->verbose)
            {
                printf("Options 'quiet' and 'verbose' are mutually exclusive.\n");
                return 1;
            }
            arguments->quiet = true;
            break;
        case OPT_OUT_FILE:
            arguments->outFile = arg;
            break;
        case OPT_ERR:
            arguments->err = arg;
            break;
        case OPT_WARN:
            arguments->warn = arg;
            break;
        case OPT_PEDANTIC:
            if(arguments->streetRules)
            {
                printf("Options 'pedantic' and 'street-rules' are mutually exclusive.\n");
                return 1;
            }
            arguments->pedantic = true;
            break;
        case OPT_STREET_RULES:
            if(arguments->pedantic)
            {
                printf("Options 'pedantic' and 'street-rules' are mutually exclusive.\n");
                return 1;
            }
            arguments->streetRules = true;
            break;
        
        case ARGP_KEY_ARG:
            if(state->arg_num > 1)
            {
                argp_usage(state);
            }

            arguments->args[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if(state->arg_num < 1)
            {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = 
{
    .options = options,
    .parser = parseOpt,
    .args_doc = argsDoc,
    .doc = doc
};

bool doArgp(struct AppArgs *appArgs, int argc, char **argv)
{
    appArgs->args[0] = NULL;

    appArgs->verbose = false;
    appArgs->quiet = false;
    appArgs->outFile = "-";
    appArgs->err = NULL;
    appArgs->warn = NULL;
    appArgs->pedantic = false;
    appArgs->streetRules = false;

    error_t result = argp_parse(&argp, argc, argv, 0, 0, appArgs);
    return !result;
}