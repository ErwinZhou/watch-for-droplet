#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <array>
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

	//----- pickups -----
	//fixed composition: 2 small stars, 1 bright star, 6 small meteorites, 1 large
	// kind is set once in the constructor and never changes, so the sprite cost is constant
	struct Pickup {
		enum class Kind : uint8_t { SmallStar, BrightStar, SmallMeteorite, LargeMeteorite };
		Kind kind = Kind::SmallStar;
		glm::vec2 at = glm::vec2(0.0f);
	};
	std::array< Pickup, 10 > pickups;

	//art + size for a pickup kind, looked up from asset.hpp
	struct PickupArt {
		uint8_t tile_index = 0;
		uint8_t const *palettes = nullptr;
		uint32_t tiles_x = 0;
		uint32_t tiles_y = 0;
		float width = 0.0f;
		float height = 0.0f;
	};
	static PickupArt art_for(Pickup::Kind kind);

	//----- round state -----
	//a round is a 60s score run, catch as many ships as possible!
	static constexpr float RoundSeconds = 60.0f;
	static constexpr float CatchFlashSeconds = 0.2f;
	float time_remaining = RoundSeconds;
	uint32_t ships_caught = 0;
	float catch_flash = 0.0f; //seconds of green tint left after a catch
	bool won = false;
	bool lost = false;

	//deterministic
	// The answer to life the universe and everything: 42
	std::mt19937 mt = std::mt19937(0x2a);

	//----- game helper functions to update states
	static float get_speed(Speed speed);
	static void clamp_to_screen(glm::vec2 &at, float w, float h);
	void update_ship(float elapsed);
	void respawn(Pickup &p);
	void respawn_ship();
	void reset_round();
	void apply_pickup(Pickup::Kind kind, Speed &speed);
	void update_pickups();

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
