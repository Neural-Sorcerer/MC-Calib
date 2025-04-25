# 📚 About MC-Calib

**MC-Calib** is a generic and robust calibration toolbox for multi-camera systems, described in the following paper:

> 📄 Paper: [MC-Calib: A Generic and Robust Calibration Toolbox for Multi-Camera Systems](docs/MC-Calib.pdf)

![MC-Calib](docs/illustration.png)

## MC-Calib Installation Guide

### 📦 Requirements

- C++17 compatible compiler
- Install [Docker](https://docs.docker.com/engine/install/) on your system
- Required libraries (included in Docker image):
  - [Ceres Solver](http://ceres-solver.org/)
  - [Boost](https://www.boost.org/)
  - [OpenCV](https://opencv.org/) — supported versions: `4.2.0`, `4.5.5`, `4.10.0`, `4.11.0`

> 🪟 **Windows users**: Please refer to the [Windows installation guide](/docs/Windows.md)

---

### 🐳 Docker Setup

#### 1. Pull Prebuilt Images

```bash
# Production environment
docker pull bailool/mc-calib-prod:opencv4110

# Development environment
docker pull bailool/mc-calib-dev:opencv4110
```

#### 2. Run the Container

> 💡 Replace `$(pwd)` with your actual project root if not already in the MC-Calib directory.

##### Production Environment

```bash
docker run -it --rm \
  --name mc-calib-prod \
  --hostname max \
  --workdir /home/MC-Calib \
  --volume="$(pwd):/home/MC-Calib" \
  bailool/mc-calib-prod:opencv4110
```

##### Development Environment (recommended)

```bash
docker run -it \
  --name mc-calib-dev \
  --hostname max \
  --workdir /home/MC-Calib \
  --volume="$(pwd):/home/MC-Calib" \
  bailool/mc-calib-dev:opencv4110
```

#### 3. Building the Project

Compile the source code inside the Docker container using CMake and Make:

```bash
# Step by step compilation
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Or use one-step compilation
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc)
```

#### 4. Quick Start

```bash
# 0. Load data from the link below and insert into the `data` directory
/home/MC-Calib/data/Real_Images/Seq04_Converging_vision/

# 1. Modify the config file or use a prebuilt one by this path:
/home/MC-Calib/configs/calib_param.yml

# 2. Generate your own ChArUco boards
./apps/create_charuco_boards/generate_charuco ../configs/calib_param.yml

# 3. Run the calibration
./apps/calibrate/calibrate ../configs/calib_param.yml

# 4. Run post-calibration analysis
python3 python_utils/post_calibration_analysis.py -d save_path_from_calib_param.yml
```

#### 5. Container Management (optional)

```bash
# Resume container session
docker start -ai mc-calib-dev

# Remove container completely
docker rm mc-calib-dev

# Optional: Add color to the terminal prompt inside the container
export PS1="\[\e[1;32m\]\u@\h:\w\\$ \[\e[0m\]"
```

## Repository Usage

### Gaze Data Collection Calibration Steps

1. **Generate ChArUco Boards**

   1.1. Define the board specifications in your configuration file (`calib_param.yml`):

   ```yaml
   number_board: 2
   number_x_square: 8
   number_y_square: 8
   ```

   1.2 Run the ChArUco board generator:

   ```bash
   ./apps/create_charuco_boards/generate_charuco ../configs/calib_param.yml
   ```

2. **Print ChArUco Boards**

   After printing, ensure the board is mounted flat and securely, without any bends, curves, or surface waves.  
   A perfectly flat board is essential for accurate corner detection and calibration results.

3. **Collect Data for Calibration**

   To use MC-Calib effectively, follow these guidelines when preparing your data:

   - **Synchronized Capture**:  
   All cameras must capture images at the **exact same time**. The toolbox assumes synchronized input from a multi-camera rig.

   - **Global Shutter Recommended**:  
   MC-Calib is designed and tested with **global shutter cameras**. Using **rolling shutter sensors** may lead to lower calibration accuracy due to motion distortion.

   - **Minimize Motion Blur**:  
   Ensure that motion blur is limited during capture. Clear, stable frames will significantly improve calibration quality.

   - **Folder Structure**:  
   Images from each camera should be stored in **separate folders** named with a common prefix and a 3-digit index (starting from `001`), for example `Cam_001`, `Cam_002`.

   - **Image Structure**:  
   Images at `Cam_001`, `Cam_002` should be stored with 6-digit index (starting from `000000`), for example `000000.png`, `000001.png`.

   ```text
   dataset/
      ├── Cam_001/
      │   ├── 000000.png
      │   └── 000001.png
      ├── Cam_002/
      │   ├── 000000.png
      ... └── 000001.png
   ```

4. **Set Up the Calibration Configuration File**

   Customize the configuration file located at: `configs/calib_param.yml`  
   Adjust its parameters to match your specific setup.

5. **Run the Calibration**

   Execute the calibration process using your configured parameters:

   ```bash
   ./apps/calibrate/calibrate ../configs/calib_param.yml
   ```

6. **Run Post-Calibration Analysis**

   After calibration, you can analyze results such as reprojection error and pose consistency:

   ```bash
   python3 python_utils/post_calibration_analysis.py -d save_path_from_calib_param.yml
   ```

### Configuration File Example

You can find premaded configuration files for multi-camera calibration in the `configs/*.yml` directory.

To get started quickly:

1. Choose the example that matches your setup.
2. Duplicate the file.
3. Edit it with your specific camera and board settings.

```bash
######################################## Boards Parameters ###################################################
number_x_square: 5         # number of squares in the X direction
number_y_square: 5         # number of squares the Y direction
resolution_x: 500          # horizontal resolution in pixel
resolution_y: 500          # vertical resolution in pixel
length_square: 0.04        # parameters on the marker (can be kept as it is)
length_marker: 0.03        # parameters on the marker (can be kept as it is)
number_board: 3            # number of boards used for calibration (for overlapping camera 1 is enough ...)
boards_index: []           # leave it empty [] if the board index are ranging from zero to number_board; example of usage boards_index: [5,10 <-- only two board with index 5/10
square_size: 0.192         # size of each square of the board in cm/mm/whatever you want

############# Boards Parameters for different board size (leave empty if all boards have the same size) #################
number_x_square_per_board: []
number_y_square_per_board: []
square_size_per_board: []

######################################## Camera Parameters ###################################################
distortion_model: 0         # 0:Brown (perspective) // 1: Kannala (fisheye)
distortion_per_camera: []   # specify the model per camera, #leave "distortion_per_camera" empty [] if they all follow the same model (make sure that the vector is as long as cameras nb)
number_camera: 2            # number of cameras in the rig to calibrate
refine_corner: 1            # activate or deactivate the corner refinement
min_perc_pts: 0.5           # min percentage of points visible to assume a good detection

cam_params_path: "None"     # file with cameras intrinsics to initialize the intrinsic, write "None" if no initialization available 
fix_intrinsic: 0            # if 1 then the intrinsic parameters will not be estimated nor refined (initial value needed)

######################################## Images Parameters ###################################################
root_path: "../data/Synthetic_calibration_image/Scenario_1/Images"
cam_prefix: "Cam_"
keypoints_path: "../data/Synthetic_calibration_image/Scenario_1/Images/detected_keypoints_data.yaml" # to save time on keypoint detection

######################################## Optimization Parameters #############################################
quaternion_averaging: 1     # use Quaternion Averaging or median for average rotation
ransac_threshold: 10        # RANSAC threshold in pixel (keep it high just to remove strong outliers)
number_iterations: 1000     # max number of iterations for the non linear refinement

######################################## Hand-eye method #############################################
he_approach: 0              # 0: bootstrapped he technique, 1: traditional he

######################################## Output Parameters ###################################################
save_path: "../data/Synthetic_calibration_image/Scenario_1/Images"
save_detection: 1
save_reprojection: 1
camera_board_poses_index: [-1,-1]  # [cameraID, boardID], -1 means all IDs
board_poses_file_name: "calibrated_boards_poses_data.yml"
camera_params_file_name: "calibrated_cameras_data.yml"
```

### 📤 Output Explanation

After running the calibration, the toolbox automatically generates six `*.yml` output files.</br>
Python scripts for reading and processing these files can be found in the `python_utils` directory.</br>
To help you understand these files, we provide an example using the results from calibrating a hybrid stereo-vision system.

- **Camera parameters:** `calibrated_cameras_data.yml`

   ```bash
   %YAML:1.0
   ---
   nb_camera: 2 
   camera_0: # all the calibration parameters (intrinsic/extrinsic) for the camera 0
      camera_matrix: !!opencv-matrix # 3x3 intrinsic matrix
         rows: 3
         cols: 3
         dt: d
         data: [ 6.9057886528642052e+02, 0., 6.5114341701043156e+02, 0.,
            6.8919862105007201e+02, 2.6741231181725999e+02, 0., 0., 1. ]
      distortion_vector: !!opencv-matrix # 1x5 distortion vector (Brown model here)
         rows: 1
         cols: 5
         dt: d
         data: [ -5.5592652556282401e-02, 1.2691061778374907e-01,
            -2.4976766901851363e-04, 1.1847248726536302e-03,
            -6.7785776099559991e-02 ]
      distortion_type: 0 # type of distortion model (0: perspective, 1: fisheye)
      camera_group: 0 #Camera group in which this camera belong, if the calibration has been sucessful, all camera should belong to the group 0
      img_width: 1280 #image size
      img_height: 512
      camera_pose_matrix: !!opencv-matrix
         rows: 4
         cols: 4
         dt: d
         data: [ 1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1., 0., 0., 0., 0.,
            1. ] #4x4 extrinsic matrix (for camera 0, it is the identity because it is the reference)
   camera_1: # all the calibration parameters (intrinsic/extrinsic) for the camera 1
      camera_matrix: !!opencv-matrix # 3x3 intrinsic matrix for camera 1
         rows: 3
         cols: 3
         dt: d
         data: [ 3.3467577884661034e+02, 0., 6.3270889699552083e+02, 0.,
            3.3407723815119016e+02, 2.6650518594457941e+02, 0., 0., 1. ]
      distortion_vector: !!opencv-matrix # 1x4 distortion vector (fisheye model)
         rows: 1
         cols: 4
         dt: d
         data: [ 1.1763357579105141e-02, -5.1797112353852174e-03,
            2.6315580610037459e-03, 0. ]
      distortion_type: 1 # type of distortion model (0: perspective, 1: fisheye)
      camera_group: 0
      img_width: 1280
      img_height: 512
      camera_pose_matrix: !!opencv-matrix #4x4 extrinsic matrix
         rows: 4
         cols: 4
         dt: d
         data: [ 9.9999074801577947e-01, 7.7896180494682642e-04,
            -4.2304965841050025e-03, 1.9839157514973714e+01,
            -7.9020195036245652e-04, 9.9999616084980592e-01,
            -2.6559116188004227e-03, 6.1882118248103253e-02,
            4.2284114888848610e-03, 2.6592299929997965e-03,
            9.9998752443824268e-01, 1.8600285922272908e+00, 0., 0., 0., 1. ]  #4x4 extrinsic matrix, expressed in camera 0 referencial
   ```

- **Object 3D structure:** `calibrated_objects_data.yml`

   ```bash
   %YAML:1.0
   ---
   object_0: #object index (if all boards have been seen, a single object should exist)
      points: !!opencv-matrix #3xn 3D structure of the object
         rows: 3
         cols: 16
         dt: f
         data: [ 0., 9.14999962e+00, 1.82999992e+01, 2.74499989e+01, 0.,
            9.14999962e+00, 1.82999992e+01, 2.74499989e+01, 0.,
            9.14999962e+00, 1.82999992e+01, 2.74499989e+01, 0.,
            9.14999962e+00, 1.82999992e+01, 2.74499989e+01, 0., 0., 0., 0.,
            9.14999962e+00, 9.14999962e+00, 9.14999962e+00, 9.14999962e+00,
            1.82999992e+01, 1.82999992e+01, 1.82999992e+01, 1.82999992e+01,
            2.74499989e+01, 2.74499989e+01, 2.74499989e+01, 2.74499989e+01,
            0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. ]
   ```

- **Object's poses:** `calibrated_objects_pose_data.yml`

  The file provides the object's pose relative to the reference camera for each frame, where calibration boards are detected.</br>
  The data is stored as a 6×N array, where each row represents:

  - First 3 values: **Rotation** in Rodrigues angle-axis format (X, Y, Z)
  - Next 3 values: **Translation vector** (X, Y, Z)

- **Reprojection error log:** `reprojection_error_data.yml`

  This file contains the reprojection error for each detected corner, organized by camera and frame.

- **Pre-detected board corners (Input):** `detected_keypoints_data.yml`

  This file stores keypoints detected from calibration boards in advance, allowing you to reuse them across multiple calibrations without repeating the detection step.

  - **Purpose**:  
    - Speeds up calibration by skipping keypoint detection from calibration boards.

  - **Usage**:
    - Automatically generated on the **first run** if it doesn't exist.
    - Referenced using the `keypoints_path` field in the configuration file.
    - Useful when running **multiple calibrations on the same data set** to save time.

- **Board Poses (Output)** `calibrated_boards_poses_data.yml`

  This is an output file that contains the poses of the calibration boards.

  - **Purpose**:
    - Stores the estimated pose of each calibration board relative to the cameras.
    - Utilize for TV’s display‐board pose calibration.

  - **Format**:
    - Each pose is a 6-element vector:
      - First 3 values: **Rotation** in Rodrigues angle-axis format.
      - Last 3 values: **Translation vector**.
    - Organized by **camera ID**, **board ID**, and **frame ID**.

  - **Usage**:
    - Controlled by the `board_poses_file_name` field in the configuration file.
    - If set to `"None"` or left empty, this file will **not** be generated.
    - To save specific board poses use filter `camera_board_poses_index: [cameraID, boardID]` with options:
      - `[]` or `[-1,-1]` to save all poses
      - `[2,1]` to save poses from `cameraID=2`, `boardID=1`
      - `[1,-1]` to save poses from `cameraID=1`, where `-1` show that for all boardIDs.

## Datasets

The synthetic and real datasets acquired for this paper are freely available via the following links:

- [Real Data](https://drive.google.com/file/d/143jdSi5fxUGj1iEGbTIQPfSqcOyuW-MR/view?usp=sharing)
- [Synthetic Data](https://drive.google.com/file/d/1CxaXUbO4E9WmaVrYy5aMeRLKmrFB_ARl/view?usp=sharing)

Documentation is available [online](https://codedocs.xyz/rameau-fr/MC-Calib/).</br>
To generate local documentation, follow [the instructions](/docs/Documentation.md).

## Contribution

Please follow `docs/contributing.rst` when introducing changes.

## Citation

If you use this project in your research, please cite:

```text
@article{RAMEAU2022103353,
title = {MC-Calib: A generic and robust calibration toolbox for multi-camera systems},
journal = {Computer Vision and Image Understanding},
pages = {103353},
year = {2022},
issn = {1077-3142},
doi = {https://doi.org/10.1016/j.cviu.2021.103353},
url = {https://www.sciencedirect.com/science/article/pii/S1077314221001818},
author = {Francois Rameau and Jinsun Park and Oleksandr Bailo and In So Kweon},
keywords = {Camera calibration, Multi-camera system},
}
```
