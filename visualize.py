import json
import sys
import matplotlib.pyplot as plt

# Function to parse points from JSON data
def parse_points(points_x, points_y):
    return [(int(x), int(y)) for x, y in zip(points_x, points_y)]

# Function to parse fractional points (e.g., Steiner points) from JSON data
def parse_fractional_points(points_x, points_y):
    def to_float(fraction):
        numerator, denominator = map(int, fraction.split('/'))
        return numerator / denominator

    return [(to_float(x), to_float(y)) for x, y in zip(points_x, points_y)]

# Function to draw edges between points
def draw_edges(ax, points, edges, color, label):
    for edge in edges:
        p1, p2 = points[edge[0]], points[edge[1]]
        ax.plot([p1[0], p2[0]], [p1[1], p2[1]], color=color, label=label if edge == edges[0] else "")

# Main function
def main():
    if len(sys.argv) != 3:
        print("Usage: python visualize_json.py <instance_file.json> <solution_file.json>")
        sys.exit(1)

    instance_file, solution_file = sys.argv[1], sys.argv[2]

    # Load JSON data
    with open(instance_file, 'r') as f:
        instance_data = json.load(f)

    with open(solution_file, 'r') as f:
        solution_data = json.load(f)

    # Parse instance data
    points = parse_points(instance_data["points_x"], instance_data["points_y"])
    region_boundary = instance_data["region_boundary"]
    additional_constraints = instance_data["additional_constraints"]

    # Parse solution data
    steiner_points = parse_fractional_points(solution_data["steiner_points_x"], solution_data["steiner_points_y"])
    edges = solution_data["edges"]

    # Create plot
    fig, ax = plt.subplots()

    # Plot points
    points_x, points_y = zip(*points)
    ax.scatter(points_x, points_y, color='black', label='Points')

    # Plot Steiner points
    if steiner_points:
        steiner_x, steiner_y = zip(*steiner_points)
        ax.scatter(steiner_x, steiner_y, color='red', label='Steiner Points')

    # Draw additional constraints
    draw_edges(ax, points, additional_constraints, color='red', label='Constraints')

    # Draw solution edges
    all_points = points + steiner_points
    draw_edges(ax, all_points, edges, color='grey', label='Solution Edges')

     # Draw region boundary
    boundary_edges = [(region_boundary[i], region_boundary[(i + 1) % len(region_boundary)]) for i in range(len(region_boundary))]
    draw_edges(ax, points, boundary_edges, color='green', label='Boundary')

    # Set plot properties
    ax.set_aspect('equal')
    ax.set_title('Polygon Visualization')
    ax.legend()
    plt.show()

if __name__ == "__main__":
    main()
