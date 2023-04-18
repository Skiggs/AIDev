#pragma once
#include<cmath>
#include"SFML/Graphics.hpp"


inline float DotProduct(sf::Vector2f vec1, sf::Vector2f vec2)
{
	return (((double)vec1.x * (double)vec2.x) + ((double)vec1.y * (double)vec2.y));
}

inline sf::Vector2f UnitVector(sf::Vector2f vec)
{
	sf::Vector2f UnitVec;
	UnitVec.x = vec.x / sqrt((vec.x * vec.x + vec.y * vec.y));
	UnitVec.y = vec.y / sqrt((vec.x * vec.x + vec.y * vec.y));
	return UnitVec;
}

inline double magnitudeOfVec(sf::Vector2f vec)
{
	return sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

inline float Clamp(float value, float Min, float Max)
{
	if (value < Min) return Min;
	else if (value > Max) return Max;
	return value;
}

inline double Distance(sf::Vector2f pos1, sf::Vector2f pos2)
{
	return sqrt((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y));
}

inline float CheckDistance(sf::CircleShape cir, sf::Vector2f pos, sf::Vector2f pos2)
{
	sf::Vector2f lineVector = pos2 - pos;                                                           // (a) finds the vector for the line from pt1 to pt2  (b) so later I can find out its dot product to check how far the circle is (c) gives the vector in the direction of pos2 
	sf::Vector2f circle2lineVector = cir.getPosition() - pos;                                       // (a) finds the vector for the pt1 of line to circle's center (b) to check the magnitude of circle's position from pt1 of the line (c) gives the vector in the direction of circle's center 
	float distance = Distance(pos, pos2);
	sf::Vector2f unitLineVector = sf::Vector2f(lineVector.x / distance, lineVector.y / distance);   // (a) finds the unit vector for lineVector (b) so we can figure out how far is the circle from pt1 (c) gives the unit vector of line Vector
	float pro =  Clamp(DotProduct(circle2lineVector,unitLineVector),0.f,distance);        // (a) finds the dot product of unitLineVector and cir2linevector (b) so we can figure out how far is the circle on non common axis from pt1 (c) gives the dot product of the circle and unitLineVector 
	sf::Vector2f closest;                                                                           // (a) makes a vector2f variable (b) to store the value of the dotproduct bw cir2lineVec and UnitLineVec (c) creates a vector2f variable 
	closest.x = pro * unitLineVector.x + pos.x;
	closest.y = pro * unitLineVector.y + pos.y;												        // (a) multiplies the dot product with the UnitLineVector's X axis  (b) to figure out how far it is on the x-axis (c) calculates the magnitude of x axis
												                                                    // (a) multiplies the dot product with the UnitLineVector Y axis  (b) to figure out how far it is on the y-axis (c) calculates the magnitude of y axis
	return Distance(cir.getPosition(), closest);
                                                                                                    // (a) adds proVec with pt1 of the line and store it in closest (b) to figure out the closest point on the line to the circle in the same direction of the line (c) stores the closest point on the line							
}


inline float CheckDistFromPoint(sf::Vector2f pointPos, sf::Vector2f pos, sf::Vector2f pos2)
{
	sf::Vector2f lineVector = pos2 - pos;                                                           // (a) finds the vector for the line from pt1 to pt2  (b) so later I can find out its dot product to check how far the circle is (c) gives the vector in the direction of pos2 
	sf::Vector2f circle2lineVector = pointPos - pos;                                       // (a) finds the vector for the pt1 of line to circle's center (b) to check the magnitude of circle's position from pt1 of the line (c) gives the vector in the direction of circle's center 
	float distance = Distance(pos, pos2);
	sf::Vector2f unitLineVector = sf::Vector2f(lineVector.x / distance, lineVector.y / distance);   // (a) finds the unit vector for lineVector (b) so we can figure out how far is the circle from pt1 (c) gives the unit vector of line Vector
	float pro = Clamp(DotProduct(circle2lineVector, unitLineVector), 0.f, distance);                 // (a) finds the dot product of unitLineVector and cir2linevector (b) so we can figure out how far is the circle on non common axis from pt1 (c) gives the dot product of the circle and unitLineVector 
	sf::Vector2f closest;                                                                           // (a) makes a vector2f variable (b) to store the value of the dotproduct bw cir2lineVec and UnitLineVec (c) creates a vector2f variable 
	closest.x = pro * unitLineVector.x + pos.x;
	closest.y = pro * unitLineVector.y + pos.y;												        // (a) multiplies the dot product with the UnitLineVector's X axis  (b) to figure out how far it is on the x-axis (c) calculates the magnitude of x axis
																									// (a) multiplies the dot product with the UnitLineVector Y axis  (b) to figure out how far it is on the y-axis (c) calculates the magnitude of y axis
	return Distance(pointPos, closest);

}


inline sf::Vector2f RotateLine(float angle, sf::Vector2f pos1, sf::Vector2f pos2)
{
	float length = 50;
	sf::Vector2f newPoint;
	newPoint.x = pos1.x - std::cos(angle) * length;
	newPoint.y = pos1.y + std::sin(angle) * length;
	return newPoint;
}

inline sf::Vector2f Line2Mouse(sf::Vector2f pos1, sf::Vector2f pos2)
{
	float length = 50;
	sf::Vector2f newPoint;
	newPoint = pos1 + pos2 * length;
	return newPoint;
}

inline sf::Vector2f GetClosesPointToDoor(sf::CircleShape cir, sf::Vector2f pos, sf::Vector2f pos2)
{
	sf::Vector2f lineVector = pos2 - pos;                                                           // (a) finds the vector for the line from pt1 to pt2  (b) so later I can find out its dot product to check how far the circle is (c) gives the vector in the direction of pos2 
	sf::Vector2f circle2lineVector = cir.getPosition() - pos;                                       // (a) finds the vector for the pt1 of line to circle's center (b) to check the magnitude of circle's position from pt1 of the line (c) gives the vector in the direction of circle's center 
	float distance = Distance(pos, pos2);
	sf::Vector2f unitLineVector = sf::Vector2f(lineVector.x / distance, lineVector.y / distance);   // (a) finds the unit vector for lineVector (b) so we can figure out how far is the circle from pt1 (c) gives the unit vector of line Vector
	float pro = Clamp(DotProduct(circle2lineVector, unitLineVector), 0.f, distance);        // (a) finds the dot product of unitLineVector and cir2linevector (b) so we can figure out how far is the circle on non common axis from pt1 (c) gives the dot product of the circle and unitLineVector 
	sf::Vector2f closest;                                                                           // (a) makes a vector2f variable (b) to store the value of the dotproduct bw cir2lineVec and UnitLineVec (c) creates a vector2f variable 
	closest.x = pro * unitLineVector.x + pos.x;
	closest.y = pro * unitLineVector.y + pos.y;

	return closest;
}

inline sf::Vector2f GetClosesPointToDoor(sf::Vector2f EntityPos, sf::Vector2f pos, sf::Vector2f pos2)
{
	sf::Vector2f lineVector = pos2 - pos;
	sf::Vector2f circle2lineVector = EntityPos - pos;
	sf::Vector2f unitLineVector = UnitVector(lineVector);
	double pro = DotProduct(circle2lineVector, unitLineVector);

	sf::Vector2f proVec;
	proVec.x = pro * (UnitVector(lineVector)).x;
	proVec.y = pro * (UnitVector(lineVector)).y;

	sf::Vector2f closestToDoor = proVec + pos;

	return closestToDoor;
}

inline bool areLinesColliding(sf::Vector2f enemy, sf::Vector2f player, sf::Vector2f wallP1, sf::Vector2f wallP2)
{
	float x1 = enemy.x;
	float y1 = enemy.y;
	float x2 = player.x;
	float y2 = player.y;
	float x3 = wallP1.x;
	float y3 = wallP1.y;
	float x4 = wallP2.x;
	float y4 = wallP2.y;

	float denominator = ((x2 - x1) * (y4 - y3)) - ((y2 - y1) * (x4 - x3));
	float numerator1 = ((y1 - y3) * (x4 - x3)) - ((x1 - x3) * (y4 - y3));
	float numerator2 = ((y1 - y3) * (x2 - x1)) - ((x1 - x3) * (y2 - y1));

	// Detect coincident lines (has a problem, read below)
	if (denominator == 0) return numerator1 == 0 && numerator2 == 0;

	float r = numerator1 / denominator;
	float s = numerator2 / denominator;

	return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);

}

inline sf::Vector2f PointOfContactOfLines(sf::Vector2f enemy, sf::Vector2f player, sf::Vector2f wallP1, sf::Vector2f wallP2)
{

	float x1 = enemy.x;
	float y1 = enemy.y;
	float x2 = player.x;
	float y2 = player.y;
	float x3 = wallP1.x;
	float y3 = wallP1.y;
	float x4 = wallP2.x;
	float y4 = wallP2.y;

	// calculate the distance to intersection point
	float uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
	float uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		return sf::Vector2f(x1 + (uA * (x2 - x1)), y1 + (uA * (y2 - y1)));
	}
	return sf::Vector2f(NULL, NULL);
}


template<class T> inline double AngleBWVectors(T v1, T v2)
{
	//dot = (dot < -1.0 ? -1.0 : (dot > 1.0 ? 1.0 : dot));
	double angle = atan2(v1.x * v2.y - v1.y * v2.x, v1.x * v2.x + v1.y * v2.y);
	return angle;
}
