#include "../include/renderer.h"
std::map < std::string, std::shared_ptr< rsm::ImageAsset >> surfFilemaps{};
static std::vector<std::shared_ptr<Sprite>> _mglobalspritearr;
void updateAndDraw(std::shared_ptr<Sprite> sprite) {
	sprite->Draw();
}
void BatchRenderer::Render() {
	for (int i = 0u; i < ParticleEffect::m_particle_arr.size(); i++) {
		ParticleEffect* s = ParticleEffect::m_particle_arr[i];
		if (s == nullptr) {
			ParticleEffect::m_particle_arr.erase(ParticleEffect::m_particle_arr.begin() + i);
			continue;
		}
		s->Render();
	}
	for (unsigned int i = 0; i < _mglobalspritearr.size(); i++) {
		std::shared_ptr<Sprite> s = _mglobalspritearr[i];
		if (s == nullptr) {
			_mglobalspritearr.erase(_mglobalspritearr.begin() + i);
			continue;
		}
		(s->enabled) ? updateAndDraw(s) : noop;
	}
	auto c = Camera::GetInstance();
	if (c->blackoutMargins) {
		// upper or left occluder
		ImVec2 occluder1Min, occluder1Max;
		// lower or right occluder
		ImVec2 occluder2Min, occluder2Max;
		// calculate occluder positions
		if (c->isPortrait()) {
			occluder1Min = { 0,0 };
			occluder1Max = { c->m_GlobalViewport.x,  c->pointToWorldViewport({0,0}).y};
			occluder2Min = { c->pointToScreenViewport({0,c->m_VirtualViewport.y}).x,  c->m_GlobalViewport.y};
			occluder2Max = { c->m_GlobalViewport.x,  c->m_GlobalViewport.y};
		} else {

		}
		ImGui::GetForegroundDrawList()->AddRectFilled(occluder1Min, occluder1Max, 0x0000ffff);
		ImGui::GetForegroundDrawList()->AddRectFilled(occluder2Min, occluder2Max, 0x0000ffff);
	}
}
void Sprite::operator ~() {
	int idx = 0;
	for (auto k : _mglobalspritearr) {
		if (k.get() == this) {
			break;
		}
		idx++;
	}
	_mglobalspritearr.erase(_mglobalspritearr.begin() + idx);
}
ParticleEffect* ParticleEffect::getElementByID(std::string id) {
	for (int i = 0; i < m_particle_arr.size(); i++) {
		ParticleEffect* _s = m_particle_arr[i];
		if (alib_streq(id, _s->name.c_str())) {
			return _s;
		}
	}
	return nullptr;
}

ParticleEffect::ParticleEffect(ParticleEffects::stub m_particle, float m_amt) {
	if (!_m_ParticleImage) {
		_m_ParticleImage = std::make_shared<Sprite>("sprites/particles.png");
		_m_ParticleImage->transform.scale = m_particle.size;
		_m_ParticleImage->enabled = false;
		_m_ParticleImage->meta = effect.meta;
	}
	this->effect = m_particle;
	this->amt = m_amt;
	m_particle_sprite = _m_ParticleImage->copy();
	m_particle_sprite->enabled = false;
	m_particle_sprite->layer = effect.layer;
	this->m_velocity = effect.velocity;
	this->m_layer = effect.layer;
	this->m_lifetime = effect.lifetime;
	this->m_randomness = effect.randomness;
	this->m_uv = effect.uv;
	this->m_dir_angle_rad = alib_deg2rad(effect.angle);
	m_particle_sprite->uv = effect.uv;
	name = effect.id;
	for (int i = 0; i < amt; i++) {
		std::shared_ptr<Sprite> copy = m_particle_sprite->copy();
		copy->enabled = true;
		particles.push_back(std::make_pair(copy, new _stub{ .life = m_lifetime }));
	}

	m_particle_arr.push_back(this);
}

void ParticleEffect::operator ~() {
	pop(amt);
	auto v = std::find(m_particle_arr.begin(), m_particle_arr.end(), this);
	if (v != m_particle_arr.end()) {
		m_particle_arr.erase(v);
	}
}
void ParticleEffect::append(int count) {
	amt += count;
	for (int i = 0; i < count; i++) {
		std::shared_ptr<Sprite> copy = m_particle_sprite->copy();
		copy->enabled = true;
		particles.push_back(std::make_pair(copy, new _stub{ .life = m_lifetime }));
	}
}

void ParticleEffect::pop(int count) {
	amt -= count;
	for (int i = 0; i < count; i++) {
		auto& m_particle = particles.at(particles.size() - 1);
		m_particle.first->operator~();
		particles.erase(particles.end() - 1);
	}
}

void ParticleEffect::refresh() {
	for (int i = 0; i < particles.size(); i++) {
		auto& m_particle = particles.at(particles.size() - 1);
		m_particle.first->operator~();
		particles.erase(particles.end() - 1);
	}
	for (int i = 0; i < amt; i++) {
		std::shared_ptr<Sprite> copy = m_particle_sprite->copy();
		copy->enabled = true;
		particles.push_back(std::make_pair(copy, new _stub{ .life = m_lifetime }));
	}
}

void ParticleEffect::Render() {
	amt = alib_clamp(amt, 0, 512);
	if (particles.size() > 512) {
		cwError::serrof("Max particles per object reached! Expected at most 512, got %d", particles.size());
		particles.resize(512);
	}
	for (int i = 0; i < particles.size(); i++) {
		std::pair<std::shared_ptr<Sprite>, _stub*>& m_particle = particles.at(i);
		m_particle.first->layer = m_layer;
		m_particle.first->uv = m_uv;
		m_particle.first->transform.scale = effect.size;
	}
	ImGuiIO io = ImGui::GetIO();
	ImVec2 m_direction = ImVec2(
		(1 * cos(m_dir_angle_rad)),
		(1 * sin(m_dir_angle_rad))
	) * io.DeltaTime;
	// transform pass
	for (int i = 0; i < particles.size(); i++) {
		std::pair<std::shared_ptr<Sprite>, _stub*>& m_particle = particles.at(i);
		m_particle.second->life += io.DeltaTime;
		m_particle.first->transform.position = m_particle.first->transform.position + (m_direction * (m_velocity * 100) / m_particle.second->velocity_randomness);
		float velocity_randomness = ((rand() % (int)(effect.velocity * 1000 + 1))) * 0.001f + 1;
		if (m_particle.second->life > m_lifetime * m_particle.second->life_randomness) {
			m_particle.second->velocity_randomness = velocity_randomness;
			float random_x = ((rand() % ((int)m_randomness.x == 0 ? 1 : (int)m_randomness.x * 200)) * 0.01f);
			float random_y = ((rand() % ((int)m_randomness.y == 0 ? 1 : (int)m_randomness.y * 200)) * 0.01f);
			m_particle.first->transform.position = (start_pos + ImVec2(random_x, random_y));
			m_particle.second->life = 0;
			float lr = (rand() % 100) * 0.002f;
			m_particle.second->life_randomness = lr;
		}
	}
}


/*
* \brief Draws to screen immediately. You can run this, or let the batch renderer do its thing. Either works.
*
*/


// This isn't the most optimal way, but I'm not sure of how to grant ownership through this.
std::shared_ptr<Sprite> Sprite::get_ptr() {
	return self;
}

Sprite::Sprite() {
	_mglobalspritearr.push_back(std::make_shared<Sprite>(std::move(*this)));
	rect = { 0,0,32,32 };
}

void Sprite::unlockCamera() {
	setbitv(params, 0, 0);
}

void Sprite::lockCamera() {
	setbitv(params, 0, 1);
}

int Sprite::calc_uv_pair(int from, int base, int amt) {
	if (from == 0) { return base * amt; }
	if (amt == 0) { return from; }
	return from + ((base * (amt)) - base);
}

void Sprite::Update() {
	if (meta.frames > 0 && (meta.animate || meta.animate_once)) {
		if (meta.animate_once) {
			meta.animate = true;
		}
		if (meta.DELTA > meta.delay) {
			if (meta.animate_once && meta.CURRENT_FRAME == meta.frames) {
				meta.animate = false;
				meta.animate_once = false;
				if (!meta.reset_after_once) {
					meta.CURRENT_FRAME = meta.frames;
				}
				else {
					meta.CURRENT_FRAME = 1;
				}
			}
			else {
				meta.CURRENT_FRAME = meta.CURRENT_FRAME == meta.frames ? 1 : meta.CURRENT_FRAME + 1;
			}
			uv_tile.y = meta.CURRENT_FRAME;
			meta.DELTA = 0;
		}
		ImGuiIO& io = ImGui::GetIO();
		meta.DELTA += io.DeltaTime;
	}
	else if (meta.CURRENT_FRAME > 0) {
		uv_tile.y = meta.CURRENT_FRAME;
	}
	uv_final.x = calc_uv_pair(uv.x, uv.w, uv_tile.x);
	uv_final.y = calc_uv_pair(uv.y, uv.h, uv_tile.y);
	uv_final.w = uv.w;
	uv_final.h = uv.h;
	if (center) {
		_offset = ImVec2(
			Camera::GetInstance()->m_VirtualViewport.x * 0.5f,
			Camera::GetInstance()->m_VirtualViewport.y * 0.5f
		) + _cameraViewOffset + const_offset + Camera::GetInstance()->m_Offset;
	}
	else {
		_offset = Camera::GetInstance()->pointToScreenViewport(transform.position) + Camera::GetInstance()->pointToWorldViewport({ 0,0 });
	}
	rect.x = (int)_offset.x * Camera::GetInstance()->getScaleFactor();
	rect.y = (int)_offset.y * Camera::GetInstance()->getScaleFactor();
	rect.w = abs(transform.scale.x);
	rect.h = abs(transform.scale.y);
	if (center) {
		rect.x = (int)_offset.x - (transform.scale.x / 2);
		rect.y = (int)_offset.y - (transform.scale.y / 2);
	}
	// To center the actual sprite, we offset it by -half size
	center_position = ImVec2(
		rect.x + rect.w / 2,
		rect.y + rect.h / 2
	);

	if (!rect.empty()) {
		pixel_size.x = transform.scale.x / this->rect.w;
		pixel_size.y = transform.scale.x / this->rect.h;
	}
	Rect2d bounds;
	// If overlay isn't visible, then cull
	if (Camera::GetInstance()->culling) {
		// 2 pixels padding
		bounds = Rect2d::im(
			{ Camera::GetInstance()->pointToScreenViewport({0,0}) },
			{ Camera::GetInstance()->pointToScreenViewport(Camera::GetInstance()->m_VirtualViewport) }
		);
		rect = Rect2d::im(Camera::GetInstance()->pointToScreenViewport({ rect.x, rect.y }), Camera::GetInstance()->pointToScreenViewport(ImVec2{ rect.x + rect.w, rect.y + rect.h }));
		isRendering = rectIntersectsRect(rect, bounds);
		ImGui::GetBackgroundDrawList()->AddRect(bounds.min(), bounds.max(), ImColor{.6,.5f,.1f});
		ImGui::GetBackgroundDrawList()->AddText(rect.min(), ImColor{1,1,1}, isRendering ? "visible" : "culled");
		ImGui::GetBackgroundDrawList()->AddRect(rect.min(), rect.max(), ImColor{1.,.5f,1.});
	}
}

void Sprite::Draw() {
	if (!enabled && !manualDraw) { return; }
	Update();
	// cull images outside of viewport
	if (!isRendering) {
		return;
	}
	isRendering = true;
	ImVec2 _origin = transform.origin;
	if (&uv == nullptr || (&uv && uv.empty()))
	{
		//LOGI("%.2f, %.2f x %.2f, %.2f", rect.x, rect.y, rect.w, rect.h);
		//ImGui::GetBackgroundDrawList()->AddImage(Texture, { cent.x, rect_final.y }, { rect.x + rect.w, rect.y + rect.h }, { 0,0 }, {1, 1}, tint);
		// TODO: render using UV
	}
	else {
		//LOGI("U %.2f, %.2f x %.2f, %.2f", rect.x, rect.y, rect.w, rect.h);
		ImGui::GetBackgroundDrawList()->AddImage(*Texture,rect.min(), rect.max(),
			//{ uv_final.x, uv_final.y }, { uv_final.x + uv_final.w, uv_final.y + uv_final.h }, 
			{ 0,0 }, { 1, 1 });
		//ImGui::ForegroundImagePositioned(Texture, { rect.x, rect.y }, { rect.w, rect.h }, { 0,0 }, { 1,1 });
		// TODO: render entire sprite
	}
}
void Sprite::resetCache() {
	surfFilemaps.clear();
	// Sprite array persists through loads.. somehow
	/*for (std::shared_ptr<Sprite> s : _mglobalspritearr) {
		if (surfFilemaps.find(s->filename) == surfFilemaps.end()) {
			s->Texture = new rsm::ImageAsset();
			s->Texture->LoadFromAPK(s->filename.c_str());
			surfFilemaps.emplace(s->filename, s->Texture);
		} else {
			s->Texture = surfFilemaps.at(s->filename);
		}
	}*/
}
Sprite::Sprite(const char* filename, Rect2d spriteuv) {
	name = filename;
	this->filename = filename;
	uv = spriteuv;
	layer = 0;
	self = std::make_shared<Sprite>(std::move(*this));
	_mglobalspritearr.push_back(self);
	_cameraViewOffset = ImVec2(0, 0);
	unlockCamera();
	if (surfFilemaps.find(filename) == surfFilemaps.end()) {
		Texture = std::make_shared<rsm::ImageAsset>();
		Texture->LoadFromAPK(filename);
		surfFilemaps.emplace(filename, Texture);
		cwError::serrof("Sprite created, and saved texture!");
	}
	else {
		Texture = surfFilemaps.at(filename);
	}
	if (uv.x == 0 && uv.y == 0 && uv.w == 0 && uv.h == 0) {
		uv = { 0,0, (float)Texture->width, (float)Texture->height };
	}
	transform.scale = ImVec2(uv.w, uv.h);

	return;
}

void Sprite::setID(std::string i) {
	this->identifier = i;
}

std::string Sprite::getID() { return this->identifier; }

bool Sprite::eraseElementByID(std::string id) {
	for (int i = 0; i < _mglobalspritearr.size(); i++) {
		std::shared_ptr<Sprite> _s = _mglobalspritearr[i];
		if (_s->isPrivate) { continue; }
		if (alib_streq(id, _s->identifier.c_str())) {
			_mglobalspritearr.erase(_mglobalspritearr.begin() + i);
			_s->operator~();
			return true;
		}
	}
	return false;
}

std::shared_ptr<Sprite> Sprite::getElementByID(std::string id) {
	for (int i = 0; i < _mglobalspritearr.size(); i++) {
		std::shared_ptr<Sprite> _s = _mglobalspritearr[i];
		if (_s->isPrivate) { continue; }
		if (alib_streq(id, _s->identifier.c_str())) {
			return _s;
		}
	}
	return nullptr;
}

void Sprite::insertElementWithID(std::string id, std::shared_ptr<Sprite> s) {
	s->setID(id);
	_mglobalspritearr.push_back(s);
}

std::shared_ptr<Sprite> Sprite::copy() {
	Sprite s;
	s.transform = this->transform;
	s.uv = this->uv;
	s.enabled = true;
	s.setID(this->getID() + " copy");
	s.layer = this->layer;
	s.name = this->name;
	s.rect = this->rect;
	s.Texture = Texture;
	return s.self;
}

void Sprite::insert() {
	_mglobalspritearr.push_back(self);
}

void Sprite::remove() {
	eraseElementByID(this->identifier);
}

void Sprite::lua_set_uv(float x, float y, float w, float h) {
	this->uv = { x,y,w,h };
}

Rect2d Sprite::lua_get_uv() { return this->uv; }

lua_Number Sprite::lu_getLayer() { return (this->layer); }

ImVec2 Sprite::getPosition() {
	return transform.position;
}

double Sprite::getRotation() {
	return transform.angle;
}

ImVec2 Sprite::getScale() {
	return transform.scale;
}

void Sprite::Scale(ImVec2 scaleAmt) {
	this->transform.scale.x *= scaleAmt.x;
	this->transform.scale.y *= scaleAmt.y;
}

void Sprite::Move(ImVec2 moveAmt) {
	this->transform.position.x += moveAmt.x;
	this->transform.position.y += moveAmt.y;
}

void Sprite::SetTransform(Transform _transform) {
	this->transform = _transform;
}

void Sprite::Rotate(int amt) {
	this->transform.angle += amt;
}

void Sprite::SetRotation(int amt) {
	this->transform.angle = amt;
}

void Sprite::SetPosition(ImVec2 amt) {
	this->transform.position = amt;
}

void Sprite::setEnabled(bool val) {
	this->enabled = val;
}

void SpriteAnimationMeta::set_loop_func(std::function<void()> callback) {
	OnLoopOrReset = callback;
}

SpriteAnimationMeta::SpriteAnimationMeta() { }

SpriteAnimationMeta::SpriteAnimationMeta(int n_frames, float m_delay) {
	this->frames = n_frames;
	this->delay = m_delay;
}
