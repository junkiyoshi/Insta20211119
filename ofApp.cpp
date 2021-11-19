#include "ofApp.h"	

//--------------------------------------------------------------
Actor::Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = ofRandom(location_list.size());
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;
}

//--------------------------------------------------------------
void Actor::update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	if (ofGetFrameNum() % frame_span == 0) {

		auto tmp_index = this->select_index;
		this->select_index = this->next_index;
		int retry = next_index_list[this->select_index].size();
		this->next_index = next_index_list[this->select_index][(int)ofRandom(next_index_list[this->select_index].size())];
		while (--retry > 0) {

			auto destination_itr = find(destination_list.begin(), destination_list.end(), this->next_index);
			if (destination_itr == destination_list.end()) {

				if (tmp_index != this->next_index) {

					destination_list.push_back(this->next_index);
					break;
				}
			}

			this->next_index = next_index_list[this->select_index][(this->next_index + 1) % next_index_list[this->select_index].size()];
		}
		if (retry <= 0) {

			destination_list.push_back(this->select_index);
			this->next_index = this->select_index;
		}
	}

	auto param = ofGetFrameNum() % frame_span;
	auto distance = location_list[this->next_index] - location_list[this->select_index];
	this->location = location_list[this->select_index] + distance / frame_span * param;

	this->log.push_front(this->location);
	while (this->log.size() > 10) { this->log.pop_back(); }
}

//--------------------------------------------------------------
glm::vec3 Actor::getLocation() {

	return this->location;
}

//--------------------------------------------------------------
deque<glm::vec3> Actor::getLog() {

	return this->log;
}

//--------------------------------------------------------------
void Actor::setColor(ofColor color) {

	this->color = color;
}

//--------------------------------------------------------------
ofColor Actor::getColor() {

	return this->color;
}


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(0);
	ofSetRectMode(ofRectMode::OF_RECTMODE_CENTER);

	ofSeedRandom(39);

	ofFbo fbo;
	ofPixels pixels;
	fbo.allocate(ofGetWidth(), ofGetHeight());
	fbo.begin();
	ofClear(0);

	this->font.loadFont("fonts/Kazesawa-bold.ttf", 300, true, true, true);
	string word = "18";
	this->font.drawString(word, ofGetWidth() * 0.5 - font.stringWidth(word) * 0.5, ofGetHeight() * 0.5 - font.stringHeight(word) * 0.05);
	word = "00";
	this->font.drawString(word, ofGetWidth() * 0.5 - font.stringWidth(word) * 0.5, ofGetHeight() * 0.5 + font.stringHeight(word) * 1.05);

	fbo.end();
	fbo.readToPixels(pixels);

	ofColor pix_color;
	int span = 16;
	for (float x = 0 + span * 0.5; x < ofGetWidth(); x += span) {

		for (float y = 0 + span * 0.5; y <= ofGetHeight(); y += span) {

			pix_color = pixels.getColor(x, y);
			if (pix_color != ofColor(0, 0)) {

				this->location_list.push_back(glm::vec3(x - ofGetWidth() * 0.5, y - ofGetHeight() * 0.5, 0));
			}
		}
	}

	for (auto& location : this->location_list) {

		vector<int> next_index = vector<int>();
		int index = -1;
		for (auto& other : this->location_list) {

			index++;
			if (location == other) { continue; }

			float distance = glm::distance(location, other);
			if (distance <= span) {

				next_index.push_back(index);
			}
		}

		this->next_index_list.push_back(next_index);
	}

	ofColor color;
	vector<int> hex_list = { 0xaaff00, 0xffaa00, 0xff00aa, 0xaa00ff, 0x00aaff };
	vector<ofColor> base_color_list;
	for (auto hex : hex_list) {

		color.setHex(hex);
		base_color_list.push_back(color);
	}

	for (int i = 0; i < 500; i++) {

		this->actor_list.push_back(make_unique<Actor>(this->location_list, this->next_index_list, this->destination_list));
		this->actor_list.back()->setColor(base_color_list[(int)ofRandom(base_color_list.size())]);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_span = 10;
	int prev_index_size = 0;

	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.5);

	ofSetColor(128, 128);
	ofSetLineWidth(1);
	for (auto& location : this->location_list) {

		ofDrawCircle(location, 2);
	}

	ofSetLineWidth(3);
	for (auto& actor : this->actor_list) {

		// 非常に手抜きな実装。
		// 枠があれば、きっとカッコいいけど、今日はここまで。
		ofSetColor(actor->getColor());
		for (auto& l : actor->getLog()) {

			//ofDrawRectangle(l, 10, 10);
			ofDrawCircle(l, 5);
		}
	}
}


//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}