import cv2
# import numpy as np


# Run image through a Gaussian filter and then convolve the filtered image with the original
# to create an unsharp mask filter
def sharp_image(img):
    gaussian = cv2.GaussianBlur(img, (0, 0), 2.0)
    sharpened = cv2.addWeighted(img, 7.0, gaussian, -6, 0)

    return sharpened


# Adjust HSV values for the desired effect
def adjust_saturation(img):
    (h, s, v) = cv2.split(img)
    s = s * 2
    v = v * 2
    h = h // 4
    resimage = cv2.merge([h, s, v])
    return resimage


# create dank meme effect image
def deep_fry(imgname):
    image = cv2.imread(imgname)
    sharper = sharp_image(image)
    fried = adjust_saturation(sharper)
    return fried