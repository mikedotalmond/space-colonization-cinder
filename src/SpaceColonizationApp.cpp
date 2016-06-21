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

	sc.options.budPositions.resize(1);
	sc.options.budPositions[0] = Rand::randVec3();

	sc.options.hormonePositions.resize(1000);
	for (size_t i = 0; i < 1000; i++) {
		sc.options.hormonePositions[i] = Rand::randVec3();
	}

	lastBudCount = 0;
	sc.reset();
}


float phase = 0.0f;
void SpaceColonizerApp::update() {
	scData = sc.iterate();

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

	// still growing?
	if (n != lastBudCount) {
		for (size_t i = 0; i < n; i++) {
			Bud b = scData.buds[i];
			if (scData.buds[i].hasParent) {
				gl::drawLine(scData.buds[i].parentPos*100.0f, scData.buds[i].position*100.0f);
			}
		}
	}
	else {
		reset();
	}

	lastBudCount = n;
}

CINDER_APP(SpaceColonizerApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings *settings) {
	settings->setMultiTouchEnabled(false);
	settings->setWindowSize(640, 640);
	settings->setFrameRate(60);
})