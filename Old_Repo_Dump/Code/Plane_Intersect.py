import numpy as np

Anchor0_Pos = [3,0,3]   #### Primary anchor
Anchor1_Pos = [-3,3,3]  #### Aux 1
Anchor2_Pos = [3,3,-3]  #### Aux 2
Anchor3_Pos = [-3,0,-3] #### Aux 3

def FindRanges(Tag_Pos):
    Anchor0_Range = distancefrom(Anchor0_Pos, Tag_Pos)
    Anchor1_Range = distancefrom(Anchor1_Pos, Tag_Pos)
    Anchor2_Range = distancefrom(Anchor2_Pos, Tag_Pos)
    Anchor3_Range = distancefrom(Anchor3_Pos, Tag_Pos)
    return Anchor0_Range, Anchor1_Range, Anchor2_Range, Anchor3_Range

def find_plane_of_intersection(center1, center2, radius1, radius2):
    # Calculate the vector connecting the centers of the spheres
    vector_between_centers = np.array(center2) - np.array(center1)

    # Calculate the distance between the centers of the spheres
    distance_between_centers = np.linalg.norm(vector_between_centers)

    # Check if the spheres intersect
    if distance_between_centers > radius1 + radius2:
        return None  # Spheres do not intersect, so there is no plane of intersection

    # Normalize the vector between centers to get the normal vector to the plane
    normal_vector = vector_between_centers / distance_between_centers

    # The equation of the plane: Ax + By + Cz + D = 0
    A, B, C = normal_vector
    D = -np.dot(normal_vector, center1)

    return (A, B, C, D)

def find_intersection_of_planes(planes):
    # Check if there are at least 3 planes
    if len(planes) < 3:
        raise ValueError("At least 3 planes are required for intersection.")

    # Extract the normal vectors and D values from each plane
    normals = np.array([plane[:3] for plane in planes])
    D_values = np.array([plane[3] for plane in planes])

    # Find the rank of the normal vectors matrix
    rank = np.linalg.matrix_rank(normals)

    # Check the rank to determine the type of intersection
    if rank == 2:
        return None  # The planes are parallel or coincident, no intersection
    elif rank == 3:
        # The planes intersect at a point, solve the linear system
        intersection_point = np.linalg.solve(normals, -D_values)
        return intersection_point
    else:
        # The planes are dependent, they intersect along a line
        intersection_line = np.linalg.lstsq(normals, -D_values, rcond=None)[0]
        return intersection_line
    
def get_planes():
    planes = []
    planes.append(find_plane_of_intersection(Anchor0_Pos,Anchor1_Pos,2.83,4.13))
    planes.append(find_plane_of_intersection(Anchor0_Pos,Anchor2_Pos,2.83,6.41))
    planes.append(find_plane_of_intersection(Anchor0_Pos,Anchor3_Pos,2.83,7.49))
    
    #planes.append(find_plane_of_intersection(Anchor1_Pos,Anchor2_Pos,4.13,6.41))
    #planes.append(find_plane_of_intersection(Anchor1_Pos,Anchor3_Pos,4.13,7.49))
    
    #planes.append(find_plane_of_intersection(Anchor2_Pos,Anchor3_Pos,6.41,7.49))
    
    return planes
    