
#ifndef CAMERA_H
#define CAMERA_H
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include <math.h>
#include <vector>
double distanceSquared(const ImVec2& p1, const ImVec2& p2);
struct Line2d;
struct Rect2d;
struct Circle2d;
struct Polygon2d;
struct PhysicsObject {
	enum PhysicsObjectType {
		LINE, RECT, CIRCLE, POLY
	};
	const bool isLine();
	const bool isRect();
	const bool isCircle();
	const bool isPoly();
	const PhysicsObjectType getType();
	virtual bool collides(PhysicsObject& other) const = 0;
};

struct Polygon2d : public PhysicsObject{
	Polygon2d(const std::vector<ImVec2>& vertices);

	// Function to check if a point is inside the concave polygon
	bool isPointInside(const ImVec2& point) const;
	std::vector<ImVec2>& getVertices();
	size_t vertex_count();
	std::vector<ImVec2> vertices;

	// Helper function to check if a ray intersects a segment
	bool rayIntersectsSegment(const ImVec2& point, const ImVec2& segmentStart, const ImVec2& segmentEnd) const;

	bool collides(PhysicsObject& other) const override;
};
struct Line2d : public PhysicsObject{
	Line2d();
	Line2d(ImVec2 s, ImVec2 e);
	ImVec2 start, end;
	// Override the collides method
	bool collides(PhysicsObject& other) const override;
};

#pragma pack(push, 1)
struct Rect2d : public PhysicsObject {
	static bool isEmpty(const Rect2d* r);
	static inline bool rectIntersects(const Rect2d& l, const Rect2d& r);
	bool empty();
	Rect2d(float _x, float _y, float _w, float _h);
	Rect2d(ImVec2 pos, ImVec2 size);
	static Rect2d im(ImVec2 start, ImVec2 end);
	Rect2d() {}
	ImVec2 min();
	ImVec2 max();
	float x, y, w, h;
	operator ImRect();
	bool collides(PhysicsObject& other) const override;
};
#pragma pack(pop)

struct Circle2d : public PhysicsObject {
	Circle2d() {}
	Circle2d(ImVec2 c, double radii) : centerX(c.x), centerY(c.y), radius(radii) {}
	double centerX, centerY, radius;
	bool collides(PhysicsObject& other) const override;
};
// Function to check if a point is inside a Circle
bool pointInCircle(const ImVec2& point, const Circle2d& circle);
bool lineIntersectsCircle(const Line2d& line, const Circle2d& circle, bool checkInside = false);
// Function to check if a polygon intersects with a line
bool polygonIntersectsLine(const Polygon2d& polygon, const Line2d& line);
bool polygonIntersectsRect(const Polygon2d& polygon, const Rect2d& rect);

// Function to check if a polygon intersects with a circle
bool polygonIntersectsCircle(const Polygon2d& polygon, const Circle2d& circle, bool checkInside = false);
bool rectIntersectsCircle(const Rect2d& rect, const Circle2d& circle, bool checkInside = false);

bool polygonIntersectsPolygon(const Polygon2d& polygon1, const Polygon2d& polygon2);
bool circleIntersectsCircle(Circle2d circle1, Circle2d circle2);
bool doIntersect(const ImVec2& p1, const ImVec2& q1, const ImVec2& p2, const ImVec2& q2);
bool pointInRect(const ImVec2& point, const Rect2d& rect);
bool rectIntersectsRect(const Rect2d& rect1, const Rect2d& rect2);
bool lineIntersectsRect(const Line2d& line, const Rect2d& rect, bool checkInside = false);

struct Transform {
	static void recalcScale(ImVec2* base_sc, ImVec2 scale);
	ImVec2 position = { 0,0 };
	// scale in pixels! To set via a percentage, use Transform::recalcScale(&tr_t.scale, {amt_x, amt_y});
	ImVec2 scale = { 0,0 };
	ImVec2 origin = { 0,0 };
	double angle = 0;
};
namespace VecUtils {
	static ImVec2 down = { 0, -1 };
	static ImVec2 up = { 0, 1 };
	static ImVec2 left = { -1, 0 };
	static ImVec2 right = { 1, 0 };
	static ImVec2 parse_angle(float angle);
	static float parse_angle_vec(ImVec2 v);

}
class Camera {
public:
	bool culling = true;
	bool blackoutMargins = true;
	// This is EXCLUSIVELY for translating the virtual (world) viewport into screen viewport!
	ImVec2 m_VirtualViewport = ImVec2(1024,1024); // World viewport (ie, 1024x512)
	ImVec2 m_GlobalViewport = ImVec2(720, 720); // Screen viewport (ie, 1920x1080)
	ImVec2 pointToScreenViewport(ImVec2 pos);
	float getScaleFactor();
	ImVec2 pointToWorldViewport(ImVec2 pos);
	float scale = 1.0f;
	ImVec2 m_Offset;
	bool isPortrait();
	ImVec2* m_target = new ImVec2(0,0);
	static Camera* GetInstance();
	ImVec2 getViewport();
	void SetTarget(ImVec2* targ);;
	void Update();
	static void lu_SetTarget(ImVec2* target);
	static float lu_GetScale();
	static void lu_SetScale(float _target);
	static ImVec2 lu_GetPosition();
	static void lu_SetPosition(ImVec2 * _target);
	static ImVec2 ImVec2ToCameraSpace(ImVec2 v);
	Camera();
	Camera(ImVec2* targ);
private:
};
#endif // CAMERA_H