import os
import cv2
import numpy as np

# Get current directory 
current_directory = os.path.dirname(__file__)

# Load the image
file_name = 'Pyramid_img1.jpg'
file_path = os.path.join(current_directory, file_name)
image = cv2.imread(file_path)


# Convert to grayscale
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# Apply Gaussian blur
blurred = cv2.GaussianBlur(gray, (5, 5), 0)

# Edge detection using Canny
edges = cv2.Canny(blurred, 50, 150)

# Find contours
contours, _ = cv2.findContours(edges, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

# Loop over the contours
for contour in contours:
    # Approximate the contour
    epsilon = 0.04 * cv2.arcLength(contour, True)
    approx = cv2.approxPolyDP(contour, epsilon, True)

    # If the approximated contour has 3 vertices, it's a triangle
    if len(approx) == 3:
        # Draw the contour on the image
        cv2.drawContours(image, [approx], 0, (0, 255, 0), 3)

# Display the result
window1 = "Image with Triangle Detection"
cv2.namedWindow(window1, cv2.WINDOW_NORMAL)
cv2.resizeWindow(window1, 600, 600)
cv2.imshow(window1, image)
cv2.waitKey(0)
cv2.destroyAllWindows()
