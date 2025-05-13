# Rigidity Check Feature for McCalib

## Overview

This feature enhances the McCalib library by adding a rigidity check between Charuco boards. It prevents boards that are not physically connected (i.e., can move independently) from being merged into a single 3D object.

## Problem Addressed

In the original implementation, if two or more Charuco boards are visible in the same camera view, they are considered part of the same 3D object, even if they are not physically connected. This can lead to incorrect calibration results when boards can move independently, such as when one board is displayed on a screen and another is physically moved around.

## Solution

The rigidity check examines the relative transformation between pairs of boards across multiple observations. If the transformation is consistent (within a specified threshold), the boards are considered rigidly connected and can be merged into a single 3D object. Otherwise, they are treated as separate objects.

## Configuration Parameters

Two new parameters have been added to the configuration file:

1. `enable_rigidity_check` (boolean): Enables or disables the rigidity check feature.
   - Default: `true`
   - Set to `false` to revert to the original behavior.

2. `rigidity_threshold` (float): The threshold (in meters) for determining if boards are rigidly connected.
   - Default: `0.05` (5 centimeters)
   - Lower values make the check more strict, requiring more consistent relative positions.
   - Higher values are more lenient, allowing more variation in relative positions.

## How It Works

1. For each pair of boards that are seen together in multiple frames:
   - The relative transformation (rotation and translation) between the boards is calculated for each frame.
   - The variance of these transformations is computed.
   - If the variance is below the threshold, the boards are considered rigidly connected.

2. Only rigidly connected boards are merged into the same 3D object.

## Example Configuration

```yaml
# Rigidity check parameters
enable_rigidity_check: 1  # Enable rigidity check
rigidity_threshold: 0.05  # Threshold for rigidity check in meters
```

## Logging

The system logs information about the rigidity check:

- At startup, it logs whether the rigidity check is enabled and the threshold value.
- During the board graph initialization, it logs the variance of transformations for each board pair and whether they are considered rigid.
- When a board pair fails the rigidity check, it logs a message indicating that the boards are not rigidly connected.

## Use Cases

This feature is particularly useful in scenarios where:

1. Multiple Charuco boards are used, but not all are physically connected.
2. Some boards are displayed on screens or monitors while others are physical boards.
3. Different parts of a calibration setup can move independently.

By correctly identifying which boards are rigidly connected, the calibration results become more accurate and reliable.
