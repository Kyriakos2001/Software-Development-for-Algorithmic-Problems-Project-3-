#!/bin/bash

# Set parameters
BASE_DIRECTORY_NAME=c_convex_closed_constraints
INPUT_DIRECTORY="data/cgshop_challenge/$BASE_DIRECTORY_NAME"
MAKEFILE_INPUT_DIRECTORY="../$INPUT_DIRECTORY"
OUTPUT_DIRECTORY="./results/randomization"
DEBUG_DIRECTORY="./results/randomization/debug"

# Create the output directory if it doesn't exist
mkdir -p "$OUTPUT_DIRECTORY"
mkdir -p "$DEBUG_DIRECTORY"

# Methods to iterate over
# METHODS=("ac" "ls" "sa" "sals" "acls")
METHODS=("ac" "ls" "sa" "sals" "acls")

for METHOD in "${METHODS[@]}"; do
  RESULT_CSV="${OUTPUT_DIRECTORY}/${BASE_DIRECTORY_NAME}_${METHOD}.csv"
  
  echo "File Name,Initial Obtuse,Loops,Final Obtuse,Steiner Points,Energy,p" > "$RESULT_CSV"
  
  echo "Running method: $METHOD"

  for file in "$INPUT_DIRECTORY"/*.json; do
    if [[ ! -e "$file" ]]; then
      echo "No JSON files found in $INPUT_DIRECTORY."
      exit 1
    fi

    FILE_NAME=$(basename "$file")
    echo "Processing file: $FILE_NAME with method: $METHOD"

    # Call Makefile with the correct parameters
    OUTPUT=$(make DIRECTORY="$MAKEFILE_INPUT_DIRECTORY" FILE="$FILE_NAME" $METHOD  2>&1)

    # Debug: Save the full output in a log
    echo "$OUTPUT" > "$DEBUG_DIRECTORY/${BASE_DIRECTORY_NAME}_${METHOD}.log"

    # Adjust your parsing logic to avoid lookbehind assertions
    INITIAL_OBTUSE=$(echo "$OUTPUT" | grep "Initial obtuse triangles" | sed -E 's/.*Initial obtuse triangles[[:space:]]*:[[:space:]]*//')
    FINAL_OBTUSE=$(echo "$OUTPUT" | grep "Total obtuse triangles" | sed -E 's/.*Total obtuse triangles[[:space:]]*:[[:space:]]*//')
    STEINER=$(echo "$OUTPUT" | grep "Total steiner points" | sed -E 's/.*Total steiner points[[:space:]]*:[[:space:]]*//')
    LOOPS=$(echo "$OUTPUT" | grep "Iterations for convergence" | sed -E 's/.*Iterations for convergence[[:space:]]*:[[:space:]]*//;s/ of.*//')
    ENERGY=$(echo "$OUTPUT" | grep "Energy - Final" | sed -E 's/.*Energy - Final[[:space:]]*:[[:space:]]*//')
    P_METRIC=$(echo "$OUTPUT" | grep "Convergence rate metric" | sed -E 's/.*Convergence rate metric[[:space:]]*:[[:space:]]*//')


    # Handle missing data
    if [ -z "$INITIAL_OBTUSE" ]; then INITIAL_OBTUSE="N/A"; fi
    if [ -z "$FINAL_OBTUSE" ]; then FINAL_OBTUSE="N/A"; fi
    if [ -z "$STEINER" ]; then STEINER="N/A"; fi
    if [ -z "$LOOPS" ]; then LOOPS="N/A"; fi
    if [ -z "$ENERGY" ]; then ENERGY="N/A"; fi
    if [ -z "$P_METRIC" ]; then P_METRIC="N/A"; fi

    echo "DEBUG: Extracted -> Initial: $INITIAL_OBTUSE, Loops: $LOOPS, Final: $FINAL_OBTUSE, Steiner: $STEINER, Energy: $ENERGY, p: $P_METRIC"

    # Append to CSV
    echo "$FILE_NAME,$INITIAL_OBTUSE,$LOOPS,$FINAL_OBTUSE,$STEINER,$ENERGY,$P_METRIC" >> "$RESULT_CSV"
  done
  
  echo "Results for $METHOD saved to $RESULT_CSV"
done