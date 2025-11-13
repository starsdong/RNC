#!/bin/bash

# Step 1: Run average.py for track_1 to track_8
for i in {1..8}
do
    DIR="track_$i"
    echo "Processing $DIR ..."

    if [ -d "$DIR" ]; then
        python3 average.py "$DIR"
    else
        echo "  Skipped (directory not found: $DIR)"
    fi
done

echo "All track files processed."

# Step 2: Run merge.py on the output directory
echo "Running merge.py on output/ ..."
python3 merge.py output/

echo "Merge completed."
