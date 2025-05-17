#include <bnp/bnp.h>

#include <boost/uuid/name_generator_sha1.hpp>
#include <boost/uuid/time_generator_v1.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string_view>
#include <string>
#include <stdexcept>
#include <random>

using namespace std;
using namespace boost::uuids;

void visit_object(auto &&v, auto &&o) {
    for (; v; ++v)
        visit(v, o);
}

struct command_line_parser {
    command_line_parser(const char *const *argv) {
        argv++;
        this->argv = argv;
        parse_argument();
    }
    operator bool() const {
        return *argv != nullptr;
    }
    command_line_parser &operator++() {
        if (!consumed)
            throw runtime_error(string("Unrecognized argument ") + string(arg));
        consumed = false;
        argv++;
        parse_argument();
        return *this;
    }
    void parse_argument() {
        if (!*argv) {
            arg = {};
            return;
        }
        arg = *argv;
        dashed = (arg.substr(0, 2) == "--");
        if (dashed)
            arg = arg.substr(2);
    }
    string_view value() {
        // to read key-value pairs
        argv++;
        if (*argv)
            return *argv;
        throw runtime_error(string(arg) + " requires a value.");
        return {};
    }

    const char *const *argv;
    string_view arg;
    bool consumed = false;
    bool dashed = false;
};

void flag(
    command_line_parser &p, string_view n, bool &f, string_view description
) {
    if (!p.dashed || n != p.arg)
        return;
    f = true;
    p.consumed = true;
}

void argument(
    command_line_parser &p, string_view n, string_view &s, 
    string_view description
) {
    if (!p.dashed || n != p.arg)
        return;
    s = p.value();
    p.consumed = true;
}

struct help_printer {
    operator bool() const {
        return !done;
    }
    help_printer &operator++() {
        done = true;
        return *this;
    }
    bool done = false;
};

void flag(
    help_printer &p, string_view n, bool &f, string_view description
) {
    cout << "--" << n << endl << "\t" << description << endl;
}

void argument(
    help_printer &p, string_view n, string_view &s, 
    string_view description
) {
    cout << 
        "--" << n << " [" << n << ']' << endl << "\t" << description << endl;
}

struct arguments {
    string_view command;
    string_view name, uuid_namespace, domain, node;
    bool uuid1 = false, uuid4 = false, uuid5 = false;
    bool help = false;
};

void visit(auto &&v, arguments &args) {
    argument(
        v, "name", args.name, "Name used to create a version 5 UUID."
    );
    argument(
        v, "namespace", args.uuid_namespace, 
        "Namespace UUID to be used to create a version 5 UUID."
    );
    argument(
        v, "domain", args.domain, 
        "A domain name that is used to create a UUID in the DNS namespace, "
        "which is then used as the namespace to create a version 5 UUID."
    );
    argument(
        v, "node", args.node, 
        "A UUID to take the node identifier from when "
        "creating a time based UUID."
    );
    flag(
        v, "uuid1", args.uuid1, 
        "Create a time-based UUID. If node is a UUID, "
        "create a UUID for the same node."
    );
    flag(v, "uuid4", args.uuid4, "Create a random UUID.");
    flag(
        v, "uuid5", args.uuid5, 
        "Create a name-based UUID. Requires name and domain or namespace."
    );
    
    flag(
        v, "help", args.help, 
        "Print help."
    );
}

int main(int argc, const char *argv[]) {
    arguments a;
    try {
        visit_object(command_line_parser(argv), a);
    } catch(runtime_error &e) {
        cout << e.what() << endl;
        a.help = true;
    }

    if (a.help) {
        visit_object(help_printer(), a);
        return -1;
    }

    if (a.uuid1) {
        if (a.node.empty()) {
            cout << time_generator_v1()() << endl;
        } else {
            random_device r;
            time_generator_v1::state_type state{
                .timestamp = 0,
                .clock_seq = (uint16_t)r(),
            };
            auto node_uuid = boost::lexical_cast<uuid>(a.node);
            if (node_uuid.version() != 1 && node_uuid.version() != 6)
                cerr << "UUID does not have a node-identifier." << endl;
            auto node = node_uuid.node_identifier();
            cout << time_generator_v1(node, state)() << endl;
        }
        
    } else if (a.uuid4) {
        cout << random_generator()() << endl;

    } else if (a.uuid5) {
        uuid uuid_namespace;
        if (a.uuid_namespace.empty()) {
            uuid_namespace = name_generator_sha1(ns::dns())(string(a.domain));
        } else {
            uuid_namespace = boost::lexical_cast<uuid>(a.uuid_namespace);
        }
        cout << name_generator_sha1(uuid_namespace)(string(a.name)) << endl;

    } else {
        visit_object(help_printer(), a);
        return -1;
    }
    
    return 0;
}
