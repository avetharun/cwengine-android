#if !defined(RENDERER_HPP)
#define RENDERER_HPP

#include <functional>
#include <iostream>
#include <vector>
#include <map>
#include "../utils.hpp"
#include "camera.h"
#include <thread>
#include <typeinfo>
#include <typeindex>
#include <math.h>
#include <algorithm>
#ifndef AST_LUA_HPP
#include "lua.hpp"
#endif
#include "abt/cwerror.h"
#include <lauxlib.h>
#include "../imgui/imgui.h"

#include <variant>
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_extras.h"
#include "../rsm_renderer.h"

enum IMAGE_LAYERS : int {
	TOP = INT32_MAX,
	BOTTOM = INT32_MIN
};

struct SpriteAnimationMeta {
	bool animate = true;
	bool animate_once = false;
	bool reset_after_once = true;
	int CURRENT_FRAME = 0;
	float DELTA = 0;
	int frames = 0;
	float delay = 0;
	float alpha_start = 1;
	float alpha_end = 1;
	std::function<void()> OnLoopOrReset = []() {};
	void set_loop_func(std::function<void()> callback);
	SpriteAnimationMeta();
	SpriteAnimationMeta(int n_frames, float m_delay);
};
struct Sprite{
	std::shared_ptr<Sprite> self;
	std::shared_ptr<Sprite> get_ptr();
	ImColor tint{ 1,1,1,1 };
	SpriteAnimationMeta meta;
	/**
	*  \brief Should the batch renderer draw this?
	*/
	bool enabled = true;
	/**
	*  \brief Is this variable something associated with an object, and if so should it be allowed to be retrieved by getElementByID()?
	*/
	bool isPrivate = false;
	/**
	 *  \brief Sprite paramaters
	 *	\param bit definitions:
	 *  \param | 0 : tells renderer to render sprite by world coordinates.
	 *
	 *
	 * \param ..
	 */
	char params;
	std::string name, filename;
	std::string identifier;
	/**
	 *  \brief What layer is the sprite on? (basically Z offset)
	 */
	int32_t layer = -10;
	bool enableCameraOffset = false; // Enables/disables if camera::m_Offset will offset sprite position.
	// Centers sprite in the middle of camera
	bool center = false;

	Sprite();;
	unsigned char* pixels = nullptr;
	int channels = 4;
	Rect2d uv{};
	Rect2d __uv_last{};
	float alpha = 1;
	/*
	Theory:
	Starting position of uv = uv.x, uv.y
	Multiply xy with the following vector, which will be the "tile" of the sprite. Check if in bounds.
	*/
	ImVec2 uv_tile{0,0};
	Rect2d uv_final{};
	void unlockCamera();
	void lockCamera();
	float alpha_delta;
	float alpha_from = 1;
	float alpha_to = 1;
	ImVec2 center_position{};
	ImVec2 _offset{};
	ImVec2 const_offset{};
	ImVec2 _cameraViewOffset{};
	Rect2d rect = {};
	Transform transform{};
	int calc_uv_pair(int from, int base, int amt);
	
	ImVec2 pixel_size;
	void Update();
	bool manualDraw = false;
	bool isRendering;
	std::shared_ptr<rsm::ImageAsset> Texture;
	/*
	* \brief Draws to screen immediately. You can run this, or let the batch renderer do its thing. Either works.
	* 
	*/
	void Draw();
	// This is used because when reloading the app (ie, leaving then going back into the app) will cause errors!
	static void resetCache();
	
	void operator ~();
	Sprite(const char* filename, Rect2d spriteuv = Rect2d{ 0,0,0,0 });
	void setID(std::string i);
	std::string getID();
	static bool eraseElementByID(std::string id);
	static std::shared_ptr<Sprite> getElementByID(std::string id);
	static void insertElementWithID(std::string id, std::shared_ptr<Sprite> s);
	std::shared_ptr<Sprite> copy();
	void insert();
	void remove();


	void lua_set_uv(float x, float y, float w, float h);
	Rect2d lua_get_uv();
	lua_Number lu_getLayer();
	ImVec2 getPosition();
	double getRotation();
	ImVec2 getScale();
	void Scale(ImVec2 scaleAmt);
	void Move(ImVec2 moveAmt);
	void SetTransform(Transform _transform);
	void Rotate(int amt);
	void SetRotation(int amt);
	void SetPosition(ImVec2 amt);
	void setEnabled(bool val);
};
struct ParticleEffects {
	struct stub {
		Rect2d uv;
		std::string id;
		float lifetime;
		ImVec2 size;
		ImVec2 randomness = {12, 12};
		float velocity = 10;
		float angle = 90; // down
		int layer = 0;
		SpriteAnimationMeta meta;
	};
};
struct ParticleEffect {
	static inline std::shared_ptr<Sprite> _m_ParticleImage = nullptr;
	ParticleEffects::stub effect;
	struct _stub {
		float life = 0;
		float life_randomness = 0.8f;
		float velocity_randomness = 1;
	};
	std::vector<std::pair<std::shared_ptr<Sprite>, _stub*>> particles;
	int amt;
	bool collides;
	int m_layer;
	float m_velocity;
	float m_lifetime;
	float m_dir_angle_rad = 0;
	ImVec2 m_randomness;
	ImVec2 start_pos;
	Rect2d m_uv;
	bool dtor;
	std::shared_ptr<Sprite> m_particle_sprite;
	std::string name;

	static ParticleEffect* getElementByID(std::string id);
	ParticleEffect(ParticleEffects::stub m_particle, float m_amt);
	void operator ~();
	void append(int count);
	void pop(int count);
	void refresh();
	void Render();
	static inline std::vector<ParticleEffect*> m_particle_arr = {};
};
struct BatchRenderer {
	static void Render();
};









/*
* \brief Draws to screen immediately. You can run this, or let the batch renderer do its thing. Either works.
*
*/

#endif