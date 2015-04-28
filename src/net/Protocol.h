/**
 * Protocol specifications.
 *
 * @date       April 18, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      This file contains all of the common things that will be used by the server and client.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

// ENUMS -----------------------------------------------------------------------

// Listed in chronological order of a simple scenario in which
// a particle is created from one screen, crosses to another screen, then disconnects.
enum MessageType
{
	NO_MT, CLIENT_INFO, CROSS_SCREENS, CROSS_STATUS, UPDATE_POS, REMOVE_TRACKING, CLIENT_DISCONNECTED
};

enum CrossingDirection
{
	NO_CD, LEFT, RIGHT
};

enum CrossingStatus
{
	NOT_CROSSED, CROSSED_LOCAL, CROSSED_GLOBAL
};

//-----------------------------------------------------------------------------<

// PARAMS ----------------------------------------------------------------------

struct ParticleParams
{
	sf::Color colorBegin, colorEnd;
};

struct DynamicClientParams
{
	std::string name;
	ParticleParams pp;
};

//-----------------------------------------------------------------------------<

// SCREEN ----------------------------------------------------------------------

struct Client;

struct Screen
{
	typedef sf::Uint16 ID;

	ID id;
	Client* owner;
};

//-----------------------------------------------------------------------------<

// CLIENT ----------------------------------------------------------------------

struct Client
{
	sf::TcpSocket socket;

	typedef sf::Uint16 ID;

	static const ID ID_MYSELF = 0;

	ID id;
	size_t screenIdx;
	std::vector<Screen*> externalScreenOccupancies;
	DynamicClientParams params;
};

//-----------------------------------------------------------------------------<

#endif // PROTOCOL_H
