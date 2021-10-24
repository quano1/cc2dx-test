#include <cocos2d.h>
// #include "logger.h"

// class BaseEntity
// {
// public:
// 	// BaseEntity() : m_id(s_id++) {}

// 	// inline int getId() { return m_id; }

// private:
// 	// static int s_id;
// 	// int m_id;
// };

// class MovingEntity: public cocos2d::Sprite
// {
// public:
// 	MovingEntity() : 
// 		heading_(cocos2d::Vec2::ZERO), 
// 		side_(cocos2d::Vec2::ZERO), 
// 		velocity_(cocos2d::Vec2::ZERO), 
// 		mass_(1), 
// 		max_speed_(0), 
// 		max_force_(0), 
// 		max_turn_rate_(0) {}

// 	virtual ~MovingEntity() = default;
// 	virtual bool initWithFile(std::string const &) override;

//     static MovingEntity *create(const std::string &filename)
//     {
// 		MovingEntity *sprite = new (std::nothrow) MovingEntity();
// 	    if (sprite && sprite->initWithFile(filename))
// 	    {
// 	    	// sprite->setTag(sprite->getId());
// 	        sprite->autorelease();
// 	        return sprite;
// 	    }
// 	    CC_SAFE_DELETE(sprite);
// 	    return nullptr;
//     }
	
// 	virtual void addForce(cocos2d::Vec2 const &);
// 	virtual void update(float) override;

// 	inline void setTarget(cocos2d::Vec2 const &pos) { target_ = pos; }

// 	// inline auto &getSigSteering() { return ms_steering; }
// 	// template<class Class, class... Args>
// 	template<class Class/*, class Class, class R, class... Args*/> 
// 	inline void connectSignalSteering(Class *instance/*, R (Class::*method) (Args...)*/) 
// 	{ 
// 		steering_.connect(Simple::slot(instance, &Class::onMessage));
// 	}

// 	void setMovEntity(float mass, float max_speed, float max_force, float max_turn_rate)
// 	{
// 		mass_ = mass;
// 		max_speed_ = max_speed;
// 		max_force_ = max_force;
// 		max_turn_rate_ = max_turn_rate;
// 	}

// private:
// 	cocos2d::Vec2 
// 		heading_, 
// 		side_, 
// 		velocity_, 
// 		target_;

// 	float 
// 		mass_, 
// 		max_speed_, 
// 		max_force_, 
// 		max_turn_rate_;

// 	Simple::Signal<cocos2d::Vec2 (std::string const &, 
// 	                              float,
// 	                              cocos2d::Vec2 const &, 
// 	                              cocos2d::Vec2 const &, 
// 	                              cocos2d::Vec2 const &)> steering_;

// };

