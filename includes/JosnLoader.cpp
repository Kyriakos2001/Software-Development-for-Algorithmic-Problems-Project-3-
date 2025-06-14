#include <iostream>
#include <vector>

#include "JsonLoader.h"

// Namespaces
using namespace std;

void JsonLoader::load(const char* inputfile, bool load_hyperparameters) {
    try {
        boost::property_tree::read_json(inputfile, pt);
    } catch (const std::exception& e) {
        std::cerr << "Error reading JSON file: " << e.what() << std::endl;
        exit(1);
    }

    instance_uid = pt.get<std::string>("instance_uid");
    num_points = pt.get<int>("num_points");
    num_constraints = pt.get<int>("num_constraints");

    for (auto& point : pt.get_child("points_x")) {
        points_x.push_back(point.second.get_value<int>());
    }
    for (auto& point : pt.get_child("points_y")) {
        points_y.push_back(point.second.get_value<int>());
    }
    for (auto& boundary : pt.get_child("region_boundary")) {
        region_boundary.push_back(boundary.second.get_value<int>());
    }

    for (auto& constraint : pt.get_child("additional_constraints")) {
        int first = constraint.second.front().second.get_value<int>();
        int second = constraint.second.back().second.get_value<int>();
        additional_constraints.emplace_back(first, second);
    }

    // load method, parameters etc.

    if (load_hyperparameters) {
        method = pt.get<std::string>("method");
        
        if (method == "legacy") {
            L = pt.get<int>("parameters.L");
        } else if (method == "local") {
            L = pt.get<int>("parameters.L");
        } else if (method == "sa" || method == "sals") {
            L = pt.get<int>("parameters.L");
            alpha = pt.get<float>("parameters.alpha");
            beta = pt.get<float>("parameters.beta");
        } else if (method == "ant" || method == "acls") {
            L = pt.get<int>("parameters.L");
            alpha = pt.get<float>("parameters.alpha");
            beta = pt.get<float>("parameters.beta");
            xi = pt.get<float>("parameters.xi");
            psi = pt.get<float>("parameters.psi");
            lambda = pt.get<float>("parameters.lambda");
            kappa = pt.get<float>("parameters.kappa");
        }
    }
}

void JsonLoader::print() {
    std::cout << "Instance UID: " << instance_uid << "\n";
    std::cout << "Method      : " << method << "\n";
    std::cout << "Number of Points: " << num_points << "\n";
    std::cout << "Number of Constraints: " << num_constraints << "\n";

    std::cout << "\nPoints X: ";
    for (int x : points_x) {
        std::cout << x << " ";
    }

    std::cout << "\nPoints Y: ";
    for (int y : points_y) {
        std::cout << y << " ";
    }

    std::cout << "\nRegion Boundary: ";
    for (int boundary : region_boundary) {
        std::cout << boundary << " ";
    }

    std::cout << "\nAdditional Constraints:\n";
    for (const auto& constraint : additional_constraints) {
        std::cout << "[" << constraint.first << ", " << constraint.second << "]\n";
    }
}

vector<Point> JsonLoader::getPoints() {
    vector<Point> points;

    unsigned int N = points_x.size();

    for (unsigned int i = 0; i < N; i++) {
        points.emplace_back(Point(points_x[i], points_y[i]));
    }

    return points;
}

std::vector<std::pair<int, int>> JsonLoader::getConstraints() {
    return additional_constraints;
}

std::vector<int> JsonLoader::getRegionBoundaries() {
    return region_boundary;
}

string JsonLoader::getInstance() const {
    return instance_uid;
}

string JsonLoader::getMethod() const {
    return method;
}

int JsonLoader::getL() const {
    return L;
}

float JsonLoader::getAlpha() const {
    return alpha;
}

float JsonLoader::getBeta() const {
    return beta;
}

float JsonLoader::getXi() const {
    return xi;
}

float JsonLoader::getPsi() const {
    return psi;
}

float JsonLoader::getLambda() const {
    return lambda;
}

float JsonLoader::getKappa() const {
    return kappa;
}



