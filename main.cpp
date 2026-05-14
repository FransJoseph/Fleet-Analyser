#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <random>
#include <algorithm>
#include <ctime>
#include <random>
#include <array>

using namespace std;

struct Ship {
    string name;
    long ship_class = 8; //0 - convoy, 1 - destroyer, 2 - light cruiser, 3 - heavy cruiser, 4 - battlecruiser, 5 - battleship, 6 - super heavy battleship, 7 - light carrier, 8 - aircraft carrier, 9 - light submarine, 10 - heavy submarine
    double ship_production_cost;
    double max_ORG;
    double ORG=max_ORG;
    double max_HP;
    double HP=max_HP;
    double speed;
    int ship_manpower;
    double reliability;
    double ship_XP;    //0-100 Fresh, 100-300 Regular, 300-750 Trained, 750-900 Seasoned, 900-1000 Veterans

    long deck_size;
    long fighter_amount;
    long bomber_amount;

    double light_attack;
    double light_piercing;
    double heavy_attack;
    double heavy_piercing;
    double torpedo_attack;
    double ship_armor;
    double ship_AA;

    double surface_visibility;
    double sub_visibility;
    double surface_detection;
    double sub_detection;

    long light_batteries;   //Amount of light batteries, larger cruiser type light batteries also apply, all light batteries mounted on heavy cruisers also count, those mounted on "secondaries" slot on light cruisers also apply here, dual purpose main battery also counts (wiki states that batteries on screens counts here, without such details)
    long heavy_batteries;   //Medium ones also counts (named accordingly heavy/medium batteries in the wiki)
    long secondary_batteries;   //Dual-purpose ones also counts, light batteries mounted on light cruisers on "secondaries" slot does not count here
    long torpedo_tubes;
    array<double, 10> critical_hits = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  //Array that stores critical hits sustained by ship (and value of damage done)
};

struct Plane {
    string name;
    double air_attack;
    double air_defence;
    double agility;
    double speed;
    double naval_attack;    //Just naval bomber naval attack
    double naval_targeting;
    double night_operations_penalty;
    double XP;   //0-100 Rockies, 100-300 Regular, 300-700 Trained, 700-900 Seasoned, 900-1000 Veterans
    double production_cost;
};

class Fleet {
public:
    Ship ship;
    Plane cvFighter;    //Carrier based Fighter
    Plane cvBomber;     //Carrier based Naval Bomber

    string tag;    //Country name aka "tag"

    double dockyard_output;
    double research_speed;
    double refill_cost;

    double sortie_efficiency;
    double fighters_sortie_efficiency;

    double naval_damage_modifier;
    double naval_defence_modifier;

    double chance_to_score_critical_hit_modifier;
    double chance_to_receive_critical_hit;
    double effect_on_sustained_critical_hit_modifier;

    double naval_targeting_modifier;
    double naval_agility;
    double naval_air_attack_from_carriers;
    double naval_air_targeting_from_carriers;
    double naval_air_agility_from_carriers;

    //Individual battle specific modifiers
    double positioning; //Naval positioning in battles
    double battle_line_screening;   //Screening of Capital Ships provided by Screen Ships
    double carrier_screening;   //Screening of Carriers & Convoys provided by Capital Ships
    double carrier_traffic;
    double retreat_decision_chance = 0;
    double ship_recovery_rate = 0.20;

    //Modifiers to be moved
    long fighters_in_the_air;
    long naval_bombers_in_the_air;
    double fighters_destroyed_by_friendly_planes;
    double naval_bombers_destroyed_by_friendly_planes;
    double battle_sortie_eff;   //Sortie efficiency in battle
    double battle_fighters_sortie_efficiency;   //Fighters sortie efficiency in battles
    bool RNGResult; //Was RNG based action successful or not?
    double disruption_by_interception;  //Disruption caused by presence of enemy fighters, it is assigned to country that generated it
    double disruption_reduction_ratio;  //Reduction of previous disruption by escorting fighters, it reduces enemy disruption (assigned to them)
    double disruption;     //Final disruption that was also reduced by Naval Bomber stats
    double detection = 1.0;
    double air_attack_modifier = 0;
    double agility_modifier = 0;
    double night_operations_penalty_modifier = 0;
    double XPprogress;  //% of max buffs or de-buffs that air wing receives from XP
    double xp_naval_targeting;
    double xp_air_attack;
    double xp_agility;
    double xp_night_penalty;   //Night operations penalty modifier
    long escorted_bombers;  //Bombers that survived air combat phase, ship AA and was not forced to return to carrier due to disruption caused by enemy fighters

};

class Battle : public Fleet {
public:

    long hour = 1;   //1-24
    long day = 1;
    bool IsDay = false;   //Day = 1, Night = 0
    long weather = 0;    //0 - clear, 1 - rain, 2 - storm, 3 - sandstorm

    Fleet A;   //Variable describing fleet A (Attacker)
    Fleet D;   //Variable describing fleet A (Defender)

    Fleet* aa;
    Fleet* dd;

    static void fileReading (bool overload, Fleet *a, Fleet *d);  //Reading variables from files, if overload = true, file will only load certain variables to restore some overloaded values like: "cvFighter.air_attack"

    void applyXP (Fleet *a, Fleet *d);  //Applying effects of XP & modifiers
    static array<double, 9> applyCriticalHits (array<double, 10> critical_hits, long ship_class);
    void sortieDeployment (Fleet *a, Fleet *d) const;   //Calculating amount of planes in the air
    static double airCombat (double detection, long attacking_planes, long fighters_visibility, long bombers_visibility, double air_attack, double air_defence, double attacker_agility, double defender_agility, double attacker_speed, double defender_speed, bool target_bomber);
    static double airXPGain (double detection, long attacking_planes, long fighters_visibility, long bombers_visibility, double sortie_eff, double carr_traffic, bool target_bomber);
    bool RNG (double wantedChance);  //Beloved RNG
    long shipAADefence (double bomber_agility, long bombers_amount, double shipAA);    //Calculating planes shoot by ship AA
    static double finalDisruption (double disruption_by_interception, double air_defence, double air_attack, double speed, long total_bombers); //Calculating disruption damage factor

    void airBattle (Fleet *a, Fleet *d);  //Calculating damage done to both types of enemy planes by interception, by ship AA and planes disrupted by enemy

    static long strikeTargeting (long bombers, double targeting);
    static double shipAADamageReduction (double target_AA, double fleet_AA);
    static double RNGSimple (double max_number);   //RNG with maximum number possible to be drawn being flexible
    static long criticalHits (long ship_class, long light_batteries, long heavy_batteries, long secondary_batteries, long torpedo_tubes, array<double, 10> critical_hits);
    static array<double,10> bomberDamage (double naval_attack, double AA_damage_reduction, double critical_hit_chance, double chance_to_receive_critical_hit, double target_reliability, double effects_on_critical_hit, long ship_class, long light_batteries, long heavy_batteries, long secondary_batteries, long torpedo_tubes, array<double, 10> critical_hits);

    static void carrierStrike (Fleet *a, Fleet *d);

    Battle() {

        aa = &A;
        dd = &D;

        fileReading(false, aa, dd);    //Reading from files to variables

        while (day <= 2) {
            while (hour <= 24) {
                if (hour == 6) { IsDay = true; cout<<"\nThere is is day now\n"; }
                if (hour == 18) { IsDay = false; cout<<"\nThere is night now\n"; }
                if (hour%8 == 0) {
                    fileReading(true, aa, dd);    //Restoring overloaded variables
                    applyXP(aa, dd);  //Applying XP
                    sortieDeployment(aa, dd);   //Calculating amount of planes in the air
                    airBattle(aa, dd);  //Calculating damage done to both types of enemy planes in the air
                    carrierStrike(aa, dd);      //Calculating damage done to enemy ship by planes
                } hour++;
            } hour=1; day++;
        }
    }

};

bool Battle::RNG (const double wantedChance) { //Beloved RNG

    random_device                  rand_dev;
    mt19937                        generator(rand_dev());
    uniform_int_distribution<int>  distr(0, 1);

    if (distr(generator) < wantedChance) { RNGResult = true; } else { RNGResult = false; }
    return RNGResult;   //Hoi4 probably uses RNG with "<" as CHANCE_TO_DAMAGE_PART_ON_CRITICAL_HIT description in defines suggests, despite "<=" being slightly more accurate

}

double Battle::RNGSimple (const double max_number) {    //"Simple" because it returns number, rather than result of action. To be clear - it is more complex to be more accurate

    //Type of random number distribution
    std::uniform_real_distribution<double> dist(0, max_number);  //(min, max)

    //Mersenne Twister: Good quality random number generator
    std::mt19937 rng;
    //Initialize with non-deterministic seeds
    rng.seed(random_device{}());

    return dist(rng);
}

array<double, 9> Battle::applyCriticalHits (const array<double, 10> critical_hits, const long ship_class) {

    double heavy_attack_modifier = 1;
    double light_attack_modifier = 1;
    double torpedo_attack_modifier = 1;
    double naval_speed_modifier = 1;
    double retreat_decision_chance = 1;
    double ship_ORG_modifier = 1;
    double ship_recovery_rate = 1;
    double sub_visibility_modifier = 1;
    double sortie_efficiency_modifier = 1;

    if (critical_hits[0] == 1) heavy_attack_modifier -= 0.7; light_attack_modifier -= 0.7; torpedo_attack_modifier -= 0.7;  //Magazine Hit
    if (critical_hits[1] == 1) naval_speed_modifier -= 0.5; retreat_decision_chance -= 0.9; //Rudder Jammed
    if (critical_hits[2] == 1) naval_speed_modifier -= 0.9; //Broken Propeller
    if (critical_hits[3] == 1) ship_ORG_modifier -= 0.5; ship_recovery_rate -= 0.8;    //Heavy Fires
    if (critical_hits[4] == 1) if (ship_class > 8) sub_visibility_modifier += 0.66; else sortie_efficiency_modifier -= 0.5;    //Ballast Tanks Inoperable/Flight Deck Damaged
    if (critical_hits[5] > 0) light_attack_modifier -= 0.5 * critical_hits[5]; //Main Battery Mount Disabled
    if (critical_hits[6] > 0) heavy_attack_modifier -= 0.5 * critical_hits[6]; //Main Battery Turret Destroyed
    if (critical_hits[7] > 0) light_attack_modifier -= 0.25 * critical_hits[7]; //Secondaries Knocked Out
    if (critical_hits[8] > 0) torpedo_attack_modifier -= 0.5 * critical_hits[8]; //Torpedo Tubes Disabled

    const array<double, 9> modifiers = {heavy_attack_modifier, light_attack_modifier, torpedo_attack_modifier, naval_speed_modifier, retreat_decision_chance, ship_ORG_modifier, ship_recovery_rate, sub_visibility_modifier, sortie_efficiency_modifier};

    return modifiers;
};

void Battle::applyXP (Fleet *a, Fleet *d) {

    //Planes

    //Reset to base values
    a->cvFighter.naval_targeting = 10;
    a->cvBomber.naval_targeting = 10;
    a->cvFighter.night_operations_penalty = 1;
    a->cvBomber.night_operations_penalty = 1;

    d->cvFighter.naval_targeting = 10;
    d->cvBomber.naval_targeting = 10;
    d->cvFighter.night_operations_penalty = 1;
    d->cvBomber.night_operations_penalty = 1;

    //XP cap
    a->cvFighter.XP = min(a->cvFighter.XP, 1000.0);
    a->cvBomber.XP = min(a->cvBomber.XP, 1000.0);
    d->cvFighter.XP = min(d->cvFighter.XP, 1000.0);
    d->cvBomber.XP = min(d->cvBomber.XP, 1000.0);

    a->ship.ship_XP = min(a->ship.ship_XP, 1000.0);
    d->ship.ship_XP = min(a->ship.ship_XP, 1000.0);

    //Attacker

    //Fighters
    if (a->cvFighter.XP < 100) { a->XPprogress = a->cvFighter.XP / 100 - 1; a->xp_naval_targeting = 0.05 * a->XPprogress; a->xp_air_attack = 0.1 * a->XPprogress; a->xp_agility = 0.1 * a->XPprogress; a->xp_night_penalty = -0.1 * a->XPprogress; } //Rockies (0-100), it gives de-buffs (hence "- 1")
    if (a->cvFighter.XP >= 100 && a->cvFighter.XP < 300) { min( (a->XPprogress = a->cvFighter.XP - 100) / 200, 1.0 ); a->xp_naval_targeting = 0.01 * a->XPprogress; a->xp_air_attack = 0.03 * XPprogress; a->xp_agility = 0.04 * XPprogress; a->xp_night_penalty = -0.03 * XPprogress; }   //Regular (100-300) - bonuses starts in trained level and ends with regular
    if (a->cvFighter.XP >= 300 && a->cvFighter.XP < 700) { min( (a->XPprogress = a->cvFighter.XP - 100) / 600, 1.0 ); a->xp_naval_targeting = 0.03 * a->XPprogress; a->xp_air_attack = 0.10 * XPprogress; a->xp_agility = 0.14 * XPprogress; a->xp_night_penalty = -0.105 * XPprogress; }
    if (a->cvFighter.XP >= 700) { min( (XPprogress = cvFighter.XP - 100) / 800, 1.0 ); a->xp_naval_targeting = 0.05 * a->XPprogress; a->xp_air_attack = 0.15 * XPprogress; a->xp_agility = 0.20 * XPprogress; a->xp_night_penalty = -0.15 * XPprogress; }
    a->cvFighter.air_attack *= 1 + a->air_attack_modifier + a->naval_air_attack_from_carriers + a->xp_air_attack;
    a->cvFighter.agility *= 1 + a->agility_modifier + a->naval_air_agility_from_carriers + a->xp_agility;
    a->cvFighter.night_operations_penalty -= a->night_operations_penalty_modifier + a->xp_night_penalty;

    //Naval Bombers
    if (a->cvBomber.XP < 100) { a->XPprogress = a->cvBomber.XP / 100 - 1; a->xp_naval_targeting = 0.05 * a->XPprogress; a->xp_air_attack = 0.1 * a->XPprogress; a->xp_agility = 0.1 * a->XPprogress; a->xp_night_penalty = -0.1 * a->XPprogress; } //Rockies (0-100), it gives de-buffs (hence "- 1")
    if (a->cvBomber.XP >= 100  && a->cvBomber.XP < 300) { min( (a->XPprogress = a->cvBomber.XP - 100) / 200, 1.0 ); a->xp_naval_targeting = 0.01 * a->XPprogress; a->xp_air_attack = 0.03 * XPprogress; a->xp_agility = 0.04 * XPprogress; a->xp_night_penalty = -0.03 * XPprogress; }   //Regular (100-300) - bonuses starts in trained level and ends with regular
    if (a->cvBomber.XP >= 300 && a->cvBomber.XP < 700) { min( (a->XPprogress = a->cvBomber.XP - 100) / 600, 1.0 ); a->xp_naval_targeting = 0.03 * a->XPprogress; a->xp_air_attack = 0.10 * XPprogress; a->xp_agility = 0.14 * XPprogress; a->xp_night_penalty = -0.105 * XPprogress; }
    if (a->cvBomber.XP >= 700) { min( (XPprogress = cvBomber.XP - 100) / 800, 1.0 ); a->xp_naval_targeting = 0.05 * a->XPprogress; a->xp_air_attack = 0.15 * XPprogress; a->xp_agility = 0.20 * XPprogress; a->xp_night_penalty = -0.15 * XPprogress; }
    a->cvBomber.naval_targeting *= 1 + a->naval_targeting_modifier + a->naval_air_targeting_from_carriers + a->xp_naval_targeting;
    a->cvBomber.air_attack *= 1 + a->air_attack_modifier + a->naval_air_attack_from_carriers + a->xp_air_attack;
    a->cvBomber.agility *= 1 + a->agility_modifier + a->naval_air_agility_from_carriers + a->xp_agility;
    a->cvBomber.night_operations_penalty -= a->night_operations_penalty_modifier + a->xp_night_penalty;
    a->cvBomber.naval_attack *= 1 + a->naval_air_attack_from_carriers;

    //Defender

    //Fighters
    if (d->cvFighter.XP < 100) { d->XPprogress = d->cvFighter.XP / 100 - 1; d->xp_naval_targeting = 0.05 * d->XPprogress; d->xp_air_attack = 0.1 * d->XPprogress; d->xp_agility = 0.1 * d->XPprogress; d->xp_night_penalty = -0.1 * d->XPprogress; } //Rockies (0-100), it gives de-buffs (hence "- 1")
    if (d->cvFighter.XP >= 100 && d->cvFighter.XP < 300) { min( (d->XPprogress = d->cvFighter.XP - 100) / 200, 1.0 ); d->xp_naval_targeting = 0.01 * d->XPprogress; d->xp_air_attack = 0.03 * XPprogress; d->xp_agility = 0.04 * XPprogress; d->xp_night_penalty = -0.03 * XPprogress; }   //Regular (100-300) - bonuses starts in trained level and ends with regular
    if (d->cvFighter.XP >= 300 && d->cvFighter.XP < 700) { min( (d->XPprogress = d->cvFighter.XP - 100) / 600, 1.0 ); d->xp_naval_targeting = 0.03 * d->XPprogress; d->xp_air_attack = 0.10 * XPprogress; d->xp_agility = 0.14 * XPprogress; d->xp_night_penalty = -0.105 * XPprogress; }
    if (d->cvFighter.XP >= 700) { min( (XPprogress = cvFighter.XP - 100) / 800, 1.0 ); d->xp_naval_targeting = 0.05 * d->XPprogress; d->xp_air_attack = 0.15 * XPprogress; d->xp_agility = 0.20 * XPprogress; d->xp_night_penalty = -0.15 * XPprogress; }
    d->cvFighter.air_attack *= 1 + d->air_attack_modifier + d->naval_air_attack_from_carriers + d->xp_air_attack;
    d->cvFighter.agility *= 1 + d->agility_modifier + d->naval_air_agility_from_carriers + d->xp_agility;
    d->cvFighter.night_operations_penalty -= d->night_operations_penalty_modifier + d->xp_night_penalty;

    //Naval Bombers
    if (d->cvBomber.XP < 100) { d->XPprogress = d->cvBomber.XP / 100 - 1; d->xp_naval_targeting = 0.05 * d->XPprogress; d->xp_air_attack = 0.1 * d->XPprogress; d->xp_agility = 0.1 * d->XPprogress; d->xp_night_penalty = -0.1 * d->XPprogress; } //Rockies (0-100), it gives de-buffs (hence "- 1")
    if (d->cvBomber.XP >= 100 && d->cvBomber.XP < 300) { min( (d->XPprogress = d->cvBomber.XP - 100) / 200, 1.0 ); d->xp_naval_targeting = 0.01 * d->XPprogress; d->xp_air_attack = 0.03 * XPprogress; d->xp_agility = 0.04 * XPprogress; d->xp_night_penalty = -0.03 * XPprogress; }   //Regular (100-300) - bonuses starts in trained level and ends with regular
    if (d->cvBomber.XP >= 300 && d->cvBomber.XP < 700) { min( (d->XPprogress = d->cvBomber.XP - 100) / 600, 1.0 ); d->xp_naval_targeting = 0.03 * d->XPprogress; d->xp_air_attack = 0.10 * XPprogress; d->xp_agility = 0.14 * XPprogress; d->xp_night_penalty = -0.105 * XPprogress; }
    if (d->cvBomber.XP >= 700) { min( (XPprogress = cvBomber.XP - 100) / 800, 1.0 ); d->xp_naval_targeting = 0.05 * d->XPprogress; d->xp_air_attack = 0.15 * XPprogress; d->xp_agility = 0.20 * XPprogress; d->xp_night_penalty = -0.15 * XPprogress; }
    d->cvBomber.naval_targeting *= 1 + d->naval_targeting_modifier + d->naval_air_targeting_from_carriers + d->xp_naval_targeting;
    d->cvBomber.air_attack *= 1 + d->air_attack_modifier + d->naval_air_attack_from_carriers + d->xp_air_attack;
    d->cvBomber.agility *= 1 + d->agility_modifier + d->naval_air_agility_from_carriers + d->xp_agility;
    d->cvBomber.night_operations_penalty -= d->night_operations_penalty_modifier + d->xp_night_penalty;
    d->cvBomber.naval_attack *= 1 + d->naval_air_attack_from_carriers;

    //Ships

    //Attacker
    if (a->ship.ship_XP < 100) { a->naval_damage_modifier -= 0.1; a->naval_defence_modifier -= 0.1; a->fighters_sortie_efficiency -= 0.1; }   //Fresh - lvl 1
    if (a->ship.ship_XP >= 300) { a->naval_damage_modifier += 0.033; a->naval_defence_modifier += 0.033; a->fighters_sortie_efficiency += 0.05; } //Regular - lvl 3 (Trained - lvl 2 is neutral)
    if (a->ship.ship_XP >= 750) { a->naval_damage_modifier += 0.033; a->naval_defence_modifier += 0.033; a->fighters_sortie_efficiency += 0.05; } //Seasoned - lvl 4
    if (a->ship.ship_XP >= 900) { a->naval_damage_modifier += 0.034; a->naval_defence_modifier += 0.034; a->fighters_sortie_efficiency += 0.05; } //Veterans - lvl 5

    //Defender
    if (d->ship.ship_XP < 100) { d->naval_damage_modifier -= 0.1; d->naval_defence_modifier -= 0.1; d->fighters_sortie_efficiency -= 0.1; }   //Fresh - lvl 1
    if (d->ship.ship_XP >= 300) { d->naval_damage_modifier += 0.033; d->naval_defence_modifier += 0.033; d->fighters_sortie_efficiency += 0.05; } //Regular - lvl 3 (Trained - lvl 2 is neutral)
    if (d->ship.ship_XP >= 750) { d->naval_damage_modifier += 0.033; d->naval_defence_modifier += 0.033; d->fighters_sortie_efficiency += 0.05; } //Seasoned - lvl 4
    if (d->ship.ship_XP >= 900) { d->naval_damage_modifier += 0.034; d->naval_defence_modifier += 0.034; d->fighters_sortie_efficiency += 0.05; } //Veterans - lvl 5

    //Critical Hit Application
    array<double, 9> modifiers{};

    //Attacker
    modifiers = applyCriticalHits(a->ship.critical_hits, a->ship.ship_class);

    a->ship.heavy_attack *= modifiers[0];
    a->ship.light_attack *= modifiers[1];
    a->ship.torpedo_attack *= modifiers[2];
    a->ship.speed *= modifiers[3];
    a->retreat_decision_chance *= modifiers[4];
    a->ship.max_ORG *= modifiers[5];
    a->ship_recovery_rate *= modifiers[6];
    a->ship.sub_visibility *= modifiers[7];
    a->sortie_efficiency += modifiers[8];

    //Defender
    modifiers = applyCriticalHits(d->ship.critical_hits, d->ship.ship_class);

    d->ship.heavy_attack *= modifiers[0];
    d->ship.light_attack *= modifiers[1];
    d->ship.torpedo_attack *= modifiers[2];
    d->ship.speed *= modifiers[3];
    d->retreat_decision_chance *= modifiers[4];
    d->ship.max_ORG *= modifiers[5];
    d->ship_recovery_rate *= modifiers[6];
    d->ship.sub_visibility *= modifiers[7];
    d->sortie_efficiency += modifiers[8];

}

void Battle::sortieDeployment (Fleet *a, Fleet *d) const {    //Calculating amount of planes in the air

    //Carrier traffic
    a->carrier_traffic = 1; d->carrier_traffic = 1; // Reset to base value
    a->detection = 1; d->detection = 1;

    //Taking IsDay/night cycle into account
    if (!IsDay) {
        a->carrier_traffic -= 0.8; d->carrier_traffic -= 0.8;
        //a->detection -= 0.2 * a->night_operations_penalty_modifier; d->detection -= 0.2 * d->night_operations_penalty_modifier;
    }
    //Taking weather into account
    if (weather != 0) {   //  May be useless for naval-air battles
        switch (weather) {
            case 1: /*a->detection -= 0.1; d->detection -= 0.1;*/ break;    //Do this even appy?
            case 2: /*a->detection -= 0.2; d->detection -= 0.2;*/ a->carrier_traffic -= 0.05; d->carrier_traffic -= 0.05; break;
            default: break;
        }
    }

    //Malus for OVERCROWDING YOUR PRECIOUS CARRIER
    if ( a->ship.fighter_amount + a->ship.bomber_amount > a->ship.deck_size) {
        a->carrier_traffic -= (a->ship.fighter_amount + a->ship.bomber_amount ) / a->ship.deck_size - 1;    //Give unsymmetrical, harsh results, but carrier overcrowding should be impossible anyway
    }

    //Sortie efficiency
    a->battle_sortie_eff = a->sortie_efficiency * (a->ship.ORG/a->ship.max_ORG);
    a->battle_fighters_sortie_efficiency = (a->sortie_efficiency + a->fighters_sortie_efficiency) * (a->ship.ORG/a->ship.max_ORG);
    //Actual amount of planes
    a->naval_bombers_in_the_air = floor(a->ship.bomber_amount * a->battle_sortie_eff * a->carrier_traffic); //Rounded to nearest, rounded down or never rounded?
    if (a->naval_bombers_in_the_air > a->ship.bomber_amount) { a->naval_bombers_in_the_air = a->ship.bomber_amount; }   //Sadly cap must be done in this way
    a->fighters_in_the_air = floor(a->ship.fighter_amount * a->battle_fighters_sortie_efficiency * a->carrier_traffic); //Rounded to nearest, rounded down or never rounded?
    if (a->fighters_in_the_air > a->ship.fighter_amount) { a->fighters_in_the_air = a->ship.fighter_amount; }   //Hard cap

    //Defender

    if ( d->ship.fighter_amount + d->ship.bomber_amount > d->ship.deck_size) {  //Malus for OVERCROWDING YOUR PRECIOUS CARRIER
        d->carrier_traffic -= (d->ship.fighter_amount + d->ship.bomber_amount ) / d->ship.deck_size - 1;    //Give unsymmetrical, harsh results, but carrier overcrowding should be impossible anyway
    }

    //Sortie efficiency
    d->battle_sortie_eff = d->sortie_efficiency * (d->ship.ORG/d->ship.max_ORG);
    d->battle_fighters_sortie_efficiency = (d->sortie_efficiency + d->fighters_sortie_efficiency) * (d->ship.ORG/d->ship.max_ORG);
    //Actual amount of planes
    d->naval_bombers_in_the_air = floor(d->ship.bomber_amount * d->battle_sortie_eff * d->carrier_traffic); //Rounded to nearest, rounded down or never rounded?
    if (d->naval_bombers_in_the_air > d->ship.bomber_amount) { d->naval_bombers_in_the_air = d->ship.bomber_amount; }   //Sadly cap must be done in this way
    d->fighters_in_the_air = floor(d->ship.fighter_amount * d->battle_fighters_sortie_efficiency * d->carrier_traffic); //Rounded to nearest, rounded down or never rounded?
    if (d->fighters_in_the_air > d->ship.fighter_amount) { d->fighters_in_the_air = d->ship.fighter_amount; }   //Hard cap

}

double Battle::airCombat (const double detection, long attacking_planes, long fighters_visibility, long bombers_visibility, const double air_attack, const double air_defence, const double attacker_agility, const double defender_agility, const double attacker_speed, const double defender_speed, const bool target_bomber) {

    double plane_to_plane_damage;

    //Target selection, do air detection even matter in naval battles?
    if (bombers_visibility <= 0) { fighters_visibility = floor(fighters_visibility * detection); }  //Escort is fully visible while planes are actually escorted
    bombers_visibility = floor(bombers_visibility * detection);
    double target_visibility;
    if (target_bomber) { target_visibility = bombers_visibility; } else { target_visibility = fighters_visibility; }

    if (fighters_visibility + bombers_visibility <= 0 || target_visibility <= 0 || attacking_planes <= 0) { plane_to_plane_damage = 0; }   //If no target can be found, neither can be destroyed, also DO NOT DIVIDE BY 0
    else {
        if (attacking_planes > target_visibility * 2) { attacking_planes = floor(target_visibility * 2); } //At most two times as many own planes can attack as enemies are visible
        const double base_air_damage = floor( attacking_planes * (target_visibility / (fighters_visibility + bombers_visibility)) ) * air_attack * 0.2;

        if (base_air_damage > 0 && air_defence <= 0) { plane_to_plane_damage = target_visibility; }  //Destroy all targeted planes because they are made of air
        else {
            double agility_mitigation;  //Only applies if defender has an agility advantage
            if (defender_agility > attacker_agility) {
                agility_mitigation = round( base_air_damage * 0.3 * (min(defender_agility / attacker_agility, 3.5) - 1) );
            } else { agility_mitigation = 0; }

            double bonus_from_relative_speed;   //Only applies if attacker has a speed advantage
            if (attacker_speed > defender_speed) {
                bonus_from_relative_speed = round( base_air_damage * 0.5 * (min(attacker_speed / defender_speed, 2.75) - 1) );
            } else { bonus_from_relative_speed = 0; }

            double bonus_from_absolute_speed;   //Only applies if attacker has a speed advantage
            if (attacker_speed > defender_speed) {  //With max possible speed of 1200 km/h in case of speed advantage, base air attack must be at least 1.6(6) to give any bonus
                bonus_from_absolute_speed = round( base_air_damage * 0.025 * min(attacker_speed / 100, 1200.0 / 100.0) );
            } else { bonus_from_absolute_speed = 0; }

            //Damage done
            plane_to_plane_damage = max( (base_air_damage - agility_mitigation + bonus_from_relative_speed + bonus_from_absolute_speed) * 5 * 0.01 / air_defence, 0.001 );
        }
    }
    return plane_to_plane_damage;
}

double Battle::airXPGain (const double detection, const long attacking_planes, long fighters_visibility, long bombers_visibility, const double sortie_eff, const double carr_traffic, const bool target_bomber) {

    double gainedXP = 0;

    if (bombers_visibility <= 0) { fighters_visibility = floor(fighters_visibility * detection); }  //Escort is fully visible while planes are actually escorted
    bombers_visibility = floor(bombers_visibility * detection);
    double target_visibility;
    if (target_bomber) { target_visibility = bombers_visibility; } else { target_visibility = fighters_visibility; }

    if (fighters_visibility + bombers_visibility <= 0 || target_visibility <= 0 || attacking_planes <= 0) { gainedXP = 0; }    //If no target can be found, planes cannot experience themselves in combat, also DO NOT DIVIDE BY 0
    else {
        gainedXP = 0.85 * sortie_eff * carr_traffic * (target_visibility / (fighters_visibility + bombers_visibility));
        if (attacking_planes > target_visibility * 2) { gainedXP *= target_visibility * 2 / attacking_planes; }
    }

    return gainedXP;
}

long Battle::shipAADefence (const double bomber_agility, const long bombers_amount, const double shipAA) {

    long bombers_shoot_by_ship_AA = 0;

    if (bombers_amount <= 0) { bombers_shoot_by_ship_AA = 0; }
    else {
        RNGResult = RNG(0.2835 - 0.001 * bomber_agility);  //Determining the chance to hit incoming bomber and putting it into RNG
        if (RNGResult) {    //Ship AA hits bomber
            const double damage_to_bombers = max(bombers_amount * 0.01 * shipAA, 0.0021);    //Calculating amount that was shot by ship AA

            RNGResult = RNG(damage_to_bombers - floor(damage_to_bombers));   //RNG determinate if amount was rounded up, or down
            if (RNGResult) { bombers_shoot_by_ship_AA = ceil(damage_to_bombers); }   //Amount was rounded up
            else { bombers_shoot_by_ship_AA = floor(damage_to_bombers); } //Amount was rounded down
        }
    }

    return bombers_shoot_by_ship_AA;
}

double Battle::finalDisruption (const double disruption_by_interception, const double air_defence, const double air_attack, const double speed, const long total_bombers) {

    double disruption_damage_factor = 0;

    if (total_bombers <= 0) { disruption_damage_factor = 0; }
    else {
        const double defence_factor = air_defence / 200.0;
        const double attack_factor = air_attack / 200.0;
        const double speed_factor = speed / 1200.0;

        disruption_damage_factor = 1 - sqrt( disruption_by_interception / (10 * total_bombers * (1 + defence_factor) * (1 + attack_factor) * (1 + speed_factor)) );
    }

    return disruption_damage_factor;
}

void Battle::airBattle (Fleet *a, Fleet *d) {  //Damage to certain enemy plane types by certain friendly plane types

    cout<<"\n";
    cout<<"Japanese Fighters in the air: "<<a->fighters_in_the_air<<" Japanese Naval Bombers in the air: "<<a->naval_bombers_in_the_air<<endl;
    cout<<"American Fighters in the air: "<<d->fighters_in_the_air<<" American Naval Bombers in the air: "<<d->naval_bombers_in_the_air<<endl;
    cout<<"\n";

    //Attacker
    //Damage to Fighters
    a->fighters_destroyed_by_friendly_planes = 0;   //Reset because function adds damage done by different plane types
    //Fighters attacking enemy Fighters
    a->fighters_destroyed_by_friendly_planes += airCombat(a->detection, a->fighters_in_the_air, d->fighters_in_the_air, d->naval_bombers_in_the_air, a->cvFighter.air_attack, d->cvFighter.air_defence,a->cvFighter.agility, d->cvFighter.agility, a->cvFighter.speed, d->cvFighter.speed, false);
    //Naval Bombers attacking enemy Fighters
    a->fighters_destroyed_by_friendly_planes += airCombat(a->detection, a->naval_bombers_in_the_air, d->fighters_in_the_air, d->naval_bombers_in_the_air, a->cvBomber.air_attack, d->cvFighter.air_defence,a->cvBomber.agility, d->cvFighter.agility, a->cvBomber.speed, d->cvFighter.speed, false);

    //Damage to Naval Bombers
    a->naval_bombers_destroyed_by_friendly_planes = 0;  //Reset because function adds damage done by different plane types
    //Fighters attacking enemy Naval Bombers
    a->naval_bombers_destroyed_by_friendly_planes += airCombat(a->detection, a->fighters_in_the_air, d->fighters_in_the_air, d->naval_bombers_in_the_air, a->cvFighter.air_attack, d->cvBomber.air_defence,a->cvFighter.agility, d->cvBomber.agility, a->cvFighter.speed, d->cvBomber.speed, true);
    //Naval Bombers attacking enemy Naval Bombers
    a->naval_bombers_destroyed_by_friendly_planes += airCombat(a->detection, a->naval_bombers_in_the_air, d->fighters_in_the_air, d->naval_bombers_in_the_air, a->cvBomber.air_attack, d->cvBomber.air_defence,a->cvBomber.agility, d->cvBomber.agility, a->cvBomber.speed, d->cvBomber.speed, true);

    //------------------------------------------------------------------------------------------------------------------

    //Defender
    //Damage to Fighters
    d->fighters_destroyed_by_friendly_planes = 0;   //Reset because function adds damage done by different plane types
    //Fighters attacking enemy Fighters
    d->fighters_destroyed_by_friendly_planes += airCombat(d->detection, d->fighters_in_the_air, a->fighters_in_the_air, a->naval_bombers_in_the_air, d->cvFighter.air_attack, a->cvFighter.air_defence,d->cvFighter.agility, a->cvFighter.agility, d->cvFighter.speed, a->cvFighter.speed, false);
    //Naval Bombers attacking enemy Fighters
    d->fighters_destroyed_by_friendly_planes += airCombat(d->detection, d->naval_bombers_in_the_air, a->fighters_in_the_air, a->naval_bombers_in_the_air, d->cvBomber.air_attack, a->cvFighter.air_defence,d->cvBomber.agility, a->cvFighter.agility, d->cvBomber.speed, a->cvFighter.speed, false);

    //Damage to Naval Bombers
    d->naval_bombers_destroyed_by_friendly_planes = 0;  //Reset because function adds damage done by different plane types
    //Fighters attacking enemy Naval Bombers
    d->naval_bombers_destroyed_by_friendly_planes += airCombat(d->detection, d->fighters_in_the_air, a->fighters_in_the_air, a->naval_bombers_in_the_air, d->cvFighter.air_attack, a->cvBomber.air_defence,d->cvFighter.agility, a->cvBomber.agility, d->cvFighter.speed, a->cvBomber.speed, true);
    //Naval Bombers attacking enemy Naval Bombers
    d->naval_bombers_destroyed_by_friendly_planes += airCombat(d->detection, d->naval_bombers_in_the_air, a->fighters_in_the_air, a->naval_bombers_in_the_air, d->cvBomber.air_attack, a->cvBomber.air_defence,d->cvBomber.agility, a->cvBomber.agility, d->cvBomber.speed, a->cvBomber.speed, true);

    //Disruption
    a->disruption_by_interception = a->fighters_in_the_air * (1 + a->cvFighter.speed / 1200) * (1 + a->detection) * 2;
    d->disruption_by_interception = d->fighters_in_the_air * (1 + d->cvFighter.speed / 1200) * (1 + d->detection) * 2;

    //Disruption reduction ratio
    if (d->disruption_by_interception > 0) { a->disruption_reduction_ratio = 2 * a->fighters_in_the_air * (1 + a->cvFighter.speed / 1200) * (1 + a->cvFighter.agility / 110) * (1 + a->cvFighter.air_attack / 100) * (1 + a->detection) / d->disruption_by_interception; }
    else { a->disruption_reduction_ratio = 0; }

    if (a->disruption_by_interception > 0) { d->disruption_reduction_ratio = 2 * d->fighters_in_the_air * (1 + d->cvFighter.speed / 1200) * (1 + d->cvFighter.agility / 110) * (1 + d->cvFighter.air_attack / 100) * (1 + d->detection) / a->disruption_by_interception; }
    else { d->disruption_reduction_ratio = 0; }

    //Disruption reduction - here disruption is now affected by both enemy interception and friendly escort
    d->disruption_by_interception -= min(d->disruption_by_interception * a->disruption_reduction_ratio, d->disruption_by_interception);
    a->disruption_by_interception -= min(a->disruption_by_interception * d->disruption_reduction_ratio, a->disruption_by_interception);

    //XP gain~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Attacker
    a->cvFighter.XP += airXPGain(a->detection, a->fighters_in_the_air, d->fighters_in_the_air, d->naval_bombers_in_the_air, a->battle_fighters_sortie_efficiency, a->carrier_traffic, false);
    a->cvFighter.XP += airXPGain(a->detection, a->fighters_in_the_air, d->fighters_in_the_air, d->naval_bombers_in_the_air, a->battle_fighters_sortie_efficiency, a->carrier_traffic, true);
    a->cvBomber.XP += airXPGain(a->detection, a->naval_bombers_in_the_air, d->fighters_in_the_air, d->naval_bombers_in_the_air, a->battle_sortie_eff, a->carrier_traffic, false);
    a->cvBomber.XP += airXPGain(a->detection, a->naval_bombers_in_the_air, d->fighters_in_the_air, d->naval_bombers_in_the_air, a->battle_sortie_eff, a->carrier_traffic, true);

    //Defender
    d->cvFighter.XP += airXPGain(d->detection, d->fighters_in_the_air, a->fighters_in_the_air, a->naval_bombers_in_the_air, d->battle_fighters_sortie_efficiency, d->carrier_traffic, false);
    d->cvFighter.XP += airXPGain(d->detection, d->fighters_in_the_air, a->fighters_in_the_air, a->naval_bombers_in_the_air, d->battle_fighters_sortie_efficiency, d->carrier_traffic, true);
    d->cvBomber.XP += airXPGain(d->detection, d->naval_bombers_in_the_air, a->fighters_in_the_air, a->naval_bombers_in_the_air, d->battle_sortie_eff, d->carrier_traffic, false);
    d->cvBomber.XP += airXPGain(d->detection, d->naval_bombers_in_the_air, a->fighters_in_the_air, a->naval_bombers_in_the_air, d->battle_sortie_eff, d->carrier_traffic, true);

    //Damage application to enemy planes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //Attacker
    //Damage to Fighters
    RNGResult = RNG(a->fighters_destroyed_by_friendly_planes);
    if (RNGResult) { d->ship.fighter_amount -= ceil(a->fighters_destroyed_by_friendly_planes); }
    else { d->ship.fighter_amount -= floor(a->fighters_destroyed_by_friendly_planes); }
    if (d->ship.fighter_amount < 0 ) { d->ship.fighter_amount = 0; cout<<"Something is wrong with air battle damage"<<endl; }

    //Damage to Naval Bombers
    RNGResult = RNG(a->naval_bombers_destroyed_by_friendly_planes);
    if (RNGResult) { d->ship.bomber_amount -= ceil(a->naval_bombers_destroyed_by_friendly_planes); }
    else { d->ship.bomber_amount -= floor(a->naval_bombers_destroyed_by_friendly_planes); }
    if (d->ship.bomber_amount < 0 ) { d->ship.bomber_amount = 0; cout<<"Something is wrong with air battle damage"<<endl; }

    //------------------------------------------------------------------------------------------------------------------

    //Defender
    //Damage to Fighters
    RNGResult = RNG(d->fighters_destroyed_by_friendly_planes);
    if (RNGResult) { a->ship.fighter_amount -= ceil(d->fighters_destroyed_by_friendly_planes); }
    else { a->ship.fighter_amount -= floor(d->fighters_destroyed_by_friendly_planes); }
    if (a->ship.fighter_amount < 0 ) { a->ship.fighter_amount = 0; cout<<"Something is wrong with air battle damage"<<endl; }

    //Damage to Naval Bombers
    RNGResult = RNG(d->naval_bombers_destroyed_by_friendly_planes);
    if (RNGResult) { a->ship.bomber_amount -= ceil(d->naval_bombers_destroyed_by_friendly_planes); }
    else { a->ship.bomber_amount -= floor(d->naval_bombers_destroyed_by_friendly_planes); }
    if (a->ship.bomber_amount < 0 ) { a->ship.bomber_amount = 0; cout<<"Something is wrong with air battle damage"<<endl; }

    //Ship AA fire
    a->naval_bombers_in_the_air -= shipAADefence(a->cvBomber.agility, a->naval_bombers_in_the_air, d->ship.ship_AA);
    if (a->ship.bomber_amount < 0 ) { a->ship.bomber_amount = 0; cout<<"Something is wrong with AA damage"<<endl; }

    d->naval_bombers_in_the_air -= shipAADefence(d->cvBomber.agility, d->naval_bombers_in_the_air, a->ship.ship_AA);
    if (a->ship.bomber_amount < 0 ) { a->ship.bomber_amount = 0; cout<<"Something is wrong with AA damage"<<endl; }

    //Disruption (now also reduced by stats of Naval Bombers) affects amount of them able to pass through enemy interception

    a->escorted_bombers = ceil(max(a->naval_bombers_in_the_air * finalDisruption(d->disruption_by_interception, a->cvBomber.air_defence, a->cvBomber.air_attack, a->cvBomber.speed, a->ship.bomber_amount), 0.0));
    d->escorted_bombers = ceil(max(d->naval_bombers_in_the_air * finalDisruption(a->disruption_by_interception, d->cvBomber.air_defence, d->cvBomber.air_attack, d->cvBomber.speed, d->ship.bomber_amount), 0.0));

    cout<<"Escorted bombers: "<<a->escorted_bombers<<endl;
    cout<<"Escorted bombers: "<<d->escorted_bombers<<endl;

}

long Battle::strikeTargeting (const long bombers, const double targeting) {

    const long amount_of_planes_that_scored_a_hit = floor(bombers * min(max(targeting / 10 * 0.31, 0.0), 1.0));

    return amount_of_planes_that_scored_a_hit;
}

double Battle::shipAADamageReduction (const double target_AA, const double fleet_AA) {

    const double damage_reduction = min(max(0.25 * pow(target_AA + fleet_AA * 0.25, 0.2), 0.0), 0.75);

    return damage_reduction;
}

long Battle::criticalHits (const long ship_class, const long light_batteries, const long heavy_batteries, const long secondary_batteries, const long torpedo_tubes, const array<double, 10> critical_hits) {

    long modules_with_same_weight = 0;
    long what_module = 0;   //0 - Nothing critical, 1 - Magazine, 2 - Rudder, 3 - Propeller, 4 - Heavy Fires, 5 - Ballast Tanks Inoperable/Flight Deck Damaged, 6 - Light Battery, 7 - Heavy battery, 8 - Secondary battery, 9 - Torpedo tube

    double weights_sum = 0.1 + 1 + 0.5 + 0.25;  //All ships have at least this sum
    weights_sum += heavy_batteries + secondary_batteries + light_batteries + torpedo_tubes;   //Batteries and torpedo tubes can also be damaged (in every possible case)
    if (ship_class > 6) { weights_sum++; }  //Both types of submarines and carriers have a unique extra "base ship" module critical hit
    //Side note: Medium Batteries = Heavy Batteries, after multiple tests: Light Batteries on Heavy Cruisers can be damaged by: "Main Battery Mount Disabled" critical hit that supposedly happens only on screens and Medium Batteries on Light Cruisers are damaged just like Heavy Batteries
    //It is unknown if Medium Batteries on Light Cruisers adds a weight to "Main Battery Mount Disabled" (check critical hit table on Hoi4 wiki), but tests on Light Cruisers with 1 Light and Medium Battery each given more hits scored on Medium ones, so considering /\ and that it is rather false statement

    //Determining what part was damaged
    double drawn_number = RNGSimple(1);  //Hoi4 probably uses rng generator with "<", despite "<=" being slightly more accurate
    if (drawn_number < 0.1 / weights_sum && critical_hits[1] == 0) {    //Magazine hit (ship can have up to one of this critical hit)
        what_module = 1;
    } else {
        if (drawn_number < (0.25 + 0.1) / weights_sum && critical_hits[2] == 0) {    //Rudder jammed (-75% modifier)
            what_module = 2;
        } else {
            if (drawn_number < (0.5 + 0.25 + 0.1) / weights_sum && critical_hits[3] == 0) {  //Broken propeller (-50% modifier)
                what_module = 3;
            } else {    //Module with a weight at least 1 was hit

                //From this point we need to take potentially equal weights of components into account. Heavy fires have 1, the same as a single torpedo tube for example
                drawn_number = RNGSimple(weights_sum - 0.85);   //Only cases with a weight of 1 are now examined
                if (drawn_number < 1 && critical_hits[4] == 0) {    //Heavy fires - first case
                    what_module = 4;
                } else {
                    if (drawn_number < 2) {
                        for (modules_with_same_weight = 0; modules_with_same_weight < 1; modules_with_same_weight++) {
                            if (ship_class > 6 && what_module != 5 && critical_hits[5] == 0) { what_module = 5; break; }
                            if (light_batteries > 0 && what_module != 6 && critical_hits[6] < light_batteries) { what_module = 6; break; }  //If there is any battery to be targeted of that type, this is not already selected module and still there is/are module/s of that type that can be damaged, select that to be scored
                            if (heavy_batteries > 0 && what_module != 7 && critical_hits[7] < heavy_batteries) { what_module = 7; break; }  //First and last condition may be merged /\
                            if (secondary_batteries > 0 && what_module != 8 && critical_hits[8] < secondary_batteries) { what_module = 8; break; }
                            if (torpedo_tubes > 0 && what_module != 9 && critical_hits[9] < torpedo_tubes) { what_module = 9; break; }
                        }
                    }
                    else {
                        if (drawn_number < 3) {
                            for (modules_with_same_weight = 0; modules_with_same_weight < 2; modules_with_same_weight++) {
                                if (ship_class > 6 && what_module != 5 && critical_hits[5] == 0) { what_module = 5; break; }
                                if (light_batteries > 0 && what_module != 6 && critical_hits[6] < light_batteries) { what_module = 6; break; }
                                if (heavy_batteries > 0 && what_module != 7 && critical_hits[7] < heavy_batteries) { what_module = 7; break; }
                                if (secondary_batteries > 0 && what_module != 8 && critical_hits[8] < secondary_batteries) { what_module = 8; break; }
                                if (torpedo_tubes > 0 && what_module != 9 && critical_hits[9] < torpedo_tubes) { what_module = 9; break; }
                            }
                        } else {
                            if (drawn_number < 4) {
                                for (modules_with_same_weight = 0; modules_with_same_weight < 3; modules_with_same_weight++) {
                                    if (ship_class > 6 && what_module != 5 && critical_hits[5] == 0) { what_module = 5; break; }
                                    if (light_batteries > 0 && what_module != 6 && critical_hits[6] < light_batteries) { what_module = 6; break; }
                                    if (heavy_batteries > 0 && what_module != 7 && critical_hits[7] < heavy_batteries) { what_module = 7; break; }
                                    if (secondary_batteries > 0 && what_module != 8 && critical_hits[8] < secondary_batteries) { what_module = 8; break; }
                                    if (torpedo_tubes > 0 && what_module != 9 && critical_hits[9] < torpedo_tubes) { what_module = 9; break; }
                                }
                            } else {
                                if (drawn_number < 5) {
                                    for (modules_with_same_weight = 0; modules_with_same_weight < 4; modules_with_same_weight++) {
                                        if (ship_class > 6 && what_module != 5 && critical_hits[5] == 0) { what_module = 5; break; }
                                        if (light_batteries > 0 && what_module != 6 && critical_hits[6] < light_batteries) { what_module = 6; break; }
                                        if (heavy_batteries > 0 && what_module != 7 && critical_hits[7] < heavy_batteries) { what_module = 7; break; }
                                        if (secondary_batteries > 0 && what_module != 8 && critical_hits[8] < secondary_batteries) { what_module = 8; break; }
                                        if (torpedo_tubes > 0 && what_module != 9 && critical_hits[9] < torpedo_tubes) { what_module = 9; break; }
                                    }
                                } else {
                                    if (drawn_number < 6) {
                                        for (modules_with_same_weight = 0; modules_with_same_weight < 5; modules_with_same_weight++) {
                                            if (ship_class > 6 && what_module != 5 && critical_hits[5] == 0) { what_module = 5; break; }
                                            if (light_batteries > 0 && what_module != 6 && critical_hits[6] < light_batteries) { what_module = 6; break; }
                                            if (heavy_batteries > 0 && what_module != 7 && critical_hits[7] < heavy_batteries) { what_module = 7; break; }
                                            if (secondary_batteries > 0 && what_module != 8 && critical_hits[8] < secondary_batteries) { what_module = 8; break; }
                                            if (torpedo_tubes > 0 && what_module != 9 && critical_hits[9] < torpedo_tubes) { what_module = 9; break; }
                                        }
                                    } else {    //Last case - does not need to be checked and is one that needs to select latest option, so loop to get to it and stop at right moment is unnecessary
                                        if (ship_class > 6 && critical_hits[5] == 0) { what_module = 5; }
                                        if (light_batteries > 0 && critical_hits[6] < light_batteries) { what_module = 6; }
                                        if (heavy_batteries > 0 && critical_hits[7] < heavy_batteries) { what_module = 7; }
                                        if (secondary_batteries > 0 && critical_hits[8] < secondary_batteries) { what_module = 8; }
                                        if (torpedo_tubes > 0 && critical_hits[9] < torpedo_tubes) { what_module = 9; }
                                    }
                                }
                            }
                        }
                    }
                }//End of cases with a weight of 1
            }//End of "Broken Propeller" else case (modules with a weight of at least 1)
        }//End of "Rudder Jammed" case
    }//End of "Magazine Hit" case

    return what_module;
}

array<double,10> Battle::bomberDamage (const double naval_attack, const double AA_damage_reduction, const double critical_hit_chance, const double chance_to_receive_critical_hit, const double target_reliability, const double effects_on_critical_hit, const long ship_class, const long light_batteries, const long heavy_batteries, const long secondary_batteries, const long torpedo_tubes, array<double, 10> critical_hits) {

    double damage_to_ship = 0;
    double critical_hit_chance_real;
    double critical_hit_damage_multiplier = 1;    //Most critical hits have a modifier of 1, so it is easier to set up this once in the beginning
    long what_module = 0;   //0 - Nothing critical, 1 - Magazine, 2 - Rudder, 3 - Propeller, 4 - Heavy Fires, 5 - Ballast Tanks Inoperable/Flight Deck Damaged, 6 - Light Battery, 7 - Heavy battery, 8 - Secondary battery, 9 - Torpedo tube

    if (target_reliability <= 0) { critical_hit_chance_real = 1; }
    else { critical_hit_chance_real = 0.1 * (1 + critical_hit_chance) * (1 + chance_to_receive_critical_hit) / target_reliability; }

    double drawn_number = RNGSimple(1);     //We use more complex RNG to get better randomness, otherwise many hits would result in critical hit on a critical part (just like plane-plane dmg works)
    if (drawn_number <= critical_hit_chance_real) {    //Critical hit happens

        drawn_number = RNGSimple(1);
        if (drawn_number <= critical_hit_chance_real && ship_class != 0) {     //Critical hit on a critical part happens, convoys does not have any modules that can be damaged separately

            what_module = criticalHits(ship_class, light_batteries, heavy_batteries, secondary_batteries, torpedo_tubes, critical_hits);
            switch (what_module) {    //Note, maximum number of specific critical hit = amount of modules of that type (always 1 for "base ship" type critical hit)
                case 1: critical_hit_damage_multiplier = 9; critical_hits[1] = 1; cout<<"Magazine Hit!"<<endl; break;     //Magazine Hit (+900% modifier)
                case 2: critical_hit_damage_multiplier = 0.25; critical_hits[2] = 1; cout<<"Rudder Jammed!"<<endl; break;    //Rudder Jammed (-75% modifier)
                case 3: critical_hit_damage_multiplier = 0.5; critical_hits[3] = 1; cout<<"Broken Propeller!"<<endl; break;     //Broken Propeller (-50% modifier)
                case 4: critical_hits[4] = 1; cout<<"Heavy Fires!"<<endl; break;      //Heavy Fires (no modifier to multiplier)
                case 5: critical_hit_damage_multiplier = 0.25; critical_hits[5] = 1; if (ship_class < 9) { cout<<"Flight Deck Damaged!"<<endl; } else { cout<<"Ballast Tanks Inoperable!"<<endl; } break;    //Ballast Tanks Inoperable/Flight Deck Damaged (-75% modifier)
                case 6: critical_hits[6]++; cout<<"Main Battery Mount Disabled!"<<endl; break;  //Main Battery Mount Disabled (no modifier)
                case 7: critical_hits[7]++; cout<<"Main Battery Turret Destroyed!"<<endl; break;  //Main Battery Turret Destroyed (no modifier)
                case 8: critical_hits[8]++; cout<<"Secondaries knocked out!"<<endl; break;  //Secondaries knocked out (no modifier)
                case 9: critical_hits[9]++; cout<<"Torpedo tubes disabled!"<<endl; break;  //Torpedo Tubes Disabled (no modifier)
                default: break;
            }
            damage_to_ship = naval_attack * (1 - AA_damage_reduction) * critical_hit_damage_multiplier * (1 + effects_on_critical_hit);
        } else {
            //Calculating damage (and bonus) from regular critical hit, because critical hit left critical parts unharmed
            cout<<"Critical Hit!"<<endl;
            critical_hit_damage_multiplier = min(max(1 + (1 - target_reliability) * 3, 0.0), 4.0);
            damage_to_ship = naval_attack * (1 - AA_damage_reduction) * critical_hit_damage_multiplier * (1 + effects_on_critical_hit);
        }
    } else {    //Calculating damage from regular hit, because hit was not critical
        damage_to_ship = naval_attack * (1 - AA_damage_reduction);
    }

    critical_hits[0] = damage_to_ship;  //We store damage done to array

    return critical_hits;
}

void Battle::carrierStrike (Fleet *a, Fleet *d) {

    long succesful_hits;
    double damage_done;
    array<double, 10> critical_hits{};

    //Attacker
    cout<<"\nStrike done by: "<<a->tag<<endl;

    succesful_hits = strikeTargeting(a->escorted_bombers, a->cvBomber.naval_targeting);
    a->cvBomber.XP += floor(succesful_hits / a->ship.bomber_amount) * 0.3;    //Adding XP to bombers that successfully completed their mission
    cout<<"Successful hits: "<<succesful_hits<<endl;

    for (int i = 0; i < succesful_hits; i++) {
        critical_hits = bomberDamage(a->cvBomber.naval_attack, shipAADamageReduction(d->ship.ship_AA, d->ship.ship_AA), a->chance_to_score_critical_hit_modifier, d->chance_to_receive_critical_hit, d->ship.reliability, d->effect_on_sustained_critical_hit_modifier, d->ship.ship_class, d->ship.light_batteries, d->ship.heavy_batteries, d->ship.secondary_batteries, d->ship.torpedo_tubes, d->ship.critical_hits);
        damage_done = critical_hits[0];
        for (int j = 1; j <= 9; j++) {    //Assigning critical hits to enemy ship
            d->ship.critical_hits[j] = critical_hits[j];
        }
        if (critical_hits[4] == 1) {    //Heavy Fires gives extra dmg and dmg bonus to ORG dmg
            if (damage_done * 2 * 0.83 + 5 > d->ship.ORG) { d->ship.ORG = 0; }  //ORG would be negative
            else { d->ship.ORG -= damage_done * 2 * 0.83 + 5; cout<<"Damage done to ORG: "<<damage_done * 2 * 0.83 + 5<<endl; }     //Applying dmg to ORG
        } else {    //No critical hit with extra ORG dmg
            if (damage_done * 0.83 > d->ship.ORG) { d->ship.ORG = 0; }  //ORG would be negative
            else { d->ship.ORG -= damage_done * 0.83; cout<<"Damage done to ORG: "<<damage_done * 0.83<<endl; }     //Applying dmg to ORG
        }
        if (damage_done * 0.75 > d->ship.HP) { d->ship.HP = 0; }    //HP would be negative
        else { d->ship.HP -= damage_done * 0.75; cout<<"Damage done to HP: "<<damage_done * 0.75<<endl; }   //Applying dmg to HP
    }

    //Defender
    cout<<"\nStrike done by: "<<d->tag<<endl;

    succesful_hits = strikeTargeting(d->escorted_bombers, d->cvBomber.naval_targeting);
    d->cvBomber.XP += floor(succesful_hits / d->ship.bomber_amount) * 0.3;    //Adding XP to bombers that successfully completed their mission
    cout<<"Successful hits: "<<succesful_hits<<endl;

    for (int i = 0; i < succesful_hits; i++) {
        critical_hits = bomberDamage(d->cvBomber.naval_attack, shipAADamageReduction(a->ship.ship_AA, a->ship.ship_AA), d->chance_to_score_critical_hit_modifier, a->chance_to_receive_critical_hit, a->ship.reliability, a->effect_on_sustained_critical_hit_modifier, a->ship.ship_class, a->ship.light_batteries, a->ship.heavy_batteries, a->ship.secondary_batteries, a->ship.torpedo_tubes, a->ship.critical_hits);
        damage_done = critical_hits[0];
        for (int j = 1; j <= 9; j++) {    //Assigning critical hits to enemy ship
            a->ship.critical_hits[j] = critical_hits[j];
        }
        if (critical_hits[4] == 1) {    //Heavy Fires gives extra dmg and dmg bonus to ORG dmg
            if (damage_done * 2 * 0.83 + 5 > a->ship.ORG) { a->ship.ORG = 0; }  //ORG would be negative
            else { a->ship.ORG -= damage_done * 2 * 0.83 + 5; cout<<"Damage done to ORG: "<<damage_done * 2 * 0.83 + 5<<endl; }     //Applying dmg to ORG
        } else {    //No critical hit with extra ORG dmg
            if (damage_done * 0.83 > a->ship.ORG) { a->ship.ORG = 0; }  //ORG would be negative
            else { a->ship.ORG -= damage_done * 0.83; cout<<"Damage done to ORG: "<<damage_done * 0.83<<endl; }     //Applying dmg to ORG
        }
        if (damage_done * 0.75 > a->ship.HP) { a->ship.HP = 0; }
        else { a->ship.HP -= damage_done * 0.75; cout<<"Damage done to HP: "<<damage_done * 0.75<<endl; }
    }

    cout<<"\n";
    cout<<"Japanese ship HP: "<<a->ship.HP<<endl;
    cout<<"Japanese ship ORG: "<<a->ship.ORG<<endl;
    cout<<"American ship HP: "<<d->ship.HP<<endl;
    cout<<"American ship ORG: "<<d->ship.ORG<<endl;
    cout<<"\n";

}

void Battle::fileReading (const bool overload, Fleet *a, Fleet *d) {

    string fileInput="";    //Variable containing raw text from file
    int row=0;   //Row of data from the file, first row = 0
    int whereMark=0;
    string fileArray[66];
    string reloadArray[66]; //Secondary array to restore overloaded variables (cv.Fighter air_attack etc.)

    //For Attacker ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ifstream FileA("Fleet_A.txt");
    while (row <= 66) {
        getline (FileA, fileInput);
        whereMark = fileInput.find("=");
        fileArray[row] = fileInput.substr(whereMark + 1, 40);
        row++;
    }
    FileA.close();
    row=0;

    if (overload) { //We just restore overloaded variables
        while (row <= 65) {
            switch (row) {
                case 7: a->sortie_efficiency = stod(fileArray[row]); break;
                case 8: a->fighters_sortie_efficiency = stod(fileArray[row]); break;
                case 27: a->ship.max_ORG = stod(fileArray[row]); break;
                case 31: a->ship.speed = stod(fileArray[row]); break;
                case 38: a->ship.light_attack = stod(fileArray[row]); break;
                case 40: a->ship.heavy_attack = stod(fileArray[row]); break;
                case 42: a->ship.torpedo_attack = stod(fileArray[row]); break;
                case 46: a->ship.sub_visibility = stod(fileArray[row]); break;
                case 50: a->cvFighter.air_attack = stod(fileArray[row]); break;
                case 52: a->cvFighter.agility = stod(fileArray[row]); break;
                case 57: a->cvBomber.air_attack = stod(fileArray[row]); break;
                case 59: a->cvBomber.agility = stod(fileArray[row]); break;
                default: break;
            } row++;
        } row = 0;
    } else {
        while (row <= 65) {    //Assigning data from sign to right variables (and converting them if needed)
            switch (row) {
                case 0: a->tag = fileArray[row]; break;  //Country name aka "tag"
                case 1: a->dockyard_output = stod(fileArray[row]); break;
                case 2: a->research_speed = stod(fileArray[row]); break;
                case 3: a->refill_cost = stod(fileArray[row]); break;
                case 7: a->sortie_efficiency = stod(fileArray[row]); break;
                case 8: a->fighters_sortie_efficiency = stod(fileArray[row]); break;
                case 14: a->naval_damage_modifier = stod(fileArray[row]); break;
                case 15: a->naval_defence_modifier = stod(fileArray[row]); break;
                case 16: a->chance_to_score_critical_hit_modifier = stod(fileArray[row]); break;
                case 17: a->chance_to_receive_critical_hit = stod(fileArray[row]); break;
                case 18: a->effect_on_sustained_critical_hit_modifier = stod(fileArray[row]); break;
                case 19: a->naval_targeting_modifier = stod(fileArray[row]); break;
                case 20: a->naval_agility = stod(fileArray[row]); break;
                case 21: a->naval_air_attack_from_carriers = stod(fileArray[row]); break;
                case 22: a->naval_air_targeting_from_carriers = stod(fileArray[row]); break;
                case 23: a->naval_air_agility_from_carriers = stod(fileArray[row]); break;
                case 24: a->ship.name = fileArray[row]; break;
                case 25: a->ship.ship_class = stoi(fileArray[row]); break;
                case 26: a->ship.ship_production_cost = stod(fileArray[row]); break;
                case 27: a->ship.max_ORG = stod(fileArray[row]); break;
                case 28: a->ship.ORG = stod(fileArray[row]); break;
                case 29: a->ship.max_HP = stod(fileArray[row]); break;
                case 30: a->ship.HP = stod(fileArray[row]); break;
                case 31: a->ship.speed = stod(fileArray[row]); break;
                case 32: a->ship.ship_manpower = stoi(fileArray[row]); break;
                case 33: a->ship.reliability = stod(fileArray[row]); break;
                case 34: a->ship.ship_XP = stoi(fileArray[row]); break;
                case 35: a->ship.deck_size = stoi(fileArray[row]); break;
                case 36: a->ship.fighter_amount = stoi(fileArray[row]); break;
                case 37: a->ship.bomber_amount = stoi(fileArray[row]); break;
                case 38: a->ship.light_attack = stod(fileArray[row]); break;
                case 39: a->ship.light_piercing = stod(fileArray[row]); break;
                case 40: a->ship.heavy_attack = stod(fileArray[row]); break;
                case 41: a->ship.heavy_piercing = stod(fileArray[row]); break;
                case 42: a->ship.torpedo_attack = stod(fileArray[row]); break;
                case 43: a->ship.ship_armor = stod(fileArray[row]); break;
                case 44: a->ship.ship_AA = stod(fileArray[row]); break;
                case 45: a->ship.surface_visibility = stod(fileArray[row]); break;
                case 46: a->ship.sub_visibility = stod(fileArray[row]); break;
                case 47: a->ship.surface_detection = stod(fileArray[row]); break;
                case 48: a->ship.sub_detection = stod(fileArray[row]); break;
                case 49: a->cvFighter.name = fileArray[row]; break;
                case 50: a->cvFighter.air_attack = stod(fileArray[row]); break;
                case 51: a->cvFighter.air_defence = stod(fileArray[row]); break;
                case 52: a->cvFighter.agility = stod(fileArray[row]); break;
                case 53: a->cvFighter.speed = stod(fileArray[row]); break;
                case 54: a->cvFighter.XP = stoi(fileArray[row]); break;
                case 55: a->cvFighter.production_cost = stod(fileArray[row]); break;
                case 56: a->cvBomber.name = fileArray[row]; break;
                case 57: a->cvBomber.air_attack = stod(fileArray[row]); break;
                case 58: a->cvBomber.air_defence = stod(fileArray[row]); break;
                case 59: a->cvBomber.agility = stod(fileArray[row]); break;
                case 60: a->cvBomber.speed = stod(fileArray[row]); break;
                case 61: a->cvBomber.XP = stoi(fileArray[row]); break;
                case 62: a->cvBomber.production_cost = stod(fileArray[row]); break;
                case 63: a->cvBomber.naval_attack = stod(fileArray[row]); break;
                case 64: a->cvBomber.naval_targeting = stod(fileArray[row]); break;
                default: break;
            } row++;
        } row=0;
    }

    //For Defender ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ifstream FileD("Fleet_D.txt");
    while (row <= 66) {
        getline (FileD, fileInput);
        whereMark = fileInput.find("=");
        fileArray[row] = fileInput.substr(whereMark + 1, 40);
        row++;
    }
    FileD.close();
    row = 0;

    if (overload) {  //We just restore overloaded variables
        while (row <= 65) {
            switch (row) {
                case 7: d->sortie_efficiency = stod(fileArray[row]); break;
                case 8: d->fighters_sortie_efficiency = stod(fileArray[row]); break;
                case 27: d->ship.max_ORG = stod(fileArray[row]); break;
                case 31: d->ship.speed = stod(fileArray[row]); break;
                case 38: d->ship.light_attack = stod(fileArray[row]); break;
                case 40: d->ship.heavy_attack = stod(fileArray[row]); break;
                case 42: d->ship.torpedo_attack = stod(fileArray[row]); break;
                case 46: d->ship.sub_visibility = stod(fileArray[row]); break;
                case 50: d->cvFighter.air_attack = stod(fileArray[row]); break;
                case 52: d->cvFighter.agility = stod(fileArray[row]); break;
                case 57: d->cvBomber.air_attack = stod(fileArray[row]); break;
                case 59: d->cvBomber.agility = stod(fileArray[row]); break;
                default: break;
            } row++;
        } row = 0;
    } else {
        while (row <= 65) {    //Assigning data from sign to right variables (and converting them if needed)
            switch (row) {
                case 0: d->tag = fileArray[row]; break;  //Country name aka "tag"
                case 1: d->dockyard_output = stod(fileArray[row]); break;
                case 2: d->research_speed = stod(fileArray[row]); break;
                case 3: d->refill_cost = stod(fileArray[row]); break;
                case 7: d->sortie_efficiency = stod(fileArray[row]); break;
                case 8: d->fighters_sortie_efficiency = stod(fileArray[row]); break;
                case 14: d->naval_damage_modifier = stod(fileArray[row]); break;
                case 15: d->naval_defence_modifier = stod(fileArray[row]); break;
                case 16: d->chance_to_score_critical_hit_modifier = stod(fileArray[row]); break;
                case 17: d->chance_to_receive_critical_hit = stod(fileArray[row]); break;
                case 18: d->effect_on_sustained_critical_hit_modifier = stod(fileArray[row]); break;
                case 19: d->naval_targeting_modifier = stod(fileArray[row]); break;
                case 20: d->naval_agility = stod(fileArray[row]); break;
                case 21: d->naval_air_attack_from_carriers = stod(fileArray[row]); break;
                case 22: d->naval_air_targeting_from_carriers = stod(fileArray[row]); break;
                case 23: d->naval_air_agility_from_carriers = stod(fileArray[row]); break;
                case 24: d->ship.name = fileArray[row]; break;
                case 25: d->ship.ship_class = stoi(fileArray[row]); break;
                case 26: d->ship.ship_production_cost = stod(fileArray[row]); break;
                case 27: d->ship.max_ORG = stod(fileArray[row]); break;
                case 28: d->ship.ORG = stod(fileArray[row]); break;
                case 29: d->ship.max_HP = stod(fileArray[row]); break;
                case 30: d->ship.HP = stod(fileArray[row]); break;
                case 31: d->ship.speed = stod(fileArray[row]); break;
                case 32: d->ship.ship_manpower = stoi(fileArray[row]); break;
                case 33: d->ship.reliability = stod(fileArray[row]); break;
                case 34: d->ship.ship_XP = stoi(fileArray[row]); break;
                case 35: d->ship.deck_size = stoi(fileArray[row]); break;
                case 36: d->ship.fighter_amount = stoi(fileArray[row]); break;
                case 37: d->ship.bomber_amount = stoi(fileArray[row]); break;
                case 38: d->ship.light_attack = stod(fileArray[row]); break;
                case 39: d->ship.light_piercing = stod(fileArray[row]); break;
                case 40: d->ship.heavy_attack = stod(fileArray[row]); break;
                case 41: d->ship.heavy_piercing = stod(fileArray[row]); break;
                case 42: d->ship.torpedo_attack = stod(fileArray[row]); break;
                case 43: d->ship.ship_armor = stod(fileArray[row]); break;
                case 44: d->ship.ship_AA = stod(fileArray[row]); break;
                case 45: d->ship.surface_visibility = stod(fileArray[row]); break;
                case 46: d->ship.sub_visibility = stod(fileArray[row]); break;
                case 47: d->ship.surface_detection = stod(fileArray[row]); break;
                case 48: d->ship.sub_detection = stod(fileArray[row]); break;
                case 49: d->cvFighter.name = fileArray[row]; break;
                case 50: d->cvFighter.air_attack = stod(fileArray[row]); break;
                case 51: d->cvFighter.air_defence = stod(fileArray[row]); break;
                case 52: d->cvFighter.agility = stod(fileArray[row]); break;
                case 53: d->cvFighter.speed = stod(fileArray[row]); break;
                case 54: d->cvFighter.XP = stoi(fileArray[row]); break;
                case 55: d->cvFighter.production_cost = stod(fileArray[row]); break;
                case 56: d->cvBomber.name = fileArray[row]; break;
                case 57: d->cvBomber.air_attack = stod(fileArray[row]); break;
                case 58: d->cvBomber.air_defence = stod(fileArray[row]); break;
                case 59: d->cvBomber.agility = stod(fileArray[row]); break;
                case 60: d->cvBomber.speed = stod(fileArray[row]); break;
                case 61: d->cvBomber.XP = stoi(fileArray[row]); break;
                case 62: d->cvBomber.production_cost = stod(fileArray[row]); break;
                case 63: d->cvBomber.naval_attack = stod(fileArray[row]); break;
                case 64: d->cvBomber.naval_targeting = stod(fileArray[row]); break;
                default: break;
            } row++;
        } row = 0;
    }
}

int main() {

    Battle battle;

    return 0;
}