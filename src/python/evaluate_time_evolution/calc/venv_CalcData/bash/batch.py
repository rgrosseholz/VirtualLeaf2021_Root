#!/usr/bin/env python3
import subprocess
import shutil
import os
import xml.etree.ElementTree as ET

# Define the ranges for your parameters
params_f = [1.5,1.6,1.7]
params_c = [100,110,120,130]

for f in params_f:
    for c in params_c:
        # Load the template XML
        tree = ET.parse('/home/lasse/VirtualLeaf2021_Root/data/leaves/root3.xml')
        root = tree.getroot()

        # Update parameter 'f'
        elem_f = root.find(".//par[@name='f']")
        if elem_f is not None:
            elem_f.set('val', str(f))
        else:
            print(f"Warning: Parameter 'f' not found for f={f}, c={c}")

        # Update parameter 'g'
        elem_c = root.find(".//par[@name='c']")
        if elem_c is not None:
            elem_c.set('val', str(c))
        else:
            print(f"Warning: Parameter 'c' not found for f={f}, c={c}")

        # Create a unique filename for this specific combination
        # Example: root_f1.9_g0.5.xml
        filename = f'/home/lasse/VirtualLeaf2021_Root/data/batchJobs/root_f_{f}_c_{c}.xml'
        tree.write(filename)

        print(f"Running: f={f}, c={c} -> {filename}")
        
        # Execute the program
        subprocess.run(['/home/lasse/VirtualLeaf2021_Root/bin/VirtualLeaf', '-b', '-l', filename])
        job_folder = f'/home/lasse/VirtualLeaf2021_Root/data/batchJobs/run_f{f}_c{c}'
        
        shutil.copytree('/home/lasse/transRoot', job_folder, dirs_exist_ok=True)
        shutil.rmtree('/home/lasse/transRoot')
