#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
  evaluate_time_evolution_wrapper.sh <current_xml_path>

  example:
  ./src/bash/evaluate_time_evolution_wrapper.sh \
      /path/to/leaf.007000.xml

Arguments:
  current_xml_path      Path to the XML file for the current iteration

The script will:
  1. determine the next numbered iteration from the highest existing iteration_* directory
  2. create a numbered output folder under the repository root
  3. compare current_xml_path with the matching file from the previous iteration directory
  4. stage the current XML snapshot and latest PDF and XML into the evaluator data directory
  5. run the time-evolution Python evaluator to generate plots
EOF
}

if [[ $# -ne 1 ]]; then
    usage
    exit 1
fi


CURRENT_XML="$1"

if [[ ! -f "$CURRENT_XML" ]]; then
    echo "Current XML file not found: $CURRENT_XML" >&2
    exit 1
fi


PROJECT_ROOT="$(cd "/home/lasse/VirtualLeaf2021_Root" && pwd)"
EVAL_SCRIPT="$PROJECT_ROOT/src/python/evaluate_time_evolution/Data/venv_CalcData/dataWork.py"

shopt -s nullglob
iteration_dirs=("$PROJECT_ROOT/src/python/evaluate_time_evolution/Data"/iteration_*)
MAX_ITERATION=-1
for iteration_dir in "${iteration_dirs[@]}"; do
    iteration_name=$(basename "$iteration_dir")
    if [[ "$iteration_name" =~ ^iteration_([0-9]+)$ ]]; then
        iteration_num=${BASH_REMATCH[1]}
        if (( iteration_num > MAX_ITERATION )); then
            MAX_ITERATION=$iteration_num
        fi
    fi
done

if (( MAX_ITERATION >= 0 )); then
    ITERATION=$((MAX_ITERATION + 1))
    PREV_ITERATION=$MAX_ITERATION
else
    ITERATION=0
    PREV_ITERATION=-1
fi

RUN_DIR="$PROJECT_ROOT/src/python/evaluate_time_evolution/Data/iteration_${ITERATION}"
PREVIOUS_DIR="$PROJECT_ROOT/src/python/evaluate_time_evolution/Data/iteration_${PREV_ITERATION}"
EVAL_DATA_DIR="$RUN_DIR/data"

if [[ -d "$PREVIOUS_DIR" && -d "$PREVIOUS_DIR/data" ]]; then
    prev_xmls=("$PREVIOUS_DIR/data"/*.xml)
    if (( ${#prev_xmls[@]} > 0 )); then
        PREVIOUS_XML=$(printf '%s\n' "${prev_xmls[@]}" | sort | tail -n 1)
    else
        PREVIOUS_XML=""
    fi
else
    PREVIOUS_XML=""
fi

DIFF_FILE="$RUN_DIR/diff_${ITERATION}.txt"
DATA_DIR="/home/lasse/lateral_root"

mkdir -p "$RUN_DIR"


if [[ -f "$PREVIOUS_XML" ]]; then
    if command -v xmldiff >/dev/null 2>&1; then
        xmldiff "$PREVIOUS_XML" "$CURRENT_XML" > "$DIFF_FILE" || true
    else
        diff -u "$PREVIOUS_XML" "$CURRENT_XML" > "$DIFF_FILE" || true
    fi
else
    {
        echo "Previous XML snapshot was not found: $PREVIOUS_XML"
        echo "A diff could not be generated."
    } > "$DIFF_FILE"
fi

mkdir -p "$EVAL_DATA_DIR"

STAGED_NAME="leaf.$(printf '%06d' "$ITERATION").xml"
cp "$CURRENT_XML" "$EVAL_DATA_DIR/$STAGED_NAME"

NEWEST_PDF=$(find "$DATA_DIR" -maxdepth 1 -name "leaf.*.pdf" -printf '%T@ %p\n' | sort -nr | cut -d' ' -f2- | head -n1)
if [[ -n "$NEWEST_PDF" ]]; then
    cp "$NEWEST_PDF" "$EVAL_DATA_DIR/"
fi

NEWEST_XML=$(find "$DATA_DIR" -maxdepth 1 -name "leaf.*.xml" -printf '%T@ %p\n' | sort -nr | cut -d' ' -f2- | head -n1)
if [[ -n "$NEWEST_XML" ]]; then
    cp "$NEWEST_XML" "$EVAL_DATA_DIR/"
fi

DEFAULT_PYTHON_BIN="$PROJECT_ROOT/src/python/evaluate_time_evolution/Data/venv_CalcData/.venv_test/bin/python"
if [[ -x "$DEFAULT_PYTHON_BIN" ]]; then
    PYTHON_BIN="${PYTHON_BIN:-$DEFAULT_PYTHON_BIN}"
else
    PYTHON_BIN="${PYTHON_BIN:-python3}"
fi

if ! command -v "$PYTHON_BIN" >/dev/null 2>&1; then
    echo "Python interpreter not found: $PYTHON_BIN" >&2
    exit 1
fi

if ! "$PYTHON_BIN" -c 'import matplotlib' >/dev/null 2>&1; then
    echo "The Python evaluator requires matplotlib, but it is not available in the active $PYTHON_BIN environment." >&2
    exit 1
fi

"$PYTHON_BIN" "$EVAL_SCRIPT" "$DATA_DIR" "$EVAL_DATA_DIR"

cp -r "$DATA_DIR" /home/lasse/wurzel
rm "$DATA_DIR"/*.xml
rm "$DATA_DIR"/*."pdf"


printf '\nDone.\n'
printf 'Run directory: %s\n' "$RUN_DIR"
printf 'Diff file: %s\n' "$DIFF_FILE"
printf 'Evaluator data directory: %s\n' "$EVAL_DATA_DIR"
