#include "../include/camera.h"
#include "../include/physics.h"
#include <math.h>


ImVec2 operator - (const ImVec2& _1) {
	return { -_1.x, -_1.y };
}
ImVec2 operator + (const ImVec2& _1, const float& _2) {
	return { _1.x + _2, _1.y + _2 };
}
ImVec2 operator - (const ImVec2& _1, const float& _2) {
	return { _1.x - _2, _1.y - _2 };
}
ImVec2 operator * (const ImVec2& _1, const float& _2) {
	return { _1.x * _2, _1.y * _2 };
}
ImVec2 operator / (const ImVec2& _1, const float& _2) {
	return { _1.x / _2, _1.y / _2 };
}

double distanceSquared(const ImVec2& p1, const ImVec2& p2) {
	return pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2);
}

const bool PhysicsObject::isLine() {
	return dynamic_cast<Line2d*>(this);
}

const bool PhysicsObject::isRect() {
	return dynamic_cast<Rect2d*>(this);
}

const bool PhysicsObject::isCircle() {
	return dynamic_cast<Circle2d*>(this);
}

const bool PhysicsObject::isPoly() {
	return dynamic_cast<Polygon2d*>(this);
}

const PhysicsObject::PhysicsObjectType PhysicsObject::getType() {
	if (isLine()) { return LINE; }
	if (isCircle()) { return CIRCLE; }
	if (isPoly()) { return POLY; }
	if (isRect()) { return RECT; }
}

// Function to check if a point is inside a Circle
bool pointInCircle(const ImVec2& point, const Circle2d& circle) {
	double distanceSquared = pow(point.x - circle.centerX, 2) + pow(point.y - circle.centerY, 2);
	return distanceSquared <= pow(circle.radius, 2);
}

bool lineIntersectsCircle(const Line2d& line, const Circle2d& circle, bool checkInside) {
	// Check if the start or end point of the line is inside the circle
	if (pointInCircle(line.start, circle) || pointInCircle(line.end, circle)) {
		return true;
	}

	double dx = line.end.x - line.start.x;
	double dy = line.end.y - line.start.y;

	double a = dx * dx + dy * dy;
	double b = 2 * (dx * (line.start.x - circle.centerX) + dy * (line.start.y - circle.centerY));
	double c = (line.start.x - circle.centerX) * (line.start.x - circle.centerX) +
		(line.start.y - circle.centerY) * (line.start.y - circle.centerY) -
		circle.radius * circle.radius;

	double discriminant = b * b - 4 * a * c;

	// If the discriminant is non-negative, the line intersects the circle
	return discriminant >= 0;
}

// Function to check if a polygon intersects with a line
bool polygonIntersectsLine(const Polygon2d& polygon, const Line2d& line) {
	// Check if any segment of the line intersects with the polygon
	size_t numVertices = polygon.vertices.size();
	for (size_t i = 0; i < numVertices; ++i) {
		const ImVec2& vertex1 = polygon.vertices.at(i);
		const ImVec2& vertex2 = polygon.vertices.at((i + 1) % numVertices);

		if (doIntersect(line.start, line.end, vertex1, vertex2)) {
			return true;
		}
	}

	return false;
}

bool polygonIntersectsRect(const Polygon2d& polygon, const Rect2d& rect) {
	size_t numVertices = polygon.vertices.size();
	for (size_t i = 0; i < numVertices; ++i) {
		const ImVec2& vertex1 = polygon.vertices.at(i);
		const ImVec2& vertex2 = polygon.vertices.at((i + 1) % numVertices);
		if (lineIntersectsRect({ vertex1, vertex2 }, rect, true)) { return true; }
	}
	return false;
}

// Function to check if a polygon intersects with a circle
bool polygonIntersectsCircle(const Polygon2d& polygon, const Circle2d& circle, bool checkInside) {
	size_t numVertices = polygon.vertices.size();
	// Check if any segment of the polygon intersects with the circle
	for (size_t i = 0; i < numVertices; ++i) {
		const ImVec2& vertex1 = polygon.vertices.at(i);
		const ImVec2& vertex2 = polygon.vertices.at((i + 1) % numVertices);

		if (lineIntersectsCircle({ vertex1, vertex2 }, circle, checkInside)) {
			return true;
		}
	}

	return false;
}

bool rectIntersectsCircle(const Rect2d& rect, const Circle2d& circle, bool checkInside) {
	// Check if any corner of the rectangle is inside the circle
	ImVec2 rectCorners[4] = {
		{ rect.x, rect.y },
		{ rect.x + rect.w, rect.y },
		{ rect.x + rect.w, rect.y + rect.h },
		{ rect.x, rect.y + rect.h }
	};

	for (int i = 0; i < 4; ++i) {
		if (pointInCircle(rectCorners[i], circle)) {
			return true;
		}
	}

	// Check if any side of the rectangle intersects with the circle
	Line2d rectSides[4] = {
		{ rectCorners[0], rectCorners[1] },
		{ rectCorners[1], rectCorners[2] },
		{ rectCorners[2], rectCorners[3] },
		{ rectCorners[3], rectCorners[0] }
	};

	for (int i = 0; i < 4; ++i) {
		if (lineIntersectsCircle(rectSides[i], circle)) {
			return true;
		}
	}

	return false;
}

bool polygonIntersectsPolygon(const Polygon2d& polygon1, const Polygon2d& polygon2) {
	// Check if any segment of one polygon intersects with the other polygon
	size_t numVertices1 = polygon1.vertices.size();
	size_t numVertices2 = polygon2.vertices.size();
	for (size_t i = 0; i < numVertices1; ++i) {
		const ImVec2& vertex1 = polygon1.vertices.at(i);
		const ImVec2& vertex2 = polygon1.vertices.at((i + 1) % numVertices1);

		if (polygonIntersectsLine(polygon2, { vertex1, vertex2 })) {
			return true;
		}
	}
	return false;
}

bool circleIntersectsCircle(Circle2d circle1, Circle2d circle2) {
	return pow(circle2.centerX - circle1.centerX, 2) + pow(circle2.centerX - circle1.centerY, 2) <= pow(circle1.radius + circle2.radius, 2);
}

bool doIntersect(const ImVec2& p1, const ImVec2& q1, const ImVec2& p2, const ImVec2& q2) {
	double o1 = ((q1.y - p1.y) * (p2.x - p1.x)) - ((q1.x - p1.x) * (p2.y - p1.y));
	double o2 = ((q1.y - p1.y) * (q2.x - p1.x)) - ((q1.x - p1.x) * (q2.y - p1.y));
	double o3 = ((q2.y - p2.y) * (p1.x - p2.x)) - ((q2.x - p2.x) * (p1.y - p2.y));
	double o4 = ((q2.y - p2.y) * (q1.x - p2.x)) - ((q2.x - p2.x) * (q1.y - p2.y));

	return (o1 * o2 < 0) && (o3 * o4 < 0);
}

bool pointInRect(const ImVec2& point, const Rect2d& rect) {
	return (point.x >= rect.x && point.x <= (rect.x + rect.w) &&
		point.y >= rect.y && point.y <= (rect.y + rect.h));
}

bool rectIntersectsRect(const Rect2d& rect1, const Rect2d& rect2) {
	return Rect2d::rectIntersects(rect1, rect2);
}

bool lineIntersectsRect(const Line2d& line, const Rect2d& rect, bool checkInside) {
	// Check if the start or end point of the line is inside the rectangle
	if (Rect2d::isEmpty(&rect))
		if (pointInRect(line.start, rect) || pointInRect(line.end, rect)) {
			return true;
		}

	// .___.
	// |   | : 1
	// #---'
	// .___.
	// |   | : 2
	// '---#
	// .___#
	// |   | : 3
	// '---'
	// #___.
	// |   | : 4
	// '---'
	ImVec2 rectPoints[4] = {
		{ rect.x, rect.y },
		{ rect.x + rect.w, rect.y },
		{ rect.x + rect.w, rect.y + rect.h },
		{ rect.x, rect.y + rect.h }
	};
	// [1,2], [2,3], [3,4], [4,1]
	for (int i = 0; i < 4; ++i) {
		ImVec2 rectSegmentStart = rectPoints[i];
		ImVec2 rectSegmentEnd = rectPoints[(i + 1) % 4];

		if (doIntersect(line.start, line.end, rectSegmentStart, rectSegmentEnd)) {
			return true;
		}
	}

	return false;
}


// Helper function to check if a ray intersects a segment


// Function to check if a point is inside the concave polygon

Polygon2d::Polygon2d(const std::vector<ImVec2>& vertices) : vertices(vertices) {}

bool Polygon2d::isPointInside(const ImVec2& point) const {
	int count = 0;
	size_t numVertices = vertices.size();

	for (size_t i = 0; i < numVertices; ++i) {
		const ImVec2& vertex1 = vertices[i];
		const ImVec2& vertex2 = vertices[(i + 1) % numVertices];

		if (rayIntersectsSegment(point, vertex1, vertex2)) {
			++count;
		}
	}

	return count % 2 == 1;  // Inside if odd number of intersections
}

std::vector<ImVec2>& Polygon2d::getVertices() { return vertices; }

size_t Polygon2d::vertex_count() { return vertices.size(); }

bool Polygon2d::rayIntersectsSegment(const ImVec2& point, const ImVec2& segmentStart, const ImVec2& segmentEnd) const {
	return ((segmentStart.y > point.y) != (segmentEnd.y > point.y)) &&
		(point.x < (segmentEnd.x - segmentStart.x) * (point.y - segmentStart.y) /
			(segmentEnd.y - segmentStart.y) + segmentStart.x);
}

bool Polygon2d::collides(PhysicsObject& other) const {
	switch (other.getType()) {
	case POLY: return polygonIntersectsPolygon(*this, *static_cast<Polygon2d*>(&other));
	case CIRCLE: return polygonIntersectsCircle(*this, *static_cast<Circle2d*>(&other));
	case RECT: return polygonIntersectsRect(*this, *static_cast<Rect2d*>(&other));
	case LINE: return polygonIntersectsLine(*this, *static_cast<Line2d*>(&other));
	}
	return false;
}
bool Circle2d::collides(PhysicsObject& other) const {
	switch (other.getType()) {
	case POLY: return polygonIntersectsCircle(*static_cast<Polygon2d*>(&other), *this);
	case CIRCLE: return circleIntersectsCircle(*this, *static_cast<Circle2d*>(&other));
	case RECT: return rectIntersectsCircle(*static_cast<Rect2d*>(&other), *this);
	case LINE: return lineIntersectsCircle(*static_cast<Line2d*>(&other), *this);
	}
	return false;
}
Line2d::Line2d() {}
Line2d::Line2d(ImVec2 s, ImVec2 e) :start(s),end(e){}
bool Line2d::collides(PhysicsObject& other) const {
	switch (other.getType()) {
	case POLY: return polygonIntersectsLine(*static_cast<Polygon2d*>(&other), *this);
	case CIRCLE: return lineIntersectsCircle(*this, *static_cast<Circle2d*>(&other));
	case RECT: return lineIntersectsRect(*this, *static_cast<Rect2d*>(&other));
	case LINE: {
		auto& otherLine = *static_cast<Line2d*>(&other);
		return doIntersect(otherLine.start, otherLine.end, this->start, this->end);
	};
	}
	return false;
}
bool Rect2d::isEmpty(const Rect2d* r) {
	return r->w == 0 || r->h == 0;
}
bool Rect2d::rectIntersects(const Rect2d& l, const Rect2d& r) {
	return !((l.x + l.w) < r.x || (r.x + r.w) < l.x ||
		(l.y + l.h) < r.y || (r.y + r.h) < l.y);
}
bool Rect2d::empty() { return isEmpty(this); }
Rect2d::Rect2d(float _x, float _y, float _w, float _h) {
	x = _x;
	y = _y;
	w = _w;
	h = _h;
}
Rect2d Rect2d::im(ImVec2 start, ImVec2 end) {
	return { start, end - start };
};
Rect2d::Rect2d(ImVec2 lhr, ImVec2 rhr) :x(lhr.x), y(lhr.y), w(rhr.x), h(rhr.y) {};
ImVec2 Rect2d::max() { return {x + w, y + h}; }
ImVec2 Rect2d::min() { return {x, y}; }
Rect2d::operator ImRect() {
	return { x,y,x + w, y + h };
}
bool Rect2d::collides(PhysicsObject& other) const {
	switch (other.getType()) {
	case POLY: return polygonIntersectsRect(*static_cast<Polygon2d*>(&other), *this);
	case CIRCLE: return rectIntersectsCircle(*this, *static_cast<Circle2d*>(&other));
	case RECT: return rectIntersectsRect(*this, *static_cast<Rect2d*>(&other));
	case LINE: return lineIntersectsRect(*static_cast<Line2d*>(&other), *this);
	}
	return false;
}