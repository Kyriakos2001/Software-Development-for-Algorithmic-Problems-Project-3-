#pragma once

#include <gmp.h>
#include <iostream>
#include <map>
#include <vector>

// Macros and headers for CGAL
#include "cgal_definitions.h"

// Macros and headers for boost
#include "boost_definitions.h"

// Macros and headers for QT
#include "qt_definitions.h"

// Configuration
#include "triangulation_configuration.h"

// Support classes
#include "AntColonyStructures.h"
#include "JsonExporter.h"
#include "JsonLoader.h"
#include "RandomizationMethod.h"
#include "graph_definitions.h"
#include "steiner_strategies.h"
#include "utils.hpp"

// Namespaces
using namespace std;

template <typename U>
class AntColonySearch {
private:
    float calculateEnergy(float alpha, float beta, int obtuse_triangles, int steiner_points) {
        return alpha * obtuse_triangles + beta * steiner_points;
    }

    float radius_to_height_ratio(const Point& p1, const Point& p2, const Point& p3) {
        // Calculate side lengths of the triangle
        float a = std::sqrt(CGAL::to_double(CGAL::squared_distance(p2, p3)));
        float b = std::sqrt(CGAL::to_double(CGAL::squared_distance(p1, p3)));
        float c = std::sqrt(CGAL::to_double(CGAL::squared_distance(p1, p2)));

        // Calculate the semi-perimeter of the triangle
        float s = (a + b + c) / 2.0f;

        // Calculate the area of the triangle using Heron's formula
        float area = std::sqrt(s * (s - a) * (s - b) * (s - c));

        // Check if the area is zero to prevent division by zero
        if (area == 0) {
            std::cerr << "Degenerate triangle detected!" << std::endl;
            return -1.0f; // Indicate an error
        }

        // Calculate the circumradius R
        float R = (a * b * c) / (4.0f * area);

        // Find the longest side
        float longest_side = std::max({a, b, c});

        // Calculate the height corresponding to the longest side
        float height = (2.0f * area) / longest_side;

        // Calculate the radius-to-height ratio
        float rho = R / height;

        return rho;
    }

    float heuristic_function_vertex_projection(float p) {
        if (p <= 1.0) {
            return 0.0f;
        }
        return std::max(0.0f, (p - 1) / p);
    }

    float heuristic_function_circumcenter(float p) {
        return p / (2.0f + p);
    }

    float heuristic_function_midpoint(float p) {
        if (p >= 1.5f) {
            return 0.0f; // Midpoint heuristic is not applicable for p >= 1.5
        }

        float heuristic = (3.0f - 2.0f * p) / 3.0f;

        return std::max(0.0f, heuristic);
    }

    float heuristic_function_mean_adjacent(int adjacent_obtuse_count) {
        if (adjacent_obtuse_count >= 2) {
            return 1.0f; // Highest priority
        }

        // Otherwise, return 0 (no prioritization)
        return 0.0f;
    }

    float steiner_probability(vector<float>& tsp, vector<float>& h, int k, int chi, int psi) {
        // tsp[]: Array of pheromone values τ_sp for each Steiner point option
        // h[]: Array of heuristic values η_sp for each Steiner point option
        // total_options: Total number of Steiner point options
        // k: Index of the specific Steiner point we are calculating probability for
        // chi: χ parameter (default = 1)
        // psi: ψ parameter (default = 1)

        // Calculate the numerator for the specific option k
        int total_options = (int)tsp.size();

        float numerator = pow(tsp[k], chi) * pow(h[k], psi);

        // Calculate the denominator (sum over all options)
        float denominator = 0.0f;

        for (int i = 0; i < total_options; i++) {
            denominator += pow(tsp[i], chi) * pow(h[i], psi);
        }

        // Avoid division by zero
        if (denominator == 0.0f) {
            return 0.0f; // Assign zero probability if denominator is zero
        }

        // Calculate and return the probability
        return numerator / denominator;
    }

    float pheromone_reinforcement(bool reduced_obtuse_triangles, int obtuse_count, int steiner_count, float alpha, float beta) {
        // Parameters:
        // reduced_obtuse_triangles: Whether the Steiner point reduced the number of obtuse triangles
        // obtuse_count: The current number of obtuse triangles
        // steiner_count: The number of Steiner points added
        // alpha: Weight for the obtuse count term (default = 1.0)
        // beta: Weight for the Steiner count term (default = 1.0)

        if (reduced_obtuse_triangles) {
            // Calculate the reinforcement value
            return 1.0f / (1.0f + alpha * obtuse_count + beta * steiner_count);
        } else {
            // No reinforcement if obtuse triangles are not reduced
            return 0.0f;
        }
    }

    float pheromone_evaporation_and_reinforcement(float current_pheromone, float delta_pheromone, float evaporation_rate) {
        // Parameters:
        // current_pheromone: Current pheromone level (τ_sp)
        // delta_pheromone: Pheromone reinforcement (Δτ_sp)
        // evaporation_rate: Evaporation rate (λ), where 0 ≤ λ < 1

        // Ensure evaporation rate is within valid bounds
        if (evaporation_rate < 0.0f || evaporation_rate >= 1.0f) {
            std::cerr << "Error: Evaporation rate (λ) must be in the range [0, 1)." << std::endl;
            exit(100);
        }

        // Apply the formula: τ'_sp = (1 - λ) * τ_sp + Δτ_sp
        float updated_pheromone = (1.0f - evaporation_rate) * current_pheromone + delta_pheromone;

        return updated_pheromone;
    }

    int countObtuseNeighbors(Graph& graph, Point& a, Point& b, Point& c) {
        int counter = 0;

        for (int vertex_index = 0; vertex_index < 3; vertex_index++) {
            int neighbor_vertex_index = 0;

            Face* face = utils::findNeighbor(graph, a, b, c, vertex_index, neighbor_vertex_index);

            if (face != nullptr) {
                if (utils::is_obtuse(face->vertex(0)->point(), face->vertex(1)->point(), face->vertex(2)->point())) {
                    counter++;
                }
            }
        }

        return counter;
    }

    int selectMethodByProbability(Graph& graph, vector<steiner_stategies::Strategy>& strategies, CDT::Face_handle& face, Pheromones& pheromones, float xi, float psi) {
        Point a = face->vertex(0)->point();
        Point b = face->vertex(1)->point();
        Point c = face->vertex(2)->point();

        int adjacent_obtuse_count = countObtuseNeighbors(graph, a, b, c);

        float p = radius_to_height_ratio(a, b, c);

        float h_vertex_projection = heuristic_function_vertex_projection(p);
        float h_circumcenter = heuristic_function_circumcenter(p);
        float h_midpoint_longest_edge = heuristic_function_midpoint(p);
        float h_adjacent_obture_triangles = heuristic_function_mean_adjacent(adjacent_obtuse_count);

        // cout << "adjacent_obtuse_count: "  << adjacent_obtuse_count << endl;

        vector<float> h_values(strategies.size());

        for (unsigned int i = 0; i < strategies.size(); i++) {
            steiner_stategies::Strategy strategy = strategies[i];

            if (strategy == steiner_stategies::MAX_EDGE) {
                h_values[i] = h_midpoint_longest_edge;
            } else if (strategy == steiner_stategies::PERICENTER) {
                h_values[i] = h_circumcenter;
            } else if (strategy == steiner_stategies::POLYGON) {
                h_values[i] = h_adjacent_obture_triangles;
            } else if (strategy == steiner_stategies::PROJECTION) {
                h_values[i] = h_vertex_projection;
            } else {
                h_values[i] = 0;
            }
        }

        vector<float> psp_values(strategies.size());

        for (unsigned int i = 0; i < strategies.size(); i++) {
            psp_values[i] = steiner_probability(pheromones.values, h_values, i, xi, psi);
        }

        // for (unsigned int i = 0; i < strategies.size(); i++) {
        //     cout << psp_values[i] << " " ;
        // }

        // cout << endl;

        vector<float> cumulative_psp_values(strategies.size());

        for (unsigned int i = 0; i < strategies.size(); i++) {
            if (i == 0) {
                cumulative_psp_values[i] = psp_values[i];
            } else {
                cumulative_psp_values[i] = cumulative_psp_values[i - 1] + psp_values[i];
            }
        }

        cout << "Probabilities: ";

        for (unsigned int i = 0; i < strategies.size(); i++) {
            cout << cumulative_psp_values[i] << " ";
        }

        cout << endl;

        float random_variate = 0.01f * (rand() % 100);

        if (random_variate < cumulative_psp_values[0]) {
            return 0;
        }

        for (unsigned int i = 1; i < strategies.size(); i++) {
            if (random_variate < cumulative_psp_values[i]) {
                return i;
            }
        }

        return (int)strategies.size() - 1;
    }

public:
    vector<Point> triangulate(vector<steiner_stategies::Strategy>& strategies, Graph& graph, JsonLoader& loader, Polygon& boundaryPolygon, float alpha, float beta) {
        vector<Point> steinerPoints;
        vector<double> pn;
        CDT& cdt = *(graph.cdt);

        unsigned int total_methods = strategies.size();
        Pheromones pheromones(total_methods);

        int MAX_ITERATIONS = loader.getL();
        float xi = loader.getXi();
        float psi = loader.getPsi();
        float lambda = loader.getLambda();
        float kappa = loader.getKappa();

        int obtuse_triangles_initial = utils::countObtuseTriangles(cdt, *(graph.boundaryPolygon));
        int obtuse_triangles_after = 0;

        int convergence_iterations = 0;

        bool local_minimum_reached = false;

        cout << "# Initial Energy : " << calculateEnergy(alpha, beta, obtuse_triangles_initial, steinerPoints.size()) << endl;
        cout << "# Max iterations : " << MAX_ITERATIONS << endl;
        cout << "# Xi: " << xi << endl;
        cout << "# Psi: " << psi << endl;
        cout << "# Lambda: " << lambda << endl;
        cout << "# Kappa : " << kappa << endl;

        for (int loop = 0; loop < MAX_ITERATIONS; loop++) { // Cycles ...
            int obtuse_triangles_before = utils::countObtuseTriangles(cdt, *(graph.boundaryPolygon));
            float E_current = calculateEnergy(alpha, beta, obtuse_triangles_before, steinerPoints.size());
            float E_next = 0;

            convergence_iterations++;

            cout << " *** Current Energy: " << E_current << ", Pheromones = [" << pheromones << "]" << endl;

            std::vector<CDT::Face_handle> finite_faces; // all faces

            for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
                finite_faces.push_back(fit);
            }

            std::vector<CDT::Face_handle> obtuse_finite_faces; // obtuse faces

            for (auto fit : finite_faces) {
                Point a = fit->vertex(0)->point();
                Point b = fit->vertex(1)->point();
                Point c = fit->vertex(2)->point();

                bool result = utils::is_obtuse(a, b, c);

                if (result) {
                    obtuse_finite_faces.push_back(fit);
                }
            }

            if (obtuse_finite_faces.size() == 0) { // Stop if no obtuse faces left
                break;
            }

            std::vector<CDT::Face_handle> obtuse_finite_face_per_ant; // One face for each ant

            //
            // Find how many ants can work
            //

            for (int k = 0; k < kappa && obtuse_finite_faces.size() > 0; k++) {
                int j = rand() % obtuse_finite_faces.size();

                CDT::Face_handle face = obtuse_finite_faces[j];

                obtuse_finite_face_per_ant.push_back(face);

                obtuse_finite_faces.erase(obtuse_finite_faces.begin() + j);
            }

            int workingAnts = obtuse_finite_face_per_ant.size();

            cout << "Working ants: " << workingAnts << " from " << kappa << endl;

            //
            // Select triangulation method
            //
            vector<int> methodsPerAnt;

            for (int i = 0; i < workingAnts; i++) {
                int m = selectMethodByProbability(graph, strategies, obtuse_finite_face_per_ant[i], pheromones, xi, psi);

                methodsPerAnt.push_back(m);
            }

            //
            // Log
            //
            for (int i = 0; i < workingAnts; i++) {
                Point a = obtuse_finite_face_per_ant[i]->vertex(0)->point();
                Point b = obtuse_finite_face_per_ant[i]->vertex(1)->point();
                Point c = obtuse_finite_face_per_ant[i]->vertex(2)->point();

                cout << "  Ant " << i << ": Face: " << a << " , " << b << " , " << c << " ";

                steiner_stategies::printStrategy(strategies[methodsPerAnt[i]]);

                cout << endl;
            }

            vector<Point*> pointsPerAnt;
            vector<float> energyPerAnt;

            //
            // For each ant find steiner point and energy
            //
            for (int i = 0; i < workingAnts; i++) {
                CDT::Face_handle fit = obtuse_finite_face_per_ant[i];
                Point a = fit->vertex(0)->point();
                Point b = fit->vertex(1)->point();
                Point c = fit->vertex(2)->point();
                int N = methodsPerAnt[i];

                steiner_stategies::Strategy& selected_strategy = strategies[N];

                CDT cdt_copy = cdt;
                Graph graph_copy;
                graph_copy.cdt = &cdt_copy;
                graph_copy.boundaryPolygon = graph.boundaryPolygon;

                if (selected_strategy == steiner_stategies::Strategy::PERICENTER) { // if max edge is constraint skip ...
                    int i = utils::find_obtuse_angle(a, b, c);                      // 0:a, 1:b, 2:c
                    if (i == -1) {
                        cout << "CRITICAL ERROR: find_obtuse_angle failed " << endl;
                        exit(1);
                    }

                    std::tuple<int, int> edge_indices = utils::findOppositeEdge(i); // a:1,2 b:0,2 c:0,1

                    Point& p1 = fit->vertex(std::get<0>(edge_indices))->point();
                    Point& p2 = fit->vertex(std::get<1>(edge_indices))->point();

                    bool is_constraint = utils::checkConstraints(cdt_copy, boundaryPolygon, p1, p2);

                    if (is_constraint) {
                        pointsPerAnt.push_back(nullptr);
                        energyPerAnt.push_back(0);
                        continue;
                    }
                }

                Point* s = steiner_stategies::generateSteinerPoint(graph_copy, a, b, c, selected_strategy);

                if (s != nullptr) {
                    if (utils::is_steiner_point_valid(boundaryPolygon, *s)) {
                        cdt_copy.insertByStrategy(*s, selected_strategy);
                        steiner_stategies::removeConflictPoints(graph_copy, a, b, c, selected_strategy);

                        pointsPerAnt.push_back(s);
                    } else {
                        pointsPerAnt.push_back(nullptr);
                    }

                    cout << " i = " << i << " , " << *s << endl;

                    int copy_obtuse_triangles_after = utils::countObtuseTriangles(cdt_copy, *(graph.boundaryPolygon));

                    float E_next = calculateEnergy(alpha, beta, copy_obtuse_triangles_after, steinerPoints.size() + 1);

                    energyPerAnt.push_back(E_next);

                    // cout << "\t";
                    // steiner_stategies::printStrategy(selected_strategy);
                    // cout << " - New energy: " << E_next << " - Method succeeded " << copy_obtuse_triangles_after << endl;

                    // --------------------------------------------------------- energy
                } else {
                    // cout << "\t";
                    // steiner_stategies::printStrategy(selected_strategy);
                    // cout << " - New energy: " << E_next << " - Method failed    " << endl;
                    energyPerAnt.push_back(0);
                    pointsPerAnt.push_back(nullptr);
                }
            }

            //
            // Log
            //

            for (int i = 0; i < workingAnts; i++) {
                if (pointsPerAnt[i] != nullptr) {
                    bool reduces_energy = energyPerAnt[i] < E_current;

                    cout << "  Ant " << i << " : " << *pointsPerAnt[i] << ", energy: " << energyPerAnt[i] << ", reduces energy: " << (reduces_energy ? "true" : "false") << endl;

                    // if (!reduces_energy) {
                    //     pointsPerAnt[i] = nullptr;
                    // }
                } else {
                    cout << "  Ant " << i << " : " << "null" << ", energy: " << energyPerAnt[i] << endl;
                }
            }

            //
            // Find conflicts - Save best triangulation by comparing 2 ants each time
            //
            for (int i = 0; i < workingAnts; i++) {
                for (int j = i + 1; j < workingAnts; j++) {
                    if (pointsPerAnt[i] != nullptr && pointsPerAnt[j] != nullptr) {
                        int selected_strategy1 = methodsPerAnt[i];
                        int selected_strategy2 = methodsPerAnt[j];
                        Point* s1 = pointsPerAnt[i];
                        Point* s2 = pointsPerAnt[j];
                        float energy1 = energyPerAnt[i];
                        float energy2 = energyPerAnt[j];

                        Point a1 = obtuse_finite_face_per_ant[i]->vertex(0)->point();
                        Point b1 = obtuse_finite_face_per_ant[i]->vertex(1)->point();
                        Point c1 = obtuse_finite_face_per_ant[i]->vertex(2)->point();

                        Point a2 = obtuse_finite_face_per_ant[j]->vertex(0)->point();
                        Point b2 = obtuse_finite_face_per_ant[j]->vertex(1)->point();
                        Point c2 = obtuse_finite_face_per_ant[j]->vertex(2)->point();

                        CDT cdt_copy_1 = cdt;
                        CDT cdt_copy_2 = cdt;

                        cdt_copy_1.insertByStrategy(*s1, selected_strategy1);
                        steiner_stategies::removeConflictPoints(cdt_copy_1, a1, b1, c1, selected_strategy1);

                        cdt_copy_1.insertByStrategy(*s2, selected_strategy1);
                        steiner_stategies::removeConflictPoints(cdt_copy_1, a2, b2, c2, selected_strategy1);

                        cdt_copy_2.insertByStrategy(*s2, selected_strategy2);
                        steiner_stategies::removeConflictPoints(cdt_copy_2, a2, b2, c2, selected_strategy2);

                        cdt_copy_2.insertByStrategy(*s1, selected_strategy2);
                        steiner_stategies::removeConflictPoints(cdt_copy_2, a1, b1, c1, selected_strategy2);

                        if (cdt_copy_1 != cdt_copy_2) {
                            if (energy1 < energy2) {
                                pointsPerAnt[j] = nullptr;
                            } else {
                                pointsPerAnt[i] = nullptr;
                            }
                        }
                    }
                }
            }

            //
            // Find combined energy
            //
            CDT cdt_copy = cdt;
            for (int i = 0; i < workingAnts; i++) {
                if (pointsPerAnt[i] != nullptr) {
                    int selected_strategy = methodsPerAnt[i];
                    Point* s = pointsPerAnt[i];

                    Point a1 = obtuse_finite_face_per_ant[i]->vertex(0)->point();
                    Point b1 = obtuse_finite_face_per_ant[i]->vertex(1)->point();
                    Point c1 = obtuse_finite_face_per_ant[i]->vertex(2)->point();

                    cdt_copy.insertByStrategy(*s, selected_strategy);

                    steiner_stategies::removeConflictPoints(cdt_copy, a1, b1, c1, selected_strategy);
                }
            }

            int copy_obtuse_triangles_after_all_ants = utils::countObtuseTriangles(cdt_copy, *(graph.boundaryPolygon));
            int added_points = 0;

            for (int i = 0; i < workingAnts; i++) {
                if (pointsPerAnt[i] != nullptr) {
                    added_points++;
                }
            }

            float E_next_all_ants = calculateEnergy(alpha, beta, copy_obtuse_triangles_after_all_ants, steinerPoints.size() + added_points);

            int temp = steinerPoints.size();

            if (E_next_all_ants < E_current) {
                //
                // Apply triangulation
                //
                for (int i = 0; i < workingAnts; i++) {
                    if (pointsPerAnt[i] != nullptr) {
                        int selected_strategy = methodsPerAnt[i];
                        Point* s = pointsPerAnt[i];

                        Point a = obtuse_finite_face_per_ant[i]->vertex(0)->point();
                        Point b = obtuse_finite_face_per_ant[i]->vertex(1)->point();
                        Point c = obtuse_finite_face_per_ant[i]->vertex(2)->point();

                        cdt.insertByStrategy(*s, selected_strategy);
                        steiner_stategies::removeConflictPoints(graph, a, b, c, selected_strategy);

                        steinerPoints.push_back(*s);
                    }
                }

                int step = steinerPoints.size() - temp;

                cout << "Steiner: " << steinerPoints.size() << endl;
                cout << "Step   : " << step << endl;
                cout << "obtuse_triangles_before   : " << step << endl;
                cout << "copy_obtuse_triangles_after_all_ants   : " << step << endl;

                pn.push_back(utils::calculate_p(steinerPoints.size(), step, obtuse_triangles_before, copy_obtuse_triangles_after_all_ants));

                //
                // Update pheromones
                //
                obtuse_triangles_after = utils::countObtuseTriangles(cdt, *(graph.boundaryPolygon));

                int reduced_obtuse_triangles = obtuse_triangles_after - obtuse_triangles_before;

                E_next = calculateEnergy(alpha, beta, obtuse_triangles_after, steinerPoints.size());

                for (unsigned int i = 0; i < total_methods; i++) {
                    float delta_pheromone = pheromone_reinforcement(reduced_obtuse_triangles, obtuse_triangles_after, steinerPoints.size(), alpha, beta);
                    pheromones.values[i] = pheromone_evaporation_and_reinforcement(pheromones.values[i], delta_pheromone, lambda);

                    cout << "pheromone: " << i << " changed to : " << pheromones.values[i] << endl;
                }

                if (reduced_obtuse_triangles == 0) {
                    // int x = RandomizationMethod<U>::tryMethod(cdt, boundaryPolygon, loader, pn, RANDOMIZATION_RETRIES);

                    // if (x < obtuse_triangles_after) {
                    //     obtuse_triangles_after = x;
                    // }
                }
            }

            cout << " ### Cycle: " << loop << " - Initial: " << obtuse_triangles_initial << ", before: " << obtuse_triangles_before << ", after: " << obtuse_triangles_before;

            cout << " Energy: " << E_current << " updated to " << E_next << endl;

            int x = utils::countObtuseTriangles(cdt, *(graph.boundaryPolygon));

            int reduced_obtuse_triangles = obtuse_triangles_after - x;

            int obtuse_triangles_after = x;

            if (ENABLE_RANDOMIZATION_METHOD && reduced_obtuse_triangles == 0) {
                int x = RandomizationMethod<U>::tryMethod(cdt, boundaryPolygon, loader, pn, steinerPoints.size(), RANDOMIZATION_RETRIES);

                if (x < obtuse_triangles_after) {
                    obtuse_triangles_after = x;
                    local_minimum_reached = false;
                }
            }

            if (obtuse_triangles_after == 0) {
                local_minimum_reached = true;
                break;
            }
        }

        double p = utils::average(pn);

        obtuse_triangles_after = utils::countObtuseTriangles(cdt, *(graph.boundaryPolygon));

        cout << "***********************************************************************" << endl;
        cout << " - Initial obtuse triangles   : " << obtuse_triangles_initial << endl;
        cout << " - Total obtuse triangles     : " << obtuse_triangles_after << endl;
        cout << " - Total steiner points       : " << steinerPoints.size() << endl;
        cout << " - Global minimum reached     : " << local_minimum_reached << endl;
        cout << " - Iterations for convergence : " << convergence_iterations << " of " << MAX_ITERATIONS << endl;
        cout << " - Energy - Initial           : " << alpha * obtuse_triangles_initial << endl;
        cout << " - Energy - Final             : " << alpha * obtuse_triangles_after + beta * steinerPoints.size() << endl;
        cout << " - Alpha                      : " << alpha << endl;
        cout << " - Beta                       : " << beta << endl;
        cout << " - Convergence rate metric   : " << p << endl;
        cout << "***********************************************************************" << endl;

        return steinerPoints;
    }
};