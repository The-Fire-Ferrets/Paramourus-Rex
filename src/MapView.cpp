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
//Level tlevel_idxextures
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
int MapView::flowers[size];
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
int MapView::min_flowers[size];
int MapView::max_flowers[size];
//COmmentary 
std::map<int, sf::Text> MapView::commentary;
std::map<DisplayContactPair, sf::Vector2f> MapView::commentary_positions;
std::map<DisplayContactPair, std::vector<std::string>> MapView::commentary_strings;
std::map<DisplayContactPair, int> MapView::commentary_occurance;
std::map<DisplayContactPair, int> MapView::commentary_actions;
std::map<int, sf::Clock> MapView::commentary_timer;
sf::Vector2f MapView::commentary_pos;
int MapView::commentary_idx;
int MapView::level_idx;
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
	commentary_positions.clear();
	commentary_strings.clear();
	commentary_actions.clear();
	commentary_occurance.clear();
	commentary_timer.clear();
	commentary.clear();
	commentary_idx = 0;
	level_idx = -1;
	num_levels = -1;
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
	else if (!strcmp(attr.name(), "CommentaryX")) {
            commentary_pos.x = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "MapView::CreateMap: Error reading attribute for " << attr.name() << std::endl;
                }
        }
	else if (!strcmp(attr.name(), "CommentaryY")) {
            commentary_pos.y = std::strtol(attr.value(), &temp, 10);
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

	commentary_timer.insert(std::pair<int, sf::Clock>(-1, sf::Clock()));
	commentary.insert(std::pair<int, sf::Text>(-1, sf::Text("", font, 5)));
    //Iterates over XML to get components to add
    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling()) {
	if (!strcmp(tool.name(), "Commentary")) {		
			for (pugi::xml_node tool1 = tool.first_child(); tool1; tool1 = tool1.next_sibling()) {
				ActorId display;
				ActorId actor_id;
				ActorId contact;
				int actions_val;
				int occurance;
				for (pugi::xml_attribute attr = tool1.first_attribute(); attr; attr = attr.next_attribute()) {
					if (!strcmp(attr.name(), "Actor")) {
						actor_id = attr.value();
					}
					else if (!strcmp(attr.name(), "Display")) {
						display = attr.value();
					}
					else if (!strcmp(attr.name(), "Contact")) {
						contact = attr.value();
					}
					else if (!strcmp(attr.name(), "Action")) {
						actions_val = (std::strtol(attr.value(), &temp, 10));
						if (*temp != '\0') {
							std::cout << "MapView::Create: Error reading attribute for " << attr.name() << std::endl;
						}
					}
					else if (!strcmp(attr.name(), "Occurance")) {
						occurance = (std::strtol(attr.value(), &temp, 10));
						if (*temp != '\0') {
							std::cout << "MapView::Create: Error reading attribute for " << attr.name() << std::endl;
						}
					}
				}
				commentary_strings.insert(std::pair<DisplayContactPair, std::vector<std::string>>(DisplayContactPair(display, ContactPair(actor_id, contact)), std::vector<std::string>()));
				commentary_actions.insert(std::pair<DisplayContactPair, int>(DisplayContactPair(display, ContactPair(actor_id, contact)), actions_val));	
				commentary_occurance.insert(std::pair<DisplayContactPair, int>(DisplayContactPair(display, ContactPair(actor_id, contact)), occurance));				
				for (pugi::xml_node tool2 = tool1.first_child(); tool2; tool2 = tool2.next_sibling()) {
					for (pugi::xml_attribute attr = tool2.first_attribute(); attr; attr = attr.next_attribute()) {
						commentary_strings[DisplayContactPair(display, ContactPair(actor_id, contact))].push_back(fitStringToCommentaryBox(attr.value()));
					}
				}
			}
		}
	else {
		levels[++num_levels] = tool.name();	
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
		            std::cout << "MapView::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
		        }
		    }
		    else if (!strcmp(attr.name(),"Height")) {
		        sizes[num_levels].y = std::strtol(attr.value(), &temp, 10);
		        if (*temp != '\0') {
		            std::cout << "MapView::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
		        }
		    }
		    else if (!strcmp(attr.name(),"Height")) {
		        sizes[num_levels].y = std::strtol(attr.value(), &temp, 10);
		        if (*temp != '\0') {
		            std::cout << "MapView::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
		        }
		    }
		    else if (!strcmp(attr.name(),"Height")) {
		        sizes[num_levels].y = std::strtol(attr.value(), &temp, 10);
		        if (*temp != '\0') {
		            std::cout << "MapView::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
		        }
		    }
			else if (!strcmp(attr.name(),"MinFlowers")) {
		        min_flowers[num_levels] = std::strtol(attr.value(), &temp, 10);
		        if (*temp != '\0') {
		            std::cout << "MapView::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
		        }
		    }
		else if (!strcmp(attr.name(),"MaxFlowers")) {
		        max_flowers[num_levels] = std::strtol(attr.value(), &temp, 10);
		        if (*temp != '\0') {
		            std::cout << "MapView::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
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

    }

    for (int i = 0; i < num_levels; i++) {
        sprites[i] = sf::Sprite(textures[i], sf::IntRect(0, 0, textures[i].getSize().x, textures[i].getSize().y));
        sprites[i].setScale(sizes[i].x/(textures[i].getSize()).x, sizes[i].y/(textures[i].getSize()).y);
	sprites[i].setPosition(positions[i] + sf::Vector2f(-45, -20));
    }


}

/** Checks to see if level was clicked on and switches to it
 **
 **/
void MapView::update(sf::RenderWindow *window, int* state, float time) {
	EventManagerInterface::setViewDelegate(delegate);
	EventManagerInterface::setCurrentActorList(NULL);
    if (reset) {
	int last_state = view_state;
	if (commentary_idx < commentary_strings[DisplayContactPair("Homer", ContactPair("", ""))].size()) {
		commentary[-1] = sf::Text((commentary_strings[DisplayContactPair("Homer", ContactPair("", ""))])[commentary_idx], font, 15);
		commentary[-1].setPosition(commentary_pos);
		commentary_timer[-1].restart();
		if (commentary_idx == commentary_strings[DisplayContactPair("Homer", ContactPair("", ""))].size()) {
			level_idx = 0;
		}
	}
	view_state = 0;
	render(window);
	resetPopulationValues();
	view_state = last_state;
    }	

    if (LevelView::player == NULL) {
	int last_state = view_state;
	view_state = 0;
	render(window);
	int flowers_temp[] = {fireflowers_count[1], earthflowers_count[1], airflowers_count[1], waterflowers_count[1]};
	LevelView::Create("LevelPlayer", state, flowers_temp);
	view_state = last_state;
	LevelView::cleanUp();
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed && view_state != 0) {
        pressed = true;
        const sf::Vector2i pos = sf::Mouse::getPosition(*window);
        for (int i = 0; i < level_idx + 3; i++) {
		if ( i < num_levels) {
		    if (sprites[i].getGlobalBounds().contains(pos.x, pos.y)) {
			if (i > 1 && view_state == 1) {
				view_state = 0;
				render(window);
				int flowers[] = {fireflowers_count[i], earthflowers_count[i], airflowers_count[i], waterflowers_count[i]};
				LevelView::Create(levels[i].c_str(), state, flowers);
				LevelView::start();
				reset = true;
				view_state = 1;
				*state = 1;
			}
			else if (i <= 1) {
				int last_state = view_state;
				if (view_state == 2)
					CraftView::view_state = 2;
				view_state = 0;
				render(window);
				reset = true;
				view_state = last_state;
				*state = 3;
			}
		    }
			else if (title_sprite.getGlobalBounds().contains(pos.x, pos.y)) {
			*state = 5;
			MapView::level_idx = -1;
			MapView::commentary_idx = 0;
			LevelView::player->reset();
		    }
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
	for (int i = 0; i < num_levels; i++) {
		if (i > 1) {
			if (min_flowers[i] == max_flowers[i])
				flowers[i] = max_flowers[i];
			else
				flowers[i] = rand() % (max_flowers[i] - min_flowers[i]) + min_flowers[i];
			fireflowers_count[i] = (int)(flowers[i] * fireflowers[i]);
			earthflowers_count[i] = (int)(flowers[i] * earthflowers[i]);
			waterflowers_count[i] = (int)(flowers[i] * waterflowers[i]);
			airflowers_count[i] = (int)(flowers[i] * airflowers[i]);
			flowers_string[i] = "F:" + std::to_string(fireflowers_count[i]) + " E:" + std::to_string(earthflowers_count[i]) + " A:" + std::to_string(airflowers_count[i]) + " W:" + std::to_string(waterflowers_count[i]);
			flowers_text[i] = sf::Text(flowers_string[i], font);
			flowers_text[i].setCharacterSize(15);
			flowers_text[i].setStyle(sf::Text::Bold);
			flowers_text[i].setColor(sf::Color::Black);
			flowers_text[i].setPosition(positions[i] + sf::Vector2f(30.0/2 - flowers_text[i].getGlobalBounds().width/2, -1.5*flowers_text[i].getGlobalBounds().height));
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
	else if (view_state != 0) {
		window->draw(background);
		window->draw(title_sprite);
		for (int i = 0; i < level_idx + 3; i++) {
			if ( i < num_levels) {
				if (i > 1 && view_state == 1) {
					window->draw(flowers_text[i]);
				}
				else if (i <= 1)
					window->draw(sprites[i]);
			}		
		}
		std::shared_ptr<ActorComponent>     ac;
		std::shared_ptr<CollectorComponent>   cc;
		ac = LevelView::player->components[CollectorComponent::id];
		cc = std::dynamic_pointer_cast<CollectorComponent>(ac);

		cc->render(window, false);

		if ((commentary_idx < commentary_strings[DisplayContactPair("Homer", ContactPair("", ""))].size() && commentary_timer[-1].getElapsedTime().asSeconds() < 30))
			window->draw(commentary[-1]);
	}
}

//Helper function to print hints correctly
std::string MapView::fitStringToCommentaryBox(std::string str) {
	// get dialogue box bound
	int width = 100;
	int height = 300;
	int beginX = 0;
	int beginY = 0;
	//commentary_positions.push_back(sf::Vector2f(beginX, beginY));
	int endX = beginX+width;
	int max_width = endX-beginX;

	int endY = beginY+height;
	int max_height = (endY-beginY);

	// text object used to see how close each word puts us to the bounds
	sf::Text temp;
	temp.setFont(font);
	temp.setCharacterSize(commentary.begin()->second.getCharacterSize());

	// current string and width
	std::vector<std::string> boxes;
	std::string fitted_string = "";
	float current_width = 0.f;
	float word_width = 0.f, word_height = 0.f;
	// split the dialogue into words;
	std::vector<std::string> words = split(str, ' ');

	// for each word...
	for (std::string word : words) {
		// get the bounding box
		temp.setString(word + " ");
		word_width = temp.findCharacterPos(temp.getString().getSize()).x;

		// general word height (changes, hence the max)
		sf::FloatRect bounds = temp.getGlobalBounds();
		int line_spacing = font.getLineSpacing(temp.getCharacterSize());
		word_height = std::max(bounds.height-bounds.top+line_spacing, word_height);

		// the height of the full string so far
		temp.setString(fitted_string);
		float full_height = temp.getGlobalBounds().height - temp.getGlobalBounds().top;

		// will it go past the horizontal bound?
		if (current_width + word_width > max_width) {
			// will it go past the vertical bound?
			if (max_height - full_height < word_height) {
				boxes.push_back(fitted_string);
				fitted_string = word + " ";
				current_width = word_width;
			}
			else {
				fitted_string += "\n" + word + " ";
				current_width = word_width;
			}
		}
		else {
			// just add to string
			fitted_string += word + " ";
			current_width += word_width;
		}
	}
	boxes.push_back(fitted_string);
	// done
	return boxes.front();
}
