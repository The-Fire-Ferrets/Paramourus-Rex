#include "TitleView.h"

//Event delegate holder
EventDelegate TitleView::delegate = NULL;
//Background
sf::Sprite TitleView::background;
//Texture and string
sf::Texture TitleView::background_texture;
std::string TitleView::playbutton_string;
//Button to player game
int TitleView::playbutton_size;
sf::Text TitleView::playbutton;
//Button to enter tutorial
std::string TitleView::tutorialbutton_string;
int TitleView::tutorialbutton_size;
sf::Text TitleView::tutorialbutton;
std::string TitleView::tutoriallevel;
//Button to enter introduction
std::string TitleView::introbutton_string;
int TitleView::introbutton_size;
std::string TitleView::introdialogue;
sf::Text TitleView::introbutton;
//Font
sf::Font TitleView::font;
//Determiend single button press
bool TitleView::pressed = false;
//View state
int TitleView::view_state = 1;

/** Creates the title from the give configuration file
 **
 **/
void TitleView::Create(const char* resource) {	
    //Holds referenced to loaded XML file		
    pugi::xml_document doc;

	if (delegate == NULL)
		delegate.bind(&TitleView::update);
    //Error check to see if file was loaded correctly
    pugi::xml_parse_result result;
    std::string resource_str(resource);
    if (!(result = doc.load_file(("./assets/" + resource_str + ".xml").c_str()))) {
        std::cout << "TitleView::CreateLevel(...): Failed to load" << std::endl;
        std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
    }

    //Used to iterate over XML file to get attributes
    pugi::xml_node tools = doc.child(resource);
    for (pugi::xml_attribute attr = tools.first_attribute(); attr; attr = attr.next_attribute()) {
        if (!strcmp(attr.name(), "Background")) {
            background_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
            background = sf::Sprite(background_texture, sf::IntRect(0, 0, background_texture.getSize().x, background_texture.getSize().y));
		background.setScale(1.0*Configuration::getWindowWidth()/background_texture.getSize().x, 1.0*Configuration::getWindowHeight()/background_texture.getSize().y);	
            background.setPosition(sf::Vector2f(0,0));
        }
	 else if (!strcmp(attr.name(), "Font")) {
			font.loadFromFile(("./assets/" + (std::string)attr.value()).c_str());
	    }
    }
	//Iterates over XML to get components to add
	for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling()) {
		char* temp;	
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!strcmp(tool.name(), "PlayButton") && !strcmp(attr.name(), "Text")) {
				playbutton_string = attr.value();
				playbutton = sf::Text(playbutton_string, font);
			}
			else  if (!strcmp(tool.name(), "PlayButton") && !strcmp(attr.name(), "Size")) {	
				playbutton_size =std::strtol(attr.value(), &temp, 10);
				playbutton.setCharacterSize(playbutton_size);
				playbutton.setPosition(Configuration::getWindowWidth()/2 - playbutton.getGlobalBounds().width/2, Configuration::getWindowHeight()/2  - playbutton.getGlobalBounds().height/2 - 20);
				if (*temp != '\0') {
					std::cout << "TitleView::Create: Error reading attribute for " << tool.name() << " " << attr.name() << std::endl;
				}
			}
			else if (!strcmp(tool.name(), "TutorialButton") && !strcmp(attr.name(), "Text")) {
				tutorialbutton_string = attr.value();
				tutorialbutton = sf::Text(tutorialbutton_string, font);
			}
			else  if (!strcmp(tool.name(), "TutorialButton") && !strcmp(attr.name(), "Size")) {	
				tutorialbutton_size =std::strtol(attr.value(), &temp, 10);
				tutorialbutton.setCharacterSize(tutorialbutton_size);
				tutorialbutton.setPosition(Configuration::getWindowWidth()/2 - tutorialbutton.getGlobalBounds().width/2 + 300, Configuration::getWindowHeight()/2  - tutorialbutton.getGlobalBounds().height/2);
				if (*temp != '\0') {
					std::cout << "TitleView::Create: Error reading attribute for " << tool.name() << " " << attr.name() << std::endl;
				}
			}
			else if (!strcmp(tool.name(), "TutorialButton") && !strcmp(attr.name(), "Level")) {
				tutoriallevel = attr.value();
			}
			else if (!strcmp(tool.name(), "IntroButton") && !strcmp(attr.name(), "Text")) {
				introbutton_string = attr.value();
				introbutton = sf::Text(introbutton_string, font);
			}
			else if (!strcmp(tool.name(), "IntroButton") && !strcmp(attr.name(), "Dialogue")) {
				introdialogue = attr.value();
			}
			else  if (!strcmp(tool.name(), "IntroButton") && !strcmp(attr.name(), "Size")) {	
				introbutton_size =std::strtol(attr.value(), &temp, 10);
				introbutton.setCharacterSize(tutorialbutton_size);
				introbutton.setPosition(Configuration::getWindowWidth()/2 - introbutton.getGlobalBounds().width/2 - 300, Configuration::getWindowHeight()/2  - introbutton.getGlobalBounds().height/2);
				if (*temp != '\0') {
					std::cout << "TitleView::Create: Error reading attribute for " << tool.name() << " " << attr.name() << std::endl;
				}
			}
		}
	}	
}

/** Checks to see if level was clicked on and switches to it
 **
 **/
void TitleView::update(sf::RenderWindow *window, int* state, float time) {
	EventManagerInterface::setViewDelegate(delegate);
	EventManagerInterface::setCurrentActorList(NULL);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed && view_state == 1) {
        pressed = true;
        const sf::Vector2i pos = sf::Mouse::getPosition(*window);
	    if (playbutton.getGlobalBounds().contains(pos.x, pos.y)) {
		view_state = 0;
		MapView::level_idx = 10;
		MapView::view_state = 1;
		MapView::commentary_idx = 1;
		*state = 0;
		view_state = 1;
	    }
		else if (introbutton.getGlobalBounds().contains(pos.x, pos.y)) {
			view_state = 0;
			render(window);
			DialogueView::Create(introdialogue.c_str(), state);
			*state = 2;
			view_state = 1;
		}
		else if (tutorialbutton.getGlobalBounds().contains(pos.x, pos.y)) {
			view_state = 0;
			MapView::commentary_idx = 0;
			render(window);
			int fireflowers_count = 1;
			int earthflowers_count = 1;
			int airflowers_count = 1;
			int waterflowers_count = 1;
			int flowers[] = {fireflowers_count, earthflowers_count, airflowers_count, waterflowers_count};
			LevelView::Create(tutoriallevel.c_str(), state, flowers);
			LevelView::start();
			*state = 1;
			view_state = 1;
		}
		
    }
    else if (!(sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
        pressed = false;
    }
}

/** Checks for events and update accordingly
 **
 **/
void TitleView::update(EventInterfacePtr e) {

}

/** Renders the title onto the window
 **
 **/
void TitleView::render(sf::RenderWindow *window) {
	if (view_state == 0) {
		window->clear(sf::Color::White);
		window->draw(Configuration::getLoadingSprite());
		window->display();
	}
	else if (view_state == 1) {	
		window->draw(background);
		window->draw(playbutton);
		window->draw(introbutton);
		window->draw(tutorialbutton);
	}
}
