/*
 * This file is part of the Simutrans project under the Artistic License.
 * (see LICENSE.txt)
 */

#ifndef BODEN_WEGE_STRASSE_H
#define BODEN_WEGE_STRASSE_H


#include "weg.h"

// number of different traffic directions
#define MAX_WAY_STAT_DIRECTIONS 2

class vehicle_base_t;

/**
 * Cars are able to drive on roads.
 */
class strasse_t : public weg_t
{
public:
	static bool show_masked_ribi;
	static bool show_reservations;
	
	enum {
		AVOID_CITYROAD   = 0x01, // this street avoid becoming cityroad.
		CITYCAR_NO_ENTRY = 0x02, // citycar cannot enter this road.
		USE_GIVEN_HEIGHT = 0x04  // this flag is used only for construction
	};

private:
	/**
	* @author THLeaderH
	*/
	overtaking_mode_t overtaking_mode;

	/**
	* Mask used by oneway_mode road
	* @author THLeaderH
	*/
	uint8 ribi_mask_oneway:4;
	
	/**
	* strasse_t specific flags
	* @author THLeaderH
	*/
	uint8 street_flags;

	/**
	* 0 = calculate automatically
	* 1 = north-south traffic has priority
	* 2 = east-west traffic has priority
	* @author THLeaderH
	*/
	uint8 prior_direction_setting;

	/**
	* array for statistical values
	* store directional statistics to calculate prior_direction
	* direction: 0 = north-south, 1 = east-west
	*/
	sint16 directional_statistics[MAX_WAY_STAT_MONTHS][MAX_WAY_STATISTICS][MAX_WAY_STAT_DIRECTIONS];

	void init_statistics() OVERRIDE;
	
	/**
	* tile reservation system
	* to prevent a grid-lock in an intersection...
	* This does not support citycars!
	* A tile is devided into 4 small rectangles.
	*        N
	*   -----------
	*  |     |     |
	*  |  0  |  1  |
	* W|-----|-----|  E
	*  |  2  |  3  |
	*  |     |     |
	*   -----------
	*        S
	*
	* @author THLeaderH
	*/
	vehicle_base_t* reserved_by[4];

public:
	static const way_desc_t *default_strasse;

	strasse_t(loadsave_t *file);
	strasse_t();

	inline waytype_t get_waytype() const OVERRIDE {return road_wt;}

	void set_gehweg(bool janein);

	void rdwr(loadsave_t *file) OVERRIDE;

	/**
	* Overtaking mode (declared in simtypes.h)
	* halt_mode = vehicles can stop on passing lane
	* oneway_mode = condition for one-way road
	* twoway_mode = condition for two-way road
	* loading_only_mode = overtaking a loading convoy only
	* prohibited_mode = overtaking is completely forbidden
	* inverted_mode = vehicles can go only on passing lane
	* @author teamhimeH
	*/
	overtaking_mode_t get_overtaking_mode() const { return overtaking_mode; };
	void set_overtaking_mode(overtaking_mode_t o) { overtaking_mode = o; };

	void set_ribi_mask_oneway(ribi_t::ribi ribi) { ribi_mask_oneway = (uint8)ribi; }
	// used in wegbauer. param @allow is ribi in which vehicles can go. without this, ribi cannot be updated correctly at intersections.
	void update_ribi_mask_oneway(ribi_t::ribi mask, ribi_t::ribi allow);
	ribi_t::ribi get_ribi_mask_oneway() const { return (ribi_t::ribi)ribi_mask_oneway; }
	virtual ribi_t::ribi get_ribi() const OVERRIDE;

	virtual void rotate90() OVERRIDE;

	void book(int amount, way_statistics type, ribi_t::ribi dir);
	void new_month() OVERRIDE;

	image_id get_front_image() const OVERRIDE;
	
	virtual FLAGGED_PIXVAL get_outline_colour() const OVERRIDE;
	/*
	 * to show reservations if needed
	 */
	virtual image_id get_outline_image() const OVERRIDE { return weg_t::get_image(); }

	// related to tile reservation system
	// return true if succeeded
	bool reserve(vehicle_base_t* r, bool is_overtaking, koord3d pos_prev, koord3d pos_next);
	bool unreserve(vehicle_base_t* r);
	void unreserve_all();
	bool is_reserved_by_others(vehicle_base_t* r, bool is_overtaking, koord3d pos_prev, koord3d pos_next);
	
	uint8 get_street_flag() const { return street_flags; }
	void set_street_flag(uint8 s) { street_flags = s; }
	bool get_avoid_cityroad() const { return street_flags&AVOID_CITYROAD; }
	void set_avoid_cityroad(bool s) { s ? street_flags |= AVOID_CITYROAD : street_flags &= ~AVOID_CITYROAD; }
	bool get_citycar_no_entry() const { return street_flags&CITYCAR_NO_ENTRY; }
	void set_citycar_no_entry(bool s) { s ? street_flags |= CITYCAR_NO_ENTRY : street_flags &= ~CITYCAR_NO_ENTRY; }

};

#endif
