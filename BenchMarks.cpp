
#include <boost/config.hpp>
#include <iostream>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <fstream>/*ifstream description*/
#include <boost/graph/labeled_graph.hpp>

#include <stack>
#if defined UNIX
#include <unistd.h>
#else
#include <direct.h> // _getcwd
#endif
#include "config.hpp"

/**
 * @brief Definition of supported unary and binary operations.
 */
std::set<std::string> unary = { "out", "reg", "not" };
std::set<std::string> binary = { "and", "nand", "or", "nor", "xor", "xnor" };

using namespace boost;

typedef subgraph< adjacency_list<vecS, vecS, directedS,
    property<vertex_color_t, int>, property<edge_index_t, int> > > Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor Node;

std::vector<std::string>
split(const std::string line, char delimiter)
{
    // Variable declarations
    std::vector<std::string> tokens;
    std::string token;

    // Turn string into stream
    std::stringstream stream(line);

    // Extract tokens from line
    while (getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

Graph parse(const std::string file_name, std::vector<std::string>* gates)
{
    std::string filePath = file_name; //"/home/lucien/git/FVTSCMC/SILVER/test/" + file_name + ".nl";
    using namespace boost;
    std::vector<std::string> tokens, annotations;
    std::string line, token;
    // std::vector<std::string> gates;
    Graph model;
    char tmp[256];
#if defined UNIX
    getcwd(tmp, 256);
#else
    _getcwd(tmp, 256);
#endif
    std::cout << "Current working directory: " << tmp << std::endl;
    std::string dir;
    dir.assign(tmp, strlen(tmp));
#if defined UNIX
    filePath = dir + "/" + file_name;
#else
    filePath = dir + "\\" + file_name;
#endif

    std::ifstream description(filePath);
    //std::ifstream description;
    //description.open("gf256inv_dom2.txt", std::ios::in);
    if (!description.is_open()) {
        std::cout << filePath << std::endl;
        printf("File to parse does not exists, check your path. File must be in `nl` subdirectory in the current directory\n");
        exit(0);
    }
    while (std::getline(description, line))
    {
        tokens = split(line, ' ');

        Node node = add_vertex(model);
        (*gates).push_back(tokens[0]);
        tokens.push_back(tokens[0]);

        if (unary.find(tokens[0]) != unary.end()) {
            add_edge(node, std::stoi(tokens[1]), model);
        }
        else if (binary.find(tokens[0]) != binary.end()) {
            add_edge(node, std::stoi(tokens[1]), model);
            add_edge(node, std::stoi(tokens[2]), model);
        }
        else if (!(tokens[0] == "in" || tokens[0] == "ref")) {
            std::cerr << "[ERR-PARSER] Unsupported node detected: line #" << node + 1 << std::endl;
        }

    }
    // std::cout << "model:" << std::endl;
    // print_graph(model, get(vertex_index, model));

    std::cout << std::endl;

    description.close();
    return model;
}
/**
 * mk_out used by nl_to_mv
 */
std::vector<int> mk_out(std::vector<std::string> gates, Graph model) {
    std::vector<int> out;
    for (int i = 0; i < gates.size(); ++i)
    {
        if (gates[i] == "out") {
            int op1 = target(*(out_edges(i, model).first + 0), model);
            out.push_back(op1);
        }
    }
    return out;
}

/**
 * Related command line arguments are num_secret, num_share, num_random, file_name, step
 */
void nl_to_mv() {
    std::string file_name = opt.insfile;
    int num_secret = opt.num_secret;
    int num_output = opt.num_output;

    int num_inshares = opt.num_inshares;
    int num_outshares = opt.num_outshares;
    int num_random = opt.num_ref;

    int step_in = opt.step_in;
    int step_out = opt.step_out;

    std::vector<std::string> gates;
    Graph model = parse(file_name, &gates);
    std::pair<boost::adjacency_list<>::vertex_iterator,
        boost::adjacency_list<>::vertex_iterator> vs = boost::vertices(model);

    std::string circuit = file_name;
    std::size_t found = file_name.find_last_of("/\\");
    // std::cout << " file: " << file_name.substr(found+1) << '\n';
    if (found != std::string::npos)
        circuit = file_name.substr(found + 1);
    else circuit = file_name;
    circuit = circuit.replace(circuit.find(".nl"), 3, "");
    //circuit = circuit.replace(circuit.find("-"), 1, "_");
    std::string mvfile = file_name;
    if (mvfile.find("nl\\") != std::string::npos) {
        // path in windows system
        mvfile = mvfile.replace(mvfile.find("nl\\"), 3, "mv\\");
    }
    else if(mvfile.find("nl/") != std::string::npos) {
        // path in linux system
        mvfile = mvfile.replace(mvfile.find("nl/"), 3, "mv/");
    }
    mvfile = mvfile.replace(mvfile.find(".nl"), 3, ".mv");
    mvfile = mvfile.replace(mvfile.find(".mv"), 3, "_g.mv");
    //coutfile.open("/home/lucien/git/FVTSCMC/maskVerif/test/" + file_name + ".mv"); //open is the method of ofstream
    std::ofstream coutfile(mvfile);
    if (!coutfile.is_open()) {
        std::cout << "The location of mvfile: " << mvfile << std::endl;
        printf("The location of mvfile does not exists, please create the parent folders.\n");
        exit(0);
    }
    //std::ofstream coutfile;
    //coutfile.open(mvfile); //open is the method of ofstream

    int op1, op2;
    coutfile << "proc " + circuit + ":\n  inputs: ";
    for (int i = 0; i < num_secret; ++i)
    {
        coutfile << char('a' + i) << " = ";
        for (int j = 0; j < num_inshares; ++j) {
            if (j < num_inshares - 1) coutfile << "tmp" << i / step_in * step_in * num_inshares + (i % step_in) + step_in * j << " + ";
            else coutfile << "tmp" << i / step_in * step_in * num_inshares + (i % step_in) + step_in * j << "";
        }
        if (i < num_secret - 1) coutfile << ", ";
        else coutfile << "\n";

    }
    std::vector<int> out = mk_out(gates, model);
    coutfile << "  outputs:";
    int num_guards = out.size() - num_secret * num_outshares;
    for (int i = 0; i < num_output/*out.size() / num_outshares*/; ++i)
    {
        if (i < num_output)
        {
            coutfile << "a" << char('a' + i) << " = ";
            for (int j = 0; j < num_outshares; ++j) {
                if (j < num_outshares - 1) coutfile << "tmp" << out[i / step_out * step_out * num_outshares + (i % step_out) + step_out * j] << " + ";
                else coutfile << "tmp" << out[i / step_out * step_out * num_outshares + (i % step_out) + step_out * j] << "";
            }
        }
        if (i < num_output - 1) coutfile << ", ";
        else if (num_random != 0) coutfile << "\n";
        else if (num_random == 0) coutfile << ";\n\n";
    }
    if (num_random != 0)coutfile << "  randoms:";
    for (int i = 0; i < num_random; ++i)
    {
        if (i < num_random - 1) coutfile << "tmp" << i + num_secret * num_inshares << ",";
        else coutfile << "tmp" << i + num_secret * num_inshares << ";\n\n";
    }
    for (auto node = vs.first; node != vs.second; node++) {
        if (gates[*node] == "reg" /*|| gates[*node] == "out"*/)
        {
            op1 = target(*(out_edges(*node, model).first + 0), model);
            coutfile << "  tmp" << *node << " = ![tmp" << op1 << "];" << std::endl;
        }
        if (gates[*node] == "not")
        {
            op1 = target(*(out_edges(*node, model).first + 0), model);
            coutfile << "  tmp" << *node << " := ~tmp" << op1 << ";" << std::endl;
        }
        if (gates[*node] == "and") {
            op1 = target(*(out_edges(*node, model).first + 0), model);
            op2 = target(*(out_edges(*node, model).first + 1), model);
            coutfile << "  tmp" << *node << " := tmp" << op1 << " * tmp" << op2 << ";" << std::endl;
        }

        if (gates[*node] == "xor") {
            op1 = target(*(out_edges(*node, model).first + 0), model);
            op2 = target(*(out_edges(*node, model).first + 1), model);
            coutfile << "  tmp" << *node << " := tmp" << op1 << " + tmp" << op2 << ";" << std::endl;
        }

        if (gates[*node] == "nor") {
            op1 = target(*(out_edges(*node, model).first + 0), model);
            op2 = target(*(out_edges(*node, model).first + 1), model);
            coutfile << "  tmp" << *node << " := ~tmp" << op1 << " * ~tmp" << op2 << ";" << std::endl;
        }

        if (gates[*node] == "nand") {
            op1 = target(*(out_edges(*node, model).first + 0), model);
            op2 = target(*(out_edges(*node, model).first + 1), model);
            coutfile << "  tmp" << *node << " := ~(tmp" << op1 << " * tmp" << op2 << ");" << std::endl;
        }

        if (gates[*node] == "or") {
            op1 = target(*(out_edges(*node, model).first + 0), model);
            op2 = target(*(out_edges(*node, model).first + 1), model);
            coutfile << "  tmp" << *node << " := ~(~tmp" << op1 << " * ~tmp" << op2 << ");" << std::endl;
        }

        if (gates[*node] == "xnor") {
            op1 = target(*(out_edges(*node, model).first + 0), model);
            op2 = target(*(out_edges(*node, model).first + 1), model);
            coutfile << "  tmp" << *node << " := ~(tmp" << op1 << " + tmp" << op2 << ");" << std::endl;
        }
    }
    coutfile << "end\n\n";
    coutfile << "order " + std::to_string((opt.order)) + " Probing " + circuit;
    coutfile << "\n";
    coutfile.close();
    std::ifstream ifs(mvfile);
    std::string content((std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));
    //std::cout << content << std::endl;
    content = content.replace(content.find(" Probing "), 9, " noglitch Probing ");
    ifs.close();
    std::string ng_mvfile = mvfile.replace(mvfile.find("_g.mv"), 5, "_ng.mv");
    coutfile.open(ng_mvfile);
    coutfile << content;
}

int main(int argc, char* argv[])
{
    /* Build the argument parser and run it on argc/argv */
    po::options_description desc = build_argument_parser(&opt);
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (po::error e)
    {
        std::cout << e.what();
        return 0;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (opt.tool == "maskVerif") {
        nl_to_mv();
    }

    return 0;
}
