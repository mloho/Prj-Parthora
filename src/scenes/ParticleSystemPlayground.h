/**
 * Project Parthora's main game scene.
 *
 * @date       March 9, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes
 */

#ifndef PARTICLESYSTEMPLAYGROUND_H
#define PARTICLESYSTEMPLAYGROUND_H

#include <SFML/Audio.hpp>
#include <iostream>

#include "../engine/Scene.h"
#include "../core/Renderer.h"
#include "../core/object/SGO.h"
#include "../core/object/TGO.h"
#include "../net/client/Connection.h"

#include "../graphics/Fireball.h"
#include "../graphics/LewisParticle.h"

class ParticleSystemPlayground : public Scene
{
private:
	sf::View view_hud, view_main;

	Renderer renderer;

	sf::Shader shader_shake, shader_shockwave;

	ParticleSystem* ps_1;
	TGO ps_1_label;

	sf::Texture tex_1;
	SGO thing_1, thing_2;

	sf::Vector2f lastEmitterPos;

	sf::Music music1;

	sf::SoundBuffer sb;
	sf::Sound swoosh;
	float swooshVolume;
	int swooshStoppedLen;

	sf::Vector2f view_main_offset;

	Connection conn;

public:
	ParticleSystemPlayground(AppWindow &window) :
		Scene(window, "Particle System Playground"), renderer(window, 1000)
	{
		music1.openFromFile("D:/Dropbox/SFML-Game - backup/SFML-Game/Data/audio/gardenparty_mono.wav");

		tex_1.loadFromFile("Data/textures/placeholder.png");
		thing_1.sprite().setTexture(tex_1);
		thing_1.middleAnchorPoint(true);

		ps_1_label.text().setFont(*scene_log.text().getFont());
		ps_1_label.text().setCharacterSize(15);
		ps_1_label.text().setString("ps_1");

		ps_1 = new Fireball();

		ps_1->add(ps_1_label);

		conn.setReceiveHandler(std::bind(&ParticleSystemPlayground::onReceive, this, std::placeholders::_1, std::placeholders::_2));
	}

	~ParticleSystemPlayground()
	{
		delete ps_1;
	}

	void onload() override
	{
		getWindow().setMouseCursorVisible(false);

		view_hud = view_main = getWindow().getCurrentView();

		shader_shake.loadFromFile("D:/Repositories/term-project/Source/Assets/shaders/wave.vert", sf::Shader::Vertex);
		shader_shockwave.loadFromFile("D:/Repositories/term-project/Source/Assets/shaders/pound.vert", sf::Shader::Vertex);

		music1.setPosition(view_main.getCenter().x, view_main.getCenter().y, 0);
		music1.setMinDistance(1000);
		music1.setLoop(true);

		swooshStoppedLen = 0;
		sb.loadFromFile("Data/audio/SWOOSH_loop.wav");
		swoosh.setBuffer(sb);
		swoosh.setLoop(true);
		swoosh.setVolume(0);
		swoosh.play();

		ps_1_label.text().setPosition(10, -10);

		thing_1.sprite().setPosition(music1.getPosition().x, music1.getPosition().y);

		conn.start("localhost", 12345);

		Packet p;

		p.mType = MessageType::UPDATE_INFO;

		p.add("Teguh");

		p.add('0' + ps_1->colorBegin.r);
		p.add('0' + ps_1->colorBegin.g);
		p.add('0' + ps_1->colorBegin.b);
		p.add('0' + ps_1->colorBegin.a);

		p.add('0' + ps_1->colorEnd.r);
		p.add('0' + ps_1->colorEnd.g);
		p.add('0' + ps_1->colorEnd.b);
		p.add('0' + ps_1->colorEnd.a);

		p.add(0);
		p.add(0);

		conn.send(p);
	}

	void unload() override
	{
		getWindow().setMouseCursorVisible(true);

		swoosh.stop();
		music1.stop();

		conn.stop();
	}

	void handleEvent(const sf::Event &event) override
	{
		Scene::handleEvent(event);

		static bool vSync = false, particleBuilder = false, music1Toggle = true;
		static float view_main_offset_value = 5, view_main_offset_max = 15;

		switch (event.type)
		{
		case sf::Event::MouseWheelMoved:
			view_main.zoom(1 - event.mouseWheel.delta * 0.0625f);
			break;

		case sf::Event::KeyPressed:
			switch (event.key.code)
			{
			case sf::Keyboard::A:
				if (view_main_offset.x > -view_main_offset_max)
					view_main_offset.x += -view_main_offset_value;
				break;
			case sf::Keyboard::D:
				if (view_main_offset.x < view_main_offset_max)
					view_main_offset.x += view_main_offset_value;
				break;
			case sf::Keyboard::W:
				if (view_main_offset.y > -view_main_offset_max)
					view_main_offset.y += -view_main_offset_value;
				break;
			case sf::Keyboard::S:
				if (view_main_offset.y < view_main_offset_max)
					view_main_offset.y += view_main_offset_value;
				break;
			}
			break;

		case sf::Event::KeyReleased:
			switch (event.key.code)
			{
			case sf::Keyboard::BackSpace:
				getWindow().removeScene(this->getID());
				break;
			case sf::Keyboard::Space:
				randomizeParticleColors(ps_1);
				break;
			case sf::Keyboard::Delete:
				ps_1->clear();
				break;

			case sf::Keyboard::A:
			case sf::Keyboard::D:
				view_main_offset.x = 0;
				break;
			case sf::Keyboard::W:
			case sf::Keyboard::S:
				view_main_offset.y = 0;
				break;

			case sf::Keyboard::V:
				getWindow().setVerticalSyncEnabled(vSync = !vSync);
				break;
			case sf::Keyboard::P:
				particleBuilder = !particleBuilder;
				ps_1->setBuilder(particleBuilder ? ParticleBuilders::pbPoint : ParticleBuilders::pbSprite);
				break;
			case sf::Keyboard::M:
				music1Toggle = !music1Toggle;
				if (music1Toggle) music1.stop();
				else music1.play();
				break;

			case sf::Keyboard::Return:
				// example:
				Packet p;
				p.mType = MessageType::UPDATE_POS;
				p.add<float>(ps_1->emitterPos.x);
				p.add<float>(ps_1->emitterPos.y);
				conn.send(p);
				break;
			}
			break;

		case sf::Event::Resized:
			view_hud = getWindow().getCurrentView();
			view_main.setSize(view_hud.getSize());
			view_main.setCenter(getWindow().getSize().x * 0.5f, getWindow().getSize().y * 0.5f);
			break;
		}
	}

	void update(const sf::Time &deltaTime) override
	{
		static float wavePhase = 0;
		static sf::Vector2f waveAmp;

		lastEmitterPos = ps_1->emitterPos;
		ps_1->emitterPos = getWindow().getMousePositionRelativeToWindowAndView(view_main);

		ps_1->update(deltaTime);

		sf::Vector2f delta(abs(ps_1->emitterPos.x - lastEmitterPos.x), abs(ps_1->emitterPos.y - lastEmitterPos.y));
		float magnitude = sqrt(delta.x * delta.x + delta.y * delta.y);

		if (magnitude > 5.0f)
		{
			swooshVolume += magnitude > 2.0f ? 2.0f : magnitude;
			swooshStoppedLen = 0;
		}
		else
		{
			swooshVolume -= 2.0f;
			swooshStoppedLen++;
		}

		swooshVolume = swooshVolume < 0.f ? 0.f : swooshVolume;
		swooshVolume = swooshVolume > 100.f ? 100.f : swooshVolume;

		if (swooshStoppedLen > 69)
		{
			swooshStoppedLen = 0;
			wavePhase = 0;
		}

		swoosh.setPitch(swooshVolume / 100);
		swoosh.setVolume(swooshVolume);

		++wavePhase;
		waveAmp.x = magnitude / 5;
		if (waveAmp.x > 15) waveAmp.x = 15;
		waveAmp.y = waveAmp.x;

		shader_shake.setParameter("wave_phase", wavePhase);
		shader_shake.setParameter("wave_amplitude", waveAmp);
		shader_shockwave.setParameter("pound_position", ps_1->emitterPos);
		shader_shockwave.setParameter("pound_total_radius", 5);
		shader_shockwave.setParameter("pound_inner_radius", 3);

		view_main.move(view_main_offset);

		sf::Listener::setPosition(view_main.getCenter().x, view_main.getCenter().y, -100);

		scene_log.text().setString(
			"FPS: " + std::to_string(getWindow().getFPS())
			+ "\n"
			+ "\nRenderer>"
			+ "\nDraw calls: " + std::to_string(renderer.getDrawCallCount())
			+ "\nSprites   : " + std::to_string(renderer.getSpriteCount())
			+ "\n"
			+ "\nParticles>"
			+ "\ntotal: " + std::to_string(ParticleSystem::TotalParticleCount)
			+ "\n" + ps_1_label.text().getString() + " count   : " + std::to_string(ps_1->getParticleCount())
			+ "\n" + ps_1_label.text().getString() + " vertices: " + std::to_string(ps_1->getVertexCount())
			);
	}

	void render() override
	{
		getWindow().clear();

		renderer.resetStats();

		getWindow().setView(view_main); ///////////////////////////////////

		renderer.begin();

		renderer.draw(thing_1);

		renderer.end();

		renderer.begin();

		renderer.states.blendMode = sf::BlendAdd;
		renderer.states.shader = &shader_shake;
		renderer.draw(ps_1);

		renderer.end();

		getWindow().setView(view_hud); ////////////////////////////////////

		renderer.begin();

		renderer.draw(scene_log);

		renderer.end();

		getWindow().display();
	}

	void randomizeParticleColors(ParticleSystem* p)
	{
		p->colorBegin = sf::Color(rand() & 255, rand() & 255, rand() & 255);
		p->colorEnd = sf::Color(rand() & 255, rand() & 255, rand() & 255);
	}

	void onReceive(Packet p, sf::TcpSocket& sock)
	{

	}
};

#endif /* PARTICLESYSTEMPLAYGROUND_H */