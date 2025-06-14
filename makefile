# METHOD?=simple
METHOD?=local
# METHOD?=sa
# METHOD?=ant

# R ?= true
R ?= false

#
# Local search arguments
#
L ?= 5

#
# Simulated annealing arguments
#
ALPHA ?= 2.5
BETA ?= 0.4

#
# Ant colony arguments
#
XI ?= 1.0
YI ?= 3.0
LAMBDA ?= 0.5
K ?= 7


#
# File system arguments
#

DIRECTORY ?= "../data/cgshop_challenge/a_convex_no_constraints"
FILE ?= "point-set_100_05594822.instance.json"

# DIRECTORY ?= "../data/cgshop_challenge/c_convex_closed_constraints"
# FILE ?= "simple-polygon-exterior_100_37aaf06f.instance.json"

# DIRECTORY ?= "../data/cgshop_challenge/d_non_convex_ortho_no_constraints"
# FILE ?= "ortho_20_5a9e8244.instance.json"

#
# Run search methods
#
.PHONY: ls
ls:
	@echo "Running LS: $(DIRECTORY)/$(FILE).json => $(DIRECTORY)/output/$(FILE)_output.json ..."
	cd build; mkdir -p $(DIRECTORY)/output
	cd build; make && ./polyg "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json" -m ls -L $(L) -R $(R)
#	cd build; python ../visualize.py "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json"
	cd build; python ../validate.py "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json"

.PHONY: sa
sa:
	@echo "Running LS: $(DIRECTORY)/$(FILE).json => $(DIRECTORY)/output/$(FILE)_output.json ..."
	cd build; mkdir -p $(DIRECTORY)/output
	cd build; make && ./polyg "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json" -m sa -L $(L) -a $(ALPHA) -b $(BETA) -R $(R)
#	cd build; python ../visualize.py "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json"
	cd build; python ../validate.py "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json"

.PHONY: sals
sals:
	@echo "Running LS: $(DIRECTORY)/$(FILE).json => $(DIRECTORY)/output/$(FILE)_output.json ..."
	cd build; mkdir -p $(DIRECTORY)/output
	cd build; make && ./polyg "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json" -m sals -L $(L) -a $(ALPHA) -b $(BETA) -R $(R)
	cd build; python ../visualize.py "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json"
	cd build; python ../validate.py "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json"

.PHONY: ac
ac:
	@echo "Running LS: $(DIRECTORY)/$(FILE).json => $(DIRECTORY)/output/$(FILE)_output.json ..."
	cd build; mkdir -p $(DIRECTORY)/output
	cd build; make && ./polyg "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json" -m ant -L $(L) -a $(ALPHA) -b $(BETA) -x $(XI) -y $(YI) -l $(LAMBDA) -k $(K) -R $(R) 
#	cd build; python ../visualize.py "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json"
	cd build; python ../validate.py "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json"

.PHONY: acls
acls:
	@echo "Running LS: $(DIRECTORY)/$(FILE).json => $(DIRECTORY)/output/$(FILE)_output.json ..."
	cd build; mkdir -p $(DIRECTORY)/output
	cd build; make && ./polyg "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json" -m acls -L $(L) -a $(ALPHA) -b $(BETA) -x $(XI) -y $(YI) -l $(LAMBDA) -k $(K) -R $(R) 
#	cd build; python ../visualize.py "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json"
	cd build; python ../validate.py "$(DIRECTORY)/$(FILE)" "$(DIRECTORY)/output/$(FILE)_output.json"

#
# Legacy
# 
.PHONY: legacy
legacy:
	cd build; make && ./polyg ../data/cgshop_challenge/a_convex_no_constraints/point-set_10_4bcb7c21.instance.json ../data_outputs/test.json -m legacy -L 1

.PHONY: run
run:
	cd build; make && ./polyg ../data/simple/input.json ../data_outputs/output.json

.PHONY: test1
test1:
	cd build; make && ./polyg ../data/simple/instance_1_$(METHOD).json ../data_outputs/output_1_$(METHOD).json

.PHONY: test2
test2:
	cd build; make && ./polyg ../data/simple/instance_2_$(METHOD).json ../data_outputs/output_2_$(METHOD).json

.PHONY: test3
test3:
	cd build; make && ./polyg ../data/simple/instance_3_$(METHOD).json ../data_outputs/output_3_$(METHOD).json

.PHONY: test4
test4:
	cd build; make && ./polyg ../data/simple/instance_4_$(METHOD).json ../data_outputs/output_4_$(METHOD).json

.PHONY: test5
test5:
	cd build; make && ./polyg ../data/simple/instance_5_$(METHOD).json ../data_outputs/output_5_$(METHOD).json

.PHONY: test6
test6:
	cd build; make && ./polyg ../data/simple/instance_6_$(METHOD).json ../data_outputs/output_6_$(METHOD).json					

.PHONY: test7
test7:
	cd build; make && ./polyg ../data/simple/instance_7_$(METHOD).json ../data_outputs/output_7_$(METHOD).json

.PHONY: clean
clean:
	rm -rf build

.PHONY: all
all: test1 test2 test3 test4 test5 test6 test7


.PHONY: build
build: clean
	mkdir build
	cd build; cmake ..

.PHONY: dist
dist: clean
	mkdir build
	cd build; cmake -DCMAKE_BUILD_TYPE=Release ..


.PHONY: debug
debug:	
	cd build; gdb --args ./polyg "../data/cgshop_challenge/a_convex_no_constraints/point-set_150_1fb326cf.instance.json" "../data/cgshop_challenge/d_non_convex_ortho_no_constraints/output/debug.json" -m sa -L 50


