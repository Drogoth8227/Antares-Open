import numpy as np
import sympy as sym
import random

Anchor0_Pos = [3,0,3]   #### Primary anchor
Anchor1_Pos = [-3,3,3]  #### Aux 1
Anchor2_Pos = [3,3,-3]  #### Aux 2
Anchor3_Pos = [-3,0,-3] #### Aux 3
Anchors_Pos = list(np.array([Anchor0_Pos, Anchor1_Pos, Anchor2_Pos, Anchor3_Pos]))

Tag_Start_Pos = [0,0,0]

#### Inputs
#### TOF from anchor - gives sphere around anchor
#### TDOA between 2 anchors - gives hyperboloid surface

#### subtract one sphere from another gives plane circle is on
#### use eq: (plane vector/(x^2+y^2+z^2)^0.5) = distance of plane from x,y,z point
#### radius of circle = (TOF distance^2-distance between plane and point^2)^0.5

#### Use intersection of planes, less compute vs points + can intersect with hyperbolic surfaces from tdoa


def distancefrom(first_point, second_point):    #### test module
    return numpy.sqrt(((first_point[0]-second_point[0])**2)+((first_point[1]-second_point[1])**2)+((first_point[2]-second_point[2])**2))

def FindRanges(Tag_Pos, Variation=0):             #### test module
    Anchor0_Range = (distancefrom(Anchor0_Pos, Tag_Pos)+(0.001*np.random.normal(0,15)))
    Anchor1_Range = (distancefrom(Anchor1_Pos, Tag_Pos)+(0.001*np.random.normal(0,15)))
    Anchor2_Range = (distancefrom(Anchor2_Pos, Tag_Pos)+(0.001*np.random.normal(0,15)))
    Anchor3_Range = (distancefrom(Anchor3_Pos, Tag_Pos)+(0.001*np.random.normal(0,15)))
    return Anchor0_Range, Anchor1_Range, Anchor2_Range, Anchor3_Range

def FindRangesRand(Tag_Pos, Variation):             #### test module
    Anchor0_Range = (distancefrom(Anchor0_Pos, Tag_Pos)+(0.001*random.randrange(-Variation,Variation)))
    Anchor1_Range = (distancefrom(Anchor1_Pos, Tag_Pos)+(0.001*random.randrange(-Variation,Variation)))
    Anchor2_Range = (distancefrom(Anchor2_Pos, Tag_Pos)+(0.001*random.randrange(-Variation,Variation)))
    Anchor3_Range = (distancefrom(Anchor3_Pos, Tag_Pos)+(0.001*random.randrange(-Variation,Variation)))
    return Anchor0_Range, Anchor1_Range, Anchor2_Range, Anchor3_Range

def AvgPos(Tag_Pos, Variation):                     #### test module
    Avg0 = ((FindCramersPoint(Tag_Pos,Variation)[0][0]+FindCramersPoint(Tag_Pos,Variation)[1][0]+FindCramersPoint(Tag_Pos,Variation)[2][0])/3)
    Avg1 = ((FindCramersPoint(Tag_Pos,Variation)[0][1]+FindCramersPoint(Tag_Pos,Variation)[1][1]+FindCramersPoint(Tag_Pos,Variation)[2][1])/3)
    Avg2 = ((FindCramersPoint(Tag_Pos,Variation)[0][2]+FindCramersPoint(Tag_Pos,Variation)[1][2]+FindCramersPoint(Tag_Pos,Variation)[2][2])/3)
    return Avg0,Avg1,Avg2

def FindAvgErr(Tag_Pos, Variation, Repeats):        #### test module
    ErrorTable = []
    ErrorRanges = []
    for i in range(Repeats):
        ErrorTable.append((AvgPos(Tag_Pos, Variation)[0]-Tag_Pos[0],AvgPos(Tag_Pos, Variation)[1]-Tag_Pos[1],AvgPos(Tag_Pos, Variation)[2]-Tag_Pos[2]))
        ErrorRanges.append(distancefrom(AvgPos(Tag_Pos, Variation),Tag_Pos))
    TotalErr = (sum(ErrorRanges)/len(ErrorRanges))
    ####return ErrorTable, ErrorRanges, TotalErr
    return TotalErr
        

def FindCramersPoint(Point, Variation):         #### test module
    Ranges = FindRangesRand(Point,Variation)
    Plane01 = FindPlane(Anchor0_Pos,Anchor1_Pos,Ranges[0],Ranges[1])
    Plane02 = FindPlane(Anchor0_Pos,Anchor2_Pos,Ranges[0],Ranges[2])
    Plane03 = FindPlane(Anchor0_Pos,Anchor3_Pos,Ranges[0],Ranges[3])
    
    Plane12 = FindPlane(Anchor1_Pos,Anchor2_Pos,Ranges[1],Ranges[2])
    Plane13 = FindPlane(Anchor1_Pos,Anchor3_Pos,Ranges[1],Ranges[3])
    
    Plane23 = FindPlane(Anchor2_Pos,Anchor3_Pos,Ranges[2],Ranges[3])
    
    Point010203 = Cramers([Plane01,Plane02,Plane03])  #### all planes from anchor 0
    Point011213 = Cramers([Plane01,Plane12,Plane13])  #### all planes from anchor 1
    Point021223 = Cramers([Plane02,Plane12,Plane23])  #### all planes from anchor 2
    Point031323 = Cramers([Plane03,Plane13,Plane23])  #### all planes from anchor 3
    
    return Point010203, Point011213, Point021223, Point031323

''''  
def CramersIntersect(Plane1,Plane2,Plane3):
    coeff = [Plane1,Plane2,Plane3]
    return Cramers(coeff)
'''


#### Put everything below into rust/c/cpp whatever


def MakeSphere(Point, Range):
    a = (Point[0]*-2)
    a_v = (Point[0]*Point[0])
    b = (Point[1]*-2)
    b_v = (Point[1]*Point[1])
    c = (Point[2]*-2)
    c_v = (Point[2]*Point[2])
    d = (a_v + b_v + c_v - (Range * Range))
    return [a,b,c,d]
  
def FindPlane(Origin_Pos, Far_Pos, Origin_Range, Far_Range):
    Origin_Sphere = MakeSphere(Origin_Pos, Origin_Range)
    Far_Sphere = MakeSphere(Far_Pos, Far_Range)
    a = Origin_Sphere[0] - Far_Sphere[0]
    b = Origin_Sphere[1] - Far_Sphere[1]
    c = Origin_Sphere[2] - Far_Sphere[2]
    d = Origin_Sphere[3] - Far_Sphere[3]
    return a,b,c,d

def FindPoint(Ranges):
    Plane01 = FindPlane(Anchor0_Pos,Anchor1_Pos,Ranges[0],Ranges[1])
    Plane02 = FindPlane(Anchor0_Pos,Anchor2_Pos,Ranges[0],Ranges[2])
    Plane03 = FindPlane(Anchor0_Pos,Anchor3_Pos,Ranges[0],Ranges[3])
    
    Plane12 = FindPlane(Anchor1_Pos,Anchor2_Pos,Ranges[1],Ranges[2])
    Plane13 = FindPlane(Anchor1_Pos,Anchor3_Pos,Ranges[1],Ranges[3])
    
    Plane23 = FindPlane(Anchor2_Pos,Anchor3_Pos,Ranges[2],Ranges[3])
    
    Point010203 = Cramers([Plane01,Plane02,Plane03])  #### all planes from anchor 0
    Point011213 = Cramers([Plane01,Plane12,Plane13])  #### all planes from anchor 1
    Point021223 = Cramers([Plane02,Plane12,Plane23])  #### all planes from anchor 2
    Point031323 = Cramers([Plane03,Plane13,Plane23])  #### all planes from anchor 3
    
    return Point010203, Point011213, Point021223, Point031323

def determinantOfMatrix(mat):   #finds determinant of the matrix
    ans = (mat[0][0] * (mat[1][1] * mat[2][2] -
                        mat[2][1] * mat[1][2]) -
           mat[0][1] * (mat[1][0] * mat[2][2] -
                        mat[1][2] * mat[2][0]) +
           mat[0][2] * (mat[1][0] * mat[2][1] -
                        mat[1][1] * mat[2][0]))
    return ans

def Cramers(coeff):
    # Matrix d using coeff as given in
    # cramer's rule
    d = [[coeff[0][0], coeff[0][1], coeff[0][2]],
         [coeff[1][0], coeff[1][1], coeff[1][2]],
         [coeff[2][0], coeff[2][1], coeff[2][2]]]
     
    # Matrix d1 using coeff as given in
    # cramer's rule
    d1 = [[coeff[0][3], coeff[0][1], coeff[0][2]],
          [coeff[1][3], coeff[1][1], coeff[1][2]],
          [coeff[2][3], coeff[2][1], coeff[2][2]]]
     
    # Matrix d2 using coeff as given in
    # cramer's rule
    d2 = [[coeff[0][0], coeff[0][3], coeff[0][2]],
          [coeff[1][0], coeff[1][3], coeff[1][2]],
          [coeff[2][0], coeff[2][3], coeff[2][2]]]
     
    # Matrix d3 using coeff as given in
    # cramer's rule
    d3 = [[coeff[0][0], coeff[0][1], coeff[0][3]],
          [coeff[1][0], coeff[1][1], coeff[1][3]],
          [coeff[2][0], coeff[2][1], coeff[2][3]]]
 
    # Calculating Determinant of Matrices
    # d, d1, d2, d3
    D = determinantOfMatrix(d)
    D1 = determinantOfMatrix(d1)
    D2 = determinantOfMatrix(d2)
    D3 = determinantOfMatrix(d3)
      
    x = D1 / D  # Apply Cramer's Rule
    y = D2 / D
         
    z = D3 / D  # calculating z using cramer's rule
         
    return -x,-y,-z