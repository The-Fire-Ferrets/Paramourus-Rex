#include "TitleView.h"

sf::Sprite TitleView::background;
sf::Texture TitleView::background_texture;
std::string TitleView::playbutton_string;
int TitleView::playbutton_size;
sf::Text TitleView::playbutton;
std::string TitleView::tutorialbutton_string;
int TitleView::tutorialbutton_size;
sf::Text TitleView::tutorialbutton;
std::string TitleView::introbutton_string;
int TitleView::introbutton_size;
sf::Text TitleView::introbutton;
sf::Font TitleView::font;
bool TitleView::playbutton_pressed = false;
/** Creates the title from the give configuration file
 **
 **/
void TitleView::Create(const char* resource) {	
    //Holds referenced to loaded XML file		
    pugi::xml_document doc;

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
            background = sf::Sprite(background_texture, sf::IntRect(0, 0, Configuration::getWindowWidth(), Configuration::getWindowHeight()));
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
				playbutton.setPosition(Configuration::getWindowWidth()/2 - playbutton.getGlobalBounds().width/2, Configuration::getWindowHeight()/2  - playbutton.getGlobalBounds().height/2 - 50);
				if (*temp != '\0') {
					std::cout << "TitleView::Create: Error reading attribute for " << tool.name() << " " << attr.name() << std::endl;
				}
			}
			else if (!strcmp(tool.name(), "TutorialButton") && !strcmp(attr.name(), "Text")) {
				tutorialbutton_string = attr.value();
				tutorialbutton = sf::Text(Tutorialbutton_string, font);
			}
			else  if (!strcmp(tool.name(), "TutorialButton") && !strcmp(attr.name(), "Size")) {	
				tutorialbutton_size =std::strtol(attr.value(), &temp, 10);
				tutorialbutton.setCharacterSize(tutorialbutton_size);
				tutorialbutton.setPosition(Configuration::getWindowWidth()/2 - tutorialbutton.getGlobalBounds().width/2, Configuration::getWindowHeight()/2  - tutorialbutton.getGlobalBounds().height/2 - 50);
				if (*temp != '\0') {
					std::cout << "TitleView::Create: Error reading attribute for " << tool.name() << " " << attr.name() << std::endl;
				}
			}
			else if (!strcmp(tool.name(), "IntroButton") && !strcmp(attr.name(), "Text")) {
				introbutton_string = attr.value();
				introbutton = sf::Text(Tutorialbutton_string, font);
			}
			else  if (!strcmp(tool.name(), "IntroButton") && !strcmp(attr.name(), "Size")) {	
				introbutton_size =std::strtol(attr.value(), &temp, 10);
				introbutton.setCharacterSize(tutorialbutton_size);
				introbutton.setPosition(Configuration::getWindowWidth()/2 - introbutton.getGlobalBounds().width/2 - 50, Configuration::getWindowHeight()/2  - introbutton.getGlobalBounds().height/2 - 50);
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
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !playbutton_pressed) {
        playbutton_pressed = true;
        const sf::Vector2i pos = sf::Mouse::getPosition(*window);
	    if (playbutton.getGlobalBounds().contains(pos.x, pos.y)) {
		*state = 0;
		/*if (i > 1) {
			view_state = 0;
			render(window);
			int flowers[] = {fireflowers_count[i], earthflowers_count[i], airflowers_count[i], waterflowers_count[i]};
			LevelView::Create(levels[i].c_str(), state, flowers);
			DialogueView::Create(levels[i].c_str(), state);
			LevelView::start();
			reset = true;
			view_state = 1;
			*state = 1;
		}
		else {
			view_state = 0;
			render(window);
			reset = true;
			CraftView::Create("Craft",state);
			view_state = 1;
			*state = 3;
		}*/
	    }
    }
    else if (!(sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
        playbutton_pressed = false;
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
	window->draw(background);
	window->draw(playbutton);
}
