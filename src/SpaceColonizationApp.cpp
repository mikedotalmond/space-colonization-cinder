#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "SpaceColonizer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SpaceColonizerApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	void reset();

	SpaceColonizer sc;
	SCData scData;
	int lastBudCount = 0;
	bool completed = false;
	int completeCount = 0;
	float phase = 0.0f;

	gl::GlslProgRef defaultShader;
	CameraPersp		mCamera;
};

void SpaceColonizerApp::setup() {
	sc = SpaceColonizer();
	defaultShader = gl::context()->getStockShader(gl::ShaderDef().color());
	mCamera = CameraPersp();
	mCamera.setPerspective(70, 1280/720, 1.0f, 1000.0f); // default CameraPersp setting
	mCamera.lookAt(vec3(0,0,-200), vec3());
	reset();
}


void SpaceColonizerApp::mouseDown(MouseEvent event) {
	reset();
}


void SpaceColonizerApp::reset(){

	completeCount = 0;
	completed = false;

	sc.options.budPositions.resize(1);
	sc.options.budPositions[0] = Rand::randVec3();// *vec3(1, 1, 0);

	sc.options.hormonePositions.resize(1000);
	for (size_t i = 0; i < 1000; i++) {
		sc.options.hormonePositions[i] = Rand::randVec3();// *vec3(1, 1, 0);
	}

	lastBudCount = 0;
	sc.reset();
}


void SpaceColonizerApp::update() {
	
	if(!completed) scData = sc.iterate();

	// still growing?
	size_t n = scData.buds.size();
	if (n > 0 && n == lastBudCount) {
		completed = true;
		completeCount++;
		if (completeCount > 240) {
			reset();
		}
	}
	lastBudCount = n;


	vec3 p = vec3(sinf(phase), 0.0f, cosf(phase)) * 200.0f;
	mCamera.lookAt(p, vec3());

	phase += .01f;
}


void SpaceColonizerApp::draw(){

	gl::ScopedGlslProg lineScope(defaultShader);
	gl::ScopedMatrices matScope;
	gl::setMatrices(mCamera);

	gl::clear( Color( .1f, .1f, .1f) );

	gl::lineWidth(1.0f); 
	gl::color(1.0f, 1.0f, 1.0f, 1.0f);

	size_t n = scData.buds.size();
	for (size_t i = 0; i < n; i++) {
		Bud b = scData.buds[i];
		if (scData.buds[i].hasParent) {
			gl::drawLine(scData.buds[i].parentPos*100.0f, scData.buds[i].position*100.0f);
		}
	}

}

CINDER_APP(SpaceColonizerApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings *settings) {
	settings->setMultiTouchEnabled(false);
	settings->setWindowSize(640, 640);
	settings->setFrameRate(60);
})