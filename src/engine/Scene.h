#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <SFML/Graphics.hpp>
#include "../core/object/TGO.h"

class AppWindow;

class Scene
{
public:
	typedef unsigned int ID;

	typedef std::shared_ptr<Scene> Ptr; //!< Shared pointer.

	template < class S >
	static std::shared_ptr<S> Create(AppWindow &window, const std::string &name = "");

	Scene(AppWindow &window, const std::string &name = "");
	virtual ~Scene();

	const std::string& getName() const;
	void setName(const std::string &name);

	ID getID() const;

	virtual void onload();
	virtual void unload();

	virtual void handleEvent(const sf::Event &event);

	/////
	/// 1) Update content (in order)
	/// 2) Update views if needed
	/////
	virtual void update(const sf::Time &deltaTime);

	/////
	/// Do below procedure as many times as needed for all views (in order):
	/// 1) Set appropriate view for the RenderWindow
	/// 2) Draw appropriate graphics for that view
	/////
	virtual void render();

	AppWindow &getWindow() const;

protected:
	sf::Font scene_font;
	TGO scene_log;

private:
	static ID SCENE_ID;

	AppWindow &m_window;
	ID m_id;
	std::string m_sceneName;
};

template < class S >
std::shared_ptr<S> Scene::Create(AppWindow &window, const std::string &name) {
	auto ptr = std::make_shared<S>(window);
	if (!name.empty()) ptr->setName(name);
	return ptr;
}

#endif /* SCENE_H */