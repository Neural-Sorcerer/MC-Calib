#!/usr/bin/env python3
"""
Script to apply a black mask to images in a folder.

This script loads all images from ./data/DeltaX/Cam_002/,
adds a black rectangle with coordinates (700, 150) to (1320, 530),
and saves the modified images to ./data/DeltaX/Cam_002_mask/ in PNG format.
"""
import os
import cv2
import tqdm
import argparse
import numpy as np

from pathlib import Path


def apply_mask_to_images(input_dir, output_dir, x1=700, y1=150, x2=1320, y2=530):
    """
    Apply a black mask to all images in the input directory and save to output directory.
    
    Args:
        input_dir (str): Path to the input directory containing images
        output_dir (str): Path to the output directory where masked images will be saved
        x1, y1 (int): Top-left corner coordinates of the rectangle
        x2, y2 (int): Bottom-right corner coordinates of the rectangle
    """
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Get all image files in the input directory
    image_extensions = ['.jpg', '.jpeg', '.png', '.bmp', '.tif', '.tiff']
    image_files = []
    
    for ext in image_extensions:
        image_files.extend(list(Path(input_dir).glob(f'*{ext}')))
        image_files.extend(list(Path(input_dir).glob(f'*{ext.upper()}')))
    
    if not image_files:
        print(f"No image files found in {input_dir}")
        return
    
    print(f"Found {len(image_files)} images in {input_dir}")
    
    # Process each image
    for img_path in tqdm.tqdm(image_files, desc="Processing images"):
        # Read the image
        img = cv2.imread(str(img_path))
        
        if img is None:
            print(f"Could not read image: {img_path}")
            continue
        
        # Apply the black rectangle mask
        cv2.rectangle(img, (x1, y1), (x2, y2), (0, 0, 0), -1)
        
        # Create output filename with .png extension
        output_filename = Path(output_dir) / f"{img_path.stem}.png"
        
        # Save the masked image
        cv2.imwrite(str(output_filename), img)
    
    print(f"Processed images saved to {output_dir}")


def main():
    parser = argparse.ArgumentParser(description='Apply black mask to images')
    parser.add_argument('--input', type=str, default='./data/DeltaX/Cam_002',
                        help='Input directory containing images (default: ./data/DeltaX/Cam_002)')
    parser.add_argument('--output', type=str, default='./data/DeltaX/Cam_002_mask',
                        help='Output directory for masked images (default: ./data/DeltaX/Cam_002_mask)')
    parser.add_argument('--x1', type=int, default=700, help='X-coordinate of top-left corner (default: 700)')
    parser.add_argument('--y1', type=int, default=150, help='Y-coordinate of top-left corner (default: 150)')
    parser.add_argument('--x2', type=int, default=1320, help='X-coordinate of bottom-right corner (default: 1320)')
    parser.add_argument('--y2', type=int, default=530, help='Y-coordinate of bottom-right corner (default: 530)')
    
    args = parser.parse_args()
    
    # Print the parameters
    print(f"Applying mask with coordinates: ({args.x1}, {args.y1}) to ({args.x2}, {args.y2})")
    print(f"Input directory: {args.input}")
    print(f"Output directory: {args.output}")
    
    # Apply mask to images
    apply_mask_to_images(args.input, args.output, args.x1, args.y1, args.x2, args.y2)


if __name__ == "__main__":
    main()
