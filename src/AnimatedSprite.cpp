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

#include <AnimatedSprite.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <cassert>

AnimatedSprite::AnimatedSprite()
    : m_shader      (nullptr),
    m_frameCount    (0u),
    m_currentFrame  (0u),
    m_firstFrame    (0u),
    m_lastFrame     (0u),
    m_frameRate     (0.f),
    m_elapsedTime   (0.f),
    m_loop          (false),
    m_playing       (false){}

AnimatedSprite::AnimatedSprite(const sf::Texture& t)
    : m_sprite      (t),
    m_shader        (nullptr),
    m_textureSize   (t.getSize()),
    m_frameCount    (0u),
    m_currentFrame  (0u),
    m_firstFrame    (0u),
    m_lastFrame     (0u),
    m_frameRate     (0.f),
    m_elapsedTime   (0.f),
    m_loop          (false),
    m_playing       (false){}

//public
void AnimatedSprite::update(float dt)
{
    if (m_playing)
    {
        float frameTime = 1.f / m_frameRate;
        m_elapsedTime += dt;

        while (m_elapsedTime >= frameTime && (m_currentFrame < m_lastFrame || m_loop))
        {
            //while not at the end of the range or loop set to true
            //move frame to the right
            m_subRect.left += m_frameSize.x;

            //wrap around if at texture edge
            if (m_subRect.left + m_subRect.width > m_textureSize.x)
            {
                m_subRect.left = 0;
                m_subRect.top += m_frameSize.y;
            }

            //update frame data
            m_elapsedTime -= frameTime;
            
            m_currentFrame++;
            if (m_currentFrame > m_lastFrame)
            {
                if (m_loop)
                    setFrame(m_firstFrame);
                else
                    m_playing = false;
            }
        }

        m_sprite.setTextureRect(m_subRect);
    }
}

void AnimatedSprite::setTexture(const sf::Texture& t)
{
    m_sprite.setTexture(t);
    m_textureSize = t.getSize();
}

void AnimatedSprite::setNormalMap(const sf::Texture& t)
{
    m_normalMap = t;
}

void AnimatedSprite::setShader(sf::Shader& shader)
{
    m_shader = &shader;
}

void AnimatedSprite::setFrameSize(const sf::Vector2i& size)
{
    m_frameSize = size;
    m_subRect.width = size.x;
    m_subRect.height = size.y;
}

const sf::Vector2i& AnimatedSprite::getFrameSize() const
{
    return m_frameSize;
}

void AnimatedSprite::setFrameCount(sf::Uint8 count)
{
    m_frameCount = count;
}

void AnimatedSprite::setFrameRate(float rate)
{
    assert(rate >= 0.f);
    m_frameRate = rate;
    //m_playing = (rate > 0.f);
}

float AnimatedSprite::getFrameRate() const
{
    return m_frameRate;
}

void AnimatedSprite::setLooped(bool loop)
{
    m_loop = loop;
}

bool AnimatedSprite::looped() const
{
    return m_loop;
}

void AnimatedSprite::play(sf::Int16 start, sf::Int16 end)
{
    assert(start < m_frameCount && (end < m_frameCount));

    m_playing = (start == end) ? false : true;
    m_currentFrame = m_firstFrame = start;
    m_lastFrame = (end < 0) ? m_frameCount - 1 : end;

    //set position of starting frame / sub rect
    setFrame(start);
}

void AnimatedSprite::play(Animation animation)
{
    setLooped(animation.m_loop);
    play(animation.m_startFrame, animation.m_endFrame);
}

bool AnimatedSprite::playing() const
{
    return m_playing;
}

void AnimatedSprite::setPaused(bool paused)
{
    m_playing = !paused;
}

sf::FloatRect AnimatedSprite::getLocalBounds() const
{
    return sf::FloatRect(getOrigin(), static_cast<sf::Vector2f>(getFrameSize()));
}

sf::FloatRect AnimatedSprite::getGlobalBounds() const
{
    return getTransform().transformRect(getLocalBounds());
}

//private
void AnimatedSprite::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.shader = m_shader;
    if (m_shader)
    {
        m_shader->setParameter("u_normalMap", m_normalMap);
        m_shader->setParameter("u_diffuseMap", sf::Shader::CurrentTexture);
    }
    rt.draw(m_sprite, states);
}

void AnimatedSprite::setFrame(sf::Uint8 index)
{
    assert(index < m_frameCount);

    auto position = m_textureSize.x / m_frameSize.x;

    auto x = index % position;
    auto y = index / position;

    m_subRect.left = x * m_frameSize.x;
    m_subRect.top = y * m_frameSize.y;

    m_sprite.setTextureRect(m_subRect);
    m_currentFrame = index;
}