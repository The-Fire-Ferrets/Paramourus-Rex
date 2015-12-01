#include "DialogueView.h" 
#include "CraftView.h"
#include "Utilities.h"
#include <vector>
#include <algorithm>
#include <cmath>

// Total size of pointer arrays
const int DialogueView::size = 20;

// Dialogue name
std::string DialogueView::name;
//State of view 0 = intro; 1 = normal
int DialogueView::view_state = 1;
// DIALOGUE VARIABLES
// Total number of dialogue boxes
int DialogueView::numDialogues = 0;
int DialogueView::index = 0;
// Array holding pointers to the dialogue that's to fill the boxes at cutscenes
std::vector<std::pair<std::string, std::string>> DialogueView::boxes;
std::vector<std::pair<std::string, std::string>> DialogueView::dialogues;
// Text in the specific node we are looking at that will show up in Dialogue box
std::string DialogueView::dialogue_str;
//Dialogue setPosition
sf::Vector2f DialogueView::dialogue_pos;

// Actual text to be rendered
sf::Text DialogueView::text;
sf::Font DialogueView::font;

// BACKGROUND VARIABLES
sf::Texture DialogueView::background_texture;
sf::Sprite DialogueView::background;
//sf::RectangleShape DialogueView::backlay;

sf::Texture DialogueView::backlay_texture;
sf::Sprite DialogueView::backlay;

// character art to be rendered
sf::Texture  DialogueView::lhs_character_tex;
sf::Sprite   DialogueView::lhs_character_sprite;

sf::Texture  DialogueView::rhs_character_tex;
sf::Sprite   DialogueView::rhs_character_sprite;

bool DialogueView::pressed = false;
bool DialogueView::player_response = false;
bool DialogueView::draw_overlay = true;

// dialogue music
sf::SoundBuffer DialogueView::buffer;
sf::Sound DialogueView::sound;

bool DialogueView::solved = false;
int DialogueView::num_times_impressed = 0;
int DialogueView::response;

// commentary display
sf::Texture DialogueView::commentary_prompt_texture;
sf::Sprite  DialogueView::commentary_prompt;
std::string DialogueView::commentary_string_flower;
std::string DialogueView::commentary_string_choices;
sf::Text    DialogueView::commentary_flower;
sf::Text    DialogueView::commentary_choices;
bool        DialogueView::display_commentary_flower  = false;
bool        DialogueView::display_commentary_choices = false;

/** Searches for the correct dialogue box the player is on and populates the text with what you want Diana to be saying 
 ** resource: filename for XML  file we are getting the dialogue from. Currently just level0, only level we have.
 ** state: current game state (state should be 2 if in dialogue view)
 **/
void DialogueView::Create(const char* resource, int* state){
	dialogues.clear();
	boxes.clear();
	//Reset the index before each iteration
	index = 0;
	// reference to XML file we are getting our info from
	pugi::xml_document doc;

	pugi::xml_parse_result result;
	std::string fileString(resource);
	name = fileString;
	if (name == "Level0") {
		view_state = 3;
	}
	// Checks to make sure XML file exists and was correctly loaded
	if (!(result = doc.load_file(("./assets/dialogue/" + fileString + ".xml").c_str()))){
		std::cout << "DialogueView::Create(...): Failed to load" << std::endl;
		std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
	}

	// this is called every time update is called -
	// we iterate through the xml file until we get to the correct index,
	// and then we display the text found in that node.
	//Used to iterate over XML file to get attributes
	pugi::xml_node tools = doc.child(resource);
	char* temp;	
	for (pugi::xml_attribute attr = tools.first_attribute(); attr; attr = attr.next_attribute()) {
		if (!strcmp(attr.name(), "NumDialogues")){
			numDialogues = atoi(attr.value());
		}
		else if (!strcmp(attr.name(), "Background")) {
			background_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
			background = sf::Sprite(background_texture, sf::IntRect(0, 0, Configuration::getWindowWidth(), Configuration::getWindowHeight()));
			background.setPosition(sf::Vector2f(0,0));
		}
		else if (!strcmp(attr.name(), "Backlay")) {
			backlay_texture.loadFromFile(("./assets/sprites/" + std::string(attr.value())).c_str());
		}
		else if (!strcmp(attr.name(), "Prompt")) {
			commentary_prompt_texture.loadFromFile(("./assets/sprites/" + std::string(attr.value())).c_str());
		}
		else if (!strcmp(attr.name(), "Font")) {
			font.loadFromFile(("./assets/" + (std::string)attr.value()).c_str());
			text.setFont(font);
		}
		else if (!strcmp(attr.name(), "Text_Size")) {
			text.setCharacterSize(std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Text_X")) {
			dialogue_pos.x = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Text_Y")) {
			dialogue_pos.y = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Flower")) {
			StrongActorPtr flower = CraftView::getFlower(attr.value());
			if (flower) {
				CraftView::removeFlower(flower);
				CraftView::updateFlowerCount(attr.value());
				++num_times_impressed;
				solved = true;
			}
			else{
				num_times_impressed--;
				solved = false;
			}
		}
		else if (!strcmp(attr.name(), "Response")){
			response = atoi(attr.value());
		}
	}

	text.setPosition(dialogue_pos);
	text.setColor(sf::Color::Black);

	unsigned int width = Configuration::getWindowWidth()/1.05;
	unsigned int height = Configuration::getWindowHeight()/4;
	unsigned int posX = Configuration::getWindowWidth()/40;
	unsigned int posY = Configuration::getWindowHeight()/1.4;

	sf::Vector2u backlay_size = backlay_texture.getSize();
	backlay = sf::Sprite(backlay_texture, sf::IntRect(0, 0, backlay_size.x, backlay_size.y));
	backlay.setPosition(posX, posY);
	backlay.setScale((Configuration::getWindowWidth()/1.05) / backlay_size.x,
			(Configuration::getWindowHeight()/4.0) / backlay_size.y);
	text.setColor(sf::Color::Black);
	text.setPosition(posX, posY);

	// position the commentary box
	sf::Vector2u prompt_size = commentary_prompt_texture.getSize();
	commentary_prompt = sf::Sprite(commentary_prompt_texture, sf::IntRect(0, 0, prompt_size.x, prompt_size.y));
	commentary_prompt.setPosition(75.f, 75.f);

	// Create phil_ and diana_sprite here I think?
	lhs_character_tex.loadFromFile("./assets/sprites/Phil.png");
	lhs_character_sprite = sf::Sprite(lhs_character_tex);
	lhs_character_sprite.setPosition(posX, posY-lhs_character_tex.getSize().y-5);

	// get commentary text and set positions
	if (fileString == "Level1") {
		pugi::xml_node commentary_tools = tools.child("Commentary");
		commentary_string_flower = std::string(commentary_tools.first_attribute().value());
		commentary_flower.setFont(font);
		commentary_flower.setCharacterSize(25);
		commentary_flower.setString(fitStringToBox(commentary_string_flower, 25)[0]);
		if (view_state == 2) { // only if in tutorial
			display_commentary_flower = true;
		}

		sf::Vector2f commentary_position = commentary_prompt.getPosition();
		sf::FloatRect commentary_bounds = commentary_prompt.getGlobalBounds();
		sf::FloatRect text_bounds = commentary_flower.getGlobalBounds();
		commentary_flower.setPosition(commentary_position.x + commentary_bounds.width/2.f - text_bounds.width/2.f,
				commentary_position.y + commentary_bounds.height/2.f - text_bounds.height/2.f);
		commentary_flower.setColor(sf::Color::Black);

		commentary_tools = commentary_tools.next_sibling();
		commentary_string_choices = std::string(commentary_tools.first_attribute().value());
		commentary_choices.setFont(font);
		commentary_choices.setCharacterSize(25);
		commentary_choices.setString(fitStringToBox(commentary_string_choices, 25)[0]);
		commentary_position = commentary_prompt.getPosition();
		commentary_bounds = commentary_prompt.getGlobalBounds();
		text_bounds = commentary_choices.getGlobalBounds();
		commentary_choices.setPosition(commentary_position.x + commentary_bounds.width/2.f - text_bounds.width/2.f,
				commentary_position.y + commentary_bounds.height/2.f - text_bounds.height/2.f);
		commentary_choices.setColor(sf::Color::Black);
	}

	// navigating through xml files and storing the actual dialogue into array
	if (fileString != "Level0" && fileString != "Level7") {
		tools = (solved == true) ? (tools.child("Correct")) : (tools.child("Incorrect"));
	}
	
	if (fileString == "Level7"){ 
		tools = (num_times_impressed > 2) ? (tools.child("Correct")) : (tools.child("Incorrect"));
	}
	
	for (pugi::xml_node tool = tools.first_child(); tool; tool =tool.next_sibling()){
		std::string speaker = "";
		std::string dialogue = "";
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!strcmp(attr.name(), "Speaker") && strcmp(attr.value(), "Narrator")) {
				speaker = std::string(attr.value());
				dialogue += speaker+ ": ";
			}
			if (!strcmp(attr.name(), "Text")){
				dialogue += attr.value();
				dialogues.push_back(std::pair<std::string, std::string>(speaker, dialogue));
			}
		}
	}

	// generatre dialogue boxes
	sf::Vector2f box_size = sf::Vector2f(backlay.getGlobalBounds().width, backlay.getGlobalBounds().height);
	for (std::pair<std::string, std::string> dialogue : dialogues) {
		std::vector<std::string> fitted = fitStringToBox(dialogue.second, text.getCharacterSize(), box_size);
		for (std::string line: fitted) {
			dialogue.first = (dialogue.first == "") ? ("Narrator") : (dialogue.first);
			boxes.push_back(std::pair<std::string, std::string>(dialogue.first, line));
		}
	}

	if (!buffer.loadFromFile("./assets/music/romantic-arpeggio-loop.ogg")) {
		std::cout << "DialogueView::Create: failed to load music" << std::endl;
	}
	sound.setBuffer(buffer);
	sound.setLoop(true);
	sound.play();

	// this is really hacky, but whatever
	if (fileString != "Level1") { // definitely OUT of tutorial mode
		view_state = 1;
	}
}


/** If a key is pressed, updates the index so that 
 ** it switches to the next desired block of text.  Called from main.
 **/
void DialogueView::update(sf::RenderWindow *window, int* state){
	sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
	if (mouse_pos.x > 0.f && mouse_pos.x < Configuration::getWindowWidth()
			&& mouse_pos.y > 0.f && mouse_pos.y < Configuration::getWindowHeight())
	{
		if (display_commentary_flower) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed && commentary_prompt.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
				display_commentary_flower = false;
				pressed = true;
				return;
			}
		}
		else if (display_commentary_choices) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed && commentary_prompt.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
				display_commentary_choices = false;
				pressed = true;
				return;
			}
		}

		if ((sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed) || index == 0){
			pressed = true;
			//std::cout << index << " " << boxes.size() << std::endl;
			// if we are at least at one of Diana's two responses to your answer
			if (index >= boxes.size() && draw_overlay == true){
				// stop displaying text, wait for user response before closing dialogueview
				if (view_state == 1 || view_state == 2) {
					if (name != "TimeOut") {
						//Show a new level
						MapView::level_idx++;
						//Show new commentary
						MapView::commentary_idx++;
						//Return view states to narmal if not already
						MapView::view_state = 1;
					}
					DialogueView::view_state = 1;
					*state = 0;
				}
				else if (view_state == 3)
					*state = 5;
				cleanUp();
			}

			// If it's two dialogues away from the end, we know it's at Phil's dialogue (except in case of Level0 and Level6)
			else if (index < boxes.size() && player_response == false){
				if (boxes[index].first == "Narrator") {
					rhs_character_tex = sf::Texture();
				}
				else if (boxes[index].first == "Phil"){
					rhs_character_tex.loadFromFile("./assets/sprites/Diana-Neutral.png");
					player_response = true;
					if (view_state == 2) {
						display_commentary_choices = true;
					}
				}
				else if (boxes[index].first == "Diana") {
					rhs_character_tex.loadFromFile("./assets/sprites/Diana-Neutral.png");
				}
				else if (boxes[index].first == "PlayerWin"){
					background_texture.loadFromFile("./assets/backgrounds/PlayerWin.png");
					// wait here for input from player to either close the game or to start over
					draw_overlay=false;
				}
				else if (boxes[index].first == "Diana") {
					rhs_character_tex.loadFromFile("./assets/sprites/Diana-Neutral.png");
				}
				else if (boxes[index].first == "PlayerWin"){
					background_texture.loadFromFile("./assets/backgrounds/PlayerWin-Text.png");
					// wait here for input from player to either close the game or to start over
					draw_overlay = false;
				}
				else if (boxes[index].first == "PlayerLose"){
					background_texture.loadFromFile("./assets/backgrounds/PlayerLose-Text.png");
					draw_overlay = false;
				}
				else {
					//std::cout << "Load ./assets/sprites/" + boxes[index].first + ".png" << std::endl;
					rhs_character_tex.loadFromFile("./assets/sprites/" + boxes[index].first + ".png");
				}
				unsigned int width = Configuration::getWindowWidth()/1.05;
				unsigned int posX = Configuration::getWindowWidth()/40;
				unsigned int posY = Configuration::getWindowHeight()/1.4;

				rhs_character_sprite = sf::Sprite(rhs_character_tex);
				rhs_character_sprite.setPosition(posX+width-rhs_character_tex.getSize().x, posY-rhs_character_tex.getSize().y-5);
				//rhs_character_sprite.setPosition(lhs_character_sprite.getPosition());

				text.setString(boxes[index].second);
				index++;
			}
		}
		else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			pressed = false;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && draw_overlay == false){
	 	// reset ongoing game state
		CraftView::total_craft_visits = 0;
		CraftView::clearInventory();
		//DialogueView::num_times_impressed = 0;
		MapView::level_idx = -1;
		MapView::commentary_idx = 0;
		MapView::resetPopulationValues();
		LevelView::player->reset();
		// rest the view
		view_state = 0;
		*state = 5; 
	}

	// wait for player to choose dialogue option 1 or 2 embedded in text
	// update Diana's opinion, then end dialogueView accordingly
	if (name != "Level0" || "Level7"){
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && player_response == true){
			if (response == 1){
				num_times_impressed++;
				// std::cout << "Load ./assets/sprites/Diana-Happy.png" << std::endl;
				rhs_character_tex.loadFromFile("./assets/sprites/Diana-Happy.png");
			}
			else{
				num_times_impressed--;
				//std::cout << "Load ./assets/sprites/Diana-Unimpressed.png" << std::endl;
				rhs_character_tex.loadFromFile("./assets/sprites/Diana-Unimpressed.png");
			}
			unsigned int width = Configuration::getWindowWidth()/1.05;
			unsigned int posX = Configuration::getWindowWidth()/40;
			unsigned int posY = Configuration::getWindowHeight()/1.4;

			rhs_character_sprite = sf::Sprite(rhs_character_tex);
			rhs_character_sprite.setPosition(posX+width-rhs_character_tex.getSize().x, posY-rhs_character_tex.getSize().y-5);

			// skip to Diana's first response, the response to option 1
			//index++;
			text.setString(boxes[index].second);
			index = boxes.size();
			player_response = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) && player_response == true){
			if (response == 2){
				num_times_impressed++;
				//std::cout << "Load ./assets/sprites/Diana-Happy.png" << std::endl;
				rhs_character_tex.loadFromFile("./assets/sprites/Diana-Happy.png");
			}
			else{
				num_times_impressed--;
				//std::cout << "Load ./assets/sprites/Diana-Unimpressed.png" << std::endl;
				rhs_character_tex.loadFromFile("./assets/sprites/Diana-Unimpressed.png");
			}
			unsigned int width = Configuration::getWindowWidth()/1.05;
			unsigned int posX = Configuration::getWindowWidth()/40;
			unsigned int posY = Configuration::getWindowHeight()/1.4;

			rhs_character_sprite = sf::Sprite(rhs_character_tex);
			rhs_character_sprite.setPosition(posX+width-rhs_character_tex.getSize().x, posY-rhs_character_tex.getSize().y-5);

			// skip to Diana's first response, the response to option 1
			index++;
			text.setString(boxes[index].second);
			index = boxes.size();
			player_response = false;
		}

	}
}

/** Checks for events and updates accordingly
 * **
 */
void DialogueView::update(EventInterfacePtr e){

}


// Draws the dialogue, Diana and Phil
void DialogueView::render(sf::RenderWindow *window){
	window->draw(background);
	
	if (draw_overlay == true){
	    window->draw(backlay);
	    window->draw(text);
	    // once Diana and Phil sprites are finished, will be rendered here as well
	    window->draw(lhs_character_sprite);
	    window->draw(rhs_character_sprite);
	}

	if (display_commentary_flower) {
		window->draw(commentary_prompt);
		window->draw(commentary_flower);
	}
	else if (display_commentary_choices) {
		window->draw(commentary_prompt);
		window->draw(commentary_choices);
	}
}

/** Play nice with the resources...
 **
 **/
void DialogueView::cleanUp() {
	sound.stop();
	solved = false;
}

std::vector<std::string> DialogueView::fitStringToBox(std::string str, int character_size, sf::Vector2f box_size, bool center) {
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

	//std::cout << width << " " << height << std::endl;
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
			std::size_t newline_loc = word.find("\n");
			if (current_width + word_width > max_width || newline_loc != std::string::npos) {
				if (center) {
					int num_spaces = (int) ((max_width - current_width)/2)/space_width;
					for (int s_num = 0; s_num < num_spaces; s_num++)
						next_line = " " + next_line + " ";
				}

				std::string newline = (newline_loc != std::string::npos) ? ("") : ("\n");
				fitted_string += next_line + newline;
				next_line = word + " ";
				current_width = word_width;
			}
			else {
				// just add to string
				next_line += word + " ";
				current_width += word_width;
				size_found = true;
			}

			// the height of the full string so far
			temp.setString(fitted_string);
			float full_height = temp.getGlobalBounds().height - temp.getGlobalBounds().top;

			// will it go past the vertical bound?
			if (max_height - full_height < word_height) {
				//curr_size--;
				size_found = false;
				boxes.push_back(fitted_string);
				fitted_string = "";
				current_width = word_width;
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
		return std::vector<std::string>(1, "");
	}
	return boxes;
}
