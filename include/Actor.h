#ifndef ACTOR_H
#define ACTOR_H

#include "Constants.h"
#include "ActorComponent.h"
#include "EventInterface.h"
#include "EventManagerInterface.h"
#include "ContactEvent.h"
#include "LevelView.h"

class Actor {
    friend class ActorFactory;
    friend class ActorComponent;
    public:
        static int instances;
        int getInstance(void);

        ActorComponents components;
        ActorId getId(void);

        explicit Actor(void);
        bool Init(pugi::xml_node* elem);
        void PostInit(pugi::xml_node* elem);
        ~Actor(void);

        void move(float distance, sf::Vector2f direction);

        void update(float time);
        void render(sf::RenderWindow *window);
        void reset(void);
        void restart(void);
        void quit(void);

        void getEvent(EventInterfacePtr e);
        bool hasComponent(ComponentId component);
        bool hasAttribute(std::string attr);

        sf::Vector2f getPosition(void);
        void setPosition(sf::Vector2f pos);

        sf::Vector2f getSize(void);
        void setSize(sf::Vector2f sz);

        void setVisible(bool v);
        bool getVisible(void);

        int getState(void);
        void setState(int actorState);
        int* getGameState(void);
        void setGameState(int state);
        void resetGameState(int* state);

        void addDelegate(EventType type);

        sf::FloatRect* getBoundary(void);

        Actor* getCopy(void);
        const Actor* getCopy(void) const;
    private:
        ActorId id;
	const static int num_directions;
        std::vector<EventDelegate> delegateFuncList;

        std::string sprite_filename[4];

        sf::Vector2f position;
	sf::Texture sprite_texture[4];
	sf::Vector2f size;
	sf::Sprite sprite[4];
	int sprite_idx;

        int instance;
        int* game_state;
        int state;
        bool visible;
        sf::FloatRect* obstacle;
        sf::FloatRect* boundary;

        void AddComponent(StrongActorComponentPtr component);
        void updateBoundary(void);
};

#endif
