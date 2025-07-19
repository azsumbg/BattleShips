#pragma once

#ifdef BATTLE_EXPORTS
#define BATTLE_API __declspec(dllexport)
#else
#define BATTLE_API __declspec(dllimport)
#endif

constexpr float scr_width{ 1000.0f };
constexpr float scr_height{ 800.0f };

constexpr int MAX_COLS{ 18 };
constexpr int MAX_ROWS{ 15 };

namespace dll
{
	enum class BATTLE_API dirs { vert = 0, hor = 1 };
	
	enum class BATTLE_API ships { small_ship = 0, mid_ship1 = 1, mid_ship2 = 2, 
		big_ship1 = 3, big_ship2 = 4, min_ship = 5,
	};

	enum class BATTLE_API content { free = 0, used = 1, near_ship = 2, explosion = 3, fire = 4 };

	struct BATTLE_API FPOINT
	{
		float x{ 0 };
		float y{ 0 };
	};
	struct BATTLE_API TILE
	{
		FPOINT start{};
		FPOINT end{};

		int number = 0;
		int col{ 0 };
		int row{ 0 };
		
		bool bombarded = false;

		content state = content::free;
	};

	class BATTLE_API GRID
	{
	private:
		float width{ 50.0f };
		float height{ 50.0f };

	public:
		TILE grid[18][15]{};

		GRID();

		FPOINT GetTileDims(int tile_number);
		int GetTileNumber(FPOINT position);
		int GetTileCol(float x_position) const;
		int GetTileRow(float y_position) const;
	
		bool IsAvailable(int tile_number)const;
	
		void Release();
	};

	class BATTLE_API SHIP
	{
	private:
		ships type{ ships::min_ship };
		
		int ship_parts_alive = 1;

		void sort_tiles();

	public:
		TILE ship_tile[4]{};
		dirs dir = dirs::hor;

		SHIP(ships _type, TILE* _ship_tile, int size, dirs _to_where);

		int ship_healt() const;
		void hit_ship(int which_part);
		ships get_type()const;

		void Release();

		friend BATTLE_API SHIP* ShipFactory(ships what, TILE* myTiles, int size, dirs where, GRID& host_grid);
	};

	BATTLE_API void SetNearTiles(GRID& set_grid, TILE* ship, int size);

	BATTLE_API SHIP* ShipFactory(ships what, TILE* myTiles, int size, dirs where, GRID& host_grid);

	typedef SHIP* Ship;

	typedef TILE* Tile;
}