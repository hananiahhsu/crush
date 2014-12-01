/*********************************************************************
Matt Marchant 2014
http://trederia.blogspot.com

Crush - Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

#include <BodyState.hpp>
#include <Node.hpp>

#include <cassert>

BodyState::BodyState(CollisionWorld::Body* b)
    : m_body(b)
{
    assert(b);
}

CollisionWorld::Body* BodyState::getBody() const
{
    return m_body;
}

const sf::Vector2f& BodyState::getVelocity() const
{
    return m_body->m_velocity;
}

void BodyState::setVelocity(const sf::Vector2f& vel)
{
    m_body->m_velocity = vel;
}

void BodyState::move(const sf::Vector2f& amount)
{
    m_body->move(amount);
}

sf::Uint16 BodyState::getFootSenseCount() const
{
    return m_body->m_footSenseCount;
}

float BodyState::getFriction() const
{
    return m_body->m_friction;
}

void BodyState::kill()
{
    game::Event evt;
    evt.type = game::Event::Despawn;
    evt.despawn.type = Category::None;
    raiseEvent(evt);
}

void BodyState::raiseEvent(const game::Event& evt)
{
    m_body->notify(*m_body, evt);
}