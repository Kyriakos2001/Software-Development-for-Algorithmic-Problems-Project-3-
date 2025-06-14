#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include "JsonExporter.h"

using namespace std;

JsonExporter::JsonExporter(string instance_uid) {
    content_type = "CG_SHOP_2025_Solution";
    this->instance_uid = instance_uid;
}

void JsonExporter::print() const {
    std::cout << "Content Type: " << content_type << std::endl;
    std::cout << "Instance UID: " << instance_uid << std::endl;

    std::cout << "Steiner Points (X): ";
    for (const auto& x : steiner_points_x) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "Steiner Points (Y): ";
    for (const auto& y : steiner_points_y) {
        std::cout << y << " ";
    }
    std::cout << std::endl;

    std::cout << "Edges: ";
    for (const auto& edge : edges) {
        std::cout << "(" << edge.first << ", " << edge.second << ") ";
    }
    std::cout << std::endl;
}

void JsonExporter::save(const char* outputfile) {
    const char * tempfile = "tmp";
    std::ofstream json_file_out(outputfile);

    for (string temp : steiner_points_x) {
        pt_steiner_points_x.push_back(boost::property_tree::ptree::value_type("", temp));
    }

    for (string temp : steiner_points_y) {
        pt_steiner_points_y.push_back(boost::property_tree::ptree::value_type("", temp));
    }

    for (const auto& edge : edges) {
        boost::property_tree::ptree edge_node;

        std::string x_str = std::to_string(edge.first);
        std::string y_str = std::to_string(edge.second);

        edge_node.push_back(boost::property_tree::ptree::value_type("", x_str));

        edge_node.push_back(boost::property_tree::ptree::value_type("", y_str));

        pt_edges.push_back(boost::property_tree::ptree::value_type("", edge_node));
    }

    pt_root.put("content_type", "CG_SHOP_2025_Solution");
    pt_root.put("instance_uid", this->instance_uid);
    pt_root.add_child("steiner_points_x", pt_steiner_points_x);
    pt_root.add_child("steiner_points_y", pt_steiner_points_y);
    pt_root.add_child("edges", pt_edges);
    
    boost::property_tree::write_json(tempfile, pt_root);

    std::ifstream infile(tempfile);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open input file: " << tempfile << std::endl;
        return;
    }

    // Open the output file for writing
    std::ofstream outfile(outputfile);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open output file: " << outputfile << std::endl;
        infile.close();
        return;
    }


    // Flag to detect when inside the "edges" array
    bool inside_edges = false;

    std::string line;
    while (std::getline(infile, line)) {
        if (line.find("\"edges\"") != std::string::npos) {
            inside_edges = true;
            outfile << line << '\n';
            continue;
        } 

        if (inside_edges) {
            std::stringstream modified_line;
            bool inside_number = false;

            for (size_t i = 0; i < line.size(); ++i) {
                if (line[i] == '"') {
                    if (inside_number) {
                        inside_number = false;
                    } else {
                        inside_number = true;
                    }
                } else {
                    if (inside_number && (isdigit(line[i]) || line[i] == '.' || line[i] == '-' || line[i] == ',')) {
                        modified_line.put(line[i]);
                    } else if (!inside_number) {
                        modified_line.put(line[i]);
                    }
                }
            }

            outfile << modified_line.str() << '\n';
        } else {
            outfile << line << '\n';
        }

        // if (inside_edges && line.find("]") != std::string::npos) {
        //     inside_edges = false;
        // }
    }

    infile.close();
    outfile.close();

    unlink(tempfile);
}
