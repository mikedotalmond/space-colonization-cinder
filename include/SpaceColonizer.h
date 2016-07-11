#pragma once

#include "cinder/app/App.h"

#include "cinder/Log.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

enum SCGrowType { SPLIT, NO_SPLIT };

enum SCType { _2D, _3D };

struct Hormone {
	int state;
	vec3 position;
};

struct Bud {
	int state;
	vec3 position;
	bool split;
	bool hasParent;
	int parentIndex;
	bool branched;
	bool hasDirection;
	vec3 direction;
	vector<Hormone> hormones;
};

struct SCData {
	vector<Bud> buds;
	vector<Hormone> hormones;
};


struct SCOptions {
	SCType type;
	float deadZone;
	float growthStep;
	float splitChance;
	int numHormones;
	int startBuds;
	float centerRadius;
	float viewAngle;
	float branchAngle;
	float viewDistance;
	SCGrowType growType;
	vector<vec3> budPositions;
	vector<vec3> hormonePositions;
};

class SpaceColonizer {

	public:

		SCOptions options;

		SpaceColonizer(SCOptions options);
		SpaceColonizer();
		~SpaceColonizer();

		vector<Bud> iterate();

		void reset(SCOptions options);
		void reset();
		bool isComplete();

		static vec3 randomVec3();

	private:

		bool completed;
		int lastBudCount;
		vec3 avgVec;
		vector<Bud> buds;
		vector<Hormone> hormones;
		vector<vector<int>> hormonesForBud;

		void setup(SCOptions o);
		void generateHormones();
		void generateBuds();
		void findAttractors();
		void addBud(Bud & bud);



		void calculateAverageVec(int index);

		vec3 nextDirection(vec3 & budPos, bool rotate);
		vec3 nextDirectionForBranch(vec3 & budPos);

		bool splitBranch(int parentIndex);
};