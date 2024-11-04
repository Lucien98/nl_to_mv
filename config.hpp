#include <string>
#include <boost/program_options.hpp>

namespace po = boost::program_options;


/* Contains all configuration options for the Silver verify tool. */
typedef struct Options {

    /* number of secret variable */
    uint32_t num_secret;

    /* number of output secret variable */
    uint32_t num_output;

    /* number of input shares */
    uint32_t num_inshares;

    /* number of output shares */
    uint32_t num_outshares;

    /* the step between the id of input shares */
    uint32_t step_in;

    /* the step between the id of output shares */
    uint32_t step_out;

    /* number of refreshing bits */
    uint32_t num_ref;

    /* Instruction list */
    std::string insfile;

    /* Convert to the input file of which tool*/
    std::string tool;

    /* security order */
    uint32_t order;
} options;

po::options_description build_argument_parser(
    options* opt
) {
    po::options_description all("Benchmark arguments");
    all.add_options()
        ("help", "Show the help message")
		
        ("num_secret", po::value<uint32_t>(&opt->num_secret)->default_value(4),
            "number of secret inputs")

        ("num_output", po::value<uint32_t>(&opt->num_output)->default_value(4),
            "number of secret outputs")

        ("num_inshares", po::value<uint32_t>(&opt->num_inshares)->default_value(2),
            "number of input shares")

        ("num_outshares", po::value<uint32_t>(&opt->num_outshares)->default_value(2),
            "number of output shares")

        ("step_in", po::value<uint32_t>(&opt->step_in)->default_value(4),
            "the step between the id of input shares")

        ("step_out", po::value<uint32_t>(&opt->step_out)->default_value(4),
            "the step between the id of output shares")

        ("num_ref", po::value<uint32_t>(&opt->num_ref)->default_value(4),
            "number of refreshing bits")

        ("insfile", po::value<std::string>(&opt->insfile)->default_value("test/aes/aes_dom1/aes_sbox_dom1.nl"),
            "Instruction list to convert. ")

        ("tool", po::value<std::string>(&opt->tool)->default_value("maskVerif"),
            "Convert to the input file of which tool. ")

        ("order", po::value<uint32_t>(&opt->order)->default_value(1),
            "The security order of the masked implementation")
        ;

    return all;
}

/* Configuration options / command line arguments */
options opt;
