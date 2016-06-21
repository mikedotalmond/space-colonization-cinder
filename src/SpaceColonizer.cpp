#include "SpaceColonizer.h"

#include "cinder/Rand.h"


static const SCOptions DefaultOptions = {
	_3D, // type
	.1f, //deadzone
	.02f, //growthStep
	.2f, //splitChance
	1000, //numHormones
	1, //startBuds
	1.0f, //centerRadius
	0.873f, // viewAngle // 50
	0.524f, // branchAngle // 30
	.2f, //viewDistance,
	SPLIT, // growType
};


SpaceColonizer::SpaceColonizer(SCOptions scOptions) {
	setup(scOptions);
}

SpaceColonizer::SpaceColonizer() {
	setup(DefaultOptions);
}

SpaceColonizer::~SpaceColonizer(){

}


void SpaceColonizer::setup(SCOptions o) {
	options = o;
	buds = vector<Bud>();
	hormones = vector<Hormone>();
	hormonesForBud = vector<vector<int>>();
	reset();
}


SCData SpaceColonizer::iterate(){ 

	findAttractors();

	const size_t n = buds.size();
	for (size_t i = 0; i < n; i++) {

		if (buds[i].state == 1) continue;

		if (hormonesForBud[i].size() == 0) {
			options.hormonePositions.resize(0);
			buds[i].state++;
			continue;
		}

		vec3 budPos = vec3(buds[i].position);
		calculateAverageVec(i);

		bool didSplit = splitBranch(budPos);
		vec3 nextDir = nextDirection(budPos, didSplit);
		vec3 nextPos = budPos + nextDir;
		
		buds[i].state++;
		buds.push_back({0, nextPos, false, true, buds[i].position, true, nextDir });

		const size_t hSize = hormonesForBud[i].size();
		vector<Hormone> budHormones = vector<Hormone>(hSize);
		for (size_t j = 0; j < hSize; j++) {
			budHormones[j] = hormones[hormonesForBud[i][j]];
		}
		buds[i].hormones = budHormones;
	}

	return { buds, hormones };
}


void SpaceColonizer::reset(SCOptions options) {
	this->options = options;
	reset();
}


void SpaceColonizer::reset() {
	generateHormones();
	generateBuds();
}


void SpaceColonizer::generateHormones() {
	
	vec3 pos;
	size_t length = options.hormonePositions.size();
	hormones = vector<Hormone>();

	// use the supplied hormone positions
	if (length > 0) {
		for (size_t i = 0; i < length; i++) {
			pos = vec3(options.hormonePositions[i]);
			hormones.push_back({ 0, pos });
		}
	} else {
		// generate a random set of hormone positions
		length = options.numHormones;
		for (size_t i = 0; i < length; i++) {
			pos = randomVec3() * options.centerRadius;
			if (options.type == _2D) pos.z = 0;
			hormones.push_back({ 0, pos });
		}
	}
}


void SpaceColonizer::generateBuds() {

	buds.resize(0);
	bool haveBuds = options.budPositions.size() > 0;	
	size_t length = haveBuds ? options.budPositions.size()  : options.startBuds;

	vec3 pos;

	if (haveBuds) {
		vector<vec3> positions = options.budPositions;
		for (size_t i = 0; i < length; i++) {
			pos = positions[i];
			buds.push_back({ 0, vec3(pos) });
		}
	}
	else {
		for (size_t i = 0; i < length; i++) {
			pos = randomVec3() * options.centerRadius;
			if (options.type == _2D) pos.z = 0.0f;
			buds.push_back({ 0, pos });
		}
	}
}


void SpaceColonizer::findAttractors() {

	size_t budCount = buds.size();

	hormonesForBud.resize(budCount);
	for (size_t i = 0; i < budCount; i++) hormonesForBud[i].resize(0);

	float vd2 = options.viewDistance * options.viewDistance;
	float dz2 = options.deadZone * options.deadZone;
	
	for (size_t i = 0; i < hormones.size(); i++) {

		if (hormones[i].state != 0) continue;

		int minDistIndex = -1;
		float minDist = vd2;

		for (size_t j = 0; j < budCount; j++) {

			if (buds[j].state > 0) continue;

			float distSq = distance2(hormones[i].position, buds[j].position);

			if (buds[j].hasDirection) {
				vec3 budPosDirNorm = normalize(vec3(buds[j].direction));
				vec3 hormPosNorm = normalize(hormones[i].position - buds[j].position);
				float dt = dot(budPosDirNorm, hormPosNorm);
				float angle = acos(dt);
				if (angle > options.viewAngle * 2.0f) continue;
			}

			if (distSq < minDist) {
				minDist = distSq;
				minDistIndex = j;
			}
		}

		if (minDistIndex == -1) continue;

		hormonesForBud[minDistIndex].push_back(i);

		if (minDist < dz2 && minDistIndex != -1) {
			hormones[i].state++;
		}
	}
}


void SpaceColonizer::calculateAverageVec(int index) {

	int avgPosCount = 0;
	vec3 avgPos = vec3();
	vector<int> h = hormonesForBud[index];
	size_t n = h.size();

	for (size_t i = 0; i < n; i++) {
		int j = h[i];
		Hormone hormone = hormones[j];
		avgPos += hormone.position;
		avgPosCount++;
	}

	avgVec = avgPos * (1.0f / (float)avgPosCount);
}



vec3 SpaceColonizer::nextDirection(vec3 budPos, bool rotate) {

	vec3 dir = avgVec - budPos;
	dir = normalize(dir) * options.growthStep;

	if (rotate && options.growType == SPLIT) {
		float a = options.branchAngle;
		float sin = sinf(a);
		float cos = cosf(a);
		dir.x = dir.x * cos + dir.y * sin;
		dir.y = -(dir.x * sin) + dir.y * cos;
	}

	return dir;
}



vec3 SpaceColonizer::nextDirectionForBranch(vec3 budPos) {

	float a = -options.branchAngle;
	float sin = sinf(a);
	float cos = cosf(a);

	vec3 dir = avgVec - budPos;
	dir = normalize(dir) * options.growthStep;
	
	dir.x = dir.x * cos + dir.y * sin;
	dir.y = -(dir.x * sin) + dir.y * cos;

	return dir;
}



bool SpaceColonizer::splitBranch(vec3 parentPos) {

	if (Rand::randFloat() > (1.0 - options.splitChance)) {
		vec3 branchNextDir = nextDirectionForBranch(parentPos);
		vec3 branchNextPos = parentPos + branchNextDir;
		buds.push_back({ 0, branchNextPos, true, true, parentPos, true, branchNextDir });
		return true;
	}

	return false;
}


// random position within the volume of a sphere
vec3 SpaceColonizer::randomVec3() {
	float x = 2.0f * Rand::randFloat() - 1.0f;
	float y = 2.0f * Rand::randFloat() - 1.0f;
	float z = 2.0f * Rand::randFloat() - 1.0f;
	float rr = Rand::randFloat();
	float len = sqrtf(x*x + y*y + z*z);
	return vec3(rr * x / len, rr * y / len, rr * z / len);
}