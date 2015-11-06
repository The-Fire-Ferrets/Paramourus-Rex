#include "MapView.h"

EventDelegate MapView::delegate = NULL;
//Maxmium number of levels
const int MapView::size = 20;
//Total number of levels
int MapView::num_levels = 0;
//Level names
std::string MapView::levels[size];
//Level sprites
sf::Sprite MapView::sprites[size];
//Level textures
sf::Texture MapView::textures[size];
//Level sprite positions
sf::Vector2f MapView::positions[size];
//Level sprite sizes
sf::Vector2f MapView::sizes[size];
//Map Background sprite
sf::Sprite MapView::background;
//Hold probability of flowers
float MapView::fireflowers[size];
float MapView::earthflowers[size];
float MapView::airflowers[size];
float MapView::waterflowers[size];
int MapView::fireflowers_count[size];
int MapView::earthflowers_count[size];
int MapView::airflowers_count[size];
int MapView::waterflowers_count[size];
std::string MapView::flowers_string[size];
sf::Text MapView::flowers_text[size];
sf::Font MapView::font;
//Number flowers
int MapView::flowers;
//Map Background texture
sf::Texture MapView::background_texture;
//Make sure to respond to only 1 click
bool MapView::pressed = false;
//Reset population values
bool MapView::reset = true;
//View state 0: Loading, 1: Normal
int MapView::view_state = 1;
//Back button to title screen
sf::Sprite MapView::title_sprite;
sf::Texture MapView::title_texture;
sf::Vector2f MapView::title_size;
/** Creates the map from the give configuration file
 **
 **/
void MapView::Create(const char* resource) {
    //Holds referenced to loaded XML file		
    pugi::xml_document doc;
	if (delegate == NULL)
		delegate.bind(&MapView::update);
    //Error check to see if file was loaded correctly
    pugi::xml_parse_result result;
    std::string resource_str(resource);
    if (!(result = doc.load_file(("./assets/" + resource_str + ".xml").c_str()))) {
        std::cout << "LevelView::CreateLevel(...): Failed to load" << std::endl;
        std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
    }
	 char* temp;
    //Used to iterate over XML file to get attributes
    pugi::xml_node tools = doc.child(resource);
    for (pugi::xml_attribute attr = tools.first_attribute(); attr; attr = attr.next_attribute()) {
        if (!strcmp(attr.name(), "Background")) {
            background_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
            background = sf::Sprite(background_texture, sf::IntRect(0, 0, Configuration::getWindowWidth(), Configuration::getWindowHeight()));
            background.setPosition(sf::Vector2f(0,0));
        }
	else if (!strcmp(attr.name(), "TitleSprite")) {
            title_texture.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
        }
	else if (!strcmp(attr.name(), "TitleHeight")) {
            title_size.y = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "MapView::CreateMap: Error reading attribute for " << attr.name() << std::endl;
                }
        }
	else if (!strcmp(attr.name(), "TitleWidth")) {
            title_size.x = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "MapView::CreateMap: Error reading attribute for " << attr.name() << std::endl;
                }
        }
	 else if (!strcmp(attr.name(), "Font")) {
			font.loadFromFile(("./assets/" + (std::string)attr.value()).c_str());
	    }
    }

	title_sprite = sf::Sprite(title_texture, sf::IntRect(0, 0, title_texture.getSize().x, title_texture.getSize().y));
	title_sprite.setScale(title_size.x/(title_texture.getSize()).x, title_size.y/(title_texture.getSize()).y);
            title_sprite.setPosition(sf::Vector2f(0,Configuration::getWindowHeight() - title_sprite.getGlobalBounds().height));

    //Iterates over XML to get components to add
    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling(), num_levels++) {
        levels[num_levels] = tool.name();	
        for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
            if (!strcmp(attr.name(), "Sprite")) {
                textures[num_levels].loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
            }
            else if (!strcmp(attr.name(), "X")) {
                positions[num_levels].x = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "MapView::CreateMap: Error reading attribute for " << tool.name() << std::endl;
                }
            }
            else if (!strcmp(attr.name(), "Y")) {
                positions[num_levels].y =std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "MapView::CreateMap: Error reading attribute for " << tool.name() << std::endl;
                }
            }
            else if (!strcmp(attr.name(),"Width")) {
                sizes[num_levels].x = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
                }
            }
            else if (!strcmp(attr.name(),"Height")) {
                sizes[num_levels].y = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
                }
            }
	    else if (!strcmp(attr.name(),"Height")) {
                sizes[num_levels].y = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
                }
            }
	    else if (!strcmp(attr.name(),"Height")) {
                sizes[num_levels].y = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
                }
            }
	    else if (!strcmp(attr.name(),"FireFlower")) {
                fireflowers[num_levels] = std::strtof(attr.value(), &temp);
            }
	    else if (!strcmp(attr.name(),"AirFlower")) {
                airflowers[num_levels] = std::strtof(attr.value(), &temp);
            }
	    else if (!strcmp(attr.name(),"WaterFlower")) {
                waterflowers[num_levels] = std::strtof(attr.value(), &temp);
            }
	    else if (!strcmp(attr.name(),"EarthFlower")) {
                earthflowers[num_levels] = std::strtof(attr.value(), &temp);
            }
        }

    }

    for (int i = 0; i < num_levels; i++) {
        sprites[i] = sf::Sprite(textures[i], sf::IntRect(0, 0, textures[i].getSize().x, textures[i].getSize().y));
        sprites[i].setScale(sizes[i].x/(textures[i].getSize()).x, sizes[i].y/(textures[i].getSize()).y);
	sprites[i].setPosition(positions[i]);
    }

}

/** Checks to see if level was clicked on and switches to it
 **
 **/
void MapView::update(sf::RenderWindow *window, int* state, float time) {
	EventManagerInterface::setViewDelegate(delegate);
	EventManagerInterface::setCurrentActorList(NULL);
    if (reset) {
	view_state = 0;
	render(window);
	resetPopulationValues();
	view_state = 1;
    }	

    if (LevelView::player == NULL) {
	view_state = 0;
	render(window);
	int flowers_temp[] = {fireflowers_count[1], earthflowers_count[1], airflowers_count[1], waterflowers_count[1]};
	LevelView::Create(levels[1].c_str(), state, flowers_temp);
	view_state = 1;
	LevelView::cleanUp();
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed && view_state == 1) {
        pressed = true;
        const sf::Vector2i pos = sf::Mouse::getPosition(*window);
        for (int i = 0; i < num_levels; i++) {
            if (sprites[i].getGlobalBounds().contains(pos.x, pos.y)) {
		if (i > 1) {
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
		}
            }
		else if (title_sprite.getGlobalBounds().contains(pos.x, pos.y)) {
		*state = 5;
		LevelView::player->reset();
            }
        }
    }
    else if (!(sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
        pressed = false;
    }
}

/** Resets population values
 **
**/
void MapView::resetPopulationValues(void) {
	flowers = rand() % 20 + 5;
	for (int i = 0; i < num_levels; i++) {
		if (i > 1) {
			fireflowers_count[i] = (int)(flowers * fireflowers[i]);
			earthflowers_count[i] = (int)(flowers * earthflowers[i]);
			waterflowers_count[i] = (int)(flowers * waterflowers[i]);
			airflowers_count[i] = (int)(flowers * airflowers[i]);
			flowers_string[i] = "F:" + std::to_string(fireflowers_count[i]) + " E:" + std::to_string(earthflowers_count[i]) + " A:" + std::to_string(airflowers_count[i]) + " W:" + std::to_string(waterflowers_count[i]);
			flowers_text[i] = sf::Text(flowers_string[i], font);
			flowers_text[i].setCharacterSize(15);
			flowers_text[i].setStyle(sf::Text::Bold);
			flowers_text[i].setColor(sf::Color::Black);
			flowers_text[i].setPosition(positions[i] + sf::Vector2f(sizes[i].x/2 - flowers_text[i].getGlobalBounds().width/2, -1.5*flowers_text[i].getGlobalBounds().height));
		}
	}
	reset = false;
}
/** Checks for events and update accordingly
 **
 **/
            void MapView::update(EventInterfacePtr e) {

            }

/** Renders the map onto the window
 **
 **/
void MapView::render(sf::RenderWindow *window) {	
	if (view_state == 0) {
		window->clear(sf::Color::White);
		window->draw(Configuration::getLoadingSprite());
		window->display();
	}
	else if (view_state == 1) {
		window->draw(background);
		window->draw(title_sprite);
		for (int i = 0; i < num_levels; i++) {
			window->draw(sprites[i]);
			if (i > 1) {
				window->draw(flowers_text[i]);
			}		
		}
		std::shared_ptr<ActorComponent>     ac;
		std::shared_ptr<CollectorComponent>   cc;
		ac = LevelView::player->components[CollectorComponent::id];
		cc = std::dynamic_pointer_cast<CollectorComponent>(ac);

		cc->render(window, false);
	}
}
