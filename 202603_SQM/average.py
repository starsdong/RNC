import pandas as pd
import glob
import os
import sys

# Columns to drop in the All Scores (Raw) sheet
DROP_COLS = [
    "Speakers (affiliation)",
    "Speakers (email)",
    "Submitted for tracks",
    "Comment",
    "Comments"
]

# ----- Read folder from command-line argument -----
if len(sys.argv) < 2:
    print("Usage: python3 average.py <folder_path>")
    sys.exit(1)

FOLDER = sys.argv[1]

if not os.path.isdir(FOLDER):
    raise NotADirectoryError(f"Not a valid folder: {FOLDER}")

# ----- Auto-generate output filename -----
folder_name = os.path.basename(os.path.normpath(FOLDER))

# Ensure output directory exists
OUTPUT_DIR = "output"
os.makedirs(OUTPUT_DIR, exist_ok=True)

OUTPUT_FILE = os.path.join(OUTPUT_DIR, f"{folder_name}_scores.xlsx")

# ----- Configuration -----
SHEET_NAME = 0
SCORE_COL = "Score"
# -----------------------------------------

files = glob.glob(os.path.join(FOLDER, "*.xlsx"))
if not files:
    raise FileNotFoundError(f"No .xlsx files found in {FOLDER}")

dfs = []
common_cols = None

# ---- Load & determine common non-score columns ----
for f in files:
    df = pd.read_excel(f, sheet_name=SHEET_NAME)
    if SCORE_COL not in df.columns:
        raise KeyError(f"'{SCORE_COL}' not found in {os.path.basename(f)}. Columns: {list(df.columns)}")

    df[SCORE_COL] = pd.to_numeric(df[SCORE_COL], errors="coerce")

    non_score_cols = [c for c in df.columns if c != SCORE_COL]
    if common_cols is None:
        common_cols = set(non_score_cols)
    else:
        common_cols &= set(non_score_cols)

    dfs.append(df)

if not common_cols:
    raise ValueError("No common non-score columns found across files.")

first_df = dfs[0]
key_cols = [c for c in first_df.columns if c in common_cols]

# ---- Build wide sheet ----
wide = None
score_col_names = []

for f, df in zip(files, dfs):
    col_name = os.path.basename(f)
    score_col_names.append(col_name)

    piece = df[key_cols + [SCORE_COL]].copy()
    piece = piece.rename(columns={SCORE_COL: col_name})

    if wide is None:
        wide = piece
    else:
        wide = wide.merge(piece, on=key_cols, how="outer")

# Optional: sort per-file columns alphabetically
fixed_cols = key_cols + sorted(score_col_names, key=str.lower)
wide = wide[fixed_cols]

# ---- Drop specific unwanted columns from the wide sheet ----
#cols_to_drop = [c for c in DROP_COLS if c in wide.columns]
#if cols_to_drop:
#    print("Dropping columns from All Scores (Raw):")
#    for c in cols_to_drop:
#        print(f"  - {c}")
#    wide = wide.drop(columns=cols_to_drop)

# ---- Row-wise statistics ----
scores_only = wide[sorted([c for c in score_col_names if c in wide.columns], key=str.lower)]

row_count = scores_only.count(axis=1)
row_mean  = scores_only.mean(axis=1, skipna=True)
row_min   = scores_only.min(axis=1, skipna=True)
row_max   = scores_only.max(axis=1, skipna=True)
row_std   = scores_only.std(axis=1, ddof=1, skipna=True)
row_var   = scores_only.var(axis=1, ddof=1, skipna=True)

final = wide[key_cols].copy()
final[SCORE_COL] = row_mean
final["Count"]   = row_count
final["Min"]     = row_min
final["Max"]     = row_max
final["Std"]     = row_std
final["Var"]     = row_var

# ---- Write output ----
with pd.ExcelWriter(OUTPUT_FILE, engine="xlsxwriter") as writer:
    final.to_excel(writer, sheet_name="Averaged Scores", index=False)
    wide.to_excel(writer, sheet_name="All Scores (Raw)", index=False)

print(f"Done. Wrote '{OUTPUT_FILE}' with:")
print(" - 'Averaged Scores': mean Score + Count/Min/Max/Std/Var")
print(" - 'All Scores (Raw)': per-file score columns (with selected columns dropped)")
