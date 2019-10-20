#include <iostream>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 900), "SFML Double pendulum");
	tgui::Gui gui(window);
	// tgui::TextBox::Ptr test = tgui::TextBox::create();
	// test->setPosition(0, 0);
	// test->setSize(100, 50);
	// gui.add(test);
	

	double angle1 = 90.f;
	double angle2 = 45.f;
	double angle1_acc = 0;
	double angle2_acc = 0;
	double angle1_vel = 0;
	double angle2_vel = 0;
	double length1 = 0.1;
	double length2 = 0.1;
	double maxMass = 1;
	double mass1 = 1;
	double mass2 = 0.2;
	double g = 9.81;
	double scale = 1000;
	double timeScale = 1.0/5;
	double maxLenght = 0.2;
	double minLenght = 0.02;
	float lineThickness = 5;
	sf::Vector2u windowSize = window.getSize();
	
	sf::Font font;
	if(!font.loadFromFile("resources/Mermaid1001.ttf")){
		std::cout << "Could not load font" << std::endl;
	}

	auto horLayout1 = tgui::HorizontalLayout::create();
	horLayout1->setSize("90%", 50);
	horLayout1->setPosition("5%", "&.height - 100");
	// gui.add(horLayout1);
	
	auto horLayout2 = tgui::HorizontalLayout::create();
	horLayout2->setSize("90%", 50);
	horLayout2->setPosition("5%", "&.height - 200");
	// gui.add(horLayout2);
	
	auto vertLayout = tgui::VerticalLayout::create();
	vertLayout->setSize("90%", 200);
	vertLayout->setPosition("5%", "&.height - 200");
	vertLayout->add(horLayout1);
	vertLayout->add(horLayout2);
	gui.add(vertLayout);
	
	sf::Text massText;
	massText.setFont(font);
	massText.setString("Mass");
	massText.setPosition(50, windowSize.y - 150);
	massText.setFillColor(sf::Color::Black);
	auto massSlider1 = tgui::Slider::create(0.1f, maxMass);
	// massSlider1->setPosition({"10%", "80%"});
	// massSlider1->setSize("20%", 15);
	massSlider1->setStep(0.005);
	horLayout1->add(massSlider1);
	// gui.add(massSlider1);
	tgui::Slider::Ptr massSlider2 = tgui::Slider::create(0.01f, maxMass);
	// massSlider2->setPosition({"10%", "85%"});
	// massSlider2->setSize("20%", 15);
	massSlider2->setStep(0.005);
	horLayout1->add(massSlider2);
	// gui.add(massSlider2);
	
	sf::Text lengthText;
	lengthText.setFont(font);
	lengthText.setString("Length");
	lengthText.setPosition(270, windowSize.y - 150);
	lengthText.setFillColor(sf::Color::Black);
	tgui::Slider::Ptr lengthSlider1 = tgui::Slider::create(minLenght, maxLenght);
	lengthSlider1->setPosition(270, windowSize.y - 100);
	lengthSlider1->setSize(180, 15);
	lengthSlider1->setStep(0.0005);
	lengthSlider1->setValue(length1);
	horLayout2->add(lengthSlider1);
	// gui.add(lengthSlider1);
	tgui::Slider::Ptr lengthSlider2	= tgui::Slider::create(minLenght, maxLenght);
	lengthSlider2->setPosition(270, windowSize.y - 50);
	lengthSlider2->setSize(180, 15);
	lengthSlider2->setStep(0.0005);
	lengthSlider2->setValue(length2);
	horLayout2->add(lengthSlider2);
	// gui.add(lengthSlider2);

	// sf::Texture pause_tex;
	// if(!pause_tex.loadFromFile("resources/icon_pause.png")){
		// std::cout << "Pause icon could not be found" << std::endl;
	// }

	bool paused = false;
	tgui::Texture pause_tex("resources/icon_pause.png");
	tgui::Picture::Ptr pause_img = tgui::Picture::create(pause_tex);
	pause_img->setPosition(20, 20);
	pause_img->connect("clicked", [& paused](){
			paused = !paused;
			if (paused){
				std::cout << "pause" << std::endl;
			}
			else{
				std::cout << "play" << std::endl;
			}
			});
	// pause_img->connect("MouseEntered", [window](){});
	// pause_img->connect("MouseLeft", shadeButton);

	// tgui::Button::Ptr pause_button = tgui::Button::create();
	// pause_button->setPosition(20, 20);
	// pause_button->setSize(pause_img->getSize());

	// gui.add(pause_button);
	gui.add(pause_img);
	// sf::Sprite pause_sprite;
	// pause_sprite.setTexture(pause_tex);
	// pause_sprite.setPosition(20, 20);
	
	sf::Vector2f startPos(250, 100);
	

	sf::Clock timer;
	double deltaTime;
	while (window.isOpen())
	{
		// masses
		mass1 = massSlider1->getValue();
		mass2 = massSlider2->getValue();
		double mass1Percent = mass1/maxMass;
		double mass2Percent = mass2/maxMass;
		sf::CircleShape weight1(std::max(10., mass1/maxMass * 15.));
		sf::Color weigth1Color(100*mass1Percent, 255 * (1-mass1Percent), 255 * (1-mass1Percent));
		weight1.setFillColor(weigth1Color);
		weight1.setOrigin(weight1.getRadius(), weight1.getRadius());
		weight1.setPointCount(100);

		sf::CircleShape weight2(std::max(10., mass2/maxMass * 15.));
		sf::Color weigth2Color(100*mass2Percent, 255 * (1-mass2Percent), 255 * (1-mass2Percent));
		weight2.setFillColor(weigth2Color);
		weight2.setOrigin(weight2.getRadius(), weight2.getRadius());
		weight2.setPointCount(100);

		//lenghts
		length1 = lengthSlider1->getValue();
		length2 = lengthSlider2->getValue();
		sf::RectangleShape firstLine (sf::Vector2f(lineThickness, length1*scale));
		firstLine.setOrigin(lineThickness / 2., 0);
		firstLine.setPosition(startPos.x, startPos.y);
		firstLine.setFillColor(sf::Color::Black);

		sf::RectangleShape secondLine (sf::Vector2f(lineThickness, length2*scale));
		secondLine.setOrigin(lineThickness / 2., 0);
		secondLine.setFillColor(sf::Color::Black);

		
		// std::stringstream mousePos;
		// mousePos << sf::Mouse::getPosition(window).x << " " << sf::Mouse::getPosition(window).y;
		// test->setText(mousePos.str());
		deltaTime = timeScale * timer.restart().asSeconds();
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type){
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::Resized:
					window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
					gui.setView(window.getView());
				break;

				default:
					break;
			}
			gui.handleEvent(event);
		}
		if(!paused){
			// from myphsicslab
			double top1 = -g *(2*mass1 + mass2)* sin(angle1) - mass2*g*sin(angle1 - 2*angle2)
				- 2 * sin(angle1-angle2)*mass2*(angle2_vel*angle2_vel*length2 + angle1_vel*angle1_vel*length1*cos(angle1-angle2));
			double top2 = 2*sin(angle1-angle2)*( (angle1_vel*angle1_vel*length1*(mass1+mass2))
					+ g*(mass1+mass2)*cos(angle1) + angle2_vel*angle2_vel*length2*mass2*cos(angle1-angle2) );
			double bottomPart = (2*mass1 + mass2 - mass2*cos(2*(angle1 - angle2)));
			angle1_acc = top1 / (length1 * bottomPart);
			angle2_acc = top2 / (length2 * bottomPart);
			angle1 += deltaTime * angle1_vel;
			angle2 += deltaTime * angle2_vel;
			angle1_vel = angle1_vel + deltaTime * angle1_acc;
			angle2_vel = angle2_vel + deltaTime * angle2_acc;
		}
		//angle1 = 0;
		//angle2 = 45.0 / 180.0 * 3.14;
		window.clear(sf::Color(193, 193, 193));
		// sf::Vector2f firstPos (startPos.x + length1 * scale * sin(angle1), startPos.y + length1 * scale * cos(angle1));
		// sf::Vector2f secondPos (firstPos.x + length2 * scale * sin(angle2), firstPos.y + length2 * scale * cos(angle2));
		// std::vector<sf::Vertex> line =
		// {
			// sf::Vertex(startPos),
			// sf::Vertex(firstPos),
			// sf::Vertex(secondPos)
		// };
		// window.draw(&line[0], line.size(), sf::LineStrip);
		sf::Vector2f firstEndPos (startPos.x + length1 * scale * sin(angle1), startPos.y + length1 * scale * cos(angle1));
		sf::Vector2f secondEndPos (firstEndPos.x + length2 * scale * sin(angle2), firstEndPos.y + length2 * scale * cos(angle2));
		firstLine.setRotation(-angle1/M_PI*180.f);
		secondLine.setRotation(-angle2/M_PI*180.f);
		secondLine.setPosition(firstEndPos.x, firstEndPos.y);
		weight1.setPosition(firstEndPos);
		weight2.setPosition(secondEndPos);
		window.draw(firstLine);
		window.draw(secondLine);
		window.draw(weight1);
		window.draw(weight2);
		// window.draw(massText);
		// window.draw(lengthText);
		gui.draw();
		window.display();
	}
}
