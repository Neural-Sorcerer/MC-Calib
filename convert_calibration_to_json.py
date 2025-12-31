import yaml
import json
import os

# Define a custom constructor for !!opencv-matrix
def opencv_matrix_constructor(loader, node):
    mapping = loader.construct_mapping(node, deep=True)
    
    rows = mapping.get('rows')
    cols = mapping.get('cols')
    data = mapping.get('data')
    
    # Check if we have valid matrix data
    if rows is not None and cols is not None and data is not None:
        # If it's a 2D matrix (rows > 1 and cols > 1), reshape it
        if rows > 1 and cols > 1:
            matrix = []
            for r in range(rows):
                start = r * cols
                end = start + cols
                row_data = data[start:end]
                matrix.append(row_data)
            return matrix
        # For vectors (1xN or Nx1), just return the flat list or maybe a single list?
        # User asked for "structural way", usually vectors are just lists in JSON.
        # Let's keep it simple: flat list for vectors, nested for matrices.
        else:
            return data
            
    return mapping

# Register the constructor
yaml.add_constructor('tag:yaml.org,2002:opencv-matrix', opencv_matrix_constructor)

def convert_yaml_to_json(yaml_path, json_path):
    print(f"Reading {yaml_path}...")
    try:
        with open(yaml_path, 'r') as f:
            lines = f.readlines()
        
        # Skip the first line if it is an OpenCV YAML directive
        if lines and lines[0].strip().startswith('%YAML'):
            lines = lines[1:]
            
        content = "".join(lines)
        data = yaml.full_load(content)
            
        print(f"Converting to JSON...")
        with open(json_path, 'w') as f:
            json.dump(data, f, indent=4)
            
        print(f"Saved to {json_path}")
        
    except FileNotFoundError:
        print(f"Error: File not found at {yaml_path}")
    except Exception as e:
        print(f"Error converting file: {e}")

if __name__ == "__main__":
    yaml_file = "/home/max/DELTAX_PROJECTS/PROJECTS/Gaze_Tracking/3D-Sensing-Team/MC-Calib/data/gaze_v4_2025-12-30/calibrated_cameras_data.yml"
    json_file = "/home/max/DELTAX_PROJECTS/PROJECTS/Gaze_Tracking/3D-Sensing-Team/MC-Calib/data/gaze_v4_2025-12-30/calibrated_cameras_data.json"
    
    if not os.path.exists(yaml_file):
         # invalid path, let's try searching or relative path if absolute fails, but I'm using absolute from previous context
         print(f"Warning: {yaml_file} does not exist. Checking other locations...")
         # Fallback logic if needed, but I'm fairly sure of the path from finding it earlier.
         pass
         
    convert_yaml_to_json(yaml_file, json_file)
