#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;


// Enumerator of shape types
enum class Type { Circle, Square, Rectangle };

// Custom shape class
class Shape {
	public:
		virtual void displayProperties() = 0;	// abstract function to display extra properties
		virtual void drawShape() = 0;			// abstract function to draw shape
		virtual bool inBorders(vec2 pos) = 0;	// abstract function to check if pos inside shape
		vec2 loc;							    // Stores x and y location
		Color col;								// (r, g, b) color values
		Type t_;
		
		Shape(vec2 l, Color c, Type t) {
			loc = l;
			col = c;
			t_ = t;
		}

		// Moves shape center in direction dir
		void move(vec2 dir) {
			loc.x += dir.x;
			loc.y -= dir.y;
		}
};

class Circle : public Shape {
	public:
		float radius;

		Circle(vec2 xy, Color rgb, float rad) : Shape { xy, rgb, Type::Circle } { radius = rad; }
		
		void drawShape() {
			gl::color(col);
			gl::drawSolidCircle(loc, radius);
		}

		// Checks if pos in borders based on eucledean distance from center
		bool inBorders(vec2 pos) {
			float dist = sqrtf(powf(pos.x - loc.x, 2.0f) + powf(pos.y - loc.y, 2.0f));
			return dist <= radius;
		}

		void displayProperties() {
			ImGui::Text("Type: Circle");
			ImGui::Text("Radius: %.2f", radius);
		}
};

class Square : public Shape {
	public:
		float size;
		Square(vec2 xy, Color rgb, float sz) :
			Shape{ xy, rgb, Type::Square } { size = sz; }
		
		void drawShape() {
			gl::color(col);
			gl::drawSolidRect(Rectf(loc.x - size/2, loc.y - size/2, loc.x + size/2, loc.y + size/2));
		}

		bool inBorders(vec2 pos) {
			return pos.x > loc.x - size / 2 && pos.x < loc.x + size / 2 && pos.y > loc.y - size / 2 && pos.y < loc.y + size / 2;
		}

		void displayProperties() {
			ImGui::Text("Type: Square");
			ImGui::Text("Size: %.2f", size);
		}
};

class Rectangle : public Shape {
	public:
		float w; 
		float h;

		Rectangle(vec2 xy, Color rgb, float wd, float ht) :
			Shape{ xy, rgb, Type::Rectangle } { w = wd; h = ht; }
		
		void drawShape() {
			gl::color(col);
			gl::drawSolidRect(Rectf(loc.x - w / 2, loc.y - h / 2, loc.x + w / 2, loc.y + h / 2));
		}
		
		bool inBorders(vec2 pos) {
			return pos.x > loc.x - w / 2 && pos.x < loc.x + w / 2 && pos.y > loc.y - h / 2 && pos.y < loc.y + h / 2;
		}

		void displayProperties() {
			ImGui::Text("Type: Rectangle");
			ImGui::Text("Width: %.2f", w);
			ImGui::Text("Height: %.2f", h);
		}
};

static void showUI(std::vector<Shape*> shapes) {
	static int selected = -1;

	if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Escape)))
		selected = -1;

	ImGui::SetNextWindowSize(ImVec2(500, 440));
	if (ImGui::Begin("Simple layout", NULL, ImGuiWindowFlags_MenuBar)) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Save")) {

				}
				if (ImGui::MenuItem("Open")) {

				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// List
		{
			ImGui::BeginChild("List", ImVec2(150, 0), true);
			for (int i = 0; i < shapes.size(); i++)
			{
				char label[128];
				sprintf(label, "Shape %d", i);
				if (ImGui::Selectable(label, selected == i))
					selected = i;
			}
			ImGui::EndChild();
		}
		ImGui::SameLine();

		// Right
		if (selected > -1) {
			ImGui::BeginGroup();
			ImGui::BeginChild("Properties", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("Shape: %d", selected);
			ImGui::Separator();

			Shape* cur = shapes[selected];
			ImGui::Text("Location: (%.0f,%.0f)", cur->loc.x, cur->loc.y);
			ImGui::Text("Color in HSV: (%.2f, %.2f, %.2f)", cur->col.r, cur->col.g, cur->col.b);
			cur->displayProperties();

			ImGui::EndChild();
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}

// We'll create a new Cinder Application by deriving from the App class.
class BasicApp : public App {
	public:
		void mouseDown( MouseEvent event) override;

		void keyDown( KeyEvent event ) override;

		void draw() override;

		void setup() override;

	private:
		std::vector<Shape*> shapes;
};

void prepareSettings( BasicApp::Settings* settings ) {
	settings->setMultiTouchEnabled( false );
}

void BasicApp::mouseDown( MouseEvent event) {
	// On left mouse button create a circle
	if (event.isLeft()) {
		Shape* pShape = new Circle(event.getPos(), Color::hex(Rand::randUint()), Rand::randFloat(1.0f, 100.0f));
		shapes.push_back(pShape);
	}
	// On right mouse button delete a circle
	else if (event.isRight()) {

		// We proceed in reverse order to always delete the topmost shape
		for (int i = shapes.size()-1; 0 <= i; i--)  {

			// For each shape check if clicked
			if (shapes[i]->inBorders(event.getPos())) {
				shapes.erase(shapes.begin()+i);
				break;
			}
		}
	}
}

void BasicApp::keyDown( KeyEvent event ) {
	if (event.getCode() == KeyEvent::KEY_w) {
		shapes[shapes.size() - 1]->move(vec2(0.0f, -10.0f));
	}
	else if (event.getCode() == KeyEvent::KEY_s) {
		shapes[shapes.size() - 1]->move(vec2(0.0f, 10.0f));
	}
	else if (event.getCode() == KeyEvent::KEY_a) {
		shapes[shapes.size() - 1]->move(vec2(-10.0f, 0.0f));
	}
	else if (event.getCode() == KeyEvent::KEY_d) {
		shapes[shapes.size() - 1]->move(vec2(10.0f, 0.0f));
	}
}

void BasicApp::draw(){
	gl::clear( Color::gray( 0.1f ) );

	BasicApp:showUI(shapes);

	for (int i = 0; i < shapes.size(); i++) {
		shapes[i]->drawShape();
	}
}

void BasicApp::setup() {
	ImGui::Initialize();
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl, prepareSettings )
