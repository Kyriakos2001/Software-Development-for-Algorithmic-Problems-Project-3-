from cgshop2025_pyutils.data_schemas.instance import Cgshop2025Instance
from cgshop2025_pyutils.data_schemas.solution import Cgshop2025Solution
from cgshop2025_pyutils import verify

import json
import sys

if len(sys.argv) != 3:
    print("Usage: python visualize_json.py <instance_file.json> <solution_file.json>")
    sys.exit(1)

instance_file, solution_file = sys.argv[1], sys.argv[2]

with open(instance_file, 'r') as file:
    data_in = json.load(file)

with open(solution_file, 'r') as file:
    data_out = json.load(file)

instance = Cgshop2025Instance(
    instance_uid=data_in["instance_uid"],
    num_points=data_in["num_points"],
    points_x=data_in["points_x"],
    points_y=data_in["points_y"],
    region_boundary=data_in["region_boundary"],
    num_constraints=data_in["num_constraints"],
    additional_constraints=data_in["additional_constraints"],
)

solution = Cgshop2025Solution(
    content_type="CG_SHOP_2025_Solution",
    instance_uid=data_out["instance_uid"],
    steiner_points_x=data_out["steiner_points_x"],
    steiner_points_y=data_out["steiner_points_y"],
    edges=data_out["edges"],
)

result = verify(instance, solution, strict=True)

if result.num_obtuse_triangles != -1:
    print(f"No. obtuse triangles: {result.num_obtuse_triangles}\nNo. Steiner points: {result.num_steiner_points}")
else:
    print("Errors:")

    for err in result.errors: print(f"- {err}")
