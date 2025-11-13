#!/usr/bin/env python3
import pandas as pd
import glob
import os
import sys
from functools import reduce

# Columns to drop from the "All Scores (Raw, Merged)" sheet
DROP_COLS = [
    "Speakers (affiliation)",
    "Speakers (email)",
    "Submitted for tracks",
    "Comment",
    "Comments",
]

# ----- Usage -----
# python3 merge.py output/
if len(sys.argv) < 2:
    print("Usage: python3 merge.py <folder_with_scores_xlsx>")
    sys.exit(1)

FOLDER = sys.argv[1]

if not os.path.isdir(FOLDER):
    raise NotADirectoryError(f"Not a valid folder: {FOLDER}")

# ----- Find all *_scores.xlsx files -----
files = glob.glob(os.path.join(FOLDER, "*_scores.xlsx"))
if not files:
    raise FileNotFoundError(f"No *_scores.xlsx files found in {FOLDER}")

print("Merging the following files:")
for f in files:
    print("  -", os.path.basename(f))

dfs_raw = []
common_cols = None

# ----- Load 'All Scores (Raw)' from each file and determine common key columns -----
for f in files:
    df = pd.read_excel(f, sheet_name="All Scores (Raw)")
    dfs_raw.append(df)

    if common_cols is None:
        common_cols = set(df.columns)
    else:
        common_cols &= set(df.columns)

if not common_cols:
    raise ValueError("No common columns found across 'All Scores (Raw)' sheets.")

first_df = dfs_raw[0]
key_cols = [c for c in first_df.columns if c in common_cols]

if "Id" not in key_cols:
    raise KeyError("'Id' column not found as common across all files.")

print("\nDetected key columns:")
for c in key_cols:
    print("  -", c)

# ----- Rename score columns so they are unique per file -----
dfs = []
for f, df in zip(files, dfs_raw):
    file_tag = os.path.splitext(os.path.basename(f))[0]  # e.g. "track_2_scores"
    score_cols = [c for c in df.columns if c not in key_cols]

    # Rename each score column to include the file tag
    rename_map = {c: f"{file_tag}::{c}" for c in score_cols}
    df_renamed = df.rename(columns=rename_map)

    dfs.append(df_renamed)

# ----- Merge all wide tables on key columns (outer join) -----
def merge_on_keys(left, right):
    return pd.merge(left, right, on=key_cols, how="outer")

wide_merged = reduce(merge_on_keys, dfs)

# Identify score columns in the merged table
score_cols_merged = [c for c in wide_merged.columns if c not in key_cols]

# Convert score columns to numeric
for col in score_cols_merged:
    wide_merged[col] = pd.to_numeric(wide_merged[col], errors="coerce")

# ----- Build long version (Id + all individual scores) -----
long_all = wide_merged.melt(
    id_vars=key_cols,
    value_vars=score_cols_merged,
    var_name="Source",
    value_name="Score",
)

# Drop NaN scores
long_all["Score"] = pd.to_numeric(long_all["Score"], errors="coerce")
long_all = long_all.dropna(subset=["Score"])

if long_all.empty:
    raise ValueError("All scores are NaN after conversion; nothing to aggregate.")

# ----- Descriptor table from merged (not just first file) -----
desc_cols = [c for c in key_cols if c != "Id"]
base_desc = wide_merged[["Id"] + desc_cols].drop_duplicates(subset=["Id"])

# ----- Aggregate: average over ALL scores for each Id -----
agg = long_all.groupby("Id")["Score"].agg(
    Count="count",
    Mean="mean",
    Min="min",
    Max="max",
    Std="std",
    Var="var",
).reset_index()

agg = agg.rename(columns={"Mean": "Score"})

# ----- Merge descriptors with aggregated results -----
final = pd.merge(base_desc, agg, on="Id", how="right").sort_values("Id")

# ----- Drop unwanted columns in the wide merged sheet -----
cols_to_drop = [c for c in DROP_COLS if c in wide_merged.columns]
if cols_to_drop:
    print("\nDropped columns from All Scores (Raw, Merged):")
    for c in cols_to_drop:
        print("  -", c)
    wide_merged = wide_merged.drop(columns=cols_to_drop)

# ----- Drop ALL comment-related columns from All Scores (Raw, Merged) -----
comment_cols = [c for c in wide_merged.columns if "comment" in c.lower()]
if comment_cols:
    print("\nDropping comment-related columns from All Scores (Raw, Merged):")
    for c in comment_cols:
        print("  -", c)
    wide_merged = wide_merged.drop(columns=comment_cols)

# ----- Simplify score column names -----
# Converts:
#   track_2_scores::bh.xlsx   ->   2:bh
#   track_5_scores::ab        ->   5:ab
#   anything_else::xx.xlsx    ->   <tag>:xx  (fallback)
simplified_cols = {}

for col in wide_merged.columns:
    if "::" not in col:
        continue

    file_tag, reviewer = col.split("::", 1)

    # Drop .xlsx from reviewer column name
    reviewer = reviewer.replace(".xlsx", "")

    # Extract track number from file tag
    track_num = None
    if "track_" in file_tag:
        try:
            # Example: track_2_scores -> "2"
            track_num = file_tag.split("track_")[1].split("_")[0]
        except:
            pass

    # Build new simplified name
    if track_num is not None:
        new_col = f"{track_num}:{reviewer}"
    else:
        # fallback (shouldn't happen, but safe)
        new_col = f"{file_tag}:{reviewer}"

    simplified_cols[col] = new_col

# Apply renaming
wide_merged = wide_merged.rename(columns=simplified_cols)

print("\nSimplified score column names:")
for old, new in simplified_cols.items():
    print(f"  {old} -> {new}")
    
# ----- Output filename -----
folder_name = os.path.basename(os.path.normpath(FOLDER))
#OUTPUT_FILE = os.path.join(FOLDER, f"{folder_name}_MERGED_by_Id_averaged.xlsx")
OUTPUT_FILE = f"{folder_name}_MERGED_by_Id_averaged.xlsx"

# ----- Write output workbook -----
with pd.ExcelWriter(OUTPUT_FILE, engine="xlsxwriter") as writer:
    # Final averaged scores per Id
    final.to_excel(writer, sheet_name="Merged Averaged by Id", index=False)

    # Wide merged view with all scores, some metadata dropped
    wide_merged.to_excel(writer, sheet_name="All Scores (Raw, Merged)", index=False)

print(f"\nDone. Wrote '{OUTPUT_FILE}' with:")
print(" - 'Merged Averaged by Id': average Score per Id across all files & columns")
print(" - 'All Scores (Raw, Merged)': wide table with all unique score columns")
