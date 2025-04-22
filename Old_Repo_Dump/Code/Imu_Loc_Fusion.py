# -*- coding: utf-8 -*-
"""
Created on Mon Sep 11 15:07:03 2023

@author: Drogoth
"""

''' need to fuse imu data and location data
imu we get accel and gyro (accel and gyro displacment)
location we can feed from the dwm and is synchronus with imu data
'''

'''
calibrate imu rotation with taking magnitude and direction of 10 data points
use dwm to get location from start and final point, correct imu rotation with known locations of dwm points
can run all the time as no additional signalling
'''

'''
can apply a rotation matrix to the accel and gyro data to correct it
'''

'''
filter out obviously incorrect dwm ranges using accel data
ie if zero accel, dwm ranges will be fairly static
the filter strength will determine achieved accuracy
if outside of +-10mm then we eliminate 50% of values
'''

'''
if accel is low enough then find average posistion of multiple dwm ranges this should get closer to real value
should refine fast enough that any stationary movement refines to "good enough" accuracy

'''

'''
use refined value with new accel/dwm data to get movement
'''
