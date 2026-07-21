# dataWork.py

This script analyzes XML cell-layout snapshots and extracts per-cell geometric metrics such as cell center, length, width, and long-axis orientation.

## What it does

- Parses XML files matching the pattern `leaf.*.xml`
- Reads node coordinates and cell node references
- Computes:
  - cell center via polygon centroid
  - approximate cell length and width
  - long-axis vector
- Collects metrics across all timesteps
- Produces a plot of cell length and width over time

## Expected input

The script looks for XML files in the same `Data` directory

It expects files named like:

- `leaf.0.xml`
- `leaf.1.xml`
- `leaf.2.xml`

## Dependencies

Install the required Python packages if needed:

```bash
pip install matplotlib
```

## Run

From the script directory:

```bash
python dataWork.py \outputPath
```

## Output

The script prints a summary of the first few timestep files and saves a plot to the \outputPath:

- `cell_length_width_evolution_per_cell.png`


## Main functions

- `load_root(xml_path)`
- `calc_cell_center(node_points)`
- `parse_cell_metrics(xml_path)`
- `calc_length_and_width(node_points)`
- `collect_time_evolution(data_dir=DATA_DIR)`
- `plot_time_evolution(evolution, output_dir=DATA_DIR)`
