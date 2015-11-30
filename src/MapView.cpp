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
//View state 0: Loading, 1: Normal, 4: Quit Game confirmation
int MapView::view_state = 1;
//Back button to title screen
sf::Sprite MapView::title_sprite;
sf::Texture MapView::title_texture;
sf::Vector2f MapView::title_size;
int MapView::min_flowers[size];
int MapView::max_flowers[size];
//COmmentary 
std::map<int, sf::Text> MapView::commentary;
std::map<DisplayContactPair, std::vector<sf::Vector2f>> MapView::commentary_positions;
std::map<DisplayContactPair, std::vector<std::vector<std::string>>> MapView::commentary_strings;
std::map<DisplayContactPair, std::vector<int>> MapView::commentary_actions;
std::map<DisplayContactPair , std::vector<int>> MapView::commentary_occurance;
std::map<DisplayContactPair, std::vector<std::vector<int>>> MapView::commentary_sizes;
int MapView::commentary_type_idx;
std::map<int, sf::Clock> MapView::commentary_timer;
sf::Vector2f MapView::commentary_pos;
int MapView::commentary_idx;
int MapView::commentary_size;
sf::Texture MapView::commentary_prompt_texture;
sf::Sprite MapView::commentary_prompt;
bool MapView::display_commentary;
bool MapView::first_level_entered;
int MapView::level_idx;
// quit game confirmation text
sf::Text MapView::back_continue;
sf::Text MapView::back_cancel;
sf::Text MapView::back_message;
sf::Text MapView::back_question;
std::string MapView::back_continue_text;
std::string MapView::back_cancel_text;
std::string MapView::back_message_text;
std::string MapView::back_question_text;

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
	commentary_sizes.clear();
	commentary_actions.clear();
	commentary_occurance.clear();
	commentary_timer.clear();
	commentary.clear();
	commentary_idx = 0;
	commentary_pos = sf::Vector2f(-1000, -1000);
	display_commentary = false;
	first_level_entered = false;
	level_idx = -1;
	pressed = true;
	reset = true;
	num_levels = 0;
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
	else if (!strcmp(attr.name(), "Prompt")) {
            commentary_prompt_texture.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
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

	commentary_prompt = sf::Sprite(commentary_prompt_texture, sf::IntRect(0, 0, commentary_prompt_texture.getSize().x, commentary_prompt_texture.getSize().y));
        commentary_prompt.setPosition(sf::Vector2f(75,75));

	commentary_timer.insert(std::pair<int, sf::Clock>(-1, sf::Clock()));
	commentary.insert(std::pair<int, sf::Text>(-1, sf::Text("", font)));
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
				if (commentary_strings.find(DisplayContactPair(display, ContactPair(actor_id, contact))) == commentary_strings.end()) {
					commentary_strings.insert(std::pair<DisplayContactPair, std::vector<std::vector<std::string>>>(DisplayContactPair(display, ContactPair(actor_id, contact)), std::vector<std::vector<std::string>>()));
					commentary_sizes.insert(std::pair<DisplayContactPair, std::vector<std::vector<int>>>(DisplayContactPair(display, ContactPair(actor_id, contact)), std::vector<std::vector<int>>()));
				}
				commentary_strings[DisplayContactPair(display, ContactPair(actor_id, contact))].push_back(std::vector<std::string>());
				commentary_sizes[DisplayContactPair(display, ContactPair(actor_id, contact))].push_back(std::vector<int>());
				if (commentary_actions.find(DisplayContactPair(display, ContactPair(actor_id, contact))) == commentary_actions.end()) {
					commentary_actions.insert(std::pair<DisplayContactPair, std::vector<int>>(DisplayContactPair(display, ContactPair(actor_id, contact)), std::vector<int>()));
				}
				commentary_actions[DisplayContactPair(display, ContactPair(actor_id, contact))].push_back(actions_val);	

				if (commentary_occurance.find(DisplayContactPair(display, ContactPair(actor_id, contact))) == commentary_occurance.end()) {
				commentary_occurance.insert(std::pair<DisplayContactPair, std::vector<int>>(DisplayContactPair(display, ContactPair(actor_id, contact)), std::vector<int>()));			
				}
				commentary_occurance[DisplayContactPair(display, ContactPair(actor_id, contact))].push_back(occurance);		
				for (pugi::xml_node tool2 = tool1.first_child(); tool2; tool2 = tool2.next_sibling()) {
					for (pugi::xml_attribute attr = tool2.first_attribute(); attr; attr = attr.next_attribute()) {
						if (display == "Homer") {
							commentary_strings[DisplayContactPair(display, ContactPair(actor_id, contact))].back().push_back(fitStringToCommentaryBox(attr.value()));
						}
						else {
							commentary_strings[DisplayContactPair(display, ContactPair(actor_id, contact))].back().push_back(fitStringToCommentaryBox(attr.value(), 5, sf::Vector2f(Configuration::getGameViewWidth()/2, Configuration::getGameViewHeight()/2), false));
						}
						commentary_sizes[DisplayContactPair(display, ContactPair(actor_id, contact))].back().push_back(commentary_size);	
					}
				}
			}
		}
	else {
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
		num_levels++;
	}

    }

    for (int i = 0; i < num_levels; i++) {
        sprites[i] = sf::Sprite(textures[i], sf::IntRect(0, 0, textures[i].getSize().x, textures[i].getSize().y));
        sprites[i].setScale(sizes[i].x/(textures[i].getSize()).x, sizes[i].y/(textures[i].getSize()).y);
	sprites[i].setPosition(positions[i] + sf::Vector2f(-45, -20));
    }

	back_continue_text = "Yes, Cancel";
	back_cancel_text = "No, Continue";
	back_question_text = "Are you sure?";
	back_message_text = "Leaving now will erase all progress.";

	back_continue = sf::Text(back_continue_text, font, 30);
	back_cancel = sf::Text(back_cancel_text, font, 30);
	back_question = sf::Text(back_question_text, font, 40);
	back_message = sf::Text(back_message_text, font, 20);

	back_message.setColor(sf::Color::Black);
	back_question.setColor(sf::Color::Black);
	back_continue.setColor(sf::Color::Black);
	back_cancel.setColor(sf::Color::Black);	

	back_question.setPosition(Configuration::getWindowWidth()/2 - back_question.getGlobalBounds().width/2, Configuration::getWindowHeight()/4);
	back_message.setPosition(Configuration::getWindowWidth()/2 - back_message.getGlobalBounds().width/2, Configuration::getWindowHeight()/4 + back_question.getGlobalBounds().height + 10);
	back_continue.setPosition(Configuration::getWindowWidth()/4 - back_continue.getGlobalBounds().width/2, Configuration::getWindowHeight()/2 + 20);
	back_cancel.setPosition(3*Configuration::getWindowWidth()/4  - back_cancel.getGlobalBounds().width/2, Configuration::getWindowHeight()/2 + 20);
}

/** Checks to see if level was clicked on and switches to it
 **
 **/
void MapView::update(sf::RenderWindow *window, int* state, float time) {
    EventManagerInterface::setViewDelegate(delegate);
    EventManagerInterface::setCurrentActorList(NULL);
    if (reset) {
        int last_state = view_state;
	if (view_state == 1)
		commentary_type_idx = 1;
	else if (view_state == 2)
		commentary_type_idx = 0;

        if (commentary_idx < commentary_strings[DisplayContactPair("Homer", ContactPair("", ""))].size() && !first_level_entered) {
            commentary[-1] = sf::Text((commentary_strings[DisplayContactPair("Homer", ContactPair("", ""))][commentary_idx])[commentary_type_idx], font);
		commentary[-1].setCharacterSize((commentary_sizes[DisplayContactPair("Homer", ContactPair("", ""))][commentary_idx])[commentary_type_idx]);
            commentary[-1].setPosition(commentary_prompt.getPosition().x + commentary_prompt.getGlobalBounds().width/2 - commentary[-1].getGlobalBounds().width/2, commentary_prompt.getPosition().y + commentary_prompt.getGlobalBounds().height/2 - commentary[-1].getGlobalBounds().height/2  );
		commentary[-1].setColor(sf::Color::Black);
            commentary_timer[-1].restart();
		display_commentary = true;
            if (commentary_idx == commentary_strings[DisplayContactPair("Homer", ContactPair("", ""))].size()) {
                level_idx = 0;
            }
        }
	else {
		display_commentary = false;
	}

	if ( commentary_timer[-1].getElapsedTime().asSeconds() > 30 )
		display_commentary = false;
        view_state = 0;
        render(window);
        resetPopulationValues();
        view_state = last_state;
    }	


    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed && (view_state == 1 || view_state == 2 || view_state == 4)) {
        pressed = true;
        const sf::Vector2i pos = sf::Mouse::getPosition(*window);
	if (!display_commentary) {
		for (int i = 0; i < level_idx + 3; i++) {
		    if ( i < num_levels  && view_state != 4) {
		        if (sprites[i].getGlobalBounds().contains(pos.x, pos.y)) {
		            if (i > 1 && view_state == 1) {
		                view_state = 0;
		                render(window);
		                int flowers[] = {fireflowers_count[i], earthflowers_count[i], airflowers_count[i], waterflowers_count[i]};
		                LevelView::Create(levels[i].c_str(), state, flowers);
		                LevelView::start();
				first_level_entered = true;
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
			}
			}
		        if (title_sprite.getGlobalBounds().contains(pos.x, pos.y) && (view_state == 1 || view_state == 2)) {
		            view_state = 4;
		        }
		        else if (view_state == 4 && back_cancel.getGlobalBounds().contains(pos.x, pos.y)) {
		            view_state = 1;
		        }
		        else if (view_state == 4 && back_continue.getGlobalBounds().contains(pos.x, pos.y)) {
		            // reset ongoing game state
		            CraftView::total_craft_visits = 1;
		            CraftView::clearInventory();
		            DialogueView::num_times_impressed = 0;
		            MapView::level_idx = -1;
		            MapView::commentary_idx = 0;
		            MapView::resetPopulationValues();
				//std::cout << "HERE" << std::endl;
		            LevelView::player->reset();
		            // rest the view
		            view_state = 0;
		            *state = 5;
		        }
	}
	display_commentary = false;
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
		if (((i > 1 && level_idx + 2 >= num_levels) || i == level_idx + 2) || level_idx == -1) {
			if (min_flowers[i] == max_flowers[i])
				flowers[i] = max_flowers[i];
			else
				flowers[i] = rand() % (max_flowers[i] - min_flowers[i]) + min_flowers[i] + 1;
			fireflowers_count[i] = round(1.0 * flowers[i] * fireflowers[i]);
			earthflowers_count[i] = round(1.0 * flowers[i] * earthflowers[i]);
			waterflowers_count[i] = round(1.0 * flowers[i] * waterflowers[i]);
			airflowers_count[i] = round(1.0 * flowers[i] * airflowers[i]);
			flowers_string[i] = "F:" + std::to_string(fireflowers_count[i]) + " E:" + std::to_string(earthflowers_count[i]) + " A:" + std::to_string(airflowers_count[i]) + " W:" + std::to_string(waterflowers_count[i]);
			flowers_text[i] = sf::Text(flowers_string[i], font);
			flowers_text[i].setCharacterSize(15);
			flowers_text[i].setStyle(sf::Text::Bold);
			flowers_text[i].setColor(sf::Color::Black);
			flowers_text[i].setPosition(positions[i] + sf::Vector2f(30.0/2 - flowers_text[i].getGlobalBounds().width/2, -1.5*flowers_text[i].getGlobalBounds().height));
		}
		else {
			flowers[i] = 0;
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
		if (display_commentary) {
			window->draw(commentary_prompt);
			if ((commentary_idx < commentary_strings[DisplayContactPair("Homer", ContactPair("", ""))].size()))
				window->draw(commentary[-1]);
		}

		if (view_state == 4) {
			window->draw(commentary_prompt);
			window->draw(back_message);
			window->draw(back_continue);
			window->draw(back_cancel);
			window->draw(back_question);
		}
	}
}

//Helper function to print hints correctly
std::string MapView::fitStringToCommentaryBox(std::string str, int character_size, sf::Vector2f box_size, bool center) {
	// get dialogue box bound
	int width;
	int height;
	if (box_size.x == 0  || box_size.y == 0) {
		width = 600;
		height = 400;
	}
	else {
		width = box_size.x;
		height = box_size.y;
	}
	int beginX = 0;
	int beginY = 0;
	//commentary_positions.push_back(sf::Vector2f(beginX, beginY));
	int endX = beginX+width;
	int max_width = endX-beginX;

	int endY = beginY+height;
	int max_height = (endY-beginY);

	//To figure out correct size
	bool size_found = false;
	std::vector<std::string> boxes;
	std::string fitted_string;
	int curr_size;
	if (character_size <= 0) {
		curr_size = 50;
		character_size = 50;
	}
	else {
		curr_size = character_size;
		character_size = 1;
	}

	while (!size_found && curr_size > 0 && character_size-- > 0) {
		// text object used to see how close each word puts us to the bounds
		sf::Text temp;
		temp.setFont(font);
		temp.setCharacterSize(curr_size);
		// current string and width
		fitted_string = "";
		std::string next_line = "";
		float current_width = 0.f;
		float space_width = 0.f, word_width = 0.f, word_height = 0.f;

		//gET WIDTH OF SPACE CHARACTER
		temp.setString(" ");
		space_width = temp.findCharacterPos(temp.getString().getSize()).x;
		// split the dialogue into words;
		std::vector<std::string> words = split(str, ' ');

		// for each word...
		for (std::string word : words) {
			// get the bounding box
			temp.setString(word + " ");
			word_width = temp.findCharacterPos(temp.getString().getSize()).x;
			word_height = temp.findCharacterPos(temp.getString().getSize()).y;

			// will it go past the horizontal bound?
			if (current_width + word_width > max_width) {
				if (center) {
					int num_spaces = (int) ((max_width - current_width)/2)/space_width;
					for (int s_num = 0; s_num < num_spaces; s_num++)
						next_line = " " + next_line + " ";
				}
		
				fitted_string += next_line + "\n";
				next_line = word + " ";
				current_width = word_width;
			}
			else {
				// just add to string
				next_line += word + " ";
				current_width += word_width;
				size_found = true;
			}

			// general word height (changes, hence the max)
			sf::FloatRect bounds = temp.getGlobalBounds();
			int line_spacing = font.getLineSpacing(temp.getCharacterSize());
			word_height = std::max(bounds.height-bounds.top+line_spacing, word_height);

			// the height of the full string so far
			temp.setString(fitted_string);
			float full_height = temp.getGlobalBounds().height - temp.getGlobalBounds().top;

			// will it go past the vertical bound?
			if (max_height - full_height < word_height) {
				curr_size--;
				size_found = false;
				break;
			}
		}
		if (center) {
			int num_spaces = (int) ((max_width - current_width)/2)/space_width;
			for (int s_num = 0; s_num < num_spaces; s_num++)
				next_line = " " + next_line + " ";
		}
		fitted_string += next_line + "\n";
	}
	boxes.push_back(fitted_string);
	// done
	if (curr_size <= 0) {
		commentary_size = 0;
		return "";
	}
	commentary_size = curr_size;
	return boxes.front();
}
