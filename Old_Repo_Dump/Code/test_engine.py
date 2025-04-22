from scipy.optimize import minimize
import numpy as np

Anchor0_Pos = [3,0,3]   #### Primary anchor
Anchor1_Pos = [-3,3,3]  #### Aux 1
Anchor2_Pos = [3,3,-3]  #### Aux 2
Anchor3_Pos = [-3,0,-3] #### Aux 3
Anchors_Pos = list(np.array([Anchor0_Pos, Anchor1_Pos, Anchor2_Pos, Anchor3_Pos]))

Tag_Start_Pos = [0,0,0]

def distancefrom(first_point, second_point):
    return numpy.sqrt(((first_point[0]-second_point[0])**2)+((first_point[1]-second_point[1])**2)+((first_point[2]-second_point[2])**2))

def FindRanges(Tag_Pos):
    Anchor0_Range = distancefrom(Anchor0_Pos, Tag_Pos)
    Anchor1_Range = distancefrom(Anchor1_Pos, Tag_Pos)
    Anchor2_Range = distancefrom(Anchor2_Pos, Tag_Pos)
    Anchor3_Range = distancefrom(Anchor3_Pos, Tag_Pos)
    return Anchor0_Range, Anchor1_Range, Anchor2_Range, Anchor3_Range

def gps_solve(distances_to_station, stations_coordinates):
	def error(x, c, r):
		return sum([(np.linalg.norm(x - c[i]) - r[i]) ** 2 for i in range(len(c))])

	l = len(stations_coordinates)
	S = sum(distances_to_station)
	# compute weight vector for initial guess
	W = [((l - 1) * S) / (S - w) for w in distances_to_station]
	# get initial guess of point location
	x0 = sum([W[i] * stations_coordinates[i] for i in range(l)])
    # optimize distance from signal origin to border of spheres
	return minimize(error, x0, args=(stations_coordinates, distances_to_station), method='Nelder-Mead').x,x0


if __name__ == "__main__":
	stations = list(np.array([[1,1], [0,1], [1,0], [0,0]]))
	distances_to_station = [0.1, 0.5, 0.5, 1.3]
	print(gps_solve(distances_to_station, stations))