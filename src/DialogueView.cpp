#include "DialogueView.h"

// Total number of dialogue boxes
int DialogueView::index = 0;

// Array holding pointers to the dialogue that's to fill the boxes at cutscenes
std::string DialogueView::boxes[15];

// File name of background image
std::string DialogueView::background;

// Text in the specific node we are looking at that will show up in Dialogue box
std::string DialogueView::text;

// Level name
std::string DialogueView::level;

// Total number of dialogue boxes in this specific cutscene, prefaced as the first node in each subsection
// i.e.
// <NumDialogues>
// <DialogueNodeStart>
// 	<Text>Actual Dialogue</text>
//	<Background>.bmp</Background>
// </DialogueNodeStart>
int DialogueView::maxDialogues = 0;


/** Searches for the correct dialogue box the player is on and populates the text with what you want Diana to be saying 
** resource: filename for XML  file we are getting the dialogue from. Currently just level0, only level we have.
** state: current game state (state should be 2 if in dialogue view)
**/
std::string DialogueView::getDialogue(const char* resource, int* state){
	
	// reference to current location in dialogue array
	int count=0;
	
	// reference to XML file we are getting our info from
	pugi::xml_document doc;

	pugi::xml_parse_result result;
	std::string fileString(resource);

	// Checks to make sure XML file exists and was correctly loaded
	if (!(result = doc.load_file(("./assets/levels/" + fileString + ".xml").c_str()))){
		std::cout << "DialogueView::CreateDialogue(...): Failed to load" << std::endl;
		std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
	}
	
	// this is called every time update is called -
	// we iterate through the xml file until we get to the correct index,
	// and then we display the text found in that node.
	
	pugi::xml_node tools = doc.child(resource);
	pugi::xml_attribute attr = tools.first_attribute();
	maxDialogues = atoi(attr.value());
	
	// navigating to correct dialogue node so it can return the text in it
	while (count < index && index <= maxDialogues){
	    count++;
	    attr=attr.next_attribute();
	}
	
	// Need to set the text attribute into the boxes[] array here
	if (!strcmp(attr.name(), "Text")){
	    text = attr.value();
	    attr=attr.next_attribute();
	    if (!strcmp(attr.name(), "Background")){
		background = attr.value();
	    }
	}

	std::cout << "Setting text.";
	return text;

}


/** If a key is pressed, updates the index so that 
** it switches to the next desired block of text.  Called from main.
**/
void DialogueView::update(const char* resource, int* state){
	index++;
	if (index > maxDialogues){
		std::cout << "Finished all dialogues for this level.";
		DialogueView::changeState(state);
	}
	else{
	    DialogueView::getDialogue(resource, state);
	}
}


/** Dialogue for this particular scene has ended; 
** informs eventManager accordingly to move on to the next level view
**/
void DialogueView::changeState(int* state){
	int newState = 1;
	state = &newState;
}


