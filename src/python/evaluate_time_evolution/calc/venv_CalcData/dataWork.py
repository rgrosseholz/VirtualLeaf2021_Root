import argparse
import math
import re
import sys
from pathlib import Path

import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET

DEFAULT_DATA_DIR = Path("/home/lasse/lateral_root")
XML_PATTERN = 'leaf.*.xml'
TIMESTEP_RE = re.compile(r'\.(\d+)$')
LENGTH_HISTOGRAM_BIN_WIDTH = 1
WIDTH_HISTOGRAM_BIN_WIDTH = 2

def load_root(xml_path):
    tree = ET.parse(xml_path)
    return tree.getroot()

def calc_cell_center(node_points):
    """Compute the polygon centroid (cell center) from the node coordinates."""
    if len(node_points) < 3:
        raise ValueError('At least three nodes are required to compute the cell center.')

    area2 = 0.0
    cx = 0.0
    cy = 0.0

    for i, (x_i, y_i) in enumerate(node_points):
        x_j, y_j = node_points[(i + 1) % len(node_points)]
        cross = x_i * y_j - x_j * y_i
        area2 += cross
        cx += (x_i + x_j) * cross
        cy += (y_i + y_j) * cross

    area = abs(area2) / 2.0
    if abs(area) < 1e-12:
        raise ValueError('Cell area is zero, cannot compute the center.')

    center_x = cx / (6.0 * area)
    center_y = cy / (6.0 * area)
    return center_x, center_y


def parse_cell_metrics(xml_path):
    root = load_root(xml_path)
    node_coords = {}
    for index, node in enumerate(root.findall('.//nodes/node')):
        node_coords[index] = (float(node.get('x', 0.0)), float(node.get('y', 0.0)))

    cell_metrics = []
    for cell in root.findall('.//cells/cell'):
        cell_index = int(cell.get('index', -1))
        cell_type = int(cell.get('cell_type', -1))
        point_list = []
        for node in cell.findall('node'):
            node_number = int(node.get('n', -1))
            x, y = node_coords.get(node_number, (None, None))
            point_list.append((x, y))

        center = calc_cell_center(point_list)
        length, width, long_axis = calc_length_and_width(point_list)
        length2 = calc_length2(point_list)
        cell_metrics.append({
            'cell_index': cell_index,
            'cell_type': cell_type,
            'center': center,
            'length': length,
            'length2': length2,
            'width': width,
            'long_axis': long_axis,
        })

    return cell_metrics


def calc_length2(node_points):
    """Calculate an alternate length using the average of the two highest and two lowest nodes."""
    if len(node_points) < 2:
        raise ValueError('At least two nodes are required to compute length2.')

    sorted_by_y = sorted(node_points, key=lambda p: p[1])
    lowest = sorted_by_y[:2]
    highest = sorted_by_y[-2:]

    avg_low_x = sum(x for x, _ in lowest) / len(lowest)
    avg_low_y = sum(y for _, y in lowest) / len(lowest)
    avg_high_x = sum(x for x, _ in highest) / len(highest)
    avg_high_y = sum(y for _, y in highest) / len(highest)

    return math.hypot(avg_high_x - avg_low_x, avg_high_y - avg_low_y)


def calc_length_and_width(node_points):
    """Approximate the C++ CellBase::CalcLength routine in Python."""
    if len(node_points) < 3:
        raise ValueError('At least three nodes are required to compute cell length and width.')

    my_intgrl_xx = 0.0
    my_intgrl_xy = 0.0
    my_intgrl_yy = 0.0
    my_intgrl_x = 0.0
    my_intgrl_y = 0.0
    my_area = 0.0

    for i, point_i in enumerate(node_points):
        j = (i + 1) % len(node_points)
        point_j = node_points[j]

        x_i, y_i = point_i
        x_j, y_j = point_j

        cross = x_i * y_j - x_j * y_i
        my_area += cross

        my_intgrl_xx += ((x_i * x_i + x_j * x_i + x_j * x_j) * cross)
        my_intgrl_xy += ((x_j * y_i - x_i * y_j) * (x_i * (2 * y_i + y_j) + x_j * (y_i + 2 * y_j)))
        my_intgrl_yy += (cross * (y_i * y_i + y_j * y_i + y_j * y_j))
        my_intgrl_x += ((x_j + x_i) * cross)
        my_intgrl_y += ((y_j + y_i) * cross)

    my_area = abs(my_area) / 2.0
    if my_area == 0.0:
        raise ValueError('Cell area is zero, cannot compute length and width.')

    intrx = my_intgrl_x / 6.0
    intry = my_intgrl_y / 6.0
    ixx = my_intgrl_xx / 12.0 - (intrx * intrx) / my_area
    ixy = my_intgrl_xy / 24.0 + (intrx * intry) / my_area
    iyy = my_intgrl_yy / 12.0 - (intry * intry) / my_area

    rhs1 = (ixx + iyy) / 2.0
    rhs2 = math.sqrt((ixx - iyy) * (ixx - iyy) + 4.0 * ixy * ixy) / 2.0
    lambda_b = rhs1 + rhs2
    if lambda_b < 0 :
        length = 0 
    else :
        length = 4.0 * math.sqrt(lambda_b / my_area)
    if (rhs1 - rhs2) < 0 :
        width = 0
    else :
        width = 4.0 * math.sqrt((rhs1 - rhs2) / my_area)

    long_axis = (-ixy, lambda_b - ixx, 0.0)

    return length, width, long_axis


def _timestep_from_path(xml_path):
    stem = xml_path.stem
    match = TIMESTEP_RE.search(stem)
    if not match:
        raise ValueError(f"Unable to parse timestep from filename: {xml_path.name}")
    return int(match.group(1))


def collect_time_evolution(data_dir=DEFAULT_DATA_DIR): 
    xml_files = sorted(data_dir.glob(XML_PATTERN), key=_timestep_from_path)
    evolution = []

    for xml_path in xml_files:
        timestep = _timestep_from_path(xml_path)
        metrics = parse_cell_metrics(xml_path)
        evolution.append((timestep, metrics))

    return evolution


def compute_type_average_metrics(evolution):
    cell_types = sorted({item['cell_type'] for _, metrics in evolution for item in metrics})
    averages = {}

    for cell_type in cell_types:
        avg_length_series = []
        avg_width_series = []

        for _, metrics in evolution:
            type_cells = [item for item in metrics if item['cell_type'] == cell_type]
            if not type_cells:
                avg_length_series.append(float('nan'))
                avg_width_series.append(float('nan'))
                continue

            avg_length_series.append(sum(item['length2'] for item in type_cells) / len(type_cells))
            avg_width_series.append(sum(item['width'] for item in type_cells) / len(type_cells))

        averages[cell_type] = {
            'avg_length': avg_length_series,
            'avg_width': avg_width_series,
        }

    return averages


def _build_bin_edges(values, bin_width):
    if not values:
        return None
    if bin_width <= 0:
        raise ValueError('Bin width must be greater than zero.')

    min_value = min(values)
    max_value = max(values)
    start = math.floor(min_value / bin_width) * bin_width
    end = math.ceil(max_value / bin_width) * bin_width

    edges = []
    current = start
    while current <= end + 1e-12:
        edges.append(current)
        current += bin_width

    return edges


def plot_histograms(evolution, output_dir=DEFAULT_DATA_DIR, length_bin_width=LENGTH_HISTOGRAM_BIN_WIDTH, width_bin_width=WIDTH_HISTOGRAM_BIN_WIDTH):
    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    if not evolution:
        return []

    selected_timesteps = [evolution[0], evolution[-1]]
    plot_paths = []
    cell_types = sorted({item['cell_type'] for _, metrics in evolution for item in metrics})

    for timestep, metrics in selected_timesteps:
        for cell_type in cell_types:
            type_metrics = [item for item in metrics if item['cell_type'] == cell_type]
            lengths = [item['length2'] for item in type_metrics]
            widths = [item['width'] for item in type_metrics]

            if lengths:
                length_edges = _build_bin_edges(lengths, length_bin_width)
                fig, ax = plt.subplots(figsize=(8, 5))
                print(f"hist timestep={timestep} type={cell_type} n={len(lengths)} min={min(lengths):.3f} max={max(lengths):.3f}")
                print(f"  edges={length_edges[:3]} ... {length_edges[-3:]}")
                ax.hist(lengths, bins=length_edges, color='steelblue', edgecolor='black')
                ax.set_title(f'Cell length distribution at timestep {timestep} for cell type {cell_type}')
                ax.set_xlabel('Length')
                ax.set_ylabel('Count')
                ax.grid(True, alpha=0.3)
                fig.tight_layout()
                out_path = output_dir / f'length_histogram_type_{cell_type}_time_{timestep}.png'
                fig.savefig(out_path, dpi=150)
                plt.close(fig)
                plot_paths.append(out_path)

#            if widths:
#                width_edges = _build_bin_edges(widths, width_bin_width)
#                fig, ax = plt.subplots(figsize=(8, 5))
#                ax.hist(widths, bins=width_edges, color='lightcoral', edgecolor='black')
#                ax.set_title(f'Cell width distribution at timestep {timestep} for cell type {cell_type}')
#                ax.set_xlabel('Width')
#                ax.set_ylabel('Count')
#                ax.grid(True, alpha=0.3)
#                fig.tight_layout()
#                out_path = output_dir / f'width_histogram_type_{cell_type}_time_{timestep}.png'
#                fig.savefig(out_path, dpi=150)
#                plt.close(fig)
#                plot_paths.append(out_path)

    return plot_paths


def plot_time_evolution(evolution, output_dir=DEFAULT_DATA_DIR):
    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    times = [t for t, _ in evolution]
    cell_types = sorted({item['cell_type'] for _, metrics in evolution for item in metrics})
    plot_paths = []
    type_averages = compute_type_average_metrics(evolution)

    for cell_type in cell_types:
        fig, ax = plt.subplots(2, 1, figsize=(12, 9), sharex=True)
        ax[0].set_title(f'Cell length over time for cell type {cell_type}')
        ax[1].set_title(f'Cell width over time for cell type {cell_type}')

        cell_ids = sorted({
            item['cell_index']
            for _, metrics in evolution
            for item in metrics
            if item['cell_type'] == cell_type
        })

        for cell_id in cell_ids:
            length_series = [
                next((item['length2'] for item in metrics if item['cell_index'] == cell_id and item['cell_type'] == cell_type), float('nan'))
                for _, metrics in evolution
            ]
            width_series = [
                next((item['width'] for item in metrics if item['cell_index'] == cell_id and item['cell_type'] == cell_type), float('nan'))
                for _, metrics in evolution
            ]

            ax[0].plot(times, length_series, marker='o', linewidth=1.5, label=f'Cell {cell_id}')
            ax[1].plot(times, width_series, marker='s', linewidth=1.5, label=f'Cell {cell_id}')

        avg_length = type_averages[cell_type]['avg_length']
        avg_width = type_averages[cell_type]['avg_width']
        ax[0].plot(times, avg_length, color='black', linestyle='--', linewidth=2.5, label='Average length')
        ax[1].plot(times, avg_width, color='black', linestyle='--', linewidth=2.5, label='Average width')

        ax[0].set_ylabel('Length')
        ax[0].grid(True, alpha=0.3)
        ax[0].legend(loc='best', fontsize=8)

        ax[1].set_xlabel('Timestep')
        ax[1].set_ylabel('Width')
        ax[1].grid(True, alpha=0.3)
        ax[1].legend(loc='best', fontsize=8)

        fig.tight_layout()
        out_path = output_dir / f'cell_length_width_evolution_type_{cell_type}.png'
        fig.savefig(out_path, dpi=150)
        plt.close(fig)
        plot_paths.append(out_path)

    return plot_paths


def plot_length_by_center(evolution, output_dir=DEFAULT_DATA_DIR):
    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    if not evolution:
        return []

    selected_timesteps = [evolution[0]]
    if len(evolution) > 1:
        selected_timesteps.append(evolution[-1])

    plot_paths = []
    cell_types = sorted({item['cell_type'] for _, metrics in evolution for item in metrics})

    for cell_type in cell_types:
        fig, ax = plt.subplots(figsize=(8, 5))
        for timestep, metrics in selected_timesteps:
            type_metrics = [item for item in metrics if item['cell_type'] == cell_type]
            centers = [item['center'][1] for item in type_metrics]
            lengths = [item['length'] for item in type_metrics]
            if centers:
                ax.scatter(centers, lengths, alpha=0.8, label=f'Timestep {timestep}')

        ax.set_title(f'Cell length by center coordinate for cell type {cell_type}')
        ax.set_xlabel('Cell center y coordinate')
        ax.set_ylabel('Cell length')
        ax.grid(True, alpha=0.3)
        ax.legend(loc='best')
        fig.tight_layout()
        out_path = output_dir / f'cell_length_by_center_type_{cell_type}.png'
        fig.savefig(out_path, dpi=150)
        plt.close(fig)
        plot_paths.append(out_path)

    return plot_paths


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Evaluate time evolution of leaf XML data.')
    parser.add_argument('data_dir', nargs='?', default=DEFAULT_DATA_DIR,
                        help='Directory containing XML timestep files')
    parser.add_argument('output_dir', nargs='?', default=Path(__file__).resolve().parent / 'output',
                        help='Directory to write generated plots')
    args = parser.parse_args()

    data_dir = Path(args.data_dir)
    output_dir = Path(args.output_dir)

    evolution = collect_time_evolution(data_dir=data_dir)
    print(f'Analyzed {len(evolution)} timestep files from {data_dir}')

    for timestep, metrics in evolution[:3]:
        print(f'Timestep {timestep}: cells={len(metrics)}')
        for item in metrics[:2]:
            print(
                f'  Cell {item["cell_index"]}: '
                f'center=({item["center"][0]:.3f}, {item["center"][1]:.3f}), '
                f'length={item["length"]:.3f}, '
                f'length2={item["length2"]:.3f}, '
                f'width={item["width"]:.3f}'
            )

    # time_plot_paths = plot_time_evolution(evolution, output_dir=output_dir)
    center_plot_paths = plot_length_by_center(evolution, output_dir=output_dir)
    hist_plot_paths = plot_histograms(
        evolution,
        output_dir=output_dir,
        length_bin_width=LENGTH_HISTOGRAM_BIN_WIDTH,
        width_bin_width=WIDTH_HISTOGRAM_BIN_WIDTH,
    )
    plot_paths = center_plot_paths + hist_plot_paths
    print(f'Wrote {len(plot_paths)} plot(s) to {output_dir}')
    for plot_path in plot_paths:
        print(f'  {plot_path}')

    type_averages = compute_type_average_metrics(evolution)
    for cell_type, averages in type_averages.items():
        avg_length = sum(averages['avg_length']) / len(averages['avg_length'])
        avg_width = sum(averages['avg_width']) / len(averages['avg_width'])
        print(f'Cell type {cell_type}: average length={avg_length:.3f}, average width={avg_width:.3f}')
