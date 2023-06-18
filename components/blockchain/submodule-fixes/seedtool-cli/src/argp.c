#include "argp.h"

error_t argp_parse(const struct argp *argp, int argc, char **argv, unsigned flags,
                   int *end_index, void *input)
{
    error_t err = 0;

    // argp->options;
    argp_parser_t fparser = argp->parser;

    struct argp_state state;
    state.root_argp = argp;
    state.argc = argc;
    state.argv = argv;
    state.next = 0;
    state.flags = flags;
    state.arg_num = argc - 1;
    state.quoted = 0;
    state.input = input;
    state.child_inputs = NULL;
    state.hook = NULL;
    state.name = "";
    state.err_stream = NULL; /* For errors; initialized to stderr. */
    state.out_stream = NULL; /* For information; initialized to stdout. */
    state.pstate = NULL; /* Private, for use by argp.  */

    return err;


//     struct parser parser;
//     /* If true, then err == EBADKEY is a result of a non-option argument failing
//        to be parsed (which in some cases isn't actually an error).  */
//     int arg_ebadkey = 0;
//     if (!(flags & ARGP_NO_HELP))
//     /* Add our own options.  */
//     {
//         struct argp_child *child = alloca(4 * sizeof(struct argp_child));
//         struct argp *top_argp = alloca(sizeof(struct argp));
//         /* TOP_ARGP has no options, it just serves to group the user & default
//        argps.  */
//         memset(top_argp, 0, sizeof(*top_argp));
//         top_argp->children = child;
//         memset(child, 0, 4 * sizeof(struct argp_child));
//         if (argp)
//             (child++)->argp = argp;
//         (child++)->argp = &argp_default_argp;
//         if (argp_program_version || argp_program_version_hook)
//             (child++)->argp = &argp_version_argp;
//         child->argp = 0;
//         argp = top_argp;
//     }
//     /* Construct a parser for these arguments.  */
//     err = parser_init(&parser, argp, argc, argv, flags, input);
//     if (!err)
//     /* Parse! */
//     {
//         while (!err)
//             err = parser_parse_next(&parser, &arg_ebadkey);
//         err = parser_finalize(&parser, err, arg_ebadkey, end_index);
//     }
//     return err;
// }

// switch (key)
// {
// case ARGP_KEY_INIT:
//     break;
// case 'c':
//     raw.count = arg;
//     break;
// case 'd':
//     raw.random_deterministic = arg;
//     break;
// case 'g':
//     raw.sskr_groups.push_back(arg);
//     break;
// case 'h':
//     raw.ints_high = arg;
//     break;
// case 'i':
//     raw.input_format = arg;
//     break;
// case 'l':
//     raw.ints_low = arg;
//     break;
// case 'o':
//     raw.output_format = arg;
//     break;
// case 't':
//     raw.sskr_groups_threshold = arg;
//     break;
// case 'u':
//     raw.is_ur = true;
//     raw.max_fragment_length = arg != NULL ? arg : "";
//     break;
// case 'p':
//     raw.fountain_parts = arg;
//     break;
// case ARGP_KEY_ARG:
//     raw.args.push_back(arg);
//     break;
// case ARGP_KEY_END:
// {

}