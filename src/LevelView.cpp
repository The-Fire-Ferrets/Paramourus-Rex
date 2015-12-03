#include "LevelView.h"
#include "CollectableComponent.h"

EventDelegate LevelView::delegate = NULL;
//Total size of pointer arrays
const int LevelView::size = 20;
//Number of actors populating the level
int LevelView::num_actors = 0;
//Array holding pointers of actors populating the level
std::vector<StrongActorPtr> LevelView::actorList;
//Holds background texture
sf::Texture LevelView::background_texture;
sf::Texture LevelView::minimap_background_texture;
//Edge texture
sf::Texture LevelView::edge_texture;
//Holds background
sf::Sprite LevelView::background;
sf::Sprite LevelView::minimap_background;
//Holds edge
sf::Sprite LevelView::edge;
//Holds level name
std::string LevelView::name;
//Level clock
sf::Clock LevelView::level_clock;
//Holds Level time
sf::Text LevelView::timer;
//Holds level timer text
std::string LevelView::timer_string = "10:00";
//Hold text font
sf::Font LevelView::font;
//Timer position
sf::Vector2f LevelView::timer_position;
//game view
sf::View LevelView::gameView;
//minimap view
sf::View LevelView::minimapView;
//pause map view
sf::View LevelView::pauseView;
//reference to player
StrongActorPtr LevelView::player = NULL;
//Level duration in ms
int LevelView::duration;
//Minimap border
sf::Sprite LevelView::minimap_border;
//State
int LevelView::view_state = 1;
// level music
sf::SoundBuffer LevelView::buffer;
sf::Sound LevelView::sound;
// Level text
std::map<int, sf::Text> LevelView::commentary;
std::map<DisplayContactPair, std::vector<sf::Vector2f>> LevelView::commentary_positions;
std::map<DisplayContactPair, std::vector<std::vector<std::string>>> LevelView::commentary_strings;
std::map<DisplayContactPair, std::vector<int>> LevelView::commentary_actions;
std::map<DisplayContactPair , std::vector<int>> LevelView::commentary_occurance;
std::map<DisplayContactPair, std::vector<std::vector<int>>> LevelView::commentary_sizes;
std::map<int, sf::Clock> LevelView::commentary_timer;
bool LevelView::commentary_change = true;
std::vector<std::vector<pugi::xml_node>> LevelView::actions;
pugi::xml_document LevelView::doc;
sf::Texture LevelView::commentary_prompt_texture;
sf::Sprite LevelView::commentary_prompt;
sf::Vector2f LevelView::commentary_pos;
int LevelView::commentary_size;
bool LevelView::display_commentary;
//Back Button
sf::Sprite LevelView::back_button;
sf::Texture LevelView::title_texture;
sf::Vector2f LevelView::title_size;	
bool LevelView::pressed = false;
bool LevelView::reveal_homer;
//Update targers
int LevelView::flowers_left;
//Checks to see if player is being chased
int LevelView::inVision;
//FLashing when timer is nearing end
int LevelView::flashing;
//Keep track of level timer
float LevelView::timer_time;
//Level timeout graphics
sf::Texture LevelView::timeout_texture;
sf::Sprite LevelView::timeout_sprite;
int LevelView::last_action;
int* LevelView::game_state;
// pause screen medium map
bool LevelView::pause_key_pressed = false;
bool LevelView::back_key_pressed = false;
bool LevelView::call_key_pressed = false;
bool LevelView::vases_full = false;
sf::Text LevelView::title;
std::string LevelView::title_text;
sf::Text LevelView::load_state;
std::string LevelView::load_text;
//Back button text
sf::Text LevelView::back_continue;
sf::Text LevelView::back_cancel;
sf::Text LevelView::back_message;
sf::Text LevelView::back_question;
std::string LevelView::back_continue_text;
std::string LevelView::back_cancel_text;
std::string LevelView::back_message_text;
std::string LevelView::back_question_text;
int LevelView::last_state;
StrongActorPtr LevelView::homer;
bool LevelView::space_pressed;
bool LevelView::received_new_commentary;
bool LevelView::testing_view;
bool LevelView::testing_key_pressed = false;
/** Creates and populates a level and all its components based on XML configuration
 ** resource: filename for xml
 ** state: current game state
 **/
void LevelView::Create(const char* resource, int* state, int flowers[]) {
	//Reference to current location in Actor population array
	//Holds referenced to loaded XML file
	title_text = "";
	homer = NULL;
	load_text = "Loading...";
	title_text += (std::string) resource + ": ";
	delegate.bind(&LevelView::update);
	Pathfinder::Create(Configuration::getWindowWidth(), Configuration::getWindowHeight(), 10);
	//Reset values
	num_actors = 0;
	flowers_left = 0;
	last_action = 0;
	reveal_homer = false;
	pause_key_pressed = false;
	back_key_pressed = false;
	call_key_pressed = false;
	testing_key_pressed = false;
	vases_full = false;
	testing_view = false;
	inVision = 0;
	flashing = 0;
	actorList.clear();
	actions.clear();
	commentary_positions.clear();
	commentary_strings.clear();
	commentary_actions.clear();
	commentary_occurance.clear();
	commentary_timer.clear();
	commentary.clear();
	commentary_pos = sf::Vector2f(-1000, -1000);
	display_commentary = false;
	received_new_commentary = false;
	game_state = state;
	view_state = 0;
	//Error check to see if file was loaded correctly
	pugi::xml_parse_result result;
	std::string resource_str(resource);
	if (!(result = doc.load_file(("./assets/levels/" + resource_str + ".xml").c_str()))) {
		std::cout << "LevelView::Create(...): Failed to load" << std::endl;
		std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
	}

	//Used to iterate over XML file to get attributes
	pugi::xml_node tools = doc.child(resource);
	char* temp;
	for (pugi::xml_attribute attr = tools.first_attribute(); attr; attr = attr.next_attribute()) {
		if (!strcmp(attr.name(), "Name")) {
			name = attr.value();
			title_text += name;
			if (!strcmp(attr.value(), "Tutorial") && *state == 5) {
				view_state = 2;
			}
		}
		else if (!strcmp(attr.name(), "Background")) {
			background_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
			background = sf::Sprite(background_texture, sf::IntRect(0, 0, background_texture.getSize().x, background_texture.getSize().y));
			background.scale((1.0*Configuration::getWindowWidth())/(background_texture.getSize().x), (1.0*Configuration::getWindowHeight())/(background_texture.getSize().y));
			background.setPosition(sf::Vector2f(0,0));
		}
		else if (!strcmp(attr.name(), "MinimapBackground")) {
			minimap_background_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
			minimap_background = sf::Sprite(minimap_background_texture);
			minimap_background.setScale(Configuration::getWindowWidth()/10.f, Configuration::getWindowHeight()/10.f);
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
		else if (!strcmp(attr.name(), "Edge")) {
			if (strcmp(attr.value(), "")) {
				edge_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
				edge = sf::Sprite(edge_texture, sf::IntRect(0, 0, edge_texture.getSize().x, edge_texture.getSize().y));
				edge.scale((1.5*Configuration::getWindowWidth())/(edge_texture.getSize().x), (1.5*Configuration::getWindowHeight())/(edge_texture.getSize().y));
				edge.setPosition(sf::Vector2f(-100,-100));
			}
		}
		else if (!strcmp(attr.name(), "Font")) {
			font.loadFromFile(("./assets/" + (std::string)attr.value()).c_str());
			timer = sf::Text(timer_string, font);
		}
		else if (!strcmp(attr.name(), "Text_Size")) {
			timer.setCharacterSize(std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Text_X")) {
			timer_position.x = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Text_Y")) {
			timer_position.y = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Duration")) {
			duration = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
	}
	//Setup back button
	back_button = sf::Sprite(title_texture, sf::IntRect(0, 0, title_texture.getSize().x, title_texture.getSize().y));
	back_button.setScale(title_size.x/(title_texture.getSize()).x, title_size.y/(title_texture.getSize()).y);
	back_button.setPosition(sf::Vector2f(-1000,-1000));

	title = sf::Text(title_text, font, 25);
	title.setColor(sf::Color::Black);
	load_state = sf::Text(load_text, font, 25);
	load_state.setColor(sf::Color::Black);

	commentary_prompt = sf::Sprite(commentary_prompt_texture, sf::IntRect(0, 0, commentary_prompt_texture.getSize().x, commentary_prompt_texture.getSize().y));
	//commentary_prompt.setScale(1.0 * Configuration::getGameViewWidth() / Configuration::getWindowWidth(), 1.0 * Configuration::getGameViewHeight() / Configuration::getWindowHeight());

	title.setPosition(Configuration::getWindowWidth()/2 - title.getGlobalBounds().width/2, Configuration::getWindowHeight()/2 - title.getGlobalBounds().height - 10);

	load_state.setPosition(Configuration::getWindowWidth()/2 - load_state.getGlobalBounds().width/2, Configuration::getWindowHeight()/2 + 10);
	
	//Setup timeout graphics
	const char* timeout_file = {"./assets/backgrounds/TimeOut.png"};
	timeout_texture.loadFromFile(timeout_file);
	timeout_sprite = sf::Sprite(timeout_texture, sf::IntRect(0, 0, timeout_texture.getSize().x, timeout_texture.getSize().y));
	timeout_sprite.setScale(1.0 * Configuration::getGameViewWidth()/(timeout_texture.getSize()).x, 1.0 * Configuration::getGameViewHeight()/(timeout_texture.getSize()).y);
	timer.setPosition(timer_position);

	back_continue_text = "Yes, Cancel";
	back_cancel_text = "No, Continue";
	back_question_text = "Are you sure?";
	back_message_text = "Leaving without Homer's help will result in losing all your flowers!";

	back_continue = sf::Text(back_continue_text, font, 20);
	back_cancel = sf::Text(back_cancel_text, font, 20);
	back_question = sf::Text(back_question_text, font, 30);
	back_message = sf::Text(back_message_text, font, 10);

	back_message.setColor(sf::Color::Black);
	back_question.setColor(sf::Color::Black);
	back_continue.setColor(sf::Color::Black);
	back_cancel.setColor(sf::Color::Black);	

	back_question.setPosition(Configuration::getWindowWidth()/2 - back_question.getGlobalBounds().width/2, Configuration::getWindowHeight()/4);
	back_message.setPosition(Configuration::getWindowWidth()/2 - back_message.getGlobalBounds().width/2, Configuration::getWindowHeight()/4 + back_question.getGlobalBounds().height + 10);
	back_continue.setPosition(Configuration::getWindowWidth()/4 - back_continue.getGlobalBounds().width/2, Configuration::getWindowHeight()/2 + 20);
	back_cancel.setPosition(3*Configuration::getWindowWidth()/4  - back_cancel.getGlobalBounds().width/2, Configuration::getWindowHeight()/2 + 20);

	commentary_prompt.setPosition(sf::Vector2f(75, 75));

	commentary_timer.insert(std::pair<int, sf::Clock>(-1, sf::Clock()));
	commentary.insert(std::pair<int, sf::Text>(-1, sf::Text("", font, 5)));
	commentary[-1].setPosition(sf::Vector2f(commentary_prompt.getPosition().x + 35, commentary_prompt.getPosition().y + 35));
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
							std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
						}
					}
					else if (!strcmp(attr.name(), "Occurance")) {
						occurance = (std::strtol(attr.value(), &temp, 10));
						if (*temp != '\0') {
							std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
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
		else if (!strcmp(tool.name(), "Action")) {		
			for (pugi::xml_node tool1 = tool.first_child(); tool1; tool1 = tool1.next_sibling()) {
				//std::cout << tool1.name() << std::endl;
				actions.push_back(std::vector<pugi::xml_node>());
				for (pugi::xml_node tool2 = tool1.first_child(); tool2; tool2 = tool2.next_sibling()) {
					actions.back().push_back(tool2);
				}
			}
		}
		else if (!strcmp(tool.name(), "Homer")) {		
			generateActor(&tool, state);
			homer = (actorList.back());
		}
		else if (!strcmp(tool.name(), "Player") && player == NULL) {		
			generateActor(&tool, state);
			player = (actorList.back());
			gameView.setCenter(Configuration::getGameViewCenter());
		}
		else if (!strcmp(tool.name(), "Player") && player != NULL) {
			actorList.push_back(player);
			player->PostInit(&tool);
			gameView.setCenter(Configuration::getGameViewCenter());
			Pathfinder::addToGrid(player->getBoundary(), player->getPathType(), player->getTargetType());
			commentary_timer.insert(std::pair<int, sf::Clock>(player->getInstance(), sf::Clock()));
			commentary.insert(std::pair<int, sf::Text>(player->getInstance(), sf::Text("", font, 5)));
			num_actors++;
		}
		else {
			if (!strcmp(tool.name(), "WaterFlower")) {
				int count = flowers[3];
				flowers_left += count;
				generateActor(&tool, state, count);
			}
			else if (!strcmp(tool.name(), "FireFlower")) {
				int count = flowers[0];
				flowers_left += count;
				generateActor(&tool, state, count);
			}
			else if (!strcmp(tool.name(), "EarthFlower")) {
				int count = flowers[1];
				flowers_left += count;
				generateActor(&tool, state, count);
			}
			else if (!strcmp(tool.name(), "AirFlower")) {
				int count = flowers[2];
				flowers_left += count;
				generateActor(&tool, state, count);
			}
			else {
				generateActor(&tool, state);
				}
		}
	}
	
	//Minimap border
	minimap_border = sf::Sprite(Configuration::getMinimapBorder(), sf::IntRect(0, 0, (Configuration::getMinimapBorder().getSize()).x, (Configuration::getMinimapBorder().getSize()).y));
	minimap_border.setScale(1.0*Configuration::getWindowWidth()/(Configuration::getMinimapBorder().getSize()).x, 1.0*Configuration::getWindowHeight()/(Configuration::getMinimapBorder().getSize()).y);	
	minimap_border.setPosition(sf::Vector2f(0,0));
	//Set views so can only see a quarter of the map at once
	gameView = sf::View(sf::FloatRect(0, 0, Configuration::getGameViewWidth(), Configuration::getGameViewHeight()));
	//Set minimap to see entire map
	minimapView = sf::View(sf::FloatRect(0, 0, Configuration::getMiniMapViewWidth() + 100, Configuration::getMiniMapViewHeight() + 100));

	if (!buffer.loadFromFile("./assets/music/thunderdrum-game-loop.ogg")) {
		std::cout << "LevelView::Create: error loading music" << std::endl;
	}
	EventManagerInterface::setViewDelegate(delegate);

	//COmpletes final initializations
	std::vector<StrongActorPtr>::iterator it;
	for (it = actorList.begin(); it != actorList.end(); it++) {
		(*it) ->PostInit();
	}
	gameView.setCenter(Configuration::getGameViewCenter());
	//Sets up sound
	if (name != "LevelPlayer") {
		sound.setBuffer(buffer);
		sound.setLoop(true);
		sound.play();
	}

	//Loads the map into pathfinder
	view_state = 0;
	Pathfinder::generatingPaths = true;
	std::thread(Pathfinder::generateHCosts).detach();
}

//Helper function to generate actors
void LevelView::generateActor(pugi::xml_node* elem, int* state, int generate) {
	char* temp;
	for (pugi::xml_attribute attr = elem->first_attribute(); attr; attr = attr.next_attribute()) {
		if (!strcmp(attr.name(), "Generate")) {
			if (!strcmp(elem->name(), "WaterFlower") || !strcmp(elem->name(), "EarthFlower") || !strcmp(elem->name(), "FireFlower") || !strcmp(elem->name(), "AirFlower")) 
				flowers_left -= generate;
			generate = (std::strtol(attr.value(), &temp, 10));
			if (!strcmp(elem->name(), "WaterFlower") || !strcmp(elem->name(), "EarthFlower") || !strcmp(elem->name(), "FireFlower") || !strcmp(elem->name(), "AirFlower"))
				flowers_left += generate;
			if (*temp != '\0') {
				std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
	}
	while (generate-- > 0) {
		StrongActorPtr new_actor = ActorFactory::CreateActor(elem->name(), state);
		new_actor->PostInit(elem);
		actorList.push_back(new_actor);
		num_actors++;
		Pathfinder::addToGrid(new_actor->getBoundary(), new_actor->getPathType(), new_actor->getTargetType());
		commentary_timer.insert(std::pair<int, sf::Clock>(new_actor->getInstance(), sf::Clock()));
		commentary.insert(std::pair<int, sf::Text>(new_actor->getInstance(), sf::Text("", font, 5)));
	}
}

//Return the level name
std::string LevelView::getName(void) {
	return name;
}

//Return the number of actors
int LevelView::getNumActors(void) {
	return num_actors;
}

/** Checks to see if level was clicked on and switches to it
 **
 **/
void LevelView::update(sf::RenderWindow *window, int* state, float time) {
	//Handle near-end game flashing
	flashing = 1;
	game_state = state;
	//Gets the time left in level
	timer_time = (duration - level_clock.getElapsedTime().asMilliseconds());

	//Switch actorlist to current view
	EventManagerInterface::setCurrentActorList(&actorList);
	Configuration::setGameViewDimensions(gameView.getSize().x, gameView.getSize().y);
	bool ready_to_play = false;

	//Checks to see if done generating paths
	if (!Pathfinder::generatingPaths && view_state == 0) {
		if (name == "LevelPlayer") {
			for (auto it = actorList.begin(); it != actorList.end(); it++) {
				if (player != *it && homer != *it) {
					if (!EventManagerInterface::get()->queueEvent(new ContactEvent(0.f, player->getInstance(), (*it)->getInstance())) )
                        			std::cout << "LevelView::update: Unable to queue event" << std::endl;
					if (!EventManagerInterface::get()->queueEvent(new ContactEvent(0.f, (*it)->getInstance(), player->getInstance())) )
                        			std::cout << "LevelView::update: Unable to queue event" << std::endl;
				}
			}
			view_state = 1;
		}
		else {
			load_text = "Press the Space Bar to Start!";
			load_state.setString(load_text);
			load_state.setPosition(Configuration::getWindowWidth()/2 - load_state.getGlobalBounds().width/2, Configuration::getWindowHeight()/2 + 10);
		
			ready_to_play = true;
			//std::cout << "Pathfinder Path Generation Success!" << std::endl;
		}
	}

	// should we pause the screen?
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M) && !pause_key_pressed && (view_state == 1 || view_state == 2 || view_state == 3)) {
		pause_key_pressed = true;
		if (view_state == 3) {
			view_state = last_state;
		}
		else if (view_state != 0) {
			last_state = view_state;
			view_state = 3;
			duration = timer_time;
		}
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
		pause_key_pressed = false;
	}
	
	// should we pause the screen?
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::B) && !back_key_pressed && (view_state == 1 || view_state == 2 || view_state == 4)) {
		back_key_pressed = true;
		if (view_state == 4) {
			view_state = last_state;
		}
		else {
			last_state = view_state;
			duration = timer_time;
			view_state = 4;
		}
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
		back_key_pressed = false;
	}

	// should we pause the screen?
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && !testing_key_pressed && (view_state == 1 || view_state == 2)) {
		testing_key_pressed = true;
		testing_view = !testing_view;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
		testing_key_pressed = false;
	}

	// should we pause the screen?
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && !call_key_pressed && (view_state == 1 || (view_state == 2 && last_action < 0))) {
		call_key_pressed = true;
		EventInterfacePtr event;
		event.reset(new ContactEvent(0.f, player->getInstance(), -1));
		update(event);
	}
	

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && view_state == 0 && ready_to_play && !space_pressed) {
		if (name == "Tutorial") {
			view_state = 2;
			EventInterfacePtr event;
			event.reset(new ContactEvent(0.f, -1, -1));
			update(event);
		}
		else {
			view_state = 1;
		}
		space_pressed = true;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && display_commentary && !space_pressed) {
		space_pressed = true;
		display_commentary = false;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		space_pressed = false;
	}

	//Checks to see if back button has been selected
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed && (view_state == 1 || view_state == 2 || view_state == 4)) {
		pressed = true;
		const sf::Vector2i pos = sf::Mouse::getPosition(*window);
		if (back_button.getGlobalBounds().contains(pos.x * Configuration::getGameViewWidth() / Configuration::getWindowWidth() +  Configuration::getGameViewPosition().x, pos.y * Configuration::getGameViewHeight() / Configuration::getWindowHeight() +  Configuration::getGameViewPosition().y)) {
			last_state = view_state;
			duration = timer_time;
			view_state = 4;
		}
		else if (view_state == 4 && back_cancel.getGlobalBounds().contains(pos.x, pos.y)) {
			view_state = last_state;
		}
		else if (view_state == 4 && back_continue.getGlobalBounds().contains(pos.x, pos.y)) {
			view_state = last_state;
			if (view_state == 2) {
				view_state = 0;
				LevelView::player->reset();
				*state = 5;
				cleanUp();
				view_state = 1;
			}
			else {
				view_state = 0;
				LevelView::player->reset();
				MapView::view_state = 1;
				*state = 0;
				cleanUp();
				view_state = 1;
			}
		}
	}
	else if (!(sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
		pressed = false;
	} 

	//If still rendering paths return
	if (view_state == 0 || view_state == 4 || view_state == 3 || display_commentary) {
		level_clock.restart();
		return;
	}
	
	
	//Ends the level after timeout
	if (timer_time/1000 <= -3) {
		if (view_state == 1) {
			const char* time_out = {"TimeOut"};
			DialogueView::Create(time_out, state);
			MapView::view_state = 1;
			LevelView::player->reset();
			//std::cout << "HERE" << std::endl;
			*state = 2;
		}
		else if (view_state == 2) {
			view_state = 1;
			LevelView::player->reset();
			MapView::view_state = 2;
			MapView::commentary_idx = 0;
			//std::cout << "HERE" << std::endl;
			MapView::reset = true;
			*state = 0;
		}
		cleanUp();
	}
	else if (timer_time/1000 >= 0) {
		//Resets flashing for timeout
		flashing = 0;
		
		//Updates timer display
		std::ostringstream out;
		out << std::setprecision(2) << std::fixed << timer_time/1000;
		timer_string = out.str();
		timer.setString(timer_string);
	
		//Updates target grids for moving targets such as the player in pathfinder
		//Updates normally for all other objects
		std::vector<StrongActorPtr>::iterator it;
		//if (testing_view)
		//	std::cout << player->getPosition().x << " " << player->getPosition().y << std::endl;
		for (it = actorList.begin(); it != actorList.end(); it++) {
			if ((*it)->getPathType() == -4) {
				(*it)->update(time);
				sf::Vector2f init_pos = (*it)->getInitialPosition();
				sf::Vector2f start_pos = (*it)->getStartPosition();
				sf::Vector2f new_pos = (*it)->getPosition();
				if ((start_pos != new_pos) && (*it)->getVisible() && Pathfinder::canUpdateTargetGrid(init_pos)) {
					std::thread(&Pathfinder::updateTargetGrid, init_pos, new_pos).detach();
					(*it)->setStartPosition(new_pos);
				}
			}
			else {
				(*it)->update(time);
			}
			commentary[(*it)->getInstance()].setPosition((*it)->getPosition() + (*it)->getSize());
		}
		gameView.setCenter(Configuration::getGameViewCenter());		

		//Check to see if conditions met to display back button
		//std::cout << flowers_left << " " << vases_full << " " << inVision << std::endl; 
		if (call_key_pressed && inVision <= 0) {
				reveal_homer = true;
		}
		//Set timer to bottom right corner
		sf::Vector2f gameView_bottom_corner = Configuration::getGameViewCenter();
		gameView_bottom_corner.x += Configuration::getGameViewWidth()/2 - timer.getGlobalBounds().width;
		gameView_bottom_corner.y += Configuration::getGameViewHeight()/2 - timer.getGlobalBounds().height * 1.25;
		back_button.setPosition(sf::Vector2f(Configuration::getGameViewPosition().x,Configuration::getGameViewPosition().y + Configuration::getGameViewHeight() - back_button.getGlobalBounds().height));
		timer.setPosition(gameView_bottom_corner);
	}

	//Places the time
		//Geout sprite
	timeout_sprite.setPosition(sf::Vector2f(Configuration::getGameViewPosition().x, Configuration::getGameViewPosition().y));
	
	//Determines when to flash the screen when nearing timeout
	if (view_state == 1) {
		if ( (timer_time/1000 > 9.9 && timer_time/1000 <= 10) || (timer_time/1000 > 5.9 && timer_time/1000 <= 6) || (timer_time/1000 > 3.9 && timer_time/1000 <= 4) || (timer_time/1000 > 2.9 && timer_time/1000 <= 3) || (timer_time/1000 > 1.9 && timer_time/1000 <= 2) || (timer_time/1000 > .9 && timer_time/1000 <= 1) || (timer_time/1000 <= 0)) {
			flashing = 1;
		}
	}
	if (received_new_commentary) {
		received_new_commentary = false;
		display_commentary = true;
	}

	if (name == "LevelPlayer" && view_state == 1 && flowers_left == 0) {
		view_state = 0;
		MapView::view_state = 1;
		*state = 0;
		//cleanUp();
		view_state = 1;
	}
	
}

/** Checks for events and update accordingly
 **
 */
void LevelView::update(EventInterfacePtr e) {
	//In tutotial, update hints to display based on if event is acheived
	if (e == NULL || name == "LevelPlayer")
		return;

	EventType event_type = e->getEventType();
	if (event_type == ContactEvent::event_type) {
		StrongActorPtr actor_ptr = NULL;
		if ( (int) e->getSender() >= 0) {
			actor_ptr = getActor(e->getSender());
		}
		StrongActorPtr contact_ptr = NULL;
		if ( (int) e->getReceiver() >= 0) {
			contact_ptr = getActor(e->getReceiver());
		}
		
		if (actor_ptr != NULL && contact_ptr != NULL) {
			if (actor_ptr->isOfType("Homer") && contact_ptr->isOfType("Player") && reveal_homer) {
				if (view_state == 2) {
					view_state = 0;
					MapView::view_state = 2;
					MapView::commentary_idx = 0;
					MapView::reset = true;
					*game_state = 0;
					cleanUp();
					view_state = 1;
				}
				else {
					view_state = 0;
					MapView::view_state = 1;
					*game_state = 0;
					cleanUp();
					view_state = 1;
				}
				return;
			}
		}
			for (auto itr_cs = commentary_strings.begin(); itr_cs != commentary_strings.end(); itr_cs++) {
				bool found = false;
				ActorId display_id = itr_cs->first.first;
				ActorId actor_id = itr_cs->first.second.first;
				ActorId contact_id = itr_cs->first.second.second;
				//std::cout << "HERE1" << std::endl;
				if (actor_ptr == NULL && contact_ptr == NULL && (display_id == "Homer" || display_id == "Player")  && contact_id == "") {
					found  = true;
				}
				else if (actor_ptr == NULL && contact_ptr != NULL && display_id == "Homer") {
					if (contact_ptr->isOfType(contact_id))
						found = true;
				}
				else if (contact_ptr == NULL && actor_ptr != NULL && contact_id == "") {
					if (actor_ptr->isOfType(actor_id))
						found = true;
				}
				else if (actor_ptr != NULL && contact_ptr != NULL ) {
					if (actor_ptr->isOfType(actor_id) && contact_ptr->isOfType(contact_id))
						found = true;
				}
				if (found) {
					//std::cout << "HERE2" << std::endl;
					int count = -1;
					int temp_count = -1;
					for (auto itr_occ = commentary_occurance[DisplayContactPair(display_id, ContactPair(actor_id, contact_id))].begin(); itr_occ != commentary_occurance[DisplayContactPair(display_id, ContactPair(actor_id, contact_id))].end(); itr_occ++) {
						temp_count++;
						if (*itr_occ > 0) {
							(*itr_occ)--;
							count = temp_count;
							break;
						}
						else if (*itr_occ < 0) {
							count = 0;
							break;
						}
					}
					if (count >= 0) {
						//std::cout << "HERE3" << std::endl;
						int action = commentary_actions[DisplayContactPair(display_id, ContactPair(actor_id, contact_id))][count];
						if (action >= 0) {
							for (auto action_itr = actions[action].begin(); action_itr != actions[action].end(); action_itr++) {
								//std::cout << "GENERATED" << std::endl;
								pugi::xml_node temp = *action_itr;
								if (!strcmp(temp.name(), "FireFlower") || !strcmp(temp.name(), "EarthFlower") || !strcmp(temp.name(), "WaterFlower") || !strcmp(temp.name(), "AirFlower"))
									flowers_left++;
								//std::cout << "GENERATED2" << std::endl;
								generateActor(&(temp), game_state);
							}
						}

						if (contact_id == "FireFlower" || contact_id == "EarthFlower" || contact_id == "WaterFlower" || contact_id == "AirFlower")
							if (vases_full)
								count = 1;
						//std::cout << count << std::endl;
						int r = rand() % (itr_cs->second)[count].size();
						if (display_id == "Homer") {
							commentary[-1] = sf::Text(((itr_cs->second)[count])[r], font);
							commentary[-1].setCharacterSize(((commentary_sizes[DisplayContactPair(display_id, ContactPair(actor_id, contact_id))])[count])[r]);
							commentary[-1].setColor(sf::Color::Black);
							commentary[-1].setPosition(sf::Vector2f(commentary_prompt.getPosition().x + commentary_prompt.getGlobalBounds().width/2 - commentary[-1].getGlobalBounds().width/2, commentary_prompt.getPosition().y + commentary_prompt.getGlobalBounds().height/2 - commentary[-1].getGlobalBounds().height/2));		
							commentary_timer[-1].restart();
							duration = timer_time;
							received_new_commentary = true;
						}
						else {
							//std::cout << "Here4 " << display_id << " " << actor_id << " " << contact_id << " " << action << " " << ((itr_cs->second)[count])[r] <<  std::endl;
							commentary[e->getSender()] = sf::Text(((itr_cs->second)[count])[r], font);
							commentary[e->getSender()].setCharacterSize(((commentary_sizes[DisplayContactPair(display_id, ContactPair(actor_id, contact_id))])[count])[r]);
							commentary_timer[e->getSender()].restart();
						}
						//std::cout << action << std::endl;
						last_action = action;
						break;
					}
				}	
		}
	}
}

/** Renders the map onto the window
**
**/
void LevelView::render(sf::RenderWindow *window) {

	//Loading screen
	if (name == "LevelPlayer") {
		window->clear(sf::Color::White);
		window->draw(Configuration::getLoadingSprite());
		window->display();
		return;
	}
	if (view_state == 0) {
		window->clear(sf::Color::White);
		window->setView(window->getDefaultView());
		//window->draw(Configuration::getLoadingSprite());
		window->draw(title);
		window->draw(load_state);
		window->display();
		return;
	}

	//Game display	
	if (timer_time/1000 >= 0) {
		if (flashing && view_state == 1) {
			window->clear(sf::Color::Black);
			window->display();
		}
		//Get the player location and center gameView to it
		window->setView(window->getDefaultView());
		gameView.setViewport(sf::FloatRect(0, 0, 1, 1));
		
		if (!testing_view)
			window->setView(gameView);
		//Update graphics	
		//window->draw(edge);
		window->draw(background);
		window->draw(minimap_border);
		std::vector<StrongActorPtr>::iterator it;
		for (it = actorList.begin(); it != actorList.end(); it++) {
			if (homer != *it) {
				(*it)->render(window, false);
				if ((commentary_timer[(*it)->getInstance()].getElapsedTime().asSeconds() < 4))
					window->draw(commentary[(*it)->getInstance()]);
			}
		}

		if (reveal_homer)
			homer->render(window, false);

		player->render(window, false);


		window->draw(back_button);
		window->draw(timer);

		//Set minimap view
		window->setView(window->getDefaultView());
		minimapView.setViewport(sf::FloatRect(.9, 0, .1, .1));
		window->setView(minimapView);

		//Update graphics
		window->draw(minimap_background);
		//window->draw(timer);
		for (it = actorList.begin(); it != actorList.end(); it++) {
			if (!(*it)->isOfType("Obstacle") && !(*it)->isOfType("Wall") && !(*it)->isOfType("Homer")) {
				(*it)->render(window, true);
			}
		}
		player->render(window, true);
		if (reveal_homer)
			homer->render(window, true);
		//window->draw(minimap_border);

		if (view_state == 3) {
			// reset the view to the pause map
			pauseView.setViewport(sf::FloatRect(0.2, 0.2, 0.8, 0.8));
			window->setView(pauseView);

			// actual rendering
			window->draw(background);
			for (it = actorList.begin(); it != actorList.end(); it++)
				if (!(*it)->isOfType("Homer")) {
					(*it)->render(window, true);
				}
			if (reveal_homer)
				homer->render(window, true);
			player->render(window, true);
		}
	
		if (((view_state == 2 && commentary_timer[-1].getElapsedTime().asSeconds() < 30) || commentary_timer[-1].getElapsedTime().asSeconds() < 4) && display_commentary) {
			window->setView(window->getDefaultView());
			window->draw(commentary_prompt);
			window->draw(commentary[-1]);
			window->display();
		}

		//Quit Screen
		if (view_state == 4) {
			window->setView(window->getDefaultView());
			window->draw(commentary_prompt);
			window->draw(back_message);
			window->draw(back_continue);
			window->draw(back_cancel);
			window->draw(back_question);
			window->display();
		}

	}
	else {
		//Timeout display
		window->setView(window->getDefaultView());
		gameView.setViewport(sf::FloatRect(0, 0, 1, 1));
		window->setView(gameView);
		player->render(window, false);
		window->draw(timeout_sprite);
	}

	
	
}

/** Ready the level for start
 **
 **/
void LevelView::start(void) {
	level_clock.restart();
}

/** Return the actor with given instance
 **
 **/
StrongActorPtr LevelView::getActor(int instance) {
	std::vector<StrongActorPtr>::iterator it;
	for (it = actorList.begin(); it != actorList.end(); it++) {
		if ((*it)->getInstance() == instance) {
			return *it;
		}
	}
	return NULL;
}

/** Return the actor with given instance
 **
 **/
void LevelView::removeActor(int instance) {
	std::vector<StrongActorPtr>::iterator it;
	for (it = actorList.begin(); it != actorList.end(); ++it) {
		if ((*it)->getInstance() == instance) {
			actorList.erase(it);
			break;
		}
	}
}
/** Clean up level after completion
 **
 **/
void LevelView::cleanUp(void) {
	std::vector<StrongActorPtr>::iterator it;
	for (it = actorList.begin(); it != actorList.end(); it++) {
		if (*it != player) {
			(*it)->quit();
		}
	}
	num_actors = 0;
	EventManagerInterface::get()->reset();
	ActorFactory::reset();
	actorList.clear();
	sound.stop();
	view_state = 1;
}

/** Quit level
 **
 **/
void LevelView::quit(void) {
	cleanUp();
}


//Helper function to print hints correctly
std::string LevelView::fitStringToCommentaryBox(std::string str, int character_size, sf::Vector2f box_size, bool center) {
	// get dialogue box bound
	int width;
	int height;
	if (box_size.x == 0  || box_size.y == 0) {
		width = commentary_prompt.getGlobalBounds().width - 35;
		height = commentary_prompt.getGlobalBounds().height - 35;
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
		// text object used to see how close each word pfitted_suts us to the bounds
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
