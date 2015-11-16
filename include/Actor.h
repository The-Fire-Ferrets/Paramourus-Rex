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
	friend class CraftableComponent;  // to update sprite after a craftevent
	public:
		static int instances;
		int getInstance(void);

		ActorComponents components;
		ActorId getId(void) const;
		bool operator==(const Actor& rhs) const;

		explicit Actor(void);
		bool Init(pugi::xml_node* elem);
		void PostInit(pugi::xml_node* elem);
		void PostInit();
		~Actor(void);

		void move(float distance, sf::Vector2f dir);
		void move(sf::Vector2f next_pos, sf::Vector2f dir);
		void update(float time);
		void render(sf::RenderWindow *window, bool minimap);
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
		void resetVases(void);
		void addDelegate(EventType type);
		std::vector<sf::FloatRect*> getBoundary(void);
		sf::FloatRect* intersects(StrongActorPtr other_actor, bool use_border = false);
		sf::FloatRect* intersects(sf::FloatRect bound, bool use_border = false);
		sf::FloatRect* contains(sf::Vector2f pnt, bool use_border = false);
		bool causesDamage(void);
		sf::Vector2f getStartPosition(void);
		sf::Vector2f getInitialPosition(void);
		void setStartPosition(sf::Vector2f pos);
		Actor* getCopy(void);
		const Actor* getCopy(void) const;
		int getPathType(void);
		int getTargetType(void);
		sf::Vector2f getDirection(void);
		void setDirection(sf::Vector2f dir);

	private:
		ActorId id;
		sf::Vector2f start_position;
		sf::Vector2f initial_position;
		const static int num_directions;
		sf::Vector2f direction;
		std::vector<EventDelegate> delegateFuncList;
		bool damage = false;
		std::string sprite_filename[4];
		std::string spriteMinimap_filename;
		sf::Vector2f position;
		sf::Texture sprite_texture[4];
		sf::Texture spriteMinimap_texture;
		sf::Vector2f size;
		sf::Sprite sprite[4];
		sf::VertexArray sprite_vertexarray;
		bool use_vertexarray = false;
		sf::Sprite sprite_minimap;
		int sprite_idx;
		bool initial_init = true;
		bool initial_postinit = true;
		int path_type;
		int target_type;
		int instance;
		int* game_state;
		int state;
		bool visible;
		int border = 30;
		sf::FloatRect* obstacle;
		std::vector<sf::FloatRect*> boundary;
		bool renderToMinimap;
		bool renderToGameView;
		void AddComponent(StrongActorComponentPtr component);
		void updateBoundary(void);
		void setMinimapSpritePosition(sf::Vector2f pos);
};

#endif
