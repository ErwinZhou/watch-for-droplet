#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <random>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- units -----
	// All game state below is in PIXELS, stored as float
	// Tiles and uint8_t appear only at the PPU boundary
	static constexpr float TileSize = 8.0f; //one tile is 8x8 pixels

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//some weird background animation:
	float background_fade = 0.0f;

	//----- player state -----
	enum class Speed {
		Normal,
		Accelerated,
		Lightspeed
	};
	struct Player {
		glm::vec2 droplet_at = glm::vec2(60.0f, 150.0f);
		Speed droplet_speed = Speed::Normal;
		float width = 0.0f;
		float height = 0.0f;
		void speed_up(bool all_the_way);
		void speed_down(bool all_the_way);
	} droplet;

	//----- NPC state -----
	struct Spacecraft {
		glm::vec2 ship_at = glm::vec2(180.0f, 40.0f);
		Speed ship_speed = Speed::Accelerated;
		float width = 0.0f;
		float height = 0.0f;
		glm::vec2 direction = glm::vec2(1.0f, 0.0f); //unit vector
		float direction_timer = 0.0f; //seconds left before re-rolling direction
		void speed_up() { ship_speed = Speed::Accelerated; return; }
		void speed_down() { ship_speed = Speed::Normal; return; }
	} ship;

	//deterministic
	// The answer to life the universe and everything: 42
	std::mt19937 mt = std::mt19937(0x2a);

	//----- game helper functions to update states
	static float get_speed(Speed speed);
	static void clamp_to_screen(glm::vec2 &at, float w, float h);
	void update_ship(float elapsed);

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
