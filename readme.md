# DO NOT deliver 

- .vscode
- build
- docs
- .git
- readme.md
- makefile
- .gitignore

Do this

# How to run program

```bash
    cd build
    cmake ..
    make
    ./polyg input.json output.json
```

# Algorithm

- Input: points and edges
- Output: non obtuse triangles

| Step | Algorithm                                            | Result                          |
| ---- | ---------------------------------------------------- | ------------------------------- |
| 1    | add points                                           | just points                     |
| 2    | add edges                                            | graph                           |
| 3    | add Delanau                                          | obtuse and non obtuse triangles |
| 4a   | for each triangle, that is obtuse, try edge flip     | obtuse and non obtuse triangles |
| 4b   | for each triangle, that is obtuse, add steiner point | obtuse and non obtuse triangles |
| 5    | Repeat step 4 until no obtuse exist                  | non obtuse triangles            |

# How to run program

Ερωτήσεις:

```
Δίνεται Α(x1,y1), B(x2,y2), C(x3,y3):

Ζητούμενο: Γράψτε κώδικα σε CGAL που να βρίσκει αν το πολύγωνο είναι αμβλυγώνιο.
```

```
Δίνεται Α(x1,y1), B(x2,y2), C(x3,y3):

Βρες που θα τοποθετηθεί το steiner point (βλέπε e-class)
```


# Class (cgshop_challenge)

- L : iterations
- OT: Obtuse triangles
- SP: Steiner points
- E: energy = a*OT + b*SP

- a = 2
- b = 0.2

| File                                | Starting | LS - L | LS - OT | LS - SP | LS - E | SA - L | SA - OT | SA - SP | SA - E | AC - L | AC - OT | AC - SP | AC - E |
| ----------------------------------- | -------- | ------ | ------- | ------- | ------ | ------ | ------- | ------- | ------ | ------ | ------- | ------- | ------ |
| point-set_10_4bcb7c21.instance.json | 6        | 4      | 2       | 4       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_7451a2a9.instance.json | 5        | 3      | 2       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |
| point-set_10_97578aae.instance.json | 6        | 3      | 3       | 3       | X      | 0      | 0       | 0       | 0      | 0      | 0       | 0       | 0      |



# Run examples:

Directories:

- a_convex_no_constraints
- b_convex_open_constraints
- c_convex_closed_constraints
- d_non_convex_ortho_no_constraints
- e_non_convex


```bash
    make DIRECTORY="../data/cgshop_challenge/a_convex_no_constraints" FILE="point-set_10_4bcb7c21.instance.json" L=15 ls
```

```bash
    make DIRECTORY="../data/cgshop_challenge/a_convex_no_constraints" FILE="point-set_10_4bcb7c21.instance.json" L=15 a=2 b=0.2 sa
```

```bash
    make DIRECTORY="../data/cgshop_challenge/a_convex_no_constraints" FILE="point-set_10_4bcb7c21.instance.json" L=15 a=2 b=0.2 x=1 y=3 l=0.5 k=10 ac
```